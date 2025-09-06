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

#include "xgloves.h"

REGISTER_CLASS(XGloves);

_MAIN_ITEM_STRUCT GLOVES_STRUCT[] = {

    {IT_GLOVES,	"gloves",	']',	"1d2",	"1d2",	"1d3",	"1d1",	"1d1",	"1d2-1", ISET_ALLLEATHER,	2,	1,	100,	IQ_AVG,	""},
    {IT_GAUNTLETS,	"gauntlets",	']',	"1d3+1",	"1d3+1",	"1d3+1",	"1d1",	"1d2",	"1d3-1", ISET_ALLMETAL,	3,	1,	10,	IQ_FAIR, ""},
    {IT_KNUCKLES,	"knuckles",	']',	"1d2+1",	"1d2+1",	"1d2-1",	"1d1",	"1d3",	"1d2+2", ISET_HARDMETAL,	4,	1,	10,	IQ_GOOD, ""}
};

XItemBasicStructure gi_gloves(GLOVES_STRUCT, 3);

XGloves::XGloves(ITEM_TYPE it)
{
    im = IM_GLOVES;
    BasicFill(it, &gi_gloves);
    bp = BP_GLOVES;
    dice.Z = 0;
    RNG = 0;
}

XGloves::XGloves(XGloves * gloves) : XClothe((XClothe*)gloves)
{
}

