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

#include "uniquei.h"

REGISTER_CLASS(XGlamdring);
XGlamdring::XGlamdring() : XWeapon(IT_BROADSWORD)
{
	strcpy(name, "rune-covered broadsword");
	it = IT_BROADSWORD;
	view = '|';
	weight = 700;
	value = 2000;
	_DV = 0;
	_PV = 0;
	_HIT = 12;
	dice.Setup(4, 5, 10);
	RNG = 0;
	if (r)
		delete r;
	if (s)
		delete s;
	r = new XResistance("poison:1d1+99 stun:1d1+99 confuse:1d1+99 fire:1d1+99 cold:1d1+99");
	s = new XStats("St:1d1+9 Dx:1d1+9 To:1d1+9");
	brt = (BRAND_TYPE)(BR_FIRE | BR_COLD | BR_DEMONSLAYER);
	special_property = SPP_NONE;
}

void XGlamdring::toString(char * buf)
{
	GetArtifactName(buf, "broadsword named \"Glamdring\"");
}

REGISTER_CLASS(XDeathHack);
XDeathHack::XDeathHack() : XWeapon(IT_LONGDAGGER)
{
	strcpy(name, "rune-covered long dagger");
	it = IT_LONGDAGGER;
	weight = 70;
	value = 500;
	_DV = 0;
	_PV = 0;
	_HIT = 6;
	dice.Setup(2, 3, 12);
	RNG = 0;
	if (r)
		delete r;
	if (s)
		delete s;
	r = new XResistance("");
	s = new XStats("");
	brt = (BRAND_TYPE)(BR_COLD | BR_DEMONSLAYER);
	special_property = SPP_NONE;
}

void XDeathHack::toString(char * buf)
{
	GetArtifactName(buf, "long dagger named \"Death Hack\"");
}


REGISTER_CLASS(XAvanorDefender);
XGUID XAvanorDefender::avanordefender_guid = 0;
XAvanorDefender::XAvanorDefender() : XWeapon(IT_LONGSWORD)
{
	strcpy(name, "rune-covered long sword");
	it = IT_LONGSWORD;
	weight = 110;
	value = 3000;
	_DV = 0;
	_PV = 0;
	_HIT = 18;
	dice.Setup(4, 4, 15);
	RNG = 0;
	if (r)
		delete r;
	if (s)
		delete s;
	r = new XResistance("poison:0d0+100 stun:0d0+100 confuse:0d0+100 fire:0d0+100 cold:0d0+100");
	s = new XStats("St:0d0+10 To:0d0+10");
	brt = (BRAND_TYPE)(BR_COLD | BR_FIRE | BR_DEMONSLAYER);
	special_property = SPP_NONE;
	avanordefender_guid = xguid;
}

void XAvanorDefender::toString(char * buf)
{
	GetArtifactName(buf, "long sword named \"Avanor's Defender\"");
}

void XAvanorDefender::Restore(XFile * f)
{
	XWeapon::Restore(f);
	avanordefender_guid = xguid;
}

REGISTER_CLASS(XForestBrotherCloak);
XForestBrotherCloak::XForestBrotherCloak() : XCloak(IT_CLOAK)
{
	strcpy(name, "forest brother's cloak");
	it = IT_FORESTBROTHERCLOAK;
	color = xGREEN;
	weight = 20;
	value = 100;
	_DV = 3;
	_PV = 1;
	_HIT = 0;
	dice.Setup(1, 2, 0);
	RNG = 0;
	if (r)
		delete r;
	if (s)
		delete s;
	r = new XResistance("poison:0d0+20 stun:0d0+20");
	s = new XStats("");
	brt = BR_NONE;
	special_property = SPP_NONE;
}

void XForestBrotherCloak::toString(char * buf)
{
	XCloak::toString(buf);
}

REGISTER_CLASS(XGreatElementalRing);
XGreatElementalRing::XGreatElementalRing() : XRing()
{
	if (r)
		delete r;
	if (s)
		delete s;
	r = new XResistance("fire:1d1+79 cold:1d1+79 acid:1d1+79 see_invisible:0d0+20");
	s = new XStats("Wi:1d1+4");
	strcpy(name, "Great Elemental Ring");
}



REGISTER_CLASS(XAvanorScepter);
XAvanorScepter::XAvanorScepter() : XWeapon(IT_SCEPTER)
{
	strcpy(name, "ancient scepter");
	it = IT_SCEPTER;
	view = '|';
	weight = 400;
	value = 12000;
	_DV = 0;
	_PV = 0;
	_HIT = 8;
	dice.Setup(1, 12, 10);
	RNG = 0;
	if (r)
		delete r;
	if (s)
		delete s;
	r = new XResistance("stun:1d1+99 confuse:1d1+99 fire:1d1+99 cold:1d1+99 see_invisible:0d0+20");
	s = new XStats("St:1d1+15");
	brt = (BRAND_TYPE)(BR_FIRE | BR_COLD | BR_DEMONSLAYER);
	special_property = SPP_NONE;
}

void XAvanorScepter::toString(char * buf)
{
	GetArtifactName(buf, "scepter of Avanor");
}

REGISTER_CLASS(XAvanorCrown);
XAvanorCrown::XAvanorCrown() : XCap(IT_CROWN)
{
	strcpy(name, "golden crown");
	it = IT_SCEPTER;
	view = '[';
	weight = 100;
	value = 8000;
	_DV = 2;
	_PV = 2;
	_HIT = 0;
	dice.Setup(0, 0, 0);
	RNG = 0;
	if (r)
		delete r;
	if (s)
		delete s;
	r = new XResistance("stun:1d1+99 confuse:1d1+99 fire:1d1+99 cold:1d1+99 acid:1d1+99 see_invisible:0d0+20");
	s = new XStats("St:0d0+5 Dx:0d0+5 To:0d0+5 Wi:0d0+5 Ma:0d0+5");
	special_property = SPP_NONE;
}

void XAvanorCrown::toString(char * buf)
{
	GetArtifactName(buf, "crown of Avanor");
}


REGISTER_CLASS(XTorinAxe);
XTorinAxe::XTorinAxe() : XWeapon(IT_GREATAXE)
{
	strcpy(name, "ancient axe");
	it = IT_GREATAXE;
	view = '\\';
	weight = 1200;
	value = 12000;
	_DV = 0;
	_PV = 0;
	_HIT = 8;
	dice.Setup(2, 8, 5);
	RNG = 0;
	if (r)
		delete r;
	if (s)
		delete s;
	r = new XResistance("stun:1d1+99 confuse:1d1+99 see_invisible:0d0+20");
	s = new XStats("To:1d1+10");
	brt = (BRAND_TYPE)(BR_COLD);
	special_property = SPP_NONE;
}

void XTorinAxe::toString(char * buf)
{
	GetArtifactName(buf, "Axe of Torin");
}

REGISTER_CLASS(XTorinShield);
XTorinShield::XTorinShield() : XShield(IT_MEDIUMSHIELD)
{
	strcpy(name, "ancient shield");
	it = IT_MEDIUMSHIELD;
	view = ')';
	weight = 100;
	value = 8000;
	_DV = 12;
	_PV = 2;
	_HIT = 0;
	dice.Setup(0, 0, 0);
	RNG = 0;
	if (r)
		delete r;
	if (s)
		delete s;
	r = new XResistance("stun:1d1+99 confuse:1d1+99 fire:1d1+99 cold:1d1+99 acid:1d1+99 see_invisible:0d0+20");
	s = new XStats("To:0d0+5");
	special_property = SPP_NONE;
}

void XTorinShield::toString(char * buf)
{
	GetArtifactName(buf, "Shield of Torin");
}




REGISTER_CLASS(XBlackClub);
XBlackClub::XBlackClub() : XWeapon(IT_CLUB)
{
	strcpy(name, "black club");
	it = IT_CLUB;
	view = '/';
	weight = 8000;
	value = 10000;
	color = xDARKGRAY;
	_DV = -10;
	_PV = 0;
	_HIT = -15;
	dice.Setup(2, 20, 0);
	RNG = 0;
	if (r)
		delete r;
	if (s)
		delete s;
	r = new XResistance("fire:0d0+50");
	s = new XStats("");
	brt = (BRAND_TYPE)(BR_FIRE);
	special_property = SPP_NONE;
}

void XBlackClub::toString(char * buf)
{
	GetArtifactName(buf, "club of black obsidian");
}
