--------------------------- THE MAP ALPHABET ------------------------------
--
-- What the characters of a pattern mean, for every pattern that does not
-- say otherwise itself. The engine knows none of this: a character with
-- no translation here and none of its own just gets a floor tile that
-- suits its neighbours, and a blank leaves the map untouched.

SetDefaultTranslations{
	['.'] = XTileType.GREEN_GRASS,
	[','] = XTileType.CAVE_FLOOR,
	[';'] = XTileType.STONE_FLOOR,
	['#'] = XTileType.STONE_WALL,
	['='] = XTileType.WATER,
	['&'] = XTileType.TREE,
	['X'] = XTileType.FENCE,
	['+'] = function(x, y) Door(x, y) end,
	['/'] = function(x, y) Door(x, y, true) end,
	['^'] = function(x, y) Trap(x, y) end,
}

-- What may be invented underneath a door, a chest or a shopkeeper, in
-- order of preference: the last one any neighbour is standing on wins,
-- so a door cut into a stone corridor gets stone floor while one in a
-- field gets grass.
SetFloorPriority{
	XTileType.GREEN_GRASS,
	XTileType.SAND,
	XTileType.PATH,
	XTileType.ROAD,
	XTileType.CAVE_FLOOR,
	XTileType.STONE_FLOOR,
	XTileType.OBSIDIAN_FLOOR,
	XTileType.GOLDEN_FLOOR,
}

-- Ground the hero has seen but is not looking at now is drawn at this
-- share of its usual brightness, so the field of view reads at a glance.
SetRememberedBrightness(60)

-- No two patches of the same ground are quite the same shade, so that a
-- cavern floor or a stand of trees reads as ground rather than as one
-- colour stamped out a hundred times. The three numbers are how far a
-- cell may stray in brightness (per cent), in hue (degrees of turn) and
-- in saturation (per cent of what the colour has). The hue turns only a
-- few degrees, so a green stays a green and water stays the colour of
-- water. Greys have no saturation to vary and stay exactly grey.
-- SetTileJitter(0, 0, 0) gives back the flat colours.
SetTileJitter(5, 12, 15)
