-- THE ABANDONED MINES ------------------------------------------------------
--
-- Three workings on the plain south of the valley, each a single descent of
-- five to ten levels. They stand in for the three unnamed random caves of
-- the original game - "Random Place Level 1" and so on - which were cut
-- when the world moved to Lua and took the Eye of Raa, and with it
-- Roderick's quest, out of reach. Their shape follows those caves': the
-- same depth, the same width, the same thin slow population, harder the
-- deeper you go, with the Eye at the bottom of one of them.
--
-- Where the original differed: it rolled its layout once per cave and
-- applied it to every level, and it always buried the Eye in the *third*
-- cave, so two of its three entrances never held anything. Here the mine
-- that has it is drawn afresh each game.
--
-- Each mine is a spec in MINES below. What they share - depth, classes,
-- ladder, how a level is settled, how the entrance is hung on the plain -
-- lives in MakeMine(); what makes one different from another is the
-- digging pattern, the ruin over the shaft, and anything its `on_level`
-- hook does.

-- Every mine there is, in the order their own files register them. Each
-- entry carries:
--
--   id        the location id its levels are numbered off ("FORLORN" ->
--             FORLORN1, FORLORN2, ...), and what its stairway names
--   brief     the short name the status line shows, likewise numbered
--   name      what it is called on screen
--   presets   the digging patterns it may take; one is drawn per game and
--             used for every level, so a mine looks like itself all the
--             way down
--   ruin      draws the building over its shaft, given x, y and the mine
--   on_level  optional, called as (level, depth, workings) for anything
--             this mine alone does
MINES = {}

-- What crawled in when the workings were breached. The classes are the
-- ones the original random caves drew from; the ladder is what limits how
-- bad it gets, and a level takes everything of its rung and below.
--
-- It stops at HI on purpose. The level is a ceiling, not a set - see
-- XCreatureStorage::CreateRnd(), which takes any creature whose own level
-- is at most this one - and every creature in the game sits in that pool,
-- the named uniques included. The original's ladder was arithmetic rather
-- than a list and reached UNIQUE on a ten-level dungeon, which would have
-- settled a second Todin.
MINE_CLASSES = {"rat", "bat", "feline", "canine", "reptile", "kobold", "insect", "goblin", "undead"}

MINE_LADDER = {
	CreatureTemplate.VERY_LOW,
	CreatureTemplate.LOW,
	CreatureTemplate.ABOVE_LOW,
	CreatureTemplate.AVG,
	CreatureTemplate.ABOVE_AVG,
	CreatureTemplate.HI,
}

-- Where the three ruins stand. This is the ground the original caves put
-- their entrances on - x 115..180, y 60..80 of the plain - which is what
-- Roderick means when he says the Eye was hidden in the caves far south of
-- here. Every ruin below is 21x11, so these are far enough apart not to
-- overlap, and clear of the undead tomb at (100,50) and the black tower at
-- (155,44) above them.
MINE_SITES = {
	{ x = 116, y = 62 },
	{ x = 141, y = 67 },
	{ x = 163, y = 58 },
}

-- Filled in by AssignMines(), which must run before the valley is drawn:
-- MINE_AT_SITE[i] is the mine whose ruin stands on MINE_SITES[i], and
-- MINE_WITH_EYE is the one the thief died in.
MINE_AT_SITE = {}
MINE_WITH_EYE = nil


-- One level of one mine, and the whole of what the three have in common.
local function MakeMine(mine)
	-- Five to ten levels, as the caves this replaces had.
	local depth = Rand(6) + 5
	local preset = mine.presets[Rand(#mine.presets) + 1]

	-- Where on the ladder the first level starts, so that two games of the
	-- same depth are not the same descent.
	local footing = Rand(2)

	for i = 1, depth do
		local workings = Delve(preset)
		workings.width = 80
		workings.height = 50

		CreateLocation(mine.id .. i, mine.brief .. i,
		               "Dungeon " .. mine.name .. " Level " .. i,
		               XLocation.DELVE, workings)

		if (i == 1) then
			Way(XStairWay.UP, "MAIN")
		else
			Way(XStairWay.UP, mine.id .. (i - 1))
		end

		if (i < depth) then
			Way(XStairWay.DOWN, mine.id .. (i + 1))
		end

		-- Four of each class rather than five, and one attempt per 50000
		-- rather than per 25000: the numbers the original caves used, which
		-- is a thinner and slower population than the rest of the world.
		local rung = footing + i

		if (rung > #MINE_LADDER) then
			rung = #MINE_LADDER
		end

		Settle(MINE_CLASSES, MINE_LADDER[rung], 4, 50000)

		-- Whatever this particular mine does with its own levels.
		if (mine.on_level) then
			mine.on_level(i, depth, workings)
		end

		-- The thief got this far and no further. Dropped as the level is
		-- built rather than after all three mines are standing, because an
		-- item can only be placed into the location that is current - so
		-- which mine has it is decided up front, in AssignMines(), and the
		-- outcome is the same.
		if (mine == MINE_WITH_EYE and i == depth) then
			DropItem(CreateObject("eye_of_raa"))
		end
	end
end


-- Decides which ruin stands where, and which mine the Eye is in. Called
-- before MakeAvanorValley(), because the valley draws the ruins and has to
-- know what it is drawing.
function AssignMines()
	local order = {}

	for i, mine in ipairs(MINES) do
		order[i] = mine
	end

	-- Fisher-Yates. Rand(n) answers 0..n-1.
	for i = #order, 2, -1 do
		local j = Rand(i) + 1
		order[i], order[j] = order[j], order[i]
	end

	for i = 1, #MINE_SITES do
		MINE_AT_SITE[i] = order[i]
	end

	MINE_WITH_EYE = MINES[Rand(#MINES) + 1]
end


-- Draws the three ruins onto the plain. Called from MakeAvanorValley(),
-- while MAIN is the location being built - a pattern can only be drawn
-- into the current location, which is also why the mines themselves are
-- created afterwards and their stairways bind later.
function DrawMineEntrances()
	for i, site in ipairs(MINE_SITES) do
		MINE_AT_SITE[i].ruin(site.x, site.y, MINE_AT_SITE[i])
	end
end


function MakeAbandonedMines()
	for _, mine in ipairs(MINES) do
		MakeMine(mine)
	end
end
