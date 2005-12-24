function LoadScripts()
	dofile("./world/ids.lua");
	dofile("./world/creatures.lua");
	dofile("./world/valley.lua");
	dofile("./world/locations.lua");
	dofile("./world/quests.lua");
	InitAllCreatures();
end

function r()
	LoadScripts()
end

