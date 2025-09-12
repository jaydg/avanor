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

#ifndef __WSKILLS_H
#define __WSKILLS_H

#include <cmath>
#include <cstdlib>

#include "engine/defs.h"
#include "engine/global.h"
#include "item/itemdef.h"

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
        XWarSkills();
        int GetLevel(WSK_TYPE wt);
        int GetN(int level);
        int GetDV(WSK_TYPE wt);
        int GetHIT(WSK_TYPE wt);
        int GetDMG(WSK_TYPE wt);
        int GetUseTime(WSK_TYPE wt);
        const char* GetName(WSK_TYPE wt);
        int GetMarks(WSK_TYPE wt)
        {
            return marks_counter[wt] / 10;
        }

        void UseSkill(WSK_TYPE wt, int time = 1);

        void SetLevel(WSK_TYPE wt, int level);

        void Store(XFile * f);
        void Restore(XFile * f);

        int marks_counter[WSK_EOF];
        int levels[WSK_EOF];
};

#endif
