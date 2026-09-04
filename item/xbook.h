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

#ifndef XBOOK_H
#define XBOOK_H

#include <cereal/archives/json.hpp>
#include <cereal/types/base_class.hpp>

#include "item/item.h"
#include <sol/forward.hpp>

#include "magic/magic.h"

// One sort of book. There is no book name of its own: a book teaches
// exactly one spell, so the spell's id names the book too - what used to
// be a BOOK_NAME enum mirroring SPELL_NAME member for member.
//
// Filled from world/items/books.lua as that script loads.
struct BOOK_REC {
    SPELL_NAME spell_name;
    int rarity{0};

    // What this game calls it before anyone has read it, and whether
    // anyone has. The only two fields that are not content.
    std::string look;
    bool identified{false};

    static int total_value;

    // The row teaching a spell, or nullptr for one nothing defines.
    static BOOK_REC* Find(const SPELL_NAME& spell);

    // A book drawn by rarity - what the world hands out when it does not
    // say which book.
    static SPELL_NAME GetRandomBook();
};

// Fluent builder for one sort of book:
//
//   Book.new("fire_bolt"):Chance(50):Register()
//
// The id is the spell it teaches; nothing else about a book varies.
class BookBuilder
{
    public:
        explicit BookBuilder(std::string spell);

        BookBuilder& Chance(int rarity);

        void Register();

    private:
        std::string spell;
        int rarity{0};
};

// The looks an unread book can have, given to the engine by
// world/items/books.lua and dealt out at random, one per book, each game.
void SetBookAppearances(const sol::table& looks);

class XBook: public XItem
{
    public:
        DECLARE_CREATOR(XBook, XItem)
        explicit XBook(const SPELL_NAME& spell = SP_NONE);
        XBook(XBook * copy) : XItem((XItem*)copy)
        {
            spell_name = copy->spell_name;
            reader_guid = copy->reader_guid;
            left_to_read = copy->left_to_read;
        }

        XItem* MakeCopy() override
        {
            return new XBook(this);
        }

        bool isIdentified() override;
        void Identify() override;
        std::string toString() override;
        int Compare(XObject * o) override;
        virtual int onRead(XCreature * reader);

        // Non-template, concrete-archive-typed (like XPotion::Save/
        // LoadTable): book_descr[] is private to xbook.cpp.
        static void SaveTable(cereal::JSONOutputArchive& ar);
        static void LoadTable(cereal::JSONInputArchive& ar);

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XItem>(this));
            ar(spell_name, left_to_read, reader_guid);
        }

        int left_to_read;

        // The spell this book teaches, which is also its identity.
        SPELL_NAME spell_name;
    protected:
        XGUID reader_guid;
};

#endif
