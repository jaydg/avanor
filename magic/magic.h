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

#ifndef MAGIC_H
#define MAGIC_H

#include <vector>

#include "creature/creature.h"
#include "magic/effect.h"
#include "magic/spelldef.h"

class XSpell
{
        XSpell() {}

        int cast_count;
        int eff_level;
    public:
        SPELL_NAME spell_name;
        XSpell(SPELL_NAME spn);
        SPELL_NAME GetSpellName()
        {
            return spell_name;
        }

        EFFECT GetEffect();
        void Cast(); //calling after successful casting of spell
        int GetManaCost();
        int GetEffectivity()
        {
            return eff_level;
        }

        MAGIC_SCHOOL GetSchool();
        void GainLevel(int n = 1)
        {
            eff_level += n;
        }

        void toString(char* buf);
        static const char* GetName(SPELL_NAME spn);
        const char* GetName()
        {
            return GetName(spell_name);
        }
};

class XMagic
{
    public:
        XMagic();
        explicit XMagic(XMagic*) = delete;

        RESULT Cast(XSpell * spell, XCreature * caster);
        int GetSpellRange(XSpell * spell, XCreature * caster);
        int Train(MAGIC_SCHOOL school, int count);
        int GainLevel(MAGIC_SCHOOL school, int n = 1);
        int LevelToString(MAGIC_SCHOOL school, char* buf);
        void Learn(SPELL_NAME spell);
        XSpell* GetSpell(SPELL_NAME spell);
        int GetLevel(MAGIC_SCHOOL ms)
        {
            return magic_level[ms];
        }

        std::vector<XSpell*> spells;

        void Store(XFile * f);
        void Restore(XFile * f);
    protected:
        int magic_level[MS_EOF];
        int magic_count[MS_EOF];
};

#endif
