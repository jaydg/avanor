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
#include "xshedule.h"
#include "xmapobj.h"
#include "xtime.h"


XQuickRing::XQuickRing()
{
	size = 1024;
	begin = 0;
	end = 0;
	data = new XMapObject*[size];
}

XQuickRing::~XQuickRing()
{
	delete[] data;
}

void XQuickRing::Resize()
{
	XMapObject ** tmp = new XMapObject*[size * 2];
	memcpy(tmp, data + begin, (size - begin) * sizeof(XMapObject*));
	memcpy(tmp + size - begin, data, begin * sizeof(XMapObject*));
	delete[] data;
	data = tmp;
	begin = 0;
	end = size;
	size = size * 2;
}


void XQuickRing::PushBack(XMapObject * pObj)
{
	assert(pObj->isValid());
	pObj->AddRef();
	data[end] = pObj;
	end++;
	if (end >= size)
		end = 0;
	if (end == begin)
		Resize();
}


XMapObject * XQuickRing::PopFront()
{
	assert(begin != end);
	XMapObject * tmp = data[begin];
	begin++;
	if (begin >= size)
		begin = 0;
	if (!tmp->isValid())
	{
		tmp->Release();
		return PopFront();
	}
	else
	{
		tmp->Release();
		return tmp;
	}
}

XMapObject * XQuickRing::Front()
{
	if (!data[begin]->isValid())
	{
		data[begin]->Release();
		begin++;
		if (begin >= size)
			begin = 0;
		return Front();
	}
	return data[begin];
}

bool XQuickRing::isEmpty()
{
	if (begin == end)
		return true;
	while (begin != end)
	{
		if (data[begin]->isValid())
			return false;
		data[begin]->Release();
		begin++;
		if (begin >= size)
			begin = 0;
	}
	return begin == end;
}

void XQuickRing::StoreRing(XFile * f)
{
	int i;
	if (begin < end)
	{
		int ts = end - begin;
		f->Write(&ts);
		for (i = begin; i <= end; i++)
			data[i]->Store(f);
	} else
	{
		int ts = size - begin + end;
		f->Write(&ts);
		for (i = begin; i < size; i++)
			data[i]->Store(f);

		for (i = 0; i < end; i++)
			data[i]->Store(f);
	}
}

void XQuickRing::RestoreRing(XFile * f)
{
	int ts;
	f->Read(&ts);
	for (int i = 0; i < ts; i++)
	{
		XMapObject * p = (XMapObject *)XObject::RestorePointer(f, NULL);
		PushBack(p);
	}
}


void XSheduler::Place(XObject * p)
{
	assert(p->isValid());
	assert(dynamic_cast<XObject *>(p));

	long shift, index;

	if (p->ttm < XSHEDULER_TIME_SLICE * (XSHEDULER_STEPS_AHEAD - 1))
		shift = p->ttm / XSHEDULER_TIME_SLICE + 1; 
	else
		shift = (XSHEDULER_STEPS_AHEAD - 1); 

	index = shift + head;
	if (index >= XSHEDULER_STEPS_AHEAD) index -= XSHEDULER_STEPS_AHEAD;
	p->ttm -= shift * XSHEDULER_TIME_SLICE;
	data[index].push_back(p);
//	data[index].PushBack(p);
}

void XSheduler::Add(XObject * p)
{
	assert(p->isValid());
	assert(dynamic_cast<XObject *>(p));
	p->AddRef();
	Place(p);
}

XObject * XSheduler::Get()
{
	while (1)
	{
		int empty_count = 0;
		while (data[head].empty()) 
//		while (data[head].isEmpty())
   		{
   	   		_time += XSHEDULER_TIME_SLICE;
   			if (++head >= XSHEDULER_STEPS_AHEAD)
			{
				head -= XSHEDULER_STEPS_AHEAD;
				empty_count++;
				if (empty_count > XSHEDULER_STEPS_AHEAD)
					return NULL;
			}
   			XTime::RunTime();
   		}

   		XObject * p = data[head].begin();
		//XMapObject * p = data[head].Front();
		assert(dynamic_cast<XObject *>(p));

		if (p->ttm < 0) return p;
   		data[head].pop_front();
		//data[head].PopFront();
		Place(p);
	}
}

XObject * XSheduler::Remove()
{
	assert(!data[head].empty() && data[head].begin()->isValid());
	assert(data[head].begin()->ttm < 0);
	XObject * p = data[head].begin(); 
	data[head].pop_front();
	return p;
//	return data[head].PopFront();
}

void XSheduler::Store(XFile * f)
{
	f->Write(&_time, sizeof(_time));
	f->Write(&head, sizeof(head));

	XList<XObject *>::iterator it;

	for (int i = 0; i < XSHEDULER_STEPS_AHEAD; i++)
	{
		data[i].StoreList(f);
		//data[i].StoreRing(f);
	}
}

void XSheduler::Restore(XFile * f)
{
	f->Read(&_time, sizeof(_time));
	f->Read(&head, sizeof(head));

	for (int i = 0; i < XSHEDULER_STEPS_AHEAD; i++)
	{
		data[i].RestoreList(f);
		//data[i].RestoreRing(f);
	}
}
