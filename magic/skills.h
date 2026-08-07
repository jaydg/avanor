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

#ifndef SKILLS_H
#define SKILLS_H

#include <map>

#include <cereal/cereal.hpp>

#include "magic/skill.h"

typedef std::map<XSkill::Skill, XSkill*> XSkillMap;

class XSkills
{
    protected:
        XSkillMap skills;
        // access to the skills is required for showing them
        friend class XHero;

    public:
        XSkills() {}

        ~XSkills() {}

        int GetLevel(XSkill::Skill skt);
        void UseSkill(XSkill::Skill skt, int n = 1);
        XSkill* GetSkill(XSkill::Skill skt);
        void Learn(XSkill::Skill skt, int level = 1);

        // `skills` owns its XSkill* values as plain raw pointers (see
        // Learn()), not a smart-pointer container Cereal has built-in
        // support for - handled manually here rather than changing the
        // map's value type.
        template<class Archive>
        void save(Archive& ar) const
        {
            size_t count = skills.size();
            ar(count);

            for (const auto& [skt, skill] : skills) {
                ar(*skill);
            }
        }

        template<class Archive>
        void load(Archive& ar)
        {
            size_t count{};
            ar(count);

            for (size_t i = 0; i < count; i++) {
                auto* skill = new XSkill(XSkill::Skill{}, 1);
                ar(*skill);
                skills[skill->skt] = skill;
            }
        }
};

#endif
