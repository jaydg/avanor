------------------------------ THE TERRAIN --------------------------------
--
-- What this world's ground is made of and how its generators shape it.
-- Every level names one of these when it is created; the numbers here are
-- what gives Avanor its own feel, and nothing in the engine assumes them.

-- Caves and dungeons are cut out of magma. More or wider blobs would make
-- an open cavern; fewer or narrower, a warren.
CAVE = {
	wall = XTileType.MAGMA,
	floor = XTileType.CAVE_FLOOR,
	blobs = 150,
	blob_radius = 3,

	-- Underground you see by what you carry, so nothing is added to
	-- what the creature's own eyes make out.
	sight = 0,
}

-- Built dungeons: one room per 200 cells of floor, and a door at one
-- corridor junction in three. room_chance is how often the level is built
-- with one of the rooms from world/rooms.lua; the rest are made up within
-- the bounds below, one in ten of them trapped.
function Dungeon(room_chance)
	return {
		wall = CAVE.wall,
		floor = CAVE.floor,
		cells_per_room = 200,
		door_odds = 3,
		sight = 0,
		room_chance = room_chance or 0,

		room_width = { 4, 10 },
		room_height = { 4, 6 },
		room_exits = { 2, 3 },
		trap_odds = 10,
		max_traps = 5,
	}
end

-- A level drawn by hand: the engine invents nothing, the pattern the
-- script draws is the whole of it. `fill` is what is left wherever that
-- pattern draws nothing, so it wants to be something solid - and the
-- level is exactly as large as it says, no bigger.
function Drawn(width, height)
	return {
		-- What the level is cut out of, left wherever the pattern
		-- draws nothing, and the ground it invents under a door or a
		-- chest that no neighbour can speak for.
		fill = XTileType.MAGMA,
		floor = XTileType.CAVE_FLOOR,

		sight = 0,

		width = width or 80,
		height = height or 20,
	}
end

-- A floor above another level: it covers only the part of that level
-- named here, and shares its coordinates, so its pattern is drawn at the
-- same place as the building it stands on. Everything the pattern leaves
-- alone is a hole - what shows through is the level below, its ground
-- and whatever walks there, and there is no floor to step onto.
function Above(below, x, y, width, height)
	return {
		below = below,
		origin = { x, y },

		width = width,
		height = height,

		-- Looking out over open country, as far as the level below.
		sight = 30,
	}
end

-- The valley: grassland, a third of it wooded, ringed by mountains that
-- erode inward into foothills.
PLAIN = {
	ground = XTileType.GREEN_GRASS,
	cover = XTileType.TREE,
	slope = { XTileType.HILL, XTileType.LOW_MOUNTAIN, XTileType.MOUNTAIN, XTileType.HIGH_MOUNTAIN },
	cover_odds = 3,
	border_depth = 4,
	erosion = 2,

	-- Open country under the sky: sight reaches as far as the trees
	-- and hills allow, rather than as far as a torch.
	sight = 30,
	width = 200,
	height = 90,
}

-- What the shops of this world are built of.
SHOP = { wall = XTileType.STONE_WALL, floor = XTileType.STONE_FLOOR }
