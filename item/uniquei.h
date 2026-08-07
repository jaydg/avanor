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

#ifndef UNIQUEI_H
#define UNIQUEI_H

#include "item/incl_i.h"

class XGlamdring : public XWeapon
{
    public:
        XGlamdring();
        DECLARE_CREATOR(XGlamdring, XWeapon);
        int Compare(XObject * o) override
        {
            return -1;
        }

        std::string toString() override;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XWeapon>(this));
        }
};

class XDeathHack : public XWeapon
{
    public:
        XDeathHack();
        DECLARE_CREATOR(XDeathHack, XWeapon);
        int Compare(XObject * o) override
        {
            return -1;
        }

        std::string toString() override;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XWeapon>(this));
        }
};

class XAvanorDefender : public XWeapon
{
    public:
        XAvanorDefender();
        DECLARE_CREATOR(XAvanorDefender, XWeapon);
        int Compare(XObject * o) override
        {
            return -1;
        }

        std::string toString() override;
        static XGUID avanordefender_guid;
        void Restore(XFile * f) override;

        // avanordefender_guid mirrors this instance's own guid (see the
        // ctor and the existing Restore() above) - resetting it
        // unconditionally here is a correctness no-op on save (it
        // already matches) and exactly what's needed on load.
        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XWeapon>(this));
            avanordefender_guid = guid();
        }
};

class XForestBrotherCloak : public XCloak
{
    public:
        XForestBrotherCloak();
        std::string toString() override;
        DECLARE_CREATOR(XForestBrotherCloak, XCloak);
        int Compare(XObject * o) override
        {
            return -1;
        }

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XCloak>(this));
        }
};

class XGreatElementalRing : public XRing
{
    public:
        XGreatElementalRing();
        DECLARE_CREATOR(XGreatElementalRing, XRing);
        int Compare(XObject * o) override
        {
            return -1;
        }

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XRing>(this));
        }
};

class XAvanorScepter : public XWeapon
{
    public:
        XAvanorScepter();
        DECLARE_CREATOR(XAvanorScepter, XWeapon);
        int Compare(XObject * o) override
        {
            return -1;
        }

        std::string toString() override;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XWeapon>(this));
        }
};

class XAvanorCrown : public XCap
{
    public:
        XAvanorCrown();
        DECLARE_CREATOR(XAvanorCrown, XCap);
        int Compare(XObject * o) override
        {
            return -1;
        }

        std::string toString() override;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XCap>(this));
        }
};

class XAvanorMitre : public XCap
{
    public:
        XAvanorMitre();
        DECLARE_CREATOR(XAvanorMitre, XCap);
        int Compare(XObject * o) override
        {
            return -1;
        }

        std::string toString() override;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XCap>(this));
        }
};

class XTorinAxe : public XWeapon
{
    public:
        XTorinAxe();
        DECLARE_CREATOR(XTorinAxe, XWeapon);
        int Compare(XObject * o) override
        {
            return -1;
        }

        std::string toString() override;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XWeapon>(this));
        }
};

class XTorinShield : public XShield
{
    public:
        XTorinShield();
        DECLARE_CREATOR(XTorinShield, XShield);
        int Compare(XObject * o) override
        {
            return -1;
        }

        std::string toString() override;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XShield>(this));
        }
};

class XDwarfCrown : public XCap
{
    public:
        XDwarfCrown();
        DECLARE_CREATOR(XDwarfCrown, XCap);
        int Compare(XObject * o) override
        {
            return -1;
        }

        std::string toString() override;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XCap>(this));
        }
};

class XBlackClub : public XWeapon
{
    public:
        XBlackClub();
        DECLARE_CREATOR(XBlackClub, XWeapon);
        int Compare(XObject * o) override
        {
            return -1;
        }

        std::string toString() override;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XWeapon>(this));
        }
};

#endif
