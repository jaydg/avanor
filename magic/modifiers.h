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

#ifndef MODIFIERS_H
#define MODIFIERS_H

#include <algorithm>

#include "creature/creature.h"
#include "engine/xobject.h"

enum MODIFIER_TYPE {
    MOD_UNKNOWN = -1,
    MOD_WOUND,
    MOD_POISON,
    MOD_CONFUSE,
    MOD_STUN,
    MOD_HEROISM,
    MOD_DISEASE,
    MOD_SEE_INVISIBLE,
    MOD_ACID_RESISTANCE,
    MOD_FIRE_RESISTANCE,
    MOD_COLD_RESISTANCE,
    MOD_POISON_RESISTANCE,

    MOD_PARALYSE,
    MOD_WEAK,
    MOD_RESISTANCE,
    MOD_BOOST_STATS,
    MOD_BOOST_SPEED,
    MOD_SLOWNESS,

    MOD_DELAYED, // start modifier (poison, wound, weak) after some time
    MOD_EOF
};

enum MODIFIER_RESULT {
    MR_OK = 0,
    MR_REMOVE = 1,
    MR_DIE = 2,
};

class XBasicModifier
{
    protected:
        XBasicModifier() {}

    public:
        XBasicModifier(MODIFIER_TYPE mt, int _val, XCreature * _cr = nullptr);

        virtual ~XBasicModifier() {
            setter = nullptr;
        }

        virtual int Compare(XBasicModifier *mod)
        {
            if (mod->mdt == mdt && mod->setter == setter) {
                return 0;
            } else {
                return -1;
            }
        }

        virtual void Concat(XBasicModifier* o)
        {
            val += (o)->val;
        }

        virtual MODIFIER_RESULT Run(XCreature * owner)
        {
            if (owner->_HP <= 0) {
                owner->Die(setter);
                return MR_DIE;
            } else {
                val--;
                return val > 0 ? MR_OK : MR_REMOVE;
            }

        }

        virtual int onSet(XCreature * owner)
        {
            return 1;
        }

        virtual int onRemove(XCreature * owner)
        {
            return 1;
        }

        virtual const char* GetDisplayName(int xval)
        {
            return "err";
        }

        virtual const char* SetMsg()
        {
            return "";
        }

        virtual const char* RemoveMsg()
        {
            return "";
        }

        virtual const char* ChangeMsg(int val)
        {
            return "";
        }

        virtual const char* ApplyMsg()
        {
            return "";
        }

        virtual void Store(XFile * f);
        virtual void Restore(XFile * f);

        MODIFIER_TYPE mdt;
        int val; // value of modifier;
        XPtr<XCreature> setter;
    protected:
};

class XModWound : public XBasicModifier
{
    public:
        XModWound(int _val, XCreature * _cr = nullptr) : XBasicModifier(MOD_WOUND, _val, _cr) {}

        XModWound()
        {
            assert(0);
        }

        const char* GetDisplayName(int xval) override
        {
            if (xval > 0) {
                if (xval < 3) {
                    return MSG_LIGHTGRAY"graze";
                } else if (xval >= 3 && xval < 8) {
                    return MSG_LIGHTGRAY "light cut";
                } else if (xval >= 8 && xval < 20) {
                    return MSG_YELLOW "severely cut";
                } else if (xval >= 20 && xval < 50) {
                    return MSG_RED "deep wound";
                } else if (xval >= 50 && xval < 100) {
                    return MSG_RED "deep gash";
                } else if (xval >= 100) {
                    return MSG_DARKGRAY "mortal wound";
                }

                return "err";
            } else {
                return "";
            }
        }

        const char* SetMsg() override
        {
            return "You've been wounded.";
        }

        const char* RemoveMsg() override
        {
            return "Your wounds heal.";
        }

        const char* ChangeMsg(int val) override
        {
            return val > 0 ? "You are wounded again." : "Your bleeding slows.";
        }

        const char* ApplyMsg() override
        {
            return "You lose blood!";
        }

        MODIFIER_RESULT Run(XCreature * owner) override;
};


class XModPoison : public XBasicModifier
{
    public:
        XModPoison(int _val, XCreature * _cr = nullptr) : XBasicModifier(MOD_POISON, _val * 10, _cr) {}

        XModPoison()
        {
            assert(0);
        }

        const char* GetDisplayName(int xval) override
        {
            return MSG_GREEN "poisoned";
        }

        const char* SetMsg() override
        {
            return "You are poisoned!";
        }

        const char* RemoveMsg() override
        {
            return "You feel relieved.";
        }

        const char* ChangeMsg(int val) override
        {
            return val > 0 ? "You are poisoned again!" : "You feel somewhat relieved.";
        }

        const char* ApplyMsg() override
        {
            return "You feel the poison coursing through your body.";
        }

        MODIFIER_RESULT Run(XCreature * owner) override;
};


class XModConfuse : public XBasicModifier
{
    public:
        XModConfuse(int _val, XCreature * _cr = nullptr) : XBasicModifier(MOD_CONFUSE, _val, _cr) {}

        XModConfuse()
        {
            assert(0);
        }

        const char* GetDisplayName(int xval) override
        {
            return MSG_LIGHTGRAY "confused";
        }

        const char* SetMsg() override
        {
            return "You are confused.";
        }

        const char* RemoveMsg() override
        {
            return "Your thoughts clear.";
        }

        const char* ChangeMsg(int val) override
        {
            return val > 0 ? "Your confusion grows." : "You feel a little clearer.";
        }

        const char* ApplyMsg() override
        {
            return "You stagger.";
        }

        MODIFIER_RESULT Run(XCreature * owner) override;
};

class XModStun : public XBasicModifier
{
    public:
        XModStun(int _val, XCreature * _cr = nullptr) : XBasicModifier(MOD_STUN, _val, _cr) {}

        XModStun()
        {
            assert(0);
        }

        const char* GetDisplayName(int xval) override
        {
            return xval < 10 ? MSG_LIGHTGRAY "stunned " : MSG_YELLOW "heavily stunned ";
        }

        const char* SetMsg() override
        {
            return "You are stunned.";
        }

        const char* RemoveMsg() override
        {
            return "You feel steady again.";
        }

        const char* ChangeMsg(int val) override
        {
            return val > 0 ? "You're stunned again." : "You feel a bit clearer.";
        }

        const char* ApplyMsg() override
        {
            return "You stagger.";
        }

        int onSet(XCreature * owner) override;
        int onRemove(XCreature * owner) override;
};

class XModHeroism : public XBasicModifier
{
    public:
        XModHeroism(int _val, XCreature * _cr = nullptr) : XBasicModifier(MOD_HEROISM, _val, _cr) {}

        XModHeroism()
        {
            assert(0);
        }

        const char* GetDisplayName(int xval) override
        {
            return MSG_LIGHTGRAY "hero";
        }

        const char* SetMsg() override
        {
            return "You feel like a hero.";
        }

        const char* RemoveMsg() override
        {
            return "Your courage fades.";
        }

        const char* ChangeMsg(int val) override
        {
            return val > 0 ? "Your resolve weakens." : "Your courage grows.";
        }

        const char* ApplyMsg() override
        {
            return "";
        }

        int onSet(XCreature * owner) override;
        int onRemove(XCreature * owner) override;
};

class XModDisease : public XBasicModifier
{
    public:
        XModDisease(int _val, XCreature * _cr = nullptr) : XBasicModifier(MOD_DISEASE, _val, _cr) {}

        XModDisease()
        {
            assert(0);
        }

        const char* GetDisplayName(int xval) override
        {
            return MSG_GREEN "disease";
        }

        const char* SetMsg() override
        {
            return "You feel ill.";
        }

        const char* RemoveMsg() override
        {
            return "Your health returns.";
        }

        const char* ChangeMsg(int val) override
        {
            return val > 0 ? "You become more diseased." : "You start to feel better.";
        }

        const char* ApplyMsg() override
        {
            return "";
        }

        int onSet(XCreature * owner) override;
        int onRemove(XCreature * owner) override;
        MODIFIER_RESULT Run(XCreature * owner) override;
};


class XModWeak : public XBasicModifier
{
    public:
        XModWeak(int _val, XCreature * _cr = nullptr) : XBasicModifier(MOD_WEAK, _val, _cr) {}

        XModWeak()
        {
            assert(0);
        }

        const char* GetDisplayName(int xval) override
        {
            return MSG_CYAN "weakness";
        }

        const char* SetMsg() override
        {
            return "You feel very weak.";
        }

        const char* RemoveMsg() override
        {
            return "Your strength returns.";
        }

        const char* ChangeMsg(int val) override
        {
            return val > 0 ? "You feel a little stronger." : "Your weakness grows.";
        }

        const char* ApplyMsg() override
        {
            return "";
        }

        int onSet(XCreature * owner) override;
        int onRemove(XCreature * owner) override;
        MODIFIER_RESULT Run(XCreature * owner) override;
};


class XModParalyse : public XBasicModifier
{
    public:
        XModParalyse(int _val, XCreature * _cr = nullptr) : XBasicModifier(MOD_PARALYSE, _val, _cr) {}

        XModParalyse()
        {
            assert(0);
        }

        const char* GetDisplayName(int xval) override
        {
            return MSG_WHITE "paralyzed!";
        }

        const char* SetMsg() override
        {
            return "You've been paralyzed!";
        }

        const char* RemoveMsg() override
        {
            return "You can move again.";
        }

        const char* ChangeMsg(int val) override
        {
            return val > 0 ? "Your muscles start to loosen." : "Your muscles tighten.";
        }

        const char* ApplyMsg() override
        {
            return "";
        }

        MODIFIER_RESULT Run(XCreature * owner) override;
};

class XModDelayed : public XBasicModifier
{
    public:
        XModDelayed(MODIFIER_TYPE _mt, int value, int delay,
            XCreature * _cr = nullptr) : XBasicModifier(MOD_DELAYED, delay, _cr),
            set_mt(_mt), set_val(value)
        {}

        XModDelayed()
        {
            assert(0);
        }

        MODIFIER_RESULT Run(XCreature * owner) override;
        void Store(XFile * f) override;
        void Restore(XFile * f) override;

        int Compare(XBasicModifier *o) override
        {
            if (XBasicModifier::Compare(o) == 0 && set_mt == ((XModDelayed*)o)->set_mt) {
                return 0;
            } else {
                return -1;
            }

        }

        virtual void Concat(XModDelayed* mod)
        {
            val = std::min(val, mod->val);
            set_val += mod->set_val;
            XBasicModifier::Concat(mod); //hack
        }

    protected:
        MODIFIER_TYPE set_mt;
        int set_val;
};

class XModSeeInvisible : public XBasicModifier
{
    public:
        XModSeeInvisible(int _val, XCreature * _cr = nullptr) : XBasicModifier(MOD_SEE_INVISIBLE, _val, _cr) {}

        XModSeeInvisible()
        {
            assert(0);
        }

        const char* GetDisplayName(int xval) override
        {
            return MSG_LIGHTGRAY "perceptive";
        }

        const char* SetMsg() override
        {
            return "You feel more perceptive.";
        }

        const char* RemoveMsg() override
        {
            return "You feel less perceptive.";
        }

        const char* ChangeMsg(int val) override
        {
            return val > 0 ? "You feel less perceptive." : "You feel more preceptive.";
        }

        const char* ApplyMsg() override
        {
            return "";
        }

        int onSet(XCreature * owner) override;
        int onRemove(XCreature * owner) override;
};

class XModBoostSpeed : public XBasicModifier
{
    public:
        XModBoostSpeed(int _val, XCreature * _cr = nullptr) : XBasicModifier(MOD_BOOST_SPEED, _val, _cr) {}

        XModBoostSpeed()
        {
            assert(0);
        }

        const char* GetDisplayName(int xval) override
        {
            return "";
        }

        const char* SetMsg() override
        {
            return "You feel very quick.";
        }

        const char* RemoveMsg() override
        {
            return "You slow down to normal.";
        }

        const char* ChangeMsg(int val) override
        {
            return val > 0 ? "You feel quicker." : "You feel slower.";
        }

        const char* ApplyMsg() override
        {
            return "";
        }

        int onSet(XCreature * owner) override;
        int onRemove(XCreature * owner) override;
};

class XModSlowness : public XBasicModifier
{
    public:
        XModSlowness(int _val, XCreature * _cr = nullptr) : XBasicModifier(MOD_SLOWNESS, _val, _cr) {}

        XModSlowness()
        {
            assert(0);
        }

        const char* GetDisplayName(int xval) override
        {
            return "";
        }

        const char* SetMsg() override
        {
            return "You feel very slow.";
        }

        const char* RemoveMsg() override
        {
            return "You feel your speed return.";
        }

        const char* ChangeMsg(int val) override
        {
            return val > 0 ? "You feel slower." : "You feel quicker.";
        }

        const char* ApplyMsg() override
        {
            return "";
        }

        int onSet(XCreature * owner) override;
        int onRemove(XCreature * owner) override;
};

class XModAcidResistance : public XBasicModifier
{
    public:
        XModAcidResistance(int _val, XCreature * _cr = nullptr) : XBasicModifier(MOD_ACID_RESISTANCE, _val, _cr) {}

        XModAcidResistance()
        {
            assert(0);
        }

        const char* GetDisplayName(int xval) override
        {
            return "";
        }

        const char* SetMsg() override
        {
            return "You feel safer.";
        }

        const char* RemoveMsg() override
        {
            return "You feel less safe.";
        }

        const char* ChangeMsg(int val) override
        {
            return val > 0 ? "." : ".";
        }

        const char* ApplyMsg() override
        {
            return "";
        }

        int onSet(XCreature * owner) override;
        int onRemove(XCreature * owner) override;
};

class XModFireResistance : public XBasicModifier
{
    public:
        XModFireResistance(int _val, XCreature * _cr = nullptr) : XBasicModifier(MOD_FIRE_RESISTANCE, _val, _cr) {}

        XModFireResistance()
        {
            assert(0);
        }

        const char* GetDisplayName(int xval) override
        {
            return "";
        }

        const char* SetMsg() override
        {
            return "You feel safer.";
        }

        const char* RemoveMsg() override
        {
            return "You feel less safe.";
        }

        const char* ChangeMsg(int val) override
        {
            return val > 0 ? "." : ".";
        }

        const char* ApplyMsg() override
        {
            return "";
        }

        int onSet(XCreature * owner) override;
        int onRemove(XCreature * owner) override;
};

class XModColdResistance : public XBasicModifier
{
    public:
        XModColdResistance(int _val, XCreature * _cr = nullptr) : XBasicModifier(MOD_COLD_RESISTANCE, _val, _cr) {}

        XModColdResistance()
        {
            assert(0);
        }

        const char* GetDisplayName(int xval) override
        {
            return "";
        }

        const char* SetMsg() override
        {
            return "You feel safer.";
        }

        const char* RemoveMsg() override
        {
            return "You feel less safe.";
        }

        const char* ChangeMsg(int val) override
        {
            return val > 0 ? "." : ".";
        }

        const char* ApplyMsg() override
        {
            return "";
        }

        int onSet(XCreature * owner) override;
        int onRemove(XCreature * owner) override;
};

class XModPoisonResistance : public XBasicModifier
{
    public:
        XModPoisonResistance(int _val, XCreature * _cr = nullptr) : XBasicModifier(MOD_POISON_RESISTANCE, _val, _cr) {}

        XModPoisonResistance()
        {
            assert(0);
        }

        const char* GetDisplayName(int xval) override
        {
            return "";
        }

        const char* SetMsg() override
        {
            return "You feel safer.";
        }

        const char* RemoveMsg() override
        {
            return "You feel less safe.";
        }

        const char* ChangeMsg(int val) override
        {
            return val > 0 ? "." : ".";
        }

        const char* ApplyMsg() override
        {
            return "";
        }

        int onSet(XCreature * owner) override;
        int onRemove(XCreature * owner) override;
};

#endif
