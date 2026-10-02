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

#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <zstd.h>

#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/polymorphic.hpp>

#include "engine/xarchive.h"
#include "map/map.h"
#include "game/game.h"
#include "game/location.h"
#include "game/quest.h"
#include "game/xtime.h"
#include "item/xbook.h"
#include "item/xherb.h"
#include "item/xpotion.h"
#include "item/xenhance.h"
#include "item/xscroll.h"

constexpr unsigned int SAVE_GAME_VERSION = 0x0000089;
constexpr unsigned int SAVE_GAME_CONTROL = 0x9ABCDEF;

// ZSTD compression level: 1 provides excellent speed/size tradeoff
// (0.02s compression, ~0.6MB for 62MB input)
constexpr int ZSTD_SAVEGAME_LEVEL = 1;

// ============================================================================
// Helper: Compress string data with ZSTD
// Returns empty vector on error
// ============================================================================
std::vector<char> CompressWithZstd(const std::string& data, int level = ZSTD_SAVEGAME_LEVEL) {
    if (data.empty()) {
        return {};
    }

    // Get maximum possible compressed size
    size_t const max_compressed_size = ZSTD_compressBound(data.size());
    std::vector<char> compressed(max_compressed_size);

    // Compress the data
    size_t const actual_size = ZSTD_compress(
        compressed.data(), max_compressed_size,
        data.data(), data.size(),
        level);

    if (ZSTD_isError(actual_size)) {
        return {};
    }

    compressed.resize(actual_size);
    return compressed;
}

// ============================================================================
// Helper: Decompress vector data with ZSTD
// Returns empty string on error
// ============================================================================
std::string DecompressWithZstd(const std::vector<char>& compressed) {
    if (compressed.empty()) {
        return {};
    }

    // Get the decompressed size from frame header
    unsigned long long const decompressed_size = ZSTD_getFrameContentSize(
        compressed.data(), compressed.size());

    if (decompressed_size == 0) {
        return {};
    }

    std::string decompressed(decompressed_size, '\0');

    size_t const actual_size = ZSTD_decompress(
        decompressed.data(), decompressed_size,
        compressed.data(), compressed.size());

    if (ZSTD_isError(actual_size)) {
        return {};
    }

    return decompressed;
}

// ============================================================================
// Store game state to compressed file
// ============================================================================
bool XArchive::StoreGame(const char* slot)
{
    // Saving happens mid-turn (from the hero's own key handling), so the
    // deferred-release graveyard could still hold objects evicted earlier
    // in this same turn - invalidated, but still lockable through the
    // scheduler's weak_ptr entries, which Cereal would happily serialize
    // as live objects. Release them first so they can't leak into the
    // save as zombies.
    XObject::DrainDeferred();

    // First, serialize game state to JSON string using Cereal
    std::string serialized_data;
    std::ostringstream oss;

    {
        // cereal::JSONOutputArchive only writes the document's closing brace
        // in its own destructor. Scoped separately from oss, so ar destructs
        // and finalizes the document before oss.str() below ever runs.
        cereal::JSONOutputArchive ar(oss);

        ar(SAVE_GAME_VERSION);
        ar(::guid);

        // Tile ids are handed out in world/tiles.lua's order, so they mean
        // nothing on their own. Storing the names beside them lets a game
        // saved under one tile list be restored under another.
        std::vector<std::string> tile_names = XTileType::Names();
        ar(tile_names);

        ar(Game.locations);

        ar(XQuest::quest);

        XBook::SaveTable(ar);
        XPotion::SaveTable(ar);
        XScroll::SaveTable(ar);
        XEnhance::SaveTable(ar);
        PlantDefinition::SaveTable(ar);

        XTime::serialize(ar);

        ar(XGame::hero_guid);
        ar(Game.Scheduler);

        // main_creature is a raw XCreature* (used pervasively as one
        // throughout gameplay code, not worth converting) - saved as a
        // weak_ptr so it resolves via the same shared_ptr identity
        // tracking as everything else in this archive. Must come after
        // Game.locations above: that's what actually registers this
        // creature's shared_ptr id with Cereal.
        ar(XCreature::ToWeakPtr(XCreature::main_creature));

        ar(SAVE_GAME_CONTROL);
    }

    serialized_data = oss.str();

    std::vector<char> compressed = CompressWithZstd(serialized_data, ZSTD_SAVEGAME_LEVEL);

    // Whether the game reached the disk, honestly - this used to answer
    // "saved" whatever happened, so a save that never got written said so
    // to nobody: not to the player, and not to --test-save, which prints
    // PASS on this very answer.
    if (compressed.empty()) {
        std::cerr << "save: could not compress the game" << std::endl;

        return false;
    }

    const std::string path = vMakePath(HOME_DIR, std::string(slot) + ".svg.zst");
    std::ofstream file(path, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "save: could not open " << path << std::endl;

        return false;
    }

    file.write(compressed.data(), compressed.size());
    file.close();

    if (!file) {
        std::cerr << "save: could not write " << path << std::endl;

        return false;
    }

    return true;
}

// ============================================================================
// Restore game state from file (compressed or uncompressed)
// ============================================================================
// The files one slot can be in: what the game writes now, and what older
// versions wrote and it still reads. One list, so that reading a save,
// asking whether there is one and throwing one away cannot disagree about
// what a saved game consists of.
static std::vector<std::string> SlotPaths(const char* slot)
{
    return {
        vMakePath(HOME_DIR, std::string(slot) + ".svg.zst"),
        vMakePath(HOME_DIR, std::string(slot) + ".svg"),
    };
}

bool XArchive::HasSavedGame(const char* slot)
{
    for (const auto& path : SlotPaths(slot)) {
        std::error_code ec;

        if (std::filesystem::is_regular_file(path, ec)) {
            return true;
        }
    }

    return false;
}

bool XArchive::DeleteSavedGame(const char* slot)
{
    bool gone = true;

    for (const auto& path : SlotPaths(slot)) {
        std::error_code ec;

        // remove() answers false for a file that was not there, which is
        // not a failure to delete it - only a file still standing
        // afterwards is.
        std::filesystem::remove(path, ec);

        if (std::filesystem::exists(path, ec)) {
            std::cerr << "save: could not delete " << path << std::endl;
            gone = false;
        }
    }

    return gone;
}

XArchive::LoadResult XArchive::RestoreGame(const char* slot)
{
    LoadResult res;
    res.game_version = SAVE_GAME_VERSION;

    const std::vector<std::string> paths = SlotPaths(slot);
    const std::string& compressed_path = paths[0];
    const std::string& plain_path = paths[1];

    std::string serialized_data;

    // What the game writes now.
    if (std::ifstream file(compressed_path, std::ios::binary | std::ios::ate);
        file.is_open()) {
        res.path = compressed_path;

        const auto file_size = static_cast<size_t>(file.tellg());
        file.seekg(0, std::ios::beg);

        std::vector<char> compressed(file_size);

        if (file.read(compressed.data(), file_size)) {
            serialized_data = DecompressWithZstd(compressed);
        }
    }

    // What older versions wrote, and still read.
    if (serialized_data.empty()) {
        if (std::ifstream file(plain_path); file.is_open()) {
            res.path = plain_path;
            serialized_data.assign(std::istreambuf_iterator<char>(file),
                                   std::istreambuf_iterator<char>());
        }
    }

    if (res.path.empty()) {
        res.outcome = LoadResult::NO_SAVE;
        return res;
    }

    if (serialized_data.empty()) {
        // A file is there and nothing came out of it - an empty save, or
        // one whose compressed stream will not unpack.
        res.outcome = LoadResult::DAMAGED;
        return res;
    }

    return RestoreFromSerializedData(serialized_data, res);
}

// ============================================================================
// Internal helper: Restore game state from serialized JSON string
// Used by both compressed and uncompressed restore paths
// ============================================================================
XArchive::LoadResult XArchive::RestoreFromSerializedData(
    const std::string& serialized_data, LoadResult res)
{
    try {
        std::istringstream iss(serialized_data);
        cereal::JSONInputArchive ar(iss);

        unsigned int version = 0;
        ar(version);

        if (version != SAVE_GAME_VERSION) {
            // Readable, and written by another build of the game. Nothing
            // converts between save formats, so this is as far as it goes.
            res.outcome = LoadResult::WRONG_VERSION;
            res.save_version = version;

            return res;
        }

        ar(::guid);

        std::vector<std::string> tile_names;
        ar(tile_names);

        ar(Game.locations);

        // Every map cell holds a tile id; if the ids moved since the save
        // was written, they are translated through the names now, before
        // anything reads a map.
        if (const std::vector<XTileType::Id> remap = XTileType::RemapFrom(tile_names); !remap.empty()) {
            for (const auto& [key, loc] : Game.locations) {
                if (loc && loc->map) {
                    loc->map->RemapTiles(remap);
                }
            }
        }

        ar(XQuest::quest);

        XBook::LoadTable(ar);
        XPotion::LoadTable(ar);
        XScroll::LoadTable(ar);
        XEnhance::LoadTable(ar);
        PlantDefinition::LoadTable(ar);

        XTime::serialize(ar);

        ar(XGame::hero_guid);
        ar(Game.Scheduler);

        std::weak_ptr<XCreature> main_creature_weak;
        ar(main_creature_weak);
        XCreature::main_creature = main_creature_weak.lock().get();

        unsigned int control = 0;
        ar(control);

        if (control != SAVE_GAME_CONTROL) {
            // The word that should close every save is not there, so the
            // file is truncated or scrambled however far it got.
            res.outcome = LoadResult::DAMAGED;

            return res;
        }

        // Everything the save does not carry, re-derived now that the
        // whole world is back: a floor above another level saves only
        // the *name* of the level it covers (XLocation::below), so the
        // pointers that make a hole in that floor show the level below
        // have to be made again here. This cannot happen before the
        // load - XLocation::Restoration() runs first, when
        // Game.locations is still empty and there is nothing to link.
        XLocation::ValidateWorld(false);
        XLocation::LinkLevels();
    } catch (const cereal::Exception&) {
        // Malformed, foreign or truncated: readable as far as it went and
        // not an Avanor save of any version. Still not a hard failure.
        res.outcome = LoadResult::DAMAGED;

        return res;
    }

    res.outcome = LoadResult::OK;

    return res;
}
