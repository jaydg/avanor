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

#ifndef __CREATURE_H
#define __CREATURE_H

#include <math.h>
#include "xlist.h"
#include "std_ai.h"
#include "bodypart.h"
#include "magic.h"
#include "skills.h"
#include "wskills.h"
#include "xpotion.h"
#include "xcorpse.h"
#include "incl_i.h"
#include "cr_defs.h"
#include "deity.h"

enum AI_TYPE {AI_HERO = 0, AI_SIMPLE = 1};

enum ACTION {
	A_UNKNOWN        = 0, 
	A_MOVE           = 1, 
//	A_MOVETOSTAIRWAY = 2, 
	A_ATTACK         = 3, 
	A_CAST           = 4,
	A_EAT            = 5,
	A_READ           = 6,
	A_USETOOL        = 7,
};


enum TACTICS_STATE
{
	TS_COWARD,
	TS_DEFENSIVE,
	TS_NORMAL,
	TS_AGRESSIVE,
	TS_BERSERKER,
};

enum CR_NAME_TYPE
{
	CRN_T1, //you, the kobold, someone
	CRN_T2, //you, he/she, it
	CRN_T3, //your, him/her, it
	CRN_T4, //yours, his/hers, its
};



/////// attack structures ///////////
// 1) Creatures can make one attack with several consiquences:
//		a) just a simple damage
//		b) stun, paralyze, poison
//		c) fire, cold or other damage (same with 'b')
// 2) Creature extend it damage by some fetures
//		a) spawn when attack was successful
//		b) 'eat' armour or weapon

enum EXTENDED_ATTACK
{
	EA_NONE,
	EA_SPAWN,
};

struct MELEE_ATTACK
{
	EXTENDED_ATTACK e_attack;
	RESISTANCE r_attack;
	int prob; //0..100
};




class XAI;
class XStandardAI;
class XModifer;
class XMagic;
class XPotion;
class XBook;
class XScroll;
enum MAGIC_SCHOOL;

struct ACTION_DATA
{
	ACTION_DATA() : item(NULL), action(A_MOVE) {}
	ACTION action;
	XPtr<XItem> item;
	void Store(XFile * f);
	void Restore(XFile * f);
};


typedef int (ITEM_FILTR)(XItem *);

struct XTREQUEST
{
	int max_rng;
	ITEM_FILTR * item_filtr;
};

struct XTRESPONSE
{
	XPoint pt;
	XObject * o;
};

enum MISSILE_FL_TYPE
{
	MFT_ARROW,
	MFT_BALL,
};

enum MF_RESULT
{
	MF_HIT,
	MF_AVOID,
	MF_BLOCK,
};

struct MF_DATA //struct for missile flight
{
	int sx; //start position;
	int sy;
	int ex; //target position
	int ey;
	MISSILE_FL_TYPE arrow_type;
	int arrow_color;
	int max_range; //if creature avoid missile - it flight away.
	int missile_speed; //greater speed, harder to avoid.
	int to_hit; //hit bonus of missile;
	XPoint pt; //position where arrow stopped
	XLocation * l; //where it is...
};

struct DAMAGE_DATA //struct for inflict damage to creature
{
	XCreature * target;
	XCreature * attacker;
	const char * attacker_name; //if specified this name, than write it instead attacker name
	int damage;
};

struct _CREATURE;

class XCreature : public XBaseObject
{
public:
	XItemList contain;
	XList<XBodyPart *> components;
	CR_PERSON_TYPE creature_person_type;
	const char * creature_description;
	CREATURE_NAME creature_name; //allow to store less info into save file
	const _CREATURE * super_info; //full information about Creature Creation struct...
public:
	DECLARE_CREATOR(XCreature, XBaseObject);
	XCreature();
	virtual void Invalidate();

	virtual int Compare(XObject * o) {return 1;}

	int TestMove();
	virtual void NewMove();
	virtual void Move();
	virtual void HideOldView();
	virtual void ShowNewView();
	virtual void PutStatus();
	virtual void DoMove();
	int Run();

	XStandardAI * xai;

	unsigned long _EXP;
	int level;
	int base_exp;
	unsigned long ExpOfLevel(int lev);
	void AddExp(unsigned long exp);

	int GetSpeed();

	CR_GENDER GetGender();
	char * GetGenderStr();

	int lttm; //long doing time to move
	int isDisturb; //is creature disturbed duaring lttm

	ACTION_DATA action_data;
	virtual int stopAction();

	void Regenerate();
	int onHeal(int _hp);
	int onRestorePP(int _pp);

	virtual void IncLevel();

	virtual int Read(XItem * item);
	int continueRead();

	virtual int Eat(XAnyFood * food);
	int continueEat();

	virtual int UseItem(XItem * item);
	int continueUseItem();



	int base_nutrio; //stomach size for normal satiation
	int nutrio; //stomach satioation;
	int nutrio_speed; //speed with which nutrients decrease;
	int DecNutrio();
	FOOD_FEELING food_feeling;

	void MoveStairWay();

	virtual void Attack();
	virtual void Die(XCreature * killer);

	XBodyPart * GetRNDBodyPart(ITEM_MASK xim, RBP_FLAG rbpf);
	XBodyPart * GetRNDBodyPart();
	int GetHITFHBonus(XItem * weapon);
	int GetShieldDVBonus();
	int GetDMGFHBonus(XItem * weapon);
	int GetHIT();
	int GetDV(XCreature * attacker = NULL);
	int GetDMG();
	int GetPV();
	int GetResistance(RESISTANCE tr);
	int GetVisibleRadius();
	int GetTacticsDVBonus();
	int GetTacticsHITBonus();
	int GetTacticsDMGBonus();

	virtual int GetTarget(TARGET_REASON tr, XPoint * pt = NULL, int max_range = 0, XObject ** back = NULL);//Get target for a spell
	virtual XItem * onIdentifyItem() {return NULL;}
	virtual int TargetOp(TARGET_REASON tr, XTREQUEST * rq, XTRESPONSE * rp);
	virtual XItem * SelectItem(ITEM_FILTR * filtr) {return NULL;}

	int Shoot(int tx, int ty);
	XItem * GetItem(BODYPART bp, int count = 0);
	XBodyPart * GetBodyPart(BODYPART bp, int count = 0);
	int CanWear(XItem * item);
	int Wear(XItem * item); //if can Wear, Wear it.

	XModifer   * md;
	XMagic     * m;
	XSkills    * sk;
	XWarSkills * wsk;

	XResistance  added_resists;
	XStats       added_stats;
	XStats       max_stats;
	int          added_DV;
	int          added_PV; 
	int          added_HIT;
	int          added_DMG;
	int          added_RNG;
	int          added_HP;
	int          added_PP;
	int          added_speed;
/*	unsigned int added_xproperty;*/

	
	bool ContainItem(XItem * item); //Adds item to creature inventory, increase cqrried weight.
	int DropItem(XItem * i);
	int PickUpItem(XItem * i);
	CARRY_STATE GetCarryState();
	int CarryValue(CARRY_STATE cs);
	bool CarryItem(XItem * item);
	void UnCarryItem(XItem * item);
	int carried_weight;

	int MoneyOp(int money_count); // if money_count >= 0 then add money, else sub.

	int GetStats(STATS st);
	int GainAttr(STATS st, int val);
	int GainResist(RESISTANCE rs, int val);
	int GetMaxHP();
	int GetMaxPP();
	int GetExp();
	int GetCreatureStrength();
//	static int AttackCreature(ATTACK_DATA * pData);
	static int InflictDamage(DAMAGE_DATA * pData);
	int AttackCreature(XCreature * target, int dmg); //hit tgt for dmg damage!
	int AttackCreature(XCreature * target, int dmg, RESISTANCE tr, const char * magic_name); //hit tgt for dmg damage!
	int onMagicDamage(int dmg, RESISTANCE tr);
	int CauseEffect(XCreature * attacker, int dmg, RESISTANCE tr);
	int CauseEffect(int dmg, BRAND_TYPE brt);
	int onAttacked(XCreature * attacker, int dmg);
	int onAttackedByMagic(XCreature * attacker, int dmg, RESISTANCE tr, const char * magic_name);

	virtual char * GetMeleeAttackMsg() {return "attack";}

	virtual void FirstStep(int _x, int _y, XLocation * _l);
	virtual void LastStep();

	static MF_RESULT MissileFlight(MF_DATA * mfd);

	virtual void Store(XFile * f);
	virtual void Restore(XFile * f);

	CREATURE_CLASS creature_class;
	GROUP_ID group_id; //orcs warparty has id 1, bandits - id 2, etc

	virtual char * StdAnswer() {return "You receive no answer.";}
	virtual int Chat(XCreature * chatter, char * msg);

	virtual int onGiveItem(XCreature * giver, XItem * item);

	TACTICS_STATE tactics;
	void ChangeTactics(TACTICS_STATE tact) {tactics = tact;}
	char * GetWoundMsg(int flag = 0);

	void GetRangeAttackInfo(int * range, int * hit, XDice * dmg);

	CREATURE_SIZE creature_size;
	int attack_energy;
	int move_energy;
	int base_speed;
	int MeleeAttack(XCreature * target, XItem * weapon);
	XQList<MELEE_ATTACK> * melee_attack;

	void Sacrifice(XItem * item);
	XReligion religion;


	int isCreatureVisible(XCreature * cr); //check if creature visible or invisible
	bool isVisible(); //check if creature visible to add acction message to msgwin
	static XCreature * main_creature; //at this time is used to determine visibility of msg

	bool isHero() { return (im & IM_HERO) > 0;}

	char * GetNameEx(CR_NAME_TYPE crn);
	char * GetVerb(char * verb);

};

//Fake creature is need 
class XFakeCreature : public XCreature
{
public:
	XFakeCreature(char * fake_name) { strcpy(name, fake_name); }
};
#endif
