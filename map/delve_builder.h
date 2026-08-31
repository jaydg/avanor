/* Written by Kusigrosz in December 2008, April 2011
 * This program is in public domain, with all its bugs etc.
 * No warranty whatsoever.
 *
 * Delve a random maze/cavern. The results are written as text to stdout
 * and as a bmp file to BMP_OUTFILE (see the #defines below the comment),
 *
 * Usage:
 * default:
 *     a.out name pullflag storeflag cellnum
 * alternative:
 *     a.out pullflag storeflag cellnum
 *
 * Arguments:
 *     name: the name of the string describing the digperm array, which
 *         holds probabilities of digging being permitted for different
 *         neighbour patterns. If the name is not found, it is assumed
 *         the program is invoked in the alternative form, when the
 *         digperm table is generated randomly.
 *     pullflag: defines how next cells to be considered for digging
 *         are selected from the store; see below for details.
 *     storeflag: defines how neighbouring cells are stored; see below
 *         for details.
 *     cellnum: the maximum number of floor cells that will be generated.
 *
 * The default values of the arguments are #defined below this comment.
 *
 * Algorithm description:
 * The algorithm operates on a rectangular grid. Each cell can be WALL
 * or FLOOR. A (non-border) cell has 8 neigbours - diagonals count.
 * There is also a cell store with two operations: store a given cell
 * on top, and pull a cell from the store. There is no check for
 * repetitions, so a given cell can be stored multiple times.
 * The cell to be pulled is selected depending on the value of pullflag:
 *     PULL_CUBEROOT: randomly from all the store if N_in_store < 125,
 *         and randomly from the top 25 * cube_root(N_in_store) otherwise.
 *     PULL_ALL: randomly from all the store.
 *     PULL_BOTTOM: always the bottommost cell of the store.
 * The algorithm starts with most of the map filled with WALL, with a
 * "seed" of some FLOOR cells; their neigbouring WALL cells are in
 * store. The main loop in delveon() is repeated until the desired
 * number of FLOOR cells is reached, or there is nothing in store:
 *     Pull a cell from the store;
 *     Encode the WALL/FLOOR pattern of its 8 neighbours in an 8-bit
 *         number, and use it as an index into the digperm table, to
 *         fetch the permil probability of such pattern permitting digging.
 *     With the given probability, convert the cell to FLOOR, and store
 *         its WALL neighbours, depending on the value of the storeflag:
 *         STORE_PERM: in random order
 *         STORE_CW: clockwise (starting from a random one)
 *         STORE_CCW: counterclockwise (starting from a random one)
 *
 * The digperm table entries are 256 values from 0 to 1000; the bits
 * of the indices correspond to the neighbours of the given cell - lsb
 * is the cell to the right, then clockwise; FLOOR is 1 and WALL 0.
 * The values are permil probabilities of the cell being allowed for
 * digging. For example, an entry 750 at index 179 (10110011) means
 * that the pattern:
 *  101
 *  1c1
 *  001
 * will permit digging of the central cell with probability 0.75
 *
 * If the name of the digperm string is not found, the digperm table
 * is generated randomly in randdigperm. As such random generation
 * may result in a table that won't allow digging enough cells,
 * the table generation / cavern generation is repeated until a high
 * enough number of cells is dug.
 *
 * The digperm table used in generating the current pattern is dumped
 * to the file DIGPERM_OUTFILE.
 */

/* Adapted for Avanor. The algorithm is untouched; what differs is how it
 * talks to the rest of the game:
 *
 *  - The description above is the original's, kept whole. Two parts of it
 *    now describe something that is gone: there is no command line, and
 *    nothing is written to stdout or to a bmp/digperm file. The four
 *    arguments it documents survive exactly, as the constructor's
 *    `pattern`, `pull`, `store` and `cells` - so read "Arguments:" as a
 *    description of those.
 *  - rand() became vRand(). World generation has to be reproducible from
 *    the --seed argument, and the C library generator is not part of that
 *    stream.
 *  - struct mappiece became the game's XMap, and the FLOOR/WALL chars
 *    became the wall/floor tiles the caller names, so a level can be delved
 *    out of magma, ice or anything else the world defines.
 *  - struct cellstore (two malloc'd int arrays) became std::vector<XPoint>.
 *    The original sized it at 8 * xsize * ysize, which is exactly the worst
 *    case - eight chances to be stored for every cell - so its "replace a
 *    random entry when full" branch could never run. A vector that grows
 *    cannot reach it either, and it is gone with the malloc.
 *  - RQR(), which printed to stderr and exited the process, became assert()
 *    for what a caller got wrong. Digging that dies young is not an error:
 *    it ends the attempt, and Build() tries again.
 *  - The demo scaffolding is gone: main(), txt_dumpmpc(), checkparams(),
 *    mpc_mkempty()/mpc_fill(), the bmp output and the TRN_XSIZE/TRN_YSIZE
 *    defines. Callers say how big the map is and what it is made of.
 *  - The digperm library, and finddesc() that looked names up in it, are
 *    gone from here as well. The tables are what a level looks like rather
 *    than how the algorithm works, so they live in world/delve_patterns.lua
 *    with the rest of the world, where a designer can add to them. The
 *    strings themselves are unchanged and still in Kusigrosz's format, so
 *    any table written for the original can be pasted straight in.
 *
 * Two bugs in randdigperm() are kept as they are, because they only shape
 * the taste of the random tables and the author's notice above is explicit
 * about bugs. Both are noted where they sit, in delve_builder.cpp.
 */

#ifndef DELVE_BUILDER_H
#define DELVE_BUILDER_H

#include <string>
#include <vector>

#include "map/map.h"

class XLocation;

// Delves a random maze or cavern: floor is eaten out of solid wall one
// cell at a time, and which cells may be eaten is decided by a 256-entry
// table of neighbourhood patterns. Changing that table alone turns the
// same code from a rectangular maze into a diagonal warren into an open
// cavern - see the named patterns in delve_builder.cpp.
//
// Everything it digs hangs together by construction: a cell only enters
// the store when a neighbour of it has just been dug, so the floor is one
// connected region growing out of the seed at the centre.
class XDelveBuilder
{
    public:
        // Which cell of the store is considered next. The original's
        // pullflag; the names are its comments.
        enum class Pull {
            CUBEROOT = 1, // fluffy patterns
            ALL = 2,      // compact patterns
            BOTTOM = 3,   // usually winding patterns
        };

        // The order a dug cell's wall neighbours go into the store.
        // The original's storeflag.
        enum class Store {
            PERM = 1, // random order
            CW = 2,   // clockwise from a random one
            CCW = 3,  // counterclockwise from a random one
        };

        // How the digging permissions are arrived at, in this order:
        //
        //  - `desc` non-empty: Kusigrosz's own format, "index:permil"
        //    separated by spaces, as written in world/delve_patterns.lua.
        //  - otherwise `ngb_max` above zero: built from the parameters
        //    instead - any cell with between ngb_min and ngb_max floor
        //    neighbours may be dug, at 1000 permil where that would not
        //    join two separate pieces of floor and at `conmil` where it
        //    would.
        //  - otherwise: a table invented on the spot, which is what the
        //    original did when its first argument named nothing it knew.
        //
        // `cells` is the most floor cells to dig. 0 asks for the
        // original's default of one fifth of the map.
        XDelveBuilder(XLocation* _location, const int _w, const int _h,
                      const XTileType::Id _wall, const XTileType::Id _floor,
                      std::string _desc, const int _ngb_min, const int _ngb_max,
                      const int _conmil, const int _cells,
                      const Pull _pull, const Store _store)
            : location(_location), w(_w), h(_h), wall(_wall), floor(_floor),
              desc(std::move(_desc)), ngb_min(_ngb_min), ngb_max(_ngb_max),
              conmil(_conmil), cells(_cells), pull(_pull), store(_store)
        {
        }

        void Build();

    private:
        XLocation* location;
        int w, h;
        XTileType::Id wall;
        XTileType::Id floor;
        std::string desc;
        int ngb_min, ngb_max, conmil;
        int cells;
        Pull pull;
        Store store;
};

#endif
