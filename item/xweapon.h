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

#ifndef XWEAPON_H
#define XWEAPON_H

#include <cereal/types/base_class.hpp>

#include "item/item.h"

struct WeaponSkillBinding {
    ItemType it;
    XWarSkills::Type ws;
};

class XWeapon : public XItem
{
    public:
        DECLARE_CREATOR(XWeapon, XItem);
        XWeapon(ItemType it = ItemType::RANDOM);
        XWeapon(XWeapon * copy) : XItem((XItem*)copy) {}

        XItem* MakeCopy() override
        {
            return new XWeapon(this);
        }

        int BindWeapon();
        std::string toString() override;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XItem>(this));
        }
    protected:
        std::string GetTemplate(AttackEffectType mask, int isRight = 1);
};

#endif
