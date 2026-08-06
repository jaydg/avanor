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

#include <cassert>

#include "engine/xmapobj.h"
#include "engine/xscheduler.h"
#include "game/xtime.h"

std::shared_ptr<XObject> XScheduler::Lock(const Entry& e)
{
    if (const auto* sp = std::get_if<std::shared_ptr<XObject>>(&e)) {
        return *sp;
    }

    return std::get<std::weak_ptr<XObject>>(e).lock();
}

void XScheduler::Place(Entry e)
{
    auto sp = Lock(e);
    assert(sp && sp->isValid());

    long shift;

    if (sp->ttm < XSCHEDULER_TIME_SLICE * (XSCHEDULER_STEPS_AHEAD - 1)) {
        shift = sp->ttm / XSCHEDULER_TIME_SLICE + 1;
    } else {
        shift = (XSCHEDULER_STEPS_AHEAD - 1);
    }

    long index = shift + head;

    if (index >= XSCHEDULER_STEPS_AHEAD) {
        index -= XSCHEDULER_STEPS_AHEAD;
    }

    sp->ttm -= shift * XSCHEDULER_TIME_SLICE;
    data[index].push_back(std::move(e));
}

void XScheduler::Add(XObject* p)
{
    assert(p->isValid());

    if (p->weak_from_this().expired()) {
        Place(std::shared_ptr<XObject>(p));
    } else {
        Place(p->weak_from_this());
    }
}

std::shared_ptr<XObject> XScheduler::Get()
{
    while (true) {
        int empty_count = 0;

        while (data[head].empty()) {
            _time += XSCHEDULER_TIME_SLICE;

            if (++head >= XSCHEDULER_STEPS_AHEAD) {
                head -= XSCHEDULER_STEPS_AHEAD;
                empty_count++;

                if (empty_count > XSCHEDULER_STEPS_AHEAD) {
                    return nullptr;
                }
            }

            XTime::RunTime();
        }

        auto it = data[head].begin();
        auto sp = Lock(*it);

        // Dead entry? Either the object is gone entirely (weak_ptr expired),
        // or it's still physically alive - kept that way by our own strong
        // entry, for objects the scheduler solely owns - but has already
        // Invalidate()'d itself directly (e.g. XHerbBush/XCorpse deciding
        // they're done from within Run()/Pick(), outside the normal
        // "Run() returns false" removal path). Either way, erasing the
        // entry here drops whatever ownership it held.
        if (!sp || !sp->isValid()) {
            data[head].erase(it);
            continue;
        }

        if (sp->ttm < 0) {
            return sp;
        }

        Entry entry = std::move(*it);
        data[head].erase(it);
        Place(std::move(entry));
    }
}

void XScheduler::Remove()
{
    assert(!data[head].empty());
    data[head].erase(data[head].begin());
}

void XScheduler::Store(XFile * f)
{
    f->Write(&_time, sizeof(_time));
    f->Write(&head, sizeof(head));

    for (auto & i : data) {
        // FIXME: Implement when porting saving/restoring to Cereal
        // i.StoreList(f);
    }
}

void XScheduler::Restore(XFile * f)
{
    f->Read(&_time, sizeof(_time));
    f->Read(&head, sizeof(head));

    for (auto & i : data) {
        // FIXME: Implement when porting saving/restoring to Cereal
        // i.RestoreList(f);
    }
}
