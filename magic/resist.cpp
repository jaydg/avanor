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

#include "resist.h"
#include "xfile.h"
#include "global.h"

RESIST_REC resists_data[] = {
{"unknown",			FLU_NONE},
{"white",			FLU_CREATURE},
{"black",			FLU_CREATURE},
{"fire",			FLU_ALL},
{"water",			FLU_ALL},
{"air",				FLU_ALL},
{"earth",			FLU_ALL},
{"acid",			FLU_ALL},
{"cold",			FLU_ALL},
{"poison",			FLU_CREATURE},
{"decease",			FLU_CREATURE},
{"paralyse",		FLU_CREATURE},
{"stun",			FLU_CREATURE},
{"confuse",			FLU_CREATURE},
{"blind",			FLU_CREATURE},
{"light",			FLU_CREATURE},
{"darkness",		FLU_CREATURE},
{"invisible",		FLU_CREATURE},
{"see_invisible",	FLU_CREATURE},
"eof", 				FLU_NONE};


XResistance::XResistance(XResistance * xr)
{
	Set(xr);
}

XResistance::XResistance()
{
	for (int i = R_WHITE; i < R_EOF; i++)
		SetResistance((RESISTANCE)i, 0);
}

XResistance::XResistance(const char * str1)
{
	XStringProc xsp1(str1);
	char buf[256];
	XDice d;
	for(int i = R_WHITE; i < R_EOF; i++)
	{
		if (xsp1.GetParam(buf, resists_data[i + 1].name))
		{
			d.Setup(buf);
			SetResistance((RESISTANCE)i, d.Throw());
		} else
			SetResistance((RESISTANCE)i, 0);
	}
}

void XResistance::Set(XResistance * r)
{
	if (r)
		for (int i = R_WHITE; i < R_EOF; i++)
			SetResistance((RESISTANCE)i, r->GetResistance((RESISTANCE)i));
}

void XResistance::Add(XResistance * r)
{
	if (r)
		for (int i = R_WHITE; i < R_EOF; i++)
			resistances[i] += r->resistances[i];
}

void XResistance::Sub(XResistance * r)
{
	if (r)
		for (int i = R_WHITE; i < R_EOF; i++)
			resistances[i] -= r->resistances[i];
};

void XResistance::Store(XFile * f)
{
	f->Write(&resistances[R_WHITE], sizeof(int), R_EOF);
}

void XResistance::Restore(XFile * f)
{
	f->Read(&resistances[R_WHITE], sizeof(int), R_EOF);
}


char * resist_name[] = 
{ "White magic", "Black magic", "Fire magic", "Water magic", "Air magic", "Earth magic", 
"Acid", "Cold", "Poison", "Disease", "Paralyse",
"Stun", "Confuse", "Blind",
"Light", "Darkness", "Invisible", "See Invisible"
};

char * XResistance::GetResistanceName(RESISTANCE r)
{
	return resist_name[r];
}

char * resist_level[] = 
{
MSG_LIGHTRED "awful", MSG_LIGHTRED "bad", 
MSG_LIGHTGRAY "none", MSG_LIGHTGREEN "mediocre", MSG_LIGHTGREEN "fair",
MSG_LIGHTGREEN "good", MSG_YELLOW "excellent", MSG_WHITE "complete"
};

char * XResistance::GetResistanceLevel(RESISTANCE r)
{
	if (resistances[r] < -50)
		return resist_level[0];
	else if (resistances[r] < 0)
		return resist_level[1];
	else if (resistances[r] == 0)
		return resist_level[2];
	else if (resistances[r] < 10)
		return resist_level[3];
	else if (resistances[r] < 40)
		return resist_level[4];
	else if (resistances[r] < 80)
		return resist_level[5];
	else if (resistances[r] < 100)
		return resist_level[6];
	else
		return resist_level[7];
}
