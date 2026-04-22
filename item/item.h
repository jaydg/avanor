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

#ifndef ITEM_H
#define ITEM_H

#include "creature/bodypart.h"
#include "engine/xbaseobj.h"
#include "engine/xlist.h"
#include "item/itemdb.h"
#include "item/itemdef.h"

#define DUR_INFINITE 65535

enum RESULT {
    FAIL,
    SUCCESS,
    CONTINUE,
    ABORT,
};

class XCreature;

class XItem : public XBaseObject
{
    protected:
        XPtr<XCreature> owner;
    public:
        DECLARE_CREATOR(XItem, XBaseObject);
        void StatsToString(char* buf);
        void MainFill(_MAIN_ITEM_STRUCT * is);
        XItem();
        XItem(XItem * copy);
        virtual XObject* MakeCopy()
        {
            assert(0);
            return NULL;
        }

        virtual void Invalidate();

        int BasicFill(ITEM_TYPE it, XItemBasicStructure * pData);
        void PropFill(ITEM_SET is, int val = 1000);
        void SpecialFill();

        virtual void toString(char* buf);

        BODY_PART bp;    // fit to what bp???
        ITEM_TYPE it;   // main type of item such IT_POTION
        WSK_TYPE wt;    // other type of item
        ITEM_QUALITY quality; // quality of item. Need for generation and may be basic identification...

        int durability; // DUR_INFINITE - infinite, Other - finite 1Dur == 1000 turn
        int ModifyDur(int val);

        int identify;    // 0 - not identifed, 1 - identifed
        int is_selected; // it is need to user interface....

        virtual int Compare(XObject * o); // compare objects
        virtual int isIdentifed()
        {
            return identify; // object can be indetifed or not
        }

        virtual void Identify(int level)
        {
            identify = level; //set object indetifed
        }

        int value;
        virtual int GetValue(); // Get Value of Object (for shop, monster AI etc.)

        SPECIAL_PROPERTY special_property; // slow digestion and other propertyes
        int special_number; // number of a struct with extension name such a "of Strength"
        BRAND_TYPE brt;
        int material_index; // reference to material;
        _ITEMPROP* GetMaterial(int index);


        void Drop(XLocation * location, int _x, int _y);
        void Pickup(XCreature * picker);

        virtual int onWear(XCreature * cr);
        virtual int onUnWear(XCreature * cr);

        virtual int onPickUp(XCreature * cr);
        virtual int onPutOn(XCreature * cr);

        virtual int onHit(XCreature * user, XCreature * target);

        virtual RESULT onUse(USE_ITEM_STATE uis, XCreature * cr)
        {
            return FAIL;
        }

        virtual void Store(XFile * f);
        virtual void Restore(XFile * f);

        bool SetOwner(XCreature * new_owner);
        XPtr<XCreature> &GetOwner()
        {
            return owner;
        }

        void UnCarry();

    protected:
        void GetFullName(char* buf, const char* templ = NULL);
        void GetArtifactName(char* buf, const char* real_name);
};

typedef XSortedList<XItem*>::iterator it_iterator;
typedef XSortedList<XItem*> XItemList;

#endif
