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

#include "magic/skills.h"
#include "engine/xobject.h"

#include <memory>

int XSkills::GetLevel(XSkill::Skill skt)
{
    XSkill* xsk;

    if (xsk = GetSkill(skt)) {
        return xsk->GetLevel();
    } else {
        return 0;
    }
}

void XSkills::UseSkill(XSkill::Skill skt, int n)
{
    XSkill* skill = GetSkill(skt);

    if (skill) {
        skill->UseSkill(n);
    }
}

XSkill* XSkills::GetSkill(XSkill::Skill skt)
{
    const auto it = skills.find(skt);

    return it == skills.end() ? nullptr : it->second.get();
}

void XSkills::Learn(XSkill::Skill skt, int level)
{
    // Learning a skill already known replaces what was there - the old
    // one is freed by the assignment rather than left behind.
    skills[skt] = std::make_unique<XSkill>(skt, level);
}
