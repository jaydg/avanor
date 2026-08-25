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

#ifndef CHAMBERS_BUILDER_H
#define CHAMBERS_BUILDER_H

#include "map/map.h"

class XLocation;

// Rounded chambers, grown one cell at a time on a grid coarser than the map
// and then scaled up onto it. No two chambers touch; each pair that could be
// joined is joined by a single doorway through the rock between them, and some
// chambers are then eroded away until what is left of them is a corridor.
// Lakes go in last, wherever they do not cut the level in two.
//
// Nothing in here knows it is underground. Hand it trees for the wall and
// grass for the floor and the same chambers come out as glades in a forest,
// joined by paths.
//
// The shape of the algorithm is Boris the Brave's description of the
// Cave & Glade generator at
// https://www.boristhebrave.com/2023/11/19/how-does-cave-glade-generator-work/
// On a character grid there are no hexagons to hide, so the doubly connected
// edge list is an array of chamber numbers and the outline refinement is one
// pass that rounds off corners.
class XChambersBuilder
{
    public:
        // How the chambers are grown and joined.
        struct Shape {
            // How many chambers to grow, and how many coarse cells each
            // of them is made of.
            int min_areas, max_areas;
            int min_size, max_size;

            // Map tiles to a coarse cell. A chamber of a dozen cells is
            // about four cells across, so this is what decides whether
            // that reads as a closet or as a hall.
            int scale;

            // How much a cell already surrounded by the chamber is
            // preferred over one hanging off its edge. Above zero the
            // chambers come out round; below it they send out tendrils.
            double gamma;

            // What share of the chambers is eroded down to a corridor,
            // and how much of the largest one survives that, per cent.
            int corridors;
            int corridor_left;

            // Per cent of the doorways beyond the ones needed to reach
            // every chamber. At zero the level is a tree and there is
            // exactly one way round it.
            int loop_odds;

            // How far the finished outline is allowed to wander off the
            // coarse grid it was rasterised from, per cent of the
            // boundary. Without this every wall is a run of whole cells
            // meeting at a right angle, which reads as masonry rather
            // than as rock. Set it to zero for quarried stone.
            int roughness;

            // Tiles across one bulge or bite of that wandering. Small
            // makes a ragged edge, large makes a lobed one.
            int roughness_grain;

            // Passes that round off the corners the coarse grid leaves.
            int smooth;
        };

        // The lakes. Leaving `shallow` unset makes a dry level.
        struct Water {
            XTileType::Id shallow;
            XTileType::Id deep;

            // What share of the floor goes under water, per cent, and
            // what share of that water is out of its depth. Shares
            // rather than thresholds: the noise piles up around its
            // middle, so a fixed water line floods all of a level or
            // none of it depending on where the lattice fell.
            int level;
            int deep_level;

            // Tiles across one lake-sized feature of the noise.
            int grain;
        };

        XChambersBuilder(XLocation* _location, int _w, int _h, XTileType::Id _wall, XTileType::Id _floor,
                         const Shape& _shape, const Water& _water)
            : location(_location), w(_w), h(_h), wall(_wall), floor(_floor),
              shape(_shape), water(_water)
        {
        }

        void Build();

    private:
        XLocation* location;
        int w, h;
        XTileType::Id wall;
        XTileType::Id floor;
        Shape shape;
        Water water;
};

#endif
