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

#include "anycr.h"
#include "itemf.h"
#include "xapi.h"
#include "unique.h"
#include "quest.h"
#include "item_misc.h"


REGISTER_CLASS(XAnyCreature);

XAnyCreature::XAnyCreature(_CREATURE * cr) : XCreature()
{
	view = cr->view;
	color = cr->color;
	strcpy(name, cr->name);

	creature_class = cr->cr_class;


	dice.Setup(cr->dice);

	s = new XStats(cr->stats);
	r = new XResistance(cr->r);

	max_stats.Set(s);
	
	XBodyPart::Create(this, cr->body);

	XDice d;
	d.Setup(cr->move_energy);
	ttmb = d.S;
	ttm = ttmb;

	d.Setup(cr->creature_weight);
	weight = d.S;

	d.Setup(cr->attack_energy);
	attack_energy = d.S;

	d.Setup(cr->move_energy);
	move_energy = d.S;

	d.Setup(cr->speed);
	base_speed = d.S;

	creature_size = cr->creature_size;
	
	d.Setup(cr->dv);
	_DV = d.S;

	d.Setup(cr->pv);
	_PV = d.S;

	d.Setup(cr->hit);
	_HIT = d.S;

	d.Setup(cr->hp);
	MAX_HP = d.S;
	_HP = GetMaxHP();

	d.Setup(cr->pp);
	MAX_PP = d.S;
	_PP = d.S;

	base_exp = GetCreatureStrength();
	creature_person_type = cr->person;
	creature_description = cr->creature_description;

	melee_attack = &cr->melee_attack;

//Setup AI
	xai->SetAIFlag((AI_FLAG)(cr->ai_flags));

//EQUIP CREATURE
	for (XQList<EQUIP_REC>::iterator it = cr->equipment.begin(); it != cr->equipment.end(); it++)
	{
		for (int i = 0; i < (*it).count; i++)
			if (vRand(100) < (*it).probability)
			{
				XItem * item = NULL;
				if ((*it).it == IT_UNKNOWN)
					item = ICREATEA((ITEM_MASK)((*it).mask));
				else
					item = ICREATEB((ITEM_MASK)((*it).mask), (*it).it, 0, 10000000);
				if (item->im & IM_BODY && item->it == IT_DRESS 
					 && GetGender() == GEN_MALE)
				{
					// This is a kludge to prevent a "Roderick in a dress" scenario.
					item->it = IT_CLOTHES;
					strcpy(item->name,"clothes");
				}
				if (CanWear(item))
				{
					if (item->im & IM_MISSILEW) //if it is missile weapon we need to create proper ammo
					{
						XItem * missile = ICREATEB(IM_MISSILE, IT_ARROW, 0, 10000000);
						ContainItem(missile);
					}
					Wear(item);
				}
				else
					ContainItem(item);
			}
	}

	//wear random items if it wasn't weared before.
	XBodyPart * hand_1 = NULL;
	XBodyPart * hand_2 = NULL;
	for (XList<XBodyPart *>::iterator bp_it = components.begin(); bp_it != components.end(); bp_it++)
	{
		if (!(*bp_it)->Item() && vRand(100) < cr->equip_probability)
		{
			if (bp_it->bp_uin == BP_HAND)
			{
				if (hand_1 == NULL)
						hand_1 = bp_it;
					else
						hand_2 = bp_it;
			} else if (bp_it->bp_uin == BP_MISSILE)
				continue;
			else
				bp_it->Wear(ICREATE(bp_it->GetProperIM(), 0, 1000000));
		}
	}
	if (hand_1)
	{
		XItem * weapon = ICREATE(IM_WEAPON, 0, 10000000);
		wsk->SetLevel(weapon->wt, 2); //just basic weapon level
		hand_1->Wear(weapon);
	}

	if (hand_2)
		hand_2->Wear(ICREATE(IM_SHIELD, 0, 10000000));

// supress invisibility, add see invisible
	XBodyPart * neck = GetBodyPart(BP_NECK);
	XBodyPart * ring1 = GetBodyPart(BP_RING, 0);
	XBodyPart * ring2 = GetBodyPart(BP_RING, 1);

	if (cr->generation_flags & GFS_SUPRESS_INVIS)
	{
		if (neck && neck->Item() && neck->Item()->r->GetResistance(R_INVISIBLE) > 0)
			neck->UnWear()->Invalidate();

		if (ring1 && ring1->Item() && ring1->Item()->r->GetResistance(R_INVISIBLE) > 0)
			ring1->UnWear()->Invalidate();

		if (ring2 && ring2->Item() && ring2->Item()->r->GetResistance(R_INVISIBLE) > 0)
			ring2->UnWear()->Invalidate();
	}
	if (cr->generation_flags & GFS_SEE_INVIS)
	{
		while (1)
		{
			if (neck && neck->Item() && neck->Item()->r->GetResistance(R_SEEINVISIBLE))
				break;
			if (ring1 && ring1->Item() && ring1->Item()->r->GetResistance(R_SEEINVISIBLE))
				break;
			if (ring2 && ring2->Item() && ring2->Item()->r->GetResistance(R_SEEINVISIBLE))
				break;

			if (ring1->Item())
				ring1->UnWear()->Invalidate();
			ring1->Wear(new XRing(ENH_SEEINVISIBLE));
			break;
		}
	}


	//Create Money if components more than 2
	if (components.size() > 2 && vRand(3) == 0)
	{
		for (int i = 0; i < vGetHighBitNum(cr->crl) + 1; i++)
		{
			XItem * it = ICREATEA(IM_MONEY);
			ContainItem(it);
		}
	}

//Learn skill
	for (XQList<SKILL_REC>::iterator sk_it = cr->skills.begin(); sk_it != cr->skills.end(); sk_it++)
		sk->Learn((*sk_it).skt, (*sk_it).level);

//Learns spells
	for (XQList<SPELL_NAME>::iterator sp_it = cr->spells.begin(); sp_it != cr->spells.end(); sp_it++)
		m->Learn((*sp_it));

}

void XAnyCreature::Die(XCreature * killer)
{
	if (creature_class == CR_UNDEAD)
	{
		if (creature_name == CN_SKELETON && vRand() % 12 == 0)
		{
			XItem * it = new XBone();
			it->Drop(l.get(), x, y);
		}
	}

	if (creature_class == CR_ORC)
	{
		if (killer && killer->im & IM_HERO)
		{
			XQuest::quest.orcs_killed++;
		}
		XQuest::quest.total_orcs_killed++;
	}

	if (creature_class == CR_RAT && vRand(40) == 0)
	{
		if (creature_name == CN_RAT || creature_name == CN_LARGE_RAT)
		{
			XItem * it = new XRatTail();
			DropItem(it);
		} else if (creature_name == CN_BAT || creature_name == CN_HUGE_BAT)
		{
			XItem * it = new XBatWing();
			DropItem(it);
		}
	}


	if (vRand(5) == 0/*MAX_HP + _HP > 0*/ && !(creature_class & CR_UNDEAD))
	{
		DropItem(new XCorpse(this, &super_info->pCorpseData));
	}
	XCreature::Die(killer);
}

int XAnyCreature::onGiveItem(XCreature * giver, XItem * item)
{
	if (creature_name == CN_ROYAL_GUARD && group_id == GID_GUARDIAN)
	{
		if (item->im & IM_WEAPON && item->brt & BR_ORCSLAYER && (item->wt == WSK_SWORD))
		{
			XQuest::quest.guards_get_orc_slay++;
			msgwin.Add("'Thank you.'");
			contain.Add(item);
			return 1;
		} else
			return 0;
	}
	return 0;
}


CREATURE_NAME XCreatureStorage::last_name = CN_NONE;

void XCreatureStorage::View(CREATURE_NAME cn, const char * name, char view, int color, CR_PERSON_TYPE person, CREATURE_LEVEL crl, CREATURE_CLASS crcl)
{
	last_name = cn;
	assert(creature_storage[last_name].name == NULL);
	creature_storage[last_name].name = name;
	creature_storage[last_name].view = view;
	creature_storage[last_name].color = color;
	creature_storage[last_name].person = person;
	creature_storage[last_name].crl = crl;
	creature_storage[last_name].cr_class = crcl;
}

void XCreatureStorage::Basic(const char * speed, const char * base_energy, const char * combat_energy, CREATURE_SIZE csize, const char * weight)
{
	creature_storage[last_name].speed = speed;
	creature_storage[last_name].move_energy = base_energy;
	creature_storage[last_name].attack_energy = combat_energy;
	creature_storage[last_name].creature_weight = weight;
}

void XCreatureStorage::Body(const char * body, int prob, unsigned int gen_flags)
{
	creature_storage[last_name].body = body;
	creature_storage[last_name].equip_probability = prob;
	creature_storage[last_name].generation_flags = gen_flags;
}

void XCreatureStorage::SetAI(unsigned int aif)
{
	creature_storage[last_name].ai_flags = aif;
}

void XCreatureStorage::S(const char * stats)
{
	creature_storage[last_name].stats = stats;
}

void XCreatureStorage::R(const char * resists)
{
	creature_storage[last_name].r = resists;
}

void XCreatureStorage::Combat(const char * hit, const char * dice)
{
	creature_storage[last_name].hit = hit;
	creature_storage[last_name].dice = dice;
}

void XCreatureStorage::Melee(BRAND_TYPE br, int prob)
{
	MELEE_ATTACK ma;
	ma.e_attack = EA_NONE;
	ma.br_attack = br;
	ma.prob = prob;
	creature_storage[last_name].melee_attack.push_back(ma);
}

void XCreatureStorage::Melee(EXTENDED_ATTACK ea, int prob)
{
	MELEE_ATTACK ma;
	ma.e_attack = ea;
	ma.br_attack = BR_NONE;
	ma.prob = prob;
	creature_storage[last_name].melee_attack.push_back(ma);
}

void XCreatureStorage::Main(const char * dv, const char * pv, const char * hp, const char * pp)
{
	creature_storage[last_name].dv = dv;
	creature_storage[last_name].pv = pv;
	creature_storage[last_name].hp = hp;
	creature_storage[last_name].pp = pp;
}

void XCreatureStorage::D(const char * descr)
{
	creature_storage[last_name].creature_description = descr;
}

void XCreatureStorage::Learn(SKILL_TYPE skt, int lvl)
{
	SKILL_REC scr;
	scr.level = lvl;
	scr.skt = skt;
	creature_storage[last_name].skills.push_back(scr);
}

void XCreatureStorage::Learn(SPELL_NAME spn)
{
	creature_storage[last_name].spells.push_back(spn);
}

void XCreatureStorage::Equip(unsigned int mask, int count, int prob)
{
	EQUIP_REC er;
	er.mask = mask;
	er.count = count;
	er.probability = prob;
	er.it = IT_UNKNOWN;
	creature_storage[last_name].equipment.push_back(er);
}

void XCreatureStorage::Equip(unsigned int mask, ITEM_TYPE it, int prob)
{
	EQUIP_REC er;
	er.mask = mask;
	er.count = 1;
	er.probability = prob;
	er.it = it;
	creature_storage[last_name].equipment.push_back(er);
}

void XCreatureStorage::Corpse(int roating_time, FOOD_TYPE ft)
{
	creature_storage[last_name].pCorpseData.roating_time = roating_time;
	creature_storage[last_name].pCorpseData.ft = ft;
}

void XCreatureStorage::CorpseEffects(CORPSE_EFFECT_TYPE cet, int val)
{
	CORPSE_EFFECT ce;
	ce.type = cet;
	ce.value = val;
	creature_storage[last_name].pCorpseData.effect.push_back(ce);
}

_CREATURE * XCreatureStorage::GetCreatureData(CREATURE_NAME cn)
{
	return &creature_storage[cn];
}

XCreature *  XCreatureStorage::Create(CREATURE_NAME cn)
{
	_CREATURE * cr = &creature_storage[cn];
	XCreature * tcr = NULL;
	if (cn < CN_UNIQUE)
	{
		tcr = new XAnyCreature(cr);
	} else
	{
		switch (cn)

		{
			case CN_ELDER_GRIDOR: tcr = new  XVillageElder(cr); break;
			case CN_BANDIT: tcr = new XBandit(cr); break;
			case CN_SHOPKEEPER: tcr = new XShopkeeper(cr); break;
			case CN_JORGUS: tcr = new XJorgus(cr); break;
			case CN_GEKTA: tcr = new XGekta(cr); break;
			case CN_OZORIK: tcr = new XOzorik(cr); break;
			case CN_YOHJISHIRO: tcr = new XYohjishiro(cr); break;
			case CN_GEFEON: tcr = new XGefeon(cr); break;
			case CN_RODERIK: tcr = new XRoderick(cr); break;
			case CN_BEELZEVILE: tcr = new XBeelzvile(cr); break;
			case CN_TODIN: tcr = new XTodin(cr); break;
			case CN_TORIN: tcr = new XTorin(cr); break;
			case CN_AHKULAN: tcr = new XAhkUlan(cr); break;
			case CN_XSHEE_VOO: tcr = new XXSheeVoo(cr); break;
			case CN_MAGNUSH: tcr = new XMagnush(cr); break;
			case CN_HIGHPRIEST: tcr = new XHighPriest(cr); break;
		}

	}
	tcr->creature_name = cn;
	RestoreCreatureInfo(tcr);
	return tcr;

	assert(0);
	return NULL;
}


XCreature * XCreatureStorage::CreateRnd(CREATURE_CLASS cc, int lvl)
{
	int set = vGetBitNumber(vGetRandomBit(cc));
	int count = 100;
	while (count > 0)
	{
		int r = vRand(creature_set[set].count);
		if (creature_storage[creature_set[set].cn[r]].crl <= lvl)
			return Create(creature_set[set].cn[r]);
		count--;
	}
	return Create(creature_set[set].cn[0]);
}

void XCreatureStorage::RestoreCreatureInfo(XCreature * cr)
{
	cr->melee_attack = &creature_storage[cr->creature_name].melee_attack;
	cr->creature_description = creature_storage[cr->creature_name].creature_description;
	cr->super_info = &creature_storage[cr->creature_name];
}



