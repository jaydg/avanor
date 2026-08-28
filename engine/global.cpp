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

#include <fmt/format.h>

#include <algorithm>
#include <cmath>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>
#include <utility>

#include <sol/sol.hpp>

#include <ncpp/NotCurses.hh>
#include <ncpp/Plane.hh>

#include "engine/global.h"

void RegisterColorEnum(sol::state_view& lua)
{
    lua.new_enum("xColor",
        "xBLACK", xBLACK,
        "xBLUE", xBLUE,
        "xGREEN", xGREEN,
        "xCYAN", xCYAN,
        "xRED", xRED,
        "xMAGENTA", xMAGENTA,
        "xBROWN", xBROWN,
        "xLIGHTGRAY", xLIGHTGRAY,
        "xDARKGRAY", xDARKGRAY,
        "xLIGHTBLUE", xLIGHTBLUE,
        "xLIGHTGREEN", xLIGHTGREEN,
        "xLIGHTCYAN", xLIGHTCYAN,
        "xLIGHTRED", xLIGHTRED,
        "xLIGHTMAGENTA", xLIGHTMAGENTA,
        "xYELLOW", xYELLOW,
        "xWHITE", xWHITE
    );
}

unsigned long total_cr = 0;
unsigned long total_it = 0;
unsigned long cr_kiled = 0;
unsigned long cr_died = 0;

std::string SCOLOR(const unsigned rgb)
{
    return fmt::format("<#{:06x}>", rgb & 0xFFFFFF);
}

int __animation_flag = 100;

int size_x = 80;
int size_y = 25;

int cursor_pos_x = 0;
int cursor_pos_y = 0;
unsigned current_attr = xLIGHTGRAY;

// Out of sight, and drawn that way - see SetRememberedBrightness().
int remembered_brightness = 100;

// How much ground colour varies from cell to cell - see SetTileJitter().
int tile_jitter = 10;
int tile_hue_jitter = 12;
int tile_saturation_jitter = 15;

#ifdef XLINUX
// The terminal. Owned here, created by vInit() and stopped by vFinit().
ncpp::NotCurses* nc = nullptr;
ncpp::Plane* screen = nullptr;

// vKbhit() has to take a key out of notcurses to know one is there, so
// it keeps it for the vGetch() that follows.
ncinput pending_key{};
bool has_pending_key = false;
#endif

#ifdef XWIN32
    #include <windows.h>
    HANDLE hStdout;
    HANDLE hStdin;
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    CONSOLE_CURSOR_INFO ccInfo;
    CHAR_INFO* vscreenw;
#endif

void vInit()
{
#ifdef XLINUX
    std::filesystem::create_directory(vMakePath(HOME_DIR, ""));
#endif

#ifdef XWIN32
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    GetConsoleCursorInfo(hStdout, &ccInfo);

    GetConsoleScreenBufferInfo(hStdout, &csbiInfo);
    size_x = csbiInfo.srWindow.Right - csbiInfo.srWindow.Left + 1;
    size_y = csbiInfo.srWindow.Bottom - csbiInfo.srWindow.Top + 1;

    if (size_x != csbiInfo.dwSize.X || size_y != csbiInfo.dwSize.Y) {
        COORD coord = { (short)size_x, (short)size_y };

        if (!SetConsoleScreenBufferSize(hStdout, coord)) {
            assert(false);
        }
    }

    vscreenw = new CHAR_INFO[size_x * size_y];

#endif //XWIN32

#ifdef XLINUX
    notcurses_options opts{};
    opts.loglevel = NCLOGLEVEL_SILENT;
    opts.flags = NCOPTION_SUPPRESS_BANNERS;

    nc = new ncpp::NotCurses(opts);
    screen = nc->get_stdplane();

    unsigned rows = 0;
    unsigned cols = 0;
    screen->get_dim(&rows, &cols);

    size_x = static_cast<int>(cols);
    size_y = static_cast<int>(rows);

    // Every cell of the screen paints, including the blank ones: planes
    // parked underneath it (see vStore()) must never show through.
    uint64_t opaque_black = 0;
    ncchannels_set_fg_rgb(&opaque_black, xLIGHTGRAY);
    ncchannels_set_bg_rgb(&opaque_black, xBLACK);
    screen->set_base(" ", 0, opaque_black);

    nc->cursor_disable();
#endif //XLINUX

    vClrScr();
}

void vUpdateScreenSize()
{
#ifdef XLINUX
    unsigned rows = 0;
    unsigned cols = 0;

    // refresh() repaints the terminal from the planes as they now stand
    // and reports the size it found; get_dim() is asked afterwards
    // because the standard plane is the one the game draws into and it
    // is that which has to agree with size_x/size_y.
    nc->refresh(&rows, &cols);
    screen->get_dim(&rows, &cols);

    size_x = static_cast<int>(cols);
    size_y = static_cast<int>(rows);
#endif
}

void vClrScr()
{
#ifdef XWIN32
    CHAR_INFO blank_char = { ' ', 7 };
    int screenbuf_size = size_x * size_y;

    for (int i = 0; i < screenbuf_size; i++) {
        vscreenw[i] = blank_char;
    }
#else
    screen->erase();
    screen->set_fg_rgb(xLIGHTGRAY);
    screen->set_bg_rgb(xBLACK);
#endif
}

void vFinit()
{
#ifdef XWIN32
    SetConsoleScreenBufferSize(hStdout, csbiInfo.dwSize);
    SetConsoleWindowInfo(hStdout, true, &csbiInfo.srWindow);
    SetConsoleCursorInfo(hStdout, &ccInfo);
    delete[] vscreenw;
#endif
#ifdef XLINUX
    if (nc) {
        nc->stop();
        delete nc;
        nc = nullptr;
        screen = nullptr;
    }
#endif
}

void vRefresh()
{
#ifdef XWIN32
    COORD buffer_size = { (short)size_x, (short)size_y };
    COORD buffer_coord = { 0, 0 };
    SMALL_RECT write_region = { 0, 0, size_x - 1, size_y - 1 };

    WriteConsoleOutput(hStdout, vscreenw, buffer_size, buffer_coord, &write_region);
#endif

#ifdef XLINUX
    nc->render();
#endif
}

void vPutCh(const int x, const int y, char ch, unsigned rgb)
{
    assert(x >= 0 && y >= 0 && x <= size_x && y <= size_y);
#ifdef XWIN32
    CHAR_INFO tmp = { ch, attr };
    vscreenw[x + y * size_x] = tmp;
#else
    // The plane holds what the screen will look like; notcurses works
    // out what actually has to be sent when vRefresh() renders it.
    screen->set_fg_rgb(rgb);
    screen->putc(y, x, ch < ' ' ? ' ' : ch);
#endif
};

char vTestCh(const int x, const int y)
{
#ifdef XWIN32
    return vscreenw[x + y * size_x].Char.AsciiChar;
#else
    uint16_t stylemask = 0;
    uint64_t channels = 0;
    char ch = ' ';

    if (char* egc = screen->get_at(y, x, &stylemask, &channels)) {
        ch = egc[0];
        free(egc);
    }

    return ch;
#endif
}

void vPutCh(int x, int y, char ch)
{
#ifdef XWIN32
    vscreenw[x + y * size_x].Char.AsciiChar = ch;
#else
    // No colour given: the cell keeps the one it already has.
    uint16_t stylemask = 0;
    uint64_t channels = 0;

    if (char* egc = screen->get_at(y, x, &stylemask, &channels)) {
        free(egc);
        screen->set_channels(channels);
    }

    screen->putc(y, x, ch < ' ' ? ' ' : ch);
#endif
};

void vGotoXY(int x, int y)
{
    cursor_pos_x = x;
    cursor_pos_y = y;
}

void vGetCursorPos(int* x, int* y)
{
    *x = cursor_pos_x;
    *y = cursor_pos_y;
}

int vGetS(char* s, const int buffer_size)
{
    int cx;
    int cy;
    vGetCursorPos(&cx, &cy);
    int buffer_pos = 0;
    strcpy(s, "");

    do {
        vXGotoXY(cx + strlen(s), cy);
        const int ch = vGetch();

        s[buffer_pos] = ch;

        if (ch == 13 || ch == 27) {
            s[buffer_pos] = 0;

            if (ch == 13) {
                return 1;
            } else {
                return 0;
            }
        }

        // Terminals vary in which code their Backspace key actually sends
        // (ASCII BS or DEL) - accept both rather than just one, otherwise
        // the code the terminal doesn't happen to use gets inserted into
        // the string as a literal control character instead of deleting.
        if ((ch == KEY_BACKSPACE || ch == KEY_DEL) && buffer_pos > 0) {
            s[buffer_pos - 1] = ' ';
            s[buffer_pos] = 0;
            vGotoXY(cx, cy);
            vPutS(s);
            buffer_pos--;
        } else if (ch != KEY_BACKSPACE && ch != KEY_DEL) {
            buffer_pos++;
        }

        s[buffer_pos] = 0;
        vGotoXY(cx, cy);
        vPutS(s);
        vRefresh();
    } while (buffer_pos < buffer_size - 1);

    return 1;
}

void vDelay(const int n)
{
    // One animation step: the caller has already drawn the frame and called
    // vRefresh(), and this is how long it stays on screen before the next one
    // replaces it. n is milliseconds, as it was for the DOS build's delay()
    // - the only platform where this was ever implemented; the Windows branch
    // was empty from the 2003 import onwards, and Linux inherited that
    // emptiness. std::this_thread needs no #ifdef and behaves the same on every
    // platform this builds for.
    if (n <= 0) {
        return;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(n));
}

int vKbhit()
{
#ifdef XLINUX
    ncinput ni{};
    const struct timespec now = {0, 0};

    if (nc->get(&now, &ni) == 0) {
        return 0;
    }

    // Something was typed; hand it back on the next vGetch().
    pending_key = ni;
    has_pending_key = true;

    return 1;

#else
    return kbhit();
#endif
}

int vGetch()
{
#ifdef XLINUX
    // notcurses decodes the terminal's escape sequences itself, so what
    // arrives here is either a Unicode codepoint or one of its NCKEY_*
    // synthetic keys.
    ncinput ni{};

    while (true) {
        uint32_t key;

        if (has_pending_key) {
            ni = pending_key;
            key = ni.id;
            has_pending_key = false;
        } else {
            key = nc->get(true, &ni);
        }


        if (key == static_cast<uint32_t>(-1) || ni.evtype == NCTYPE_RELEASE) {
            continue;
        }

        // SIGWINCH arrives on the input queue like anything else. notcurses
        // has already resized its own planes by the time we see it; what is
        // left is to tell the game how much room it has and to get the old
        // picture back on the screen, which stops the display being left in
        // pieces if whoever is waiting here does not know how to lay itself
        // out again.
        if (key == NCKEY_RESIZE) {
            vUpdateScreenSize();

            return KEY_RESIZE;
        }

        switch (key) {
            case NCKEY_UP:        return KEY_UP;
            case NCKEY_DOWN:      return KEY_DOWN;
            case NCKEY_LEFT:      return KEY_LEFT;
            case NCKEY_RIGHT:     return KEY_RIGHT;
            case NCKEY_HOME:      return KEY_HOME;
            case NCKEY_END:       return KEY_END;
            case NCKEY_PGUP:      return KEY_PGUP;
            case NCKEY_PGDOWN:    return KEY_PGDOWN;
            case NCKEY_CENTER:    return KEY_CENTER;
            case NCKEY_ENTER:     return KEY_ENTER;
            case NCKEY_BACKSPACE: return KEY_BACKSPACE;
            case NCKEY_DEL:       return KEY_DEL;
            case NCKEY_ESC:       return KEY_ESC;
            default:
                break;
        }

        const bool ctrl_held = ncinput_ctrl_p(&ni);

        if (ctrl_held && key >= 'a' && key <= 'z') {
            return static_cast<int>(key - 'a' + 1);
        }

        if (ctrl_held && key >= 'A' && key <= 'Z') {
            return static_cast<int>(key - 'A' + 1);
        }

        if (key < 0x80) {
            return static_cast<int>(key);
        }
    }

#else
    int ch = getch();

    if (ch == 0 || ch == 224) {
        ch = KEY_EXTENDED_CODE | getch();
    }

    return ch;
#endif
}

int vXGetch(const char* ch_buf)
{
    size_t slen = strlen(ch_buf);

    while (true) {
        const int ch = vGetch();

        for (size_t i = 0; i < slen; i++) {
            if (ch == ch_buf[i]) {
                return ch;
            }
        }

        if (ch == KEY_ESC) {
            return 0;
        }
    }
}

void vXGotoXY(int x, int y)
{
#ifdef XLINUX
    nc->cursor_enable(y, x);
#endif
}

void vHideCursor()
{
#ifdef XWIN32
    //	CONSOLE_CURSOR_INFO ConsoleCursorInfo = ccInfo;
    //	ConsoleCursorInfo.bVisible = FALSE;
    //	SetConsoleCursorInfo(hStdout, &ConsoleCursorInfo);
#endif

#ifdef XLINUX
    nc->cursor_disable();
#endif
}

namespace {

// The scheme in force. Avanor's own, which reproduces exactly the colours
// the screens were written with: labels and framing in brown, values in
// yellow, keys in cyan, everything else light gray.
const ColourScheme avanor_scheme = {
    "avanor",
    {
        xLIGHTGRAY,  // ROLE_TEXT
        xBROWN,      // ROLE_DECORATION
        xBROWN,      // ROLE_LABEL
        xYELLOW,     // ROLE_VALUE
        xYELLOW,     // ROLE_EMPHASIS
        xCYAN,       // ROLE_KEY
        xYELLOW,     // ROLE_SELECTOR
        xYELLOW,     // ROLE_WARNING

        xLIGHTGRAY,  // ROLE_PROGRESS_NONE
        xLIGHTGREEN, // ROLE_PROGRESS_BASIC
        xGREEN,      // ROLE_PROGRESS_SKILLED
        xYELLOW,     // ROLE_PROGRESS_EXPERT
        xLIGHTRED,   // ROLE_PROGRESS_MASTER
        xRED,        // ROLE_PROGRESS_SENIOR_MASTER
        xDARKGRAY,   // ROLE_PROGRESS_GRANDMASTER

        xBLACK,      // 0x1F is the escape byte - never resolved, never used

        xRED,        // ROLE_QUALITY_TERRIBLE
        xLIGHTRED,   // ROLE_QUALITY_POOR
        xLIGHTGRAY,  // ROLE_QUALITY_NEUTRAL
        xLIGHTGREEN, // ROLE_QUALITY_FAIR
        xYELLOW,     // ROLE_QUALITY_GOOD
        xWHITE,      // ROLE_QUALITY_PERFECT

        xLIGHTGRAY,  // ROLE_SEVERITY_MILD
        xYELLOW,     // ROLE_SEVERITY_NOTABLE
        xRED,        // ROLE_SEVERITY_SEVERE
        xDARKGRAY    // ROLE_SEVERITY_CRITICAL
    }
};

const ColourScheme* current_scheme = &avanor_scheme;

// Escape bytes 0-15 are the old sixteen colours, in their old order.
const unsigned legacy_palette[16] = {
    xBLACK, xBLUE, xGREEN, xCYAN, xRED, xMAGENTA, xBROWN, xLIGHTGRAY,
    xDARKGRAY, xLIGHTBLUE, xLIGHTGREEN, xLIGHTCYAN, xLIGHTRED, xLIGHTMAGENTA, xYELLOW, xWHITE
};

// What a role is called when it is written into a string. The order does
// not matter here; the names do.
const struct {
    const char* name;
    TextRole role;
} role_names[] = {
    {"TEXT", ROLE_TEXT},
    {"DECORATION", ROLE_DECORATION},
    {"LABEL", ROLE_LABEL},
    {"VALUE", ROLE_VALUE},
    {"EMPHASIS", ROLE_EMPHASIS},
    {"KEY", ROLE_KEY},
    {"SELECTOR", ROLE_SELECTOR},
    {"WARNING", ROLE_WARNING},

    {"PROGRESS_NONE", ROLE_PROGRESS_NONE},
    {"PROGRESS_BASIC", ROLE_PROGRESS_BASIC},
    {"PROGRESS_SKILLED", ROLE_PROGRESS_SKILLED},
    {"PROGRESS_EXPERT", ROLE_PROGRESS_EXPERT},
    {"PROGRESS_MASTER", ROLE_PROGRESS_MASTER},
    {"PROGRESS_SENIOR_MASTER", ROLE_PROGRESS_SENIOR_MASTER},
    {"PROGRESS_GRANDMASTER", ROLE_PROGRESS_GRANDMASTER},

    {"QUALITY_TERRIBLE", ROLE_QUALITY_TERRIBLE},
    {"QUALITY_POOR", ROLE_QUALITY_POOR},
    {"QUALITY_NEUTRAL", ROLE_QUALITY_NEUTRAL},
    {"QUALITY_FAIR", ROLE_QUALITY_FAIR},
    {"QUALITY_GOOD", ROLE_QUALITY_GOOD},
    {"QUALITY_PERFECT", ROLE_QUALITY_PERFECT},

    {"SEVERITY_MILD", ROLE_SEVERITY_MILD},
    {"SEVERITY_NOTABLE", ROLE_SEVERITY_NOTABLE},
    {"SEVERITY_SEVERE", ROLE_SEVERITY_SEVERE},
    {"SEVERITY_CRITICAL", ROLE_SEVERITY_CRITICAL}
};

// The macros in global.h spell these bytes out, because they must stay
// string literals for the compiler to fold them into their text.
static_assert(ROLE_TEXT == 0x10, "MSG_TEXT's escape byte no longer matches");
static_assert(ROLE_DECORATION == 0x11, "MSG_DECORATION's escape byte no longer matches");
static_assert(ROLE_LABEL == 0x12, "MSG_LABEL's escape byte no longer matches");
static_assert(ROLE_VALUE == 0x13, "MSG_VALUE's escape byte no longer matches");
static_assert(ROLE_EMPHASIS == 0x14, "MSG_EMPHASIS's escape byte no longer matches");
static_assert(ROLE_KEY == 0x15, "MSG_KEY's escape byte no longer matches");
static_assert(ROLE_SELECTOR == 0x16, "MSG_SELECTOR's escape byte no longer matches");
static_assert(ROLE_WARNING == 0x17, "MSG_WARNING's escape byte no longer matches");
static_assert(ROLE_PROGRESS_NONE == 0x18, "MSG_PROGRESS_NONE's escape byte no longer matches");
static_assert(ROLE_PROGRESS_GRANDMASTER == 0x1E, "MSG_PROGRESS_GRANDMASTER's escape byte no longer matches");
static_assert(ROLE_QUALITY_TERRIBLE == 0x20, "MSG_QUALITY_TERRIBLE's escape byte no longer matches");
static_assert(ROLE_QUALITY_PERFECT == 0x25, "MSG_QUALITY_PERFECT's escape byte no longer matches");
static_assert(ROLE_SEVERITY_MILD == 0x26, "MSG_SEVERITY_MILD's escape byte no longer matches");
static_assert(ROLE_SEVERITY_CRITICAL == 0x29, "MSG_SEVERITY_CRITICAL's escape byte no longer matches");

// No role may collide with the byte that introduces one.
static_assert(ROLE_COUNT > 0x1F && ROLE_QUALITY_TERRIBLE > 0x1F, "a role would be indistinguishable from the escape");

} // namespace

void SetColourScheme(const ColourScheme& scheme)
{
    current_scheme = &scheme;
}

std::string ExpandMarkup(const std::string_view text)
{
    std::string out;
    out.reserve(text.size());

    for (size_t pos = 0; pos < text.size(); pos++) {
        if (text[pos] != '<') {
            out += text[pos];
            continue;
        }

        // "<<" is a '<' that means itself.
        if (pos + 1 < text.size() && text[pos + 1] == '<') {
            out += '<';
            pos++;
            continue;
        }

        const size_t close = text.find('>', pos + 1);

        if (close == std::string_view::npos) {
            out += text[pos];
            continue;
        }

        const std::string_view name = text.substr(pos + 1, close - pos - 1);

        // <#rrggbb>: a colour given outright.
        if (name.size() == 7 && name[0] == '#'
            && name.find_first_not_of("0123456789abcdefABCDEF", 1) == std::string_view::npos) {
            out += RGB_ESCAPE;
            out += name.substr(1);
            pos = close;
            continue;
        }

        bool known = false;

        for (const auto& [role_name, role] : role_names) {
            if (name == role_name) {
                out += '\x1F';
                out += static_cast<char>(role);
                pos = close;
                known = true;
                break;
            }
        }

        // Not one of ours - a creature's name, a bit of punctuation -
        // so it stays as written.
        if (!known) {
            out += text[pos];
        }
    }

    return out;
}

unsigned DimRGB(const unsigned rgb, const int percent)
{
    const unsigned r = ((rgb >> 16) & 0xFF) * percent / 100;
    const unsigned g = ((rgb >> 8) & 0xFF) * percent / 100;
    const unsigned b = (rgb & 0xFF) * percent / 100;

    return (r << 16) | (g << 8) | b;
}

void SetRememberedBrightness(const int percent)
{
    remembered_brightness = std::clamp(percent, 0, 100);
}

namespace {

// A number in [0, 1) fixed by where the cell is and which of the three
// qualities is asking. Not vRand(): the same cell has to come back with
// the same answer every time it is drawn, or the ground would crawl as
// the hero walks over it.
float JitterAt(const int x, const int y, const unsigned quality)
{
    unsigned h = static_cast<unsigned>(x) * 374761393u
               + static_cast<unsigned>(y) * 668265263u
               + quality * 2246822519u;

    h = (h ^ (h >> 13)) * 1274126177u;
    h ^= h >> 16;

    return static_cast<float>(h & 0xFFFFFFu) / static_cast<float>(0x1000000u);
}

// A number in [-1, 1), for the qualities that move either way.
float SignedJitterAt(const int x, const int y, const unsigned quality)
{
    return JitterAt(x, y, quality) * 2.0F - 1.0F;
}

struct HSV {
    float h;    // degrees, [0, 360)
    float s;    // [0, 1]
    float v;    // [0, 1]
};

HSV ToHSV(const unsigned r, const unsigned g, const unsigned b)
{
    const float rf = static_cast<float>(r) / 255.0F;
    const float gf = static_cast<float>(g) / 255.0F;
    const float bf = static_cast<float>(b) / 255.0F;

    const float top = std::max({rf, gf, bf});
    const float bottom = std::min({rf, gf, bf});
    const float span = top - bottom;

    float hue = 0.0F;

    if (span > 0.0F) {
        if (top == rf) {
            hue = 60.0F * (gf - bf) / span;
        } else if (top == gf) {
            hue = 60.0F * (2.0F + (bf - rf) / span);
        } else {
            hue = 60.0F * (4.0F + (rf - gf) / span);
        }
    }

    if (hue < 0.0F) {
        hue += 360.0F;
    }

    return {hue, top > 0.0F ? span / top : 0.0F, top};
}

unsigned FromHSV(const HSV& c)
{
    const float sector = c.h / 60.0F;
    const int part = static_cast<int>(sector) % 6;
    const float frac = sector - static_cast<float>(static_cast<int>(sector));

    const float p = c.v * (1.0F - c.s);
    const float q = c.v * (1.0F - c.s * frac);
    const float t = c.v * (1.0F - c.s * (1.0F - frac));

    float rf = 0.0F;
    float gf = 0.0F;
    float bf = 0.0F;

    switch (part) {
        case 0:  rf = c.v; gf = t;   bf = p;   break;
        case 1:  rf = q;   gf = c.v; bf = p;   break;
        case 2:  rf = p;   gf = c.v; bf = t;   break;
        case 3:  rf = p;   gf = q;   bf = c.v; break;
        case 4:  rf = t;   gf = p;   bf = c.v; break;
        default: rf = c.v; gf = p;   bf = q;   break;
    }

    const auto byte = [](const float f) {
        return static_cast<unsigned>(std::lround(std::clamp(f, 0.0F, 1.0F) * 255.0F));
    };

    return (byte(rf) << 16) | (byte(gf) << 8) | byte(bf);
}

} // namespace

unsigned JitterRGB(const unsigned rgb, const int x, const int y)
{
    if (TileJitter() <= 0 && TileHueJitter() <= 0 && TileSaturationJitter() <= 0) {
        return rgb;
    }

    HSV c = ToHSV((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF);

    if (c.v <= 0.0F) {
        return rgb;     // black: nothing to vary
    }

    // Hue and saturation are what make a stand of trees read as a wood
    // rather than as one colour stamped out a hundred times: some of them
    // a little yellower, some greyer, some deeper. The hue turns by a few
    // degrees at most, so a green stays a green and a blue stays a blue.
    // Scaling the channels apart instead would not turn a colour, it
    // would replace it - water would wander off through lavender - and it
    // would do so unevenly, since a colour with one channel to scale can
    // only change brightness however far the factors are pushed.
    //
    // Saturation is scaled rather than offset, which is what keeps the
    // greys grey without a special case: a grey has no saturation, and
    // nothing times a factor is still nothing.
    c.h += static_cast<float>(TileHueJitter()) * SignedJitterAt(x, y, 1);
    c.h = std::fmod(c.h + 360.0F, 360.0F);

    c.s = std::clamp(c.s * (1.0F + static_cast<float>(TileSaturationJitter()) / 100.0F
                                       * SignedJitterAt(x, y, 2)),
                     0.0F, 1.0F);

    // Brightness keeps the shape it had: a factor either side of one, but
    // never far enough for the brightest channel to reach 255 and clamp,
    // since a channel that stops rising while the others carry on is hue
    // drift by another route. A colour already at full value can only
    // darken.
    const float low = 1.0F - static_cast<float>(TileJitter()) / 100.0F;
    const float high = std::min(low > 0.0F ? 1.0F / low : 1.0F, 1.0F / c.v);

    c.v = std::clamp(c.v * (low + (std::max(high, low) - low) * JitterAt(x, y, 0)), 0.0F, 1.0F);

    return FromHSV(c);
}

void SetTileJitter(const int percent, const int hue_degrees, const int saturation_percent)
{
    tile_jitter = std::clamp(percent, 0, 100);
    tile_hue_jitter = std::clamp(hue_degrees, 0, 180);
    tile_saturation_jitter = std::clamp(saturation_percent, 0, 100);
}

int TileJitter()
{
    return tile_jitter;
}

int TileHueJitter()
{
    return tile_hue_jitter;
}

int TileSaturationJitter()
{
    return tile_saturation_jitter;
}

int RememberedBrightness()
{
    return remembered_brightness;
}

unsigned PaletteRGB(const unsigned char slot)
{
    return legacy_palette[slot & 0x0F];
}

unsigned ResolveColour(const unsigned char escape_byte)
{
    if (escape_byte < ROLE_FIRST) {
        return PaletteRGB(escape_byte);
    }

    if (escape_byte >= ROLE_COUNT) {
        return xLIGHTGRAY;
    }

    return current_scheme->role[escape_byte - ROLE_FIRST];
}

void vSetAttr(const unsigned rgb)
{
    current_attr = rgb;
}

void vClrEol()
{
    while (cursor_pos_x < size_x) {
        vPutCh(cursor_pos_x, cursor_pos_y, ' ');
        cursor_pos_x++;
    }
}

// (31, x) - change attr
void vPutS(const char* s)
{
    // Roles may be written into any string the game prints; they become
    // escape bytes here, on the way to the screen.
    const std::string expanded = ExpandMarkup(s ? s : "");

    s = expanded.c_str();

    if (cursor_pos_y >= size_y) {
        cursor_pos_x = cursor_pos_y = 0;
    }

    while (true) {
        switch (const char ch = *s++) {
            case 0 :
                return;

            case 31 :
                vSetAttr(ResolveColour(static_cast<unsigned char>(*s++)));
                break;

            case RGB_ESCAPE : {
                // Six hex digits follow, put there by ExpandMarkup(), so
                // they are known to be six hex digits.
                unsigned rgb = 0;

                for (int i = 0; i < 6 && *s; i++) {
                    const char digit = *s++;

                    rgb = rgb * 16 + static_cast<unsigned>(digit <= '9' ? digit - '0'
                                                                       : (digit | 0x20) - 'a' + 10);
                }

                vSetAttr(rgb);
                break;
            }

            case 13 :
            case '\n' :
                cursor_pos_x = 0;

                if (++cursor_pos_y >= size_y) {
                    cursor_pos_y = 0;
                }

                break;

            default : {
                // Past the right edge the character is dropped, not
                // wrapped onto the next line. Several screens are laid
                // out at fixed columns for a terminal at least eighty
                // wide - the status line is - and on a narrower one
                // wrapping pushed everything below them down and took
                // the bottom of the screen round to the top. A line too
                // long for the terminal is now simply cut short.
                //
                // Line breaks written into the string still break the
                // line: those are the cases above.
                // Off the top or the left as well as off the right: on a
                // terminal only a couple of rows tall, a screen that puts
                // its status line three rows up from the bottom is
                // addressing a negative row.
                if (cursor_pos_x >= 0 && cursor_pos_x < size_x
                    && cursor_pos_y >= 0 && cursor_pos_y < size_y) {
                    vPutCh(cursor_pos_x, cursor_pos_y, ch, current_attr);
                }

                cursor_pos_x++;
            }
        }
    }
}

void vFPutS(std::ofstream &file, std::string_view s)
{
    for (std::size_t pos = 0; pos < s.size(); ++pos) {
        if (s[pos] == '\x1F') {
            // Escape sequence: skip byte 31 and 1 following byte
            ++pos;
            continue;
        }

        if (s[pos] == RGB_ESCAPE) {
            pos += RGB_ESCAPE_LENGTH - 1;
            continue;
        }
        file << s[pos];
    }
}

static unsigned long randx;

void vRandSeed(const unsigned long seed)
{
    randx = seed;
}

#define X_RAND_A   0x000343FDUL
#define X_RAND_B   0x00269EC3UL
#define X_RAND_MAX 0x7FFFFFFFUL

long vRand()
{
    randx = randx * X_RAND_A + X_RAND_B;
    unsigned long randhigh = randx & 0x7FFF0000;
    randx = randx * X_RAND_A + X_RAND_B;
    unsigned long randlow = randx >> 16;
    return randhigh | randlow;
}

long vRand(unsigned long n)
{
    assert(n > 0);

    if (n > 0x8000) {
        assert(n <= X_RAND_MAX + 1);
        randx = randx * X_RAND_A + X_RAND_B;
        unsigned long randhigh = randx & 0x7FFF0000;
        randx = randx * X_RAND_A + X_RAND_B;
        unsigned long randlow = randx >> 16;
        return (randhigh | randlow) % n;
    } else {
        randx = randx * X_RAND_A + X_RAND_B;
        return (randx >> 16) % n;
    }
}

V_BUFFER::V_BUFFER()
{
#ifdef XWIN32
    buffer = new char[size_x * size_y * sizeof(CHAR_INFO)];
#endif
}

V_BUFFER::~V_BUFFER()
{
#ifdef XWIN32
    delete[] buffer;
#else
    if (saved) {
        ncplane_destroy(static_cast<ncplane*>(saved));
    }
#endif
}

void vStore(const V_BUFFER* buf)
{
#ifdef XWIN32
    memcpy(buf->buffer, vscreenw, size_x * size_y * sizeof(CHAR_INFO));
#else
    // A saved screen is a copy of the plane; putting it back is a blit
    // from that copy, which is what notcurses' own plane machinery does.
    if (buf->saved) {
        ncplane_destroy(static_cast<ncplane*>(buf->saved));
    }

    ncplane* copy = ncplane_dup(*screen, nullptr);

    // ncplane_dup() stacks the copy immediately above its original, where
    // it would hide everything drawn next; it is only storage, so it goes
    // to the bottom, under an opaque screen.
    ncplane_move_bottom(copy);

    const_cast<V_BUFFER*>(buf)->saved = copy;
#endif
}

void vRestore(const V_BUFFER* buf)
{
#ifdef XWIN32
    memcpy(vscreenw, buf->buffer, size_x * size_y * sizeof(CHAR_INFO));
#else
    if (!buf->saved) {
        return;
    }

    auto* copy = static_cast<ncplane*>(buf->saved);

    for (int y = 0; y < size_y; y++) {
        for (int x = 0; x < size_x; x++) {
            uint16_t stylemask = 0;
            uint64_t channels = 0;

            if (char* egc = ncplane_at_yx(copy, y, x, &stylemask, &channels)) {
                screen->set_channels(channels);
                screen->putc(y, x, egc[0] ? egc[0] : ' ');
                free(egc);
            }
        }
    }
#endif
}

std::string vMakePath(std::string_view prefix, std::string_view filename)
{
    std::string path_buffer;

#ifdef XLINUX
    if (prefix[0] == '~') {
        path_buffer.assign(getenv("HOME"))
            .append(prefix.substr(1))
            .append(filename);
    } else
#endif
    {
        path_buffer.append(prefix)
            .append(filename);
    }

    return path_buffer;
}
