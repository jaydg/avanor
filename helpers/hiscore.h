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

#ifndef __HISCORE_H
#define __HISCORE_H

#include <memory>
#include <cereal/cereal.hpp>

#include "helpers/xgui.h"

#define GAME_VERSION "0.6.0"

class XFile;

class XHiScoreItem
{
    public:
        char buf[256];
        XHiScoreItem(int place, unsigned int score, const char* _name, const char* _msg, int flg, int last_record = 0);
        XHiScoreItem() : isLastRecord(0) {}

        XGuiItem_Text* toGuiItem();

        unsigned int score;
        int place;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(
                name,
                msg,
                year,
                day,
                month,
                score,
                place,
                flag
            );
        }

    protected:
        int year;
        int day;
        int month;
        char name[80];
        char msg[160];
        int isLastRecord;
        int flag; // win or death
};

class XHiScore
{
    private:
        const char* FileName = "avanor.hsc";
        const int TopRecords = 10;
        std::vector<std::shared_ptr<XHiScoreItem>> items;

    public:
        XHiScore();
        ~XHiScore();
        void AddRecord(std::shared_ptr<XHiScoreItem> item);
        void Show();
};

#endif
