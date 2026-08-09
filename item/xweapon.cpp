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

#include "engine/xapi.h"
#include "item/item_cereal.h"
#include "item/xweapon.h"
#include "magic/attack_effect_type.h"

REGISTER_CLASS(XWeapon);
CEREAL_REGISTER_TYPE(XWeapon);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XWeapon);

ItemTemplate weapon_db[] = {
    {ItemType::CLUB,	"club",	'/',	"",	"",	"",	"1d3", "",	"",	ISET_WOODSTONE,	1,	10,	200,	IQ_POOR,	""},
    {ItemType::WARHAMMER,	"war hammer",	'/',	"",	"",	"1d2-1", "1d7", "1d4",	"",	ISET_HARDMETAL,	12,	10,	50,	IQ_AVG,	""},

    {ItemType::DAGGER,	"dagger",	'/',	"",	"",	"",	"1d4", "",	"",	ISET_WEAPON,	2,	3,	150,	IQ_AVG,	""},
    {ItemType::KNIFE,	"knife",	'/',	"",	"",	"1d2-2", "1d3", "",	"",	ISET_SIMPLEWEAPON,	1,	2,	200,	IQ_POOR,	""},
    {ItemType::ORCISHDAGGER, "orcish dagger", '/',	"",	"",	"1d2+1", "1d3", "1d2+2", "",	ISET_STEEL,	3,	3,	70,	IQ_AVG,	""},
    {ItemType::LONGDAGGER,	"long dagger",	'/',	"",	"",	"1d3",	"1d4", "1d2+1", "",	ISET_STEEL,	4,	3,	50,	IQ_FAIR,	""},

    {ItemType::SHORTSWORD,	"short sword",	'/',	"",	"",	"",	"1d5", "",	"",	ISET_OBSIMETAL,	6,	5,	150,	IQ_AVG,	""},
    {ItemType::LONGSWORD,	"long sword",	'|',	"",	"",	"",	"2d4", "",	"",	ISET_OBSIMETAL,	18,	10,	60,	IQ_FAIR,	""},
    {ItemType::BROADSWORD,	"broad sword",	'|',	"",	"",	"",	"2d5", "",	"",	ISET_OBSIMETAL,	20,	12,	30,	IQ_FAIR,	""},
    {ItemType::RAPIER,	"rapier",	'|',	"",	"",	"1d3+2", "1d6", "",	"",	ISET_HARDMETAL,	7,	5,	80,	IQ_FAIR,	""},
    {ItemType::SCIMITAR,	"scimitar",	'|',	"",	"",	"1d4-1", "1d6", "1d3",	"",	ISET_HARDMETAL,	7,	6,	80,	IQ_FAIR,	""},
    {ItemType::KATANA,	"katana",	'|',	"",	"",	"1d5+5", "4d2", "1d3+2", "",	ISET_HARDMETAL,	50,	1,	50,	IQ_GOOD,	""},
    {ItemType::WAKIZASHI,	"wakizashi",	'|',	"",	"",	"1d5+3", "3d2", "1d3+1", "",	ISET_HARDMETAL,	45,	1,	80,	IQ_FAIR,	""},

    {ItemType::SMALLAXE,	"small axe",	'\\',	"",	"",	"",	"1d6", "",	"",	ISET_OBSIMETAL,	10,	6,	120,	IQ_AVG,	""},
    {ItemType::WARAXE,	"war axe",	'\\',	"",	"",	"1d3+1", "2d3", "",	"",	ISET_OBSIMETAL,	15,	7,	90,	IQ_AVG,	""},
    {ItemType::BATTLEAXE,	"battle axe",	'\\',	"",	"",	"1d3+2", "2d5", "1d3+1", "",	ISET_HARDMETAL,	25,	12,	70,	IQ_FAIR,	""},
    {ItemType::GREATAXE,	"great axe",	'\\',	"",	"",	"1d5-4", "3d4", "1d4+3", "",	ISET_HARDMETAL,	30,	20,	20,	IQ_GOOD,	""},
    {ItemType::ORCISHAXE,	"orcish axe",	'\\',	"",	"",	"1d3+1", "2d4", "1d2+2", "",	ISET_OBSIMETAL,	10,	10,	90,	IQ_AVG,	""},

    {ItemType::MACE,	"mace",	'\\',	"",	"",	"",	"1d6", "",	"",	ISET_OBSIMETAL,	10,	7,	100,	IQ_AVG,	""},
    {ItemType::FLAIL,	"flail",	'\\',	"",	"",	"",	"1d7", "",	"",	ISET_HARDMETAL,	15,	7,	80,	IQ_AVG,	""},

    {ItemType::SHORTSPEAR,	"short spear",	'/',	"",	"",	"",	"1d5", "",	"",	ISET_WOODSTONE,	3,	4,	300,	IQ_POOR,	""},
    {ItemType::LONGSPEAR,	"long spear",	'/',	"",	"",	"",	"1d9", "",	"",	ISET_STEEL,	8,	6,	150,	IQ_AVG,	""},
    {ItemType::PITCHFORK,	"pitchfork",	'/',	"",	"",	"1d2-2", "1d3", "",	"",	ISET_BLACKMETAL,	1,	3,	300,	IQ_POOR,	""},
    {ItemType::PIKE,	"pike",	'/',	"",	"",	"1d3-2", "1d10", "1d4+2", "",	ISET_BLACKMETAL,	1,	3,	80,	IQ_AVG,	""},
    {ItemType::HALBERD,	"halberd",	'/',	"",	"",	"1d3-3", "1d12", "1d4+3", "",	ISET_HARDMETAL,	1,	3,	30,	IQ_GOOD,	""},

    {ItemType::STAFF,	"staff",	'/',	"",	"",	"1d2-1", "1d10", "1d2",	"", ISET_WOOD,	8,	3,	100,	IQ_AVG,	""},
};

const int weapon_db_size = 26;

XItemBasicStructure gi_weapon(weapon_db, weapon_db_size);

_WEAPON_BIND wbind[weapon_db_size] = {
    {ItemType::CLUB,           XWarSkills::CLUB},
    {ItemType::WARHAMMER,      XWarSkills::CLUB},

    {ItemType::DAGGER,         XWarSkills::DAGGER},
    {ItemType::KNIFE,          XWarSkills::DAGGER},
    {ItemType::ORCISHDAGGER,   XWarSkills::DAGGER},
    {ItemType::LONGDAGGER,     XWarSkills::DAGGER},

    {ItemType::SHORTSWORD, XWarSkills::SWORD},
    {ItemType::LONGSWORD,  XWarSkills::SWORD},
    {ItemType::BROADSWORD, XWarSkills::SWORD},
    {ItemType::RAPIER,     XWarSkills::SWORD},
    {ItemType::SCIMITAR,   XWarSkills::SWORD},
    {ItemType::KATANA,     XWarSkills::SWORD},
    {ItemType::WAKIZASHI,  XWarSkills::SWORD},

    {ItemType::SMALLAXE,   XWarSkills::AXE},
    {ItemType::WARAXE,     XWarSkills::AXE},
    {ItemType::BATTLEAXE,  XWarSkills::AXE},
    {ItemType::GREATAXE,   XWarSkills::AXE},
    {ItemType::ORCISHAXE,  XWarSkills::AXE},

    {ItemType::MACE,       XWarSkills::MACE},
    {ItemType::FLAIL,      XWarSkills::MACE},

    {ItemType::SHORTSPEAR, XWarSkills::POLEARM},
    {ItemType::LONGSPEAR,  XWarSkills::POLEARM},
    {ItemType::PITCHFORK,  XWarSkills::POLEARM},
    {ItemType::PIKE,       XWarSkills::POLEARM},
    {ItemType::HALBERD,    XWarSkills::POLEARM},

    {ItemType::STAFF,      XWarSkills::STAVE},
};


struct WEAPON_BRAND_TYPE_NAME {
    AttackEffectType brt;
    std::string templ;
};

const int weapon_brand_name_db_size = 17;

WEAPON_BRAND_TYPE_NAME weapon_brand_name_db[weapon_brand_name_db_size] = {
    {AttackEffectType::FIRE,	"{} of Fire",	},
    {AttackEffectType::HELLFIRE,	"{} of Hell Fire",	},
    {AttackEffectType::COLD,	"{} of Cold"	},
    {AttackEffectType::ULTIMATECOLD,	"{} of Ultimate Cold"	},
    {AttackEffectType::LIGHTNING,	"{} of Lightning"	},

    {AttackEffectType::ACID,	"{} of Acid"	},
    {AttackEffectType::POISON,	"{} of Poison"	},
    {AttackEffectType::DEATH,	"{} of Death"	},

    {AttackEffectType::UNDEADSLAYER,	"{} of Slay Undead"	},
    {AttackEffectType::HUMANOIDSLAYER,	"{} of Slay Humanoids"	},
    {AttackEffectType::ANIMALSLAYER,	"{} of Slay Animals"	},
    {AttackEffectType::DRAGONSLAYER,	"{} of Dragon Slaying"	},
    {AttackEffectType::GIANTSLAYER,	"{} of Giant Slaying"	},
    {AttackEffectType::ORCSLAYER,	"{} of Slay Orcs"	},
    {AttackEffectType::TROLLSLAYER,	"{} of Slay Trolls"	},
    {AttackEffectType::TROLLSLAYER,	"{} of Slay Trolls"	},
    {AttackEffectType::DEMONSLAYER,	"{} of Slay Demons"	},
};

XWeapon::XWeapon(ItemType _it)
{
    kind = ItemKind::WEAPON;
    BasicFill(_it, &gi_weapon);
    bp = BP_HAND;
    RNG = 0;
    BindWeapon();
}

int XWeapon::BindWeapon()
{
    for (int i = 0; i < weapon_db_size; i++)
        if (wbind[i].it == it) {
            wt = wbind[i].ws;
            return 1;
        }

    assert(0);
    return 0;
}

std::string XWeapon::toString()
{
    _ITEMPROP* prop = GetMaterial(material_index);
    std::string w_name = prop->propname;

    for (int i = 0; i < weapon_db_size; i++) {
        if (it == weapon_db[i].it) {
            w_name.append(" ");
            w_name.append(weapon_db[i].name);
            break;
        }
    }

    std::string brand_templ;

    if (aet != AttackEffectType::NONE) {
        int ec = vBitsCount(static_cast<unsigned int>(aet & AttackEffectType::ELEMENTAL_MASK));
        int bc = vBitsCount(static_cast<unsigned int>(aet & AttackEffectType::BLACK_MASK));
        int sc = vBitsCount(static_cast<unsigned int>(aet & AttackEffectType::SLAYER_MASK));

        if (ec == 1 && bc == 0 && sc == 0) {
            brand_templ = GetTemplate(aet & AttackEffectType::ELEMENTAL_MASK);
        } else if (ec == 0 && bc == 0 && sc == 1) {
            brand_templ = GetTemplate(aet & AttackEffectType::SLAYER_MASK);
        } else if (ec >= 1 && bc == 0 && sc == 1) {
            brand_templ = fmt::format("Elemental {}", GetTemplate(aet & AttackEffectType::SLAYER_MASK));
        } else if (ec >= 1 && bc == 0 && sc > 1) {
            brand_templ = "Elemental {} of Slaying";
        } else if (ec == 0 && bc == 0 && sc > 1) {
            brand_templ = "{} of Slaying";
        } else if (ec > 1 && bc == 0 && sc == 0) {
            brand_templ = "Elemental {}";
        }
    }

    std::string fullname;
    if (quantity == 1) {
        if (!brand_templ.empty()) {
            fullname = fmt::format(brand_templ, w_name);
        } else {
            fullname = w_name;
        }
    } else {
        if (!brand_templ.empty()) {
            fullname = fmt::format("heap of ({})" + brand_templ,
                quantity, w_name);
        } else {
            fullname = fmt::format("heap of ({}) {}s", quantity, w_name);
        }
    }

    if (isIdentifed()) {
        if (RNG != 0) {
            fullname.append(fmt::format(" <{:+}>", RNG));
        }

        if (_DV != 0 || _PV != 0) {
            fullname.append(fmt::format(" [{:+}, {:+}]", _DV, _PV));
        }

        fullname.append(fmt::format(" ({:+}, {}d{}{:+})",
            _HIT, dice.GetCount(), dice.GetSides(), dice.GetBonus()));

        fullname.append(StatsToString());
    }

    return fullname;
}

std::string XWeapon::GetTemplate(AttackEffectType mask, int isRight)
{
    std::string brand_templ = nullptr;

    for (int j = 0; j < weapon_brand_name_db_size; j++) {
        if (weapon_brand_name_db[j].brt == mask) {
            brand_templ = weapon_brand_name_db[j].templ;
            break;
        }
    }

    return brand_templ;
}
