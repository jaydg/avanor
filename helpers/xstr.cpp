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

#include <stdio.h>
#include <string.h>

#include "helpers/xstr.h"

void XStr::Setup(const char* s, int _sz)
{
    sz = _sz;
    str = new char[sz + 1];
    memcpy(str, s, sz);
    *(str + sz) = 0;
}

XStr::XStr(const char* s)
{
    Setup(s, strlen(s));
}

XStr::XStr(const XStr & s)
{
    Setup(s.str, s.sz);
}


XStr::XStr(const char* s, int _sz)
{
    Setup(s, _sz);
}

XStr::XStr()
{
    Setup("", 0);
}

XStr::~XStr()
{
    delete[] str;
}

XStr XStr::operator +(const XStr& s)
{
    int new_sz = sz + s.sz;
    char* new_str = new char[new_sz + 1];
    memcpy(new_str, str, sz);
    memcpy(new_str + sz, s.str, s.sz + 1);
    return XStr(new_str, new_sz);
}

XStr XStr::operator +(const char * s)
{
    return operator+(XStr(s));
}

XStr &XStr::operator=(const XStr& s)
{
    delete[] str;
    sz = s.sz;
    str = new char[sz + 1];
    memcpy(str, s.str, sz + 1);
    return *this;
}

XStr &XStr::operator=(const char * s)
{
    return operator=(XStr(s));
}

XStr &XStr::operator+=(const char * s)
{
    int len = strlen(s);
    int new_sz = sz + len;
    char* new_str = new char[new_sz + 1];
    memcpy(new_str, str, sz);
    memcpy(new_str + sz, s, len + 1);
    sz = new_sz;
    delete[] str;
    str = new_str;
    return *this;
}

XStr &XStr::operator+=(const XStr& s)
{
    int new_sz = sz + s.sz;
    char* new_str = new char[new_sz + 1];
    memcpy(new_str, str, sz);
    memcpy(new_str + sz, s.str, s.sz + 1);
    sz = new_sz;
    delete[] str;
    str = new_str;
    return *this;
}

bool XStr::operator==(const char * s)
{
    return strcmp(str, s) == 0;
}

const char* XStr::c_str()
{
    return str;
}

size_t XStr::Len()
{
    return sz;
}

void XStr::Store(XFile * f)
{
    f->Write(&sz);
    f->Write(str, sz + 1);
}

void XStr::Restore(XFile * f)
{
    delete[] str;
    f->Read(&sz);
    str = new char[sz + 1];
    f->Read(str, sz + 1);
}

bool XStr::Empty()
{
    return sz == 0;
}


bool XStr::ReplaceFirst(const char* sub_string, const char* new_string)
{
    int len = sz + strlen(new_string) - strlen(sub_string);

    if (len < 0) {
        return false;
    }

    char* res = new char[len + 1];
    char* sub = strstr(str, sub_string);

    if (!sub) {
        return false;
    }

    *sub = 0;
    strcpy(res, str);
    strcat(res, new_string);
    strcat(res, str + strlen(sub_string));

    delete[] str;
    sz = len;
    str = res;
    return true;
}
