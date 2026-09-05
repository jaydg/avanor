function LoadScripts()
	dofile("./world/tiles.lua");
	dofile("./world/palette.lua");
	dofile("./world/terrain.lua");
	dofile("./world/delve_patterns.lua");
	dofile("./world/rooms.lua");
	dofile("./world/resistances.lua");
	dofile("./world/combat_skills.lua");
	dofile("./world/spells.lua");
	dofile("./world/creatures/init.lua");
	dofile("./world/items/init.lua");
	LoadItems();
	dofile("./world/uniques/init.lua");
	dofile("./world/valley.lua");
	dofile("./world/valley_extras.lua");
	dofile("./world/locations/init.lua");
	dofile("./world/hero.lua");
	dofile("./world/quests.lua");
	dofile("./world/traps.lua");
	dofile("./world/tally.lua");
end

-- Actually builds the world (as opposed to LoadScripts(), which only
-- defines functions/creature templates) - only called for a brand new
-- game. A restored game skips this.
function InitWorld()
	MakeAvanorValley()
	MakeSmallCave()
	MakeMushroomCave()
	MakeDwarvenCity()
	MakeRatCellar()
	MakeVulcano()
	MakeWizardDungeon()
	MakeDungeonForlorn()
	CreateAllQuests()
end

function r()
	LoadScripts()
end
