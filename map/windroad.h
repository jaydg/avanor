/* Written by Kusigrosz in September 2010 (parts much earlier)
 * This program is in public domain, with all its bugs etc.
 * No warranty whatsoever.
 *
 * Generating winding roads/corridors for a roguelike game.
 *
 * The functions that do road generating are:
 *
 * uti_windroad(&road, mpc, x1, y1, x2, y2, pertamt)
 *     Generates a winding road from x1, y1 to x2, y2 without
 *     sharp turns. The road winds regardless of the relative location
 *     of endpoints (unless it is too short). The parameter pertamt
 *     controls the degree of perturbation the initially straight road
 *     is subjected to; typical values of 5-50 give decent results.
 *     mpc is the pointer to the map structure (needed to make sure
 *     the winding road stays within the map.
 *
 * uti_zigzag(&road, x1, y1, x2, y2, turnpct, diagpct)
 *     Generates a randomly zigzagging road from x1, y1 to x2, y2
 *     The road zigzags only if the endpoints differ in both coordinates,
 *     Otherwise it is a straight line. The parameter turnpct is the
 *     chance of a non-forced turn in percent (so, 100/turnpct is
 *     approximately the length of a straight segment; diagpct is
 *     the chance that a diagonal turn is allowed.
 *
 * uti_sigsag(&road, x1, y1, x2, y2, turnpct, diagpct)
 *     The same as zigzag, but without sharp corners.
 */

/* Adapted for Avanor. The algorithm is unchanged; what differs is how it
 * talks to the rest of the game:
 *
 *  - rand() became vRand(). World generation has to be reproducible from
 *    the --seed argument, and the C library generator is not part of that
 *    stream.
 *  - struct seqcells (fixed MAX_SEQ arrays) became std::vector<XPoint>.
 *    kMaxCells survives only as a loop guard, since Zigzag() walks until
 *    it arrives rather than for a known number of steps.
 *  - struct mappiece became the game's XMap, which is only ever asked for
 *    its extent - InBorder() is the original uti_inbord, one cell in.
 *  - RQR(), which printed to stderr and exited the process, became assert()
 *    for things a caller got wrong. Running out of cells ends the path
 *    instead of ending the game.
 *  - The demo scaffolding is gone: uti_makempc(), putroad(), main(), and
 *    the FLOOR/WALL/TRN_XSIZE/TRN_YSIZE defines. Callers decide what a road
 *    is made of; these functions only say where it goes.
 */

#ifndef WINDROAD_H
#define WINDROAD_H

#include <vector>

#include "helpers/point.h"

class XMap;

// Paths that wander. Every function returns the cells of one path in
// order, starting at `from` and ending at `to`, with no gaps: consecutive
// cells always touch, orthogonally or diagonally. An empty result means the
// path could not be built.
namespace windroad {

// The most cells any one path may occupy. Reaching it stops the path where
// it stands rather than aborting - long before this a road has gone wrong
// anyway, and a truncated corridor is easier to look at than a crash.
constexpr int kMaxCells = 1024;

// uti_windroad(). A winding road from `from` to `to` without sharp turns,
// kept inside the map. `pertamt` controls how far the initially straight
// line is pushed about; 5-50 give decent results.
std::vector<XPoint> Wind(const XMap& map, XPoint from, XPoint to, int pertamt);

// uti_zigzag(). Zigzags only when the ends differ in both coordinates,
// otherwise a straight line. `turnpct` is the chance of an unforced turn,
// so 100/turnpct is roughly the length of a straight run; `diagpct` is the
// chance a diagonal step is allowed. Stays inside the rectangle spanned by
// the two ends, so it needs no map.
std::vector<XPoint> Zigzag(XPoint from, XPoint to, int turnpct, int diagpct);

// uti_sigsag(). The same as Zigzag(), but with the sharp corners cut off.
std::vector<XPoint> Sigsag(XPoint from, XPoint to, int turnpct, int diagpct);

// uti_rline(). The Bresenham line, exposed because it is useful on its own
// and because the two above are built from it. Not symmetrical: swapping
// the ends can give a different line.
std::vector<XPoint> Line(XPoint from, XPoint to);

} // namespace windroad

#endif
