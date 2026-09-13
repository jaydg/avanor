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

#ifndef REGISTRY_H
#define REGISTRY_H

#include <iostream>
#include <string>
#include <utility>
#include <vector>

// Where in content the engine is being called from, as "file:line", or
// empty when nothing Lua is calling. Defined in engine/xlua.cpp, declared
// here so this header needs no Lua of its own.
std::string WhereInContent();

// A table of rows that content declares, each under an id of its own:
// the modifiers, the brands, the tastes, the schools of magic, and a
// dozen more. Every one of them was the same four things written out by
// hand - a vector, a linear search for an id, a complaint when something
// names an id nothing declared, and a Register() that turned away a row
// with no id or an id already taken - so they are written once here.
//
//   Registry<BrandStats> brands{"brand"};
//
// `what` names one row for a message ("brand"). That is all the engine is
// told: which file declares the brands is the world's own business, and
// another world will keep them somewhere else entirely. Where each row
// came from is instead *observed* as it is declared, so a complaint can
// point at a real line of a real file rather than at one the engine
// guessed.
//
// Row needs a member `id` comparable with a std::string, and nothing else.
template<typename Row>
class Registry
{
    public:
        explicit Registry(const char* what) : what(what)
        {
        }

        // The row an id names, or nothing. Both constness flavours: a few
        // tables are written to after loading - an unidentified potion
        // takes a colour, an enchantment takes a look - and those need the
        // row itself rather than a copy.
        [[nodiscard]] const Row* Find(const std::string& id) const
        {
            for (const auto& row : rows) {
                if (row.id == id) {
                    return &row;
                }
            }

            return nullptr;
        }

        [[nodiscard]] Row* Find(const std::string& id)
        {
            return const_cast<Row*>(std::as_const(*this).Find(id));
        }

        // Whether anything is declared under this id, complaining by name
        // when it is not. `where` says who asked - "a brand", "the spell
        // 'fire_bolt'" - so the message names both ends of the mistake.
        [[nodiscard]] bool Exists(const std::string& id, const char* where) const
        {
            if (Find(id)) {
                return true;
            }

            std::cerr << "world: " << where << " names a " << what << " '" << id
                      << "', which nothing declares" << DeclaredIn() << std::endl;

            return false;
        }

        // Takes a finished row, or says why not. The two mistakes every
        // table used to check for itself: a row that never said what it is
        // called, and a second row claiming an id already taken.
        bool Add(Row row)
        {
            const std::string here = WhereInContent();

            if (row.id.empty()) {
                std::cerr << "world: a " << what << " with no id" << At(here) << std::endl;
                return false;
            }

            if (const Row* clash = Find(row.id)) {
                std::cerr << "world: two " << what << "s both called '" << row.id
                          << "'" << At(here) << ", the first"
                          << At(sources[static_cast<size_t>(clash - rows.data())])
                          << std::endl;

                return false;
            }

            rows.push_back(std::move(row));
            sources.push_back(here);
            return true;
        }

        // Everything declared, in the order it was declared - which is the
        // order content wrote, and so the order anything walking the whole
        // table should show.
        [[nodiscard]] const std::vector<Row>& All() const { return rows; }
        [[nodiscard]] std::vector<Row>& All() { return rows; }

        // The plain container reads, spelt the way std spells them, so
        // that a table reads like the vector it used to be everywhere
        // except where an id is involved. There is deliberately no
        // push_back(): a row goes in through Add() or not at all.
        [[nodiscard]] bool empty() const { return rows.empty(); }
        [[nodiscard]] size_t size() const { return rows.size(); }
        void clear() { rows.clear(); }

        [[nodiscard]] const Row& front() const { return rows.front(); }
        [[nodiscard]] Row& front() { return rows.front(); }
        [[nodiscard]] const Row& back() const { return rows.back(); }
        [[nodiscard]] Row& back() { return rows.back(); }

        [[nodiscard]] const Row& operator[](const size_t i) const { return rows[i]; }
        [[nodiscard]] Row& operator[](const size_t i) { return rows[i]; }

        auto begin() const { return rows.begin(); }
        auto end() const { return rows.end(); }
        auto begin() { return rows.begin(); }
        auto end() { return rows.end(); }

    private:
        // " at world/brands.lua:23", or nothing when the place is unknown -
        // a row the engine itself put in, or one declared before any script
        // was running.
        static std::string At(const std::string& where)
        {
            return where.empty() ? std::string() : " at " + where;
        }

        // Where this table's rows actually come from, for a complaint about
        // an id nothing declares: the file the last one was declared in, if
        // any has been. Observed, never assumed - a world that keeps its
        // brands somewhere else is described correctly without being asked.
        [[nodiscard]] std::string DeclaredIn() const
        {
            for (auto it = sources.rbegin(); it != sources.rend(); ++it) {
                if (!it->empty()) {
                    const std::string file = it->substr(0, it->find(':'));

                    return " - " + std::string(what) + "s are declared in " + file;
                }
            }

            return std::string();
        }

        std::vector<Row> rows;

        // Where each row above was declared, one for one with it.
        std::vector<std::string> sources;

        // A string literal living for the whole run.
        const char* what;
};

#endif
