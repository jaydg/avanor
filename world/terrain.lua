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

-- Chambers: rounded rooms grown a cell at a time, joined by single
-- doorways, a third of them eroded down to corridors, with lakes wherever
-- they do not cut the level in two. It wants room to work in - a dozen
-- chambers three tiles to the cell will not fit on the 80x20 a cave is
-- happy with.
function Chambers(width, height)
	return {
		wall = CAVE.wall,
		floor = CAVE.floor,
		water = XTileType.WATER,
		deep_water = XTileType.DEEP_WATER,

		areas = { 9, 19 },
		area_size = { 11, 14 },
		scale = 3,
		gamma = 4,
		corridors = 30,
		corridor_left = 35,
		loop_odds = 25,

		-- Rock does not run straight for a dozen tiles and turn a
		-- right angle. Without these the chambers read as masonry.
		roughness = 40,
		roughness_grain = 6,
		smooth = 1,

		-- A seventh of the floor under water, and two fifths of that
		-- out of your depth.
		water_level = 14,
		deep_level = 40,
		water_grain = 24,

		sight = 0,
		width = width or 96,
		height = height or 96,
	}
end

-- The same generator with nothing underground about it: the rock is
-- trees, the floor is grass, and what comes out is dense forest with
-- glades in it, joined by paths, and ponds where they fit. Trees can be
-- pushed through, unlike magma, so the woods are slow rather than solid.
function Forest(width, height)
	local wood = Chambers(width, height)

	wood.wall = XTileType.TREE
	wood.floor = XTileType.GREEN_GRASS

	-- Woodland wanders more than rock does, and there is usually more
	-- than one way round a tree.
	wood.corridors = 20
	wood.corridor_left = 40
	wood.loop_odds = 40

	wood.water_level = 12
	wood.deep_level = 35
	wood.water_grain = 20

	-- Out under the sky, as far as the trees allow.
	wood.sight = 30

	return wood
end

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

-- Delved: floor eaten out of solid rock one cell at a time, where what may
-- be eaten next is decided by a table of neighbourhood patterns. That table
-- is the whole character of the level, and the tables live in
-- world/delve_patterns.lua - add one there and it is usable here by name.
--
-- Delve("rmaze1")      one of the named patterns
-- Delve("rmaze1", 500) the same, but dig 500 cells rather than a fifth
-- Delve()              a table invented on the spot, different every game
--
-- A pattern that carries `pull` or `store` brings them with it, because a
-- few of them only come out right one way round.
--
-- `cells` is the most floor it may dig, and so how much of the level is
-- open; left out, it takes a fifth of the map.
function Delve(pattern, cells)
	local p = pattern and DELVE_PATTERNS[pattern]

	if (pattern and not p) then
		error("no delve pattern named '" .. pattern .. "'")
	end

	p = p or {}

	return {
		wall = CAVE.wall,
		floor = CAVE.floor,
		sight = 0,

		desc = p.desc or "",
		ngb = p.ngb or { 0, 0 },
		conmil = p.conmil or 0,
		cells = cells or 0,
		pull = p.pull or XDelve.CUBEROOT,
		store = p.store or XDelveStore.PERM,
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
