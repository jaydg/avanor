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

#ifndef UNIQUE_H
#define UNIQUE_H

#include <cereal/types/base_class.hpp>

#include "creature/anycr.h"

class XBeelzvile : public XAnyCreature
{
        XBeelzvile() {}
        friend class cereal::access;

    public:
        DECLARE_CREATOR(XBeelzvile, XAnyCreature);
        XBeelzvile(CreatureTemplate * cr);
        void NewMove() override;
        void Move() override;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XAnyCreature>(this));
        }
};

class XRoderick : public XAnyCreature
{
    protected:
        XRoderick() {}
        friend class cereal::access;

    public:
        DECLARE_CREATOR(XRoderick, XAnyCreature);
        XRoderick(CreatureTemplate * cr);

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XAnyCreature>(this));
        }
};

class XRandomMonster : public XAnyCreature
{

};

class XHighPriest : public XAnyCreature
{
    protected:
        XHighPriest() {}
        friend class cereal::access;

    public:
        DECLARE_CREATOR(XHighPriest, XAnyCreature);
        XHighPriest(CreatureTemplate * cr);

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XAnyCreature>(this));
        }
};

class XRotmoth : public XAnyCreature
{
    protected:
        XRotmoth() {}
        friend class cereal::access;

    public:
        DECLARE_CREATOR(XRotmoth, XAnyCreature);
        XRotmoth(CreatureTemplate * cr);

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XAnyCreature>(this));
        }
};

class XRotmothAI : public XStandardAI
{
    public:
        XRotmothAI() = delete;
        XRotmothAI(XCreature * cr) : XStandardAI(cr) {}

        void onWasAttacked(XCreature * attacker) override;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XStandardAI>(this));
        }
};

// XRotmothAI() is deleted - real construction always takes an owning
// XCreature*. Lives here, not in unique.cpp - same cross-TU visibility
// reasoning as XStandardAI's own CEREAL_LOAD_VIA_PLACEHOLDER_CONSTRUCT
// (see std_ai.h): XRotmothAI's construction gets instantiated from
// inside XCreature::load() itself, wherever that gets reinstantiated.
CEREAL_LOAD_VIA_PLACEHOLDER_CONSTRUCT(XRotmothAI, serialize, nullptr);

class XGiana : public XAnyCreature
{
    protected:
        XGiana() {}
        friend class cereal::access;

    public:
        DECLARE_CREATOR(XGiana, XAnyCreature);
        XGiana(CreatureTemplate * cr);
        void FirstStep(int _x, int _y, XLocation * _l) override;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XAnyCreature>(this));
        }
};

#endif
