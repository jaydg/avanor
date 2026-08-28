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

#ifndef XARCHIVE_H
#define XARCHIVE_H

class XArchive
{
    public:
        // Which file a game is written to and read back from. The tests
        // pass TEST_SLOT so that building and checking the game cannot
        // destroy somebody's actual save - StoreGame() overwrites without
        // asking, and a --test-save world has no hero in it, so a test run
        // used to leave a file that looks like a saved game and cannot be
        // played.
        static constexpr const char* PLAYER_SLOT = "avanor";
        static constexpr const char* TEST_SLOT = "avanor-test";

        static int StoreGame(const char* slot = PLAYER_SLOT);
        static int RestoreGame(const char* slot = PLAYER_SLOT);

    private:
        // Internal helper to restore from serialized JSON string
        // Used by both compressed and uncompressed restore paths
        static int RestoreFromSerializedData(const std::string& serialized_data);
};

#endif
