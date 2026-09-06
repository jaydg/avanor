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

#include <algorithm>
#include <iostream>
#include <sol/sol.hpp>

#include "creature/anycr.h"
#include "creature/shopkeeper.h"
#include "engine/xgen.h"
#include "engine/xlua.h"
#include "map/dungeon_builder.h"
#include "map/windroad.h"
#include "game/game.h"
#include "game/location.h"
#include "game/shop.h"
#include "item/itemf.h"
#include "item/item_misc.h"
#include "item/xcorpse.h"
#include "item/xherb.h"
#include "item/xmoney.h"
#include "item/xscroll.h"
#include "item/xbook.h"
#include "item/xpotion.h"
#include "lua/api_world.h"
#include "map/map_objects.h"

// CreateObject's potion overload is a borderline case kept for now only
// because sol::overload has to register all three together - it should
// collapse into one id-based call once object identity is data rather
// than a compiled enum.

namespace lua_api
{

// CreateLocation, BuildShop, DrawPattern and CreateTimerEvent stayed on
// XLocation: each reaches into its private state (brief_name/BuildCave/
// BuildPlain, the protected CreateShop(), pat_offs_x/PutPalette, event/ttm
// respectively), so they are location implementation rather than engine
// API. They still register here, and would move once XLocation grows a
// public seam for them.

//Settle(CreatureClass.RAT + CreatureClass.FELINE + CreatureClass.INSECT, CreatureTemplate.VERY_LOW)
//Settle(CreatureClass.RAT, CreatureTemplate.LOW)
//Settle(CreatureClass.RAT, CreatureTemplate.LOW, 4, 50000)
//
// `max_creature` is a ceiling per creature *class*, not per level: a mask
// naming eight classes settles up to eight times that many. `refresh` is
// how long between one spawn attempt and the next.
void Settle(CreatureClass crc, int crl, sol::optional<int> max_creature, sol::optional<int> refresh)
{
    Game.Scheduler.Add(new XUniversalGen(XLocation::current_location, crc,
                                         static_cast<CreatureTemplate::Level>(crl),
                                         max_creature.value_or(5), refresh.value_or(25000)));
}

//cr = Creature("rotmoth")
//cr = Creature("rat", [x, y, [w, h]])
sol::optional<void*> Creature(const std::string& crn, sol::optional<int> x, sol::optional<int> y, sol::optional<int> w, sol::optional<int> h)
{
    XCreature * cr = nullptr;

    if (!x) {
        cr = XLocation::current_location->NewCreature(crn);
    } else {
        int tx = *x;
        int ty = *y;
        XRect rect = w ? XRect(tx, ty, tx + *w, ty + *h) : XRect(tx, ty, tx + 1, ty + 1);
        cr = XLocation::current_location->NewCreature(crn, rect);
    }

    // No room left to put one - nil, not a null pointer dressed as a
    // value. See GetWornItem() in api_actor.cpp.
    if (!cr) {
        return sol::nullopt;
    }

    return static_cast<void*>(cr);
}

//cr = Guardian("dwarf_guard", GID_DWARVEN_GUARDIAN, x, y, [len,  hgt], [flags])
void* Guardian(const std::string& crn, const std::string& gid, int x, int y, sol::optional<int> w, sol::optional<int> h, sol::optional<int> flags)
{
    XRect rect = w ? XRect(x, y, x + *w, y + *h) : XRect(x, y, x + 1, y + 1);
    int flag = XStandardAI::GUARD_AREA;

    if (flags) {
        flag |= *flags;
    }

    XCreature * cr = XLocation::current_location->NewCreature(crn, rect, gid, flag);

    // NewCreature() already set enemy_class to NONE for a PEACEFUL
    // creature (see its own comment) - respect that instead of
    // unconditionally overwriting it here. Without this check, every
    // Guardian()-spawned PEACEFUL creature (farmers, goodwives, and
    // anyone else inheriting their template) ends up hostile to every
    // non-human/humanoid class regardless, silently defeating the whole
    // point of being flagged PEACEFUL.
    if (!(cr->xai->GetAIFlag() & XStandardAI::PEACEFUL)) {
        cr->xai->SetEnemyClass(CreatureClass::ALL ^ (CreatureClass::HUMAN | CreatureClass::HUMANOID));
    }

    return cr;
}

//GuardianClass(CreatureClass.ORC, "orcs_war_party", 10, 70, 20, 10, XStandardAI.GUARD_AREA)
void* GuardianClass(CreatureClass crc, const std::string& gid, int x, int y, sol::optional<int> w, sol::optional<int> h, sol::optional<int> flags)
{
    XRect rect = w ? XRect(x, y, x + *w, y + *h) : XRect(x, y, x + 1, y + 1);
    int flag = XStandardAI::GUARD_AREA;

    if (flags) {
        flag |= *flags;
    }

    return XLocation::current_location->NewCreature(crc, rect, gid, flag);
}

//SetStartLocation("MAIN", 26, 4, 6, 5)
// Names the location a new hero starts in, optionally narrowed to an
// area of it. Without a call to this there is no world to start a game
// in, which is why XLocation::ValidateWorld() checks the result.
void SetStartLocation(const std::string& loc_id, sol::optional<int> x, sol::optional<int> y,
                      sol::optional<int> w, sol::optional<int> h)
{
    XGame::start_location = loc_id;
    XGame::start_area = (x && y && w && h)
                            ? std::optional<XRect>(XRect(*x, *y, *x + *w, *y + *h))
                            : std::nullopt;
}

//SetWanderingAllowed("MAIN", false)
// Says whether AI creatures may wander into a location.
void SetWanderingAllowed(const std::string& loc_id, const bool allowed)
{
    if (const auto loc = Game.Location(loc_id)) {
        loc->allow_wandering_in = allowed;
    }
}

//Teleport(23, 20, "MAIN", 154, 13)
void Teleport(int x, int y, const std::string& target_loc_id, int dest_x, int dest_y)
{
    new XTeleport(x, y, XLocation::current_location, target_loc_id, dest_x, dest_y);
}

//Way(XStairWay.DOWN, "SMALL_CAVE_2")
//Way(XStairWay.DOWN, "SMALL_CAVE_2", x, y)
void Way(const XStairWay::Type type, const std::string& loc_id, sol::optional<int> x, sol::optional<int> y)
{
    if (x) {
        XLocation::current_location->NewWay(*x, *y, loc_id, type);
    } else {
        XLocation::current_location->NewWay(loc_id, type);
    }
}

//CreateObject("XCookingSet")
void* CreateObjectByName(const std::string& name)
{
    // Content first: a name world/ defined for itself wins over a C++ class
    // name, so CreateObject() reads the same either way and a food that
    // moves out of C++ needs no new call form.
    if (XItem* defined = XItemStorage::Create(name)) {
        return defined;
    }

    return XClassFactory::CreateNew((char*)name.c_str());
}

//CreateObject(ItemKind::ITEM - ItemKind::FOOD, 20, 500)
void* CreateObjectByMask(int flag, int min_val, int max_val)
{
    return ICREATE((ItemKind)(flag), min_val, max_val);
}

//CreatePotion("healing")
//
// Its own call rather than a CreateObject overload: a potion id is a
// string now, and so is the content id CreateObject already takes, so one
// single-argument CreateObject could not tell which it had been handed.
void* CreatePotion(const std::string& pn)
{
    return new XPotion(pn);
}

//CreateObject(ItemKind.WEAPON, ItemType.LONGSWORD, 1, 100)
//
// One particular sort of item, worth somewhere between the two figures -
// what the hero's starting kit asks for. The material and the exact numbers
// are still rolled, so two long swords are not the same long sword.
void* CreateObjectOfType(ItemKind kind, ItemType it, int min_val, int max_val)
{
    return XItemFactory::CreateAnyItem(kind, it, min_val, max_val);
}

//CreateScroll("fire_bolt")
//
// Scrolls and books stay their own calls rather than another CreateObject
// overload: a book is still named by a number, so one single-argument
// CreateObject could not tell a book id from a scroll id.
void* CreateScroll(const std::string& scrn)
{
    return new XScroll(scrn);
}

//CreateBook("fire_bolt") - a book is named by the spell it teaches
void* CreateBook(const std::string& spell)
{
    return new XBook(spell);
}

// Makes an item known, as the hero's own starting gear is: nothing they
// began the game with should need identifying.
void IdentifyItem(void* item)
{
    if (item) {
        ((XItem*)item)->Identify();
    }
}

//DropItem(item, 0, 0)
//DropItem(item)
void DropItem(void* item, sol::optional<int> x, sol::optional<int> y)
{
    XItem * pItem = (XItem*)item;
    int tx;
    int ty;

    if (x) {
        tx = *x;
        ty = *y;
    } else {
        const auto pt = XLocation::current_location->GetFreeXY();

        if (!pt) {
            return;
        }

        tx = pt->x;
        ty = pt->y;
    }

    if (pItem) {
        pItem->Drop(XLocation::current_location, tx, ty);
    }
}

//DropItem(item, object)
void DropItemAt(void* item, void* object)
{
    XItem * pItem = (XItem*)item;
    XMapObject * pMO = (XMapObject*)object;
    pItem->Drop(pMO->l, pMO->x, pMO->y);
}

//SetPattern(width, height,
// "###" ..
// "#.#" ..
// "###")
void SetPattern(int w, int h, const std::string& txt)
{
    // A pattern is exactly w * h characters. A row miscounted or a character
    // dropped would otherwise draw the level shifted by one from the typo
    // onwards, which is harder to see in the finished map than here.
    if (const size_t wanted = static_cast<size_t>(std::max(w, 0)) * std::max(h, 0);
        txt.size() != wanted) {
        std::cerr << "world: " << XLocation::current_location->id << " defines a "
                  << w << "x" << h << " pattern, which wants " << wanted
                  << " characters, and gives " << txt.size() << std::endl;
    }

    XLocation::current_pattern.Setup(w, h, txt);
}

//AddTranslation("1", GOLDEN_FLOOR)
//AddTranslation("A", function(x, y) Guardian('dwarf_guard', GID_DWARVEN_GUARDIAN, x, y) end)
void AddTranslation(const std::string& view, sol::object target)
{
    if (target.get_type() == sol::type::function) {
        XLocation::current_pattern.AddTranslation(view[0], target.as<sol::protected_function>());
    } else {
        XLocation::current_pattern.AddTranslation(view[0], static_cast<XTileType::Id>(target.as<int>()));
    }
}

//DefineRoom(50, 9, 7, "####+####" .., { ['#'] = XTileType.MAGMA }, OnDrawn)
// Adds a room the dungeon generator may stamp into a level. weight is
// relative to the other rooms; translations reads like AddTranslation()'s
// pairs, a glyph to either a tile or a function(x, y); on_drawn, if
// given, is called with the room's (x, y, w, h) once it is on the map.
void DefineRoom(const int weight, const int w, const int h, const std::string& pattern, sol::table translations,
                sol::optional<sol::protected_function> on_drawn)
{
    RoomTemplate room;
    room.weight = weight;

    if (const size_t wanted = static_cast<size_t>(std::max(w, 0)) * std::max(h, 0);
        pattern.size() != wanted) {
        std::cerr << "world: a room defined as " << w << "x" << h << " wants " << wanted
                  << " characters, and gives " << pattern.size() << std::endl;
    }

    room.pattern.Setup(w, h, pattern);

    for (auto& [glyph, target] : translations) {
        const char ch = glyph.as<std::string>()[0];

        if (target.get_type() == sol::type::function) {
            room.pattern.AddTranslation(ch, target.as<sol::protected_function>());
        } else {
            room.pattern.AddTranslation(ch, static_cast<XTileType::Id>(target.as<int>()));
        }
    }

    if (on_drawn) {
        room.on_drawn = *on_drawn;
    }

    room_templates.push_back(std::move(room));
}

//SetDefaultTranslations{ ['#'] = XTileType.STONE_WALL, ['+'] = Door }
// The map alphabet: what a character means in any pattern that does not
// translate it itself. Same shape as a room's own translations - a tile,
// or a function(x, y) that puts something on the cell.
void SetDefaultTranslations(sol::table translations)
{
    std::vector<XPattern::Translation> defaults;

    for (auto& [glyph, target] : translations) {
        const char ch = glyph.as<std::string>()[0];

        if (target.get_type() == sol::type::function) {
            defaults.push_back({ch, XTileType::NONE, target.as<sol::protected_function>()});
        } else {
            defaults.push_back({ch, static_cast<XTileType::Id>(target.as<int>()), {}});
        }
    }

    XPattern::SetDefaults(std::move(defaults));
}

//DefineTile("MAGMA", "magma", '#', xColor.xDARKGRAY, Movability.WALL, Visibility.WALL)
// Adds a kind of ground: what it is called, what it looks like, and how
// hard it is to cross and to see through. The id it gets appears in the
// Lua table XTileType under the name given here, so everything else
// refers to it as XTileType.MAGMA.
void DefineTile(sol::this_state s, const std::string& id_name, const std::string& name,
                const std::string& view, const unsigned color,
                const XTileType::Movability movability, const XTileType::Visibility visibility,
                sol::optional<sol::table> properties)
{
    const XTileType::Id id = XTileType::Define(id_name, view[0], static_cast<unsigned>(color), name,
                                               movability, visibility);

    if (properties) {
        XTileType::SetDiggableInto(id, properties->get_or<std::string>("diggable_into", ""));
        XTileType::SetFertile(id, properties->get_or("fertile", false));
    }

    sol::state_view lua(s);
    lua["XTileType"][id_name] = id;
}

//SetRememberedBrightness(80)
// How bright ground the hero remembers but cannot currently see is
// drawn, against 100 for what is in plain sight.
void SetRememberedBrightness(const int percent)
{
    ::SetRememberedBrightness(percent);
}

//SetTileJitter(10)
//SetTileJitter(10, 12, 15)
// How far the colour of one patch of ground may stray from the colour of
// the tile it is, so that a field of one tile reads as ground rather than
// as a flat wash: brightness either side of the tile's own, per cent; then
// optionally how far the hue may turn, in degrees, and how much of the
// colour's saturation may come and go, per cent. The hue turns only a few
// degrees, so a green stays a green - it is what makes a stand of trees
// look like a wood. All three zero turns it off.
void SetTileJitter(const int percent, const sol::optional<int> hue_degrees,
                   const sol::optional<int> saturation_percent)
{
    ::SetTileJitter(percent, hue_degrees.value_or(12), saturation_percent.value_or(15));
}

//SetFloorPriority{ XTileType.GREEN_GRASS, XTileType.CAVE_FLOOR }
// Which tiles a pattern may invent underneath the things it places,
// later entries winning over earlier ones.
void SetFloorPriority(sol::table floors)
{
    std::vector<XTileType::Id> priority;

    for (size_t i = 1; i <= floors.size(); i++) {
        priority.push_back(static_cast<XTileType::Id>(floors.get<int>(i)));
    }

    XPattern::SetFloorPriority(std::move(priority));
}

//Door(x, y) / Door(x, y, true)
void Door(const int x, const int y, sol::optional<bool> opened)
{
    new XDoor(x, y, opened.value_or(false) ? 1 : 0, XLocation::current_location);
}

//Furniture(x, y, xLIGHTRED, '~', 'a royal bad')
void* Furniture(int x, int y, int color, const std::string& view, const std::string& descr)
{
    return new XFurniture(x, y, color, view[0], (char*)descr.c_str(), XLocation::current_location);
}

//OuterObject(xLIGHTRED, '~', 'a royal bad', 'EventHandler')
sol::optional<void*> OuterObject(int color, const std::string& view, const std::string& descr, sol::optional<std::string> event)
{
    const auto pt = XLocation::current_location->GetFreeXY();

    // Nowhere to put it - nil rather than a null pointer that reads as a
    // value. See GetWornItem() in api_actor.cpp.
    if (!pt) {
        return sol::nullopt;
    }

    return new XOuterObject(pt->x, pt->y, color, view[0], (char*)descr.c_str(), XLocation::current_location, event ? event->c_str() : nullptr);
}

//OuterObject(x, y, xLIGHTRED, '~', 'a royal bad', 'EventHandler')
void* OuterObjectAt(int x, int y, int color, const std::string& view, const std::string& descr, sol::optional<std::string> event)
{
    return new XOuterObject(x, y, color, view[0], (char*)descr.c_str(), XLocation::current_location, event ? event->c_str() : nullptr);
}

// Gold on the floor beside somebody, in the location they are standing in.
// Treasure() below cannot serve here: it drops into current_location, which
// only tracks world-building and is stale once play has started.
// Which potion a herb distils into, and how hard that is. Both are dealt
// out per game rather than declared, so content cannot read them from its
// own tables - it has to ask.
// Corpses are still C++, so these three ask about one rather than content
// reading its own tables. They go when corpses become content.

// An uncooked corpse - the only thing worth putting over a fire.
bool isRawCorpse(void* item)
{
    const auto* corpse = dynamic_cast<const XCorpse*>((const XItem*)item);
    return corpse && corpse->corpse_flag != CF_COOKED;
}

// Marks it cooked and renames it. What that is worth - the nutrition and
// the weight - is content's to say, through the item accessors.
void CookCorpse(void* item)
{
    if (auto* corpse = dynamic_cast<XCorpse*>((XItem*)item)) {
        corpse->Cook();
    }
}

// Whether a corpse rots while this is set - a pot on the fire holds it.
void StopCorpseRotting(void* item, const bool stopped)
{
    if (auto* corpse = dynamic_cast<XCorpse*>((XItem*)item)) {
        corpse->roating_stopped = stopped ? 1 : 0;
    }
}

sol::optional<std::string> HerbPotion(void* item)
{
    auto* herb = dynamic_cast<XHerb*>((XItem*)item);

    if (!herb) {
        return sol::nullopt;
    }

    const PotionName pn = herb->GetTargetPotion();

    if (pn.empty()) {
        return sol::nullopt;
    }

    return pn;
}

int PotionAlchemyPower(const std::string& pn)
{
    const PotionDescription* row = PotionDescription::GetRec(pn);
    return row ? row->alchemy_power : 0;
}

// The alchemy recipes of this game. Which potions can be mixed and into
// what is dealt out afresh each game from the potions' own :Alchemy()
// levels - like the herb mapping, it is per-game state, not content - so
// content asks for it rather than declaring it.
int AlchemyRecipeCount()
{
    return XAlchemy::GetRecipeCount();
}

// The two potions of one recipe and what they make, or nothing if there
// is no recipe with that number.
sol::optional<std::tuple<std::string, std::string, std::string>> AlchemyRecipe(const int num)
{
    if (const XAlchemyRecipe* rec = XAlchemy::GetRecipe(num)) {
        return std::make_tuple(rec->pn1, rec->pn2, rec->result);
    }

    return sol::nullopt;
}

// Teach somebody a recipe. Answers false when they knew it already,
// which is what tells a scroll it taught nothing.
bool LearnAlchemyRecipe(void* who, const std::string& pn1, const std::string& pn2,
    const std::string& result)
{
    XCreature* cr = (XCreature*)who;

    if (!cr || !cr->isHero()) {
        return false;
    }

    return ((XHero*)cr)->LearnRecipe(pn1, pn2, result) != 0;
}

void DropMoney(void* who, const int amount, const int x, const int y)
{
    XCreature* cr = (XCreature*)who;

    if (!cr || !cr->l) {
        return;
    }

    XMoney* money = new XMoney(amount);
    money->Drop(cr->l, x, y);
}

void Treasure(int x, int y, int val)
{
    XMoney * money = new XMoney(vRand(val) + val);
    money->Drop(XLocation::current_location, x, y);
}

void Chest(int x, int y, sol::optional<int> cnt, sol::optional<int> flg, sol::optional<int> mnval, sol::optional<int> mxval)
{
    XChest * tchest = new XChest(cnt.value_or(5), (ItemKind)flg.value_or(static_cast<int>(ItemKind::ITEM)), mnval.value_or(100), mxval.value_or(25000));
    tchest->Drop(XLocation::current_location, x, y);
}

void Trap(int x, int y)
{
    new XTrap(x, y, XLocation::current_location);
}

// Hands the new place to the location.
static void AddEventPlace(const XRect& area, const std::string& event)
{
    XLocation::current_location->AddPlace(
        new XAnyPlace(area, XLocation::current_location, event));
}

//EventPlace('MushroomCaveEvent')
void EventPlace(const std::string& event)
{
    XRect area(0, 0, XLocation::current_location->map->len, XLocation::current_location->map->hgt);
    AddEventPlace(area, event);
}

//EventPlace(x, y, 5, 2, 'SmallCaveEvent')
void EventPlaceArea(int x, int y, int w, int h, const std::string& event)
{
    XRect area(x, y, x + w, y + h);
    AddEventPlace(area, event);
}



// ---------------------------------------------------------------------
// Map queries and generic placement.
//
// These four exist so that scatter/decoration rules - how dense, on which
// terrain, avoiding which cells - can be written in Lua instead of being
// compiled in. ScatterHerbBushes() used to be a C++ binding that hard-coded
// "one cell in eighteen, on green grass, where nothing else stands"; it is
// now a Lua function in world/valley.lua built out of exactly these calls.
// ---------------------------------------------------------------------

// Every one of these takes an optional location handle - the same void*
// that a location event handler is passed - and falls back to the location
// currently being built. World-construction scripts want the default;
// runtime event handlers (world/locations/mushroom_cave.lua's mushroom
// spawner, say) are handed a specific location and must say so.
static XLocation* ResolveLocation(const sol::optional<void*>& location)
{
    return location ? static_cast<XLocation*>(*location) : XLocation::current_location;
}

std::tuple<int, int> GetMapSize(sol::optional<void*> location)
{
    const XMap* map = ResolveLocation(location)->map;

    return {map->len, map->hgt};
}

// What a wall becomes when somebody digs through it, or nothing if it is
// not the digging sort. Declared per tile in world/tiles.lua
// ({ diggable_into = "STONE_FLOOR" }); this only reads it back.
sol::optional<int> TileDiggableInto(const int tile)
{
    const XTileType::Id into = XTileType::DiggableInto((XTileType::Id)tile);

    if (into == XTileType::NONE) {
        return sol::nullopt;
    }

    return static_cast<int>(into);
}

int GetTile(const int x, const int y, sol::optional<void*> location)
{
    return ResolveLocation(location)->map->GetXY(x, y);
}

bool HasSpecial(const int x, const int y, sol::optional<void*> location)
{
    const XLocation* l = ResolveLocation(location);

    // Nowhere at all holds nothing - see GetSpecialId() for why this is
    // asked rather than left to assert.
    if (!l->map->Cell(x, y)) {
        return false;
    }

    return l->map->GetSpecial(x, y) != nullptr;
}

// What sort of thing stands in a cell, by the id content registered it
// under - so a plant can ask whether its neighbours are its own kind.
// Nothing there, or something the engine owns rather than content,
// answers nothing.
sol::optional<std::string> GetSpecialId(const int x, const int y,
    sol::optional<void*> location)
{
    const XLocation* l = ResolveLocation(location);

    // A handler asking about its neighbours runs off the edge of the map
    // at the edge of the map. That is an ordinary answer - "nothing
    // there" - not a reason to stop the game, which is what the
    // unguarded GetSpecial() below would do.
    if (!l->map->Cell(x, y)) {
        return sol::nullopt;
    }

    XMapObject* obj = l->map->GetSpecial(x, y);
    const auto* lua_obj = dynamic_cast<XLuaObject*>(obj);

    if (!lua_obj || !lua_obj->isValid()) {
        return sol::nullopt;
    }

    return lua_obj->GetContentId();
}

// The thing itself, to read its memory or act on it. Nothing there
// answers nothing.
sol::optional<void*> GetSpecial(const int x, const int y, sol::optional<void*> location)
{
    const XLocation* l = ResolveLocation(location);

    if (!l->map->Cell(x, y)) {
        return sol::nullopt;
    }

    XMapObject* obj = l->map->GetSpecial(x, y);

    if (!obj || !obj->isValid()) {
        return sol::nullopt;
    }

    return static_cast<void*>(obj);
}

// Whether things grow in a cell - what the ground is, not what stands on
// it.
bool TileFertile(const int x, const int y, sol::optional<void*> location)
{
    const XLocation* l = ResolveLocation(location);

    if (!l->map->Cell(x, y)) {
        return false;
    }

    return XTileType::isFertile(l->map->GetXY(x, y));
}

// Puts a content-defined thing on the map, and gives it back so script
// can set it up. Nothing when the id is unknown or the cell will not
// take it.
sol::object PlaceObject(const std::string& id, const int x, const int y,
    sol::this_state s, sol::optional<void*> location)
{
    if (!FindMapObject(id)) {
        std::cerr << "world: nothing defines a map object '" << id << "'" << std::endl;

        return sol::nil;
    }

    XLocation* l = ResolveLocation(location);

    // A hole showing the level below is not somewhere to put a thing:
    // it would be written into this level's cell and read back from the
    // one underneath, so nothing would ever see it again.
    if (!l->map->OwnsCell(x, y)) {
        return sol::nil;
    }

    auto* obj = new XLuaObject(id, x, y, l);

    if (!obj->isValid()) {
        return sol::nil;
    }

    return sol::make_object(s, static_cast<void*>(obj));
}

// Which map a thing on the map is standing on. A scheduled object runs
// wherever it happens to be, which is not necessarily the location the
// player is in - so a handler that asks about its own surroundings must
// say which map it means, and this is how it names it.
sol::optional<void*> GetObjectLocation(void* object)
{
    auto* obj = static_cast<XMapObject*>(object);

    if (!obj || !obj->l) {
        return sol::nullopt;
    }

    return static_cast<void*>(obj->l);
}

// Where a thing on the map is standing. Mirrors GetCreatureXY().
std::tuple<int, int> GetObjectXY(void* object)
{
    const auto* obj = static_cast<XMapObject*>(object);

    return obj ? std::make_tuple(obj->x, obj->y) : std::make_tuple(0, 0);
}

// What a thing on the map remembers between its turns.
int Recall(void* object, const std::string& key)
{
    const auto* obj = static_cast<XLuaObject*>(object);

    return obj ? obj->Remember(key) : 0;
}

void Memorise(void* object, const std::string& key, const int value)
{
    if (auto* obj = static_cast<XLuaObject*>(object)) {
        obj->Remember(key, value);
    }
}

//SetTile(x, y, XTileType.ROAD)
// The counterpart to GetTile(). Returns false and writes nothing for a cell
// outside the map - XMap::SetXY() asserts on one, and a script is allowed to
// ask about anywhere.
bool SetTile(const int x, const int y, const int tile, sol::optional<void*> location)
{
    const XMap* map = ResolveLocation(location)->map;

    if (x < 0 || y < 0 || x >= map->len || y >= map->hgt) {
        return false;
    }

    map->SetXY(x, y, static_cast<XTileType::Id>(tile));

    return true;
}

// One path, as an array of {x =, y =} in walking order.
static sol::table ToLuaPath(sol::this_state s, const std::vector<XPoint>& path)
{
    sol::state_view lua(s);
    sol::table road = lua.create_table(static_cast<int>(path.size()), 0);
    int i = 1;

    for (const XPoint& pt : path) {
        road[i++] = lua.create_table_with("x", pt.x, "y", pt.y);
    }

    return road;
}

//road = WindingRoad(x1, y1, x2, y2, 10)
sol::table WindingRoad(sol::this_state s, const int x1, const int y1,
                       const int x2, const int y2, const int pertamt,
                       sol::optional<void*> location)
{
    return ToLuaPath(s, windroad::Wind(*ResolveLocation(location)->map,
                                       XPoint(x1, y1), XPoint(x2, y2), pertamt));
}

//road = ZigzagRoad(x1, y1, x2, y2, 30, 0)
sol::table ZigzagRoad(sol::this_state s, const int x1, const int y1,
                      const int x2, const int y2, const int turnpct, const int diagpct)
{
    return ToLuaPath(s, windroad::Zigzag(XPoint(x1, y1), XPoint(x2, y2),
                                         turnpct, diagpct));
}

//road = SigsagRoad(x1, y1, x2, y2, 30, 0)
sol::table SigsagRoad(sol::this_state s, const int x1, const int y1,
                      const int x2, const int y2, const int turnpct, const int diagpct)
{
    return ToLuaPath(s, windroad::Sigsag(XPoint(x1, y1), XPoint(x2, y2),
                                         turnpct, diagpct));
}

// A randomly chosen walkable, unoccupied cell, or nil when the map has
// none - a direct pass-through of XLocation::GetFreeXY()'s own optional.
sol::optional<std::tuple<int, int>> GetFreeXY(sol::optional<void*> location)
{
    const auto pt = ResolveLocation(location)->GetFreeXY();

    if (!pt) {
        return sol::nullopt;
    }

    return std::make_tuple(pt->x, pt->y);
}

// Creates `class_name` through the same factory CreateObject() uses, then
// places it. Returns the object so script can keep configuring it, or nil
// when the class is unknown or the cell is unusable.
sol::object PlaceSpecial(const std::string& class_name, const int x, const int y,
                         sol::this_state s, sol::optional<void*> location)
{
    auto* obj = dynamic_cast<XMapObject*>(XClassFactory::CreateNew(class_name));

    if (!obj) {
        return sol::nil;
    }

    if (!obj->PlaceAt(ResolveLocation(location), x, y)) {
        obj->Invalidate();

        return sol::nil;
    }

    return sol::make_object(s, static_cast<void*>(obj));
}

// Which god an altar serves. Sacrificing on it goes to that god
// whatever the sacrificer would otherwise have chosen.
void SetAltarDeity(void* object, const std::string& deity)
{
    if (auto* altar = dynamic_cast<XAltar*>((XMapObject*)object)) {
        if (!deity.empty() && !FindDeity(deity)) {
            std::cerr << "world: an altar is dedicated to '" << deity
                      << "', which world/deities.lua does not declare" << std::endl;
            return;
        }

        altar->SetDeity(deity);
    }
}

void RegisterWorldApi(sol::state_view& lua)
{
    lua.set_function("GetMapSize", &lua_api::GetMapSize);
    lua.set_function("GetTile", &lua_api::GetTile);
    lua.set_function("TileDiggableInto", &lua_api::TileDiggableInto);
    lua.set_function("DropMoney", &lua_api::DropMoney);
    lua.set_function("isRawCorpse", &lua_api::isRawCorpse);
    lua.set_function("CookCorpse", &lua_api::CookCorpse);
    lua.set_function("StopCorpseRotting", &lua_api::StopCorpseRotting);
    lua.set_function("HerbPotion", &lua_api::HerbPotion);
    lua.set_function("PotionAlchemyPower", &lua_api::PotionAlchemyPower);
    lua.set_function("AlchemyRecipeCount", &lua_api::AlchemyRecipeCount);
    lua.set_function("AlchemyRecipe", &lua_api::AlchemyRecipe);
    lua.set_function("LearnAlchemyRecipe", &lua_api::LearnAlchemyRecipe);
    lua.set_function("HasSpecial", &lua_api::HasSpecial);
    lua.set_function("GetSpecialId", &lua_api::GetSpecialId);
    lua.set_function("GetSpecial", &lua_api::GetSpecial);
    lua.set_function("TileFertile", &lua_api::TileFertile);
    lua.set_function("PlaceObject", &lua_api::PlaceObject);
    lua.set_function("GetObjectXY", &lua_api::GetObjectXY);
    lua.set_function("GetObjectLocation", &lua_api::GetObjectLocation);
    lua.set_function("Recall", &lua_api::Recall);
    lua.set_function("Memorise", &lua_api::Memorise);
    lua.set_function("SetTile", &lua_api::SetTile);
    lua.set_function("WindingRoad", &lua_api::WindingRoad);
    lua.set_function("ZigzagRoad", &lua_api::ZigzagRoad);
    lua.set_function("SigsagRoad", &lua_api::SigsagRoad);
    lua.set_function("GetFreeXY", &lua_api::GetFreeXY);
    lua.set_function("PlaceSpecial", &lua_api::PlaceSpecial);
    lua.set_function("SetAltarDeity", &lua_api::SetAltarDeity);

    lua.set_function("CreateLocation", &XLocation::CreateLocation);
    lua.set_function("BuildShop", &XLocation::BuildShop);
        lua.set_function("Settle", &lua_api::Settle);
        lua.set_function("Creature", &lua_api::Creature);
        lua.set_function("Guardian", &lua_api::Guardian);
        lua.set_function("GuardianClass", &lua_api::GuardianClass);
        lua.set_function("SetStartLocation", &lua_api::SetStartLocation);
        lua.set_function("SetWanderingAllowed", &lua_api::SetWanderingAllowed);
        lua.set_function("Teleport", &lua_api::Teleport);
        lua.set_function("Way", &lua_api::Way);
        lua.set_function("CreateObject", sol::overload(&lua_api::CreateObjectByName, &lua_api::CreateObjectByMask, &lua_api::CreateObjectOfType));
        lua.set_function("CreatePotion", &lua_api::CreatePotion);
        lua.set_function("CreateScroll", &lua_api::CreateScroll);
        lua.set_function("CreateBook", &lua_api::CreateBook);
        lua.set_function("Identify", &lua_api::IdentifyItem);
        lua.set_function("DropItem", sol::overload(&lua_api::DropItem, &lua_api::DropItemAt));
        lua.set_function("SetPattern", &lua_api::SetPattern);
        lua.set_function("DefineRoom", &lua_api::DefineRoom);
        lua.set_function("AddTranslation", &lua_api::AddTranslation);
        lua.set_function("SetDefaultTranslations", &lua_api::SetDefaultTranslations);
        lua.set_function("SetFloorPriority", &lua_api::SetFloorPriority);
        lua.set_function("SetRememberedBrightness", &lua_api::SetRememberedBrightness);
        lua.set_function("SetTileJitter", &lua_api::SetTileJitter);
        lua.set_function("DefineTile", &lua_api::DefineTile);
        lua.set_function("Door", &lua_api::Door);
        lua.set_function("DrawPattern", &XLocation::DrawPattern);
        lua.set_function("Furniture", &lua_api::Furniture);
        lua.set_function("OuterObject", sol::overload(&lua_api::OuterObject, &lua_api::OuterObjectAt));
        lua.set_function("Treasure", &lua_api::Treasure);
        lua.set_function("Chest", &lua_api::Chest);
        lua.set_function("Trap", &lua_api::Trap);
        lua.set_function("EventPlace", sol::overload(&lua_api::EventPlace, &lua_api::EventPlaceArea));
        lua.set_function("CreateTimerEvent", &XLocation::CreateTimerEvent);
}

} // namespace lua_api
