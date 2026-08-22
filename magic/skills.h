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
#include <memory>

#include <cereal/cereal.hpp>

#include "magic/skill.h"

// Each skill belongs to the one creature that knows it, and lives
// exactly as long as that creature's XSkills does - unique_ptr says so
// and does the freeing, which raw pointers here never did: nothing
// deleted them on Learn() overwriting an entry, and nothing deleted them
// when the map itself went away.
typedef std::map<XSkill::Skill, std::unique_ptr<XSkill>> XSkillMap;

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

        // Cereal has no built-in support for a map of unique_ptr to a
        // type without a default constructor, so the values are written
        // and read one at a time here rather than as a container.
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
                auto skill = std::make_unique<XSkill>(XSkill::Skill{}, 1);
                ar(*skill);
                skills[skill->skt] = std::move(skill);
            }
        }
};

#endif
