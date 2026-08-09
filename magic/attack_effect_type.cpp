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

#include "magic/attack_effect_type.h"

void RegisterAttackEffectTypeLua(sol::state_view& lua)
{
    lua.new_enum("AttackEffectType",
        "NONE", AttackEffectType::NONE,
        "FIRE", AttackEffectType::FIRE,
        "HELLFIRE", AttackEffectType::HELLFIRE,
        "COLD", AttackEffectType::COLD,
        "ULTIMATECOLD", AttackEffectType::ULTIMATECOLD,
        "LIGHTNING", AttackEffectType::LIGHTNING,
        "EARTH", AttackEffectType::EARTH,
        "ELEMENTAL_MASK", AttackEffectType::ELEMENTAL_MASK,
        "ACID", AttackEffectType::ACID,
        "POISON", AttackEffectType::POISON,
        "DEATH", AttackEffectType::DEATH,
        "DISEASE", AttackEffectType::DISEASE,
        "PARALYSE", AttackEffectType::PARALYSE,
        "STUN", AttackEffectType::STUN,
        "CONFUSE", AttackEffectType::CONFUSE,
        "DRAIN_LIFE", AttackEffectType::DRAIN_LIFE,
        "BLACK_MASK", AttackEffectType::BLACK_MASK,
        "UNDEADSLAYER", AttackEffectType::UNDEADSLAYER,
        "HUMANOIDSLAYER", AttackEffectType::HUMANOIDSLAYER,
        "ANIMALSLAYER", AttackEffectType::ANIMALSLAYER,
        "DRAGONSLAYER", AttackEffectType::DRAGONSLAYER,
        "GIANTSLAYER", AttackEffectType::GIANTSLAYER,
        "ORCSLAYER", AttackEffectType::ORCSLAYER,
        "TROLLSLAYER", AttackEffectType::TROLLSLAYER,
        "DEMONSLAYER", AttackEffectType::DEMONSLAYER,
        "SLAYER_MASK", AttackEffectType::SLAYER_MASK,
        "HOLYSLAYER", AttackEffectType::HOLYSLAYER,
        "EVILSLAYER", AttackEffectType::EVILSLAYER,
        "ANY", AttackEffectType::ANY
    );
}
