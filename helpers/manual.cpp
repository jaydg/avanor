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

#include "global/global.h"
#include "helpers/manual.h"

XGuiList* XManual::OpenPage(const char* page)
{
    XGuiList * list = new XGuiList();
    char tbuf[64000];

    strcpy(tbuf, "could not find file");
    char path[256];
    sprintf(path, "%s%s", vMakePath(DATA_DIR, "manual/"), page);
    FILE * f = fopen(path, "rb");

    if (f) {
        int rb = fread(tbuf, 1, 63999, f);
        fclose(f);
        tbuf[rb] = 0;
    } else {
        sprintf(tbuf, "Could not find file '%s'", path);
    }

    list->AddHtmlText(tbuf);
    return list;
}

void XManual::Run()
{
    const char* open_page = "index.html";

    while (1) {
        XGuiList * list = OpenPage(open_page);
        int res = list->Run();
        delete list;

        if (res == -1) {
            if (strcmp(open_page, "index.html") == 0) {
                break;
            } else {
                open_page = "index.html";
                continue;
            }
        }

        switch (res) {
            case 0:
                open_page = "intro.html";
                break;

            case 1:
                open_page = "qstart.html";
                break;

            case 2:
                open_page = "kblayout.html";
                break;

            case 3:
                open_page = "basic.html";
                break;

            case 4:
                open_page = "magic.html";
                break;

            case 5:
                open_page = "skills.html";
                break;

            case 6:
                open_page = "wskills.html";
                break;

            case 7:
                open_page = "equipment.html";
                break;

            case 8:
                open_page = "tactics.html";
                break;

            case 9:
                open_page = "misc.html";
                break;

            case 10:
                open_page = "testmode.html";
                break;

            case 11:
                open_page = "credits.html";
                break;
        }
    }
}
