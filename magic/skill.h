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

#ifndef SKILL_H
#define SKILL_H

struct SKILL_DB {
    const char* name;
    int use_per_level;
};

class XCreature;

#define SKILL_MAX_LEVEL 15

enum SKILL_MASTERY {
    SM_NONE	= 0,
    SM_BASIC	= 1,
    SM_SKILLED	= 2,
    SM_EXPERT	= 3,
    SM_MASTER	= 4,
    SM_GRANDMASTER	= 5,
};

class XSkill
{
        int UseSteal(XCreature * user);
        int UseDisarm(XCreature * user);
        int UseCreate(XCreature * user);
    public:
        enum Skill {
            ARCHERY,
            FINDWEAKNESS,
            HEALING,       // restore hit points
            CONCENTRATION,
            DODGE,
            TRADING,
            STEALING,
            LITERACY,
            DETECTTRAP,
            DISARMTRAP,
            COOKING,
            MINING,
            HERBALISM,
            RELIGION,
            BACKSTABBING,  // attacking creature which haven't wait attack from you
            FIRST_AID,     // cure wounds
            TACTICS,
            ALCHEMY,
            WOODCRAFT,
            CREATETRAP,
            NECROMANCY,
            ATHLETICS,
            CLIMBING,      // at this time from pits only...
        };

        XSkill() = delete;

        explicit XSkill(Skill _skt, int _level = 1);
        [[nodiscard]] Skill GetSkill() const
        {
            return skt;
        }

        const char* GetName();
        const char* GetSkillLevel();

        // called by monster when this skill was used successfully
        void UseSkill(int n = 1)
        {
            used_time += n;
        }

        int Use(XCreature * user);
        int isUseable() const;
        int IncLevel();
        int GetLevel() const
        {
            return level;
        }

        int GetMaxLevel();
        SKILL_MASTERY GetMastery();

        Skill skt;

    protected:
        int used_time;
        int	level;
};

#endif
