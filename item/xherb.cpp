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

#include "creature/creature.h"
#include "game/game.h"
#include "helpers/msgwin.h"
#include "item/xherb.h"
#include "item/xpotion.h"
#include "magic/modifer.h"

#define HERBS_COUNT 18

_HERBS herbs[] = {
    {"valeriana root",	"valeriana",	"sedative",	xGREEN,	HT_HERB,	PN_UNKNOWN,	0, 0},
    {"stellaria leave",	"stellaria",	"strange",	xGREEN,	HT_HERB,	PN_UNKNOWN,	0, 0},
    {"trifolium leave",	"trifolium",	"grassy",	xGREEN,	HT_HERB,	PN_UNKNOWN,	0, 0},
    {"trifolium flower",	"trifolium",	"sweet",	xGREEN,	HT_HERB,	PN_UNKNOWN,	0, 0},
    {"urtica leave",	"urtica",	"bitter",	xGREEN,	HT_HERB,	PN_UNKNOWN,	0, 0},
    {"convallaria flower",	"convallaria",	"sweet",	xGREEN,	HT_HERB,	PN_UNKNOWN,	0, 0},
    {"tussilago farfara leave",	"tussilago farfara",	"bitter",	xGREEN,	HT_HERB,	PN_UNKNOWN,	0, 0},
    {"melissa leave",	"melissa",	"delicate",	xGREEN,	HT_HERB,	PN_UNKNOWN,	0, 0},
    {"mentha leave",	"mentha",	"mint",	xGREEN,	HT_HERB,	PN_UNKNOWN,	0, 0},
    {"taraxacum flower",	"taraxacum",	"sweet",	xGREEN,	HT_HERB,	PN_UNKNOWN,	0, 0},
    {"paeonia root",	"paeonia",	"bitter",	xGREEN,	HT_HERB,	PN_UNKNOWN,	0, 0},
    {"plantago leave",	"plantago",	"bitter",	xGREEN,	HT_HERB,	PN_UNKNOWN,	0, 0},
    {"chamomilla flower",	"chamomilla",	"sweet",	xGREEN,	HT_HERB,	PN_UNKNOWN,	0, 0},

    {"red mushroom",	"red mushroom",	"bitter",	xRED,	HT_MUSHROOM,	PN_UNKNOWN,	0, 0},
    {"green mushroom",	"green mushroom",	"bitter",	xGREEN,	HT_MUSHROOM,	PN_UNKNOWN,	0, 0},
    {"blue mushroom",	"blue mushroom",	"bitter",	xBLUE,	HT_MUSHROOM,	PN_UNKNOWN,	0, 0},
    {"yellow mushroom",	"yellow mushroom",	"bitter",	xYELLOW,	HT_MUSHROOM,	PN_UNKNOWN,	0, 0},
    {"white mushroom",	"white mushroom",	"bitter",	xWHITE,	HT_MUSHROOM,	PN_UNKNOWN,	0, 0},
};

void _HERBS::Create()
{
    for (int i = 0; i < HERBS_COUNT; i++) {
        while (1) {
            POTION_REC * pr = POTION_REC::GetRec((POTION_NAME)vRand(PN_RANDOM));

            // check if there was such potion already
            bool continue_flag = false;

            for (int j = 0; j < i; j++) {
                if (herbs[j].pn == pr->pn) {
                    continue_flag = true;
                    break;
                }
            }

            if (continue_flag) {
                continue;
            }

            if (herbs[i].herb_type == HT_HERB) {
                if (pr->alchemy_power == 1) {
                    herbs[i].pn = pr->pn;
                    herbs[i].difficulty = vRand(4) + 1;
                    break;
                } else if (pr->alchemy_power == 2) {
                    herbs[i].pn = pr->pn;
                    herbs[i].difficulty = vRand(4) + 4;
                    break;
                }
            } else {
                if (pr->alchemy_power == 3) {
                    herbs[i].pn = pr->pn;
                    herbs[i].difficulty = vRand(4) + 7;
                    break;
                }
            }
        }
    }
}

void _HERBS::Store(XFile * f)
{
    for (int i = 0; i < HERBS_COUNT; i++) {
        f->Write(&herbs[i].pn, sizeof(POTION_NAME));
        f->Write(&herbs[i].difficulty, sizeof(herbs[i].difficulty));
        f->Write(&herbs[i].identify, sizeof(herbs[i].identify));
    }
}

void _HERBS::Restore(XFile * f)
{
    for (int i = 0; i < HERBS_COUNT; i++) {
        f->Read(&herbs[i].pn, sizeof(POTION_NAME));
        f->Read(&herbs[i].difficulty, sizeof(herbs[i].difficulty));
        f->Read(&herbs[i].identify, sizeof(herbs[i].identify));
    }
}

REGISTER_CLASS(XHerb);

XHerb::XHerb(int _herb_index) : herb_index(_herb_index), XAnyFood()
{
    weight = 3;
    food_nutrio = 15;
    strcpy(name, herbs[herb_index].herb_name);
    it = IT_HERB;
}

RESULT XHerb::onEat(XCreature * eater)
{
    if (XAnyFood::onEat(eater)) {
        return SUCCESS;
    }

    return FAIL;
}

POTION_NAME XHerb::GetTargetPotion()
{
    return herbs[herb_index].pn;
}

const char* XHerb::postEat(XCreature * eater)
{
    return herbs[herb_index].post_eat;
}

void XHerb::toString(char* buf)
{
    if (herbs[herb_index].identify) {
        strcpy(name, herbs[herb_index].herb_name);
    } else {
        if (herbs[herb_index].herb_type == HT_HERB) {
            strcpy(name, "unknown herb");
        } else {
            strcpy(name, "unknown mushroom");
        }
    }

    XAnyFood::toString(buf);
}

void XHerb::Identify(int level)
{
    herbs[herb_index].identify = 1;
}

int XHerb::isIdentifed()
{
    return herbs[herb_index].identify;
}

////////////////////////////////////////////////////////////
// XHerbBush
////////////////////////////////////////////////////////////

REGISTER_CLASS(XHerbBush);

XHerbBush::XHerbBush(int _x, int _y, XLocation * _l)
{
    l = _l;
    im = IM_OTHER;
    x = _x;
    y = _y;
    ttm = 1;
    herb_strength = 1;

    do {
        herb_index = vRand(HERBS_COUNT);
    } while (herbs[herb_index].herb_type != HT_HERB);

    view = '"';
    color = herbs[herb_index].color;

    strcpy(name, herbs[herb_index].herb_name);
    assert(l->map->GetSpecial(x, y) == NULL);
    l->map->SetSpecial(x, y, this);
    Game.Scheduler.Add(this);
}

const char* XHerbBush::GetName(XCreature * viewer)
{
    XSkill * xsk = viewer->sk->GetSkill(XSkill::Skill::HERBALISM);
    int val = 0;

    if (xsk) {
        val += xsk->GetLevel();
    }

    _HERBS * herb_data = &herbs[herb_index];

    const char* size_name = "";

    switch (herb_strength) {
        case 1:
            size_name = "small";
            break;

        case 2:
            size_name = "medium";
            break;

        case 3:
            size_name = "large";
            break;
    }

    if (herb_data->difficulty > val && !herb_data->identify) {
        sprintf(static_buffer, "%s bush of unknown herbs", size_name);
    } else {
        herb_data->identify = 1;
        sprintf(static_buffer, "%s bush of %s", size_name, herb_data->bush_name);
    }

    return static_buffer;
}

int XHerbBush::CountNeighbours(int x, int y)
{
    int N = 0;

    for (int i = x - 1; i <= x + 1; i++)
        for (int j = y - 1; j <= y + 1; j++) {
            if (i == x && j == y) {
                continue;
            }

            XMapObject * obj = l->map->GetSpecial(i, j);

            if (obj == 0 || !obj->isValid() || obj->im != IM_HERB) {
                continue;
            }

            N++;
        }

    return N;
}

int XHerbBush::Run()
{
    assert(isValid());

    int N = CountNeighbours(x, y);

    if (N < 2 || N > 3) {
        if (--herb_strength <= 0) {
            l->map->SetSpecial(x, y, NULL);
            Invalidate();
            return 0;
        }
    }

    if (N == 3 && herb_strength < 3) {
        herb_strength++;
    }

    for (int i = x - 1; i <= x + 1; i++)
        for (int j = y - 1; j <= y + 1; j++) {
            XMapObject * obj = l->map->GetSpecial(i, j);

            if (obj != 0 || l->map->GetXY(i, j) != M_GREENGRAS) {
                continue;
            }

            if (CountNeighbours(i, j) != 3) {
                continue;
            }

            new XHerbBush(i, j, l);
        }

    if (ttm <= 0 && isValid()) {
        ttm += vRand(BASE_HERB_REFRESH);
    }

    return 1;
}

void XHerbBush::Store(XFile * f)
{
    XMapObject::Store(f);
    f->Write(&herb_strength, sizeof(herb_strength));
    f->Write(&herb_index, sizeof(herb_index));
}

void XHerbBush::Restore(XFile * f)
{
    XMapObject::Restore(f);
    f->Read(&herb_strength, sizeof(herb_strength));
    f->Read(&herb_index, sizeof(herb_index));
}

XObject* XHerbBush::Pick(XCreature * picker)
{
    picker->sk->UseSkill(XSkill::Skill::HERBALISM);

    if (--herb_strength <= 0) {
        l->map->SetSpecial(x, y, NULL);
        Invalidate();
    }

    return new XHerb(herb_index);
}

////////////////////////////////////////////////////////////
// XMushSpawn
////////////////////////////////////////////////////////////

REGISTER_CLASS(XMushSpawn);

XMushSpawn::XMushSpawn(int _x, int _y, XLocation * _l)
{
    l = _l;
    im = IM_OTHER;
    x = _x;
    y = _y;
    ttm = vRand(BASE_MUSH_REFRESH);

    do {
        mush_index = vRand(HERBS_COUNT);
    } while (herbs[mush_index].herb_type != HT_MUSHROOM);

    view = '`';
    color = herbs[mush_index].color;

    strcpy(name, herbs[mush_index].herb_name);
    assert(l->map->GetSpecial(x, y) == NULL);
    l->map->SetSpecial(x, y, this);
    Game.Scheduler.Add(this);
}

const char* XMushSpawn::GetName(XCreature * viewer)
{
    _HERBS * herb_data = &herbs[mush_index];
    return herb_data->bush_name;
}

int XMushSpawn::Run()
{
    assert(isValid());

    if (ttm <= 0 && isValid()) {
        if (isVisible()) {
            msgwin.Add("Suddenly mushroom dissapered in the small cloud of spores.");
        }

        l->map->SetSpecial(x, y, NULL);
        Invalidate();
        return 0;
    }

    return 1;
}

void XMushSpawn::Store(XFile * f)
{
    XMapObject::Store(f);
    f->Write(&mush_index, sizeof(mush_index));
}

void XMushSpawn::Restore(XFile * f)
{
    XMapObject::Restore(f);
    f->Read(&mush_index, sizeof(mush_index));
}

XObject* XMushSpawn::Pick(XCreature * picker)
{
    picker->sk->UseSkill(XSkill::Skill::HERBALISM);
    l->map->SetSpecial(x, y, NULL);
    Invalidate();
    XHerb * it = new XHerb(mush_index);
    it->Identify(1);
    return it;
}
