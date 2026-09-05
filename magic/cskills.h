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

#ifndef CSKILLS_H
#define CSKILLS_H

#include <map>
#include <string>
#include <vector>

#include <cereal/cereal.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>
#include <sol/forward.hpp>

// How good somebody is with a class of weapon - swords, bows, or whatever
// else the world it lives in has. Which classes exist is content, because
// it follows entirely from what there is to fight with.
using COMBAT_SKILL = std::string;

// "No skill": an item that trains nothing, and the answer when a role
// nothing fills is asked for.
inline const COMBAT_SKILL CS_NONE;

// The jobs the engine needs *some* skill for, whatever the world calls it.
// It never names a skill - it asks for the one that fills a role, and
// content says which. A world with no shields simply leaves that role
// empty, and blocking never happens.
enum class CombatRole {
    NONE = 0,

    // Fighting with nothing in hand.
    UNARMED,

    // Blocking with something held up against a blow.
    SHIELD,

    // Flinging something by hand, with nothing to launch it.
    THROW,
};

// One class of weapon: what it is called, how it groups in the skill
// screen, which engine role it can serve, and how its bonuses grow.
// Filled from world/combat_skills.lua as that script loads.
struct CombatSkillStats {
    COMBAT_SKILL id;
    std::string name;

    // Where the skill screen lists it. Content's word, not a guess from
    // the order things happen to be declared in.
    enum class Group {
        MELEE,
        MISSILE,
        SHIELD,
    } group{Group::MELEE};

    CombatRole role{CombatRole::NONE};

    // Bonus per level, 0 to MAX_LEVEL. Short rows are padded with their
    // last value, so content need not spell out sixteen numbers to say
    // "this never helps".
    std::vector<int> base_dv;
    std::vector<int> base_hit;
    std::vector<int> base_dmg;
};

class XCombatSkills
{
    public:
        static constexpr int MAX_LEVEL = 15;

        // Registers CombatRole and the CombatSkill builder.
        static void RegisterLua(sol::state_view& lua);

        XCombatSkills() = default;

        [[nodiscard]] int GetLevel(const COMBAT_SKILL& cs) const;
        static int GetN(int level);
        [[nodiscard]] int GetDV(const COMBAT_SKILL& cs) const;
        [[nodiscard]] int GetHIT(const COMBAT_SKILL& cs) const;
        [[nodiscard]] int GetDMG(const COMBAT_SKILL& cs) const;
        [[nodiscard]] int GetUseTime(const COMBAT_SKILL& cs) const;
        [[nodiscard]] static const std::string& GetName(const COMBAT_SKILL& cs);
        [[nodiscard]] int GetMarks(const COMBAT_SKILL& cs) const;

        // The skill this creature is best at among those content gave the
        // role - so a world may offer several ways to fight bare-handed
        // and a fighter uses whichever of them they have come furthest
        // with. Empty if nothing fills the role.
        [[nodiscard]] COMBAT_SKILL Best(CombatRole role) const;

        void UseSkill(const COMBAT_SKILL& cs, int time = 1);
        void SetLevel(const COMBAT_SKILL& cs, int level);

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(marks_counter, levels);
        }

    private:
        // Keyed by id rather than indexed, so content may add or reorder
        // skills without spoiling a save. Absent means never practised.
        std::map<COMBAT_SKILL, int> marks_counter;
        std::map<COMBAT_SKILL, int> levels;
};

// Every skill content declared, in declaration order.
extern std::vector<CombatSkillStats> combat_skills;

// The row for an id, or nullptr for one nothing defines.
const CombatSkillStats* FindCombatSkill(const COMBAT_SKILL& cs);

// Fluent builder for one:
//
//   CombatSkill.new("sword")
//       :Called("Swords")
//       :Group(CombatGroup.MELEE)
//       :Defence(0, 0, 1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 12)
//       :ToHit(0, 1, 1, 1, 2, 2, 2, 3, 3, 4, 4, 5, 6, 8, 10, 12)
//       :Damage(0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 8, 10, 12, 14)
//       :Register()
class CombatSkillBuilder
{
    public:
        explicit CombatSkillBuilder(std::string id);

        CombatSkillBuilder& Called(const std::string& name);
        CombatSkillBuilder& Group(CombatSkillStats::Group group);
        CombatSkillBuilder& Role(CombatRole role);
        CombatSkillBuilder& Defence(const sol::table& rows);
        CombatSkillBuilder& ToHit(const sol::table& rows);
        CombatSkillBuilder& Damage(const sol::table& rows);

        void Register();

    private:
        CombatSkillStats t;
};

#endif
