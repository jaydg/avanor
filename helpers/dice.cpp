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

#include "dice.h"
#include "xfile.h"
#include "global.h"
#include "xapi.h"

void XDice::Setup(const char* str)
{
	if (str == NULL || strlen(str) < 2)
	{
		X = Y = 0;
		Z = 0;
		Throw();
	} else
	{
		char buf[100];
		strcpy(buf, str);
		for (unsigned int i = 0; i < strlen(buf); i++)
		{
			if (buf[i] == 'd') buf[i] = ' ';
			if ((buf[i] == '+' || buf[i] == '-') && buf[i + 1] == ' ')
				{
					buf[i + 1] = buf[i];
					buf[i] = ' ';
				}
		}
		X = Y = 0;
		Z = 0;
		sscanf(buf, "%d %d %d", &X, &Y, &Z);
		assert (X >= 0 && Y >= 0);
		Throw();
	}
}

int XDice::Throw()
{
	S = Z;
	if (Y > 0)
		for (int i = 0; i < X; i++)
			S += vRand() % Y + 1;
	return S;

}

int XDice::NThrow()
{
	return NDFunc(X * Y) + Z;
}

void XDice::Store(XFile * f)
{
	f->Write(&S, sizeof(int));
	f->Write(&X, sizeof(int));
	f->Write(&Y, sizeof(int));
	f->Write(&Z, sizeof(int));
}

void XDice::Restore(XFile * f)
{
	f->Read(&S, sizeof(int));
	f->Read(&X, sizeof(int));
	f->Read(&Y, sizeof(int));
	f->Read(&Z, sizeof(int));
}

int dfunc_data[22] =
{  750, 800,840,870,890,902,914,926,937,947,956,964,971,977,982,986,990,993,996,998,999,100000};

int XDice::DFunc()
{
	int r = vRand(1000);
	int i = 0;
	while (dfunc_data[i] < r) i++;
	return i;
}

int XDice::NDFunc(int maximum)
{
	return vRound((DFunc() * maximum) / 20.0f);
}
