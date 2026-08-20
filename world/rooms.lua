------------------------- SPECIAL DUNGEON ROOMS ---------------------------
--
-- Rooms the dungeon generator may stamp into a level instead of one of
-- its own plain rectangles. Each is a pattern like any hand-built
-- location's, with its own palette; the weights are relative to each
-- other, so raising one makes that room more common.
--
-- The generator asks one thing of the art: a '+' or '.' on the border is
-- where a corridor may attach. '+' itself needs no entry in the palette -
-- it is a closed door by default, as in every other pattern.

-- Walls are the surrounding cave's magma rather than built stone, so a
-- room reads as part of the level it was cut into.
local WALL = XTileType.MAGMA
local FLOOR = XTileType.CAVE_FLOOR

-- A chest worth finding, trapped either always or now and then.
local function RoomPalette(always_trapped)
	return {
		['#'] = WALL,
		['.'] = FLOOR,
		['~'] = function(x, y)
			Chest(x, y, Rand(6) + 1, ItemKind.ITEM, 1, 5000)

			if (always_trapped or Rand(3) == 0) then
				Trap(x, y)
			end
		end,
	}
end

-- Whoever cut the room left something behind to watch it.
local GUARD_CLASSES = { CreatureClass.UNDEAD, CreatureClass.GOBLIN, CreatureClass.KOBOLD }

function GuardRoom(x, y, w, h)
	local crc = GUARD_CLASSES[Rand(#GUARD_CLASSES) + 1]

	for i = 1, 10 do
		GuardianClass(crc, "random_guardian", x, y, w, h, XStandardAI.GUARD_AREA)
	end
end

-- A cross of pillars around two chests.
DefineRoom(50, 9, 7,
	"####+####" ..
	"##.....##" ..
	"#.##.##.#" ..
	"+.#~.~#.+" ..
	"#.##.##.#" ..
	"##.....##" ..
	"####+####",
	RoomPalette(false), GuardRoom)

-- One chest in an inner vault - the common one.
DefineRoom(200, 9, 7,
	"####+####" ..
	"#.......#" ..
	"#..#+#..#" ..
	"+..#~#..+" ..
	"#..###..#" ..
	"#.......#" ..
	"####+####",
	RoomPalette(false), GuardRoom)

-- Four chests in two side vaults, all trapped.
DefineRoom(25, 14, 9,
	"##############" ..
	"#............#" ..
	"#.####..####.#" ..
	"#.#~.#..#.~#.#" ..
	"+.#..+..+..#.+" ..
	"#.#~.#..#.~#.#" ..
	"#.####..####.#" ..
	"#............#" ..
	"##############",
	RoomPalette(true), GuardRoom)

-- Four small vaults off a surrounding corridor.
DefineRoom(25, 17, 9,
	"#################" ..
	"#...............#" ..
	"#.#+#####+#####.#" ..
	"#.#..#~~#..#~~#.#" ..
	"+.#..#..#..#..#.+" ..
	"#.#~~#..#~~#..#.#" ..
	"#.####+#####+##.#" ..
	"#...............#" ..
	"#################",
	RoomPalette(true), GuardRoom)
