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

#ifndef ANY_CR_H
#define ANY_CR_H

#include <string>
#include <unordered_map>
#include <vector>

#include <cereal/types/base_class.hpp>

#include "creature/cr_defs.h"
#include "creature/creature.h"
#include "magic/resist.h"

struct CREATURE_SET_REC {
    std::vector<CREATURE_NAME> cn;
};


struct SKILL_REC {
    XSkill::Skill skt;
    int level;
};


struct EQUIP_REC {
    unsigned int mask;
    int count;
    int probability; // 0-100
    ITEM_TYPE it;
};

enum GENERATION_FLAGS {
    GFS_SUPRESS_INVIS = 0x0001,
    GFS_SEE_INVIS = 0x0002,
};


struct _CREATURE {
    //view
    std::string name;               // "kobold"
    char view;                      // 'k'
    int color;                      // xGREEN

    // main params
    XDice speed;                    // "2d5+50"
    XDice move_energy;              // 1000
    XDice attack_energy;            // 1000
    CREATURE_SIZE creature_size;    // CS_SMALL (bonus for DV)
    XDice creature_weight;          // how much corpse weight for example

    std::string body;
    XStatsGenerator stats_gen;
    XResistGenerator r_gen;         // resistances
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
    CREATURE_CLASS cr_class;        // class of creature
    std::vector<SKILL_REC> skills;
    std::vector<SPELL_NAME> spells;
    std::vector<EQUIP_REC> equipment;
    std::vector<MELEE_ATTACK> melee_attack;
    int equip_probability;
    unsigned int generation_flags;
    bool unique = false;
};

class XCreatureStorage
{
        static CREATURE_NAME last_name;

        // The 8 unique NPCs (Bandit, Shopkeeper, Gefeon, Roderick,
        // Beelzevile, HighPriest, Rotmoth, Giana) each have a hand-written
        // C++ subclass with real custom behavior (creature/unique.h/.cpp)
        // - this is purely an implementation-dispatch table for those,
        // replacing the old switch(cn) over a small numeric range
        // (cn >= CN_UNIQUE). Every other monster, no matter what its
        // XCreature::unique flag says, gets the generic XAnyCreature.
        static const std::unordered_map<CREATURE_NAME, XCreature*(*)(_CREATURE*)> unique_creators;

    public:
        static std::unordered_map<CREATURE_NAME, _CREATURE> creature_storage;
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

class MonsterBuilder
{
    public:
        explicit MonsterBuilder(CREATURE_NAME id, CREATURE_NAME base = CN_NONE);

        MonsterBuilder& View(const std::string& name, char view, int color, CR_PERSON_TYPE person, CREATURE_LEVEL crl, CREATURE_CLASS cr_class);
        MonsterBuilder& Basic(const std::string& speed, const std::string& move_energy, const std::string& attack_energy, CREATURE_SIZE size, const std::string& weight);
        MonsterBuilder& Body(const std::string& body, int prob, unsigned int gen_flags);
        MonsterBuilder& AI(unsigned int flags);
        MonsterBuilder& Stats(const std::string& stats);
        MonsterBuilder& Resist(const std::string& resists);
        MonsterBuilder& Combat(const std::string& hit, const std::string& dice);
        MonsterBuilder& Main(const std::string& dv, const std::string& pv, const std::string& hp, const std::string& pp);
        MonsterBuilder& Description(const std::string& descr);
        MonsterBuilder& Melee(BRAND_TYPE br, int prob);
        MonsterBuilder& MeleeExtra(EXTENDED_ATTACK ea, int prob);
        MonsterBuilder& LearnSkill(XSkill::Skill skt, int lvl);
        MonsterBuilder& LearnSpell(SPELL_NAME spn);
        MonsterBuilder& Equip(unsigned int mask, ITEM_TYPE it, int prob);
        MonsterBuilder& EquipCount(unsigned int mask, int count, int prob);
        MonsterBuilder& Corpse(int rotting_time, FOOD_TYPE ft);
        MonsterBuilder& CorpseEffect(CORPSE_EFFECT_TYPE cet, int val);
        MonsterBuilder& Unique();

        void Register();

    private:
        CREATURE_NAME id;

        // {}, not left default-initialized: _CREATURE's plain int/enum
        // members (ai_flags, cr_class, ...) have no default constructor
        // of their own to zero them, unlike its std::string/XDice/vector
        // members - value-initializing here matches what
        // creature_storage[cn] (an unordered_map) already does for a
        // fresh entry.
        _CREATURE cr{};
};

class XAnyCreature : public XCreature
{
    protected:
        XAnyCreature() {}
        friend class cereal::access;

    public:
        DECLARE_CREATOR(XAnyCreature, XCreature);
        explicit XAnyCreature(_CREATURE * cr);
        void Die(XCreature * killer) override;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XCreature>(this));
        }
};

#endif
