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

#ifndef __ANY_CR_H
#define __ANY_CR_H

#include <string>

#include "creature/cr_defs.h"
#include "creature/creature.h"
#include "magic/resist.h"

struct CREATURE_SET_REC {
    CREATURE_SET_REC()
    {
        count = 0;
    }

    CREATURE_NAME cn[256]{}; //up to 256 creature of one type
    int count;
};


struct SKILL_REC {
    XSkill::Skill skt;
    int level;
};


struct EQUIP_REC {
    unsigned int mask;
    int count;
    int probability; //0-100
    ITEM_TYPE it;
};

enum GENERATION_FLAGS {
    GFS_SUPRESS_INVIS = 0x0001,
    GFS_SEE_INVIS = 0x0002,
};


struct _CREATURE {
    //view
    std::string name; //"kobold"
    char view; //'k'
    int color; //xGREEN

    //main params
    XDice speed; //"2d5+50"
    XDice move_energy; //1000
    XDice attack_energy; //1000
    CREATURE_SIZE creature_size; //CS_SMALL (bonus for DV)
    XDice creature_weight; //how much corpse weight for example

    std::string body;
    XStatsGenerator stats_gen;
    XResistGenerator r_gen; //resistances
    XDice dv;
    XDice pv;
    XDice hit;
    XDice dice;
    XDice hp;
    XDice pp;
    CREATURE_LEVEL crl;
    CR_PERSON_TYPE person;
    std::string creature_description;
    CORPSE_DATA pCorpseData;
    unsigned int ai_flags;
    CREATURE_CLASS cr_class; //class of creature
    XQList<SKILL_REC> skills;
    XQList<SPELL_NAME> spells;
    XQList<EQUIP_REC> equipment;
    XQList<MELEE_ATTACK> melee_attack;
    int equip_probability;
    unsigned int generation_flags;
};

#define XVW XCreatureStorage::View
#define XBA XCreatureStorage::Basic
#define XBO XCreatureStorage::Body
#define XA XCreatureStorage::SetAI
#define XS XCreatureStorage::S
#define XR XCreatureStorage::R
#define XM XCreatureStorage::Main
#define XD XCreatureStorage::D
#define XC XCreatureStorage::Combat
#define XAT XCreatureStorage::Melee
#define XL XCreatureStorage::Learn
#define EQ XCreatureStorage::Equip
#define COE XCreatureStorage::CorpseEffects
#define COD XCreatureStorage::Corpse

class XCreatureStorage
{
        static CREATURE_NAME last_name;
    public:
        static _CREATURE creature_storage[CN_EOF];
        static CREATURE_SET_REC creature_set[32];

        static void View(CREATURE_NAME cn, const char* name, char view, int color, CR_PERSON_TYPE person, CREATURE_LEVEL crl, CREATURE_CLASS cr_class, CREATURE_NAME cn_instance = CN_NONE);
        static void Basic(const char* speed, const char* base_energy, const char* combat_energy, CREATURE_SIZE csize, const char* weight);
        static void Body(const char* body, int prob = 0, unsigned int gen_flags = 0);
        static void SetAI(unsigned int aif);
        static void S(const char* stats);
        static void R(const char* resists);
        static void Combat(const char* hit, const char* dice);
        static void Melee(BRAND_TYPE br, int prob);
        static void Melee(EXTENDED_ATTACK ea, int prob);
        static void Main(const char* dv, const char* pv, const char* hp, const char* pp);
        static void D(const char* descr);
        static void Learn(XSkill::Skill skt, int lvl);
        static void Learn(SPELL_NAME spn);
        static void Equip(unsigned int mask, int count, int prob);
        static void Equip(unsigned int mask, ITEM_TYPE it, int prob);
        static void Corpse(int rotting_time, FOOD_TYPE ft);
        static void CorpseEffects(CORPSE_EFFECT_TYPE cet, int val);

        static void CreateQuickBase();

        static _CREATURE* GetCreatureData(CREATURE_NAME cn);
        static XCreature* Create(CREATURE_NAME cn);
        static XCreature* CreateRnd(CREATURE_CLASS cc, int lvl = CRL_ANY);
        static void RestoreCreatureInfo(XCreature * cr);
};

class XAnyCreature : public XCreature
{
    protected:
        XAnyCreature() {}

    public:
        DECLARE_CREATOR(XAnyCreature, XCreature);
        explicit XAnyCreature(_CREATURE * cr);
        void Die(XCreature * killer) override;
};

#endif
