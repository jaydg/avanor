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

#ifndef XCORPSE_H
#define XCORPSE_H

#include "item/xanyfood.h"

enum CORPSE_CONDITION {
    CCOND_NICE,
    CCOND_NORMAL,
    CCOND_SROTED,
    CCOND_ROTED,
    CCOND_VROTED,
};

enum CORPSE_FLAG {
    CF_RAW,
    CF_DRY,
    CF_COOKED,
    CF_ROASTED,
    CF_FRIZED,
};

enum CORPSE_EFFECT_TYPE {
    CET_MODIFY_ST,
    CET_MODIFY_TO,
    CET_MODIFY_MA,
    CET_MODIFY_R_FIRE,
    CET_MODIFY_R_COLD,
    CET_MODIFY_R_ACID,
    CET_MODIFY_R_POISON,
    CET_MODIFY_R_PARALYSE,
    CET_MODIFY_STOMACH,
    CET_POISON,
    CET_DISEASE,
    CET_PARALYSE,
    CET_CONFUSE,
    CET_VOMIT,
    CET_SATIATION,
};

struct CORPSE_EFFECT {
    CORPSE_EFFECT_TYPE type;
    int value;
};

struct CORPSE_DATA {
    CORPSE_DATA() : roating_time(100), ft(FT_NORMALFOOD) {}

    int roating_time;
    FOOD_TYPE ft;
    XQList<CORPSE_EFFECT> effect;
};

class XCorpse : public XAnyFood
{
    protected:
        int time_of_roating;
        CREATURE_NAME cn; //need for correct restoration of corpse;
        CORPSE_DATA* pCorpseData;
    public:
        DECLARE_CREATOR(XCorpse, XAnyFood);
        XCorpse(XCreature * corpse_owner, const CORPSE_DATA * pData, CORPSE_FLAG cf = CF_RAW);
        XCorpse(XCorpse * copy);
        XCorpse()
        {
            assert(0);
        }

        virtual XObject* MakeCopy()
        {
            return new XCorpse(this);
        }

        virtual RESULT onEat(XCreature * eater);
        int Run();
        virtual void Store(XFile * f);
        virtual void Restore(XFile * f);
        virtual const char* postEat(XCreature * eater);
        virtual int Compare(XObject * o)
        {
            return -1;
        }

        virtual int GetValue();
        virtual void toString(char* buf);
        CORPSE_CONDITION GetCondition();
        void Cook()
        {
            corpse_flag = CF_COOKED;
        }

        int corpse_flag;
        int roating_stopped; //flag for stoping of roating during Cooking
};

#endif
