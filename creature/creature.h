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

#ifndef CREATURE_H
#define CREATURE_H

#include <cstring>
#include <memory>
#include <vector>

#include <cereal/specialize.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include "creature/bodypart.h"
#include "creature/cr_defs.h"
#include "creature/deity.h"
#include <cereal/archives/json.hpp>

#include "creature/std_ai.h"
#include "item/incl_i.h"
#include "item/xanyfood.h"
#include "item/xtool.h"
#include "magic/magic.h"
#include "magic/skills.h"
#include "magic/wskills.h"

enum RBP_FLAG {
    RBP_UNKNOWN,
    RBP_BLOCK
};

enum AI_TYPE {
    AI_HERO,
    AI_SIMPLE
};

enum ACTION {
    A_UNKNOWN = 0,
    A_MOVE = 1,
    //	A_MOVETOSTAIRWAY = 2,
    A_ATTACK = 3,
    A_CAST = 4,
    A_EAT = 5,
    A_READ = 6,
    A_USE_TOOL = 7,
};

enum TACTICS_STATE {
    TS_COWARD,
    TS_DEFENSIVE,
    TS_NORMAL,
    TS_AGGRESSIVE,
    TS_BERSERKER,
};

enum CR_NAME_TYPE {
    CRN_T1, // you, the kobold, someone
    CRN_T2, // you, he/she, it
    CRN_T3, // your, him/her, it
    CRN_T4, // yours, his/hers, its
};


/////// attack structures ///////////
// 1) Creatures can make one attack with several consequences:
//	a) just a simple damage
//	b) stun, paralyze, poison
//	c) fire, cold or other damage (same with 'b')
// 2) Creature extend it damage by some features
//	a) spawn when attack was successful
//	b) 'eat' armour or weapon

enum EXTENDED_ATTACK {
    EA_NONE,
    EA_SPAWN,
};

struct MELEE_ATTACK {
    EXTENDED_ATTACK e_attack;
    BRAND_TYPE br_attack;
    int prob; // 0..100
};

class XStandardAI;
class XModifier;
class XPotion;
class XBook;
class XScroll;

struct ACTION_DATA {
    ACTION_DATA() : item(nullptr), action(A_MOVE) {}

    ACTION action;
    // Owning, not weak: an in-progress action's target item is typically
    // already erased from contain by the time it lands here (e.g. a
    // multi-turn book read), so this can be the item's only reference.
    // See XItem::Own().
    std::shared_ptr<XItem> item;
    void Store(XFile * f);
    void Restore(XFile * f);

    // `item` was never actually persisted even before Cereal (see the
    // FIXME still in Store/Restore) - new real persistence.
    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(action, item);
    }
};

typedef int (XItemFilter)(XItem*);

enum TARGET_REASON {
    TR_NONE,
    TR_ATTACK_TARGET,
    TR_ATTACK_POSITION,
    TR_ATTACK_DIRECTION,
    TR_IMPROVE,
    TR_YES_NO,      // by default - no
    TR_NO_YES,      // by default - yes
    TR_HOW_MUCH,    // enter a number between two numbers
    TR_LETTER,      // enter a letter in range
    TR_STEAL_ITEM,
    TR_SELECT_ITEM,
};

enum MISSILE_FL_TYPE {
    MFT_ARROW,
    MFT_BALL,
};

enum MF_RESULT {
    MF_HIT,
    MF_AVOID,
    MF_BLOCK,
};

// struct for missile flight
struct MF_DATA {
    int sx; // start position;
    int sy;
    int ex; // target position
    int ey;
    MISSILE_FL_TYPE arrow_type;
    int arrow_color;
    int max_range;      // if creature avoid missile - it flight away.
    int missile_speed;  // greater speed, harder to avoid.
    int to_hit;         // hit bonus of missile;
    XPoint pt;          // position where arrow stopped
    XLocation* l;       // where it is...
};

// struct for inflict damage to creature
struct DAMAGE_DATA {
    XCreature* target;
    XCreature* attacker;
    const char* attacker_name; // if this name is specified, then write it instead attacker name
    int damage;
};


//*******************************************************************************//
// We have to create one unificated function to damage any creature

enum DAMAGE_FLAGS {
    DF_BLOCKABLE	= 0x0001, //this attack can be blocked by shield
    DF_AFFECT_HIT	= 0x0002, //this attack can be avoided also it can be exact.
    DF_AFFECT_PV	= 0x0004, //this attack can be stopped by armour

    DF_MAGIC_BOLT	= DF_BLOCKABLE | DF_AFFECT_HIT | DF_AFFECT_PV,
};

struct DAMAGE_DATA_EX {
    XCreature* attacker;       // who is inflicting damage (to increase exp). can be nullptr
    const char* attack_name;   // Attacking item (e.g. Arrow, Bolt of Fire)
    int damage;                // supposed damage
    int attack_HIT;            // the target can avoid attack.
    unsigned int attack_brand; // such a cold, demon slaying,
    unsigned int flags;        // see DAMAGE_FLAGS
    XItem* weapon;             // used only in melee combat (can be undefined if attack_name is defined)
};

struct _CREATURE;

typedef std::map<GROUP_ID, XCreature*> XCreatureGroupMap;

class XCreature : public XBaseObject
{
    private:
        // orc war party has id 1, bandits - id 2, etc
        GROUP_ID group_id;

        // all group members
        static XCreatureGroupMap group_members;

    public:
        XItemList contain;
        std::vector<std::unique_ptr<XBodyPart>> components;
        CR_PERSON_TYPE creature_person_type;
        const char* creature_description;
        CREATURE_NAME creature_name; // allow to store less info into save file
        const _CREATURE* super_info; // full information about Creature Creation struct...
        char* event_handler;
        void SetEventHandler(const char* handler);
    public:
        DECLARE_CREATOR(XCreature, XBaseObject);
        XCreature();
        void Invalidate() override;

        // Safely wrap a creature pointer that may be null, or may already
        // be dead (in which case shared_from_this() would throw
        // std::bad_weak_ptr), into a weak_ptr - returns an empty weak_ptr in
        // either of those cases.
        static std::weak_ptr<XCreature> ToWeakPtr(XCreature* cr);

        int Compare(XObject* o) override
        {
            return 1;
        }

        int TestMove();
        virtual void NewMove();
        virtual void Move();
        virtual void HideOldView();
        virtual void ShowNewView();
        virtual void PutStatus();
        virtual void DoMove();

        bool Run() override;

        std::unique_ptr<XStandardAI> xai;

        unsigned long _EXP;
        int level;
        int base_exp;
        unsigned long ExpOfLevel(int lev) const;
        void AddExp(unsigned long exp);

        int GetSpeed();

        CR_GENDER GetGender();
        const char* GetGenderStr();

        int lttm;      // long doing time to move
        int isDisturb; // is creature disturbed during lttm

        void setGroupID(GROUP_ID gid);

        GROUP_ID groupID() {
            return group_id;
        }

        std::vector<XCreature*> getGroupMembers() const;

        ACTION_DATA action_data;
        virtual int stopAction();

        void Regenerate();
        int onHeal(int _hp);
        int onRestorePP(int _pp);

        virtual void IncLevel();

        virtual int Read(XItem* item);
        int continueRead();

        virtual int Eat(XAnyFood* food);
        int continueEat();

        virtual int UseItem(XTool* tool);
        int continueUseItem();

        int base_nutrio;  // stomach size for normal satiation
        int nutrio;       // stomach satiation;
        int nutrio_speed; // speed with which nutrients decrease;
        int DecNutrio();
        FOOD_FEELING food_feeling;

        void MoveStairWay();

        virtual void Attack();
        virtual void Die(XCreature * killer);

        XBodyPart* GetRNDBodyPart(ITEM_MASK xim, RBP_FLAG rbpf);
        XBodyPart* GetRNDBodyPart();
        int GetHITFHBonus(XItem* weapon);
        int GetShieldDVBonus();
        int GetDMGFHBonus(XItem* weapon);
        int GetHIT();
        int GetDV(XCreature* attacker = nullptr);
        int GetDMG();
        int GetPV();
        int GetResistance(RESISTANCE tr);
        int GetVisibleRadius();
        int GetTacticsDVBonus();
        int GetTacticsHITBonus();
        int GetTacticsDMGBonus();

        // Get target for a spell
        virtual int GetTarget(TARGET_REASON tr, XPoint* pt = nullptr, int max_range = 0, XObject** back = nullptr);
        virtual std::shared_ptr<XItem> onIdentifyItem()
        {
            return nullptr;
        }

        virtual std::shared_ptr<XItem> SelectItem(XItemFilter* filtr, bool isGetAll = false)
        {
            return nullptr;
        }

        int Shoot(int tx, int ty);
        XItem* GetItem(BODY_PART bp, int count = 0);
        XBodyPart* GetBodyPart(BODY_PART bp, int count = 0);
        bool CanWear(const XItem* item);
        bool Wear(XItem* item) const; // if can Wear, Wear it.
        bool IsWorn(const XItem* item) const;

        XModifier* md;
        XMagic* m;
        XSkills* sk;
        XWarSkills* wsk;

        XResistance added_resists;
        XStats added_stats;
        XStats max_stats;
        int added_DV;
        int added_PV;
        int added_HIT;
        int added_DMG;
        int added_RNG;
        int added_HP;
        int added_PP;
        int added_speed;

        // Adds item to creature inventory, increase carried weight.
        bool ContainItem(XItem* item);

        int DropItem(XItem* i);
        int PickUpItem(XItem* i);
        CARRY_STATE GetCarryState();
        int CarryValue(CARRY_STATE cs);
        bool CarryItem(XItem* item);
        void UnCarryItem(XItem* item);
        int carried_weight;

        int MoneyOp(int money_count); // if money_count >= 0 then add money, else sub.

        int GetStats(STATS st);
        int GainAttr(STATS st, int val);
        int GainResist(RESISTANCE rs, int val);
        int GetMaxHP();
        int GetMaxPP();
        int GetExp() const;
        int GetCreatureStrength();

        int InflictDamage(DAMAGE_DATA_EX* pData);
        int onMagicDamage(int dmg, RESISTANCE tr);
        int CauseEffect(int dmg, BRAND_TYPE brt, XCreature* attacker);
        void CausePostEffect(int dmg, BRAND_TYPE brt, XCreature* attacker);

        virtual const char* GetMeleeAttackMsg(XItem* weapon);

        virtual void FirstStep(int _x, int _y, XLocation* _l);
        virtual void LastStep();

        static MF_RESULT MissileFlight(MF_DATA* mfd);

        void Store(XFile* f) override;
        void Restore(XFile* f) override;

        // Defined in creature.cpp, where creature/anycr.h (for
        // XCreatureStorage) and the Lua C headers are already included
        // without a circular-include problem - both are needed by
        // load()/save() below but can't live in this header directly
        // (anycr.h itself includes this header).
        void FixupCreatureInfo();
        void NotifyLuaEventHandler(LUA_EVENT event) const;

        // magic/modifier.h can't be #included here (magic/modifiers.h
        // includes this header right back, and unlike XCreatureStorage/
        // Lua above, `md` actually needs dereferencing inline in
        // save()/load() below, not just calling through) - pinned to
        // the concrete JSON archive types (the only ones this project
        // uses) rather than kept generic, so these can be ordinary,
        // non-template member functions defined in creature.cpp, where
        // magic/modifier.h is available without the cycle.
        void SaveModifier(cereal::JSONOutputArchive& ar) const;
        void LoadModifier(cereal::JSONInputArchive& ar);

        // m/md/sk/wsk are owned via raw pointer (see the ctor and
        // Invalidate()), not smart pointers - dereferenced directly
        // here rather than changed to unique_ptr just for Cereal's
        // sake. On load each is heap-allocated first (mirroring the
        // existing Restore()), then its own serialize()/load()
        // populates it - same choice made for XSkills/XWarSkills.
        //
        // creature_description/super_info are non-owning pointers into
        // the static per-species table (XCreatureStorage) - re-derived
        // from creature_name via XCreatureStorage::RestoreCreatureInfo()
        // for non-hero creatures, same as the existing Restore().
        //
        // event_handler mirrors XOuterObject::onEventLua (an owned heap
        // char*, not std::string) but also fires the same Lua
        // LE_SAVE/LE_LOAD notification the existing Store/Restore
        // already do, via NotifyLuaEventHandler() (kept out of this
        // header since it needs the Lua C headers, which nothing else
        // here requires).
        //
        // xai's ai_owner isn't part of XStandardAI::serialize() (see
        // std_ai.h) - fixed up here immediately after loading it.
        //
        // contain/components/xai/sk/m/md/wsk were all entirely unsaved
        // before this (see the FIXMEs still in Store/Restore) - new
        // real persistence, not a mechanical port.
        template<class Archive>
        void save(Archive& ar) const
        {
            ar(cereal::base_class<XBaseObject>(this));
            ar(_EXP, added_DMG, added_DV, added_HIT, added_HP, added_PP, added_PV);
            ar(attack_energy, move_energy, base_speed, added_speed);
            ar(added_resists, added_RNG, added_stats);
            ar(base_exp, base_nutrio, carried_weight);
            ar(components);
            ar(creature_class, creature_size, food_feeling, group_id);
            ar(level);
            ar(*m);
            SaveModifier(ar);
            ar(nutrio, nutrio_speed, *sk);
            ar(tactics, *wsk);
            ar(xai);
            ar(action_data);
            ar(contain);
            ar(religion, max_stats);
            ar(creature_person_type, creature_name);
            ar(std::string(event_handler ? event_handler : ""));
            NotifyLuaEventHandler(LE_SAVE);
        }

        template<class Archive>
        void load(Archive& ar)
        {
            ar(cereal::base_class<XBaseObject>(this));
            ar(_EXP, added_DMG, added_DV, added_HIT, added_HP, added_PP, added_PV);
            ar(attack_energy, move_energy, base_speed, added_speed);
            ar(added_resists, added_RNG, added_stats);
            ar(base_exp, base_nutrio, carried_weight);
            ar(components);
            ar(creature_class, creature_size, food_feeling, group_id);
            ar(level);
            m = new XMagic();
            ar(*m);
            LoadModifier(ar);
            ar(nutrio, nutrio_speed);
            sk = new XSkills();
            ar(*sk);
            ar(tactics);
            wsk = new XWarSkills();
            ar(*wsk);
            ar(xai);

            if (xai) {
                xai->SetOwner(this);
            }

            ar(action_data);
            ar(contain);
            ar(religion, max_stats);
            ar(creature_person_type, creature_name);
            FixupCreatureInfo();

            std::string event;
            ar(event);

            if (event.empty()) {
                event_handler = nullptr;
            } else {
                event_handler = new char[event.size() + 1];
                std::memcpy(event_handler, event.c_str(), event.size() + 1);
            }

            NotifyLuaEventHandler(LE_LOAD);
        }

        CREATURE_CLASS creature_class;

        virtual std::string StdAnswer()
        {
            return "You receive no answer.";
        }

        virtual int Chat(XCreature* chatter, const char* msg);

        virtual int onGiveItem(XCreature* giver, XItem * item);

        TACTICS_STATE tactics;
        void ChangeTactics(const TACTICS_STATE tact)
        {
            tactics = tact;
        }

        const char* GetWoundMsg(int flag = 0);

        void GetRangeAttackInfo(int* range, int* hit, XDice* dmg);

        CREATURE_SIZE creature_size;
        int attack_energy;
        int move_energy;
        int base_speed;
        int MeleeAttack(XCreature* target, XItem* weapon);
        std::vector<MELEE_ATTACK>* melee_attack;

        void Sacrifice(XItem* item);
        XReligion religion;

        int isCreatureVisible(XCreature* cr); // check if creature visible or invisible
        bool isVisible() override;       // check if creature visible to add action message to msgwin
        static XCreature* main_creature; // at this time is used to determine visibility of msg

        bool isHero() const
        {
            return (im & IM_HERO) > 0;
        }

        const std::string GetNameEx(CR_NAME_TYPE crn);

        [[nodiscard]] std::string GetVerb(std::string verb) const;
};

// XBaseObject (an ancestor) has a member serialize(), and XCreature has
// a member load()/save() pair - Cereal's access rules mean that pair is
// ambiguous with the inherited serialize() unless disambiguated
// explicitly (see cereal/specialize.hpp's own worked example, which is
// this exact scenario).
CEREAL_SPECIALIZE_FOR_ALL_ARCHIVES(XCreature, cereal::specialization::member_load_save);

// Fake creature is need
class XFakeCreature final : public XCreature
{
    public:
        explicit XFakeCreature(std::string_view fake_name)
        {
            name = fake_name;
        }
};

#endif
