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

#include <fmt/format.h>

#include "item/xenhance.h"

REGISTER_CLASS(XEnhance);

enum ENH_COLOR {
    EC_WHITE,
    EC_BLUE,
    EC_BLACK,
    EC_GOLDEN,
    EC_COPPER,
    EC_WOODEN,
    EC_RED,
    EC_MARBLE,
    EC_PEARL,
    EC_SAPPHIRE,
    EC_DIAMOND,
    EC_RUBY,
    EC_STEEL,
    EC_GLASS,
    EC_OBSIDIAN,
    EC_RANDOM,
};

struct ENH_COLOR_TABLE {
    int color;
    const char* color_name;
    int is_used;
} ect[] = {
    {/* EC_WHITE */     xWHITE,      "white"},
    {/* EC_BLUE */      xBLUE,       "blue"},
    {/* EC_BLACK */     xDARKGRAY,   "black"},
    {/* EC_GOLDEN */    xYELLOW,     "golden"},
    {/* EC_COPPER */    xRED,        "copper"},
    {/* EC_WOODEN */    xBROWN,      "wooden"},
    {/* EC_RED */       xRED,        "red"},
    {/* EC_MARBLE */    xWHITE,      "marble"},
    {/* EC_PEARL */     xYELLOW,     "pearl"},
    {/* EC_SAPPHIRE */  xLIGHTGREEN, "sapphire"},
    {/* EC_DIAMOND */   xCYAN,       "diamond"},
    {/* EC_RUBY */      xRED,        "ruby"},
    {/* EC_STEEL */     xBLUE,       "steel"},
    {/* EC_GLASS */     xLIGHTGRAY,  "glass"},
    {/* EC_OBSIDIAN */  xDARKGRAY,   "obsidian"},
};

struct ENH_REC {
    XEnhance::Type enh;
    const char* name;
    const char* dv;     // modifiers
    const char* pv;
    const char* hit;
    const char* dice;
    const char* z;      // random z to dice;
    const char* rng;    // rng
    const char* r;      // resists
    const char* s;      // stats
    int value;
    ENH_COLOR color;
    int identify;
    static ENH_COLOR SelectColor(ENH_COLOR ec);
} enh_db[] = {
    {
        XEnhance::PROTECTION, "of protection",
        "", "1d6-2",
        "", "", "", "",
        "",
        "",
        150, ENH_REC::SelectColor(EC_RANDOM), 0
    },
    {
        XEnhance::DAMAGE, "of damage",
        "", "",
        "", "", "1d6-2", "",
        "",
        "",
        150, ENH_REC::SelectColor(EC_RANDOM), 0
    },
    {
        XEnhance::SLAYING, "of slaying",
        "", "",
        "1d6-2", "", "1d6-2", "",
        "",
        "",
        300, ENH_REC::SelectColor(EC_RANDOM), 0
    },
    {
        XEnhance::FREEACTION, "of free action",
        "", "",
        "", "", "", "",
        "stun:8d5+50 confuse:8d5+50",
        "",
        200, ENH_REC::SelectColor(EC_RANDOM), 0
    },
    {
        XEnhance::INVISIBILITY, "of invisibility",
        "", "",
        "", "", "", "",
        "invisible:0d0+10",
        "",
        500, ENH_REC::SelectColor(EC_RANDOM), 0
    },
    {
        XEnhance::SEEINVISIBLE, "of see invisible",
        "", "",
        "", "", "", "",
        "see_invisible:0d0+10",
        "",
        300, ENH_REC::SelectColor(EC_RANDOM), 0
    },
    {
        XEnhance::FIRERESIST, "of fire resistance",
        "", "",
        "", "", "", "",
        "fire:8d5+30",
        "",
        250, ENH_REC::SelectColor(EC_RANDOM), 0
    },
    {
        XEnhance::ACIDRESIST, "of acid resistance",
        "", "",
        "", "", "", "",
        "fire:8d5+25",
        "",
        300, ENH_REC::SelectColor(EC_RANDOM), 0
    },
    {
        XEnhance::POISONRESIST, "of poison resistance",
        "", "",
        "", "", "", "",
        "fire:8d5+25",
        "",
        300, ENH_REC::SelectColor(EC_RANDOM), 0
    },
    {
        XEnhance::STRENGTH, "of Strength",
        "", "",
        "", "", "", "",
        "",
        "St:1d4",
        400, ENH_REC::SelectColor(EC_RANDOM), 0
    },
    {
        XEnhance::POWER, "of Power",
        "", "",
        "", "", "", "",
        "",
        "Wi:1d4",
        400, ENH_REC::SelectColor(EC_RANDOM), 0
    }
};

ENH_COLOR ENH_REC::SelectColor(const ENH_COLOR ec)
{
    if (ec == EC_RANDOM) {
        int count = 1000;

        while (count-- > 0) {
            int rp = vRand() % EC_RANDOM;

            if (ect[rp].is_used == 0) {
                ect[rp].is_used = 1;
                return static_cast<ENH_COLOR>(rp);
            }
        }

        assert(0);
        return EC_WHITE;
    }

    assert(ect[ec].is_used == 0);
    ect[ec].is_used = 1;

    return ec;
}

constexpr int enh_db_sz = 11;

XEnhance::XEnhance(const Type enh)
{
    descr = -1;

    if (enh == RANDOM) {
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
    auto d = new XDice(is->dv);
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
    stats = std::make_unique<XStats>(is->s);
    resistances = std::make_unique<XResistance>(is->r);

    weight = 3;
}

int XEnhance::Compare(XObject* o)
{
    auto tit = dynamic_cast<XEnhance *>(o);

    if (descr == tit->descr && XItem::Compare(o) == 0) {
        return 0;
    }

    return 1;
}

std::string XEnhance::toString()
{
    if (!isIdentifed()) {
        if (quantity == 1) {
            return fmt::format("{} {}", ect[descr].color_name, name);
        }

        return fmt::format("heap of ({}) {} {}s",
            quantity, ect[descr].color_name, name);
    }

    std::string fullname;

    if (quantity == 1) {
        fullname = fmt::format("{} {}", name, enh_db[descr].name);
    } else {
        fullname = fmt::format("heap of ({}) {}s {}",
            quantity, name, enh_db[descr].name);
    }

    if (RNG != 0) {
        fullname.append(fmt::format(" <{:+}>", RNG));
    }

    if (dice.Z != 0 && _HIT != 0) {
        fullname.append(fmt::format(" ({:+}, {:+})", _HIT, dice.Z));
    } else {
        if (_HIT != 0) {
            fullname.append(fmt::format(" ({:+})", _HIT));
        }

        if (dice.Z != 0) {
            fullname.append(fmt::format(" ({:+})", dice.Z));
        }
    }

    if (_DV != 0 && _PV != 0) {
        fullname.append(fmt::format(" [{:+}, {:+}]", _DV, _PV));
    } else {
        if (_DV != 0) {
            fullname.append(fmt::format(" [{:+}]", _DV));
        }

        if (_PV != 0) {
            fullname.append(fmt::format(" [{:+}]", _PV));
        }
    }

    fullname.append(StatsToString());

    return fullname;
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
