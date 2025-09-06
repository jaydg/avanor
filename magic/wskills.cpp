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

#include "wskills.h"
#include "xfile.h"
#include "xapi.h"

_SKILL_DB _skill_db[WSK_EOF] = {
    // dv  hit dmg
    // r   hit dmg
    {
        "Unarmed fight",
        {0, 0, 0, 1, 1, 1, 2, 2, 3,	4,	5,	6,	7,	8,	10,	12}, // dv
        {0, 1, 2, 3, 4, 5, 6, 7, 8,	9,	10,	11,	12,	13,	14,	15}, // hit
        {0, 0, 0, 0, 1, 1, 1, 2, 2,	3,	3,	4,	5,	7,	9,	12}  // dmg
    },
    {
        "Daggers & Knives",
        {0, 0, 0, 1, 1, 1, 1, 2, 2,	2,	2,	3,	3,	4,	5,	6},  // dv
        {0, 1, 2, 3, 4, 5, 6, 7, 8,	9,	10,	11,	12,	14,	16,	19}, // hit
        {0, 0, 0, 1, 1, 2, 2, 3, 3,	4,	4,	5,	5,	6,	6,	8}   // dmg
    },
    {
        "Swords",
        {0, 0, 1, 2, 2, 3, 3, 4, 4,	5,	6,	7,	8,	9,	10,	12}, // dv
        {0, 1, 1, 1, 2, 2, 2, 3, 3,	4,	4,	5,	6,	8,	10,	12}, // hit
        {0, 0, 1, 1, 2, 2, 3, 3, 4,	4,	5,	6,	8,	10,	12,	14}  // dmg
    },
    {
        "Clubs & Hammers",
        {0, 0, 1, 2, 2, 3, 3, 4, 4,	5,	6,	7,	8,	9,	10,	12}, // dv
        {0, 1, 1, 1, 2, 2, 2, 3, 3,	4,	4,	5,	6,	8,	10,	12}, // hit
        {0, 0, 1, 1, 2, 2, 3, 3, 4,	4,	5,	6,	8,	10,	12,	14}  // dmg
    },
    {
        "Maces & Flails",
        {0, 0, 1, 2, 2, 3, 3, 4, 4,	5,	6,	7,	8,	9,	10,	12}, // dv
        {0, 1, 1, 1, 2, 2, 2, 3, 3,	4,	4,	5,	6,	8,	10,	12}, // hit
        {0, 0, 1, 1, 2, 2, 3, 3, 4,	4,	5,	6,	8,	10,	12,	14}  // dmg
    },
    {
        "Pole Arms",
        {0, 0, 1, 2, 2, 3, 3, 4, 4,	5,	6,	7,	8,	9,	10,	12}, // dv
        {0, 1, 1, 1, 2, 2, 2, 3, 3,	4,	4,	5,	6,	8,	10,	12}, // hit
        {0, 0, 1, 1, 2, 2, 3, 3, 4,	4,	5,	6,	8,	10,	12,	14}  // dmg
    },
    {
        "Axes",
        {0, 0, 1, 2, 2, 3, 3, 4, 4,	5,	6,	7,	8,	9,	10,	12}, // dv
        {0, 1, 1, 1, 2, 2, 2, 3, 3,	4,	4,	5,	6,	8,	10,	12}, // hit
        {0, 0, 1, 1, 2, 2, 3, 3, 4,	4,	5,	6,	8,	10,	12,	14}  // dmg
    },
    {
        "Staves",
        {0, 0, 1, 2, 2, 3, 3, 4, 4,	5,	6,	7,	8,	9,	10,	12}, // dv
        {0, 1, 1, 1, 2, 2, 2, 3, 3,	4,	4,	5,	6,	8,	10,	12}, // hit
        {0, 0, 1, 1, 2, 2, 3, 3, 4,	4,	5,	6,	8,	10,	12,	14}  // dmg
    },
    {
        "Shields",
        {0, 1, 2, 3, 5, 7, 9, 11, 12, 14, 16, 18, 20, 23, 26, 30}, // dv
        {0, 0, 0, 0, 0, 0, 0, 0, 0,	0,	0,	0,	0,	0,	0,	0},    // hit
        {0, 0, 0, 0, 0, 0, 0, 0, 0,	0,	0,	0,	0,	0,	0,	0}     // dmg
    },
    {
        "Bows",
        {0, 0, 1, 1, 1, 1, 2, 2, 2,	2,	3,	3,	3,	3,	4,	5},  // r
        {0, 1, 2, 3, 4, 5, 6, 7, 8,	9,	10,	12,	14,	16,	18,	22}, // hit
        {0, 0, 1, 1, 2, 2, 3, 3, 4,	4,	5,	6,	8,	10,	12,	14}  // dmg
    },
    {
        "Crossbows",
        {0, 0, 0, 0, 0, 0, 0, 1, 1,	1,	1,	1,	1,	2,	2,	3},  // r
        {0, 1, 2, 3, 4, 5, 6, 7, 8,	9,	10,	12,	14,	16,	18,	22}, // hit
        {0, 1, 2, 3, 4, 5, 6, 7, 8,	9,	10,	11,	12,	13,	14,	16}  // dmg
    },
    {
        "Slings",
        {0, 1, 1, 1, 2, 2, 2, 3, 3,	3,	4,	4,	4,	5,	5,	6},  // r
        {0, 1, 2, 3, 4, 5, 6, 7, 8,	9,	10,	12,	14,	16,	18,	22}, // hit
        {0, 0, 0, 0, 1, 1, 1, 1, 2,	2,	2,	3,	3,	4,	4,	6}  // dmg
    },
    {
        "Throwing items",
        {0, 1, 1, 1, 1, 1, 2, 2, 2,	2,	3,	3,	3,	4,	4,	5},  // r
        {0, 1, 2, 3, 4, 5, 6, 7, 8,	9,	10,	12,	14,	16,	18,	22}, // hit
        {0, 0, 0, 0, 1, 1, 1, 1, 2,	2,	2,	3,	3,	4,	4,	6}   // dmg
    }
};

XWarSkills::XWarSkills()
{
    for (int i = 0; i < WSK_EOF; i++) {
        marks_counter[i] = GetN(1);
        levels[i] = 0;
    }
}

const char* XWarSkills::GetName(WSK_TYPE wt)
{
    return _skill_db[wt].name;
}

int XWarSkills::GetLevel(WSK_TYPE wt)
{
    return levels[wt];
}

int XWarSkills::GetN(int level)
{
    //30, 26
    return 10 * vRound((float)(25.0 * (pow(M_E, level / 4.0) - 1)));
}

void XWarSkills::SetLevel(WSK_TYPE wt, int level)
{
    marks_counter[wt] = GetN(level + 1);
    levels[wt] = level;
}

void XWarSkills::UseSkill(WSK_TYPE wt, int time)
{
    marks_counter[wt] -= time * 10;

    if (marks_counter[wt] <= 0) {
        if (levels[wt] < 15) {
            levels[wt]++;
            marks_counter[wt] += GetN(levels[wt]);
        }
    }
}

int XWarSkills::GetDV(WSK_TYPE wt)
{
    return _skill_db[wt].base_dv[levels[wt]];
}

int XWarSkills::GetHIT(WSK_TYPE wt)
{
    return _skill_db[wt].base_hit[levels[wt]];
}

int XWarSkills::GetDMG(WSK_TYPE wt)
{
    return _skill_db[wt].base_dmg[levels[wt]];
}

int XWarSkills::GetUseTime(WSK_TYPE wt)
{
    return 1000 - GetLevel(wt) * 30;
}

void XWarSkills::Store(XFile * f)
{
    f->Write(marks_counter, sizeof(int), WSK_EOF);
    f->Write(levels, sizeof(int), WSK_EOF);
}

void XWarSkills::Restore(XFile * f)
{
    f->Read(marks_counter, sizeof(int), WSK_EOF);
    f->Read(levels, sizeof(int), WSK_EOF);
}
