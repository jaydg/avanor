/*
This file is part of "Avanor, the Land of Mystery" roguelike game

Copyright (C) 2000-2006 Vadim Gaidukevich
Copyright (C) 2025,2026 Joachim de Groot

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

#include <iostream>

#include <sol/sol.hpp>

#include "engine/simulation.h"
#include "engine/xlua.h"
#include "engine/xobject.h"
#include "game/game.h"

namespace {

Registry<SimulationStats> simulations_db{"simulation"};

// Calls a content function by name, complaining once if it is not there.
// `ok` is left alone when the name is empty - a scenario need not have
// every hook.
sol::protected_function_result Call(const std::string& fn, bool& ok,
    const sol::object& arg = sol::lua_nil)
{
    ok = false;

    if (fn.empty()) {
        return {};
    }

    sol::state_view lua(XLua::State());
    sol::protected_function handler = lua[fn];

    if (!handler.valid()) {
        std::cerr << "world: a simulation runs '" << fn
                  << "', which is not defined" << std::endl;

        return {};
    }

    auto result = arg == sol::lua_nil ? handler() : handler(arg);

    if (!result.valid()) {
        const sol::error err = result;
        std::cerr << "world: " << fn << ": " << err.what() << std::endl;

        return {};
    }

    ok = true;

    return result;
}

} // namespace

const SimulationStats* FindSimulation(const std::string& id)
{
    return simulations_db.Find(id);
}

const std::vector<SimulationStats>& AllSimulations()
{
    return simulations_db.All();
}

SimulationBuilder::SimulationBuilder(std::string id)
{
    t.id = std::move(id);
}

SimulationBuilder& SimulationBuilder::Called(const std::string& name)
{
    t.name = name;
    return *this;
}

SimulationBuilder& SimulationBuilder::Setup(const std::string& handler)
{
    t.setup = handler;
    return *this;
}

SimulationBuilder& SimulationBuilder::Finished(const std::string& handler)
{
    t.finished = handler;
    return *this;
}

SimulationBuilder& SimulationBuilder::Report(const std::string& handler)
{
    t.report = handler;
    return *this;
}

SimulationBuilder& SimulationBuilder::Turns(const int turns)
{
    t.turns = turns;
    return *this;
}

SimulationBuilder& SimulationBuilder::AskEvery(const int turns)
{
    t.ask_every = turns;
    return *this;
}

void SimulationBuilder::Register()
{
    if (t.setup.empty()) {
        std::cerr << "world: the simulation '" << t.id
                  << "' arranges nothing - it needs a :Setup()" << std::endl;

        return;
    }

    if (t.turns <= 0 || t.ask_every <= 0) {
        std::cerr << "world: the simulation '" << t.id
                  << "' would run for no turns at all" << std::endl;

        return;
    }

    simulations_db.Add(t);
}

void RegisterSimulationLua(sol::state_view& lua)
{
    lua.new_usertype<SimulationBuilder>("Simulation",
        sol::constructors<SimulationBuilder(std::string)>(),
        "Called", &SimulationBuilder::Called,
        "Setup", &SimulationBuilder::Setup,
        "Finished", &SimulationBuilder::Finished,
        "Report", &SimulationBuilder::Report,
        "Turns", &SimulationBuilder::Turns,
        "AskEvery", &SimulationBuilder::AskEvery,
        "Register", &SimulationBuilder::Register
    );
}

bool RunSimulation(const std::string& id, const int arg)
{
    const SimulationStats* row = FindSimulation(id);

    if (!row) {
        std::cerr << "There is no simulation called '" << id << "'." << std::endl;

        if (AllSimulations().empty()) {
            std::cerr << "This world declares none." << std::endl;
        } else {
            std::cerr << "This world declares:" << std::endl;

            for (const auto& s : AllSimulations()) {
                std::cerr << "  " << s.id;

                if (!s.name.empty()) {
                    std::cerr << " - " << s.name;
                }

                std::cerr << std::endl;
            }
        }

        return false;
    }

    sol::state_view lua(XLua::State());
    bool ok = false;

    Call(row->setup, ok, sol::make_object(lua, arg));

    if (!ok) {
        return false;
    }

    // The same crank RunWithoutHero() turns, stopped when content says
    // there is nothing left to decide.
    int turns = 0;

    while (turns < row->turns) {
        auto o = Game.Scheduler.Get();

        if (!o) {
            // Nothing left with a turn to take.
            break;
        }

        o->Run();
        XObject::DrainDeferred();
        turns++;

        if (!row->finished.empty() && (turns % row->ask_every) == 0) {
            bool asked = false;
            const auto answer = Call(row->finished, asked);

            if (asked && answer.get<sol::optional<bool>>().value_or(false)) {
                break;
            }
        }
    }

    bool reported = false;
    const auto line = Call(row->report, reported);

    if (reported) {
        if (const sol::optional<std::string> text = line) {
            std::cout << *text << std::endl;
        }
    }

    return true;
}
