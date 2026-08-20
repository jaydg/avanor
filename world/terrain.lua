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
		room_chance = room_chance or 0,

		room_width = { 4, 10 },
		room_height = { 4, 6 },
		room_exits = { 2, 3 },
		trap_odds = 10,
		max_traps = 5,
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
	width = 200,
	height = 90,
}

-- What the shops of this world are built of.
SHOP = { wall = XTileType.STONE_WALL, floor = XTileType.STONE_FLOOR }
