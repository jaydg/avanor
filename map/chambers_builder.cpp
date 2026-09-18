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
#include <cmath>
#include <iostream>
#include <map>
#include <utility>
#include <vector>

#include "engine/global.h"
#include "game/location.h"
#include "map/chambers_builder.h"
#include "map/map.h"

namespace {

// A coarse cell belongs to one chamber, to none, or is the one cell of
// rock two chambers are joined through.
constexpr int NOBODY = -1;
constexpr int DOORWAY = -2;

const int dx4[] = {1, -1, 0, 0};
const int dy4[] = {0, 0, 1, -1};
const int dx8[] = {1, 1, 1, 0, 0, -1, -1, -1};
const int dy8[] = {-1, 0, 1, -1, 1, -1, 0, 1};

// The coarse grid the chambers are grown on, one step above the map.
struct Grid {
    int w, h;
    std::vector<int> owner;

    Grid(const int _w, const int _h) : w(_w), h(_h), owner(_w * _h, NOBODY) {}

    [[nodiscard]] int At(const int x, const int y) const { return x + y * w; }
    [[nodiscard]] int X(const int cell) const { return cell % w; }
    [[nodiscard]] int Y(const int cell) const { return cell / w; }

    // The outermost ring is never grown into, so that whatever is
    // rasterised keeps clear of the map's own edge.
    [[nodiscard]] bool Inside(const int x, const int y) const
    {
        return x > 0 && y > 0 && x < w - 1 && y < h - 1;
    }

    // Chambers may not touch, not even at a corner: a chamber that
    // touched another would join it without a doorway, and on a square
    // grid a diagonal join is a join whose passability depends on the
    // movement rules rather than on the door graph.
    [[nodiscard]] bool ClearOfOthers(const int x, const int y, const int self) const
    {
        for (int i = 0; i < 8; i++) {
            const int nx = x + dx8[i];
            const int ny = y + dy8[i];

            if (nx < 0 || ny < 0 || nx >= w || ny >= h) {
                continue;
            }

            if (const int o = owner[At(nx, ny)]; o >= 0 && o != self) {
                return false;
            }
        }

        return true;
    }

    // How much of this cell's surroundings the chamber already holds.
    // This is what the growth weighting is a function of, so it counts
    // the corners too - they are what tells a bulge from a notch.
    [[nodiscard]] int Neighbours(const int x, const int y, const int self) const
    {
        int count = 0;

        for (int i = 0; i < 8; i++) {
            const int nx = x + dx8[i];
            const int ny = y + dy8[i];

            if (nx >= 0 && ny >= 0 && nx < w && ny < h && owner[At(nx, ny)] == self) {
                count++;
            }
        }

        return count;
    }
};

int Between(const int low, const int high)
{
    return high > low ? low + static_cast<int>(vRand(high - low + 1)) : low;
}

// Grows one chamber outward from a random free cell until it holds
// `target` cells or has nowhere left to go. Returns what it grew, which
// is empty when there was no room to start at all.
std::vector<int> GrowChamber(Grid& grid, const int self, const int target, const double gamma)
{
    // Every cell a chamber could start at, so that one is found whenever
    // one exists. Blind tries give up while there is still room, and
    // that shows as a level with fewer chambers than it asked for.
    std::vector<int> seeds;

    for (int y = 1; y < grid.h - 1; y++) {
        for (int x = 1; x < grid.w - 1; x++) {
            if (grid.owner[grid.At(x, y)] == NOBODY && grid.ClearOfOthers(x, y, self)) {
                seeds.push_back(grid.At(x, y));
            }
        }
    }

    std::vector<int> cells;

    if (seeds.empty()) {
        return cells;
    }

    cells.push_back(seeds[vRand(seeds.size())]);
    grid.owner[cells.front()] = self;

    std::vector<int> candidates;
    std::vector<double> weights;

    while (static_cast<int>(cells.size()) < target) {
        candidates.clear();
        weights.clear();

        for (const int cell : cells) {
            for (int i = 0; i < 4; i++) {
                const int nx = grid.X(cell) + dx4[i];
                const int ny = grid.Y(cell) + dy4[i];

                if (!grid.Inside(nx, ny)) {
                    continue;
                }

                const int n = grid.At(nx, ny);

                if (grid.owner[n] != NOBODY || !grid.ClearOfOthers(nx, ny, self)
                    || std::find(candidates.begin(), candidates.end(), n) != candidates.end()) {
                    continue;
                }

                candidates.push_back(n);
                weights.push_back(std::pow(static_cast<double>(grid.Neighbours(nx, ny, self)), gamma));
            }
        }

        if (candidates.empty()) {
            break;
        }

        double total = 0.0;

        for (const double weight : weights) {
            total += weight;
        }

        // Every weight is a power of at least one, so the total is only
        // ever zero if there is nothing to choose from at all.
        double roll = total * (static_cast<double>(vRand(10000)) / 10000.0);
        size_t chosen = weights.size() - 1;

        for (size_t i = 0; i < weights.size(); i++) {
            roll -= weights[i];

            if (roll < 0.0) {
                chosen = i;
                break;
            }
        }

        grid.owner[candidates[chosen]] = self;
        cells.push_back(candidates[chosen]);
    }

    return cells;
}

// Which chambers a cell of rock has as orthogonal neighbours. Two
// chambers can only be joined where the rock between them is one cell
// thin, which is exactly where such a cell exists.
std::vector<int> Borders(const Grid& grid, const int cell)
{
    std::vector<int> found;

    for (int i = 0; i < 4; i++) {
        const int nx = grid.X(cell) + dx4[i];
        const int ny = grid.Y(cell) + dy4[i];

        if (nx < 0 || ny < 0 || nx >= grid.w || ny >= grid.h) {
            continue;
        }

        if (const int o = grid.owner[grid.At(nx, ny)];
            o >= 0 && std::find(found.begin(), found.end(), o) == found.end()) {
            found.push_back(o);
        }
    }

    return found;
}

// Disjoint sets over the chambers, so that the doorways can be culled to
// the ones that actually reach somewhere new.
int Root(std::vector<int>& parent, int a)
{
    while (parent[a] != a) {
        parent[a] = parent[parent[a]];
        a = parent[a];
    }

    return a;
}

// True while the cells still form one region once `without` is taken out
// of them - the test that keeps eroding a chamber from breaking it in
// half.
bool StillWhole(const Grid& grid, const std::vector<int>& cells, const int without)
{
    std::vector<int> stack;
    std::vector<int> seen;

    for (const int cell : cells) {
        if (cell != without) {
            stack.push_back(cell);
            break;
        }
    }

    if (stack.empty()) {
        return false;
    }

    seen.push_back(stack.front());

    while (!stack.empty()) {
        const int cell = stack.back();
        stack.pop_back();

        for (int i = 0; i < 4; i++) {
            const int nx = grid.X(cell) + dx4[i];
            const int ny = grid.Y(cell) + dy4[i];

            if (nx < 0 || ny < 0 || nx >= grid.w || ny >= grid.h) {
                continue;
            }

            const int n = grid.At(nx, ny);

            if (n == without || std::find(cells.begin(), cells.end(), n) == cells.end()
                || std::find(seen.begin(), seen.end(), n) != seen.end()) {
                continue;
            }

            seen.push_back(n);
            stack.push_back(n);
        }
    }

    return seen.size() + 1 == cells.size();
}

// True while every doorway of this chamber still has a cell of it to
// open onto.
bool DoorsStillServed(const Grid& grid, const std::vector<int>& cells, const std::vector<int>& doors,
                      const int without)
{
    for (const int door : doors) {
        bool served = false;

        for (int i = 0; i < 4 && !served; i++) {
            const int nx = grid.X(door) + dx4[i];
            const int ny = grid.Y(door) + dy4[i];

            if (nx < 0 || ny < 0 || nx >= grid.w || ny >= grid.h) {
                continue;
            }

            const int n = grid.At(nx, ny);
            served = n != without && std::find(cells.begin(), cells.end(), n) != cells.end();
        }

        if (!served) {
            return false;
        }
    }

    return true;
}

// True when turning (x, y) back into rock cannot pinch the floor apart
// there. Everything still walkable next to it has to remain reachable
// without passing through it, judged over the five-by-five it sits in.
// A neighbour that could only be reached the long way round outside that
// window is counted as unreachable, so the test refuses more often than
// it strictly must - which is the right way for it to be wrong.
bool Fillable(const XMap* map, const int x, const int y, const XTileType::Id floor)
{
    std::vector<int> ends;

    for (int i = 0; i < 4; i++) {
        if (map->GetXY(x + dx4[i], y + dy4[i]) == floor) {
            ends.push_back((x + dx4[i]) + (y + dy4[i]) * map->len);
        }
    }

    // A tip of floor with one way out, or none, has nothing to pinch.
    if (ends.size() <= 1) {
        return true;
    }

    std::vector<int> seen{ends.front()};
    std::vector<int> stack{ends.front()};

    while (!stack.empty()) {
        const int cell = stack.back();
        stack.pop_back();

        for (int i = 0; i < 4; i++) {
            const int nx = cell % map->len + dx4[i];
            const int ny = cell / map->len + dy4[i];

            if (nx < x - 2 || nx > x + 2 || ny < y - 2 || ny > y + 2
                || nx < 0 || ny < 0 || nx >= map->len || ny >= map->hgt
                || (nx == x && ny == y) || map->GetXY(nx, ny) != floor) {
                continue;
            }

            const int n = nx + ny * map->len;

            if (std::find(seen.begin(), seen.end(), n) == seen.end()) {
                seen.push_back(n);
                stack.push_back(n);
            }
        }
    }

    for (const int end : ends) {
        if (std::find(seen.begin(), seen.end(), end) == seen.end()) {
            return false;
        }
    }

    return true;
}

// Value noise: random values on a lattice `grain` tiles apart, smoothed
// between, three octaves deep. Enough to give lakes an outline that
// looks like water rather than like a circle, without a Perlin
// implementation to maintain.
double Lattice(const int x, const int y, const unsigned int seed)
{
    unsigned int hash = seed + static_cast<unsigned int>(x) * 374761393u
                        + static_cast<unsigned int>(y) * 668265263u;
    hash = (hash ^ (hash >> 13)) * 1274126177u;

    return static_cast<double>((hash ^ (hash >> 16)) & 0xffffffu) / static_cast<double>(0xffffffu);
}

double Octave(const double x, const double y, const unsigned int seed)
{
    const int ix = static_cast<int>(std::floor(x));
    const int iy = static_cast<int>(std::floor(y));
    const double fx = x - ix;
    const double fy = y - iy;

    // Smoothstep, so that the lattice does not show as a diamond grid.
    const double sx = fx * fx * (3.0 - 2.0 * fx);
    const double sy = fy * fy * (3.0 - 2.0 * fy);

    const double top = Lattice(ix, iy, seed) * (1.0 - sx) + Lattice(ix + 1, iy, seed) * sx;
    const double bottom = Lattice(ix, iy + 1, seed) * (1.0 - sx) + Lattice(ix + 1, iy + 1, seed) * sx;

    return top * (1.0 - sy) + bottom * sy;
}

double Noise(const int x, const int y, const int grain, const unsigned int seed)
{
    const double step = grain > 1 ? static_cast<double>(grain) : 2.0;

    return (Octave(x / step, y / step, seed) * 4.0
            + Octave(x / (step / 2.0), y / (step / 2.0), seed + 1u) * 2.0
            + Octave(x / (step / 4.0), y / (step / 4.0), seed + 2u)) / 7.0;
}

} // namespace

void XChambersBuilder::Build()
{
    XMap*& map = location->map;

    if (!map) {
        map = new XMap(w, h);
    }

    const int len = map->len;
    const int hgt = map->hgt;

    for (int y = 0; y < hgt; y++) {
        for (int x = 0; x < len; x++) {
            map->SetXY(x, y, wall);
        }
    }

    const int scale = std::max(shape.scale, 1);

    // The coarse grid keeps a ring of cells clear of the map's edge, and
    // whatever tiles do not divide evenly into it are shared out as
    // margin on either side.
    const int cw = (len - 2) / scale;
    const int ch = (hgt - 2) / scale;

    if (cw < 5 || ch < 5) {
        std::cerr << "world: " << location->id << " is " << len << "x" << hgt
                  << ", too small to grow chambers " << scale << " tiles to the cell - "
                  << "it wants at least " << (5 * scale + 2) << "x" << (5 * scale + 2)
                  << ", or a smaller 'scale'" << std::endl;

        // Something walkable, so that the stairways placed afterwards
        // have somewhere to go and the level is at least playable.
        for (int y = 1; y < hgt - 1; y++) {
            for (int x = 1; x < len - 1; x++) {
                map->SetXY(x, y, floor);
            }
        }

        return;
    }

    Grid grid(cw, ch);

    const int ox = (len - cw * scale) / 2;
    const int oy = (hgt - ch * scale) / 2;

    // 1. Grow the chambers.
    std::vector<std::vector<int>> chambers;
    const int wanted = Between(shape.min_areas, shape.max_areas);

    for (int a = 0; a < wanted; a++) {
        std::vector<int> cells = GrowChamber(grid, static_cast<int>(chambers.size()),
                                             Between(shape.min_size, shape.max_size), shape.gamma);

        if (cells.empty()) {
            // The grid is full: whatever fits has been grown.
            break;
        }

        chambers.push_back(std::move(cells));
    }

    // 2. Find every place two chambers could be joined, and cull those
    // down to a set that reaches all of them.
    std::map<std::pair<int, int>, std::vector<int>> joins;

    for (int cell = 0; cell < cw * ch; cell++) {
        if (grid.owner[cell] != NOBODY) {
            continue;
        }

        const std::vector<int> sides = Borders(grid, cell);

        for (size_t i = 0; i < sides.size(); i++) {
            for (size_t j = i + 1; j < sides.size(); j++) {
                joins[{std::min(sides[i], sides[j]), std::max(sides[i], sides[j])}].push_back(cell);
            }
        }
    }

    std::vector<std::pair<int, int>> pairs;
    pairs.reserve(joins.size());

    for (const auto& [pair, cells] : joins) {
        pairs.push_back(pair);
    }

    // Shuffled, so that the tree that comes out of the union-find below
    // is a different tree each time rather than the lowest-numbered one.
    for (size_t i = pairs.size(); i > 1; i--) {
        std::swap(pairs[i - 1], pairs[vRand(i)]);
    }

    std::vector<int> parent(chambers.size());

    for (size_t i = 0; i < parent.size(); i++) {
        parent[i] = static_cast<int>(i);
    }

    // Doorways per chamber, which is both what the erosion below must
    // preserve and what makes a chamber worth eroding.
    std::vector<std::vector<int>> doors(chambers.size());

    for (const auto& [a, b] : pairs) {
        const int ra = Root(parent, a);
        const int rb = Root(parent, b);

        // Every pair that reaches a chamber not reached yet is kept, and
        // that alone makes the level connected. The rest are the loops.
        if (ra == rb && static_cast<int>(vRand(100)) >= shape.loop_odds) {
            continue;
        }

        parent[ra] = rb;

        const std::vector<int>& where = joins[{a, b}];
        const int door = where[vRand(where.size())];

        grid.owner[door] = DOORWAY;
        doors[a].push_back(door);
        doors[b].push_back(door);
    }

    // 3. Erode some chambers down to corridors, preferring the ones with
    // the most doorways - a chamber four ways lead through was never
    // really a room.
    std::vector<int> order(chambers.size());

    for (size_t i = 0; i < order.size(); i++) {
        order[i] = static_cast<int>(i);
    }

    std::stable_sort(order.begin(), order.end(), [&](const int a, const int b) {
        return doors[a].size() > doors[b].size();
    });

    const int erode = static_cast<int>(chambers.size()) * shape.corridors / 100;

    for (int i = 0; i < erode && i < static_cast<int>(order.size()); i++) {
        std::vector<int>& cells = chambers[order[i]];
        const int floor_left = std::max(static_cast<int>(cells.size()) * shape.corridor_left / 100, 1);

        while (static_cast<int>(cells.size()) > floor_left) {
            // Somewhere different each time, so that what is left is a
            // winding corridor rather than a chamber eaten from one end.
            const size_t start = vRand(cells.size());
            bool removed = false;

            for (size_t n = 0; n < cells.size() && !removed; n++) {
                const size_t at = (start + n) % cells.size();
                const int cell = cells[at];

                if (!StillWhole(grid, cells, cell) || !DoorsStillServed(grid, cells, doors[order[i]], cell)) {
                    continue;
                }

                grid.owner[cell] = NOBODY;
                cells.erase(cells.begin() + static_cast<long>(at));
                removed = true;
            }

            if (!removed) {
                // Every cell left is holding something up.
                break;
            }
        }
    }

    // Any chamber no doorway could reach is tunnelled to now, after the
    // erosion rather than before it: a tunnel dug earlier can be routed
    // through a chamber that is then eroded out from under it.
    // Two chambers can only take a doorway where the rock between them
    // is one cell thin, and a chamber that grew on its own in a corner
    // has no such neighbour at all. Rather than leave it stranded for
    // ConnectAllRegions to reach with a one-tile straight tunnel, drive
    // a corridor of the same width as everything else to the nearest
    // chamber already reached.
    for (size_t a = 1; a < chambers.size(); a++) {
        if (Root(parent, static_cast<int>(a)) == Root(parent, 0)) {
            continue;
        }

        int from = -1;
        int to = -1;
        int nearest = -1;

        for (const int here : chambers[a]) {
            for (size_t b = 0; b < chambers.size(); b++) {
                if (Root(parent, static_cast<int>(b)) != Root(parent, 0)) {
                    continue;
                }

                for (const int there : chambers[b]) {
                    const int span = std::abs(grid.X(here) - grid.X(there))
                                     + std::abs(grid.Y(here) - grid.Y(there));

                    if (nearest < 0 || span < nearest) {
                        nearest = span;
                        from = here;
                        to = there;
                    }
                }
            }
        }

        if (from < 0) {
            continue;
        }

        // An L, turning once. Anything it runs through that belongs to a
        // chamber is left alone - it is already floor, and taking it
        // would put a hole in that chamber's records.
        int x = grid.X(from);
        int y = grid.Y(from);
        std::vector<int> dug;

        while (x != grid.X(to) || y != grid.Y(to)) {
            if (x != grid.X(to)) {
                x += x < grid.X(to) ? 1 : -1;
            } else {
                y += y < grid.Y(to) ? 1 : -1;
            }

            if (grid.owner[grid.At(x, y)] == NOBODY) {
                grid.owner[grid.At(x, y)] = DOORWAY;
                dug.push_back(grid.At(x, y));
            }
        }

        // Both ends count as doorways of the chambers they open onto, so
        // that the erosion below cannot eat the cell this corridor
        // arrives at and strand it after all.
        if (!dug.empty()) {
            doors[a].push_back(dug.front());

            for (size_t b = 0; b < chambers.size(); b++) {
                if (std::find(chambers[b].begin(), chambers[b].end(), to) != chambers[b].end()) {
                    doors[b].push_back(dug.back());
                    break;
                }
            }
        }

        parent[Root(parent, static_cast<int>(a))] = Root(parent, 0);
    }

    // 4. Scale the coarse grid up onto the map.
    for (int cell = 0; cell < cw * ch; cell++) {
        if (grid.owner[cell] == NOBODY) {
            continue;
        }

        const int x0 = ox + grid.X(cell) * scale;
        const int y0 = oy + grid.Y(cell) * scale;

        for (int y = y0; y < y0 + scale; y++) {
            for (int x = x0; x < x0 + scale; x++) {
                map->SetXY(x, y, floor);
            }
        }
    }

    // 5. Take the outline off the coarse grid it was rasterised from.
    // Everything above this works a whole cell at a time, so every wall
    // runs in multiples of `scale` and every corner is a right angle -
    // which reads as masonry, not as rock. This is what the article does
    // by subdividing its polygons and smoothing them, in the only terms
    // a character grid has: one field of noise decides where the
    // boundary bulges out and where it takes a bite in, so that a whole
    // stretch of wall moves together rather than each tile going its own
    // way. Opening rock can never cut anything off; closing floor can,
    // so each of those is asked first.
    if (shape.roughness > 0) {
        const auto seed = static_cast<unsigned int>(vRand(0x7fffffff));
        std::vector<XPoint> opening;
        std::vector<XPoint> closing;

        for (int y = 1; y < hgt - 1; y++) {
            for (int x = 1; x < len - 1; x++) {
                int around = 0;

                for (int i = 0; i < 8; i++) {
                    if (map->GetXY(x + dx8[i], y + dy8[i]) == floor) {
                        around++;
                    }
                }

                // Only the edge moves; open rock and open floor stay.
                if (around == 0 || around == 8) {
                    continue;
                }

                const double value = Noise(x, y, shape.roughness_grain, seed) * 100.0;

                if (map->GetXY(x, y) == floor) {
                    if (value < shape.roughness) {
                        closing.emplace_back(x, y);
                    }
                } else if (value > 100 - shape.roughness) {
                    // Only where it joins what is already there. Rock
                    // that touches floor at a corner and nowhere else
                    // would open into an island: creatures walk in four
                    // directions, so a diagonal is not a way through.
                    bool orthogonal = false;

                    for (int i = 0; i < 4 && !orthogonal; i++) {
                        orthogonal = map->GetXY(x + dx4[i], y + dy4[i]) == floor;
                    }

                    if (orthogonal) {
                        opening.emplace_back(x, y);
                    }
                }
            }
        }

        for (const XPoint& point : opening) {
            map->SetXY(point.x, point.y, floor);
        }

        for (const XPoint& point : closing) {
            if (map->GetXY(point.x, point.y) == floor && Fillable(map, point.x, point.y, floor)) {
                map->SetXY(point.x, point.y, wall);
            }
        }
    }

    // 6. Round off the corners the coarse grid left behind. This only
    // ever opens rock, never fills floor, so it cannot cut anything off
    // from anything else. A threshold of five reaches into the notch
    // where two blocks meet at a diagonal and leaves the straight walls
    // between two chambers - three tiles of rock at the narrowest -
    // alone.
    for (int pass = 0; pass < shape.smooth; pass++) {
        std::vector<XPoint> opened;

        for (int y = 1; y < hgt - 1; y++) {
            for (int x = 1; x < len - 1; x++) {
                if (map->GetXY(x, y) != wall) {
                    continue;
                }

                int open = 0;

                for (int i = 0; i < 8; i++) {
                    if (map->GetXY(x + dx8[i], y + dy8[i]) == floor) {
                        open++;
                    }
                }

                if (open >= 5) {
                    opened.emplace_back(x, y);
                }
            }
        }

        for (const XPoint& point : opened) {
            map->SetXY(point.x, point.y, floor);
        }
    }

    // 7. The lakes. Each one goes in on its own and stays only if the
    // floor is still one piece without it - a lake that would strand
    // half the level simply is not there, which reads better than a
    // causeway carved through the middle of it.
    if (water.shallow != XTileType::NONE) {
        const auto seed = static_cast<unsigned int>(vRand(0x7fffffff));
        std::vector<double> depth(len * hgt, 0.0);
        std::vector<bool> flooded(len * hgt, false);
        std::vector<double> levels;

        for (int y = 0; y < hgt; y++) {
            for (int x = 0; x < len; x++) {
                if (map->GetXY(x, y) == floor) {
                    depth[x + y * len] = Noise(x, y, water.grain, seed);
                    levels.push_back(depth[x + y * len]);
                }
            }
        }

        // Averaging octaves piles the noise up around its middle, so a
        // fixed threshold floods everything or nothing depending on where
        // the lattice happened to fall. The share of the floor that goes
        // under is asked for directly instead, and the water line is
        // whatever value delivers it on this particular level.
        std::sort(levels.begin(), levels.end());

        const auto Waterline = [&levels](const int share) {
            const int over = static_cast<int>(levels.size()) * std::clamp(share, 0, 100) / 100;

            return over > 0 ? levels[levels.size() - static_cast<size_t>(over)] : 2.0;
        };

        const double shallow_line = Waterline(water.level);
        const double deep_line = Waterline(water.level * water.deep_level / 100);

        for (int cell = 0; cell < len * hgt; cell++) {
            flooded[cell] = depth[cell] >= shallow_line && map->GetXY(cell % len, cell / len) == floor;
        }

        std::vector<bool> done(len * hgt, false);

        for (int start = 0; start < len * hgt; start++) {
            if (!flooded[start] || done[start]) {
                continue;
            }

            // One lake: everything reachable from here that is under
            // water too.
            std::vector<int> lake;
            std::vector<int> stack{start};
            done[start] = true;

            while (!stack.empty()) {
                const int cell = stack.back();
                stack.pop_back();
                lake.push_back(cell);

                for (int i = 0; i < 4; i++) {
                    const int nx = cell % len + dx4[i];
                    const int ny = cell / len + dy4[i];

                    if (nx < 0 || ny < 0 || nx >= len || ny >= hgt) {
                        continue;
                    }

                    if (const int n = nx + ny * len; flooded[n] && !done[n]) {
                        done[n] = true;
                        stack.push_back(n);
                    }
                }
            }

            for (const int cell : lake) {
                const bool deep = water.deep != XTileType::NONE && depth[cell] >= deep_line;
                map->SetXY(cell % len, cell / len, deep ? water.deep : water.shallow);
            }

            if (!map->isFullyConnected()) {
                for (const int cell : lake) {
                    map->SetXY(cell % len, cell / len, floor);
                }
            }
        }
    }

    // Growing every chamber from a doorway graph that reaches all of
    // them leaves the floor connected by construction, and the lakes
    // above put back anything that was not. This is the net under all of
    // that: whatever happened, a stairway placed afterwards by GetFreeXY()
    // over the floor can be walked to from anywhere else on it.
    if (!map->isFullyConnected()) {
        map->ConnectAllRegions(floor);
    }
}
