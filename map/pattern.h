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

#ifndef PATTERN_H
#define PATTERN_H

#include <string>
#include <vector>

#include <sol/sol.hpp>

#include "map/map.h"

class XLocation;

// A block of map drawn from text: one character per cell, with a palette
// saying what each character means. Hand-built locations use it through
// SetPattern()/AddTranslation()/DrawPattern(), and the dungeon generator
// stamps its defined rooms with the same class.
class XPattern
{
    public:
        // What one character of a pattern puts on the map: either a tile
        // outright, or a script callback, which instead gets a terrain
        // fitting its surroundings and is then called to place whatever
        // stands on it - a shop, a chest, a creature.
        struct Translation {
            char glyph;
            XTileType::Type tile;
            sol::protected_function callback;
        };

        void Setup(int _w, int _h, const std::string& _text)
        {
            w = _w;
            h = _h;
            text = _text;
            translations.clear();
        }

        void AddTranslation(char glyph, XTileType::Type tile)
        {
            translations.push_back({glyph, tile, {}});
        }

        void AddTranslation(char glyph, sol::protected_function callback)
        {
            translations.push_back({glyph, XTileType::UNKNOWN, std::move(callback)});
        }

        // Stamps this pattern onto a location with its top-left corner at
        // (x, y).
        void Draw(XLocation* location, int x, int y) const;

        [[nodiscard]] int Width() const
        {
            return w;
        }

        [[nodiscard]] int Height() const
        {
            return h;
        }

        [[nodiscard]] char At(int _x, int _y) const
        {
            return text[_x + _y * w];
        }

    private:
        std::string text;
        int w = 0;
        int h = 0;
        std::vector<Translation> translations;
};

#endif
