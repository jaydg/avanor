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

#ifndef __XSimpleVector_h__
#define __XSimpleVector_h__

//////////////////////////////////////////////////////////////////////////////
// This is a very simple implementation of vector container. Is has many    //
// limitations. This container can only contain simple data types (which    //
// have no constructors or destructors). Otherwise many bad things can      //
// happen - from memory leaks to data corruption.                           //
// But it is sometimes very useful to have self growing array of ints       //
// or any other values of simple data type                                  //
//////////////////////////////////////////////////////////////////////////////

#include <string.h>

template<class TYPE> class XSimpleVector
{
private:
   long   count;
   long   maxcount;
   TYPE * ptr;
public:
   XSimpleVector(long startmaxcount = 1)
   {
      count    = 0;
      maxcount = (startmaxcount > 0) ? startmaxcount : 1;

      ptr = (TYPE *) new char[maxcount * sizeof(TYPE)];
      memset(ptr, 0, maxcount * sizeof(TYPE));
   }
   ~XSimpleVector()
   {
      delete [] (char *)ptr;
   }

   TYPE & operator[](long index)
   {
      if(index < count) return ptr[index];

      while(index >= maxcount)
      {
         maxcount <<= 1;

         TYPE * tmp = (TYPE *) new char[maxcount * sizeof(TYPE)];
         memcpy(tmp, ptr, count * sizeof(TYPE));
         memset(&tmp[count], 0, (maxcount - count) * sizeof(TYPE));
         delete [] (char *)ptr;
         ptr = tmp;
      }
      
      count = index + 1; 
      return ptr[index];
   }

   long getcount() { return count; }
   long getmaxcount() { return maxcount; }

   void clean(long startmaxcount = 1)
   {
      delete [] (char *)ptr;

      count    = 0;
      maxcount = (startmaxcount > 0) ? startmaxcount : 1;

      ptr = (TYPE *) new char[maxcount * sizeof(TYPE)];
      memset(ptr, 0, maxcount * sizeof(TYPE));
   }
};

#endif
