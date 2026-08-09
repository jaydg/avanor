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

#ifndef XGEN_H
#define XGEN_H

#include <cereal/types/base_class.hpp>
#include <cereal/types/memory.hpp>

#include "engine/xmapobj.h"
#include "map/map.h"

class XGenerator : public XMapObject
{
    protected:
        XGenerator() {}
        friend class cereal::access;

    public:
        DECLARE_CREATOR(XGenerator, XMapObject);
        explicit XGenerator(const int run_time)
        {
            ttmb = run_time;
            ttm = ttmb;
            x = -1;
            y = -1;
            im = IM_UNKNOWN;
        }

        bool Run() override
        {
            assert(0);
            return 0;
        }

        int Compare(XObject * o) override
        {
            return 1;
        }

        // XGenerator is never itself a dynamic type, just a link in the
        // chain for its two concrete subclasses below.
        //
        // `l` (the owning location) needs an exception to XMapObject::
        // serialize()'s usual "deliberately not persisted, re-derived
        // structurally" rule for it: that rule only holds for objects
        // actually placed into a location's MAP grid, where
        // XLocation::FixupMapObjectPositions()/XAnyPlace::Setup() calls
        // SetLocation() on every occupant after load. A generator is
        // never grid-placed - it only exists as a scheduled
        // XScheduler entry - so nothing else ever re-derives `l` for
        // it. Leaving it null after load crashes the very next time the
        // generator runs (e.g. XUniversalGen::Run() dereferencing
        // l->GetFreeXY()) - found via a real save/restore/explore
        // playtest of a live game.
        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XMapObject>(this));

            if constexpr (Archive::is_loading::value) {
                std::weak_ptr<XLocation> wl;
                ar(wl);
                l = wl.lock().get();
            } else {
                std::weak_ptr<XLocation> wl;

                if (l && l->isValid() && !l->weak_from_this().expired()) {
                    wl = std::static_pointer_cast<XLocation>(l->shared_from_this());
                }

                ar(wl);
            }
        }
};

class XUniversalGen final : public XGenerator
{
    protected:
        XUniversalGen() : crl(), crc() {
        }
        friend class cereal::access;

    public:
        DECLARE_CREATOR(XUniversalGen, XGenerator);
        XUniversalGen(XLocation * loc, CREATURE_CLASS _crc, CREATURE_LEVEL _crl, unsigned int _max_creature = 8, int refresh_time = 15000) : XGenerator(refresh_time)
        {
            l = loc;
            crl = _crl;
            crc = _crc;
            max_creature = _max_creature;
        }

        bool Run() override;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XGenerator>(this));
            ar(crl, crc, max_creature);
        }
    protected:
        CREATURE_LEVEL crl;
        CREATURE_CLASS crc;
        unsigned int max_creature{};
};

class XMainLocationGen final : public XGenerator
{
    public:
        DECLARE_CREATOR(XMainLocationGen, XGenerator);
        explicit XMainLocationGen(XLocation * loc) : XGenerator(1000)
        {
            l = loc;
            turns_count = 0;
        }

        XMainLocationGen() : turns_count(0) {
            assert(0);
        }

        bool Run() override;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XGenerator>(this));
            ar(turns_count);
        }
    protected:
        int turns_count;
};

// Construction routed via each class's own DECLARE_CREATOR-provided
// DUMMY_STRUCT constructor rather than the (protected/asserting)
// default constructor. These specializations have to live here rather
// than in xgen.cpp: they get triggered from inside XScheduler::serialize()
// (a template in xscheduler.h, reinstantiated per translation unit),
// same reasoning as XStandardAI/XSpell/etc.
CEREAL_LOAD_VIA_DUMMY_CONSTRUCT(XUniversalGen, serialize);
CEREAL_LOAD_VIA_DUMMY_CONSTRUCT(XMainLocationGen, serialize);

#endif
