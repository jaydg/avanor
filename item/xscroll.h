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

#ifndef XSCROLL_H
#define XSCROLL_H

#include <cereal/archives/json.hpp>
#include <cereal/types/base_class.hpp>

#include "item/item.h"
#include "magic/effect.h"

// Which scroll this is - the id world/items/scrolls.lua registered it
// under. A string, like every other content id: the list of scrolls is
// content, and the engine names none of them.
using ScrollName = std::string;

// "Any scroll": the request to pick one, weighted by rarity.
inline constexpr const char* SC_ANY = "";

// Fluent builder for one sort of scroll:
//
//   Scroll.new("fire_bolt")
//       :Called("fire bolt")
//       :Effect(XEffect.FIRE_BOLT)
//       :Worth(50)
//       :Chance(60)
//       :Register()
//
// A scroll whose reading is not an effect names a Lua function with
// :OnRead() instead, which is called with the reader and answers whether
// the reading was worth anything.
//
// The builder holds the row's fields rather than a ScrollDescription,
// which stays private to xscroll.cpp along with the table itself.
class ScrollBuilder
{
    public:
        explicit ScrollBuilder(std::string id);

        ScrollBuilder& Called(const std::string& name);
        ScrollBuilder& Effect(XEffect::Id effect);
        ScrollBuilder& Worth(int value);
        ScrollBuilder& Chance(int rarity);
        ScrollBuilder& ReadInCombat();
        ScrollBuilder& OnRead(const std::string& handler);

        void Register();

    private:
        std::string id;
        std::string real_name;
        XEffect::Id effect{XEffect::NONE};
        int value{0};
        int rarity{0};
        bool read_in_combat{false};
        std::string read_handler;
};

class XScroll : public XItem
{
    public:
        DECLARE_CREATOR(XScroll, XItem);
        explicit XScroll(const ScrollName& _scrn = SC_ANY);
        XScroll(XScroll * copy): XItem((XItem*)copy)
        {
            sc_name = copy->sc_name;
        }

        XItem* MakeCopy() override
        {
            return new XScroll(this);
        }

        // Whether a monster holding one will read it at an enemy. Said by
        // the scroll's own row (:ReadInCombat()), not decided here: which
        // scrolls are worth spending on a fight is content.
        [[nodiscard]] bool isReadInCombat() const;

        bool isIdentified() override;
        void Identify() override;
        std::string toString() override;
        bool Compare(XObject* o) override;
        virtual int onRead(XCreature * cr);

        // Non-template, concrete-archive-typed (like XPotion::Save/
        // LoadTable): ScrollDescription/scroll_descr are private to
        // xscroll.cpp. What is saved is only what this game learned - which
        // scrolls have been identified, and the label each was given - keyed
        // by id, so content may reorder or add rows without spoiling a save.
        static void SaveTable(cereal::JSONOutputArchive& ar);
        static void LoadTable(cereal::JSONInputArchive& ar);

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XItem>(this));
            ar(sc_name);
        }

        // The id of the row this scroll was made from. Was an index into
        // the table plus a duplicate enum; one id says both.
        ScrollName sc_name;
};

#endif
