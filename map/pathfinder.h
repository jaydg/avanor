/*
This file is part of "Avanor, the Land of Mystery" roguelike game

Copyright (C) 2000-2006 Vadim Gaidukevich
Copyright (C) 2025,2026 Joachim de Groot

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

#ifndef __PATHFINDER_H
#define __PATHFINDER_H

#include <functional>
#include <vector>

#include "helpers/point.h"

class XMap;

// What it costs somebody to step into a cell, and what it costs them to
// find out they cannot. A wall answers Blocked; open ground answers
// PathStep; anything in between answers more than a step, which is how a
// door somebody has to open, or a friend they have to squeeze past, ends
// up on a route only when the way round is worse.
constexpr int PathBlocked = -1;
constexpr int PathStep = 10;

// A diagonal step is longer than a straight one. Ten and fourteen rather
// than one and root two, so the whole search stays in integers.
constexpr int PathDiagonal = 14;

// Asked for each cell the search considers entering. It is asked about a
// great many cells, so it should be cheap and must not change the world.
using PathCost = std::function<int(int x, int y)>;

// The way from `from` to `to`, as the steps to take: the first entry is
// the cell to move into next and the last is `to` itself. Empty when
// there is no way - which is a real answer, not an error, and the caller
// has to tell it apart from having arrived.
//
// A* with an octile heuristic. The grid this walks is small (a level is
// at most a couple of hundred cells on a side) and the heuristic is exact
// on open ground, so on the common case - somebody crossing a room - it
// settles in a few dozen cells rather than flooding the level. `budget`
// caps how many cells it will settle before giving up, so a hopeless
// search costs a known amount rather than the whole map.
std::vector<XPoint> FindPath(const XMap& map, const XPoint& from, const XPoint& to,
                             const PathCost& cost, int budget = 4000);

#endif
