/* Written by Kusigrosz in December 2008, April 2011
 * This program is in public domain, with all its bugs etc.
 * No warranty whatsoever.
 *
 * Delve a random maze/cavern.
 *
 * See map/delve_builder.h for the original description of the algorithm
 * and its parameters, and for what was changed to bring the code into
 * Avanor.
 */

#include <array>
#include <cassert>
#include <cctype>
#include <cstdio>
#include <string>
#include <vector>

#include "engine/global.h"
#include "game/location.h"
#include "helpers/point.h"
#include "map/delve_builder.h"
#include "map/map.h"

namespace {

// The digging permissions table: for each of the 256 ways a cell's eight
// neighbours can be wall or floor, the permil chance that cell may be dug.
using DigPerm = std::array<int, 256>;

/* Rotate left an 8-bit value.
 */
constexpr int ROTL_8(const int d, const int n)
{
    return ((d >> (8 - n)) & 0xff) | ((d << n) & 0xff);
}

/* Globals: */
constexpr int Xoff[8] = {1,  1,  0, -1, -1, -1,  0,  1};
constexpr int Yoff[8] = {0,  1,  1,  1,  0, -1, -1, -1};


/* A table of neighbour patterns and their digperm indices is
 * presented below; '@' represents FLOOR and '-' WALL. A usable
 * digperm string like those above needs to contain only entries
 * with nonzero probabilities - others can be omitted.
 *
 * When creating a digperm table, the probabilities in entries
 * corresponding to the given pattern rotated by 90, 180 and 270
 * degrees are set (by setsymmetr()) to the same value - otherwise
 * the generated cavern would 'lean' in one direction.

#---   ---   ---   ---   ---   ---   ---   ---   ---   ---   @--
#-c-   -c@   -c-   -c@   -c@   -c-   -c@   -c-   @c@   @c-   -c-
#---   ---   --@   --@   -@-   -@@   @--   @-@   ---   --@   --@
0:0   1:0   2:0   3:0   5:0   6:0   9:0   10:0  17:0  18:0  34:0

#---   ---   ---   ---   ---   ---   ---   ---   ---   @--   @--
#-c@   -c@   -c@   -c-   @c@   @c@   @c-   @c@   @c-   -c@   -c@
#-@@   @-@   @@-   @@@   --@   -@-   -@@   @--   @-@   --@   -@-
7:0   11:0  13:0  14:0  19:0  21:0  22:0  25:0  26:0  35:0  37:0

#@--   @--   @--   ---   ---   ---   ---   ---   @--   @--   @--
#-c-   -c@   -c-   -c@   @c@   @c@   @c@   @c-   -c@   -c@   -c@
#-@@   @--   @-@   @@@   -@@   @-@   @@-   @@@   -@@   @-@   @@-
38:0  41:0  42:0  15:0  23:0  27:0  29:0  30:0  39:0  43:0  45:0

#@--   @--   @--   @--   @--   @--   -@-   -@-   -@-   @@-   @@-
#-c-   @c@   @c@   @c-   @c@   @c-   @c@   @c-   @c-   -c-   -c-
#@@@   --@   -@-   -@@   @--   @-@   -@-   -@@   @-@   -@@   @-@
46:0  51:0  53:0  54:0  57:0  58:0  85:0  86:0  90:0  102:0 106:0

#@-@   ---   @--   @--   @--   @--   @--   -@-   -@-   -@-   @@-
#-c-   @c@   -c@   @c@   @c@   @c@   @c-   @c@   @c@   @c-   -c@
#@-@   @@@   @@@   -@@   @-@   @@-   @@@   -@@   @-@   @@@   -@@
170:0 31:0  47:0  55:0  59:0  61:0  62:0  87:0  91:0  94:0  103:0

#@@-   @@-   @@-   @-@   @--   -@-   @@-   @@-   @@-   @@-   @-@
#-c@   -c-   @c-   -c@   @c@   @c@   -c@   @c@   @c@   @c-   -c@
#@-@   @@@   @-@   @-@   @@@   @@@   @@@   -@@   @-@   @@@   @@@
107:0 110:0 122:0 171:0 63:0  95:0  111:0 119:0 123:0 126:0 175:0

#@-@   @@-   @-@   @@@
#@c@   @c@   @c@   @c@
#@-@   @@@   @@@   @@@
187:0 127:0 191:0 255:0
 */

/* The Hamming weights (numbers of 1's) of values 0 - 255 */
static const int hamming[256] =
    {
/******  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f */
/* 00 */ 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
/* 10 */ 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
/* 20 */ 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
/* 30 */ 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
/* 40 */ 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
/* 50 */ 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
/* 60 */ 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
/* 70 */ 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
/* 80 */ 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
/* 90 */ 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
/* a0 */ 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
/* b0 */ 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
/* c0 */ 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
/* d0 */ 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
/* e0 */ 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
/* f0 */ 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
    };

/* The number of groups of 1's in the 8 cell neighbourhood of a cell.
 * The lsb corresponds to the cell to the right, then clockwise.
 * A group is a set of cells that are connected (diagonally too).
 * For example:
 * 101                 110              101
 * 1x0 2 groups        1x1 1 group      0x0 4 groups
 * 010                 010              101
 */
static const int ngb_grouptab[256] =
    {
/********  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f */
/* 00 */   0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1,
/* 10 */   1, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1,
/* 20 */   1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 2, 2, 2, 2,
/* 30 */   1, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1,
/* 40 */   1, 1, 2, 1, 2, 1, 2, 1, 2, 2, 3, 2, 2, 1, 2, 1,
/* 50 */   1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1,
/* 60 */   1, 1, 2, 1, 2, 1, 2, 1, 2, 2, 3, 2, 2, 1, 2, 1,
/* 70 */   1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1,
/* 80 */   1, 1, 2, 1, 2, 1, 2, 1, 2, 2, 3, 2, 2, 1, 2, 1,
/* 90 */   2, 2, 3, 2, 2, 1, 2, 1, 2, 2, 3, 2, 2, 1, 2, 1,
/* a0 */   2, 2, 3, 2, 3, 2, 3, 2, 3, 3, 4, 3, 3, 2, 3, 2,
/* b0 */   2, 2, 3, 2, 2, 1, 2, 1, 2, 2, 3, 2, 2, 1, 2, 1,
/* c0 */   1, 1, 2, 1, 2, 1, 2, 1, 2, 2, 3, 2, 2, 1, 2, 1,
/* d0 */   1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1,
/* e0 */   1, 1, 2, 1, 2, 1, 2, 1, 2, 2, 3, 2, 2, 1, 2, 1,
/* f0 */   1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1
    };

int rnd_i0(const int n) /* 0 <= rnd_i0(n) < n */
{
    assert(n > 0);
    return static_cast<int>(vRand(static_cast<unsigned long>(n)));
}

void rnd_perm(int* tab, const int nelem)
{
    assert(tab && (nelem >= 0));

    for (int i = 0; i < nelem; i++) {
        const int rind = rnd_i0(i + 1);

        const int tmp = tab[rind];
        tab[rind] = tab[i];
        tab[i] = tmp;
    }
}

/* Integer cube root approximation (from below) for positive ints < 2**31
 */
int uti_icbrt(const int arg)
{
    assert((arg >= 0) && (arg <= 0x7fffffff));

    if (arg == 0) {
        return 0;
    }

    int ret;

    if (arg < 1000) {
        ret = 10;
    } else if (arg < 1000000) {
        ret = 100;
    } else if (arg < 1000000000) {
        ret = 1000;
    } else {
        ret = 1290; /* the largest x for which x**3 < 2**31 */
    }

    int delta;

    do {
        /* derivative = 3 * x * x yadda yadda, but this seems to works OK */
        delta = (arg - ret * ret * ret) / (2 * ret * ret);
        ret += delta;
    } while (delta);

    if (ret * ret * ret > arg) {
        ret--;
    }

    return ret;
}

/* Is the location within the borders - with 1 cell margin
 */
bool inbord(const XMap& map, const int x, const int y)
{
    return !((x < 1) || (x >= map.len - 1) || (y < 1) || (y >= map.hgt - 1));
}

/* The store of cells waiting to be considered for digging. The original's
 * struct cellstore: a cell may be stored more than once, and pulling any
 * cell but the topmost moves the topmost into the hole it leaves.
 */
class CellStore
{
    public:
        void Store(const int x, const int y)
        {
            cells.emplace_back(x, y);
        }

        /* Remove a cell from the store and put its coords in *x, *y.
         * Selection of the cell to be removed depends on the pullflag.
         * NOTE: pulling any cell except the topmost one puts the topmost one
         * in its place.
         */
        bool Pull(int* x, int* y, const XDelveBuilder::Pull pullflag)
        {
            assert(x && y);

            if (cells.empty()) {
                return false; /* no cells */
            }

            const int index = static_cast<int>(cells.size());
            int rind;

            switch (pullflag) {
                case XDelveBuilder::Pull::CUBEROOT: /* fluffy patterns */
                    rind = (index < 125)
                        ? rnd_i0(index)
                        : index - rnd_i0(25 * uti_icbrt(index)) - 1;
                    break;

                case XDelveBuilder::Pull::ALL: /* compact patterns */
                    rind = rnd_i0(index);
                    break;

                case XDelveBuilder::Pull::BOTTOM: /* usually winding patterns */
                    rind = 0;
                    break;

                default:
                    assert(false); /* unimplemented */
                    return false;
            }

            *x = cells[rind].x;
            *y = cells[rind].y;

            if (index - 1 != rind) { /* not the topmost cell - overwrite */
                cells[rind] = cells[index - 1];
            }

            cells.pop_back();

            return true;
        }

    private:
        std::vector<XPoint> cells;
};

/* Is digging the cell at x, y permitted by digperm? Randomized,
 * uses the values in digperm as permil probability.
 */
bool ispermitted(const XMap& map, const DigPerm& digperm,
                 const int x, const int y, const XTileType::Id trn)
{
    int bitmap = 0; /* lowest bit is the cell to the right, then clockwise */

    for (int i = 0; i < 8; i++) {
        bitmap >>= 1;

        if (inbord(map, x + Xoff[i], y + Yoff[i])
            && map.GetXY(x + Xoff[i], y + Yoff[i]) == trn) {
            bitmap |= 0x80;
        }
    }

    return rnd_i0(1000) < digperm[bitmap];
}

/* Dig out an ava cell to flo and store its ava neighbours in
 * the order depending on storeflag.
 */
int digcell(XMap& map, CellStore& cstore, const int x, const int y,
            const XDelveBuilder::Store storeflag,
            const XTileType::Id flo, const XTileType::Id ava)
{
    int order[8] = {0, 1, 2, 3, 4, 5, 6, 7};

    if (!inbord(map, x, y) || (map.GetXY(x, y) != ava)) {
        return 0; /* did nothing */
    }

    map.SetXY(x, y, flo);

    switch (storeflag) {
        case XDelveBuilder::Store::PERM:
            rnd_perm(order, 8);
            break;

        case XDelveBuilder::Store::CW: {
            const int r = rnd_i0(8);

            for (int i = 0; i < 8; i++) {
                order[i] = (r + i) % 8;
            }

            break;
        }

        case XDelveBuilder::Store::CCW: {
            const int r = rnd_i0(8);

            for (int i = 0; i < 8; i++) {
                order[i] = (8 + r - i) % 8;
            }

            break;
        }

        default:
            assert(false);
            break;
    }

    for (int i = 0; i < 8; i++) {
        const int j = order[i];

        if (inbord(map, x + Xoff[j], y + Yoff[j])
            && (map.GetXY(x + Xoff[j], y + Yoff[j]) == ava)) {
            cstore.Store(x + Xoff[j], y + Yoff[j]);
        }
    }

    return 1; /* dug 1 cell */
}

/* Continue digging until cellnum or no more cells in store.
 */
int delveon(XMap& map, CellStore& cstore, const DigPerm& digperm,
            const int cellnum, const XDelveBuilder::Pull pullflag,
            const XDelveBuilder::Store storeflag,
            const XTileType::Id flo, const XTileType::Id ava)
{
    int count = 0;
    int x, y;

    assert(cellnum >= 0);
    assert(flo != ava);

    while ((count < cellnum) && cstore.Pull(&x, &y, pullflag)) {
        if (ispermitted(map, digperm, x, y, flo)) {
            count += digcell(map, cstore, x, y, storeflag, flo, ava);
        }
    }

    return count;
}

/* Select a digperm index of Hamming weight no higher than maxham.
 * Prefer those with a higher Hamming weight - among equals chose
 * with probability proportional to the value of the digperm entry.
 * If no positive probabilities found, return -1;
 */
int findseed(const DigPerm& digperm, const int maxham)
{
    int ret = -1;
    int sumprob = 0;
    int maxtillnow = 0;

    for (int i = 0; i < 256; i++) {
        if ((hamming[i] <= maxham) && (digperm[i] > 0)) {
            if (hamming[i] > maxtillnow) {
                sumprob = digperm[i];
                maxtillnow = hamming[i];
            } else if (hamming[i] == maxtillnow) {
                sumprob += digperm[i];
            } else {
                continue;
            }

            if (rnd_i0(sumprob) < digperm[i]) {
                ret = i;
            }
        }
    }

    return ret;
}

/* Generate a random cavern of cellnum cells.
 * Old type caverns can be generated by calling first:
 * filldigperm(&digperm, ngb_min, ngb_max, conmil, NULL);
 * then:
 * cavern(mpc, xorig, yorig, digperm, cellnum,
 *     PULL_CUBEROOT, STORE_PERM, FLOOR, WALL);
 */
int cavern(XMap& map, const int xorig, const int yorig,
           const DigPerm& digperm, const int cellnum,
           const XDelveBuilder::Pull pullflag,
           const XDelveBuilder::Store storeflag,
           const XTileType::Id flo, const XTileType::Id ava)
{
    int count = 0;

    assert(cellnum >= 0);
    assert(flo != ava);

    const int seed = findseed(digperm, cellnum - count);

    if (seed < 0) { /* can't start delving. */
        return count;
    }

    CellStore cstore;

    count += digcell(map, cstore, xorig, yorig, storeflag, flo, ava);

    for (int i = 0; i < 8; i++) {
        const int x = xorig + Xoff[i];
        const int y = yorig + Yoff[i];

        if ((count < cellnum) && ((seed >> i) & 0x1)) {
            count += digcell(map, cstore, x, y, storeflag, flo, ava);
        }
    }

    if (count < cellnum) {
        count += delveon(map, cstore, digperm, cellnum - count,
                         pullflag, storeflag, flo, ava);
    }

    return count;
}

/* Set the digperm entry at idx to prob, and fix the symmetry by
 * copying the value to entries at idx rotated by 2, 4, 6 bits - this
 * corresponds to pattern rotation by 90, 180, 270 degrees - to prevent
 * direction bias.
 */
void setsymmetr(DigPerm& digperm, const int idx, const int prob)
{
    assert((idx >= 0) && (idx < 256));
    assert((prob >= 0) && (prob <= 1000));

    digperm[idx] = prob;

    for (int i = 2; i <= 6; i += 2) {
        digperm[ROTL_8(idx, i)] = prob;
    }
}

/* Fill the digperm array with permil probabilities. If desc is non-null,
 * the entries in format idx:prob are scanned from the desc string,
 * otherwise they are generated depending on ngb_min, ngb_max and conmil
 * in the following way: If the number of 1s (floor neighbours) in the
 * index is less than ngb_min or more than ngb_max, the entry stays 0.
 * Otherwise, if digging the cell c wouldn't connect previously
 * unconnected groups of floor cells, the entry is set to 1000. Otherwise,
 * the entry is set to conmil.
 */
void filldigperm(DigPerm& digperm, const int ngb_min, const int ngb_max,
                 const int conmil, const std::string& desc)
{
    digperm.fill(0);

    if (!desc.empty()) { /* scan the description, ignore other params */
        for (int i = 0; i < static_cast<int>(desc.size()); i++) {
            int idx, prob;

            if (isdigit(static_cast<unsigned char>(desc[i]))
                && ((0 == i) || isspace(static_cast<unsigned char>(desc[i - 1])))
                && (2 == std::sscanf(desc.c_str() + i, "%d:%d", &idx, &prob))
                && (idx > 0) && (idx < 256) && (prob >= 0) && (prob <= 1000)) {
                setsymmetr(digperm, idx, prob);
            }
        }
    } else { /* generate from params */
        assert((ngb_min >= 0) && (ngb_min <= 3)
               && (ngb_min <= ngb_max) && (ngb_max <= 8));
        assert((conmil >= 0) && (conmil <= 1000));

        for (int i = 0; i < 256; i++) {
            if ((hamming[i] >= ngb_min) && (hamming[i] <= ngb_max)) {
                digperm[i] = (1 == ngb_grouptab[i]) ? 1000 : conmil;
            }
        }
    }
}


/* A crude way of generating a random digperm table. Will often give
 * interesting results, but certainly doesn't explore a significant part
 * of the possibilities.
 */
void randdigperm(DigPerm& digperm)
{
    /* These are indices of patterns that are the main ways of pattern
     * expansion. If the probabilities at all these entries are too
     * low, the pattern will very likely die young. */
    const int expand[] = {1, 2, 3, 6, 14};

    digperm.fill(0);

    /* Make sure the pattern has at least some possibility of expanding */
    // Kept as written. It passes the *subscript* 0..4 rather than
    // expand[0..4], so it opens one of the entries 0..4 instead of one of
    // {1, 2, 3, 6, 14}; entry 0 is "no floor neighbours at all", which
    // gives the seed somewhere to start from anywhere on the map. The
    // author's notice is explicit about bugs, and the shape of the random
    // tables is a matter of taste rather than correctness, so this is left
    // alone - Build()'s retry loop is what guarantees a usable level.
    setsymmetr(digperm, rnd_i0(sizeof(expand) / sizeof(expand[0])), 1000);

    /* usually a few, sometimes a few dozen entries with 1000 permil prob */
    // Also kept as written: '+' binds tighter than '?:', so this reads
    // (rnd_i0(5) + (0 == rnd_i0(4))) ? rnd_i0(30) : 4 and almost always
    // takes the rnd_i0(30) arm - where the comment above says it should
    // usually be a few and only sometimes a few dozen.
    const int c1000 = rnd_i0(5) + (0 == rnd_i0(4)) ? rnd_i0(30) : 4;

    for (int i = 0; i < c1000; i++) {
        setsymmetr(digperm, rnd_i0(256), 1000);
    }

    /* usually a few, sometimes a few dozen entries with 200 permil prob */
    const int c200 = rnd_i0(5) + (0 == rnd_i0(4)) ? rnd_i0(30) : 4;

    for (int i = 0; i < c200; i++) {
        setsymmetr(digperm, rnd_i0(256), 200);
    }
}

} // namespace




void XDelveBuilder::Build()
{
    XMap*& map = location->map;

    int cl = w;
    int ch = h;

    if (map) {
        cl = map->len;
        ch = map->hgt;
    } else {
        map = new XMap(cl, ch);
    }

    // The original's default: one fifth of the map turned to floor.
    const int want = (cells > 0) ? cells : (cl * ch / 5);

    // How much floor makes a level worth walking into. Below this a
    // stairway placed afterwards by GetFreeXY() would have almost nowhere
    // to go, so it is worth another attempt even if the digging "finished".
    const int enough = cl * ch / 20;

    DigPerm digperm{};
    int dug = 0;

    // The original retried only when it was inventing a table, and gave up
    // on the first result of a named one. Here every kind is retried: a
    // table is a set of probabilities, not a fixed outcome, so the same one
    // can die young once and open out the next time.
    constexpr int MAX_ATTEMPTS = 100;

    for (int attempt = 0; attempt < MAX_ATTEMPTS; attempt++) {
        if (!desc.empty()) {
            filldigperm(digperm, 0, 0, 0, desc);
        } else if (ngb_max > 0) {
            filldigperm(digperm, ngb_min, ngb_max, conmil, "");
        } else {
            randdigperm(digperm);
        }

        for (int i = 0; i < map->hgt; i++) {
            for (int j = 0; j < map->len; j++) {
                map->SetXY(j, i, wall);
            }
        }

        dug = cavern(*map, cl / 2, ch / 2, digperm, want, pull, store, floor, wall);

        // The original's own yardstick for a good enough result, plus the
        // floor above.
        if ((dug >= want || dug >= cl * ch / 3) && dug >= enough) {
            break;
        }
    }

    // A table can be so restrictive that no seed of it ever opens out - a
    // randomly invented one especially. Rather than hand back a level that
    // is very nearly solid rock, dig once more with a table built from the
    // generator's own parameter mode, permissive enough that it cannot
    // fail: any cell with at least one floor neighbour may be dug.
    if (dug < enough) {
        filldigperm(digperm, 1, 8, 1000, "");

        for (int i = 0; i < map->hgt; i++) {
            for (int j = 0; j < map->len; j++) {
                map->SetXY(j, i, wall);
            }
        }

        cavern(*map, cl / 2, ch / 2, digperm, want, pull, store, floor, wall);
    }

    // Deliberately no isFullyConnected()/ConnectAllRegions() pass here,
    // unlike the other builders.
    //
    // Everything delved grows outward from the one seed - a cell only
    // enters the store when a neighbour of it has just been dug - so the
    // floor is a single region already, and the game walks it: movement
    // is eight-way (see vCheckForCursorKey, and XCreature::NewMove, which
    // asks only whether the destination is passable).
    //
    // The engine's isFullyConnected() disagrees, because FloodFillWalkable
    // spreads in four directions only. Run over a diagonal maze it reports
    // a level in pieces, and ConnectAllRegions() then tunnels between
    // them until the maze is an open cavern - dmaze1 went from a warren of
    // one-cell passages to 871 cells of undifferentiated floor. Half of
    // Kusigrosz's tables are diagonal by design, so the check cannot be
    // applied to what this generator makes.
}
