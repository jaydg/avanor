-- Central list of creature-template files, one file per category.
--
-- Every named CreatureTemplate.UNIQUE monster's template lives with
-- its sole consumer: the ones with bespoke spawn/AI/dialogue are in
-- world/uniques/, and the couple with no bespoke behavior (just a
-- flavor-unique stat block) live in their one spawning location's
-- file under world/locations/. This file must still be dofiled
-- before both of those - some of their Monster.new() calls inherit
-- from a base template defined here.
dofile("./world/creatures/rats.lua")
dofile("./world/creatures/bats.lua")
dofile("./world/creatures/reptiles.lua")
dofile("./world/creatures/felines.lua")
dofile("./world/creatures/canines.lua")
dofile("./world/creatures/oozes.lua")
dofile("./world/creatures/insects.lua")
dofile("./world/creatures/kobolds.lua")
dofile("./world/creatures/goblins.lua")
dofile("./world/creatures/orcs.lua")
dofile("./world/creatures/humans.lua")
dofile("./world/creatures/undead.lua")
