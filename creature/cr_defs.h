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

#ifndef CR_DEFS_H
#define CR_DEFS_H

enum CREATURE_CLASS {
    CR_NONE     = 0x00000000,
    CR_RAT      = 0x00000001,
    CR_FELINE	= 0x00000002,
    CR_CANINE	= 0x00000004,
    CR_REPTILE	= 0x00000008,
    CR_INSECT	= 0x00000010,
    CR_HUMAN	= 0x00000020,
    CR_ORC      = 0x00000040,
    CR_GIANT	= 0x00000080,
    CR_KOBOLD	= 0x00000100,
    CR_UNDEAD	= 0x00000200,
    CR_GOBLIN	= 0x00000400,
    CR_DEMON	= 0x00000800,
    CR_HUMANOID	= 0x00001000,
    CR_BLOB     = 0x00002000, // warm mass, ooze
    CR_OTHER	= 0x00004000,
    CR_ALL      = CR_HUMAN | CR_INSECT | CR_KOBOLD | CR_UNDEAD | CR_GOBLIN | CR_REPTILE | CR_FELINE | CR_RAT | CR_CANINE | CR_HUMANOID,
    CR_ALL_IMPL = CR_RAT | CR_FELINE | CR_CANINE | CR_REPTILE | CR_KOBOLD | CR_INSECT | CR_GOBLIN | CR_UNDEAD,
};

enum CREATURE_NAME {
    CN_NONE       = 0,
    CN_RAT        = 1,
    CN_LARGE_RAT  = 2,

    CN_BAT        = 10,
    CN_HUGE_BAT   = 11,

    CN_DOG        = 30,

    CN_SKELETON   = 180,

    CN_UNIQUE     = 300,
    CN_BANDIT     = 302,
    CN_SHOPKEEPER = 303,
    CN_GEFEON     = 305,
    CN_RODERIK    = 307,
    CN_BEELZEVILE = 312,
    CN_HIGHPRIEST = 316,
    CN_ROTMOTH    = 317,
    CN_GIANA      = 318,

    CN_EOF        = 350
};

enum CREATURE_LEVEL	{
    CRL_VERY_LOW  = 0x0001,
    CRL_LOW       = 0x0002,
    CRL_ABOVE_LOW = 0x0004,
    CRL_AVG       = 0x0008,
    CRL_ABOVE_AVG = 0x0010,
    CRL_HI        = 0x0020,
    CRL_ABOVE_HI  = 0x0040,
    CRL_VERY_HI   = 0x0080,
    CRL_EXTREM_HI = 0x0100,
    CRL_UNIQUE    = 0x0200,
    CRL_ANY       = CRL_VERY_LOW | CRL_LOW | CRL_AVG | CRL_HI | CRL_VERY_HI,
    CRL_VL        = CRL_VERY_LOW | CRL_LOW,
    CRL_LA        = CRL_LOW | CRL_AVG,
    CRL_AH        = CRL_AVG | CRL_HI,
    CRL_HVH       = CRL_HI | CRL_VERY_HI
};

enum CREATURE_EXP {
    CRE_NONE = 0x0001,
    CRE_LOW  = 0x0002, // 1-8
    CRE_AVG  = 0x0004, // 8-16
    CRE_HI   = 0x0008, // 16 - 24
    CRE_ANY  = CRE_LOW | CRE_AVG | CRE_HI
};

enum CARRY_STATE {
    CSTATE_NORMAL     = 0x001, // the normal carrying
    CSTATE_BURDENED   = 0x002,
    CSTATE_STRAINED   = 0x003,
    CSTATE_OVERBURDEN = 0x004,
    CSTATE_DIE        = 0x005,
};

enum GROUP_ID {
    GID_NONE,
    GID_ORCS_WAR_PARTY,
    GID_FOREST_BROTHER,
    GID_GUARDIAN,
    GID_SMALL_VILLAGE_FARMER,
    GID_TOWNEE_1,
    GID_DWARVEN_GUARDIAN,
    GID_AHKULAN_GUARDIAN,
    GID_RODERICK_GUARDIAN,
    GID_RANDOM_GUARDIAN,
};

enum CREATURE_SIZE {
    CS_VERY_SMALL = 1, // insects like, rats, bats
    CS_SMALL      = 2, // kobold, hobbit like
    CS_NORMAL     = 3, // human like
    CS_LARGE      = 4, // TROLL
    CS_VERY_LARGE = 5, // Titan, dragon
};

enum FOOD_FEELING {
    FF_TOLERANT,
    FF_NORMAL,
    FF_SENSITIVE,
};

enum CR_GENDER {
    GEN_NEUTER,
    GEN_MALE,
    GEN_FEMALE,

    // TODO: Write code to pick a random gender if male & female flags on.
    GEN_RANDOM = (GEN_FEMALE | GEN_MALE)
};

enum CR_PERSON_TYPE {
    CPT_IT = GEN_NEUTER,  // It
    CPT_HE = GEN_MALE,    // He
    CPT_SHE = GEN_FEMALE, // She

    // Default you
    CPT_YOU = 0x08,

    // Genderized you
    CPT_MALE_YOU = (CPT_HE | CPT_YOU),
    CPT_FEMALE_YOU = (CPT_SHE | CPT_YOU),

    // Unique creatures
    CPT_UNIQUE = 0x10,

    // Backward compatibility
    CPT_NAMED_HE = (CPT_HE | CPT_UNIQUE),   // Munch-Munch the Dread
    CPT_NAMED_SHE = (CPT_SHE | CPT_UNIQUE), // Yohjishiro, the elven wizard
    CPT_NAMED_IT = (CPT_IT | CPT_UNIQUE)    // Gekta, the sheep dog
};

enum CR_ATTACK_TYPE {
    CRAT_MELEE_ONLY,
    CRAT_BOTH
};

#endif
