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

#include <time.h>

#include "engine/global.h"
#include "engine/xfile.h"
#include "helpers/hiscore.h"

void XHiScoreItem::SetText(int place, unsigned int score, const char* name,
    int day, int month, int year, const char* msg)
{
    char buf[256];
    char dbuf[256];

    if (flag == 0) {
        sprintf(dbuf, "Died");
    } else {
        sprintf(dbuf, "Won");
    }

    if (isLastRecord)
        sprintf(buf, "%s%3d %s %7d  %s %s %s on %d/%d/%d.\n               %s\n\n",
            MSG_WHITE, place, MSG_WHITE, score, MSG_WHITE, name, dbuf, day, month, year, msg);
    else
        sprintf(buf, "%s%3d %s %7d  %s %s %s on %d/%d/%d.\n               %s\n\n",
            MSG_CYAN, place, MSG_CYAN, score, MSG_CYAN, name, dbuf, day, month, year, msg);

    XGuiItem_Text::SetText(buf);
}

XHiScoreItem::XHiScoreItem(int _place, unsigned int _score, const char* _name, const char* _msg, int flg, int last_record)
{
    tm * _tm;
    time_t t = time(0);
    _tm = gmtime(&t);

    year = _tm->tm_year + 1900;
    month = _tm->tm_mon + 1;
    day = _tm->tm_mday;
    score = _score;
    place = _place;
    strcpy(name, _name);
    strcpy(msg, _msg);
    isLastRecord = last_record;
    flag = flg;
    SetText(place, score, name, day, month, year, msg);
}

XHiScoreItem::XHiScoreItem(XHiScoreItem * copy)
{
    year = copy->year;
    month = copy->month;
    day = copy->day;
    score = copy->score;
    place = copy->place;
    strcpy(name, copy->name);
    strcpy(msg, copy->msg);
    isLastRecord = copy->isLastRecord;
    flag = copy->flag;
    SetText(place, score, name, day, month, year, msg);
}

void XHiScoreItem::Store(XFile * f)
{
    f->Write(name, sizeof(char), 80);
    f->Write(msg, sizeof(char), 80);
    f->Write(&year, sizeof(int));
    f->Write(&day, sizeof(int));
    f->Write(&month, sizeof(int));
    f->Write(&score, sizeof(int));
    f->Write(&place, sizeof(int));
    f->Write(&flag, sizeof(int));
    f->Write(&reserved, sizeof(int) * 10);
};

void XHiScoreItem::Restore(XFile * f)
{
    f->Read(name, sizeof(char), 80);
    f->Read(msg, sizeof(char), 80);
    f->Read(&year, sizeof(int));
    f->Read(&day, sizeof(int));
    f->Read(&month, sizeof(int));
    f->Read(&score, sizeof(int));
    f->Read(&place, sizeof(int));
    f->Read(&flag, sizeof(int));
    f->Read(&reserved, sizeof(int) * 10);
    SetText(place, score, name, day, month, year, msg);
};

#define HISCORE_VERSION 0xFFEEEE0C

XHiScore::XHiScore()
{
    int i;

    for (i = 0; i < HISCORE_TOP_REC; i++) {
        items[i] = NULL;
    }

    XFile f;

    if (f.Open(HISCORE_FILE_NAME, "rb")) {
        int ver = 0;

        if (f.Read(&ver, sizeof(int)) > 0) {
            if (ver != HISCORE_VERSION) {
                f.Close();
                f.Open(HISCORE_FILE_NAME, "w");
            } else {
                int rec_count = 0;

                if (f.Read(&rec_count, sizeof(int)) > 0) {
                    for (i = 0; i < rec_count; i++) {
                        items[i] = new XHiScoreItem();
                        items[i]->Restore(&f);
                    }
                }
            }
        }

        f.Close();
    }
}

XHiScore::~XHiScore()
{
    for (int i = 0; i < HISCORE_TOP_REC; i++)
        if (items[i]) {
            delete items[i];
        }
}

void XHiScore::AddRecord(XHiScoreItem * item)
{
    int i = 0;

    for (; i < HISCORE_TOP_REC - 1; i++) {
        if (!items[i] || items[i]->score < item->score) {
            break;
        }
    }

    if (i < HISCORE_TOP_REC) {
        if (items[HISCORE_TOP_REC - 1]) {
            delete items[HISCORE_TOP_REC - 1];
        }

        for (int j = HISCORE_TOP_REC - 1; j > i ; j--) {
            items[j] = items[j - 1];

            if (items[j]) {
                items[j]->place = j + 1;
            }
        }

        items[i] = item;
        items[i]->place = i + 1;
    }

    int rec_count = 0;

    for (i = 0; i < HISCORE_TOP_REC; i++) {
        if (items[i]) {
            rec_count++;
        } else {
            break;
        }

    }

    XFile f;

    if (f.Open(HISCORE_FILE_NAME, "wb")) {
        int ver = HISCORE_VERSION;
        f.Write(&ver, sizeof(int));
        f.Write(&rec_count, sizeof(int));

        for (i = 0; i < rec_count; i++) {
            items[i]->Store(&f);
        }

        f.Close();
    }
};

void XHiScore::Show()
{
    XGuiList list;

    list.AddItem(new XGuiItem_Text(MSG_DARKGRAY
        "Pos   Score    Details"));
    list.AddItem(new XGuiItem_Text(
        "--- ---------  -------"));

    for (int i = 0; i < HISCORE_TOP_REC; i++) {
        if (items[i]) {
            XHiScoreItem * item = new XHiScoreItem(items[i]);
            list.AddItem(item);
        }
    }

    list.Run();
}
