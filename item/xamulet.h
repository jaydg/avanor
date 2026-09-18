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

#ifndef XAMULET_H
#define XAMULET_H

#include <cereal/archives/json.hpp>

#include "item/xenhance.h"

class XAmulet : public XEnhance
{
    public:
        DECLARE_CREATOR(XAmulet, XEnhance);

        explicit XAmulet(const std::string& enh = "");

        explicit XAmulet(XAmulet* copy) : XEnhance(static_cast<XEnhance *>(copy)) {}

        XItem* MakeCopy() override
        {
            return new XAmulet(this);
        }

        // aidentified_db is private to xamulet.cpp.

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XEnhance>(this));
        }
};

#endif
