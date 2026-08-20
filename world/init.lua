function LoadScripts()
	dofile("./world/rooms.lua");
	dofile("./world/creatures/init.lua");
	dofile("./world/uniques/init.lua");
	dofile("./world/valley.lua");
	dofile("./world/valley_extras.lua");
	dofile("./world/locations/init.lua");
	dofile("./world/quests.lua");
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
	CreateAllQuests()
end

function r()
	LoadScripts()
end
