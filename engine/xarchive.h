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

#ifndef XARCHIVE_H
#define XARCHIVE_H

#include <string>

class XArchive
{
    public:
        // What came of trying to read a saved game. The caller has to tell
        // these apart: no save at all is no reason not to start a new game,
        // while a save that is there and cannot be read is - the new game
        // would write over it at its first save. They used to be one int,
        // and every one of them reported as "there is not a saved game".
        struct LoadResult {
            enum Outcome {
                OK,             // the world is back
                NO_SAVE,        // no save file to read
                WRONG_VERSION,  // written by a different build of the game
                DAMAGED,        // unreadable: truncated, corrupt or foreign
            };

            Outcome outcome = NO_SAVE;

            // The file this is about, so the player can be told which one
            // to move aside. Empty when there was none.
            std::string path;

            // What WRONG_VERSION found, and what this build writes.
            unsigned int save_version = 0;
            unsigned int game_version = 0;

            bool ok() const { return outcome == OK; }

            // A word for the outcome, for a log or a failing test run. The
            // player gets told rather more than this - see XGame::Create().
            const char* what() const
            {
                switch (outcome) {
                    case OK:            return "ok";
                    case NO_SAVE:       return "no save file";
                    case WRONG_VERSION: return "wrong save format";
                    case DAMAGED:       return "damaged save file";
                }

                return "unknown";
            }
        };

        // Which file a game is written to and read back from. The tests
        // pass TEST_SLOT so that building and checking the game cannot
        // destroy somebody's actual save - StoreGame() overwrites without
        // asking, and a --test-save world has no hero in it, so a test run
        // used to leave a file that looks like a saved game and cannot be
        // played.
        static constexpr const char* PLAYER_SLOT = "avanor";
        static constexpr const char* TEST_SLOT = "avanor-test";

        static bool StoreGame(const char* slot = PLAYER_SLOT);
        static LoadResult RestoreGame(const char* slot = PLAYER_SLOT);

        // Whether there is a file to restore from at all. Deliberately
        // only asks that much and does not read it: an unplayable save is
        // still a save the player has, and the menu has to offer to
        // restore it for them to be told what is wrong with it and to be
        // rid of it.
        static bool HasSavedGame(const char* slot = PLAYER_SLOT);

        // Throws the save away, both the compressed file and the plain one
        // an older version may have left. True when nothing of the slot is
        // on disk afterwards - which includes there having been nothing to
        // begin with.
        static bool DeleteSavedGame(const char* slot = PLAYER_SLOT);

    private:
        // Internal helper to restore from serialized JSON string
        // Used by both compressed and uncompressed restore paths. Takes the
        // partly-filled result so that the file it came from is already
        // named in whatever outcome it reports.
        static LoadResult RestoreFromSerializedData(
            const std::string& serialized_data, LoadResult res);
};

#endif
