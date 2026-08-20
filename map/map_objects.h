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

#include <sol/forward.hpp>

#include "creature/deity.h"
#include "item/item.h"
#include "engine/xmapobj.h"

class XLocation;

//////////////////////////////////////////////////////////////////////
//XTrap
/////////////////////////////////////////////////////////////////////
class XTrap final : public XMapObject
{
    public:
        // What the trap does when it goes off. RANDOM is not a trap but
        // an instruction to pick one, the way the generators ask for it.
        enum class Type {
            UNKNOWN = -1,
            MAGICARROW = 0,
            FIREBOLT,
            ACIDBOLT,
            ARROW,
            TELEPORT,
            PIT,
            SPEAR_PIT,
            RANDOM
        };

        // How dangerous, and how well hidden. RANDOM again means "pick".
        enum class Level {
            VERY_LOW = 0,
            LOW = 1,
            AVG = 2,
            HI = 3,
            VERY_HI = 4,
            MASTERWORK = 5,
            RANDOM
        };

    private:
        Type trap_type = Type::UNKNOWN;
        Level trap_level;

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

        XTrap(int _x, int _y, XLocation* _l, XTrap::Level tl = XTrap::Level::RANDOM, XTrap::Type tt = XTrap::Type::RANDOM, XCreature* _owner = nullptr, XItem* items = nullptr);

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
        // Which way the stairway leads, which is also what it looks like
        // ('<' or '>'). UNKNOWN builds a stairway that is not drawn and
        // not placed on the map.
        enum class Type {
            UNKNOWN,
            UP,
            DOWN
        };

        // Registers this enum as the Lua table XStairWay.MEMBER.
        static void RegisterLua(sol::state_view& lua);

        DECLARE_CREATOR(XStairWay, XMapObject);
        XStairWay(int _x, int _y, XLocation* loc, const std::string& _ln, Type type);
        // The id of the location this stairway leads to.
        std::string ln;

        // Where this stairway comes out: the matching stairway's position
        // within location ln. Negative until Bind() pairs the two ends, which
        // is what XGame's way-binding pass tests for.
        int dest_x, dest_y;

        void Bind(XStairWay* way);

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XMapObject>(this));
            ar(ln, dest_x, dest_y);
        }

        const std::string GetName(XCreature *viewer) override
        {
            return "a stairway";
        }

    protected:
        XStairWay() : dest_x(-1), dest_y(-1) {
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
        XTeleport(int _x, int _y, XLocation* loc, const std::string& _ln, int _dest_x, int _dest_y);
        // The id of the location this pad drops you in.
        std::string ln;

        // Where this pad drops you: a position within location ln. Fixed
        // at construction, unlike a stairway's, which is paired up later.
        int dest_x, dest_y;

        int MoveIn(XCreature* cr);

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XMapObject>(this));
            ar(ln, dest_x, dest_y);
        }

        const std::string GetName(XCreature *viewer) override
        {
            return "a magic circle";
        }

    protected:
        XTeleport() : dest_x(-1), dest_y(-1) {
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

        // Places the altar and gives it its glyph, name and default colour,
        // so a bare PlaceSpecial("XAltar", x, y) from script yields a usable
        // altar. The colour is not decoration: it is how an altar's deity is
        // recorded, so script picks the deity by calling SetView() afterwards.
        bool PlaceAt(XLocation* location, int _x, int _y) override;

        // Pre-existing behaviour, not a shared_ptr-migration regression:
        // `deity` is only ever used to pick a color at construction time
        // (see the .cpp) and isn't kept as a member anywhere on this
        // class - the base's `color` field (already covered by
        // XMapObject::serialize()) is the only trace of it that
        // survives, same as the existing (also base-only) Store/Restore.
        // XReligion::SacrificeItem() reads that colour back to decide
        // which deity an altar belongs to, so colour *is* the deity here.
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
        std::string onEventLua;
    public:
        DECLARE_CREATOR(XOuterObject, XMapObject);
        XOuterObject(int _x, int _y, int _c, char _v, const char* subscr, XLocation* _l, const char* event);
        int onOuterUse(XCreature* cr) override;

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
            ar(onEventLua);
        }

    protected:
        XOuterObject() = default;
        friend class cereal::access;
};

#endif
