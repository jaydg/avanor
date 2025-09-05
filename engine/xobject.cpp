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

#include <algorithm>
#include "global.h"
#include "xdebug.h"
#include "xobject.h"

/*
#include <crtdbg.h>

struct __DEBUG_STRUCT
{
	__DEBUG_STRUCT()
	{
		int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
		tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
		_CrtSetDbgFlag(tmpDbgFlag);
	};
};

__DEBUG_STRUCT try_debug;
*/

long       XObject::invalid_count = 0;
XObject ** XObject::table = 0;   
long       XObject::count = 0;
XObject  * XObject::root  = 0;

	//REGISTER_CLASS(XObject);

XGUID guid = 1;


XClassInfo * XClassFactory::first_class = NULL;
int XClassFactory::counter = 0;

XClassFactory::XClassFactory(std::string name, CLASS_CREATOR pClassCreator, CLASS_CREATOR pClassNew)
{
	if (!first_class)
		XClassFactory::first_class = new XClassInfo(name, pClassCreator, pClassNew);
	else
	{
		XClassInfo * tmp = XClassFactory::first_class;
		while(tmp->next) tmp = tmp->next;
		tmp->next = new XClassInfo(name, pClassCreator, pClassNew);
	}
	XClassFactory::counter++;
}

XClassFactory::~XClassFactory()
{
	if(--XClassFactory::counter <= 0)
	{
		XClassInfo * tmp = XClassFactory::first_class;
		while(tmp)
		{
			XClassInfo * del = tmp;
			tmp = tmp->next;
			delete del;
		}
		XClassFactory::first_class = NULL;
	}
}

XObject * XClassFactory::Create(std::string name)
{
	XClassInfo * tmp = XClassFactory::first_class;
	while(tmp)
	{
		if (tmp->name == name) return tmp->pClassCreator();
		tmp = tmp->next;
	}
	assert(0);
	return NULL;
}

XObject * XClassFactory::CreateNew(std::string name)
{
	XClassInfo * tmp = XClassFactory::first_class;
	while(tmp)
	{
		if (tmp->name == name) return tmp->pClassNew();
		tmp = tmp->next;
	}
	return NULL;
}


void XObject::Store(XFile * f)
{
	if(bAlreadyStored)
		assert(!bAlreadyStored);
	bAlreadyStored = true;

	f->Write(&xguid, sizeof(XGUID));
	f->Write(&quantity, sizeof(int));
	f->Write(&im, sizeof(ITEM_MASK));
	f->Write(&ttm, sizeof(int));
	f->Write(&ttmb, sizeof(int));
}

void XObject::Restore(XFile * f)
{
	f->Read(&xguid, sizeof(XGUID)); 
	f->Read(&quantity, sizeof(int));
	f->Read(&im, sizeof(ITEM_MASK));
	f->Read(&ttm, sizeof(int));
	f->Read(&ttmb, sizeof(int));
}

void XObject::StoreAllObjects(XFile * f)
{
	assert(table == 0);
	long i;

	table = new XObject * [count];
	XObject * p = root;
	for(i = 0; i < count; i++) 
	{ 
		assert(p != 0);
		table[i] = p; p = p->next; 
	}
	std::sort(&table[0], &table[count]);

	f->Write(&count, sizeof(count));
	FILE * tmp = fopen("dmp.txt", "wt");
	for(i = 0; i < count; i++) 
	{
		unsigned char name_size = table[i]->GetClassName().size();
		f->Write(&name_size, sizeof(name_size));
		f->Write(table[i]->GetClassName().c_str(), sizeof(char), name_size);
		table[i]->bAlreadyStored = false;
		fprintf(tmp, "[%d] %s\n", i, table[i]->GetClassName());
	}
	fclose(tmp);
	for(i = 0; i < count; i++) 
		table[i]->Store(f);
}

void XObject::RestoreAllObjects(XFile * f)
{
	long i;
	assert(count == 0 && table == 0);
	long read_count = 0;
	f->Read(&read_count, sizeof(read_count));
	FILE * tmp = fopen("dmp2.txt", "wt");
	for(i = 0; i < read_count; i++) 
	{
		unsigned char name_size;
		f->Read(&name_size, sizeof(name_size));
		char * buf = new char[name_size + 1];
		f->Read(buf, sizeof(char), name_size);
		buf[name_size] = 0;
		DYNCREATE(buf);
		fprintf(tmp, "[%d] %s\n", i, buf);
		delete [] buf;
	}

	fclose(tmp);
	table = new XObject * [count];	
	XObject * p = root;
	for(i = 0; i < count; i++) 
	{ 
		if(p == 0)
			assert(p != 0);
		table[count - i - 1] = p; p = p->next; 
	}

	for(i = 0; i < count; i++) 
		table[i]->Restore(f);
}

void XObject::InvalidateAllObjects()
{
	while(root != NULL) root->Invalidate();
}

void XObject::StorePointer(XFile * f, XObject * p)
{
	if (p != NULL)
	{
	//  Binary search in a sorted array of pointers
		long min = 0, max = count - 1;
		while(min <= max)
		{
			long i = (min + max) / 2;
			if(table[i] < p)
			{
				min = i + 1;
			}
			else
			if(table[i] > p)
			{
				max = i - 1;
			}
			else
			{
				f->Write(&i, sizeof(i));
				return;
			}
		}
		assert(false);
	}
//  Unable to find a pointer to this object. Storing -1 to show
//  that we later need to restore this pointer value as NULL
	long i = -1;
	f->Write(&i, sizeof(i));
}

XObject * XObject::RestorePointer(XFile * f, void * owner)
{
	long i;
	f->Read(&i, sizeof(i));
	if(i == -1) return 0;
	assert(i >= 0 && i < count);
	XObject * p = table[i];
	return table[i];
}

void XObject::FreeTable()
{
	assert(table != 0);
	delete [] table;
	table = 0;
}

void XObject::DumpAll()
{
	XFile file;
	file.Open(vMakePath(HOME_DIR, "dump.txt"), "w");
	XObject * p = root;
	while (p)
	{
		p->Dump(&file);
		p = p->next;
	}
	file.Close();
}

void XObject::Dump(XFile * f)
{
	char buf[256];
	sprintf(buf, "%5d   %2d     %s", xguid, reference, GetClassName());
	f->Write(buf, strlen(buf));
	f->Write("\n", 1);
}

