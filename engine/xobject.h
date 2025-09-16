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

#ifndef __XOBJECT_H
#define __XOBJECT_H

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <string>

#include "engine/defs.h"
#include "engine/xfile.h"

enum ITEM_MASK {
    IM_UNKNOWN = 0x00000000,
    IM_HERO = 0x00000001, // let it's be
    IM_CREATURE = 0x00000002,
    IM_MONSTER = IM_HERO | IM_CREATURE,
    IM_WAY = 0x00000004,
    IM_DOOR = 0x00000008,
    IM_TRAP = 0x00000010,
    IM_TELEPORT = 0x00000020,
    IM_ALTAR = 0x00000040,
    IM_MISC = 0x00000080,

    IM_HAT = 0x00000100,
    IM_NECK = 0x00000200,
    IM_BODY = 0x00000400,
    IM_CLOAK = 0x00000800,
    IM_WEAPON = 0x00001000,
    IM_SHIELD = 0x00002000,
    IM_HAND = IM_WEAPON | IM_SHIELD, // for a bodyparts
    IM_GLOVES = 0x00004000,
    IM_RING = 0x00008000,
    IM_BOOTS = 0x00010000,
    IM_MISSILEW = 0x00020000,
    IM_MISSILE = 0x00040000,
    IM_POTION = 0x00100000,
    IM_SCROLL = 0x00200000,
    IM_BOOK = 0x00400000,
    IM_WAND = 0x00800000,
    IM_FOOD = 0x01000000,
    IM_HERB = 0x02000000,
    IM_LIGHTSOURCE = 0x04000000,
    IM_TOOL = 0x08000000,
    IM_GEM = 0x10000000,
    IM_MONEY = 0x20000000,
    IM_STACKABLE = 0x40000000, // for spells
    IM_CHEST = 0x80000000,
    IM_ITEM = 0x2FFFFF00, // all items!

    IM_OTHER = 0x80000000,
    IM_TOHIT = IM_HAT | IM_NECK | IM_BODY | IM_CLOAK | IM_GLOVES | IM_SHIELD | IM_BOOTS | IM_RING | IM_WEAPON,
    IM_ARMOUR = IM_HAT | IM_BODY | IM_CLOAK | IM_GLOVES | IM_SHIELD | IM_BOOTS,
    IM_VALUEDICE = IM_WEAPON | IM_MISSILEW | IM_MISSILE,
    IM_VALUEDVPV = IM_HAT | IM_BODY | IM_CLOAK | IM_GLOVES | IM_SHIELD | IM_BOOTS | IM_WEAPON,
    IM_VALUEHITDMG = IM_HAT | IM_BODY | IM_CLOAK | IM_GLOVES | IM_BOOTS | IM_WEAPON,
    IM_ALL = 0xFFFFFFFF
};


class XObject;
// next code is for creating class by it name
typedef XObject* (*CLASS_CREATOR)();

#define REGISTER_CLASS(__xClass) \
    XClassFactory reg##__xClass(#__xClass, (CLASS_CREATOR)__xClass::Creator, (CLASS_CREATOR)__xClass::MakeNew)

struct DUMMY_STRUCT {
};

#define DECLARE_CREATOR(__xClass, __xBaseClass) \
    __xClass(DUMMY_STRUCT * ds) : __xBaseClass(ds) {} \
    static __xClass * Creator() {DUMMY_STRUCT ds; return new __xClass(&ds);} \
    static __xClass * MakeNew() { return new __xClass(); } \
    virtual const std::string GetClassName() {return #__xClass;}

class XClassInfo
{
    public:
        XClassInfo(const std::string _name, CLASS_CREATOR p, CLASS_CREATOR n)
        {
            name = _name;
            pClassCreator = p;
            pClassNew = n;
            next = NULL;
        }

        std::string name;
        CLASS_CREATOR pClassCreator;
        CLASS_CREATOR pClassNew;
        XClassInfo* next;
};

class XClassFactory
{
    public:
        static XClassInfo* first_class;
        static int counter;
        XClassFactory(const std::string name, CLASS_CREATOR pClass, CLASS_CREATOR pClassNew);
        ~XClassFactory();
        static XObject* Create(const std::string name);
        static XObject* CreateNew(const std::string name);
};

#define DYNCREATE(x) XClassFactory::Create(x)
// end



typedef unsigned long XGUID;
extern XGUID guid;

typedef std::map<XGUID, class XObject*> XObjectMap;

// this class is a base class for most important part's of cernel
class XObject
{
    private:
        // reference count
        // objects can't be deleted till reference > 0
        int reference;
        int is_valid;

        // counter of deleted objects
        static long invalid_count;

        // used during saving
        bool bAlreadyStored;

        // all objects have a global unique inditifer
        // (it has no sense to store pointers)
        XGUID xguid;

    protected:
        // object registry
        static XObjectMap objects;
        // used in XGame::RunWithoutHero
        friend class XGame;

        // required by XScheduler and modifiers
        int ttm; // time to move
        int ttmb; // basis of time to move
        friend class XScheduler;
        friend class XModBoostSpeed;
        friend class XModSlowness;

    public:
        // some objects can be stacked to a bundle: "bundle of 23 arrow (1d4)"
        int quantity;

        // many years ago it was item mask, now it is mask for all!
        ITEM_MASK im;

        const XGUID guid()
        {
            return xguid;
        }

        void Create()
        {
            reference = 0;
            objects[xguid] = this;
        }

        static void StorePointer(XFile * f, XObject * p);
        static XObject* RestorePointer(XFile * f, void* owner);
        static void StoreAllObjects(XFile * f);
        static void RestoreAllObjects(XFile * f);
        static void InvalidateAllObjects();

        XObject(DUMMY_STRUCT * ds) : is_valid(1)
        {
            Create();
        }

        XObject() : xguid(::guid++), quantity(1), im(IM_UNKNOWN), is_valid(1)
        {
            Create();
        }

        XObject(XObject * o) : xguid(::guid++), quantity(o->quantity), im(o->im), is_valid(1),	ttm(o->ttm), ttmb(o->ttmb)

        {
            Create();
        }

        virtual ~XObject()
        {
            assert(!is_valid && reference == 0);
            invalid_count--;
        }

        void AddRef()
        {
            reference++;
        }

        void Release()
        {
            assert(reference > 0);

            if (--reference == 0 && !is_valid) {
                delete this;
            }
        }

        int GetRef()
        {
            return reference;
        }

        virtual void Invalidate()
        {
            if (!is_valid) {
                return;
            }

            is_valid = 0;
            objects.erase(xguid);

            if (reference == 0) {
                delete this;
            }
        }

        virtual int Compare(XObject * o)
        {
            return 1;
        }

        virtual void Concat(XObject * o)
        {
            assert(o->reference == 0);
            quantity += o->quantity;
            o->Invalidate();
        }

        virtual const std::string GetClassName()
        {
            return "XObject";
        }

        XObject* MakeNew()
        {
            assert(0);
            return NULL;
        }

        virtual void Dump(XFile * f);
        static void DumpAll();

        int isValid()
        {
            return is_valid;
        }

        // interface for store/restore functions
        virtual void Store(XFile * f);// = 0;
        virtual void Restore(XFile * f);// = 0;

        // Runable object...
        // If It returns zero, than object must be removed from scheduler
        virtual int Run()
        {
            return 1;
        }

        template<typename T>
        static int StoreObjectMap(XFile* f, const T& map)
        {
            using key_type = typename T::key_type;

            size_t written = 0;
            size_t count = map.size();
            f->Write(&count, sizeof(count));

            for (const auto& [key, obj] : map) {
                written += f->Write(&key, sizeof(key_type));
                StorePointer(f, obj);
            }

            return written;
        }

        template<typename T>
        static int RestoreObjectMap(XFile *f, T& map) {
            using key_type = typename T::key_type;
            using val_type = typename T::mapped_type;

            assert(map.empty());

            size_t read = 0;
            size_t count = 0;
            f->Read(&count, sizeof(count));

            while (count-- > 0) {
                key_type key;

                read += f->Read(&key, sizeof(key_type));
                map[key] = static_cast<val_type>(RestorePointer(f, NULL));
            }

            return read;
        }
};

template<class TYPE> class XPtr
{
        mutable XObject* p;
        void operator=(const XPtr<TYPE> &);
        XPtr(const XPtr<TYPE> &);
        TYPE* get_with_check() const;
    public:
        XPtr() : p(NULL) { }

        XPtr(TYPE * _p) : p(_p)
        {
            if (p != NULL) {
                p->AddRef();
            }
        }

        ~XPtr();

        bool operator==(XPtr<TYPE> &x) const
        {
            return operator TYPE * () == x.operator TYPE * ();
        }

        void operator=(TYPE * _p);

        operator TYPE * () const
        {
            return (p == NULL) ? NULL : get_with_check();
        }

        TYPE* get() const
        {
            return static_cast<TYPE*>(p);
        }

        TYPE * operator->() const
        {
            return static_cast<TYPE*>(p);
        }

        void Store(XFile * f) const;
        void Restore(XFile * f);
};

template<class TYPE> XPtr<TYPE>::~XPtr()
{
    if (p != NULL) {
        p->Release();
    }
}

template<class TYPE> void XPtr<TYPE>::operator=(TYPE * _p)
{
    if (p == (XObject*)_p) {
        return;
    }

    if (p != NULL) {
        p->Release();
    }

    p = (XObject*)_p;

    if (p != NULL) {
        p->AddRef();
    }
}

template<class TYPE> TYPE* XPtr<TYPE>::get_with_check() const
{
    if (p->isValid()) {
        return static_cast<TYPE*>(p);
    }

    p->Release();
    p = NULL;
    return NULL;
}

template<class TYPE> void XPtr<TYPE>::Store(XFile * f) const
{
    XObject::StorePointer(f, operator TYPE * ());
}

template<class TYPE> void XPtr<TYPE>::Restore(XFile * f)
{
    assert(p == NULL);
    p = XObject::RestorePointer(f, this);

    if (p != NULL) {
        p->AddRef();
    }

    assert(p == NULL || dynamic_cast<TYPE*>(p));
}

#endif
