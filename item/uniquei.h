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

#ifndef __UNIQUEI_H
#define __UNIQUEI_H

#include "incl_i.h"

class XGlamdring : public XWeapon
{
public:
	XGlamdring();
	DECLARE_CREATOR(XGlamdring, XWeapon);
	virtual int Compare(XObject * o) {return -1;}
	virtual void toString(char * buf);
};

class XDeathHack : public XWeapon
{
public:
	XDeathHack();
	DECLARE_CREATOR(XDeathHack, XWeapon);
	virtual int Compare(XObject * o) {return -1;}
	virtual void toString(char * buf);
};

class XAvanorDefender : public XWeapon
{
public:
	XAvanorDefender();
	DECLARE_CREATOR(XAvanorDefender, XWeapon);
	virtual int Compare(XObject * o) {return -1;}
	virtual void toString(char * buf);
	static XGUID avanordefender_guid;
	void Restore(XFile * f);
};

class XForestBrotherCloak : public XCloak
{
public:
	XForestBrotherCloak();
	virtual void toString(char * buf);
	DECLARE_CREATOR(XForestBrotherCloak, XCloak);
	virtual int Compare(XObject * o) {return -1;}
};

class XGreatElementalRing : public XRing
{
public:	
	XGreatElementalRing();
	DECLARE_CREATOR(XGreatElementalRing, XRing);
	virtual int Compare(XObject * o) {return -1;}
};

class XAvanorScepter : public XWeapon
{
public:
	XAvanorScepter();
	DECLARE_CREATOR(XAvanorScepter, XWeapon);
	virtual int Compare(XObject * o) {return -1;}
	virtual void toString(char * buf);
};

class XAvanorCrown : public XCap
{
public:
	XAvanorCrown();
	DECLARE_CREATOR(XAvanorCrown, XCap);
	virtual int Compare(XObject * o) {return -1;}
	virtual void toString(char * buf);
};

class XAvanorMitre : public XCap
{
public:
	XAvanorMitre();
	DECLARE_CREATOR(XAvanorMitre, XCap);
	virtual int Compare(XObject * o) {return -1;}
	virtual void toString(char * buf);
};

class XTorinAxe : public XWeapon
{
public:
	XTorinAxe();
	DECLARE_CREATOR(XTorinAxe, XWeapon);
	virtual int Compare(XObject * o) {return -1;}
	virtual void toString(char * buf);
};


class XTorinShield : public XShield
{
public:
	XTorinShield();
	DECLARE_CREATOR(XTorinShield, XShield);
	virtual int Compare(XObject * o) {return -1;}
	virtual void toString(char * buf);
};

class XDwarfCrown : public XCap
{
public:
	XDwarfCrown();
	DECLARE_CREATOR(XDwarfCrown, XCap);
	virtual int Compare(XObject * o) {return -1;}
	virtual void toString(char * buf);
};

class XBlackClub : public XWeapon
{
public:
	XBlackClub();
	DECLARE_CREATOR(XBlackClub, XWeapon);
	virtual int Compare(XObject * o) {return -1;}
	virtual void toString(char * buf);
};


#endif
