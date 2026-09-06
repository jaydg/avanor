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

#include <iostream>
#include <map>
#include <vector>

#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>

#include "creature/xhero.h"
#include "helpers/msgwin.h"
#include "item/item_cereal.h"
#include "item/xpotion.h"
#include <sol/sol.hpp>

#include "engine/xlua.h"
#include "item/xscroll.h"

REGISTER_CLASS(XScroll);
CEREAL_REGISTER_TYPE(XScroll);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XScroll);

// One sort of scroll: what it is called once known, what reading it does,
// what it is worth, and how often one turns up. Filled from
// world/items/scrolls.lua as that script loads.
struct ScrollDescription {
    ScrollName id;
    std::string real_name;
    EFFECT effect{EFFECT_NONE};
    int value{0};
    int rarity{0};
    bool read_in_combat{false};

    // A scroll whose reading is not an effect says here which Lua
    // function carries it out. The engine names no scroll of its own.
    std::string read_handler;

    // What this game calls it before anyone works out what it is, and
    // whether they have. The only two fields that are not content: they
    // belong to the game in progress and are saved with it.
    std::string label;
    bool identified{false};

    // A pronounceable nonsense name, alternating vowels and consonants.
    // Rolled here rather than in a static initialiser, which is where it
    // used to happen - before main(), before any seed, so every game got
    // the same labels and a player who learned one knew it forever.
    static std::string RollLabel()
    {
        std::string out;
        const int words = vRand() % 2 + 1;

        for (int i = 0; i < words; i++) {
            const int word_len = vRand() % (5 - words) + 3;

            for (int j = 0; j < word_len; j++) {
                if (j % 2 == 0) {
                    constexpr char vowels[] = "euioa";
                    out.push_back(vowels[vRand() % (sizeof(vowels) - 1)]);
                } else {
                    out.push_back(static_cast<char>(vRand() % 26 + 'a'));
                }
            }

            if (i < words - 1) {
                out.append(vRand() % 4 == 1 ? "-" : " ");
            }
        }

        return out;
    }
};

std::vector<ScrollDescription> scroll_descr;
static int scroll_total_rarity = 0;

// The row for an id, or nullptr if content never registered one.
static ScrollDescription* FindScroll(const ScrollName& id)
{
    for (auto& row : scroll_descr) {
        if (row.id == id) {
            return &row;
        }
    }

    return nullptr;
}

ScrollBuilder::ScrollBuilder(std::string id) : id(std::move(id)) {}

ScrollBuilder& ScrollBuilder::Called(const std::string& name)
{
    real_name = name;
    return *this;
}

ScrollBuilder& ScrollBuilder::OnRead(const std::string& handler)
{
    read_handler = handler;
    return *this;
}

ScrollBuilder& ScrollBuilder::Effect(const EFFECT& eff)
{
    effect = eff;
    return *this;
}

ScrollBuilder& ScrollBuilder::Worth(const int val)
{
    value = val;
    return *this;
}

ScrollBuilder& ScrollBuilder::Chance(const int rar)
{
    rarity = rar;
    return *this;
}

ScrollBuilder& ScrollBuilder::ReadInCombat()
{
    read_in_combat = true;
    return *this;
}

void ScrollBuilder::Register()
{
    if (id.empty()) {
        std::cerr << "world: a scroll with no id" << std::endl;
        return;
    }

    if (FindScroll(id)) {
        std::cerr << "world: two scrolls both called '" << id << "'" << std::endl;
        return;
    }

    ScrollDescription row;
    row.id = id;
    row.real_name = real_name.empty() ? id : real_name;
    row.effect = effect;
    row.value = value;
    row.rarity = rarity;
    row.read_in_combat = read_in_combat;
    row.read_handler = read_handler;

    if (!effect.empty() && !FindEffect(effect)) {
        std::cerr << "world: the scroll '" << id << "' does '" << effect
                  << "', which world/effects.lua does not declare" << std::endl;
    }

    row.label = ScrollDescription::RollLabel();

    scroll_total_rarity += rarity;
    scroll_descr.push_back(std::move(row));
}

// The row a request names, or one drawn by rarity when it names none.
static ScrollDescription* PickScroll(const ScrollName& scrn)
{
    if (scroll_descr.empty()) {
        return nullptr;
    }

    if (!scrn.empty()) {
        ScrollDescription* row = FindScroll(scrn);

        if (!row) {
            std::cerr << "world: nothing defines a scroll '" << scrn
                      << "' - substituting another" << std::endl;
        } else {
            return row;
        }
    }

    int val = vRand(scroll_total_rarity);

    for (auto& row : scroll_descr) {
        val -= row.rarity;

        if (val < 0) {
            return &row;
        }
    }

    return &scroll_descr.front();
}

XScroll::XScroll(const ScrollName& scrn)
{
    const ScrollDescription* row = PickScroll(scrn);

    if (row) {
        sc_name = row->id;
        name = row->real_name;
        value = row->value;
    }

    kind = ItemKind::SCROLL;
    bp = BP_OTHER;
    it = IT_SCROLL;
    view = '?';
    color = xLIGHTGRAY;
    weight = 2;
    dice.Setup("1d1");
}

bool XScroll::isReadInCombat() const
{
    const ScrollDescription* row = FindScroll(sc_name);
    return row && row->read_in_combat;
}

bool XScroll::isIdentified()
{
    const ScrollDescription* row = FindScroll(sc_name);
    return row && row->identified;
}

void XScroll::Identify()
{
    if (ScrollDescription* row = FindScroll(sc_name)) {
        row->identified = true;
    }
}

bool XScroll::Compare(XObject* o)
{
    assert(dynamic_cast<XScroll*>(o));
    XScroll * tit = (XScroll*)o;

    return sc_name == tit->sc_name && x == tit->x && y == tit->y;
}

std::string XScroll::toString()
{
    const ScrollDescription* row = FindScroll(sc_name);
    const std::string label = row ? row->label : sc_name;

    if (!isIdentified()) {
        if (quantity == 1) {
            return fmt::format("scroll labeled \"{}\"", label);
        }

        return fmt::format("heap of ({}) scrolls labeled \"{}\"",
            quantity, label);
    }

    if (quantity == 1) {
        return fmt::format("scroll of {}", name);
    }

    return fmt::format("heap of ({}) scrolls of {}", quantity, name);
}

int XScroll::onRead(XCreature * cr)
{
    assert(cr->isValid());

    const ScrollDescription* row = FindScroll(sc_name);

    if (!row) {
        return 0;
    }

    int flag = 0;

    if (row->effect != EFFECT_NONE) {
        if (cr->isHero()) {
            msgwin.Add(fmt::format("You read the {}.", toString()));
        } else if (cr->isVisible()) {
            msgwin.Add(fmt::format("{} reads the {}.", cr->name, toString()));
        }

        EFFECT_DATA ed;
        ed.caller	= cr;
        ed.l	= cr->l;
        ed.effect	= row->effect;
        ed.power	= 25;
        ed.call_x	= cr->x;
        ed.call_y	= cr->y;

        if (XEffect::GetReq(row->effect) == EffectTarget::DIRECTION) {
            XPoint pt;
            cr->GetTarget(TR_ATTACK_DIRECTION, &pt);
            ed.target_x = pt.x + cr->x;
            ed.target_y = pt.y + cr->y;
        } else if (XEffect::GetReq(row->effect) == EffectTarget::TARGET) {
            XPoint pt;
            cr->GetTarget(TR_ATTACK_TARGET, &pt, XEffect::GetRange(ed.effect, ed.power));
            ed.target_x = pt.x;
            ed.target_y = pt.y;
        }

        flag = XEffect::Make(&ed);
    } else if (!row->read_handler.empty()) {
        // A scroll that is not an effect: content says what reading it
        // does. This is how the recipe scroll works, and it is the reason
        // the engine no longer names any scroll at all.
        sol::state_view lua(XLua::State());
        sol::protected_function handler = lua[row->read_handler];

        if (!handler.valid()) {
            std::cerr << "world: the scroll '" << row->id << "' is read through '"
                      << row->read_handler << "', which is not defined" << std::endl;
        } else {
            const auto result = handler((void*)cr);

            if (!result.valid()) {
                const sol::error err = result;
                std::cerr << "world: reading '" << row->id << "' failed: "
                          << err.what() << std::endl;
            } else {
                flag = result.get<int>();
            }
        }
    }

    if (!cr->isValid()) {
        return flag; // $$$
    }

    if (flag == 0) {
        if (cr->isHero()) {
            msgwin.Add("You feel nothing special.");
        } else if (cr->isVisible()) {
            msgwin.Add(cr->name);
            msgwin.Add("feels nothing special.");
        }
    } else {
        if (!isIdentified() && cr->isHero()) {
            Identify();
            msgwin.Add(fmt::format("It was {}.", toString()));
        }
    }

    return flag;
}

// What one game came to know about one sort of scroll. Everything else in
// a row is content and comes back from world/items/scrolls.lua next load.
struct ScrollMemory {
    bool identified{false};
    std::string label;

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(identified, label);
    }
};

// Keyed by id rather than written in table order, so content may add rows
// or reorder them without spoiling a save.
void XScroll::SaveTable(cereal::JSONOutputArchive& ar)
{
    std::map<std::string, ScrollMemory> learned;

    for (const auto& row : scroll_descr) {
        learned[row.id] = ScrollMemory{row.identified, row.label};
    }

    ar(learned);
}

void XScroll::LoadTable(cereal::JSONInputArchive& ar)
{
    std::map<std::string, ScrollMemory> learned;
    ar(learned);

    for (auto& row : scroll_descr) {
        if (const auto it = learned.find(row.id); it != learned.end()) {
            row.identified = it->second.identified;
            row.label = it->second.label;
        }
    }
}
