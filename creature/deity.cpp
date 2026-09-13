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

#include <algorithm>
#include <cmath>
#include <iostream>

#include <fmt/format.h>
#include <sol/sol.hpp>

#include "helpers/registry.h"
#include "creature/creature.h"
#include "magic/effect.h"
#include "creature/deity.h"
#include "engine/xlua.h"
#include "helpers/msgwin.h"
#include "item/item.h"
#include "map/map_objects.h"

Registry<DeityStats> deities_db{"deity"};
Registry<DeityRank> deity_ranks_db{"rank of favour"};

const DeityStats* FindDeity(const std::string& id)
{
    return deities_db.Find(id);
}

const std::string& DeityName(const DEITY& id)
{
    if (const DeityStats* row = FindDeity(id)) {
        return row->name;
    }

    return id;
}

const DeityRank* FindRank(const std::string& id)
{
    return deity_ranks_db.Find(id);
}

const DeityRank* RankFor(const int favour)
{
    const DeityRank* best = nullptr;

    for (const auto& row : deity_ranks_db) {
        if (favour >= row.from && (!best || row.from > best->from)) {
            best = &row;
        }
    }

    return best;
}

DeityBuilder::DeityBuilder(std::string id)
{
    t.id = std::move(id);
}

DeityBuilder& DeityBuilder::Called(const std::string& name)
{
    t.name = name;
    return *this;
}

DeityBuilder& DeityBuilder::OnKill(const std::string& handler)
{
    t.on_kill = handler;
    return *this;
}

DeityBuilder& DeityBuilder::Grants(const std::string& name, const std::string& needs,
    const int cost, const std::string& effect,
    sol::optional<std::string> alternative)
{
    DeityHelp help;
    help.name = name;
    help.needs = needs;
    help.cost = cost;
    help.effect = effect;
    help.alternative = alternative.value_or(std::string());
    t.grants.push_back(help);
    return *this;
}

void DeityBuilder::Register()
{
    if (t.name.empty()) {
        t.name = t.id;
    }

    // The ranks are declared before the gods that point at them, so a
    // grant naming one that does not exist is a typo, and saying so here
    // beats waiting for somebody to pray before anyone notices.
    for (const auto& help : t.grants) {
        if (!help.effect.empty() && !FindEffect(help.effect)) {
            std::cerr << "world: '" << t.id << "' grants '" << help.name
                      << "', which does '" << help.effect
                      << "' - an effect world/effects.lua does not declare" << std::endl;
        }

        if (!FindRank(help.needs)) {
            std::cerr << "world: '" << t.id << "' grants '" << help.name
                      << "' at rank '" << help.needs
                      << "', which world/deities.lua does not declare - never offered"
                      << std::endl;
        }
    }

    deities_db.Add(t);
}

DeityRankBuilder::DeityRankBuilder(std::string id)
{
    t.id = std::move(id);
}

DeityRankBuilder& DeityRankBuilder::Called(const std::string& name)
{
    t.name = name;
    return *this;
}

DeityRankBuilder& DeityRankBuilder::From(const int favour)
{
    t.from = favour;
    return *this;
}

DeityRankBuilder& DeityRankBuilder::Score(const int points)
{
    t.score = points;
    return *this;
}

void DeityRankBuilder::Register()
{
    if (t.name.empty()) {
        t.name = t.id;
    }

    deity_ranks_db.Add(t);
}

void XDeity::RegisterLua(sol::state_view& lua)
{
    lua.new_usertype<DeityBuilder>("Deity",
        sol::constructors<DeityBuilder(std::string)>(),
        "Called", &DeityBuilder::Called,
        "OnKill", &DeityBuilder::OnKill,
        "Grants", &DeityBuilder::Grants,
        "Register", &DeityBuilder::Register
    );

    lua.new_usertype<DeityRankBuilder>("DeityRank",
        sol::constructors<DeityRankBuilder(std::string)>(),
        "Called", &DeityRankBuilder::Called,
        "From", &DeityRankBuilder::From,
        "Score", &DeityRankBuilder::Score,
        "Register", &DeityRankBuilder::Register
    );
}

void XReligion::RegisterLua(sol::state_view& lua)
{
    lua.new_usertype<XReligion>("XReligion");
}

int XReligion::GetFavour(const DEITY& deity) const
{
    const auto it = favour.find(deity);
    return it == favour.end() ? 0 : it->second;
}

void XReligion::SetFavour(const DEITY& deity, const int value)
{
    favour[deity] = value;
}

void XReligion::ChangeFavour(const DEITY& deity, const int delta)
{
    favour[deity] += delta;
}

DEITY XReligion::BestRegarded() const
{
    DEITY best;
    int best_favour = 0;
    bool any = false;

    for (const auto& row : deities_db) {
        const int val = GetFavour(row.id);

        if (!any || val > best_favour) {
            best = row.id;
            best_favour = val;
            any = true;
        }
    }

    return best;
}

// Every god is told of every kill and decides for itself whether it cares.
// The engine has no opinion about who deserves killing - that used to be
// an if/else here saying undead pleased one god and displeased the other.
void XReligion::KillCreature(XCreature* killer, XCreature* victim)
{
    if (!killer || !victim) {
        return;
    }

    sol::state_view lua(XLua::State());

    for (const auto& row : deities_db) {
        if (row.on_kill.empty()) {
            continue;
        }

        sol::protected_function handler = lua[row.on_kill];

        if (!handler.valid()) {
            std::cerr << "world: the god '" << row.id << "' watches kills through '"
                      << row.on_kill << "', which is not defined" << std::endl;
            continue;
        }

        const auto result = handler((void*)killer, (void*)victim);

        if (!result.valid()) {
            const sol::error err = result;
            std::cerr << "world: '" << row.on_kill << "' failed: " << err.what() << std::endl;
        }
    }
}

int XReligion::SacrificeItem(XCreature* cr, XItem* item, const DEITY& deity_in)
{
    const int val = cr->sk->GetLevel(XSkill::Skill::RELIGION);
    DEITY deity = deity_in;

    if (deity.empty()) {
        deity = BestRegarded();
    }

    XMapObject* tmo = cr->l->map->GetSpecial(cr->x, cr->y);
    const XAltar* altar = dynamic_cast<XAltar*>(tmo);

    // An altar's own god takes precedence over whoever the sacrificer
    // would otherwise have picked.
    if (altar && !altar->GetDeity().empty()) {
        deity = altar->GetDeity();
    }

    if (deity.empty()) {
        return 0;
    }

    if (cr->isVisible()) {
        msgwin.Add(fmt::format("{} prays to {}.", cr->name, DeityName(deity)));
        msgwin.Add(fmt::format("{} disappears in a bright light.", item->toString()));
    }

    int sacrifice_value;

    if (item->kind & ItemKind::MONEY) {
        sacrifice_value = static_cast<int>(std::sqrt(static_cast<float>(item->quantity)) + 1)
            * (val / 4 + 1);
    } else {
        sacrifice_value = static_cast<int>((std::sqrt(static_cast<float>(item->GetValue() * item->quantity)) + 1))
            * (val / 4 + 1);
    }

    cr->sk->UseSkill(XSkill::Skill::RELIGION);

    if (altar) {
        sacrifice_value *= 3;
        cr->sk->UseSkill(XSkill::Skill::RELIGION, 5);
    }

    item->UnCarry();
    item->Invalidate();

    ChangeFavour(deity, sacrifice_value);

    if (!cr->isHero() && vRand(5) == 0) {
        cr->stats->Modify(XStats::Random(), 1);

        if (cr->isVisible()) {
            msgwin.Add(fmt::format("{} looks more powerful!", cr->name));
        }
    }

    return 1;
}

const DeityRank* XReligion::GetRank(const DEITY& deity) const
{
    return RankFor(GetFavour(deity));
}

std::vector<const DeityHelp*> XReligion::AvailableHelp(const DEITY& deity) const
{
    std::vector<const DeityHelp*> available;
    const DeityStats* row = FindDeity(deity);

    if (!row) {
        return available;
    }

    const int have = GetFavour(deity);

    for (const auto& help : row->grants) {
        const DeityRank* needed = FindRank(help.needs);

        // Complained about at registration; never offered.
        if (!needed) {
            continue;
        }

        if (have >= needed->from) {
            available.push_back(&help);
        }
    }

    return available;
}

void XReligion::Pray(const DEITY& deity, const DeityHelp& help, XCreature* prayer)
{
    EFFECT effect = help.effect;

    if (!help.alternative.empty() && vRand(2) == 0) {
        effect = help.alternative;
    }

    const RESULT res = XEffect::Make(prayer, effect, 50);

    if (res == SUCCESS) {
        ChangeFavour(deity, -help.cost);
        prayer->sk->UseSkill(XSkill::Skill::RELIGION, 3);
    } else if (res != ABORT) {
        // Neither answered nor taken back: the god heard and did nothing.
        msgwin.Add("Your prayer was unheard.");
    }
}
