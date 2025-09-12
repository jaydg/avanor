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

---------------------------------------------------------------------

The code in this sourse file implements line of sight algorithm
originally developed for Angband roguelike game (description of
this algorithm can be found in cave.c file in Angband sources):

A simple, fast, integer-based line-of-sight algorithm.  By Joseph Hall,
4116 Brewster Drive, Raleigh NC 27606.  Email to jnh@ecemwl.ncsu.edu.

The algorithm implementation included in this source code is written
from scratch by Serge Semashko to have a faster and better optimized
code (in Avanor LOS is used by EVERY monster, not only by the player,
so it is very important) and avoid problems with Angband license in
order to put this code under GPL license (so it is not a derived work
but independent algorithm implementation). The code which builds the
visibility table los_info[] is currently based on Angband sources, so
it is not included into Avanor distribution.

This implementations also has some nice features not present in the
original algorightm used for Angband:
1. It is possible to track whether the center of each cell is seen
2. Each grid is processed only once, it is very useful for implementing
	fireball spells using this code (the callback will be called only
	once for each grid, so none of them will be damaged twice)

*/

#include <cassert>
#include <cstring>

#include "creature/los.h"

struct grid_delta_type {
    signed char dy;
    signed char dx;
};

struct los_info_type {
    grid_delta_type grid_delta[8];
    unsigned long bits0;
    unsigned long bits0_c;
    int next0;
    int next1;
    int d;
    int flag;
};

static los_info_type los_info[] = {
    { { { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0} }, 0x00000000, 0x00000001, 2, 2, 0, 1 },
    { { { 0, 1}, { 1, 0}, { 1, 0}, { 0, -1}, { 0, -1}, {-1, 0}, {-1, 0}, { 0, 1} }, 0x7FFFFFFF, 0x00000001, 3, 4, 1, 1 },
    { { { 1, 1}, { 1, 1}, { 1, -1}, { 1, -1}, {-1, -1}, {-1, -1}, {-1, 1}, {-1, 1} }, 0xFFFF8000, 0x80000000, 5, 5, 1, 2 },
    { { { 0, 2}, { 2, 0}, { 2, 0}, { 0, -2}, { 0, -2}, {-2, 0}, {-2, 0}, { 0, 2} }, 0x00003FFF, 0x00000001, 6, 7, 2, 1 },
    { { { 1, 2}, { 2, 1}, { 2, -1}, { 1, -2}, {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2} }, 0x7FFFFC00, 0x00180000, 7, 8, 2, 4 },
    { { { 2, 2}, { 2, 2}, { 2, -2}, { 2, -2}, {-2, -2}, {-2, -2}, {-2, 2}, {-2, 2} }, 0xFF000000, 0x80000000, 9, 9, 3, 2 },
    { { { 0, 3}, { 3, 0}, { 3, 0}, { 0, -3}, { 0, -3}, {-3, 0}, {-3, 0}, { 0, 3} }, 0x000001FF, 0x00000001, 10, 11, 3, 1 },
    { { { 1, 3}, { 3, 1}, { 3, -1}, { 1, -3}, {-1, -3}, {-3, -1}, {-3, 1}, {-1, 3} }, 0x007FFF80, 0x00004000, 11, 12, 3, 4 },
    { { { 2, 3}, { 3, 2}, { 3, -2}, { 2, -3}, {-2, -3}, {-3, -2}, {-3, 2}, {-2, 3} }, 0x7FFC0000, 0x03000000, 12, 13, 4, 4 },
    { { { 3, 3}, { 3, 3}, { 3, -3}, { 3, -3}, {-3, -3}, {-3, -3}, {-3, 3}, {-3, 3} }, 0xF8000000, 0x80000000, 14, 14, 4, 2 },
    { { { 0, 4}, { 4, 0}, { 4, 0}, { 0, -4}, { 0, -4}, {-4, 0}, {-4, 0}, { 0, 4} }, 0x0000003F, 0x00000001, 15, 16, 4, 1 },
    { { { 1, 4}, { 4, 1}, { 4, -1}, { 1, -4}, {-1, -4}, {-4, -1}, {-4, 1}, {-1, 4} }, 0x0001FFC0, 0x00000C00, 16, 17, 4, 4 },
    { { { 2, 4}, { 4, 2}, { 4, -2}, { 2, -4}, {-2, -4}, {-4, -2}, {-4, 2}, {-2, 4} }, 0x03FF8000, 0x00180000, 17, 18, 4, 4 },
    { { { 3, 4}, { 4, 3}, { 4, -3}, { 3, -4}, {-3, -4}, {-4, -3}, {-4, 3}, {-3, 4} }, 0x7F800000, 0x18000000, 18, 19, 5, 4 },
    { { { 4, 4}, { 4, 4}, { 4, -4}, { 4, -4}, {-4, -4}, {-4, -4}, {-4, 4}, {-4, 4} }, 0xE0000000, 0x80000000, 20, 20, 6, 2 },
    { { { 0, 5}, { 5, 0}, { 5, 0}, { 0, -5}, { 0, -5}, {-5, 0}, {-5, 0}, { 0, 5} }, 0x0000001F, 0x00000001, 21, 22, 5, 1 },
    { { { 1, 5}, { 5, 1}, { 5, -1}, { 1, -5}, {-1, -5}, {-5, -1}, {-5, 1}, {-1, 5} }, 0x00003FE0, 0x00000200, 22, 23, 5, 4 },
    { { { 2, 5}, { 5, 2}, { 5, -2}, { 2, -5}, {-2, -5}, {-5, -2}, {-5, 2}, {-2, 5} }, 0x003FE000, 0x00018000, 23, 24, 5, 4 },
    { { { 3, 5}, { 5, 3}, { 5, -3}, { 3, -5}, {-3, -5}, {-5, -3}, {-5, 3}, {-3, 5} }, 0x0FF80000, 0x00800000, 24, 25, 6, 4 },
    { { { 4, 5}, { 5, 4}, { 5, -4}, { 4, -5}, {-4, -5}, {-5, -4}, {-5, 4}, {-4, 5} }, 0x7E000000, 0x30000000, 25, 26, 6, 4 },
    { { { 5, 5}, { 5, 5}, { 5, -5}, { 5, -5}, {-5, -5}, {-5, -5}, {-5, 5}, {-5, 5} }, 0xC0000000, 0x80000000, 27, 27, 7, 2 },
    { { { 0, 6}, { 6, 0}, { 6, 0}, { 0, -6}, { 0, -6}, {-6, 0}, {-6, 0}, { 0, 6} }, 0x0000000F, 0x00000001, 28, 29, 6, 1 },
    { { { 1, 6}, { 6, 1}, { 6, -1}, { 1, -6}, {-1, -6}, {-6, -1}, {-6, 1}, {-1, 6} }, 0x00000FF0, 0x00000180, 29, 30, 6, 4 },
    { { { 2, 6}, { 6, 2}, { 6, -2}, { 2, -6}, {-2, -6}, {-6, -2}, {-6, 2}, {-2, 6} }, 0x0003F800, 0x00004000, 30, 31, 6, 4 },
    { { { 3, 6}, { 6, 3}, { 6, -3}, { 3, -6}, {-3, -6}, {-6, -3}, {-6, 3}, {-3, 6} }, 0x00FF0000, 0x00180000, 31, 32, 7, 4 },
    { { { 4, 6}, { 6, 4}, { 6, -4}, { 4, -6}, {-4, -6}, {-6, -4}, {-6, 4}, {-4, 6} }, 0x1FC00000, 0x03000000, 32, 33, 7, 4 },
    { { { 5, 6}, { 6, 5}, { 6, -5}, { 5, -6}, {-5, -6}, {-6, -5}, {-6, 5}, {-5, 6} }, 0x7C000000, 0x60000000, 33, 34, 8, 4 },
    { { { 6, 6}, { 6, 6}, { 6, -6}, { 6, -6}, {-6, -6}, {-6, -6}, {-6, 6}, {-6, 6} }, 0x80000000, 0x80000000, 0, 0, 8, 2 },
    { { { 0, 7}, { 7, 0}, { 7, 0}, { 0, -7}, { 0, -7}, {-7, 0}, {-7, 0}, { 0, 7} }, 0x00000007, 0x00000001, 35, 36, 7, 1 },
    { { { 1, 7}, { 7, 1}, { 7, -1}, { 1, -7}, {-1, -7}, {-7, -1}, {-7, 1}, {-1, 7} }, 0x000003F8, 0x00000040, 36, 37, 7, 4 },
    { { { 2, 7}, { 7, 2}, { 7, -2}, { 2, -7}, {-2, -7}, {-7, -2}, {-7, 2}, {-2, 7} }, 0x00007C00, 0x00003000, 37, 38, 7, 4 },
    { { { 3, 7}, { 7, 3}, { 7, -3}, { 3, -7}, {-3, -7}, {-7, -3}, {-7, 3}, {-3, 7} }, 0x001F8000, 0x00020000, 38, 39, 8, 4 },
    { { { 4, 7}, { 7, 4}, { 7, -4}, { 4, -7}, {-4, -7}, {-7, -4}, {-7, 4}, {-4, 7} }, 0x01F00000, 0x00C00000, 39, 40, 8, 4 },
    { { { 5, 7}, { 7, 5}, { 7, -5}, { 5, -7}, {-5, -7}, {-7, -5}, {-7, 5}, {-5, 7} }, 0x3F000000, 0x04000000, 40, 0, 9, 4 },
    { { { 6, 7}, { 7, 6}, { 7, -6}, { 6, -7}, {-6, -7}, {-7, -6}, {-7, 6}, {-6, 7} }, 0x70000000, 0xC0000000, 0, 0, 9, 4 },
    { { { 0, 8}, { 8, 0}, { 8, 0}, { 0, -8}, { 0, -8}, {-8, 0}, {-8, 0}, { 0, 8} }, 0x00000003, 0x00000001, 41, 42, 8, 1 },
    { { { 1, 8}, { 8, 1}, { 8, -1}, { 1, -8}, {-1, -8}, {-8, -1}, {-8, 1}, {-1, 8} }, 0x000001FC, 0x00000060, 42, 43, 8, 4 },
    { { { 2, 8}, { 8, 2}, { 8, -2}, { 2, -8}, {-2, -8}, {-8, -2}, {-8, 2}, {-2, 8} }, 0x00003E00, 0x00000C00, 43, 44, 8, 4 },
    { { { 3, 8}, { 8, 3}, { 8, -3}, { 3, -8}, {-3, -8}, {-8, -3}, {-8, 3}, {-3, 8} }, 0x0007C000, 0x0000C000, 44, 0, 9, 4 },
    { { { 4, 8}, { 8, 4}, { 8, -4}, { 4, -8}, {-4, -8}, {-8, -4}, {-8, 4}, {-4, 8} }, 0x007E0000, 0x00180000, 0, 0, 9, 4 },
    { { { 5, 8}, { 8, 5}, { 8, -5}, { 5, -8}, {-5, -8}, {-8, -5}, {-8, 5}, {-5, 8} }, 0x07E00000, 0x01800000, 0, 0, 9, 4 },
    { { { 0, 9}, { 9, 0}, { 9, 0}, { 0, -9}, { 0, -9}, {-9, 0}, {-9, 0}, { 0, 9} }, 0x00000001, 0x00000001, 0, 0, 9, 1 },
    { { { 1, 9}, { 9, 1}, { 9, -1}, { 1, -9}, {-1, -9}, {-9, -1}, {-9, 1}, {-1, 9} }, 0x000000FE, 0x00000020, 0, 0, 9, 4 },
    { { { 2, 9}, { 9, 2}, { 9, -2}, { 2, -9}, {-2, -9}, {-9, -2}, {-9, 2}, {-2, 9} }, 0x00001F00, 0x00000600, 0, 0, 9, 4 },
    { { { 3, 9}, { 9, 3}, { 9, -3}, { 3, -9}, {-3, -9}, {-9, -3}, {-9, 3}, {-3, 9} }, 0x0000F000, 0x00004000, 0, 0, 9, 4 }
};

// We prepared the grid data so that we need only 32 bits for slopes mask, it fits just
// one variable
#define BIT_MASK0 0xFFFFFFFF

// The number of grids in each octant
#define MAX_GRIDS (sizeof(los_info) / sizeof(los_info[0]))

static int GetMaxSight()
{
    int d = 0;

    for (unsigned int i = 0; i < MAX_GRIDS; i++) {
        if (los_info[i].d > d) {
            d = los_info[i].d;
        }

        if (los_info[i].next0 == los_info[i].next1) {
            los_info[i].next1 = 0;
        }
    }

    return d;
}

const int MAX_SIGHT = GetMaxSight();

// Dimentions of the cache array, it must be at least (MAX_SIGHT + 1) * 2
// in orger not to have buffer overflows. It is also strongly recommented
// that it is a power of 2, so index in this array can be found using
// just bit shifts and no multiply operations
#define CACHE_SIZE 32

void LineOfSight(
    int px,
    int py,
    int radius,
    void* opaque,
    int (*grid_callback)(void* opaque, int x, int y, int radius, int see_center))
{
    if (radius > MAX_SIGHT) {
        radius = MAX_SIGHT;
    }

    grid_callback(opaque, px, py, 0, 1);

    char cache[CACHE_SIZE * CACHE_SIZE];
#ifdef DEBUG_LOS // initialize cache with "bad" values (needed for stability test only)
    memset(cache, -1, sizeof(cache));
#endif

    // Scan each octant
    for (int o = 0; o < 8; o++) {
#ifdef DEBUG_LOS // array of flags that are used to ensure that we never enqueue the same grid twice
        char enqued_grids[MAX_GRIDS];
        memset(enqued_grids, 0, sizeof(enqued_grids));
#endif
        // Initialize horizontal/vertical and diagonal usage flagmask
        // (used in order not to call grid_callback function twice)
        int flagmask = (2L >> (o % 2)) | 4;

        if (o == 0) {
            flagmask |= 3;
        }

        if (o == 7) {
            flagmask &= ~3;
        }

        // Angbandish trick, very useful for avoiding coming to the same grid twice
        int last = 0;

        // Visibility bit vector, initially all the directions are visible
        unsigned long bits0 = BIT_MASK0;

        // Grid queue
        los_info_type *queue[MAX_GRIDS * 4];
        los_info_type** queue_head = queue;
        los_info_type** queue_tail = queue;

        // Initial grids
        *queue_tail++ = &los_info[1];
        *queue_tail++ = &los_info[2];

        // Process queue
        while (queue_head < queue_tail) {
            // Extract next grid los info
            los_info_type *p = *queue_head++;

            // Check radius and bits
            if ((bits0 & (p->bits0)) && (p->d <= radius)) {
                signed char dx = p->grid_delta[o].dx, dy = p->grid_delta[o].dy;
                signed char grid_transparant;

                char* pcache = cache + dx * CACHE_SIZE + dy + sizeof(cache) / 2;
#ifdef DEBUG_LOS // ensure that the cache size is large enough and there are no buffer overruns
                assert(pcache >= cache && pcache < cache + sizeof(cache));
#endif

                // For grids, disabled in flagmask, take their visibility value from cache
                // (the flags are chosen so that this value should already be there)
                if (p->flag & flagmask) {
                    grid_transparant = (char)grid_callback(
                        opaque, px + dx, py + dy, p->d, ((bits0 & p->bits0_c) == p->bits0_c));
                    *pcache = grid_transparant;
                } else {
                    grid_transparant = *pcache;
#ifdef DEBUG_LOS // ensure that we never read uninitialized value
                    assert(grid_transparant != -1);
#endif
                }

                if (grid_transparant) {
                    // For each visible grid, put into a queue the next grids which
                    // probably can be seen behind this one, there are up to two such grids
                    if (last != p->next0 && p->next0 != 0) {
                        *queue_tail++ = &los_info[p->next0];
#ifdef DEBUG_LOS // ensure that we never enqueue the same grid twice
                        assert(enqued_grids[p->next0] == 0);
                        enqued_grids[p->next0] = 1;
#endif
                    }

                    if (p->next1 != 0) {
                        *queue_tail++ = &los_info[last = p->next1];
#ifdef DEBUG_LOS // ensure that we never enqueue the same grid twice
                        assert(enqued_grids[p->next1] == 0);
                        enqued_grids[p->next1] = 1;
#endif
                    }
                } else {
                    // Remove visibility bit flags from for the directions this solid grid hides
                    bits0 &= ~(p->bits0);
                }
            }
        }
    }
}
