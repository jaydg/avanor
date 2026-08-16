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
#include "magic/attack_effect_type.h"
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
    AttackEffectType br_attack;
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
    AttackEffectType attack_effect; // such a cold, demon slaying,
    unsigned int flags;        // see DAMAGE_FLAGS
    XItem* weapon;             // used only in melee combat (can be undefined if attack_name is defined)
};

struct CreatureTemplate;

typedef std::map<GROUP_ID, XCreature*> XCreatureGroupMap;

class XCreature : public XBaseObject
{
    public:
        enum Gender {
            NEUTER,
            MALE,
            FEMALE,

            // TODO: Write code to pick a random gender if male & female flags on.
            RANDOM = (FEMALE | MALE)
        };

        enum PersonType {
            IT = NEUTER,  // It
            HE = MALE,    // He
            SHE = FEMALE, // She

            // Default you
            YOU = 0x08,

            // Genderized you
            MALE_YOU = (HE | YOU),
            FEMALE_YOU = (SHE | YOU),

            // Unique creatures
            UNIQUE = 0x10,

            // Backward compatibility
            NAMED_HE = (HE | UNIQUE),   // Munch-Munch the Dread
            NAMED_SHE = (SHE | UNIQUE), // Yohjishiro, the elven wizard
            NAMED_IT = (IT | UNIQUE)    // Gekta, the sheep dog
        };

        // Registers Gender and PersonType as the Lua tables Gender.MEMBER
        // and PersonType.MEMBER (e.g. PersonType.NAMED_SHE).
        static void RegisterLua(sol::state_view& lua);

    private:
        // orc war party has id 1, bandits - id 2, etc
        GROUP_ID group_id;

        // all group members
        static XCreatureGroupMap group_members;

    public:
        XItemList contain;
        std::vector<std::unique_ptr<XBodyPart>> components;
        PersonType creature_person_type;
        const char* creature_description;
        CREATURE_NAME creature_name; // allow to store less info into save file
        const CreatureTemplate* super_info; // full information about Creature Creation struct...

        // Is this one of the small set of unique NPCs with a hand-written
        // C++ subclass (see XCreatureStorage::unique_creators)? Not
        // persisted - derived from creature_name, along with
        // melee_attack/creature_description/super_info above, by
        // XCreatureStorage::RestoreCreatureInfo() on both the fresh-
        // creation and the Cereal-load path. false for the hero and
        // every ordinary monster.
        bool unique = false;
        char* event_handler;
        void SetEventHandler(const char* handler);

        // Opt-in gate for LuaEvent::AI_TURN/PRE_MOVE dispatch in NewMove()/
        // Move() - both fire for every creature, every turn, so unlike
        // Chat/Die/onGiveItem (rare, player-initiated) this can't just be
        // gated on event_handler being set: every creature with a Chat
        // handler (farmers, unique NPCs) would otherwise pay a Lua call on
        // every single move. Only creatures that explicitly opt in via
        // EnableMoveHandler() pay that cost.
        bool wants_move_hook = false;
        void EnableMoveHandler();

        // Turns wants_move_hook back off - for a handler that only needs
        // the per-turn callback for a bounded stretch of gameplay (e.g.
        // escorting a rescued NPC home), not for the rest of the game.
        void DisableMoveHandler();

        // Backing store for event_handler's Lua StoreInt/RestoreInt calls
        // (see NotifyLuaEventHandler()) - filled by StoreInt during
        // LuaEvent::SAVE, serialized, then read back and handed out via
        // RestoreInt during LuaEvent::LOAD.
        std::vector<int> lua_ints;
    public:
        DECLARE_CREATOR(XCreature, XBaseObject);
        XCreature();
    protected:
        // teardown hook, called by XObject::Invalidate()
        void OnInvalidate() override;
    public:

        // Safely wrap a creature pointer that may be null, or may already
        // be dead (in which case shared_from_this() would throw
        // std::bad_weak_ptr), into a weak_ptr - returns an empty weak_ptr in
        // either of those cases.
        static std::weak_ptr<XCreature> ToWeakPtr(XCreature* cr);

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

        XCreature::Gender GetGender();
        const char* GetGenderStr();

        int lttm;      // long doing time to move
        int isDisturb; // is creature disturbed during lttm

        void setGroupID(const GROUP_ID& gid);

        const GROUP_ID& groupID() {
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

        XBodyPart* GetRNDBodyPart(ItemKind kind, RBP_FLAG rbpf);
        XBodyPart* GetRNDBodyPart();
        int GetHITFHBonus(XItem* weapon);
        int GetShieldDVBonus();
        int GetDMGFHBonus(XItem* weapon);
        int GetHIT();
        int GetDV(XCreature* attacker = nullptr);
        int GetDMG();
        int GetPV();
        int GetResistance(XResistance::Id tr);
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

        // Adds item to creature inventory, increase carried weight. Returns
        // the surviving owned XItem, or nullptr if CarryItem() rejected it
        // (e.g. a dying creature) - `item` itself is not necessarily that
        // survivor: XItemList::insert() (see item/item.h) merges `item`
        // into an already-carried, stackable-equal item instead of keeping
        // it as its own entry when one exists, freeing `item` as part of
        // the merge. Compare the result against `item` (by pointer) to
        // tell "kept as-is" apart from "merged and freed" - never touch
        // `item` again after calling this without doing that check first.
        std::shared_ptr<XItem> ContainItem(XItem* item);

        int DropItem(XItem* i);
        int PickUpItem(XItem* i);
        CARRY_STATE GetCarryState();
        int CarryValue(CARRY_STATE cs);
        bool CarryItem(XItem* item);
        void UnCarryItem(XItem* item);
        int carried_weight;

        int MoneyOp(int money_count); // if money_count >= 0 then add money, else sub.

        int GetStats(XStats::Id st);
        int GainAttr(XStats::Id st, int val);
        int GainResist(XResistance::Id rs, int val);
        int GetMaxHP();
        int GetMaxPP();
        int GetExp() const;
        int GetCreatureStrength();

        int InflictDamage(DAMAGE_DATA_EX* pData);
        int onMagicDamage(int dmg, XResistance::Id tr);
        int CauseEffect(int dmg, AttackEffectType brt, XCreature* attacker);
        void CausePostEffect(int dmg, AttackEffectType brt, XCreature* attacker);

        virtual const char* GetMeleeAttackMsg(XItem* weapon);

        // Movement intent for the turn being resolved: where this creature
        // wants to step, decided by the AI and then acted on by Move().
        // Distinct from x/y, which stay put until the  step actually succeeds.
        int nx, ny;

        virtual void FirstStep(int _x, int _y, XLocation* _l);
        virtual void LastStep();

        static MF_RESULT MissileFlight(MF_DATA* mfd);

        // Defined in creature.cpp, where creature/anycr.h (for
        // XCreatureStorage) and the Lua C headers are already included
        // without a circular-include problem - both are needed by
        // load()/save() below but can't live in this header directly
        // (anycr.h itself includes this header).
        void FixupCreatureInfo();
        // Not const: hands lua_ints out as a mutable buffer for
        // StoreInt to append to (see game/location.h/.cpp).
        void NotifyLuaEventHandler(LuaEvent event);

        // Also defined in creature.cpp. This header does #include
        // "creature/std_ai.h" above (needed regardless: ar(xai) below
        // requires XStandardAI complete for Cereal to resolve its
        // serialize(), in every TU that instantiates this template) -
        // but that alone isn't enough to call xai->SetOwner(this)
        // directly here. It's a non-dependent expression (doesn't
        // mention Archive), so GCC resolves it eagerly, at the textual
        // point this template is first parsed - and in any TU that
        // reaches std_ai.h before creature.h (std_ai.h includes this
        // header back; the include guard makes that nested include a
        // no-op, so creature.h's own body gets parsed, StandardAI and
        // all, before std_ai.h reaches its own class XStandardAI
        // definition), that point still sees it incomplete, regardless
        // of it becoming complete later in the same file. ar(xai) above
        // doesn't hit this: Cereal's dispatch is a dependent expression,
        // resolved at instantiation time, by which point the whole TU
        // (and so std_ai.h, however it got there) has been seen.
        void FixupXaiOwner();

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
        // char*, not std::string), firing the same Lua LuaEvent::SAVE/LuaEvent::LOAD
        // notification the legacy Store/Restore (since removed) used
        // to, via NotifyLuaEventHandler() (kept out of this header
        // since it needs the Lua C headers, which nothing else here
        // requires).
        //
        // xai's ai_owner isn't part of XStandardAI::serialize() (see
        // std_ai.h) - fixed up here immediately after loading it.
        //
        // contain/components/xai/sk/m/md/wsk were all entirely unsaved
        // before this (the legacy Store/Restore, since removed, left
        // them as FIXMEs) - new real persistence, not a mechanical
        // port.
        //
        // One symmetric serialize() rather than a split save()/load()
        // pair: a base class (XBaseObject) has its own member
        // serialize(), which makes a derived load()/save() pair
        // ambiguous to Cereal unless explicitly disambiguated (see
        // cereal/specialize.hpp's own worked example) - that
        // disambiguation compiles fine, but broke Cereal's *polymorphic
        // type registration* for every XCreature subclass at runtime
        // (confirmed live: "Trying to save an unregistered polymorphic
        // type" for XAnyCreature specifically, despite its
        // CEREAL_REGISTER_TYPE being textually correct and even
        // explicitly, manually re-invoked - items, whose base hierarchy
        // never needed this disambiguation, round-tripped fine).
        // `if constexpr (Archive::is_loading::value)` reproduces the
        // load-only steps (heap-allocating m/md/sk/wsk before
        // dereferencing them, the ai_owner/event_handler/
        // FixupCreatureInfo fixups) inside one method instead.
        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XBaseObject>(this));

            // FixupCreatureInfo() re-derives them from the cell each
            // creature is found in anyway, so this only matters for a
            // creature not reached by that structural pass.
            ar(nx, ny);

            ar(_EXP, added_DMG, added_DV, added_HIT, added_HP, added_PP, added_PV);
            ar(attack_energy, move_energy, base_speed, added_speed);
            ar(added_resists, added_RNG, added_stats);
            ar(base_exp, base_nutrio, carried_weight);
            ar(components);

            if constexpr (Archive::is_loading::value) {
                // XBodyPart::owner (a weak_ptr, its only Cereal-trackable
                // form) can be legitimately unbound even in the live,
                // never-saved game - a bodypart equipped with starting
                // gear from within this very constructor runs before
                // shared_from_this() is safe (see SetOwner()'s own
                // comment) - so it isn't something safe to round-trip
                // via `owner` and re-derive owner_raw from. Re-run
                // SetOwner() structurally instead, now that `this` is
                // guaranteed shared_from_this()-safe (same reasoning as
                // XMapObject::l/SetLocation()).
                for (auto& bp : components) {
                    bp->SetOwner(this);
                }
            }

            ar(creature_class, creature_size, food_feeling, group_id);
            ar(level);

            if constexpr (Archive::is_loading::value) {
                m = new XMagic();
            }

            ar(*m);

            if constexpr (Archive::is_loading::value) {
                LoadModifier(ar);
            } else {
                SaveModifier(ar);
            }

            ar(nutrio, nutrio_speed);

            if constexpr (Archive::is_loading::value) {
                sk = new XSkills();
            }

            ar(*sk);
            ar(tactics);

            if constexpr (Archive::is_loading::value) {
                wsk = new XWarSkills();
            }

            ar(*wsk);
            ar(xai);

            if constexpr (Archive::is_loading::value) {
                FixupXaiOwner();
            }

            ar(action_data);
            ar(contain);
            ar(religion, max_stats);
            ar(creature_person_type, creature_name);
            ar(wants_move_hook);

            if constexpr (Archive::is_loading::value) {
                FixupCreatureInfo();

                std::string event;
                ar(event);

                if (event.empty()) {
                    event_handler = nullptr;
                } else {
                    event_handler = new char[event.size() + 1];
                    std::memcpy(event_handler, event.c_str(), event.size() + 1);
                }

                // lua_ints must be read back before firing: RestoreInt
                // hands its contents out sequentially as the handler runs.
                ar(lua_ints);
                NotifyLuaEventHandler(LuaEvent::LOAD);
            } else {
                ar(std::string(event_handler ? event_handler : ""));

                // Cleared first in case this creature was saved before,
                // earlier in the same run - firing appends to it via
                // StoreInt, so a stale leftover would double up.
                lua_ints.clear();
                NotifyLuaEventHandler(LuaEvent::SAVE);
                ar(lua_ints);
            }
        }

        CreatureClass creature_class;

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

        [[nodiscard]] bool isHero() const;

        const std::string GetNameEx(CR_NAME_TYPE crn);

        [[nodiscard]] std::string GetVerb(std::string verb) const;
};

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
