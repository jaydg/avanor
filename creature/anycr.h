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
#include <sol/forward.hpp>

#include "item/xcorpse.h"
#include "creature/cr_defs.h"
#include "creature/creature.h"
#include "magic/brand.h"
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
    ItemType it;
};

struct CreatureTemplate {
    enum class Level : unsigned int {
        VERY_LOW  = 0x0001,
        LOW       = 0x0002,
        ABOVE_LOW = 0x0004,
        AVG       = 0x0008,
        ABOVE_AVG = 0x0010,
        HI        = 0x0020,
        ABOVE_HI  = 0x0040,
        VERY_HI   = 0x0080,
        EXTREM_HI = 0x0100,
        UNIQUE    = 0x0200,
        ANY       = VERY_LOW | LOW | AVG | HI | VERY_HI,
        VL        = VERY_LOW | LOW,
        LA        = LOW | AVG,
        AH        = AVG | HI,
        HVH       = HI | VERY_HI
    };

    // Registers this enum as the Lua table CreatureTemplate.MEMBER
    static void RegisterLua(sol::state_view& lua);

    //view
    std::string name;               // "kobold"
    char view;                      // 'k'
    int color;                      // xGREEN

    // main params
    XDice speed;                    // "2d5+50"
    XDice move_energy;              // 1000
    XDice attack_energy;            // 1000
    XCreature::Size creature_size;  // small creatures were meant to gain DV
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
    Level crl;
    XCreature::PersonType person;
    std::string creature_description;
    XCorpse::Data pCorpseData;
    unsigned int ai_flags;
    CREATURE_CLASS cr_class;       // class of creature
    std::vector<SKILL_REC> skills;
    std::vector<SPELL_NAME> spells;
    std::vector<EQUIP_REC> equipment;
    std::vector<MELEE_ATTACK> melee_attack;
    int equip_probability;

    // What one of these may not be found wearing, and what it must be
    // found wearing, said as resistances: :Never("invisible") strips
    // anything granting it off the outfit, :Always("see_invisible") makes
    // sure something grants it. Both were a pair of hard-wired flags
    // named after those two resistances; content names any it likes now,
    // and the engine names none.
    std::vector<RESISTANCE> never;
    std::vector<RESISTANCE> always;
    bool unique = false;
};

class XCreatureStorage
{
        // XShopkeeper is the one unique NPC left with a hand-written C++
        // subclass with real custom behavior (creature/shopkeeper.h/.cpp) -
        // it's staying C++ and growing (debt tracking, buying/selling,
        // XShop), unlike every other former unique.h class (Bandit,
        // Gefeon, Roderick, Beelzevile, HighPriest, Rotmoth, Giana), whose
        // dialogue/AI/item-giving all moved to world/*.lua. This map is
        // purely an implementation-dispatch table for that one holdout,
        // replacing the old switch(cn) over a small numeric range
        // (cn >= CN_UNIQUE). Every other monster, no matter what its
        // XCreature::unique flag says, gets the generic XAnyCreature.
        static const std::unordered_map<CREATURE_NAME, XCreature*(*)(CreatureTemplate*)> unique_creators;

    public:
        static std::unordered_map<CREATURE_NAME, CreatureTemplate> creature_storage;
        // Which monsters belong to each class, so one can be drawn at
        // random from a class without walking the whole storage.
        static std::unordered_map<CREATURE_CLASS, CREATURE_SET_REC> creature_set;

        static void CreateQuickBase();

        static CreatureTemplate* GetCreatureData(CREATURE_NAME cn);
        static XCreature* Create(CREATURE_NAME cn);
        static XCreature* CreateRnd(const CreatureClassSet& cc, CreatureTemplate::Level lvl = CreatureTemplate::Level::ANY);
        static void RestoreCreatureInfo(XCreature * cr);
};

class MonsterBuilder
{
    public:
        explicit MonsterBuilder(CREATURE_NAME id, CREATURE_NAME base = CN_NONE);

        MonsterBuilder& View(const std::string& name, char view, int color, XCreature::PersonType person, CreatureTemplate::Level crl, const CREATURE_CLASS& cr_class);
        MonsterBuilder& Basic(const std::string& speed, const std::string& move_energy, const std::string& attack_energy, XCreature::Size size, const std::string& weight);
        MonsterBuilder& Body(const std::string& body, int prob);
        MonsterBuilder& Never(const std::string& resist);
        MonsterBuilder& Always(const std::string& resist);
        MonsterBuilder& AI(unsigned int flags);
        MonsterBuilder& Stats(const std::string& stats);
        MonsterBuilder& Resist(const std::string& resists);
        MonsterBuilder& Combat(const std::string& hit, const std::string& dice);
        MonsterBuilder& Main(const std::string& dv, const std::string& pv, const std::string& hp, const std::string& pp);
        MonsterBuilder& Description(const std::string& descr);
        MonsterBuilder& Melee(const std::string& br, int prob);
        MonsterBuilder& MeleeExtra(EXTENDED_ATTACK ea, int prob);
        MonsterBuilder& LearnSkill(XSkill::Skill skt, int lvl);
        MonsterBuilder& LearnSpell(SPELL_NAME spn);
        MonsterBuilder& Equip(unsigned int mask, ItemType it, int prob);
        MonsterBuilder& EquipCount(unsigned int mask, int count, int prob);
        MonsterBuilder& Corpse(int rotting_time);
        MonsterBuilder& CorpseTaste(const std::string& taste);
        MonsterBuilder& CorpseEffect(XCorpse::EffectType cet, int val);
        MonsterBuilder& CorpseStat(const std::string& stat, int val);
        MonsterBuilder& CorpseResist(const std::string& resist, int val);
        MonsterBuilder& CorpseModifier(const std::string& modifier, int val);
        MonsterBuilder& Unique();

        void Register();

    private:
        CREATURE_NAME id;

        // {}, not left default-initialized: CreatureTemplate's plain int/enum
        // members (ai_flags, cr_class, ...) have no default constructor
        // of their own to zero them, unlike its std::string/XDice/vector
        // members - value-initializing here matches what
        // creature_storage[cn] (an unordered_map) already does for a
        // fresh entry.
        CreatureTemplate cr{};
};

class XAnyCreature : public XCreature
{
    protected:
        XAnyCreature() {}
        friend class cereal::access;

    public:
        DECLARE_CREATOR(XAnyCreature, XCreature);
        explicit XAnyCreature(CreatureTemplate * cr);
        void Die(XCreature * killer) override;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XCreature>(this));
        }
};

#endif
