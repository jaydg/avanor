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

#include "xweapon.h"
#include "xapi.h"

REGISTER_CLASS(XWeapon);

_MAIN_ITEM_STRUCT weapon_db[] =
{
{IT_CLUB,		"club",			'/',		"",	"",	"",		"1d3",  "",		"", 	ISET_WOODSTONE,		1,		10,	0,	""},
{IT_WARHAMMER,	"war hammer",	'/',		"",	"",	"1d2-4","1d7",  "1d4",	"", 	ISET_HARDMETAL,		12,		10,	0,	""},
{IT_SCEPTER,	"scepter",		'/',		"",	"",	"",		"1d7",  "1d2",	"", 	ISET_HARDMETAL,		10,		10,	0,	""},

{IT_DAGGER,		"dagger",		'/',		"",	"",	"",		"1d4",  "",		"", 	ISET_WEAPON,		2,		3,	0,	""},
{IT_KNIFE,		"knife",		'/',		"",	"",	"1d2-2","1d3",  "",		"", 	ISET_SIMPLEWEAPON,	1,		2,	0,	""},
{IT_ORCISHDAGGER,"orcish dagger",'/',		"",	"",	"1d2",	"1d3",  "1d2",	"", 	ISET_STEEL,			3,		3,	0,	""},
{IT_LONGDAGGER,	"long dagger",	'/',		"",	"",	"1d3-1","1d4",  "1d2-1","", 	ISET_STEEL,			4,		3,	0,	""},

{IT_SHORTSWORD,	"short sword",	'/',		"",	"",	"",		"1d5",  "",		"", 	ISET_HARDMETAL,		6,		5,	0,	""},
{IT_LONGSWORD,	"long sword",	'|',		"",	"",	"",		"2d4",  "",		"", 	ISET_HARDMETAL,		18,		10,	0,	""},
{IT_BROADSWORD,	"broad sword",	'|',		"",	"",	"",		"2d5",  "",		"", 	ISET_HARDMETAL,		20,		12,	0,	""},
{IT_RAPIER,		"rapier",		'|',		"",	"",	"1d3-1","1d6",  "",		"", 	ISET_HARDMETAL,		7,		5,	0,	""},
{IT_SCIMITAR,	"scimitar",		'|',		"",	"",	"1",	"1d6",  "1d3-1","", 	ISET_HARDMETAL,		7,		6,	0,	""},
{IT_KATANA,		"katana",		'|',		"",	"",	"1",	"3d6",  "1d3-1","", 	ISET_HARDMETAL,		50,		1,	0,	""},
{IT_WAKIZASHI,	"wakizashi",	'|',		"",	"",	"1",	"2d6",  "1d3-1","", 	ISET_HARDMETAL,		45,		1,	0,	""},

{IT_SMALLAXE,	"small axe",	'\\',		"",	"",	"",		"1d6",  "",		"", 	ISET_HARDMETAL,		10,		6,	0,	""},
{IT_WARAXE,		"war axe",		'\\',		"",	"",	"1d3-1","2d3",  "",		"", 	ISET_HARDMETAL,		15,		7,	0,	""},
{IT_BATTLEAXE,	"battle axe",	'\\',		"",	"",	"1d3-2","2d5",  "1d3-1","", 	ISET_HARDMETAL,		25,		12,	0,	""},
{IT_GREATAXE,	"great axe",	'\\',		"",	"",	"",		"3d4",  "",		"",		ISET_HARDMETAL,		30,		20,	0,	""},
{IT_ORCISHAXE,	"orcish axe",	'\\',		"",	"",	"1d3-1","2d4",  "",		"", 	ISET_HARDMETAL,		10,		10,	0,	""},

{IT_MACE,		"mace",			'\\',		"",	"",	"",		"1d6",  "",		"", 	ISET_HARDMETAL,		10,		7,	0,	""},
{IT_FLAIL,		"flail",		'\\',		"",	"",	"",		"1d7",  "",		"", 	ISET_HARDMETAL,		15,		7,	0,	""},

{IT_SHORTSPEAR,	"short spear",	'/',		"",	"",	"",		"1d5",  "",		"", 	ISET_WOODSTONE,		3,		4,	0,	""},
{IT_LONGSPEAR,	"long spear",	'/',		"",	"",	"",		"1d9",  "",		"", 	ISET_STEEL,			8,		6,	0,	""},
{IT_PITCHFORK,	"pitchfork",	'/',		"",	"",	"1d2-4","1d3",  "",		"", 	ISET_BLACKMETAL,	1,		3,	0,	""},

{IT_STAFF,		"staff",		'/',		"",	"",	"1d2-2","1d10",  "1d2",		"", 	ISET_WOOD,		8,		3,	0,	""},
};
const int weapon_db_size = 25;

_WEAPON_BIND wbind[weapon_db_size] = {
{IT_CLUB,			WSK_CLUB},
{IT_WARHAMMER,		WSK_CLUB},
{IT_SCEPTER,		WSK_CLUB},

{IT_DAGGER,			WSK_DAGGER},
{IT_KNIFE,			WSK_DAGGER},
{IT_ORCISHDAGGER,	WSK_DAGGER},
{IT_LONGDAGGER,		WSK_DAGGER},

{IT_SHORTSWORD,		WSK_SWORD},
{IT_LONGSWORD,		WSK_SWORD},
{IT_BROADSWORD,		WSK_SWORD},
{IT_RAPIER,			WSK_SWORD},
{IT_SCIMITAR,		WSK_SWORD},
{IT_KATANA,			WSK_SWORD},
{IT_WAKIZASHI,		WSK_SWORD},

{IT_SMALLAXE,		WSK_AXE},
{IT_WARAXE,			WSK_AXE},
{IT_BATTLEAXE,		WSK_AXE},
{IT_GREATAXE,		WSK_AXE},
{IT_ORCISHAXE,		WSK_AXE},

{IT_MACE,			WSK_MACE},
{IT_FLAIL,			WSK_MACE},

{IT_SHORTSPEAR,		WSK_POLEARM},
{IT_LONGSPEAR,		WSK_POLEARM},
{IT_PITCHFORK,		WSK_POLEARM},

{IT_STAFF,			WSK_STAVE},
};


struct WEAPON_BRAND_TYPE_NAME
{
	BRAND_TYPE brt;
	char * templ;
};

const int weapon_brand_name_db_size = 17;

WEAPON_BRAND_TYPE_NAME weapon_brand_name_db[weapon_brand_name_db_size]=
{
	{BR_FIRE,			"%s of Fire",			},
	{BR_HELLFIRE,		"%s of Hell Fire",		},
	{BR_COLD,			"%s of Cold"			},
	{BR_ULTIMATECOLD,	"%s of Ultimate Cold"	},
	{BR_LIGHTNING,		"%s of Lightning"		},
	
	{BR_ACID,			"%s of Acid"			},
	{BR_POISON,			"%s of Poison"			},
	{BR_DEATH,			"%s of Death"			},

	{BR_UNDEADSLAYER,	"%s of Slay Undead"		},
	{BR_HUMANOIDSLAYER,	"%s of Slay Humanoids"	},
	{BR_ANIMALSLAYER,	"%s of Slay Animals"	},
	{BR_DRAGONSLAYER,	"%s of Dragon Slaying"	},
	{BR_GIANTSLAYER,	"%s of Giant Slaying"	},
	{BR_ORCSLAYER,		"%s of Slay Orcs"		},
	{BR_TROLLSLAYER,	"%s of Slay Trolls"		},
	{BR_TROLLSLAYER,	"%s of Slay Trolls"		},
	{BR_DEMONSLAYER,	"%s of Slay Demons"		},
};


XWeapon::XWeapon(ITEM_TYPE _it)
{
	im = IM_WEAPON;
	BasicFill(_it, weapon_db, weapon_db_size);
	bp = BP_HAND;
	RNG = 0;
	BindWeapon();
}

int XWeapon::BindWeapon()
{
	for (int i = 0; i < weapon_db_size; i++)
		if (wbind[i].it == it)
		{
			wt = wbind[i].ws;
			return 1;
		}
	assert(0);
	return 0;
}

void XWeapon::toString(char * buf)
{
//	GetFullName(buf);

	char w_name[256];
	_ITEMPROP * prop = GetMaterial(material_index);
	strcpy(w_name, prop->propname);
	for (int i = 0; i < weapon_db_size; i++)
	{
		if (it == weapon_db[i].it)
		{
			strcat(w_name, " ");
			strcat(w_name, weapon_db[i].name);
			break;
		}
	}
	
	char * brand_templ = NULL;

	char zbuf[256] = "%s";
	if (brt)
	{
		int ec = vBitsCount(brt & BR_ELEMENTAL_MASK);
		int bc = vBitsCount(brt & BR_BLACK_MASK);
		int sc = vBitsCount(brt & BR_SLAYER_MASK);
		
		if (ec == 1 && bc == 0 && sc == 0)
		{
			brand_templ = GetTemplate(brt & BR_ELEMENTAL_MASK);
		} else
		if (ec == 0 && bc == 0 && sc == 1)
		{
			brand_templ = GetTemplate(brt & BR_SLAYER_MASK);
		} else
		if (ec >= 1 && bc == 0 && sc == 1)
		{
			strcpy(zbuf, "Elemental ");
			strcat(zbuf, GetTemplate(brt & BR_SLAYER_MASK));
			brand_templ = zbuf;
		} else
		if (ec >= 1 && bc == 0 && sc > 1)
		{
			brand_templ = "Elemental %s of Slaying";
		} else
		if (ec == 0 && bc == 0 && sc > 1)
		{
			brand_templ = "%s of Slaying";
		} else
		if (ec > 1 && bc == 0 && sc == 0)
		{
			brand_templ = "Elemental %s";
		}

	}
	
	if (quantity == 1)
	{
		if (brand_templ)
			sprintf(buf, brand_templ, w_name);
		else
			sprintf(buf, "%s", w_name);
	}
	else
	{
		if (brand_templ)
		{
			char xbuf[256];
			strcpy(xbuf, "heap of (%d)");
			strcat(xbuf, brand_templ);
			sprintf(buf, xbuf, quantity, w_name);
		}
		else
			sprintf(buf, "heap of (%d) %ss", quantity, w_name);
	}



	if (isIdentifed())
	{
		strcat(buf, " ");
		char tbuf[256];

		if (RNG != 0)
		{
			sprintf(tbuf, "<%+d>", RNG);
			strcat(buf, tbuf);
		}

		if (_DV != 0 || _PV != 0)
		{
			sprintf(tbuf, "[%+d, %+d]", _DV, _PV);
			strcat(buf, tbuf);
		}
		sprintf(tbuf, "(%+d, %dd%d%+d)", _HIT, dice.X, dice.Y, dice.Z);
		strcat(buf, tbuf);

		StatsToString(tbuf);
		strcat(buf, tbuf);
	}
}

char * XWeapon::GetTemplate(unsigned int mask, int isRight)
{
	char * brand_templ = NULL;
	for (int j = 0; j < weapon_brand_name_db_size; j++)
	{
		if ((weapon_brand_name_db[j].brt ^ mask) == 0)
		{
			brand_templ = weapon_brand_name_db[j].templ;
			break;
		}
	}
	return brand_templ;
}
