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

#include "wmagic.h"
/*
int XWorldMagic::CastSpell(XCreature * cr, int x, int y, XSpell * spell, int W, int L, char * xbuf)
{
	MAGIC_TYPE2 mgt2 = spell->GetMGT2();

	XPoint pt;
	XCreature * target;
	int flag = 0;
	char tbuf[256];
	char buf[256];

	strcpy(buf, ".");

	switch (mgt2)
	{
		case MG2_BOLT:
			cr->MissileFlight(cr->x, cr->y, x, y, spell->GetRNG(W, L), 2, &pt);
			if (target = (XCreature *)cr->l->map->GetMonster(pt.x, pt.y))
			{
				int dmg = spell->GetDMG(W, L, 0);
				cr->AttackCreature(target, dmg, buf);
				flag = 1;
				sprintf(tbuf, "%s fire a %s on %s", cr->name, spell->GetName(), target->name);
			}
		break;

		case MG2_DRAIN:
			cr->MissileFlight(cr->x, cr->y, x, y, spell->GetRNG(W, L), 2, &pt);
			if (target = (XCreature *)cr->l->map->GetMonster(pt.x, pt.y))
			{
				int dmg = spell->GetDMG(W, L, 0);
				cr->AttackCreature(target, dmg, buf);
				flag = 1;
				cr->_HP += dmg;
				sprintf(tbuf, "%s drain life from %s", cr->name, target->name);
			}
			break;

		case MG2_HEAL:
			if (target = (XCreature *)cr->l->map->GetMonster(x, y))
			{
				if (target->_HP < target->GetMaxHP())
				{
					target->_HP = target->GetMaxHP();
					flag = 1;
					sprintf(tbuf, "%s feel healed", target->name);
				} else
				{
					sprintf(tbuf, "%s feel nothing special", target->name);
					flag = 0;
				}
			}
		break;

		case MG2_CURE:
			if (target = (XCreature *)cr->l->map->GetMonster(x, y))
			{
				int dmg = spell->GetDMG(W, L, 0);
				if (target->_HP < target->GetMaxHP())
				{
					target->_HP += dmg;
					if (target->_HP > target->GetMaxHP())
						target->_HP =  target->GetMaxHP();
					flag = 1;
					sprintf(tbuf, "%s feel better", target->name);
				} else
				{
					sprintf(tbuf, "%s feel nothing special", target->name);
					flag = 0;
				}
			}
		break;

		case MG2_TELEPORT:
			if (target = (XCreature *)cr->l->map->GetMonster(x, y))
			{
				cr->l->GetFreeXY(&pt);
				cr->LastStep();
				cr->FirstStep(pt.x, pt.y, cr->l);
				flag = 1;
				sprintf(tbuf, "%s teleported", cr->name);
			}
		break;

		case MG2_STORM:
		case MG2_BALL:
			cr->MissileFlight(cr->x, cr->y, x, y, spell->GetRNG(W, L), 2, &pt);
			assert(0);
		break;

		case MG2_TOUCH:
			if (cr->l->map->GetVisible(x, y) && __animation_flag)
			{
				cr->l->map->Put(cr->l->all);
				cr->l->map->PutChar(x, y, '*', spell->GetColor());
				vRefresh();
				vDelay(__animation_flag);
			}
			if (target = (XCreature *)cr->l->map->GetMonster(x, y))
			{
				int dmg = spell->GetDMG(W, L, 0);
				cr->AttackCreature(target, dmg, buf);
				flag = 1;
				sprintf(tbuf, "%s magicaly touch %s", cr->name, target->name);
			}
			break;

			default : assert(0); break;
		}

		sprintf(xbuf, "%s%s", tbuf, buf);
		return flag;
}
*/
