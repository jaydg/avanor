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

#include <cmath>
#include <fmt/format.h>
#include <sol/sol.hpp>

#include "creature/creature.h"
#include "creature/deity.h"
#include "helpers/msgwin.h"
#include "item/item.h"
#include "map/map_objects.h"

void XDeity::RegisterLua(sol::state_view& lua)
{
    lua.new_enum("XDeity",
        "LIFE", XDeity::LIFE,
        "DEATH", XDeity::DEATH
    );
}

XCreature* XDeity::death = nullptr;
XCreature* XDeity::life = nullptr;

DEITY_HELP life_help[] = {
    {"cure light wounds",	3,	PRAY_CURE_LIGHT_WOUNDS},
    {"minor divine intervention",	5,	PRAY_MINOR_INTERVENTION},
    {"cure poison",	10, PRAY_CURE_POISON},
    {"heroism",	10, PRAY_HEROISM},
    {"cure critical wounds",	20, PRAY_CURE_CRITICAL_WOUNDS},
    {"great knowledge",	30, PRAY_IDENTIFY},
    {"divine restoration",	50, PRAY_RESTORATION}
};

DEITY_HELP death_help[] = {
    {"cure light wounds",	5,	PRAY_CURE_LIGHT_WOUNDS},
    {"minor divine intervention",	5,	PRAY_MINOR_INTERVENTION},
    {"divine intervention",	5,	PRAY_INTERVENTION},
    {"divine escape",	50, PRAY_TELEPORT},
    {"cure critical wounds",	30, PRAY_CURE_CRITICAL_WOUNDS},
    {"knowledge of insight",	50, PRAY_SELF_KNOWLEDGE},
    {"major divine intervention",	5,	PRAY_MAJOR_INTERVENTION},
};

struct DEITY_ACT {
    int good;
    int bad;
};

DEITY_ACT life_deity_act[] = {
    {5, -15}, {5, -14}, {5, -12}, {5, -10}, {10, -10},
    {10, -9}, {10, -8}, {10, -7}, {10, -6}, {10, -5},
    {10, -4}, {10, -3}, {10, -2}, {10, -1}, {10, 0}, {10, 0}
};

void XReligion::KillCreature(XCreature * killer, XCreature * victim)
{
    int val = killer->sk->GetLevel(XSkill::Skill::RELIGION);
    int bad = life_deity_act[val].bad;
    int good = life_deity_act[val].good;

    if (victim->creature_class == CR_UNDEAD) {
        death_act += bad;
        life_act += good;
    } else {
        death_act += good;
        life_act += bad;
    }
}

int XReligion::SacrificeItem(XCreature * cr, XItem * item, XDeity::Id deity)
{
    int val = cr->sk->GetLevel(XSkill::Skill::RELIGION);

    if (deity == XDeity::UNKNOWN) {
        if (life_act > death_act) {
            deity = XDeity::LIFE;
        } else {
            deity = XDeity::DEATH;
        }
    }

    XMapObject * tmo = cr->l->map->GetSpecial(cr->x, cr->y);
    bool at_altar = dynamic_cast<XAltar *>(tmo) != nullptr;

    if (at_altar) {
        if (tmo->color == xWHITE) {
            deity = XDeity::LIFE;
        } else {
            deity = XDeity::DEATH;
        }
    }

    if (cr->isVisible()) {
        msgwin.Add(fmt::format("{} prays to {}.", cr->name, GetDeityName(deity)));
        msgwin.Add(fmt::format("{} disappears in a bright light.", item->toString()));
    }

    int sacrifice_value;

    if (item->kind & IM_MONEY) {
        sacrifice_value = static_cast<int>(std::sqrt(static_cast<float>(item->quantity)) + 1)
            * (val / 4 + 1);
    } else {
        sacrifice_value = static_cast<int>((std::sqrt(static_cast<float>(item->GetValue() * item->quantity)) + 1))
            * (val / 4 + 1);
    }

    cr->sk->UseSkill(XSkill::Skill::RELIGION);

    if (at_altar) {
        sacrifice_value *= 3;
        cr->sk->UseSkill(XSkill::Skill::RELIGION, 5);
    }

    item->UnCarry();
    item->Invalidate();

    if (deity == XDeity::LIFE) {
        life_act += sacrifice_value;
    } else {
        death_act += sacrifice_value;
    }

    if (!cr->isHero() && vRand(5) == 0) {
        cr->stats->Modify(XStats::Random(), 1);

        if (cr->isVisible()) {
            msgwin.Add(fmt::format("{} looks more powerful!", cr->name));
        }
    }

    return 1;
}

DEITY_RELATION XReligion::GetRelation(const XDeity::Id deity) const
{
    int val = 0;

    switch (deity) {
        case XDeity::LIFE:
            val = life_act;
            break;

        case XDeity::DEATH:
            val = death_act;
            break;

        default:
            break;
    }

    if (val < -10000000) {
        return DR_FALLEN_CHAMPION;
    } else if (val < -100) {
        return DR_VERY_BAD;
    } else if (val < 0) {
        return DR_BAD;
    } else if (val < 100) {
        return DR_NORMAL;
    } else if (val < 1000) {
        return DR_ADEPT;
    } else if (val < 3000) {
        return DR_FOLLOWER;
    } else if (val < 10000) {
        return DR_MESSIAH;
    } else {
        return DR_CHAMPION;
    }
}

const char* relation_name[] = {
    MSG_RED "fallen champion", MSG_RED "very bad", MSG_RED "bad",
    MSG_LIGHTGRAY "normal", MSG_LIGHTGRAY "adept", MSG_LIGHTGREEN "follower",
    MSG_YELLOW "messiah", MSG_WHITE "champion"
};

const char* XReligion::GetRelationName(DEITY_RELATION dr)
{
    return relation_name[dr];
}

const char* XReligion::GetDeityName(XDeity::Id deity)
{
    if (deity == XDeity::LIFE) {
        return "Tiamat";
    } else {
        return "Marduk";
    }
}

int XReligion::GetAvailHelp(const XDeity::Id deity, DEITY_HELP** help) const
{
    if (deity == XDeity::LIFE) {
        *help = &life_help[0];

    } else {
        *help = &death_help[0];
    }

    const DEITY_RELATION rel = GetRelation(deity);

    if (rel < DR_ADEPT) {
        return 0;
    }

    if (rel < DR_FOLLOWER) {
        return 2;
    }

    if (rel < DR_MESSIAH) {
        return 4;
    }

    if (rel < DR_CHAMPION) {
        return 6;
    }

    return 7;
}

int XReligion::Pray(XDeity::Id deity, DEITY_HELP * pray, XCreature * prayer)
{
    XEffect::Id effect = XEffect::CURE_LIGHT_WOUNDS;

    switch (pray->pray) {
        case PRAY_CURE_LIGHT_WOUNDS:
            effect = XEffect::CURE_LIGHT_WOUNDS;
            break;

        case PRAY_CURE_CRITICAL_WOUNDS:
            effect = XEffect::CURE_CRITICAL_WOUNDS;
            break;

        case PRAY_RESTORATION:
            effect = XEffect::RESTORATION;
            break;

        case PRAY_IDENTIFY:
            effect = XEffect::IDENTIFY;
            break;

        case PRAY_SELF_KNOWLEDGE:
            effect = XEffect::SELF_KNOWLEDGE;
            break;

        case PRAY_CURE_POISON:
            effect = XEffect::CURE_POISON;
            break;

        case PRAY_HEROISM:
            effect = XEffect::HEROISM;
            break;

        case PRAY_TELEPORT:
            effect = XEffect::TELEPORT;
            break;

        case PRAY_MINOR_PUNISHMENT:
        case PRAY_MINOR_INTERVENTION:
            effect = XEffect::MAGIC_ARROW;
            break;

        case PRAY_INTERVENTION:
            effect = vRand(2) == 0 ? XEffect::FIRE_BOLT : XEffect::ICE_BOLT;
            break;

        case PRAY_MAJOR_INTERVENTION:
            effect = vRand(2) == 0 ? XEffect::LIGHTNING_BOLT : XEffect::ACID_BOLT;
            break;
    }

    RESULT res = XEffect::Make(prayer, effect, 50);

    if (res == SUCCESS) {
        if (deity == XDeity::LIFE) {
            life_act -= pray->help_cost;
        } else {
            death_act -= pray->help_cost;
        }

        prayer->sk->UseSkill(XSkill::Skill::RELIGION, 3);
    }

    if (res != ABORT) {
        msgwin.Add("Your prayer was unheard.");
    }

    return 1;
}

