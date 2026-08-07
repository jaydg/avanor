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

#ifndef XSCHEDULER_H
#define XSCHEDULER_H

#include <memory>
#include <variant>
#include <vector>

#include <cereal/types/memory.hpp>
#include <cereal/types/variant.hpp>
#include <cereal/types/vector.hpp>

constexpr int XSCHEDULER_TIME_SLICE = 100;
constexpr int XSCHEDULER_STEPS_AHEAD = 100;

class XMapObject;

class XScheduler
{
        // An object added while nothing else owns it yet (e.g. a freshly
        // created generator) is strongly owned right here - the scheduler
        // becomes its sole owner for as long as it stays scheduled. An
        // object added while something else already owns it (a creature via
        // its map cell, a location via Game.locations[]) is only weakly
        // observed.
        using Entry = std::variant<std::shared_ptr<XObject>, std::weak_ptr<XObject>>;

        long _time, head;
        std::vector<Entry> data[XSCHEDULER_STEPS_AHEAD];

        static std::shared_ptr<XObject> Lock(const Entry& e);
        void Place(Entry e);

    public:
        XScheduler() : _time(0), head(0) { }

        ~XScheduler()
        {
            while (Get()) {
                Remove();
            }
        }

        void SetTime(long t)
        {
            _time = t;
        }

        [[nodiscard]] long GetTime() const
        {
            return _time;
        }

        void Add(XObject* p);
        std::shared_ptr<XObject> Get();
        void Remove();

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(_time, head);

            for (auto& d : data) {
                ar(d);
            }
        }

};

#endif
