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

#include "creatures.h"
#include "xapi.h"


struct FILL_OUT_CR_DB
{
	FILL_OUT_CR_DB();
};

FILL_OUT_CR_DB fill_out_cr_db;


FILL_OUT_CR_DB::FILL_OUT_CR_DB()
{
	////////////////////////////////////////////////////////////////
	// RAT
	////////////////////////////////////////////////////////////////
	

	////////////////////////////////////////////////////////////////
	// BAT
	////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////
	// REPTILE
	////////////////////////////////////////////////////////////////



	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	// Fill out creature sets for quick random generation

/*	for (int i = 0; i < CN_EOF; i++)
	{
		if (XCreatureStorage::creature_storage[i].name != NULL)
		{
			CREATURE_CLASS crc = XCreatureStorage::creature_storage[i].cr_class;
			XCreatureStorage::creature_set[vGetBitNumber(crc)].cn[XCreatureStorage::creature_set[vGetBitNumber(crc)].count] = (CREATURE_NAME)i;
			XCreatureStorage::creature_set[vGetBitNumber(crc)].count++;
		}
	}*/
}

