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

#include <algorithm>
#include <cstdlib>
#include <limits>
#include <queue>
#include <vector>

#include "map/map.h"
#include "map/pathfinder.h"

namespace {

constexpr int Unvisited = std::numeric_limits<int>::max();

// What is left to walk, at best: the diagonal part of the gap plus
// whatever straight steps remain. Exact on open ground, never an
// overestimate anywhere else, which is what keeps the answer shortest.
int Octile(const int dx, const int dy)
{
    const int adx = std::abs(dx);
    const int ady = std::abs(dy);

    return PathStep * (adx + ady) + (PathDiagonal - 2 * PathStep) * std::min(adx, ady);
}

struct Node {
    int estimate;
    int index;

    // Ordered so the priority queue hands back the cheapest, not the
    // dearest - std::priority_queue is a max-heap.
    bool operator<(const Node& other) const
    {
        return estimate > other.estimate;
    }
};

} // namespace

std::vector<XPoint> FindPath(const XMap& map, const XPoint& from, const XPoint& to,
                             const PathCost& cost, const int budget)
{
    // The part of the map that exists. A level above another one holds
    // only its own window of the world, and nothing outside it is even a
    // cell to ask about.
    const int left = map.stored_x;
    const int top = map.stored_y;
    const int width = map.stored_len;
    const int height = map.stored_hgt;

    const auto inside = [&](const int x, const int y) {
        return x >= left && x < left + width && y >= top && y < top + height;
    };

    if (!inside(from.x, from.y) || !inside(to.x, to.y)) {
        return {};
    }

    const auto index_of = [&](const int x, const int y) {
        return (y - top) * width + (x - left);
    };

    const int start = index_of(from.x, from.y);
    const int goal = index_of(to.x, to.y);

    if (start == goal) {
        return {};
    }

    // Somewhere nobody can stand: a guard's post reckoned as the middle of
    // his beat and the middle is a pillar, an item lying in a wall, a
    // corpse under a closed portcullis. Walking up to it is what was meant,
    // so the search settles for standing next to it rather than answering
    // that the world is unreachable. The flood this replaces did the same
    // by accident - it started at the target and never asked whether the
    // target could be stood on - and every failed search in a run of the
    // whole world turned out to be this case, so it is the common one, not
    // a corner.
    const bool alongside = cost(to.x, to.y) == PathBlocked;

    const auto arrived = [&](const int x, const int y) {
        return alongside
            ? std::abs(x - to.x) <= 1 && std::abs(y - to.y) <= 1
            : x == to.x && y == to.y;
    };

    std::vector<int> spent(static_cast<size_t>(width) * height, Unvisited);
    std::vector<int> came_from(static_cast<size_t>(width) * height, -1);

    std::priority_queue<Node> open;

    spent[start] = 0;
    open.push({Octile(to.x - from.x, to.y - from.y), start});

    int settled = 0;
    int reached = -1;

    while (!open.empty() && settled < budget) {
        const Node here = open.top();
        open.pop();

        const int hx = left + here.index % width;
        const int hy = top + here.index / width;

        // The first one out of the queue that will do. Cheapest when the
        // goal itself is the destination; when standing alongside will do,
        // the cheapest of the ways to stand alongside give or take the
        // difference between a straight step and a diagonal one, which is
        // not worth searching on for.
        if (arrived(hx, hy)) {
            reached = here.index;
            break;
        }

        // Already settled by a cheaper route: the queue holds one entry
        // per improvement rather than being rearranged, so stale ones
        // surface and are dropped here.
        if (here.estimate - Octile(to.x - hx, to.y - hy) > spent[here.index]) {
            continue;
        }

        settled++;

        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) {
                    continue;
                }

                const int nx = hx + dx;
                const int ny = hy + dy;

                if (!inside(nx, ny)) {
                    continue;
                }

                const int step = cost(nx, ny);

                if (step == PathBlocked) {
                    continue;
                }

                // A diagonal costs its longer step, and the extra a cell
                // asks for is paid on top of it either way.
                const int move = (dx != 0 && dy != 0)
                    ? step + (PathDiagonal - PathStep)
                    : step;

                const int next = index_of(nx, ny);
                const int through = spent[here.index] + move;

                if (through < spent[next]) {
                    spent[next] = through;
                    came_from[next] = here.index;
                    open.push({through + Octile(to.x - nx, to.y - ny), next});
                }
            }
        }
    }

    if (reached == -1) {
        return {};
    }

    // Back along the trail, then turned round: the caller wants the next
    // step first.
    std::vector<XPoint> path;

    for (int at = reached; at != start && at != -1; at = came_from[at]) {
        path.emplace_back(left + at % width, top + at / width);
    }

    std::reverse(path.begin(), path.end());

    return path;
}
