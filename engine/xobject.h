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

#ifndef XOBJECT_H
#define XOBJECT_H

#include <cassert>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>

#include <cereal/access.hpp>
#include <cereal/cereal.hpp>
#include <sol/forward.hpp>

enum ITEM_MASK {
    IM_UNKNOWN = 0x00000000,

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
    IM_OTHER = 0x02000000, // reuses the bit IM_HERB used to occupy
    IM_LIGHTSOURCE = 0x04000000,
    IM_TOOL = 0x08000000,
    IM_GEM = 0x10000000,
    IM_MONEY = 0x20000000,
    IM_STACKABLE = 0x40000000, // for spells
    IM_CHEST = 0x80000000,
    IM_ITEM = 0x2FFFFF00, // all items!

    IM_TOHIT = IM_HAT | IM_NECK | IM_BODY | IM_CLOAK | IM_GLOVES | IM_SHIELD | IM_BOOTS | IM_RING | IM_WEAPON,
    IM_ARMOUR = IM_HAT | IM_BODY | IM_CLOAK | IM_GLOVES | IM_SHIELD | IM_BOOTS,
    IM_VALUEDICE = IM_WEAPON | IM_MISSILEW | IM_MISSILE,
    IM_VALUEDVPV = IM_HAT | IM_BODY | IM_CLOAK | IM_GLOVES | IM_SHIELD | IM_BOOTS | IM_WEAPON,
    IM_VALUEHITDMG = IM_HAT | IM_BODY | IM_CLOAK | IM_GLOVES | IM_BOOTS | IM_WEAPON,
    IM_ALL = 0xFFFFFFFF
};

// Registers this enum as the Lua table ITEM_MASK.MEMBER - see the Sol2
// plan.
void RegisterItemMaskEnum(sol::state_view& lua);

class XObject;
// next code is for creating class by it name
typedef XObject* (*CLASS_CREATOR)();

#define REGISTER_CLASS(__xClass) \
    XClassFactory reg##__xClass(#__xClass, (CLASS_CREATOR)__xClass::MakeNew)

struct DUMMY_STRUCT {
};

// The DUMMY_STRUCT constructor is still real, load-bearing machinery:
// CEREAL_LOAD_VIA_DUMMY_CONSTRUCT below calls it directly to construct
// polymorphic objects during Cereal load, bypassing whatever
// inaccessible/asserting no-args constructor the class has otherwise.
// The old Creator() static wrapper that used to be the only caller of
// this constructor (via the legacy, now-removed XClassFactory::Create()/
// RestoreAllObjects()) is gone - MakeNew() (used by Location.CreateObject,
// a still-active Lua binding for runtime object creation by class name)
// is the only factory method left.
#define DECLARE_CREATOR(__xClass, __xBaseClass) \
    explicit __xClass(DUMMY_STRUCT * ds) : __xBaseClass(ds) {} \
    static __xClass * MakeNew() { return new __xClass(); } \
    virtual const std::string GetClassName() override {return #__xClass;}

// For XObject-derived classes whose only accessible no-args constructor
// exists purely as a legacy convenience or deliberately asserts (the
// type is never meant to be default-constructed in real gameplay) -
// Cereal's default polymorphic construction strategy would call that
// constructor directly during load, either failing (protected/private)
// or actively crashing (an assert(0) guard). Route construction through
// the same DUMMY_STRUCT idiom DECLARE_CREATOR already uses for exactly
// this purpose instead: skip real initialization, then let the class's
// own serialize()/load() populate every field (inherited ones included)
// immediately afterward, same as the legacy Store/Restore factory did.
#define CEREAL_LOAD_VIA_DUMMY_CONSTRUCT(__xClass, __method) \
    namespace cereal { \
        template<> struct LoadAndConstruct<__xClass> { \
            template<class Archive> \
            static void load_and_construct(Archive& ar, cereal::construct<__xClass>& construct) \
            { \
                DUMMY_STRUCT ds; \
                construct(&ds); \
                construct->__method(ar); \
            } \
        }; \
    }

// Same idea as CEREAL_LOAD_VIA_DUMMY_CONSTRUCT, for non-XObject classes
// that don't have the DECLARE_CREATOR/DUMMY_STRUCT machinery: construct
// via the class's own real constructor with placeholder arguments,
// skipping whatever assert(0)-guarded or inaccessible no-args
// constructor it has, then let its own serialize()/load() populate every
// field immediately afterward.
#define CEREAL_LOAD_VIA_PLACEHOLDER_CONSTRUCT(__xClass, __method, ...) \
    namespace cereal { \
        template<> struct LoadAndConstruct<__xClass> { \
            template<class Archive> \
            static void load_and_construct(Archive& ar, cereal::construct<__xClass>& construct) \
            { \
                construct(__VA_ARGS__); \
                construct->__method(ar); \
            } \
        }; \
    }

class XClassInfo
{
    public:
        XClassInfo(const std::string _name, CLASS_CREATOR n)
        {
            name = _name;
            pClassNew = n;
            next = nullptr;
        }

        std::string name;
        CLASS_CREATOR pClassNew;
        XClassInfo* next;
};

class XClassFactory
{
    public:
        static XClassInfo* first_class;
        static int counter;
        XClassFactory(const std::string& name, CLASS_CREATOR pClassNew);
        ~XClassFactory();
        static XObject* CreateNew(const std::string &name);
};
// end

typedef unsigned long XGUID;
extern XGUID guid;

typedef std::map<XGUID, class XObject*> XObjectMap;

// The base class for most important parts of the game
class XObject : public std::enable_shared_from_this<XObject>
{
    private:
        // reference count
        // objects can't be deleted till reference > 0
        int reference;
        int is_valid;

        // counter of deleted objects
        static long invalid_count;

        // all objects have a global unique identifier
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

        static void InvalidateAllObjects();
        static XObject* GetObject(XGUID guid) {
            if (const auto it = objects.find(guid); it != objects.end()) {
                return objects[guid];
            }

            return nullptr;
        }

        // xguid must be initialized here, not left to whatever garbage
        // was already in this object's freshly-allocated memory:
        // Create() below inserts into `objects` keyed by xguid, and
        // serialize() (further down) later reads the pre-overwrite
        // value of xguid to know which key to erase when it re-keys
        // this entry to the real, persisted guid. Uninitialized memory
        // being zero more often than not (fresh heap pages typically
        // are) meant many unrelated objects shared the same "old" key -
        // each one's serialize() call erasing entry 0 in turn, wiping
        // out whichever real, legitimately-numbered object actually
        // belonged there. Assigning a fresh ::guid++ here instead
        // guarantees every DUMMY_STRUCT-constructed object starts under
        // a unique key of its own, exactly like the normal constructor.
        XObject(DUMMY_STRUCT * ds) : xguid(::guid++), is_valid(1)
        {
            Create();
        }

        XObject() : xguid(::guid++), im(IM_UNKNOWN), is_valid(1)
        {
            Create();
        }

        XObject(XObject * o) : xguid(::guid++), im(o->im), is_valid(1),	ttm(o->ttm), ttmb(o->ttmb)

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

            if (--reference == 0 && !is_valid && weak_from_this().expired()) {
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

            // Erase by identity, not just by key: a Cereal-restored
            // object can end up assigned the same xguid as some other
            // still-live object already registered under that key (the
            // restore path re-keys `objects` to the value read from the
            // archive - see serialize() below - which says nothing about
            // whether that key was already taken). Blindly erasing by
            // key here would remove the OTHER object's registry entry
            // instead of this one's, whenever that's happened.
            if (auto it = objects.find(xguid); it != objects.end() && it->second == this) {
                objects.erase(it);
            }

            // Objects that have been wrapped in a shared_ptr somewhere (the
            // ones migrated off this legacy refcount) must never be deleted
            // here - their lifetime is governed entirely by shared_ptr
            // refcounting from this point on, and something (a container, a
            // scheduler weak_ptr's lock(), a shared_from_this() guard higher
            // up the call stack) is guaranteed to still be holding this
            // object alive for as long as we're inside this call anyway.
            if (reference == 0 && weak_from_this().expired()) {
                delete this;
            }
        }

        virtual int Compare(XObject * o)
        {
            return 1;
        }

        virtual const std::string GetClassName()
        {
            return "XObject";
        }

        // reference/is_valid are runtime bookkeeping, not persisted
        // state - always reset by Create() on construction.
        //
        // On load, the constructor's Create() already inserted this
        // object into `objects` under whatever garbage xguid it had
        // before the archive overwrites it below (the DUMMY_STRUCT/
        // placeholder constructors used for Cereal's polymorphic
        // construction don't initialize xguid the way the normal
        // constructor does) - so the map entry has to be re-keyed
        // here, or it's left dangling under a stale key that a later
        // full sweep (e.g. InvalidateAllObjects()) walks into.
        template<class Archive>
        void serialize(Archive& ar)
        {
            const XGUID old_guid = xguid;

            ar(
                cereal::make_nvp("guid", xguid),
                im,
                ttm,
                ttmb
            );

            if constexpr (Archive::is_loading::value) {
                if (old_guid != xguid) {
                    objects.erase(old_guid);

                    // Never steal an occupied slot from a different,
                    // already-registered live object - in normal
                    // production use `objects` starts empty before a
                    // restore, so this never collides, but a restore
                    // that runs while an object using this exact xguid
                    // is still alive (e.g. a round-trip test probing a
                    // live game) must not silently evict it: it would
                    // vanish from InvalidateAllObjects()'s sweep for
                    // good, even though it's still alive and reachable
                    // elsewhere.
                    if (auto it = objects.find(xguid); it == objects.end()) {
                        objects[xguid] = this;
                    }
                }
            }
        }

        int isValid()
        {
            return is_valid;
        }

        // Runnable object.
        // If it returns false, the object must be removed from the scheduler.
        virtual bool Run()
        {
            return true;
        }
};

#endif
