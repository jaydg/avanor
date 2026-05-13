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
#include <fstream>
#include <string>
#include <fmt/format.h>

#include "engine/global.h"
#include "engine/xobject.h"

long XObject::invalid_count = 0;
XObjectMap XObject::objects = XObjectMap();

XGUID guid = 1;

XClassInfo* XClassFactory::first_class = nullptr;
int XClassFactory::counter = 0;

XClassFactory::XClassFactory(const std::string& name, const CLASS_CREATOR pClassCreator, CLASS_CREATOR pClassNew)
{
    if (!first_class) {
        first_class = new XClassInfo(name, pClassCreator, pClassNew);
    } else {
        XClassInfo * tmp = first_class;

        while (tmp->next) {
            tmp = tmp->next;
        }

        tmp->next = new XClassInfo(name, pClassCreator, pClassNew);
    }

    counter++;
}

XClassFactory::~XClassFactory()
{
    if (--counter <= 0) {
        const XClassInfo* tmp = first_class;

        while (tmp) {
            const XClassInfo * del = tmp;
            tmp = tmp->next;
            delete del;
        }

        first_class = nullptr;
    }
}

XObject* XClassFactory::Create(const std::string& name)
{
    const XClassInfo * tmp = first_class;

    while (tmp) {
        if (tmp->name == name) {
            return tmp->pClassCreator();
        }

        tmp = tmp->next;
    }

    assert(0);
    return nullptr;
}

XObject* XClassFactory::CreateNew(const std::string& name)
{
    const XClassInfo* tmp = first_class;

    while (tmp) {
        if (tmp->name == name) {
            return tmp->pClassNew();
        }

        tmp = tmp->next;
    }

    return nullptr;
}

void XObject::Store(XFile * f)
{
    if (bAlreadyStored) {
        assert(!bAlreadyStored);
    }

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
    const size_t size = objects.size();
    f->Write(&size, sizeof(size_t), 1);

    std::ofstream tmp("dmp.txt");

    long i = 0;
    for (const auto& [key, obj] : objects) {
        const std::string& class_name = obj->GetClassName();
        auto name_size = static_cast<unsigned char>(class_name.size());
        f->Write(&name_size, sizeof(name_size));
        f->Write(class_name.c_str(), sizeof(char), name_size);
        obj->bAlreadyStored = false;
        tmp << fmt::format("[{:04}] {}\n", i++, class_name);
    }

    for (const auto& [key, obj] : objects) {
        obj->Store(f);
    }
}

void XObject::RestoreAllObjects(XFile * f)
{
    assert(objects.empty());

    long read_count = 0;
    f->Read(&read_count, sizeof(read_count));

    std::ofstream tmp("dmp2.txt");

    for (long i = 0; i < read_count; i++) {
        unsigned char name_size;
        f->Read(&name_size, sizeof(name_size));
        std::string class_name(name_size, '\0');
        f->Read(class_name.data(), sizeof(char), name_size);
        XClassFactory::Create(class_name);
        tmp << fmt::format("[{:04}] {}\n", i, class_name);
    }

    for (auto& [key, obj] : objects) {
        obj->Restore(f);
    }
}

void XObject::InvalidateAllObjects()
{
    for (auto& [key, obj] : objects)
    {
        obj->Invalidate();
    }
}

void XObject::StorePointer(XFile * f, XObject * p)
{
    // Default to storing -1 to show that we later
    // need to restore this pointer value as nullptr
    long guid = -1;

    if (p != nullptr) {
        guid = p->guid();
    }

    f->Write(&guid, sizeof(guid));
}

XObject* XObject::RestorePointer(XFile * f, void* owner)
{
    long guid;

    f->Read(&guid, sizeof(guid));

    if (guid == -1) {
        return nullptr;
    }
    else
    {
        return objects[guid];
    }
}

void XObject::DumpAll()
{
    XFile file;
    file.Open(vMakePath(HOME_DIR, "dump.txt"), "w");

    for (auto& [key, obj] : objects)
    {
        obj->Dump(&file);
    }

    file.Close();
}

void XObject::Dump(XFile * f)
{
    char buf[256];
    sprintf(buf, "%5lu   %2d     %s", xguid, reference, GetClassName().c_str());
    f->Write(buf, strlen(buf));
    f->Write("\n", 1);
}
