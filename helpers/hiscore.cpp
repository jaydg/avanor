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

#include <algorithm>
#include <ctime>
#include <fstream>
#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>

#include "engine/global.h"
#include "helpers/hiscore.h"

XGuiItem_Text* XHiScoreItem::toGuiItem()
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

    return new XGuiItem_Text(buf);
}

XHiScoreItem::XHiScoreItem(const int _place, const unsigned int _score, const char* _name, const char* _msg, const int flg, const int last_record)
{
    const time_t t = time(nullptr);
    const tm* _tm = gmtime(&t);

    year = _tm->tm_year + 1900;
    month = _tm->tm_mon + 1;
    day = _tm->tm_mday;
    score = _score;
    place = _place;
    strcpy(name, _name);
    strcpy(msg, _msg);
    isLastRecord = last_record;
    flag = flg;
}

#define HISCORE_VERSION 0xFFEEEE0C

XHiScore::XHiScore()
{
    // read existing scores, if present
    std::ifstream file(vMakePath(DATA_DIR, FileName));

    if (!file.is_open()) {
        return;
    }

    // file exists, we can read it
    cereal::JSONInputArchive archive(file);

    int version;

    archive(
        version,
        items
    );
}

XHiScore::~XHiScore()
{
    items.erase(items.begin(), items.end());
}

void XHiScore::AddRecord(const std::shared_ptr<XHiScoreItem>& item)
{
    items.push_back(item);

    // sort by score
    std::sort(
        items.begin(),
        items.end(),
        [](const std::shared_ptr<XHiScoreItem>& a, const std::shared_ptr<XHiScoreItem>& b) {return a->score > b->score; }
    );

    // trim to desired length
    if (items.size() > TopRecords) {
        items.resize(TopRecords);
    }

    // add place to scores
    int place = 1;
    for (const std::shared_ptr<XHiScoreItem>& _item : items) {
        _item->place = place++;
    }

    // write scores
    std::ofstream file(vMakePath(DATA_DIR, FileName));
    cereal::JSONOutputArchive archive(file);

    int version = HISCORE_VERSION;

    archive(
        version,
        items
    );
};

void XHiScore::Show() const {
    XGuiList list;

    list.AddItem(new XGuiItem_Text(MSG_DARKGRAY
        "Pos   Score    Details"));
    list.AddItem(new XGuiItem_Text(
        "--- ---------  -------"));

    for (const std::shared_ptr<XHiScoreItem>& item : items) {
        list.AddItem(item->toGuiItem());
    }

    list.Run();
}
