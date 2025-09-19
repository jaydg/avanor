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

#ifndef __XFILE_H
#define __XFILE_H

#include <cassert>
#include <cstdio>
#include <string>

class XFile
{
    public:
        XFile() : file(nullptr) {}

        ~XFile()
        {
            if (file) {
                fclose(file);
            }
        }

        bool Open(const char* name, const char* param)
        {
            file = fopen(name, param);

            if (file) {
                return true;
            } else {
                return false;
            }
        }

        void Close()
        {
            fclose(file);
            file = nullptr;
        }

        size_t Write(const void* data, const size_t block_size, const size_t block_count = 1) const
        {
            return fwrite(data, block_size, block_count, file);
        }

        template <typename T>
        size_t Write(const T* data) const
        {
            return fwrite(data, sizeof(T), 1, file);
        }

        size_t WriteStr(const std::string& str) const
        {
            const size_t sz = str.size();
            const size_t res = Write(&sz);
            return res + Write(str.c_str(), sz + 1);
        }

        size_t Read(void* data, const size_t block_size, const size_t block_count = 1) const
        {
            const unsigned int res = fread(data, block_size, block_count, file);
            assert(res == block_count);
            return res;
        }

        template <typename T>
        size_t Read(T* data) const
        {
            return fread(data, sizeof(T), 1, file);
        }

        size_t ReadStr(std::string& str) const
        {
            int sz;
            Read(&sz);
            const auto buf = new char[sz + 1];
            const size_t ret = Read(buf, sz + 1);

            str = buf;
            delete[] buf;

            return ret;
        }

    protected:
        FILE* file;
};

#endif
