-- Central list of location-family files, one file per dungeon/area
--
-- Each file's MakeXxx() function must stay a global - it's called
-- directly by name from C++, not from anywhere in this Lua codebase.
dofile("./world/locations/dwarven_city.lua")
dofile("./world/locations/mushroom_cave.lua")
dofile("./world/locations/small_cave.lua")
dofile("./world/locations/rat_cellar.lua")
dofile("./world/locations/volcano.lua")
dofile("./world/locations/wizard_dungeon.lua")
-- The three abandoned mines. mines.lua first: it declares the registry the
-- other three append themselves to, and holds everything they share.
dofile("./world/locations/mines.lua")
dofile("./world/locations/forlorn.lua")
dofile("./world/locations/forsworn.lua")
dofile("./world/locations/forfeit.lua")
