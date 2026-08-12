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

#ifndef STD_AI_H
#define STD_AI_H

#include <deque>
#include <vector>

#include <cereal/cereal.hpp>
#include <sol/forward.hpp>

#include "creature/creature.h"
#include "creature/cr_defs.h"
#include "helpers/rect.h"
#include "item/item.h"
#include "item/itemkind.h"

enum COMPANION_COMMAND {
    CC_NONE,
    CC_ATTACK,
    CC_MOVE,
    CC_WAIT,
    CC_FOLLOW,
    CC_GUARD,
};

enum SCRIPT_COMMAND {
    SCC_NONE,
    SCC_MOVE_POINT,
    SCC_MOVE_LOCATION,
    SCC_COLLECT_MUSHROOM,
    SCC_DROP_ITEM,
};

struct SCRIPT_CMD {
    SCRIPT_COMMAND cmd;
    int pt_x;
    int pt_y;
    XLocation::Id ln;
    ItemKind kind;
};

#define ENEMY_LIST_SIZE	5
class XCreature;

class XStandardAI
{
    public:
        enum Flag {
            NONE                = 0x00000000,
            ALLOW_PICK_UP       = 0x00000001, // creature can pick items

            ALLOW_MOVE_WAY_UP   = 0x00000002, // creature can move upstairs
            ALLOW_MOVE_WAY_DOWN = 0x00000004, // creature can move downstairs
            FREE_WAY            = ALLOW_MOVE_WAY_UP | ALLOW_MOVE_WAY_DOWN,
            ALLOW_MOVE_OUT      = 0x00000008, // creature can move to main location
            FREE_MOVE           = FREE_WAY | ALLOW_MOVE_OUT, // creature can move were it wants

            FIND_WAY            = 0x00000010, // creature pursuits enemy to other locations
            PEACEFUL            = 0x00000020, // creature never attack first
            COWARD              = 0x00000040, // creature fears stronger creatures, also runs away when wounded to much
            ALLOW_PACK          = 0x00000100, // creature tries to form a pack
            ALLOW_WEAR_ITEM     = 0x00000200, // creature can wear items
            GUARD_AREA          = 0x00000400, // this creature guards an area
            PROTECT_AREA        = 0x00000800, // creature attack every who enter this area
            RANDOM_MOVE         = 0x00001000, // creature can move randomly
            EXPLORER_MOVE       = 0x00002000, // creature explores dungeon
            EXECUTE_SCRIPT      = 0x00004000, // if nothing to do, execute script...
            NO_SWAP             = 0x00008000, // you can't swap place with this creature if it is friendly

            INSECT              = FREE_WAY | RANDOM_MOVE,
            LO_ANIMAL           = FREE_WAY | RANDOM_MOVE | COWARD,
            HI_ANIMAL           = FREE_WAY | RANDOM_MOVE | FIND_WAY | COWARD,
            CREATURE            = ALLOW_PICK_UP | ALLOW_WEAR_ITEM | FREE_WAY | RANDOM_MOVE | FIND_WAY | COWARD,
            HUMAN               = ALLOW_PICK_UP | ALLOW_WEAR_ITEM | FREE_MOVE | RANDOM_MOVE | FIND_WAY | COWARD,
            GHOST               = FREE_WAY | RANDOM_MOVE | FIND_WAY | COWARD,
        };

        // Registers this enum as the Lua table XStandardAI.MEMBER.
        static void RegisterLua(sol::state_view& lua);

        XStandardAI() = delete;
        XStandardAI(XCreature * _cr);

        virtual ~XStandardAI();

        void SetArea(XRect& area, XLocation::Id ln);
        void SetOwner(XCreature * cr)
        {
            ai_owner = cr;
        }
        void SetCompanion(XCreature * cr);
        void SetOrderedEnemy(XCreature * cr);

        virtual void AnalyzeGrid(int j, int i, int w);
        virtual void AnalyzeView(int radius);
        virtual void Move();
        virtual bool isPersonalEnemy(XCreature *cr);
        virtual bool isEnemy(XCreature *cr);
        virtual void onWasAttacked(XCreature * attacker);

        // The actual "someone attacked me" reaction, factored out of
        // onWasAttacked() so it's directly callable (non-virtual, so no
        // risk of re-entering an XLuaAI override) - lets a Lua AI hook run
        // the standard reaction aimed at a stand-in target instead of the
        // real attacker (e.g. RotmothAI redirecting retaliation onto a
        // rescued companion, see world/locations.lua) without needing to
        // reimplement last_enemy/invisible_hunting_mode tracking in Lua,
        // neither of which is otherwise exposed. onWasAttacked() itself
        // just forwards to this by default.
        void ReactToAttacker(XCreature * attacker);

        virtual void onDie(XCreature * killer);
        virtual void onSteal(XCreature * rogue);

        virtual int Chat(XCreature* chatter, const char* msg);
        virtual int onGiveItem(XCreature * giver, XItem * item);
        virtual int GetTargetPos(XPoint * pt);

        int Wear() const;

        void GetDirection(const XPoint* target, XPoint* direction) const;      // calculate exact direction to target
        void GetRandDirection(const XPoint* target, XPoint* direction) const;  // calculate approximate direction target
        void GetExactDirection(const XPoint* target, XPoint* direction) const; // calculate exact direction on target

        void SetAIFlag(Flag aif);
        void ResAIFlag(Flag aif);

        unsigned int GetAIFlag() const
        {
            return ai_flag;
        }

        void SetEnemyClass(CreatureClass cr_class);

        // Carries ai_flag/enemy_class/guard_area(+location) forward onto a
        // replacement AI object - SetCreatureAI() (see game/location.cpp)
        // swaps in a brand-new XLuaAI, whose own constructor (via
        // XStandardAI's) would otherwise silently reset all of this back
        // to defaults (ai_flag=NONE, enemy_class=ALL, a placeholder
        // guard_area), discarding whatever a prior Guardian()/NewCreature()
        // call already set up. A member function rather than public
        // getters/setters for each field, since it only needs to exist for
        // this one purpose.
        void CopyBaseStateTo(XStandardAI& other) const
        {
            other.ai_flag = ai_flag;
            other.enemy_class = enemy_class;
            other.guard_area = guard_area;
            other.guard_area_location = guard_area_location;
        }

        void AddPersonalEnemy(XCreature * cr);
        void RemovePersonalEnemy(const XCreature* cr);

        // Matches the legacy Store/Restore's actual scope, not just
        // what's convenient - companion/ordered_enemy/personal_enemy/
        // last_enemy (all weak_ptr<XCreature>, easy to serialize) and
        // script were never persisted even before Cereal, a
        // pre-existing design choice rather than a migration
        // regression, so left alone here too. last_moved_way/
        // known_traps (raw XMapObject*) are also still deliberately
        // unpersisted - unlike XShopKeeperAI::shop (see
        // XLocation::FixupShops()), nothing currently re-derives these
        // structurally after load, so a restored creature's AI starts
        // with no memory of traps it had already found. Worth
        // revisiting, but out of scope here.
        //
        // ai_owner isn't serialized here either - XStandardAI always
        // lives 1:1 inside its owning XCreature (see xai), so it's
        // fixed up by that XCreature's own load(), same idea as
        // XBodyPart::owner_raw.
        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(ai_flag, enemy_class, invisible_x, invisible_y, invisible_hunting_mode);
            ar(companion_command, guard_area, guard_area_location);
        }

        void SetGroupEnemy(XCreature* cr) const;

        // companion
        std::weak_ptr<XCreature> companion;
        COMPANION_COMMAND companion_command;
        std::weak_ptr<XCreature> ordered_enemy;

        std::deque<SCRIPT_CMD> script;
        void ExecuteScript(const std::vector<SCRIPT_CMD> &scr);
        void RunScript();

        // the creature who knows a trap can't activate it
        // used for random rooms guardians.
        std::vector<XMapObject*> known_traps;
        void LearnTraps();
        bool isKnowThisTrap(const XMapObject* trap);

        int sleep_well;
    protected:
        std::weak_ptr<XCreature> personal_enemy[ENEMY_LIST_SIZE];

        int FindPath(const XPoint* target, XPoint* direction) const;
        int AttackEnemy(int x, int y) const;
        int CastSpell() const;
        int Shoot() const;
        int ReadScroll() const;
        int DrinkPotion() const;
        int PickUpItems() const;

        int MoveTo(int x, int y, XLocation* l = nullptr) const;

        // creature tries to run away from attacker
        int TryToRunAway() const;

        // can move here without risk of attacking friendly creature
        bool CanMoveHere(int px, int py);

        Flag ai_flag;
        XCreature* ai_owner;
        CreatureClass enemy_class;

        XRect guard_area;
        XLocation::Id guard_area_location;

        // currently targeted creature
        XCreature* enemy;

        // distance to the closest enemy
        int enemy_dist;

        std::weak_ptr<XCreature> last_enemy;

        // used to prevent up/down moving way repeating...
        XMapObject* last_moved_way;
        int invisible_x;
        int invisible_y;
        int invisible_hunting_mode;

        int friend_avg_x;
        int friend_avg_y;

        // count of friends (which have the same enemy) for coward attack
        int friends_count;

        // distance to the closest item
        int item_dist;

        // item x coordinate
        int item_x;

        // item y coordinate
        int item_y;

        int way_dist;
        int way_x;
        int way_y;
};

// XStandardAI() is deleted - real construction always takes an owning
// XCreature*, fixed up separately by that creature's own load() (see
// the comment on serialize() above), so a null placeholder is fine
// here. Must live here, not in std_ai.cpp: XStandardAI's construction
// gets instantiated from inside XCreature::load() itself, which gets
// reinstantiated in every TU that reaches XCreature::xai (e.g. any
// item's owner weak_ptr<XCreature> chain) - a specialization declared
// only in std_ai.cpp wouldn't be visible to those other TUs.
CEREAL_LOAD_VIA_PLACEHOLDER_CONSTRUCT(XStandardAI, serialize, nullptr);

#endif
