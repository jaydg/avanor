/*
This file is part of "Avanor, the Land of Mystery" roguelike game

Copyright (C) 2000-2006 Vadim Gaidukevich
Copyright (C) 2025,2026 Joachim de Groot

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

#ifndef HISCORE_H
#define HISCORE_H

#include <memory>
#include <cereal/cereal.hpp>

#include "helpers/xgui.h"

// The release this is. GAME_VERSION is the number and nothing else - the
// Makefile reads it straight off this line to name its archives - while
// GAME_VERSION_FULL is what a build calls itself when asked.
//
// They differ when the commit being built is one no tag points at: the
// Makefile then defines GITREV as "-g<commit>" and appends it, so a build
// from the middle of development says so and cannot be taken for the
// release it was cut from. A source archive carries no repository, gets
// no GITREV, and is a release by definition.
#define GAME_VERSION "0.6.0"

#ifndef GITREV
    #define GITREV ""
#endif

#define GAME_VERSION_FULL GAME_VERSION GITREV

class XHiScoreItem
{
    public:
        XHiScoreItem(int place, unsigned int score, std::string _name, std::string _msg, int flg, int last_record = 0);
        XHiScoreItem() : isLastRecord(0) {}

        XGuiItem_Text* toGuiItem();

        unsigned int score{};
        int place{};

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
        int year{};
        int day{};
        int month{};
        std::string name;
        std::string msg;
        int isLastRecord;
        int flag{}; // win or death
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
        void AddRecord(const std::shared_ptr<XHiScoreItem>& item);
        void Show() const;
};

#endif
