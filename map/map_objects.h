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

#ifndef OTHER_MISC_H
#define OTHER_MISC_H

#include <cstring>

#include <cereal/types/base_class.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/string.hpp>

#include "creature/deity.h"
#include "item/item.h"
#include "engine/xmapobj.h"

enum TRAP_TYPE {
    TT_UNKNOWN	= -1,
    TT_MAGICARROW	= 0,
    TT_FIREBOLT,
    TT_ACIDBOLT,
    TT_ARROW,
    TT_TELEPORT,
    TT_PIT,
    TT_SPEAR_PIT,
    TT_RANDOM
};

enum TRAP_LEVEL {
    TL_VERY_LOW	= 0,
    TL_LOW	= 1,
    TL_AVG	= 2,
    TL_HI	= 3,
    TL_VERY_HI	= 4,
    TL_MASTERWORK	= 5,
    TL_RANDOM,
};

class XLocation;

//////////////////////////////////////////////////////////////////////
//XTrap
/////////////////////////////////////////////////////////////////////
class XTrap final : public XMapObject
{
        TRAP_TYPE trap_type = TT_UNKNOWN;
        TRAP_LEVEL trap_level;

        // owner to get exp.
        std::weak_ptr<XCreature> owner;

        int isVisibleForHero = 0;

        // Owning: the ammo template is created directly here and never
        // placed in any XItemList, so this is always its only reference.
        // See XItem::Own().
        std::shared_ptr<XItem> trap_item;

        bool isMagic = false;

        // required for pits
        XGUID last_activator{};
    public:
        int activation_count = 0;
        DECLARE_CREATOR(XTrap, XMapObject);
        XTrap() : trap_type(), trap_level()
        {
            assert(0);
        }

        XTrap(int _x, int _y, XLocation* _l, TRAP_LEVEL tl = TL_RANDOM, TRAP_TYPE tt = TT_RANDOM, XCreature* _owner = nullptr, XItem* items = nullptr);

        int MoveIn(XCreature * cr);

        int MoveOut(XCreature * cr);

        int Activate(XCreature * cr);

        int Check(XCreature * cr);

        int isVisible(XCreature * cr) const;

        int Disarm(XCreature * cr);
    protected:
        // teardown hook, called by XObject::Invalidate()
        void OnInvalidate() override;
    public:

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XMapObject>(this));
            ar(trap_type, trap_level, owner, trap_item, isVisibleForHero, last_activator, activation_count);
        }
};

//////////////////////////////////////////////////////////////////////
//XStairWay
/////////////////////////////////////////////////////////////////////
class XStairWay final : public XMapObject
{
    public:
        DECLARE_CREATOR(XStairWay, XMapObject);
        XStairWay(int _x, int _y, XLocation* loc, XLocation::Id _ln, STAIRWAY_TYPE type);
        XLocation::Id ln;
        int Compare(XObject* o) override
        {
            return -1;
        }

        void Bind(XStairWay* way);

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XMapObject>(this));
            ar(ln);
        }

        const std::string GetName(XCreature *viewer) override
        {
            return "a stairway";
        }

    protected:
        XStairWay() : ln() {
        }
        friend class cereal::access;
};

//////////////////////////////////////////////////////////////////////
//XTeleport
/////////////////////////////////////////////////////////////////////
class XTeleport final : public XMapObject
{
    public:
        DECLARE_CREATOR(XTeleport, XMapObject);
        XTeleport(int _x, int _y, XLocation* loc, XLocation::Id _ln, int _nx, int _ny);
        XLocation::Id ln;
        int Compare(XObject* o) override
        {
            return -1;
        }

        int MoveIn(XCreature* cr);

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XMapObject>(this));
            ar(ln);
        }

        const std::string GetName(XCreature *viewer) override
        {
            return "a magic circle";
        }

    protected:
        XTeleport() : ln() {
        }
        friend class cereal::access;
};

//////////////////////////////////////////////////////////////////////
//XDoor
//////////////////////////////////////////////////////////////////////
class XDoor final : public XMapObject
{
    public:
        DECLARE_CREATOR(XDoor, XMapObject);
        XDoor(int _x, int _y, int flg, XLocation * _l);
        void Switch();
        int isOpened;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XMapObject>(this));
            ar(isOpened);
        }

        const std::string GetName(XCreature *viewer) override
        {
            return "a door";
        }

    protected:
        XDoor() : isOpened(0) {}
        friend class cereal::access;
};

//////////////////////////////////////////////////////////////////////
//XAltar
//////////////////////////////////////////////////////////////////////
class XAltar final : public XMapObject
{
    public:
        DECLARE_CREATOR(XAltar, XMapObject);
        XAltar(int _x, int _y, XDeity::Id deity, XLocation* _l);

        // Pre-existing behaviour, not a shared_ptr-migration regression:
        // `deity` is only ever used to pick a color at construction time
        // (see the .cpp) and isn't kept as a member anywhere on this
        // class - the base's `color` field (already covered by
        // XMapObject::serialize()) is the only trace of it that
        // survives, same as the existing (also base-only) Store/Restore.
        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XMapObject>(this));
        }

        const std::string GetName(XCreature *viewer) override
        {
            return color == xWHITE ? "an altar of white granite" : "an altar of black granite";
        }

    protected:
        XAltar() {}
        friend class cereal::access;
};

//////////////////////////////////////////////////////////////////////
//XGrave
//////////////////////////////////////////////////////////////////////
class XGrave: public XMapObject
{
        int isOpened;
        XItemList hidden_items;
    public:
        DECLARE_CREATOR(XGrave, XMapObject);
        XGrave(int _x, int _y, char* subscr, XLocation* _l);
        void HideItem(XItem* item);
        int onOuterUse(XCreature* cr) override;
    protected:
        // teardown hook, called by XObject::Invalidate()
        void OnInvalidate() override;
    public:

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XMapObject>(this));
            ar(isOpened, hidden_items);
        }

    protected:
        XGrave() : isOpened(0) {}
        friend class cereal::access;
};

//////////////////////////////////////////////////////////////////////
//XFurniture
//////////////////////////////////////////////////////////////////////
class XFurniture: public XMapObject
{
    public:
        DECLARE_CREATOR(XFurniture, XMapObject);
        XFurniture(int _x, int _y, int _c, char _v, const char* subscr, XLocation* _l);

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XMapObject>(this));
        }

    protected:
        XFurniture() {}
        friend class cereal::access;
};

//////////////////////////////////////////////////////////////////////
//XOuterObject
//////////////////////////////////////////////////////////////////////
class XOuterObject final : public XMapObject
{
        char* onEventLua;
    public:
        DECLARE_CREATOR(XOuterObject, XMapObject);
        XOuterObject(int _x, int _y, int _c, char _v, const char* subscr, XLocation* _l, const char* event);
        ~XOuterObject() override;
        int onOuterUse(XCreature* cr) override;

        // onEventLua is an owned, heap-allocated C string (see the dtor)
        // rather than a std::string - Cereal has no built-in support for
        // raw char*, so save/load go through a temporary std::string.
        //
        // One symmetric serialize() rather than a split save()/load()
        // pair: as found and fixed for XCreature/XPotion/XCorpse earlier
        // this session, a split pair - even a correctly-disambiguated
        // one - silently breaks Cereal's *polymorphic type registration*
        // for the type at runtime whenever an ancestor (XMapObject here)
        // has its own plain serialize().
        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XMapObject>(this));

            if constexpr (Archive::is_loading::value) {
                std::string event;
                ar(event);

                delete[] onEventLua;

                if (event.empty()) {
                    onEventLua = nullptr;
                } else {
                    onEventLua = new char[event.size() + 1];
                    std::memcpy(onEventLua, event.c_str(), event.size() + 1);
                }
            } else {
                ar(std::string(onEventLua ? onEventLua : ""));
            }
        }

    protected:
        XOuterObject() : onEventLua(nullptr) {}
        friend class cereal::access;
};

#endif
