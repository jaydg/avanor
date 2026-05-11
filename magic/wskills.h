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

#ifndef WSKILLS_H
#define WSKILLS_H

struct _SKILL_DB {
    const char* name;
    int base_dv[16];
    int base_hit[16];
    int	base_dmg[16];
};

class XFile;

class XWarSkills
{
    public:
        enum Type {
            UNKNOWN = -1,
            UNARMED = 0,
            DAGGER,  // knives, daggers
            SWORD,   // all swords
            CLUB,    // all clubs, hammers
            MACE,    // all maces, flails
            POLEARM, // all pole arm, spears, pike etc.
            AXE,     // all axes
            STAVE,   // staves, staffs
            SHIELD,  // shields
            BOW,
            CROSSBOW,
            SLING,
            THROW,
            ALL,
            OTHER
        };

        XWarSkills();
        int GetLevel(Type wt);
        int GetN(int level);
        int GetDV(Type wt);
        int GetHIT(Type wt);
        int GetDMG(Type wt);
        int GetUseTime(Type wt);
        const char* GetName(Type wt);
        int GetMarks(Type wt)
        {
            return marks_counter[wt] / 10;
        }

        void UseSkill(Type wt, int time = 1);

        void SetLevel(Type wt, int level);

        void Store(XFile * f);
        void Restore(XFile * f);

        int marks_counter[ALL];
        int levels[ALL];
};

#endif
