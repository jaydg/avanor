-- DUNGEON FORLORN ---------------------------------------------------------
--
-- A gem mine, once. The dwarves who sank it followed the seams down and
-- then followed them further, and somewhere below the last gallery they
-- broke into something that was not a seam. What poured up through that
-- breach took the mine in a night, and the miners who got out did not go
-- back for the tools, let alone the stones. The head-house on the plain
-- south of the valley still stands, more or less, over a shaft nobody has
-- climbed down in living memory.
--
-- Later, and much smaller: a servant of King Roderick who had stolen the
-- Eye of Raa needed somewhere nobody would follow him, and got as far as
-- the bottom before whatever lives there found him. The artifact is still
-- on him.
--
-- The mine replaces the three unnamed random caves of the original game -
-- "Random Place Level 1" and so on - which were cut when the world moved
-- to Lua and took the Eye of Raa, and with it Roderick's quest, out of
-- reach. Its shape follows theirs: five to ten levels, each one a single
-- descent, settled from the same set of creature classes, harder the
-- deeper you go, with the Eye at the very bottom.

-- Which digging pattern the mine takes this game. All five are caverns
-- rather than mazes - galleries and halls, which is what a mine collapses
-- into - and each gives the whole mine its character, so every level of a
-- given game looks like it belongs to the same workings.
DUNGEON_FORLORN_PRESETS = {
	"old34_10", "old35_0", "old23_50", "old24_200", "old34_0"
}

-- What crawled in when the mine was breached. The classes are the ones the
-- original random caves drew from; the ladder is what limits how bad it
-- gets, and a level takes everything of its rung and below.
--
-- It stops at HI on purpose. The level is a ceiling, not a set - see
-- XCreatureStorage::CreateRnd(), which takes any creature whose own level
-- is at most this one - and every creature in the game sits in that pool,
-- the named uniques included. The original's ladder was arithmetic rather
-- than a list and reached UNIQUE on a ten-level dungeon, which would have
-- settled a second Todin.
DUNGEON_FORLORN_CLASSES = CreatureClass.RAT + CreatureClass.FELINE
	+ CreatureClass.CANINE + CreatureClass.REPTILE + CreatureClass.KOBOLD
	+ CreatureClass.INSECT + CreatureClass.GOBLIN + CreatureClass.UNDEAD

DUNGEON_FORLORN_LADDER = {
	CreatureTemplate.VERY_LOW,
	CreatureTemplate.LOW,
	CreatureTemplate.ABOVE_LOW,
	CreatureTemplate.AVG,
	CreatureTemplate.ABOVE_AVG,
	CreatureTemplate.HI,
}

function MakeDungeonForlorn()
	-- Five to ten levels, as the caves this replaces had.
	local depth = Rand(6) + 5
	local preset = DUNGEON_FORLORN_PRESETS[Rand(#DUNGEON_FORLORN_PRESETS) + 1]

	-- Where on the ladder the first level starts, so that two games of the
	-- same depth are not the same descent.
	local footing = Rand(2)

	for i = 1, depth do
		local workings = Delve(preset)
		workings.width = 80
		workings.height = 50

		CreateLocation("FORLORN" .. i, "Frl" .. i,
		               "Dungeon Forlorn Level " .. i, XLocation.DELVE, workings)

		if (i == 1) then
			Way(XStairWay.UP, "MAIN")
		else
			Way(XStairWay.UP, "FORLORN" .. (i - 1))
		end

		if (i < depth) then
			Way(XStairWay.DOWN, "FORLORN" .. (i + 1))
		end

		-- Four of each class rather than five, and one attempt per 50000
		-- rather than per 25000: the numbers the original caves used, which
		-- is a thinner and slower population than the rest of the world.
		local rung = footing + i

		if (rung > #DUNGEON_FORLORN_LADDER) then
			rung = #DUNGEON_FORLORN_LADDER
		end

		Settle(DUNGEON_FORLORN_CLASSES, DUNGEON_FORLORN_LADDER[rung], 4, 50000)

		-- The thief got this far and no further.
		if (i == depth) then
			DropItem(CreateObject("XEyeOfRaa"))
		end
	end
end
