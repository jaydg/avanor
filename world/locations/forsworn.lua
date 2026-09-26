-- DUNGEON FORSWORN --------------------------------------------------------
--
-- An iron mine, and a poor one. It was sunk on a thin seam by men who had
-- sworn to the crown for the right to work it, and they held that oath
-- about as long as the ore held out: the last of them walked off owing
-- wages, taking the good tools and leaving the ore-house standing open
-- with a full tub still on the track outside. Nothing dramatic happened
-- here. It was simply not worth the digging, and then it was not worth
-- coming back for, and then something else moved in.
--
-- Where Forlorn is galleries and halls, Forsworn is rectangular: adits
-- driven straight and crosscut on the square, because that is how you
-- chase a flat seam cheaply. Hence the rmaze presets - dense rectangular
-- mazes - rather than caverns.
--
-- See world/locations/mines.lua for everything the three mines share.

DUNGEON_FORSWORN_PRESETS = {
	"rmaze2", "rmaze4", "rmaze6", "rmaze8"
}


-- The ore-house: one long shed along the haulage line, its western end
-- fallen in, with the spoil they never carted away banked up outside the
-- door. Deliberately nothing like Forlorn's head-house - that one sprawls
-- on an apron of dressed stone, this is a single shed on bare sand.
local function ForswornRuin(x, y, mine)
	SetPattern(21, 11,
	"                     " ..
	"   ####  #########   " ..
	"   #;;;;;;;;;;;;;#   " ..
	"   #;;;;;;;;;;>;;#   " ..
	"   #;;;;;;;;;;;;;#   " ..
	"   ####+##########   " ..
	"    ss;;;ss          " ..
	"   sss;;;sss         " ..
	"  sssss;sssss        " ..
	"   sss   sss         " ..
	"                     ")
	AddTranslation("s", XTileType.SAND)
	AddTranslation(">", function(sx, sy)
		Way(XStairWay.DOWN, mine.id .. "1", sx, sy)
	end)
	DrawPattern(x, y)
end


table.insert(MINES, {
	id = "FORSWORN",
	brief = "Fsw",
	name = "Forsworn",
	presets = DUNGEON_FORSWORN_PRESETS,
	ruin = ForswornRuin,
})
