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

#include "creature/xhero.h"
#include "helpers/msgwin.h"
#include "item/item_cereal.h"
#include "item/xscroll.h"

REGISTER_CLASS(XScroll);
CEREAL_REGISTER_TYPE(XScroll);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XScroll);

struct ScrollDescription {
	// generate scroll name
    ScrollDescription(const char* rn, XEffect::Id eff, ScrollName scrn, int val, int rar)
    {
        real_name = rn;
        effect = eff;
        scroll_name = scrn;
        identify = 0;
        value = val;
        total_value += rar;
        rarity = rar;

        const int words = vRand() % 2 + 1;

        for (int i = 0; i < words; i++) {
            const int word_len = vRand() % (5 - words) + 3;

            for (int j = 0; j < word_len; j++) {
                // default letter
                char letter = 'X';

                if (j % 2 == 0) {
                    constexpr char vowels[] = "euioa";
                    letter = vowels[vRand() % (sizeof(vowels) - 1)];
                } else {
                    letter = static_cast<char>(vRand() % 26 + 'a');
                }

                name.push_back(letter);
            }

            if (i < words - 1) {
                if (vRand() % 4 == 1) {
                    name.append("-");
                } else {
                    name.append(" ");
                }
            }
        }
    };

    XEffect::Id effect;
    ScrollName scroll_name;
    int identify;
    std::string name;
    std::string_view real_name;
    int value;
    int rarity;

    static int total_value;
    static int GetRandomDescription(ScrollName scrn);

    // real_name/value/rarity are compile-time constants; effect/
    // scroll_name/identify/name are per-game-session mutable state (the
    // scroll<->effect scrambling, its randomly-generated flavor name,
    // and identification progress), same fields the legacy Store/
    // Restore already persisted.
    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(effect, scroll_name, identify, name);
    }
};

int ScrollDescription::total_value = 0;

ScrollDescription scroll_descr[] = {
    ScrollDescription("healing",         XEffect::HEAL,           ScrollName::HEALING,        200,  10),
    ScrollDescription("burning hands",   XEffect::BURNING_HANDS,  ScrollName::BURNING_HANDS,   20, 100),
    ScrollDescription("ice touch",       XEffect::ICE_TOUCH,      ScrollName::ICE_TOUCH,       20, 100),
    ScrollDescription("heroism",         XEffect::HEROISM,        ScrollName::HEROISM,         25, 100),
    ScrollDescription("power",           XEffect::POWER,          ScrollName::POWER,           15, 100),
    ScrollDescription("identify",        XEffect::IDENTIFY,       ScrollName::IDENTIFY,       100,  30),
    ScrollDescription("magic arrow",     XEffect::MAGIC_ARROW,    ScrollName::MAGIC_ARROW,     15, 200),
    ScrollDescription("fire bolt",       XEffect::FIRE_BOLT,      ScrollName::FIRE_BOLT,       50,  60),
    ScrollDescription("ice bolt",        XEffect::ICE_BOLT,       ScrollName::ICE_BOLT,        50,  60),
    ScrollDescription("lightning bolt",  XEffect::LIGHTNING_BOLT, ScrollName::LIGHTNING_BOLT, 100,  20),
    ScrollDescription("acid bolt",       XEffect::ACID_BOLT,      ScrollName::ACID_BOLT,      150,  15),
    ScrollDescription("summon monsters", XEffect::SUMMON_MONSTER, ScrollName::SUMMON_MONSTER,  10, 100),
    ScrollDescription("create item",     XEffect::CREATE_ITEM,    ScrollName::CREATE_ITEM,    200,  25),
    ScrollDescription("cure disease",    XEffect::CURE_DISEASE,   ScrollName::CURE_DISEASE,    40, 100),
    ScrollDescription("cure poison",     XEffect::CURE_POISON,    ScrollName::CURE_POISON,     40, 100),
    ScrollDescription("blink",           XEffect::BLINK,          ScrollName::BLINK,           70,  30),
    ScrollDescription("self knowledge",  XEffect::SELF_KNOWLEDGE, ScrollName::SELF_KNOWLEDGE, 150,  10),
    ScrollDescription("see invisible",   XEffect::SEE_INVISIBLE,  ScrollName::SEE_INVISIBLE,   40,  50),
    ScrollDescription("recipe",          XEffect::NONE,           ScrollName::RECIPE,          30,  25),
};

int ScrollDescription::GetRandomDescription(ScrollName scrn)
{
    if (scrn == ScrollName::RANDOM) {
        int val = vRand(total_value);
        int pos = -1;

        do {
            pos++;
            val -= scroll_descr[pos].rarity;
        } while (val >= 0);

        return pos;
    } else {
        for (int i = 0; i < static_cast<int>(ScrollName::RANDOM); i++)
            if (scroll_descr[i].scroll_name == scrn) {
                return i;
            }
    }

    return -1;
}

XScroll::XScroll(ScrollName scrn)
{
    descr = ScrollDescription::GetRandomDescription(scrn);
    assert(descr > -1 && descr < static_cast<int>(ScrollName::RANDOM));
    sc_name = scroll_descr[descr].scroll_name;
    name = scroll_descr[descr].real_name;
    value = scroll_descr[descr].value;
    kind = ItemKind::SCROLL;
    bp = BP_OTHER;
    it = ItemType::SCROLL;
    view = '?';
    color =	xLIGHTGRAY;
    weight = 2;
    dice.Setup("1d1");
}

int XScroll::isIdentified()
{
    return scroll_descr[descr].identify;
}

void XScroll::Identify(int level)
{
    scroll_descr[descr].identify = level;
}

int XScroll::Compare(XObject * o)
{
    assert(dynamic_cast<XScroll*>(o));
    XScroll * tit = (XScroll*)o;

    if (descr == tit->descr && x == tit->x && y == tit->y) {
        return 0;
    } else {
        if (quantity > tit->quantity) {
            return -1;
        } else {
            return 1;
        }
    }
}

std::string XScroll::toString()
{
    if (!isIdentified()) {
        if (quantity == 1) {
            return fmt::format("scroll labeled \"{}\"", scroll_descr[descr].name);
        }

        return fmt::format("heap of ({}) scrolls labeled \"{}\"",
            quantity, scroll_descr[descr].name);
    }

    if (quantity == 1) {
        return fmt::format("scroll of {}", name);
    }

    return fmt::format("heap of ({}) scrolls of {}",
        quantity, name);
}

int XScroll::onRead(XCreature * cr)
{
    assert(cr->isValid());

    int flag = 0;

    if (scroll_descr[descr].effect != XEffect::NONE) {
        if (cr->isHero()) {
            msgwin.Add(fmt::format("You read the {}.", toString()));
        } else if (cr->isVisible()) {
            msgwin.Add(fmt::format("{} reads the {}.", cr->name, toString()));
        }

        EFFECT_DATA ed;
        ed.caller	= cr;
        ed.l	= cr->l;
        ed.effect	= scroll_descr[descr].effect;
        ed.power	= 25;
        ed.call_x	= cr->x;
        ed.call_y	= cr->y;

        if (XEffect::GetReq(scroll_descr[descr].effect) == ER_DIRECTION) {
            XPoint pt;
            cr->GetTarget(TR_ATTACK_DIRECTION, &pt);
            ed.target_x = pt.x + cr->x;
            ed.target_y = pt.y + cr->y;
        } else if (XEffect::GetReq(scroll_descr[descr].effect) == ER_TARGET) {
            XPoint pt;
            cr->GetTarget(TR_ATTACK_TARGET, &pt, XEffect::GetRange(ed.effect, ed.power));
            ed.target_x = pt.x;
            ed.target_y = pt.y;
        }

        flag = XEffect::Make(&ed);
    } else {
        flag = 0;

        switch (scroll_descr[descr].scroll_name) {
            case ScrollName::RECIPE:
                if (cr->isHero()) {
                    int val = vRand(XAlchemy::GetRecipeCount());
                    XAlchemyRecipe * pRec = XAlchemy::GetRecipe(val);

                    if (pRec) {
                        flag = ((XHero*)cr)->LearnRecipe(pRec->pn1, pRec->pn2, pRec->result);
                    }
                }

                break;
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
            Identify(1);
            msgwin.Add(fmt::format("It was {}.", toString()));
        }
    }

    return flag;
}

void XScroll::SaveTable(cereal::JSONOutputArchive& ar)
{
    for (int i = 0; i < static_cast<int>(ScrollName::RANDOM); i++) {
        ar(scroll_descr[i]);
    }
}

void XScroll::LoadTable(cereal::JSONInputArchive& ar)
{
    for (int i = 0; i < static_cast<int>(ScrollName::RANDOM); i++) {
        ar(scroll_descr[i]);
    }
}
