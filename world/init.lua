function LoadScripts()
	dofile("./world/ids.lua");
	dofile("./world/creatures/init.lua");
	dofile("./world/uniques/init.lua");
	dofile("./world/valley.lua");
	dofile("./world/valley_extras.lua");
	dofile("./world/locations/init.lua");
	dofile("./world/quests.lua");
end

function r()
	LoadScripts()
end

