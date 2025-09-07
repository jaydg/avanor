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

#include <assert.h>

#include "engine/xmapobj.h"
#include "engine/xscheduler.h"
#include "game/xtime.h"

void XScheduler::Place(XObject * p)
{
    assert(p->isValid());
    assert(dynamic_cast<XObject*>(p));

    long shift, index;

    if (p->ttm < XSCHEDULER_TIME_SLICE * (XSCHEDULER_STEPS_AHEAD - 1)) {
        shift = p->ttm / XSCHEDULER_TIME_SLICE + 1;
    } else {
        shift = (XSCHEDULER_STEPS_AHEAD - 1);
    }

    index = shift + head;

    if (index >= XSCHEDULER_STEPS_AHEAD) {
        index -= XSCHEDULER_STEPS_AHEAD;
    }

    p->ttm -= shift * XSCHEDULER_TIME_SLICE;
    data[index].push_back(p);
}

void XScheduler::Add(XObject * p)
{
    assert(p->isValid());
    assert(dynamic_cast<XObject*>(p));
    p->AddRef();
    Place(p);
}

XObject* XScheduler::Get()
{
    while (1) {
        int empty_count = 0;

        while (data[head].empty()) {
            _time += XSCHEDULER_TIME_SLICE;

            if (++head >= XSCHEDULER_STEPS_AHEAD) {
                head -= XSCHEDULER_STEPS_AHEAD;
                empty_count++;

                if (empty_count > XSCHEDULER_STEPS_AHEAD) {
                    return NULL;
                }
            }

            XTime::RunTime();
        }

        XObject * p = data[head].begin();
        assert(dynamic_cast<XObject*>(p));

        if (p->ttm < 0) {
            return p;
        }

        data[head].pop_front();
        Place(p);
    }
}

XObject* XScheduler::Remove()
{
    assert(!data[head].empty() && data[head].begin()->isValid());
    assert(data[head].begin()->ttm < 0);
    XObject * p = data[head].begin();
    data[head].pop_front();
    return p;
}

void XScheduler::Store(XFile * f)
{
    f->Write(&_time, sizeof(_time));
    f->Write(&head, sizeof(head));

    XList<XObject*>::iterator it;

    for (int i = 0; i < XSCHEDULER_STEPS_AHEAD; i++) {
        data[i].StoreList(f);
    }
}

void XScheduler::Restore(XFile * f)
{
    f->Read(&_time, sizeof(_time));
    f->Read(&head, sizeof(head));

    for (int i = 0; i < XSCHEDULER_STEPS_AHEAD; i++) {
        data[i].RestoreList(f);
    }
}
