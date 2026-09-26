-- DUNGEON FORFEIT ---------------------------------------------------------
--
-- A silver mine, and for a while a very good one - good enough that it was
-- worth taking. Whose it was by right and whose it was by law stopped being
-- the same question somewhere in the paperwork, and when the crown finally
-- settled the matter it settled it by forfeit: the workings went to nobody
-- at all. The wall and the gate were built by an owner who expected to be
-- robbed, and in the end they kept out the only people who knew where the
-- silver was.
--
-- Silver runs in veins rather than seams, so this is not driven on the
-- square like Forsworn and not cavernous like Forlorn: it wanders. Hence
-- the dmaze presets.
--
-- See world/locations/mines.lua for everything the three mines share.

DUNGEON_FORFEIT_PRESETS = {
	"dmaze1", "dmaze2", "dmaze3", "dmaze5"
}


-- A walled compound, not a shed: an outer wall with one gate, and the
-- shaft-house standing on its own in the middle of the yard, so the
-- silver could be counted before anything left. Both doors still hang. The
-- two dead trees outside are the only thing here that looks old.
local function ForfeitRuin(x, y, mine)
	SetPattern(21, 11,
	"                     " ..
	"   XXXXXXXXXXXXX     " ..
	"   X;;;;;;;;;;;X     " ..
	"   X;;#######;;X     " ..
	"   X;;#;;;;;#;;X     " ..
	"   X;;#;;>;;+;;X     " ..
	"   X;;#;;;;;#;;X     " ..
	"   X;;#######;;X     " ..
	"   X;;;;;;;;;;;X     " ..
	"   XXXXXXX+XXXXX     " ..
	"       &     &       ")
	AddTranslation(">", function(sx, sy)
		Way(XStairWay.DOWN, mine.id .. "1", sx, sy)
	end)
	DrawPattern(x, y)
end


table.insert(MINES, {
	id = "FORFEIT",
	brief = "Ffe",
	name = "Forfeit",
	presets = DUNGEON_FORFEIT_PRESETS,
	ruin = ForfeitRuin,
})
