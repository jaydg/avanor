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

#include "engine/xlua.h"
#include "creature/creature.h"
#include "game/game.h"
#include "game/location.h"
#include "helpers/msgwin.h"
#include "item/itemf.h"
#include "map/map.h"
#include "map/map_objects.h"

#include <fmt/format.h>
#include <cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>

#include <sol/sol.hpp>

REGISTER_CLASS(XTrap);
CEREAL_REGISTER_TYPE(XTrap);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XMapObject, XTrap);
// XTrap()'s only no-args constructor is an assert(0) guard - real
// instances always come from the parameterized constructor, so route
// Cereal's load-time construction through the DUMMY_STRUCT idiom
// instead of that assert.
CEREAL_LOAD_VIA_DUMMY_CONSTRUCT(XTrap, serialize);

std::vector<TrapRecipe> trap_recipes;

bool TrapRecipe::Accepts(const XItem* item) const
{
    if (!item || !(item->kind & loads_kind)) {
        return false;
    }

    for (const ItemType& allowed : loads) {
        if (item->it == allowed) {
            return true;
        }
    }

    return false;
}

std::vector<TrapTypeStats> trap_types_db;

const TrapTypeStats* FindTrapType(const TRAP_TYPE& id)
{
    for (const auto& row : trap_types_db) {
        if (row.id == id) {
            return &row;
        }
    }

    return nullptr;
}

TRAP_TYPE AnyTrapType()
{
    if (trap_types_db.empty()) {
        return TRAP_ANY;
    }

    return trap_types_db[vRand(static_cast<int>(trap_types_db.size()))].id;
}

TrapTypeBuilder::TrapTypeBuilder(std::string id)
{
    t.id = std::move(id);
}

TrapTypeBuilder& TrapTypeBuilder::Looks(const int colour)
{
    t.colour = colour;
    return *this;
}

TrapTypeBuilder& TrapTypeBuilder::Casts(const std::string& effect)
{
    t.casts = effect;
    return *this;
}

TrapTypeBuilder& TrapTypeBuilder::Fires(const int to_hit)
{
    t.harm = TrapHarm::FIRES;
    t.to_hit = to_hit;
    return *this;
}

TrapTypeBuilder& TrapTypeBuilder::Impales(const int to_hit, const std::string& message)
{
    t.harm = TrapHarm::IMPALES;
    t.to_hit = to_hit;
    t.message = message;
    return *this;
}

TrapTypeBuilder& TrapTypeBuilder::Hurts(const int count, const int sides, const int bonus,
    const int to_hit, const std::string& message)
{
    t.harm = TrapHarm::HURTS;
    t.count = count;
    t.sides = sides;
    t.bonus = bonus;
    t.to_hit = to_hit;
    t.message = message;
    return *this;
}

TrapTypeBuilder& TrapTypeBuilder::Pit()
{
    t.pit = true;
    return *this;
}

void TrapTypeBuilder::Register()
{
    if (t.id.empty()) {
        std::cerr << "world: a trap type with no id" << std::endl;
        return;
    }

    if (FindTrapType(t.id)) {
        std::cerr << "world: two trap types both called '" << t.id << "'" << std::endl;
        return;
    }

    if (!t.casts.empty() && !FindEffect(t.casts)) {
        std::cerr << "world: the trap '" << t.id << "' casts '" << t.casts
                  << "', which world/effects.lua does not declare" << std::endl;
    }

    trap_types_db.push_back(t);
}

const TrapRecipe* FindTrapRecipe(const TRAP_TYPE& type)
{
    for (const TrapRecipe& r : trap_recipes) {
        if (r.type == type) {
            return &r;
        }
    }

    return nullptr;
}

TrapRecipeBuilder::TrapRecipeBuilder(const std::string& name, const std::string& type)
{
    t.name = name;
    t.type = type;
}

TrapRecipeBuilder& TrapRecipeBuilder::Level(const int level)
{
    t.level = level;
    return *this;
}

TrapRecipeBuilder& TrapRecipeBuilder::Spell(const SPELL_NAME spell)
{
    t.spell = spell;
    return *this;
}

TrapRecipeBuilder& TrapRecipeBuilder::Loads(const ItemKind kind, const sol::table& types)
{
    t.loads_kind = kind;
    t.loads.clear();

    for (size_t i = 1; i <= types.size(); i++) {
        t.loads.push_back(types[i]);
    }

    return *this;
}

TrapRecipeBuilder& TrapRecipeBuilder::Tool(const ItemType tool, const std::string& tool_name)
{
    t.needs_tool = true;
    t.tool = tool;
    t.tool_name = tool_name;
    return *this;
}

TrapRecipeBuilder& TrapRecipeBuilder::Practice(const int practice)
{
    t.practice = practice;
    return *this;
}

TrapRecipeBuilder& TrapRecipeBuilder::Fills(const int min, const int max)
{
    t.fill_min = min;
    t.fill_max = max;
    return *this;
}

void TrapRecipeBuilder::Register()
{
    // The trap types are declared above the recipes that build them, so a
    // recipe naming one that does not exist is a typo.
    if (!FindTrapType(t.type)) {
        std::cerr << "world: the recipe '" << t.name << "' builds a trap of type '"
                  << t.type << "', which world/traps.lua does not declare"
                  << std::endl;
        return;
    }

    trap_recipes.push_back(t);
}

// A trap the world placed rather than a character built arrives loaded
// with whatever its recipe says one of its sort holds - the first of the
// types that recipe accepts, which is the ordinary one. A trap built by a
// character was handed its charge and keeps it.
void XTrap::LoadFromRecipe()
{
    if (trap_item != nullptr) {
        return;
    }

    const TrapRecipe* recipe = FindTrapRecipe(trap_type);

    if (!recipe || recipe->loads.empty() || recipe->fill_max <= 0) {
        return;
    }

    trap_item = XItem::Own(ICREATEB(recipe->loads_kind, recipe->loads.front(), 0, 100000));
    trap_item->quantity = vRand(recipe->fill_max - recipe->fill_min + 1) + recipe->fill_min;

}

void XTrap::RegisterLua(sol::state_view& lua)
{
    lua.new_usertype<MapObjectBuilder>("MapObject",
        sol::constructors<MapObjectBuilder(std::string)>(),
        "View", &MapObjectBuilder::View,
        "OnRun", &MapObjectBuilder::OnRun,
        "FirstDelay", &MapObjectBuilder::FirstDelay,
        "Register", &MapObjectBuilder::Register
    );

    lua.new_usertype<TrapTypeBuilder>("TrapType",
        sol::constructors<TrapTypeBuilder(std::string)>(),
        "Looks", &TrapTypeBuilder::Looks,
        "Casts", &TrapTypeBuilder::Casts,
        "Fires", &TrapTypeBuilder::Fires,
        "Impales", &TrapTypeBuilder::Impales,
        "Hurts", &TrapTypeBuilder::Hurts,
        "Pit", &TrapTypeBuilder::Pit,
        "Register", &TrapTypeBuilder::Register
    );

    lua.new_usertype<TrapRecipeBuilder>("TrapRecipe",
        sol::constructors<TrapRecipeBuilder(const std::string&, std::string)>(),
        "Level", &TrapRecipeBuilder::Level,
        "Spell", &TrapRecipeBuilder::Spell,
        "Loads", &TrapRecipeBuilder::Loads,
        "Tool", &TrapRecipeBuilder::Tool,
        "Practice", &TrapRecipeBuilder::Practice,
        "Fills", &TrapRecipeBuilder::Fills,
        "Register", &TrapRecipeBuilder::Register
    );
}

XTrap::XTrap(const int _x, const int _y, XLocation* _l, XTrap::Level tl, const TRAP_TYPE& tt, XCreature* _owner, XItem* items)
{
    SetLocation(_l);
    x = _x;
    y = _y;
    owner = XCreature::ToWeakPtr(_owner);
    trap_item = XItem::Own(items);

    // A trap asked for by no name is any of them.
    trap_type = tt.empty() ? AnyTrapType() : tt;

    if (tl == XTrap::Level::RANDOM) {
        tl = static_cast<XTrap::Level>(vRand(static_cast<unsigned long>(XTrap::Level::RANDOM)));
    }

    trap_level = tl;
    activation_count = vRand(20) + 5;

    // How it looks, whether it works by magic, and whether it holds
    // anything - all of it from the row world/traps.lua registered.
    const TrapTypeStats* row = FindTrapType(trap_type);

    if (!row) {
        std::cerr << "world: nothing defines a trap type '" << trap_type << "'"
                  << std::endl;
        color = xBROWN;
        isMagic = true;
    } else {
        color = static_cast<xColor>(row->colour);
        isMagic = !row->casts.empty();

        if (row->harm == TrapHarm::FIRES || row->harm == TrapHarm::IMPALES) {
            LoadFromRecipe();
        }
    }

    view = '^';

    if (auto o = owner.lock(); o && o->isHero()) {
        isVisibleForHero = 1;
    } else {
        isVisibleForHero = 0;
    }

    assert(l->map->GetSpecial(x, y) == nullptr);
    l->map->SetSpecial(x, y, this);
    name = "trap";
}

int XTrap::MoveIn(XCreature* cr)
{
    assert(isValid());
    assert(cr->isValid());

    if (cr->isHero()) {
        if (!isVisibleForHero) {
            return Activate(cr);
        }
    } else {
        if (cr->xai->isKnowThisTrap(this)) {
            if (cr->isVisible()) {
                msgwin.Add(cr->GetNameEx(CRN_T1));
                msgwin.Add(cr->GetVerb("avoid"));
                msgwin.Add("a trap.");
            }
        } else {
            return Activate(cr);
        }
    }

    return 0;
}

int XTrap::MoveOut(XCreature* cr)
{
    const TrapTypeStats* row = FindTrapType(trap_type);

    if (last_activator == cr->guid() && row && row->pit) {
        // to climb out pits you should be lucky!
        if (vRand(100) < 30 + cr->sk->GetLevel(XSkill::Skill::CLIMBING) * 5 + cr->GetStats(XStats::DEX) * 2) {
            cr->sk->UseSkill(XSkill::Skill::CLIMBING);

            if (cr->isVisible()) {
                msgwin.Add(cr->GetNameEx(CRN_T1));
                msgwin.Add("managed to climb out from the pit.");
            }

            last_activator = 0;
            return 1;
        }

        if (vRand(100) < 70) {
            cr->sk->UseSkill(XSkill::Skill::CLIMBING);

            if (cr->isVisible()) {
                msgwin.Add(cr->GetNameEx(CRN_T1));
                msgwin.Add("can not to climb out from the pit.");
            }
        } else {
            // no luck!
            Activate(cr);
        }

        return 0;
    }

    last_activator = 0;
    return 1;
}

int XTrap::Activate(XCreature* cr)
{
    const TrapTypeStats* row = FindTrapType(trap_type);

    if (cr->isVisible()) {
        msgwin.Add(cr->GetNameEx(CRN_T1));

        if (row && row->pit) {
            msgwin.Add(cr->GetVerb("fall"));
            msgwin.Add("down to a pit.");
        } else {
            msgwin.Add(cr->GetVerb("activate"));
            msgwin.Add("a trap.");
        }

        isVisibleForHero = 1;
    }

    bool isTrapShouldDestroyed = false;
    last_activator = cr->guid();

    if (isMagic) {
        EFFECT_DATA ed{};
        ed.caller	= owner.lock().get();
        ed.l	= l;
        ed.power	= 10 * (static_cast<int>(trap_level) + 1);
        ed.call_x	= x;
        ed.call_y	= y;
        ed.target_x	= x;
        ed.target_y	= y;
        ed.target	= cr;

        ed.effect = row ? row->casts : EFFECT_NONE;

        XEffect::Make(&ed);
    } else {
        XItem* drop_item = nullptr;
        DAMAGE_DATA_EX dd{};

        switch (row ? row->harm : TrapHarm::NONE) {
            case TrapHarm::FIRES:
                drop_item = trap_item->MakeCopy();
                drop_item->quantity = 1;

                if (trap_item->quantity-- <= 1) {
                    isTrapShouldDestroyed = true;
                }

                dd.damage	= drop_item->dice.Throw();
                dd.attacker	= owner.lock().get();

                dd.attack_name = drop_item->GetNameEx(XItem::Article::DEFINITE);

                dd.attack_HIT = row->to_hit;
                dd.attack_effect = drop_item->aet;
                dd.flags = DF_MAGIC_BOLT;
                cr->InflictDamage(&dd);
                break;

            case TrapHarm::HURTS:
                dd.damage	= XDice(row->count, row->sides, row->bonus).GetResult();
                dd.attacker	= owner.lock().get();
                dd.attack_name	= row->message;
                dd.attack_HIT	= row->to_hit;
                dd.attack_effect	= BrandSet();
                dd.flags	= DF_MAGIC_BOLT;
                cr->InflictDamage(&dd);
                break;

            case TrapHarm::IMPALES: {
                dd.damage	= 0;

                for (int i = 0; i < trap_item->quantity; i++) {
                    dd.damage += trap_item->dice.Throw();
                }

                dd.attacker	= owner.lock().get();
                dd.attack_name	= row->message;
                dd.attack_HIT	= row->to_hit;
                dd.attack_effect	= BrandSet();
                dd.flags	= DF_MAGIC_BOLT;
                cr->InflictDamage(&dd);
            }
            break;

            case TrapHarm::NONE:
                break;
        }

        if (drop_item) {
            drop_item->Drop(l, x, y);
        }
    }

    if (isTrapShouldDestroyed || activation_count-- <= 0) {
        if (isInVisibleArea()) {
            msgwin.Add("The trap is broken.");
        }

        // Invalidate() handles the map-cell eviction itself now (see
        // XMapObject::Invalidate() + the deferred-release graveyard) -
        // no separate SetSpecial() call, no ordering to get wrong.
        Invalidate();
    }

    return 1;
}

int XTrap::Check(XCreature* cr)
{
    if (isVisibleForHero) {
        return 0;
    }

    XSkill * skill = cr->sk->GetSkill(XSkill::Skill::DETECTTRAP);
    int chance = cr->GetStats(XStats::PER) * 10;

    if (skill) {
        chance += skill->GetLevel() * 50;
    }

    chance -= static_cast<int>(trap_level) * 100;

    if (vRand() % 1000 < chance) {
        if (cr->isVisible()) {
            msgwin.Add(cr->GetNameEx(CRN_T1));
            msgwin.Add(cr->GetVerb("found"));
            msgwin.Add("a trap.");
            isVisibleForHero = 1;

            if (skill) {
                skill->UseSkill();
            }
        }

        return 1;
    }

    return 0;
}

bool XTrap::isDiscovered() const
{
    return isVisibleForHero != 0;
}

int XTrap::Disarm(XCreature * cr)
{
    const XSkill* skill = cr->sk->GetSkill(XSkill::Skill::DISARMTRAP);
    int chance = 10;

    if (skill) {
        chance += skill->GetLevel() * 100;
    }

    chance -= static_cast<int>(trap_level) * 100;
    int val = vRand() % 1000;

    if (val < chance) {
        msgwin.Add(cr->GetNameEx(CRN_T1));
        msgwin.Add(cr->GetVerb("disarm"));
        msgwin.Add("a trap successfully.");

        // Invalidate() handles the map-cell eviction itself (see
        // XMapObject::Invalidate()).
        Invalidate();

        return 1;
    }

    if (val < static_cast<int>(chance * 1.3)) {
        msgwin.Add(cr->GetNameEx(CRN_T1));
        msgwin.Add("doesn't manage to disarm a trap.");
    } else {
        Activate(cr);
    }

    return 0;
}

void XTrap::OnInvalidate()
{
    if (trap_item) {
        trap_item->Invalidate();
        trap_item = nullptr;
    }

    XMapObject::OnInvalidate();
}

REGISTER_CLASS(XStairWay);
CEREAL_REGISTER_TYPE(XStairWay);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XMapObject, XStairWay);

void XStairWay::RegisterLua(sol::state_view& lua)
{
    lua.new_enum("XStairWay",
        "UP", Type::UP,
        "DOWN", Type::DOWN
    );
}

XStairWay::XStairWay(const int _x, const int _y, XLocation* loc, const std::string& _ln, const Type type)
{
    // Unbound until Bind() finds the matching stairway on the other side.
    dest_x = -1;
    dest_y = -1;
    x = _x;
    y = _y;
    ln = _ln;
    ttm = 0;
    color = xLIGHTGRAY;
    l = loc;

    assert(loc->map->GetSpecial(x, y) == nullptr);

    switch (type) {
        case Type::UP:
            view = '<';
            loc->map->SetSpecial(x, y, this);
            name = "way up";
            break;

        case Type::DOWN:
            view = '>';
            loc->map->SetSpecial(x, y, this);
            name = "way down";
            break;

        default:
            assert(false);
            break;
    }
}

void XStairWay::Bind(XStairWay * way)
{
    dest_x = way->x;
    dest_y = way->y;
    way->dest_x = x;
    way->dest_y = y;
}

REGISTER_CLASS(XTeleport);
CEREAL_REGISTER_TYPE(XTeleport);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XMapObject, XTeleport);

XTeleport::XTeleport(const int _x, const int _y, XLocation* loc, const std::string& _ln, const int _dest_x, const int _dest_y)
{
    dest_x = _dest_x;
    dest_y = _dest_y;
    x = _x;
    y = _y;
    ln = _ln;
    ttm = 0;
    color = xWHITE;
    l = loc;
    assert(loc->map->GetSpecial(x, y) == nullptr);

    view = '0';
    loc->map->SetSpecial(x, y, this);
    name = "magic circle";
}

int XTeleport::MoveIn(XCreature* cr)
{
    if (!cr->isHero()) {
        return 1; // Citizens shouldn't want to go visit the village...
    }

    if (Game.Location(ln)->map->XGetMovability(dest_x, dest_y) == 0) {
        cr->LastStep();
        cr->FirstStep(dest_x, dest_y, Game.Location(ln).get());
    }

    return 1;
}

REGISTER_CLASS(XDoor);
CEREAL_REGISTER_TYPE(XDoor);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XMapObject, XDoor);

XDoor::XDoor(const int _x, const int _y, const int flg, XLocation* _l)
{
    SetLocation(_l);
    x = _x;
    y = _y;
    isOpened = !flg;
    Switch();
    color = xBROWN;

    assert(l->map->GetSpecial(x, y) == nullptr);
    l->map->SetSpecial(x, y, this);
    name = "door";
}

void XDoor::Switch()
{
    if (isOpened) {
        isOpened = 0;
        view = '+';
    } else {
        isOpened = 1;
        view = '/';
    }
}

REGISTER_CLASS(XAltar);
CEREAL_REGISTER_TYPE(XAltar);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XMapObject, XAltar);

XAltar::XAltar(const int _x, const int _y, const DEITY& _deity, XLocation* _l)
{
    const bool placed = PlaceAt(_l, _x, _y);
    assert(placed);

    deity = _deity;
}

bool XAltar::PlaceAt(XLocation* location, const int _x, const int _y)
{
    if (!XMapObject::PlaceAt(location, _x, _y)) {
        return false;
    }

    view = '_';
    name = "altar";
    color = xWHITE;

    return true;
}

//////////////////////////////////////////////////////////////////////
// XLuaObject
//////////////////////////////////////////////////////////////////////

std::vector<MapObjectStats> map_objects_db;

const MapObjectStats* FindMapObject(const std::string& id)
{
    for (const auto& row : map_objects_db) {
        if (row.id == id) {
            return &row;
        }
    }

    return nullptr;
}

MapObjectBuilder::MapObjectBuilder(std::string id)
{
    t.id = std::move(id);
}

MapObjectBuilder& MapObjectBuilder::View(const std::string& name,
    const std::string& view, sol::optional<int> colour)
{
    t.name = name;

    if (!view.empty()) {
        t.view = view[0];
    }

    t.colour = colour.value_or(xLIGHTGRAY);

    return *this;
}

MapObjectBuilder& MapObjectBuilder::OnRun(const std::string& handler)
{
    t.on_run = handler;
    return *this;
}

MapObjectBuilder& MapObjectBuilder::FirstDelay(const int min, sol::optional<int> max)
{
    t.first_delay_min = min;
    t.first_delay_max = max.value_or(min);
    return *this;
}

void MapObjectBuilder::Register()
{
    if (t.id.empty()) {
        std::cerr << "world: a map object with no id" << std::endl;
        return;
    }

    if (FindMapObject(t.id)) {
        std::cerr << "world: two map objects both called '" << t.id << "'" << std::endl;
        return;
    }

    if (t.name.empty()) {
        t.name = t.id;
    }

    map_objects_db.push_back(t);
}

REGISTER_CLASS(XLuaObject);
CEREAL_REGISTER_TYPE(XLuaObject);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XMapObject, XLuaObject);

XLuaObject::XLuaObject(const std::string& id, const int _x, const int _y, XLocation* _l)
{
    content_id = id;
    static_cast<void>(PlaceAt(_l, _x, _y));
}

int XLuaObject::Remember(const std::string& key) const
{
    const auto it = memory.find(key);

    return it == memory.end() ? 0 : it->second;
}

void XLuaObject::Remember(const std::string& key, const int value)
{
    memory[key] = value;
}

bool XLuaObject::PlaceAt(XLocation* location, const int _x, const int _y)
{
    if (!XMapObject::PlaceAt(location, _x, _y)) {
        return false;
    }

    const MapObjectStats* row = FindMapObject(content_id);

    if (!row) {
        std::cerr << "world: nothing defines a map object '" << content_id << "'"
                  << std::endl;

        return false;
    }

    name = row->name;
    view = row->view;
    color = static_cast<xColor>(row->colour);

    // Spread the first turns out, so a field of these does not all stir
    // at the same moment.
    const int spread = row->first_delay_max - row->first_delay_min;
    ttm = row->first_delay_min + (spread > 0 ? vRand(spread + 1) : 0);

    Game.Scheduler.Add(this);

    return true;
}

bool XLuaObject::Run()
{
    assert(isValid());

    const MapObjectStats* row = FindMapObject(content_id);

    if (!row || row->on_run.empty()) {
        Invalidate();

        return false;
    }

    sol::state_view lua(XLua::State());
    sol::protected_function handler = lua[row->on_run];

    if (!handler.valid()) {
        std::cerr << "world: '" << content_id << "' takes its turn through '"
                  << row->on_run << "', which is not defined" << std::endl;
        Invalidate();

        return false;
    }

    const auto result = handler((void*)this);

    if (!result.valid()) {
        const sol::error err = result;
        std::cerr << "world: '" << row->on_run << "' failed: " << err.what() << std::endl;

        return false;
    }

    // Something the handler did during its turn may have finished it off
    // already - it may have been picked, or burned.
    if (!isValid()) {
        return false;
    }

    // Otherwise the handler says how long until its next turn. Saying
    // nothing means it is done with, and goes off the map.
    const sol::optional<int> next = result;

    if (!next || *next <= 0) {
        Invalidate();

        return false;
    }

    ttm += *next;

    return true;
}

REGISTER_CLASS(XGrave);
CEREAL_REGISTER_TYPE(XGrave);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XMapObject, XGrave);

XGrave::XGrave(const int _x, const int _y, char* subscr, XLocation* _l)
{
    SetLocation(_l);
    x = _x;
    y = _y;
    color = xLIGHTGRAY;
    view = '+';
    assert(l->map->GetSpecial(x, y) == nullptr);
    l->map->SetSpecial(x, y, this);
    name = fmt::format("the grave signed '{}'", subscr);
    isOpened = 0;
}

void XGrave::HideItem(XItem* item)
{
    hidden_items.insert(XItem::Own(item));
}

void XGrave::OnInvalidate()
{
    // Same idiom as XChest::Invalidate(): XItem::Invalidate() has no
    // idea this set exists (it only knows how to remove itself from
    // l->map's per-cell item_list), so there's no self-removal to race
    // against - just mark every item invalid and let hidden_items' own
    // destructor release the references afterward.
    for (auto& item : hidden_items) {
        item->Invalidate();
    }

    XMapObject::OnInvalidate();
}

bool XGrave::onOuterUse(XCreature* cr)
{
    if (cr->isHero()) {
        if (isOpened) {
            msgwin.Add("This grave has already been looted.");
        } else {
            msgwin.Add("You move the headstone. Something glimmers in the dark.");
        }
    }

    // Pre-existing bug, fixed in passing: erasing from hidden_items while
    // range-for iterating it invalidated the iterator on the very next
    // increment. Use the erase-returns-next-iterator form instead.
    for (auto it = hidden_items.begin(); it != hidden_items.end();) {
        (*it)->Drop(l, x, y);
        it = hidden_items.erase(it);
    }

    isOpened = 1;
    return true;
}

REGISTER_CLASS(XFurniture);
CEREAL_REGISTER_TYPE(XFurniture);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XMapObject, XFurniture);

XFurniture::XFurniture(const int _x, const int _y, const int _c, const char _v, const char* subscr, XLocation* _l)
{
    SetLocation(_l);
    x = _x;
    y = _y;
    color = _c;
    view = _v;
    assert(l->map->GetSpecial(x, y) == nullptr);
    l->map->SetSpecial(x, y, this);
    name = subscr;
}

REGISTER_CLASS(XOuterObject);
CEREAL_REGISTER_TYPE(XOuterObject);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XMapObject, XOuterObject);
XOuterObject::XOuterObject(const int _x, const int _y, const int _c, const char _v, const char* subscr, XLocation* _l, const char* event)
{
    SetLocation(_l);
    x = _x;
    y = _y;

    SetName(subscr);
    SetView(_v, _c);
    assert(l->map->GetSpecial(x, y) == nullptr);
    l->map->SetSpecial(x, y, this);

    if (event) {
        onEventLua = event;
    }
}

bool XOuterObject::onOuterUse(XCreature* cr)
{
    if (onEventLua.empty()) {
        return XMapObject::onOuterUse(cr);
    }

    sol::state_view lua(XLua::State());
    sol::protected_function_result result = lua[onEventLua](LuaEvent::OUTER_USE, (void*)cr, (void*)this);

    return XLua::ResultToBool(result, onEventLua);
}

