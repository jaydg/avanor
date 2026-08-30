/* Written by Kusigrosz in September 2010 (parts much earlier)
 * This program is in public domain, with all its bugs etc.
 * No warranty whatsoever.
 *
 * Generating winding roads/corridors for a roguelike game.
 *
 * See map/windroad.h for the original description of the three road
 * generators, and for what was changed to bring the code into Avanor.
 */

#include <cassert>
#include <cstdlib>

#include "engine/global.h"
#include "map/map.h"
#include "map/windroad.h"

namespace windroad {

namespace {

constexpr int SQR(const int x)
{
    return x * x;
}

/* Globals: */
constexpr int Xoff[8] = {1,  1,  0, -1, -1, -1,  0,  1};
constexpr int Yoff[8] = {0,  1,  1,  1,  0, -1, -1, -1};

int rnd_i0(const int n) /* 0 <= rnd_i0(n) < n */
{
    assert(n > 0);
    return static_cast<int>(vRand(static_cast<unsigned long>(n)));
}

int rnd_ic(const int p, const int n) /* Chance p out of n. Must return 0 or 1 */
{
    return static_cast<int>(vRand(static_cast<unsigned long>(n))) < p ? 1 : 0;
}

int uti_sign(const int n)
{
    if (n > 0) {
        return 1;
    } else if (n == 0) {
        return 0;
    } else {
        return -1;
    }
}

int uti_nsteps(const int x1, const int y1, const int x2, const int y2)
{
    const int dx = abs(x1 - x2);
    const int dy = abs(y1 - y2);

    return (dx > dy) ? dx : dy;
}

/* Is the location within the borders - with 1 cell margin
 */
bool uti_inbord(const XMap& map, const int x, const int y)
{
    return !((x < 1) || (x >= map.len - 1) || (y < 1) || (y >= map.hgt - 1));
}

/* The square of the cosine of the angle between vectors p0p1 and p1p2,
 * with the sign of the cosine, in permil (1.0 = 1000).
 */
int uti_signcos2(const int x0, const int y0, const int x1, const int y1,
                 const int x2, const int y2)
{
    const int sqlen01 = SQR(x1 - x0) + SQR(y1 - y0);
    const int sqlen12 = SQR(x2 - x1) + SQR(y2 - y1);
    assert(sqlen01 && sqlen12);

    const int prod = (x1 - x0) * (x2 - x1) + (y1 - y0) * (y2 - y1);
    int val = 1000 * (prod * prod / sqlen01) / sqlen12; /* Overflow? */

    if (prod < 0) {
        val = -val;
    }

    return val;
}

/* Select random points in the provided trajectory and displace them
 * provided no sharp angles are created, and the new location isn't
 * too close or too far from the neighbours.
 */
void uti_perturb(std::vector<XPoint>& way, const XMap& map,
                 const int mindist, const int maxdist, const int pertamt)
{
    const int mincos2 = 500; /* cos^2 in 1/1000, for angles < 45 degrees */

    assert((mindist > 0) && (maxdist > mindist));

    if (way.size() < 3) { /* nothing to do */
        return;
    }

    const int len = static_cast<int>(way.size());
    const int mind2 = SQR(mindist);
    const int maxd2 = SQR(maxdist);

    for (int i = 0; i < pertamt * len; i++) {
        const int ri = 1 + rnd_i0(len - 2);
        const int rdir = rnd_i0(8);
        const int nx = way[ri].x + Xoff[rdir];
        const int ny = way[ri].y + Yoff[rdir];
        const int lox = way[ri - 1].x;
        const int loy = way[ri - 1].y;
        const int hix = way[ri + 1].x;
        const int hiy = way[ri + 1].y;
        const int lod2 = SQR(nx - lox) + SQR(ny - loy);
        const int hid2 = SQR(nx - hix) + SQR(ny - hiy);

        if (!uti_inbord(map, nx, ny)
            || (lod2 < mind2) || (lod2 > maxd2)
            || (hid2 < mind2) || (hid2 > maxd2)) {
            continue;
        }

        /* Check the angle at ri (vertex at nx, ny) */
        if (uti_signcos2(lox, loy, nx, ny, hix, hiy) < mincos2) {
            continue;
        }

        /* Check the angle at ri - 1 (vertex at lox, loy) */
        if ((ri > 1) && (uti_signcos2(way[ri - 2].x, way[ri - 2].y,
                                      lox, loy, nx, ny) < mincos2)) {
            continue;
        }

        /* Check the angle at ri + 1 (vertex at hix, hiy) */
        if ((ri < len - 2) && (uti_signcos2(nx, ny, hix, hiy,
                                            way[ri + 2].x, way[ri + 2].y) < mincos2)) {
            continue;
        }

        way[ri].Set(nx, ny);
    }
}

/* Connect the waypoints in way with straight lines, putting the result
 * in the returned structure.
 */
bool uti_connwaypts(std::vector<XPoint>& result, const std::vector<XPoint>& waypts)
{
    assert(waypts.size() > 1);

    result.clear();
    result.push_back(waypts[0]);

    for (size_t i = 0; i + 1 < waypts.size(); i++) {
        const std::vector<XPoint> segment = Line(waypts[i], waypts[i + 1]);

        for (size_t j = 1; (j < segment.size()) && (result.size() < kMaxCells); j++) {
            result.push_back(segment[j]);
        }
    }

    return result.size() < kMaxCells;
}

void uti_cutcorners(std::vector<XPoint>& seq)
{
    if (seq.size() < 3) {
        return;
    }

    size_t i = 1;
    size_t j = 1;

    for (; i + 1 < seq.size(); i++) { /* all points except the ends */
        seq[j] = seq[i];

        if (uti_nsteps(seq[j - 1].x, seq[j - 1].y, seq[i + 1].x, seq[i + 1].y) > 1) {
            j++;
        }
    }

    seq[j] = seq[i];
    j++;

    seq.resize(j);
}

} // namespace

/* The Bresenham line algorithm. Not symmetrical.
 */
std::vector<XPoint> Line(const XPoint from, const XPoint to)
{
    std::vector<XPoint> ret;

    const int x1 = from.x, y1 = from.y;
    const int x2 = to.x, y2 = to.y;

    /* "thin" line, so this check should be enough */
    assert((abs(x2 - x1) < kMaxCells) && (abs(y2 - y1) < kMaxCells));

    const int xstep = uti_sign(x2 - x1);
    const int ystep = uti_sign(y2 - y1);

    int xc = x1;
    int yc = y1;

    ret.emplace_back(xc, yc);

    if ((x1 == x2) && (y1 == y2)) {
        return ret;
    }

    int acc;

    if (abs(x2 - x1) >= abs(y2 - y1)) {
        acc = abs(x2 - x1);

        do {
            xc += xstep;
            acc += 2 * abs(y2 - y1);

            if (acc >= 2 * abs(x2 - x1)) {
                acc -= 2 * abs(x2 - x1);
                yc += ystep;
            }

            ret.emplace_back(xc, yc);
        } while ((xc != x2) && (ret.size() < kMaxCells));
    } else {
        acc = abs(y2 - y1);

        do {
            yc += ystep;
            acc += 2 * abs(x2 - x1);

            if (acc >= 2 * abs(y2 - y1)) {
                acc -= 2 * abs(y2 - y1);
                xc += xstep;
            }

            ret.emplace_back(xc, yc);
        } while ((yc != y2) && (ret.size() < kMaxCells));
    }

    return ret;
}

/* A randomized zigzag from x1, y1 to x2, y2.
 * The zigzag stays within the rectangle spanned by the ends.
 * turnpct is the percent chance of a (non-forced) turn
 * (so 100/turnpct is the average length of a straight section)
 * and diagpct is the chance of diagonal move.
 */
std::vector<XPoint> Zigzag(const XPoint from, const XPoint to,
                           const int turnpct, const int diagpct)
{
    std::vector<XPoint> ret;

    assert((diagpct >= 0) && (diagpct <= 100));
    assert((turnpct >= 0) && (turnpct <= 100));

    const int x2 = to.x, y2 = to.y;
    int xc = from.x;
    int yc = from.y;
    int deltax = 0, deltay = 0; /* will be overwritten anyway. */

    ret.emplace_back(xc, yc);

    while ((ret.size() < kMaxCells) && ((xc != x2) || (yc != y2))) {
        const int xremain = abs(x2 - xc); /* the x, y steps left to go */
        const int yremain = abs(y2 - yc);

        /* first step, random turn, or forced turn: new values for deltas */
        if ((ret.size() == 1) || rnd_ic(turnpct, 100)
            || (abs(x2 - (xc + deltax)) > xremain)
            || (abs(y2 - (yc + deltay)) > yremain)
            || ((xremain == yremain) && rnd_ic(diagpct, 100))) {
            deltax = uti_sign(x2 - xc);
            deltay = uti_sign(y2 - yc);

            if (rnd_ic(diagpct, 100)) { /* diags OK */
                if (xremain > yremain) { /* so deltax is nonzero */
                    if (rnd_ic(xremain - yremain, xremain)) {
                        deltay = 0;
                    }
                } else if (xremain < yremain) { /* so deltay is nonzero */
                    if (rnd_ic(yremain - xremain, yremain)) {
                        deltax = 0;
                    }
                }
                /* else they are equal and nonzero - deltas stay */
            } else { /* no diags */
                if (rnd_ic(xremain, xremain + yremain)) {
                    if (deltax) {
                        deltay = 0;
                    }
                } else {
                    if (deltay) {
                        deltax = 0;
                    }
                }
            }
        }

        xc += deltax;
        yc += deltay;

        ret.emplace_back(xc, yc);
    }

    return ret;
}

/* Like zigzag, but with no straight turns. Just make a zigzag and
 * cut corners.
 */
std::vector<XPoint> Sigsag(const XPoint from, const XPoint to,
                           const int turnpct, const int diagpct)
{
    std::vector<XPoint> road = Zigzag(from, to, turnpct, diagpct);
    uti_cutcorners(road);

    return road;
}

/* Generate a road from x1, y1 to x2, y2.
 */
std::vector<XPoint> Wind(const XMap& map, const XPoint from, const XPoint to,
                         const int pertamt)
{
    // The original asserted this and died; a level script can ask for a
    // road to anywhere, so refuse instead. An empty path is a caller's
    // answer, not the end of the game.
    if (!uti_inbord(map, from.x, from.y) || !uti_inbord(map, to.x, to.y)) {
        return {};
    }

    std::vector<XPoint> waypts = Line(from, to);

    if (waypts.size() < 5) { /* Too short to wind, just copy the straight line */
        return waypts;
    }

    /* Copy one cell in two/three, making sure the ends are copied */
    const int len = static_cast<int>(waypts.size());
    int j = 0;

    for (int i = 0; i < len; ) {
        waypts[j] = waypts[i];
        j++;

        if ((i < len - 5) || (i >= len - 1)) {
            i += 2 + rnd_i0(2);
        } else if (i == len - 5) {
            i += 2;
        } else {
            i = len - 1;
        }
    }

    waypts.resize(j);

    uti_perturb(waypts, map, 2, 5, pertamt); /* waypoint dist min, max */

    std::vector<XPoint> road;
    uti_connwaypts(road, waypts);
    uti_cutcorners(road); /* Connecting may sometimes make 'L' corners */

    return road;
}

} // namespace windroad
