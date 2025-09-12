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

class XFile
{
    public:
        XFile() : file(NULL) {}

        ~XFile()
        {
            if (file) {
                fclose(file);
            }
        }

        int Open(const char* name, const char* param)
        {
            file = fopen(name, param);

            if (file) {
                return 1;
            } else {
                return 0;
            }
        }

        void Close()
        {
            fclose(file);
            file = NULL;
        }

        int Write(const void* data, size_t block_size, size_t block_count = 1)
        {
            return fwrite(data, block_size, block_count, file);
        }

        int Write(const int* data)
        {
            return fwrite(data, sizeof(int), 1, file);
        }

        int Write(const unsigned int* data)
        {
            return fwrite(data, sizeof(unsigned int), 1, file);
        }

        int Read(void* data, size_t block_size, size_t block_count = 1)
        {
            unsigned int res = fread(data, block_size, block_count, file);
            assert(res == block_count);
            return res;
        }

        int Read(int* data)
        {
            return fread(data, sizeof(int), 1, file);
        }

        int Read(unsigned int* data)
        {
            return fread(data, sizeof(unsigned int), 1, file);
        }

    protected:
        FILE* file;
};

#endif
