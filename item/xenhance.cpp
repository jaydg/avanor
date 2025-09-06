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

#include "xenhance.h"

REGISTER_CLASS(XEnhance);

_ENH_COLOR_TABLE ect[] = {
    {/*EC_WHITE,*/	xWHITE,	"white"},
    {/*EC_BLUE,*/	xBLUE,	"blue"},
    {/*EC_BLACK,*/	xDARKGRAY,	"black"},
    {/*EC_GOLDEN,*/	xYELLOW,	"golden"},
    {/*EC_COPPER,*/	xRED,	"copper"},
    {/*EC_WOODEN,*/	xBROWN,	"wooden"},
    {/*EC_RED,*/	xRED,	"red"},
    {/*EC_MARBLE,*/ xWHITE,	"marble"},
    {/*EC_PERAL,*/	xYELLOW,	"pearl"},
    {/*EC_SAPHIRE,*/xLIGHTGREEN, "sapphire"},
    {/*EC_DIAMOND*/ xCYAN,	"diamond"},
    {/*EC_RUBY,*/	xRED,	"ruby"},
    {/*EC_STEEL,*/	xBLUE,	"steel"},
    {/*EC_GLASS,*/	xLIGHTGRAY,	"glass"},
    {/*EC_OBSIDIAN*/xDARKGRAY,	"obsidian"},
};

ENH_COLOR ENH_REC::SelectColor(ENH_COLOR ec)
{
    if (ec == EC_RANDOM) {
        int count = 1000;

        while (count-- > 0) {
            int rp = vRand() % EC_RANDOM;

            if (ect[rp].is_used == 0) {
                ect[rp].is_used = 1;
                return (ENH_COLOR)rp;
            }
        }

        assert(0);
        return EC_WHITE;
    } else {
        assert(ect[ec].is_used == 0);
        ect[ec].is_used = 1;
        return ec;
    }
}

ENH_REC enh_db[] = {
    {
        ENH_PROTECTION, "of protection",
        "", "1d6-2",
        "", "", "", "",
        "",
        "",
        150, ENH_REC::SelectColor(EC_RANDOM), 0
    },
    {
        ENH_DAMAGE, "of damage",
        "", "",
        "", "", "1d6-2", "",
        "",
        "",
        150, ENH_REC::SelectColor(EC_RANDOM), 0
    },
    {
        ENH_SLAYING, "of slaying",
        "", "",
        "1d6-2", "", "1d6-2", "",
        "",
        "",
        300, ENH_REC::SelectColor(EC_RANDOM), 0
    },
    {
        ENH_FREEACTION, "of free action",
        "", "",
        "", "", "", "",
        "stun:8d5+50 confuse:8d5+50",
        "",
        200, ENH_REC::SelectColor(EC_RANDOM), 0
    },
    {
        ENH_INVISIBILITY, "of invisibility",
        "", "",
        "", "", "", "",
        "invisible:0d0+10",
        "",
        500, ENH_REC::SelectColor(EC_RANDOM), 0
    },
    {
        ENH_SEEINVISIBLE, "of see invisible",
        "", "",
        "", "", "", "",
        "see_invisible:0d0+10",
        "",
        300, ENH_REC::SelectColor(EC_RANDOM), 0
    },
    {
        ENH_FIRERESIST, "of fire resistance",
        "", "",
        "", "", "", "",
        "fire:8d5+30",
        "",
        250, ENH_REC::SelectColor(EC_RANDOM), 0
    },
    {
        ENH_ACIDRESIST, "of acid resistance",
        "", "",
        "", "", "", "",
        "fire:8d5+25",
        "",
        300, ENH_REC::SelectColor(EC_RANDOM), 0
    },
    {
        ENH_POISONRESIST, "of poison resistance",
        "", "",
        "", "", "", "",
        "fire:8d5+25",
        "",
        300, ENH_REC::SelectColor(EC_RANDOM), 0
    },
    {
        ENH_STR, "of Strength",
        "", "",
        "", "", "", "",
        "",
        "St:1d4",
        400, ENH_REC::SelectColor(EC_RANDOM), 0
    },
    {
        ENH_POWER, "of Power",
        "", "",
        "", "", "", "",
        "",
        "Wi:1d4",
        400, ENH_REC::SelectColor(EC_RANDOM), 0
    }
};

const int enh_db_sz = 11;

XEnhance::XEnhance(ENHANCE enh)
{
    descr = -1;

    if (enh == ENH_EOF) {
        descr =	vRand(enh_db_sz);
    } else {
        for (int i = 0; i < enh_db_sz; i++)
            if (enh_db[i].enh == enh) {
                descr = i;
                break;
            }
    }

    assert(descr > - 1);

    ENH_REC * is = &enh_db[descr];

    color =	ect[is->color].color;
    value =	is->value;
    weight = 1;
    XDice * d;
    d = new XDice(is->dv);
    _DV = d->Throw();

    d->Setup(is->pv);
    _PV = d->Throw();

    d->Setup(is->hit);
    _HIT = d->Throw();

    d->Setup(is->dice);
    int tx = d->X;
    int ty = d->Y;

    d->Setup(is->z);
    dice.Setup(tx, ty, d->Throw());

    d->Setup(is->rng);
    RNG = d->Throw();
    delete d;
    s = new XStats(is->s);
    r = new XResistance(is->r);

    weight = 3;
}

int XEnhance::Compare(XObject * o)
{
    assert(dynamic_cast<XEnhance*>(o));

    XEnhance * tit = (XEnhance*)o;

    if (descr == tit->descr && XItem::Compare(o) == 0) {
        return 0;
    } else {
        return 1;
    }
}

void XEnhance::toString(char* buf)
{
    if (!isIdentifed()) {
        if (quantity == 1) {
            sprintf(buf, "%s %s", ect[descr].color_name, name);
        } else {
            sprintf(buf, "heap of (%d) %s %ss", quantity, ect[descr].color_name, name);
        }

        return;
    }

    if (quantity == 1) {
        sprintf(buf, "%s %s", name, enh_db[descr].name);
    } else {
        sprintf(buf, "heap of (%d) %ss %s", quantity, name, enh_db[descr].name);
    }

    char tbuf[256];

    if (RNG != 0) {
        sprintf(tbuf, "<%+d>", RNG);
        strcat(buf, tbuf);
    }

    if (dice.Z != 0 && _HIT != 0) {
        sprintf(tbuf, "(%+d, %+d)", _HIT, dice.Z);
        strcat(buf, tbuf);
    } else {
        if (_HIT != 0) {
            sprintf(tbuf, "(%+d)", _HIT);
            strcat(buf, tbuf);
        }

        if (dice.Z != 0) {
            sprintf(tbuf, "(%+d)", dice.Z);
            strcat(buf, tbuf);
        }
    }

    if (_DV != 0 && _PV != 0) {
        sprintf(tbuf, "[%+d, %+d]", _DV, _PV);
        strcat(buf, tbuf);
    } else {
        if (_DV != 0) {
            sprintf(tbuf, "[%+d]", _DV);
            strcat(buf, tbuf);
        }

        if (_PV != 0) {
            sprintf(tbuf, "[%+d]", _PV);
            strcat(buf, tbuf);
        }
    }

    StatsToString(tbuf);
    strcat(buf, tbuf);
}

void XEnhance::Store(XFile * f)
{
    XItem::Store(f);
    f->Write(&descr, sizeof(int));
}

void XEnhance::Restore(XFile * f)
{
    XItem::Restore(f);
    f->Read(&descr, sizeof(int));
}
