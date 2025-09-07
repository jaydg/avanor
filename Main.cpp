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

#include <ctype.h>
#include <stdlib.h>

#include "engine/global.h"
#include "engine/xdebug.h"
#include "game/game.h"
#include "helpers/hiscore.h"
#include "helpers/manual.h"

const char* logo_text[] = {
    MSG_CYAN
    "  .oooooo  oo      oo    .oooooo  oo     oo    .oooo.    ooooooo. ",
    " dP'   88  88      88   dP'   88  88b    88   dP'  `Yb   88    `Yb",
    "d8     88  88      88  d8     88  88Yb   88  88      88  88     88",
    "88     88  Y8      8Y  88     88  88 Yb  88  88      88  88     88",
    "88ooooo88   Yb    dY   88ooooo88  88  Yb 88  88      88  88oooodP ",
    "88     88    Yb  dY    88     88  88   Yb88  88      88  88   Yb  ",
    "88     88     YbdY     88     88  88    Y88  `8b    d8'  88    Yb ",
    "88     88      YY      88     88  88     88   `Y8bd8P    88     Yb",
    MSG_WHITE,
    "                T h e  L a n d  o f  M y s t e r y",
    "",
    MSG_GREEN
    "                      version " GAME_VERSION "\n",
    MSG_YELLOW,
    "              (c) 2000 - 2006 by Vadim Gaidukevich",
    "",
    MSG_YELLOW
    "                      http://www.avanor.com",
    "",
    "",
    " "
    MSG_LIGHTGRAY "[" MSG_WHITE "R" MSG_LIGHTGRAY "] - Restore game "
    MSG_LIGHTGRAY "[" MSG_WHITE "N" MSG_LIGHTGRAY "] - New game "
    MSG_LIGHTGRAY "[" MSG_WHITE "?" MSG_LIGHTGRAY "] - read Manual "
    MSG_LIGHTGRAY "[" MSG_WHITE "Esc" MSG_LIGHTGRAY "] - Exit",
};

void ShowLogo()
{
    int logo_height = sizeof(logo_text) / sizeof(char*), logo_width = 0, i;

    for (i = 0; i < logo_height; i++)
        if (x_strlen(logo_text[i]) > logo_width) {
            logo_width = x_strlen(logo_text[i]);
        }

    int shift_x = (size_x - logo_width) / 2;
    int shift_y = (size_y - logo_height) / 2;

    vClrScr();

    for (i = 0; i < logo_height; i++) {
        vGotoXY(shift_x, i + shift_y);
        vPutS(logo_text[i]);
    }

    vRefresh();
}

XGame Game;


int main(int argc, char* argv[])
{
#ifdef NOT_SET_RND_SEED
    vRandSeed(0);
#else
    vRandSeed((unsigned long)time(NULL));
#endif
    vInit();
    vClrScr();
    vHideCursor();

    ShowLogo();

    if (argc > 1 && strcmp(argv[1], "-test") == 0) {
        if (argc > 2) {
            vRandSeed(atoi(argv[2]));
        }

        Game.Create('T');
        Game.RunWithoutHero();
    } else if (argc > 1 && strcmp(argv[1], "-demo") == 0) {
        Game.Create('D');
        Game.RunDemo();
    } else {
        if (argc > 1 && strcmp(argv[1], "-god") == 0) {
            Game.isGodMode = true;
        } else {
            Game.isGodMode = false;
        }

        char ch;

        while (true) {
            ch = toupper(vGetch());

            if (ch == '?') {
                XManual man;
                man.Run();
                ShowLogo();
            }

            if (ch == KEY_ESC || ch == 'Z') {
                vFinit();
                return 0;
            }

            if (ch == 'R' || ch == 'N') {
                break;
            }
        }

        Game.Create(ch);
        Game.Run();
    }

    vFinit();
    return 0;
}
