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

#include "creature/creature.h"
#include "creature/cr_defs.h"
#include "helpers/rect.h"
#include "item/item.h"

enum AI_FLAG {
    AIF_NONE                = 0x00000000,
    AIF_ALLOW_PICK_UP       = 0x00000001, // creature can pick items

    AIF_ALLOW_MOVE_WAY_UP   = 0x00000002, // creature can move upstairs
    AIF_ALLOW_MOVE_WAY_DOWN = 0x00000004, // creature can move downstairs
    AIF_FREE_WAY            = AIF_ALLOW_MOVE_WAY_UP | AIF_ALLOW_MOVE_WAY_DOWN,
    AIF_ALLOW_MOVE_OUT      = 0x00000008, // creature can move to main location
    AIF_FREE_MOVE           = AIF_FREE_WAY | AIF_ALLOW_MOVE_OUT, // creature can move were it wants

    AIF_FIND_WAY            = 0x00000010, // creature pursuits enemy to other locations
    AIF_PEACEFUL            = 0x00000020, // creature never attack first
    AIF_COWARD              = 0x00000040, // creature fears stronger creatures, also runs away when wounded to much
    AIF_ALLOW_PACK          = 0x00000100, // creature tries to form a pack
    AIF_ALLOW_WEAR_ITEM     = 0x00000200, // creature can wear items
    AIF_GUARD_AREA          = 0x00000400, // this creature guards an area
    AIF_PROTECT_AREA        = 0x00000800, // creature attack every who enter this area
    AIF_RANDOM_MOVE         = 0x00001000, // creature can move randomly
    AIF_EXPLORER_MOVE       = 0x00002000, // creature explores dungeon
    AIF_EXECUTE_SCRIPT      = 0x00004000, // if nothing to do, execute script...
    AIF_NO_SWAP             = 0x00008000, // you can't swap place with this creature if it is friendly

    AIF_INSECT              = AIF_FREE_WAY | AIF_RANDOM_MOVE,
    AIF_LO_ANIMAL           = AIF_FREE_WAY | AIF_RANDOM_MOVE | AIF_COWARD,
    AIF_HI_ANIMAL           = AIF_FREE_WAY | AIF_RANDOM_MOVE | AIF_FIND_WAY | AIF_COWARD,
    AIF_CREATURE            = AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM | AIF_FREE_WAY | AIF_RANDOM_MOVE | AIF_FIND_WAY | AIF_COWARD,
    AIF_HUMAN               = AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM | AIF_FREE_MOVE | AIF_RANDOM_MOVE | AIF_FIND_WAY | AIF_COWARD,
    AIF_GHOST               = AIF_FREE_WAY | AIF_RANDOM_MOVE | AIF_FIND_WAY | AIF_COWARD,
};


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
    LOCATION ln;
    ITEM_MASK im;
};

#define ENEMY_LIST_SIZE	5
class XCreature;
class XStandardAI : public XObject
{
    protected:
        XStandardAI() {}

    public:
        DECLARE_CREATOR(XStandardAI, XObject);
        XStandardAI(XCreature * _cr);
        virtual void Invalidate();

        void SetArea(XRect& area, LOCATION ln);
        void SetOwner(XCreature * cr)
        {
            ai_owner = cr;
        }

        virtual void AnalyzeGrid(int j, int i, int w);
        virtual void AnalyzeView(int radius);
        virtual void Move();
        virtual int isPersonalEnemy(XCreature * cr);
        virtual int isEnemy(XCreature * cr);
        virtual void onWasAttacked(XCreature * attacker);
        virtual void onDie(XCreature * killer);
        virtual void onSteal(XCreature * rogue);

        virtual int Chat(XCreature * chatter = NULL, const char* msg = NULL);
        virtual int onGiveItem(XCreature * giver, XItem * item);
        virtual int GetTargetPos(XPoint * pt);

        int Wear();

        void GetDirection(XPoint * target, XPoint * direction); // calculate exact direction to target
        void GetRandDirection(XPoint * target, XPoint * direction); // calculate aproximate direction target
        void GetExactDirection(XPoint * target, XPoint * direction); // calucalte exact direction on target

        void SetAIFlag(AI_FLAG aif);
        void ResAIFlag(AI_FLAG aif);
        unsigned int GetAIFlag()
        {
            return ai_flag;
        }

        void SetEnemyClass(CREATURE_CLASS cr_class);

        void AddPersonalEnemy(XCreature * cr);
        void RemovePersonalEnemy(XCreature * cr);

        virtual void Store(XFile * f);
        virtual void Restore(XFile * f);
        void SetGroupEnemy(XCreature * enemy);


        // companion
        XPtr<XCreature> companion;
        COMPANION_COMMAND companion_command;
        XPtr<XCreature> ordered_enemy;

        XQList<SCRIPT_CMD> script;
        void ExecuteScript(XQList<SCRIPT_CMD>* scr);
        void RunScript();

        // the create who know trap can't activate it
        // used for random rooms guardians.
        XList<XMapObject*> known_traps;
        void LearnTraps();
        bool isKnowThisTrap(XMapObject * trap);

        int sleep_well;
    protected:
        XPtr<XCreature> personal_enemy[ENEMY_LIST_SIZE];


        int FindPath(XPoint * target, XPoint * direction);
        int AttackEnemy(int x, int y);
        int CastSpell();
        int Shoot();
        int ReadScroll();
        int DrinkPotion();
        int PickUpItems();

        int MoveTo(int x, int y, XLocation * l = NULL);



        int TryToRunAway(); //creature tryes to run away from attacker... if can

        bool CanMoveHere(int px, int py); //can move here without risk of attaking friendly creature

        AI_FLAG ai_flag;
        XPtr<XCreature> ai_owner;
        CREATURE_CLASS enemy_class;

        XRect guard_area;
        LOCATION guard_area_location;


        XCreature* enemy; // current targeted creature
        int enemy_dist; // distance to the closest enemy

        XPtr<XCreature> last_enemy;
        XMapObject* last_moved_way; //used to prevent up/down moving way repeating...
        int invisible_x;
        int invisible_y;
        int invisible_hunting_mode;

        int friend_avg_x;
        int friend_avg_y;
        int friends_count; //count of friends (which has same enemy) for coward attack

        int item_dist; // distance to the closest item
        int item_x; // item x coordinate
        int item_y; // item y coordinate

        int way_dist;
        int way_x;
        int way_y;
};

#endif
