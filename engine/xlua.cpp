/*
This file is part of "Avanor, the Land of Mystery" roguelike game
Home page: http://www.avanor.com/
Copyright (C) 2000-2003 Vadim Gaidukevich

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <sol/sol.hpp>

#include "creature/anycr.h"
#include "item/xscroll.h"
#include "item/xbook.h"
#include "item/xenhance.h"
#include "item/xherb.h"
#include "item/itemdb.h"
#include "item/item_misc.h"
#include "item/xtool.h"
#include "creature/bodypart.h"
#include "creature/cr_defs.h"
#include "creature/deity.h"
#include "creature/std_ai.h"
#include "engine/global.h"
#include "engine/xlua.h"
#include "map/dungeon_builder.h"
#include "map/pattern.h"
#include "game/location.h"
#include "lua/api_actor.h"
#include "lua/api_world.h"
#include "game/quest.h"
#include "item/item.h"
#include "item/itemdef.h"
#include "item/xcorpse.h"
#include "item/xpotion.h"
#include "magic/brand.h"
#include "magic/effect.h"
#include "magic/resist.h"
#include "magic/skill.h"
#include "magic/stats.h"
#include "magic/cskills.h"
#include <iostream>

#include "map/map.h"

lua_State* XLua::L = nullptr;

namespace {

// An enum table the engine registered answers an unknown member with an
// error rather than with nil. Lua hands nil back for a missing table
// field, sol2 turns that nil into 0 for an enum-typed parameter, and
// whatever was built from it - a creature with no class, a tile that is
// "nothing here" - is quietly wrong, surfacing far from the line that
// made it or not until someone is hours into a game. With this, a
// misspelt member stops the world script at the line that misspelt it.
void MakeStrict(sol::state_view& lua, const char* name)
{
    sol::optional<sol::table> table = lua[name];

    if (!table) {
        std::cerr << "lua: no table '" << name << "' to guard against typos" << std::endl;

        return;
    }

    // The members are not in the table itself - sol2 keeps a read-only
    // enum's entries behind its metatable's __index - so the guard has
    // to ask that first and complain only when it has no answer, rather
    // than replace it and take every member with it.
    sol::object inherited = sol::lua_nil;

    if (sol::object meta_object = (*table)[sol::metatable_key]; meta_object.is<sol::table>()) {
        inherited = meta_object.as<sol::table>()["__index"];
    }

    sol::table meta = lua.create_table();
    const std::string table_name = name;

    meta.set_function(sol::meta_function::index,
        [table_name, inherited](sol::this_state state, sol::table self, sol::object key) {
            sol::object value = sol::lua_nil;

            if (inherited.is<sol::table>()) {
                value = inherited.as<sol::table>().get<sol::object>(key);
            } else if (inherited.is<sol::protected_function>()) {
                if (auto result = inherited.as<sol::protected_function>()(self, key); result.valid()) {
                    value = result;
                }
            }

            if (value.valid() && value != sol::lua_nil) {
                return value;
            }

            const std::string member = key.is<std::string>() ? key.as<std::string>() : "?";

            luaL_error(state, "%s.%s does not exist", table_name.c_str(), member.c_str());

            return sol::object(sol::lua_nil);
        });

    (*table)[sol::metatable_key] = meta;
}

} // namespace


void XLua::Init()
{
    // The rooms and the map alphabet both hold sol::protected_functions
    // belonging to the state being replaced here, so neither can outlive
    // it - the world scripts fill both again on the next load.
    room_templates.clear();
    XPattern::ForgetScriptPalette();
    XTileType::ForgetTiles();

    L = lua_open();
    sol::state_view lua(L);

    XLocation::RegisterLua(lua);
    XStairWay::RegisterLua(lua);
    XTrap::RegisterLua(lua);
    RegisterLuaEventEnum(lua);
    CreatureTemplate::RegisterLua(lua);
    RegisterCrDefsEnums(lua);
    XItem::RegisterLua(lua);
    XPotion::RegisterLua(lua);
    RegisterCorpseEffectEnum(lua);
    RegisterSpellNameEnum(lua);
    XCreature::RegisterLua(lua);
    XTileType::RegisterLua(lua);
    XStandardAI::RegisterLua(lua);
    XCombatSkills::RegisterLua(lua);
    RegisterBrandLua(lua);
    XResistance::RegisterLua(lua);
    RegisterColorEnum(lua);
    RegisterBodyPartEnum(lua);
    XDeity::RegisterLua(lua);
    XReligion::RegisterLua(lua);
    XStats::RegisterLua(lua);
    XSkill::RegisterLua(lua);
    XQuest::RegisterLua(lua);
    XEffect::RegisterLua(lua);

    lua.new_enum("FoodFeeling",
        "TOLERANT", FF_TOLERANT,
        "NORMAL", FF_NORMAL,
        "SENSITIVE", FF_SENSITIVE
    );

    // What a scroll or a book can be.


    // What an ordinary item can be made of, and how well made it is. Both
    // are for the templates in world/items/ (TemplateBuilder): the set says
    // which materials the game may pick from when it builds one.
    lua.new_enum("ItemSet",
        "CLOTH", ISET_CLOTH,
        "LEATHER", ISET_LEATHER,
        "STUDEDLEATHER", ISET_STUDEDLEATHER,
        "STONE", ISET_STONE,
        "WOOD", ISET_WOOD,
        "IRON", ISET_IRON,
        "BRONZE", ISET_BRONZE,
        "BRASS", ISET_BRASS,
        "SILVER", ISET_SILVER,
        "GOLD", ISET_GOLD,
        "CRYSTAL", ISET_CRYSTAL,
        "STEEL", ISET_STEEL,
        "OBSIDIAN", ISET_OBSIDIAN,
        "MITHRIL", ISET_MITHRIL,
        "ADAMANTIUM", ISET_ADAMANTIUM,
        "SOFT", ISET_SOFT,
        "ALLLEATHER", ISET_ALLLEATHER,
        "METAL", ISET_METAL,
        "METALSOFT", ISET_METALSOFT,
        "HARDMETAL", ISET_HARDMETAL,
        "ALLMETAL", ISET_ALLMETAL,
        "OBSIMETAL", ISET_OBSIMETAL,
        "STONEFROM", ISET_STONEFROM,
        "WOODEN", ISET_WOODEN,
        "SHIELD", ISET_SHIELD,
        "BOW", ISET_BOW,
        "MISSILE", ISET_MISSILE,
        "WOODSTONE", ISET_WOODSTONE,
        "WEAPON", ISET_WEAPON,
        "SIMPLEWEAPON", ISET_SIMPLEWEAPON,
        "CROWNMETAL", ISET_CROWNMETAL,
        "BLACKMETAL", ISET_BLACKMETAL
    );

    // Item properties the tables declare. Nothing consumes these yet - the
    // engine has never acted on one - but the rows that name them would
    // otherwise say nothing at all.
    lua.new_enum("SpecialProperty",
        "NONE", SPP_NONE,
        "FAST_DIGESTION", SPP_FASTDIGESTION,
        "SLOW_DIGESTION", SPP_SLOWDIGESTION,
        "REGENERATION", SPP_REGENERATION
    );

    lua.new_enum("ItemQuality",
        "TERRIBLE", IQ_TERRIBLE,
        "VERY_BAD", IQ_VERY_BAD,
        "BAD", IQ_BAD,
        "POOR", IQ_POOR,
        "AVG", IQ_AVG,
        "FAIR", IQ_FAIR,
        "GOOD", IQ_GOOD,
        "EXCELLENT", IQ_EXCELLENT,
        "SUPERB", IQ_SUPERB
    );

    // What stage a use is at, and what a handler answers with. Both are
    // for content-defined tools (XLuaTool::onUse): the use command calls a
    // handler with START, then PROGRESS for as long as it answers CONTINUE,
    // and FINISH when it stops.
    lua.new_enum("ItemUse",
        "START", XTool::START,
        "PROGRESS", XTool::PROGRESS,
        "FINISH", XTool::FINISH
    );

    lua.new_enum("Result",
        "FAIL", FAIL,
        "SUCCESS", SUCCESS,
        "CONTINUE", CONTINUE,
        "ABORT", ABORT
    );

    // How well a food sits once eaten (XAnyFood::onEat).
    lua.new_enum("FoodType",
        "BEST", FT_BESTFOOD,
        "GOOD", FT_GOODFOOD,
        "NORMAL", FT_NORMALFOOD,
        "BAD", FT_BADFOOD,
        "VERY_BAD", FT_VERYBADFOOD,
        "VOMIT", FT_VOMIT
    );

    // Every table above holds a fixed set of names the world scripts
    // spell out by hand, so a name that is not in one is a typo, not a
    // value. XTileType is here too: it is filled by DefineTile() as
    // world/tiles.lua runs, and read by name everywhere after that.
    for (const char* enum_table : {
            "BodyPart", "CorpseEffectType", "CreatureClass",
            "CreatureSize", "CreatureTemplate", "FoodType", "Gender",
            "ItemKind", "FoodFeeling", "ItemQuality", "ItemSet", "ItemUse", "LuaEvent", "Movability",
            "MagicSchool", "Modifier", "PersonType", "PotionColor", "Result", "SpecialProperty", "SpellUse", "ScriptCommand", "ShopDoor",
            "Visibility", "xColor", "XEffect", "XLocation",
            "XQuest", "XSkill", "XStairWay", "XStandardAI",
            "BrandGroup", "CombatGroup", "CombatRole", "XStats", "XTileType",
        }) {
        MakeStrict(lua, enum_table);
    }

    // Still the only two bindings registered through the raw Lua C API
    // rather than sol2 - they take a lua_State* and hand-roll the stack.

    lua.open_libraries(sol::lib::base, sol::lib::string);

    // Sol2-bound Monster builder - registered before world scripts
    // load below, since world/creatures.lua calls it while loading.
    {
        lua.new_usertype<MonsterBuilder>("Monster",
            sol::constructors<MonsterBuilder(CREATURE_NAME), MonsterBuilder(CREATURE_NAME, CREATURE_NAME)>(),
            "View", &MonsterBuilder::View,
            "Basic", &MonsterBuilder::Basic,
            "Body", &MonsterBuilder::Body,
            "AI", &MonsterBuilder::AI,
            "Stats", &MonsterBuilder::Stats,
            "Resist", &MonsterBuilder::Resist,
            "Combat", &MonsterBuilder::Combat,
            "Main", &MonsterBuilder::Main,
            "Description", &MonsterBuilder::Description,
            "Melee", &MonsterBuilder::Melee,
            "MeleeExtra", &MonsterBuilder::MeleeExtra,
            "LearnSkill", &MonsterBuilder::LearnSkill,
            "LearnSpell", &MonsterBuilder::LearnSpell,
            "Equip", &MonsterBuilder::Equip,
            "EquipCount", &MonsterBuilder::EquipCount,
            "Corpse", &MonsterBuilder::Corpse,
            "CorpseEffect", &MonsterBuilder::CorpseEffect,
            "CorpseStat", &MonsterBuilder::CorpseStat,
            "CorpseResist", &MonsterBuilder::CorpseResist,
            "CorpseModifier", &MonsterBuilder::CorpseModifier,
            "Unique", &MonsterBuilder::Unique,
            "Register", &MonsterBuilder::Register
        );
    }

    // Sol2-bound builder for one row of an ordinary item's table.
    {
        lua.new_usertype<MaterialBuilder>("Material",
            sol::constructors<MaterialBuilder(std::string)>(),
            "Called", &MaterialBuilder::Called,
            "Looks", &MaterialBuilder::Looks,
            "Sets", &MaterialBuilder::Sets,
            "Chance", &MaterialBuilder::Chance,
            "Quality", &MaterialBuilder::Quality,
            "Body", &MaterialBuilder::Body,
            "Armour", &MaterialBuilder::Armour,
            "Combat", &MaterialBuilder::Combat,
            "Resist", &MaterialBuilder::Resist,
            "Property", &MaterialBuilder::Property,
            "Register", &MaterialBuilder::Register
        );

        lua.new_usertype<ArmourEnchantmentBuilder>("ArmourEnchantment",
            sol::constructors<ArmourEnchantmentBuilder(std::string)>(),
            "Called", &ArmourEnchantmentBuilder::Called,
            "Looks", &ArmourEnchantmentBuilder::Looks,
            "Rarity", &ArmourEnchantmentBuilder::Rarity,
            "Weight", &ArmourEnchantmentBuilder::Weight,
            "Fits", &ArmourEnchantmentBuilder::Fits,
            "Brand", &ArmourEnchantmentBuilder::Brand,
            "Property", &ArmourEnchantmentBuilder::Property,
            "Stats", &ArmourEnchantmentBuilder::Stats,
            "Resist", &ArmourEnchantmentBuilder::Resist,
            "Register", &ArmourEnchantmentBuilder::Register
        );

        lua.new_usertype<EnchantmentBuilder>("Enchantment",
            sol::constructors<EnchantmentBuilder(std::string)>(),
            "Called", &EnchantmentBuilder::Called,
            "Armour", &EnchantmentBuilder::Armour,
            "Combat", &EnchantmentBuilder::Combat,
            "Range", &EnchantmentBuilder::Range,
            "Resist", &EnchantmentBuilder::Resist,
            "Stats", &EnchantmentBuilder::Stats,
            "Worth", &EnchantmentBuilder::Worth,
            "Register", &EnchantmentBuilder::Register
        );

        lua.set_function("EnchantmentLooks", &SetEnchantmentLooks);

        lua.new_usertype<HerbBuilder>("Herb",
            sol::constructors<HerbBuilder(std::string)>(),
            "Called", &HerbBuilder::Called,
            "Growing", &HerbBuilder::Growing,
            "Mushroom", &HerbBuilder::Mushroom,
            "Taste", &HerbBuilder::Taste,
            "Looks", &HerbBuilder::Looks,
            "Register", &HerbBuilder::Register
        );

        lua.new_usertype<BookBuilder>("Book",
            sol::constructors<BookBuilder(std::string)>(),
            "Chance", &BookBuilder::Chance,
            "Register", &BookBuilder::Register
        );

        lua.set_function("BookAppearances", &SetBookAppearances);

        lua.new_usertype<PotionBuilder>("Potion",
            sol::constructors<PotionBuilder(std::string)>(),
            "Called", &PotionBuilder::Called,
            "Effect", &PotionBuilder::Effect,
            "Chance", &PotionBuilder::Chance,
            "Worth", &PotionBuilder::Worth,
            "Alchemy", &PotionBuilder::Alchemy,
            "Looks", &PotionBuilder::Looks,
            "OnDrink", &PotionBuilder::OnDrink,
            "Register", &PotionBuilder::Register
        );

        lua.new_usertype<SpellBuilder>("Spell",
            sol::constructors<SpellBuilder(std::string)>(),
            "Called", &SpellBuilder::Called,
            "Effect", &SpellBuilder::Effect,
            "School", &SpellBuilder::School,
            "Cost", &SpellBuilder::Cost,
            "Use", &SpellBuilder::Use,
            "Register", &SpellBuilder::Register
        );

        lua.new_usertype<ScrollBuilder>("Scroll",
            sol::constructors<ScrollBuilder(std::string)>(),
            "Called", &ScrollBuilder::Called,
            "Effect", &ScrollBuilder::Effect,
            "Worth", &ScrollBuilder::Worth,
            "Chance", &ScrollBuilder::Chance,
            "ReadInCombat", &ScrollBuilder::ReadInCombat,
            "OnRead", &ScrollBuilder::OnRead,
            "Register", &ScrollBuilder::Register
        );

        lua.new_usertype<TemplateBuilder>("Template",
            sol::constructors<TemplateBuilder(ItemKind, ItemType)>(),
            "View", &TemplateBuilder::View,
            "Made", &TemplateBuilder::Made,
            "Skill", &TemplateBuilder::Skill,
            "Verb", &TemplateBuilder::Verb,
            "Launcher", &TemplateBuilder::Launcher,
            "OnCreate", &TemplateBuilder::OnCreate,
            "OnOutfit", &TemplateBuilder::OnOutfit,
            "Worth", &TemplateBuilder::Worth,
            "Armour", &TemplateBuilder::Armour,
            "Combat", &TemplateBuilder::Combat,
            "Range", &TemplateBuilder::Range,
            "Chance", &TemplateBuilder::Chance,
            "Register", &TemplateBuilder::Register
        );
    }

    // Sol2-bound Item builder: everything that is not food, from a
    // machine part to an artifact.
    {
        lua.new_usertype<ItemBuilder>("Item",
            sol::constructors<ItemBuilder(std::string)>(),
            "Plain", &ItemBuilder::Plain,
            "Weapon", &ItemBuilder::Weapon,
            "Cap", &ItemBuilder::Cap,
            "Shield", &ItemBuilder::Shield,
            "Cloak", &ItemBuilder::Cloak,
            "Tool", &ItemBuilder::Tool,
            "View", &ItemBuilder::View,
            "Type", &ItemBuilder::Type,
            "Basic", &ItemBuilder::Basic,
            "Armour", &ItemBuilder::Armour,
            "Combat", &ItemBuilder::Combat,
            "Stats", &ItemBuilder::Stats,
            "Resist", &ItemBuilder::Resist,
            "Brand", &ItemBuilder::Brand,
            "Called", &ItemBuilder::Called,
            "Unique", &ItemBuilder::Unique,
            "Artifact", &ItemBuilder::Artifact,
            "Use", &ItemBuilder::Use,
            "Random", &ItemBuilder::Random,
            "Register", &ItemBuilder::Register
        );
    }

    // Sol2-bound Food builder, registered here for the same reason as the
    // Monster builder above: world/items.lua calls it while loading.
    {
        lua.new_usertype<FoodBuilder>("Food",
            sol::constructors<FoodBuilder(std::string)>(),
            "View", &FoodBuilder::View,
            "Basic", &FoodBuilder::Basic,
            "Nutrition", &FoodBuilder::Nutrition,
            "Taste", &FoodBuilder::Taste,
            "Random", &FoodBuilder::Random,
            "Register", &FoodBuilder::Register
        );
    }

    // Sol2-bound location/map-building functions
    // Registered before world scripts load below, since
    // locations.lua/valley.lua call these while loading.
    {
        lua_api::RegisterActorApi(lua);
    lua_api::RegisterWorldApi(lua);

    }

    {









    }

    lua.script_file("./world/init.lua");

    // Catch Lua errors loading data.
    // The message is what says which file and line went wrong.
    if (const sol::protected_function_result result = lua["LoadScripts"]();
        !result.valid()) {
        const sol::error err = result;
        std::cerr << "world: " << err.what() << std::endl;
        assert(false && "LoadScripts() failed - see the message above");
    }

    // Nothing can be drawn or walked on before this holds.
    XTileType::ValidateTiles();
    XCreatureStorage::CreateQuickBase();
}

bool XLua::ResultToBool(const sol::protected_function_result& result,
                        const std::string_view handler)
{
    if (!result.valid()) {
        return false;
    }

    if (const sol::optional<bool> answer = result.get<sol::optional<bool>>()) {
        return *answer;
    }

    // See the declaration in engine/xlua.h for why a number has to be
    // called out rather than quietly taken for a "no".
    if (result.get_type() == sol::type::number) {
        std::cerr << "world: " << handler
                  << " answered with a number - event handlers answer true or false"
                  << std::endl;
    }

    return false;
}
