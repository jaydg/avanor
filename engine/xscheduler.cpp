/*
This file is part of "Avanor, the Land of Mystery" roguelike game

Copyright (C) 2000-2006 Vadim Gaidukevich
Copyright (C) 2025,2026 Joachim de Groot

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
#include "item/item.h"

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
        // An XItem can reach here before it's ever been placed in any
        // XItemList (e.g. XCorpse registers with the scheduler from within
        // its own constructor) - in that case this Add() call is the
        // item's actual birth. Route it through XItem::Own() rather than a
        // plain shared_ptr, so the deleter that ends up fixed in the shared
        // control block is the one that knows to call Invalidate() - a bare
        // delete here would skip that entirely. contain/item_list inserting
        // the same item later just shares this same control block via
        // shared_from_this().
        if (auto* item = dynamic_cast<XItem*>(p)) {
            Place(std::shared_ptr<XObject>(XItem::Own(item)));
        } else {
            Place(std::shared_ptr<XObject>(p));
        }
    } else {
        Place(p->weak_from_this());
    }
}

std::shared_ptr<XObject> XScheduler::Get()
{
    while (true) {
        // Counts whole sweeps of the ring that turned up nothing at all,
        // and gives up below when there have been enough of them. It is
        // declared here, inside the outer loop, so that it starts again
        // from nothing every time something IS found - a dead entry to
        // erase, or an object to move further ahead. That reads like a
        // counter that never gets anywhere, and it is not: the question
        // it answers is "has the whole ring been empty for a while", and
        // work of any kind is the answer being no. Hoisting it out of
        // this loop would make a busy scheduler eventually declare itself
        // finished. In a two hundred second soak it reached one, once.
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
        // Invalidate()'d itself directly (e.g. a Lua map object or XCorpse deciding
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

