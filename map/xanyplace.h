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

#ifndef XANYPLACE_H
#define XANYPLACE_H

#include <cstring>
#include <string>
#include <vector>

#include <cereal/types/base_class.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>

#include "engine/xobject.h"
#include "helpers/rect.h"

class XMap;
class XCreature;
class XItem;
class XLocation;

class XAnyPlace : public XObject
{
    protected:
        XAnyPlace() {}
        friend class cereal::access;

    public:
        DECLARE_CREATOR(XAnyPlace, XObject);
        XAnyPlace(const XRect& _area, XLocation* _loc);
        XAnyPlace(const XRect& _area, XLocation* _loc, const char* _onEventLua);
        ~XAnyPlace() override;
        void Invalidate() override;

        virtual int onCreatureMove(XCreature* cr);
        virtual int onCreatureEnter(XCreature* cr);
        virtual int onCreatureLeave(XCreature* cr);
        virtual int onCreaturePickItem(XCreature* cr, XItem* item)
        {
            return 1;
        }

        virtual int onCreatureDropItem(XCreature* cr, XItem* item)
        {
            return 1;
        }

        virtual std::string onShowItem(XItem * item);

        XRect &GetArea()
        {
            return area;
        }

        virtual std::weak_ptr<XCreature> &GetOwner()
        {
            return owner;
        }

        void Setup(XLocation* _map);
        XLocation* location{};

        // `location` is deliberately not persisted - it, along with every
        // XMapTile::place pointer within `area`, is re-established structurally
        // by re-running Setup() (the same call the real constructors make)
        // once the owning XLocation has finished loading both `places` and
        // its map grid, mirroring the XMapObject::l/SetLocation() idiom.
        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XObject>(this));
            ar(area, owner);

            if constexpr (Archive::is_loading::value) {
                std::string event;
                ar(event);

                if (event.empty()) {
                    onEventLua = nullptr;
                } else {
                    onEventLua = new char[event.size() + 1];
                    std::memcpy(onEventLua, event.c_str(), event.size() + 1);
                }

                // lua_ints must be read back before firing: RestoreInt
                // hands its contents out sequentially as the handler runs.
                ar(lua_ints);
                NotifyLuaEvent(true);
            } else {
                // Cleared first in case this place was saved before,
                // earlier in the same run - firing appends to it via
                // StoreInt, so a stale leftover would double up.
                lua_ints.clear();
                NotifyLuaEvent(false);
                ar(std::string(onEventLua ? onEventLua : ""));
                ar(lua_ints);
            }
        }

    protected:
        XRect area;
        std::weak_ptr<XCreature> owner;
        char* onEventLua{};

        // Backing store for onEventLua's Lua StoreInt/RestoreInt calls
        // (see NotifyLuaEvent()) - filled by StoreInt during LuaEvent::SAVE,
        // serialized, then read back and handed out via RestoreInt
        // during LuaEvent::LOAD.
        std::vector<int> lua_ints;

    private:
        // Takes a bool rather than the LuaEvent enum (LuaEvent::LOAD/LuaEvent::SAVE) to
        // avoid needing game/location.h's full declaration here - that
        // header already includes this one, so pulling it in would be
        // circular. Maps to LuaEvent::LOAD/LuaEvent::SAVE in the .cpp, where
        // game/location.h is fully visible.
        //
        // Not const: hands lua_ints out as a mutable buffer for StoreInt
        // to append to (see game/location.h/.cpp).
        void NotifyLuaEvent(bool is_load);
};

#endif
