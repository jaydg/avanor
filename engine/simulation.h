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

#ifndef SIMULATION_H
#define SIMULATION_H

#include <string>

#include <sol/forward.hpp>

#include "helpers/registry.h"

// A question about the world, asked by playing it out: arrange something,
// let the turns run, and say how it went. "Does the sword the hero brings
// the royal guard change how the orc war party's attack ends?" is a
// question about Avanor, so the whole of it - who fights, what counts as
// finished, what is worth reporting - is written in world/simulations.lua.
// What the engine provides is only the turning of the crank.
//
//   avanor --simulate orc_attack --arg 3 --seed 7
//
// `arg` is one number the scenario reads however it likes; here it is how
// many swords the guard gets. Nothing is drawn and nothing is saved: the
// report goes to stdout, one line, for a shell loop to collect.
struct SimulationStats {
    std::string id;

    // What the scenario is, for --simulate with no name.
    std::string name;

    // Content functions, by name. Setup() arranges it and is given the
    // argument; Finished() is asked now and then whether there is
    // anything left to decide; Report() answers with the line to print.
    // Only Setup is required - a scenario with no Finished() runs to the
    // turn limit, and one with no Report() says nothing.
    std::string setup;
    std::string finished;
    std::string report;

    // How many turns to allow, and how often to ask Finished().
    int turns = 100000;
    int ask_every = 500;
};

const SimulationStats* FindSimulation(const std::string& id);
const std::vector<SimulationStats>& AllSimulations();

class SimulationBuilder
{
    public:
        explicit SimulationBuilder(std::string id);

        SimulationBuilder& Called(const std::string& name);
        SimulationBuilder& Setup(const std::string& handler);
        SimulationBuilder& Finished(const std::string& handler);
        SimulationBuilder& Report(const std::string& handler);
        SimulationBuilder& Turns(int turns);
        SimulationBuilder& AskEvery(int turns);
        void Register();

    private:
        SimulationStats t;
};

void RegisterSimulationLua(sol::state_view& lua);

// Plays one out on a world that has already been built. Answers false if
// no scenario goes by that id, having said so and listed what does.
bool RunSimulation(const std::string& id, int arg);

#endif
