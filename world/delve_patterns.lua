------------------------- DELVING PATTERNS ---------------------------------
--
-- What the delve generator digs is decided entirely by one table: for each
-- of the 256 ways a cell's eight neighbours can be wall or floor, the
-- permil chance that cell may be dug. Change nothing but that table and the
-- same algorithm gives a town grid, a diagonal warren or an open cavern.
--
-- These are the tables Kusigrosz published with the generator, with his
-- own descriptions. They are data, not engine: add your own here and it is
-- usable everywhere a name is, and nothing in C++ needs to know.
--
-- An entry is written "index:permil", separated by spaces. The index is the
-- neighbourhood read as 8 bits - lowest bit the cell to the right, then
-- clockwise, floor 1 and wall 0 - and the permil is the chance to dig. Only
-- the nonzero entries need naming. Each entry is applied to the pattern
-- rotated by 90, 180 and 270 degrees too, so a table cannot lean in one
-- direction. See map/delve_builder.h for the worked example.
--
-- A pattern may also carry `pull` and `store`, because a few of these want
-- a particular one to come out right - Kusigrosz says which in his notes
-- below. `pull` decides which cell is dug next (CUBEROOT fluffy, ALL
-- compact, BOTTOM winding) and `store` which way it turns (PERM random, CW,
-- CCW).
--
-- Instead of `desc` a pattern may give `ngb = { min, max }` and `conmil`,
-- which builds a table rather than naming one: any cell with between min
-- and max floor neighbours may be dug, at 1000 permil where digging it
-- would not join two separate pieces of floor and at `conmil` where it
-- would. Lower `conmil` means fewer loops. This is how the old* tables
-- below were originally derived, and it reaches shapes no name here has.

DELVE_PATTERNS = {
	-- rect maze with loops
	rmaze1 = {
		desc = "1:1000 3:1000 6:1000 17:1000 14:0 19:1000 25:1000 27:1000 29:0 " ..
		       "51:1000 57:1000 102:1000 59:1000 107:0 110:1000 187:1000 127:0 " ..
		       "255:0 ",
	},

	-- dense rect maze / cavern with lots of pillars (griddy - town)
	rmaze2 = {
		desc = "1:1000 17:1000 14:1000 19:1000 25:1000 27:1000 29:0 51:1000 " ..
		       "57:1000 102:1000 59:1000 107:0 110:1000 187:1000 127:0 255:0 ",
	},

	-- dense rect maze / cavern, long multipillars (no pillars?)
	rmaze3 = {
		desc = "1:1000 17:1000 14:1000 30:0 57:1000 106:0 122:0 255:0 ",
	},

	-- dense rect maze with 2x2 rooms at ends, may die young
	rmaze4 = {
		desc = "1:1000 17:1000 7:1000 14:1000 30:0 57:1000 106:0 122:0 255:0 ",
	},

	-- together with pulling always the 0-th cell, a rect town wallbuilder
	rmaze5 = {
		desc = "1:1000 6:1000 17:1000 19:1000 25:1000 27:1000 51:1000 57:1000 " ..
		       "102:1000 ",
		pull = XDelve.BOTTOM,
	},

	-- sparse rect town grid, few pillars, may die young
	rmaze6 = {
		desc = "1:1000 4:1000 14:50 16:1000 17:1000 19:1000 25:1000 27:1000 " ..
		       "49:1000 1:1000 56:50 57:1000 64:1000 68:1000 70:1000 76:1000 " ..
		       "78:1000 100:1000 102:1000 108:1000 131:50 145:1000 147:1000 " ..
		       "153:1000 177:1000 196:1000 198:1000 204:1000 224:50 228:1000 ",
	},

	-- mostly rect maze - mostly caverns actually
	rmaze7 = {
		desc = "1:1000 3:1000 6:1000 5:1000 9:1000 10:1000 17:1000 18:1000 " ..
		       "34:1000 7:1000 15:1000 23:1000 29:1000 30:1000 31:1000 55:1000 " ..
		       "61:1000 62:1000 87:1000 94:1000 103:1000 63:1000 95:1000 " ..
		       "111:1000 119:1000 123:1000 126:1000 127:1000 191:1000 255:1000 ",
	},

	-- another rect maze
	rmaze8 = {
		desc = "1:1000 3:200 6:200 17:1000 14:200 19:1000 21:1000 25:1000 " ..
		       "27:200 51:200 53:1000 54:1000 57:1000 85:1000 86:1000 102:200 ",
	},

	-- rect maze, call with STORE_CW or CCW and PULL_BOTTOM
	rmaze9 = {
		desc = "1:1000 3:1000 5:1000 7:200 14:200 18:1000 19:200 38:1000 " ..
		       "94:200 ",
		store = XDelveStore.CW,
		pull = XDelve.BOTTOM,
	},

	-- narrow diag maze with occasional straight connections
	dmaze1 = {
		desc = "2:1000 17:1000 ",
	},

	-- mostly diag narrow mesh
	dmaze2 = {
		desc = "1:100 2:1000 9:1000 10:1000 17:1000 18:1000 34:1000 ",
	},

	-- mostly diag maze with some straight corridors
	dmaze3 = {
		desc = "2:1000 9:1000 10:1000 17:1000 18:1000 21:1000 34:1000 ",
	},

	-- narrow diag maze with wider caverns
	dmaze4 = {
		desc = "2:1000 10:1000 21:1000 85:1000 170:1000 31:1000 47:1000 " ..
		       "55:1000 59:1000 61:1000 62:1000 87:1000 91:1000 94:1000 " ..
		       "103:1000 107:1000 110:1000 122:1000 171:1000 63:1000 95:1000 " ..
		       "111:1000 119:1000 123:1000 126:1000 175:1000 187:1000 127:1000 " ..
		       "191:1000 255:1000 ",
	},

	-- mixed narrow maze, try with PULL_BOTTOM
	dmaze5 = {
		desc = "2:1000 5:1000 37:200 53:200 58:1000 86:200 95:1000 122:200 " ..
		       "171:1000 ",
		pull = XDelve.BOTTOM,
	},

	-- old type cavern ngb_min = 1, ngb_max = 1, connchance = 0 permil
	old11_0 = {
		desc = "1:1000 2:1000 ",
	},

	-- old type cavern ngb_min = 1, ngb_max = 2, connchance = 0 permil
	old12_0 = {
		desc = "1:1000 2:1000 3:1000 5:1000 6:1000 ",
	},

	-- old type cavern ngb_min = 2, ngb_max = 3, connchance = 0 permil
	old23_0 = {
		desc = "3:1000 5:1000 6:1000 7:1000 13:1000 14:1000 21:1000 22:1000 ",
	},

	-- old type cavern ngb_min = 2, ngb_max = 3, connchance = 50 permil
	old23_50 = {
		desc = "3:1000 5:1000 6:1000 7:1000 9:50 10:50 11:50 13:1000 14:1000 " ..
		       "15:1000 17:50 18:50 19:50 21:1000 22:1000 23:1000 25:50 26:50 " ..
		       "27:50 29:1000 30:1000 34:50 35:50 37:50 38:50 39:50 41:50 " ..
		       "42:50 43:50 45:50 46:50 51:50 53:1000 54:1000 57:50 58:50 " ..
		       "85:1000 86:1000 90:50 102:50 106:50 170:50 ",
	},

	-- old type cavern ngb_min = 2, ngb_max = 4, connchance = 200 permil
	old24_200 = {
		desc = "3:1000 5:1000 6:1000 7:1000 9:200 10:200 11:200 13:1000 " ..
		       "14:1000 15:1000 17:200 18:200 19:200 21:1000 22:1000 23:1000 " ..
		       "25:200 26:200 27:200 29:1000 30:1000 34:200 35:200 37:200 " ..
		       "38:200 39:200 41:200 42:200 43:200 45:200 46:200 51:200 " ..
		       "53:1000 54:1000 57:200 58:200 85:1000 86:1000 90:200 102:200 " ..
		       "106:200 170:200 ",
	},

	-- old type cavern ngb_min = 3, ngb_max = 4, connchance = 0 permil
	old34_0 = {
		desc = "7:1000 13:1000 14:1000 15:1000 21:1000 22:1000 23:1000 29:1000 " ..
		       "30:1000 53:1000 54:1000 85:1000 86:1000 ",
	},

	-- old type cavern ngb_min = 3, ngb_max = 4, connchance = 10 permil
	old34_10 = {
		desc = "7:1000 11:10 13:1000 14:1000 15:1000 19:10 21:1000 22:1000 " ..
		       "23:1000 25:10 26:10 27:10 29:1000 30:1000 35:10 37:10 38:10 " ..
		       "39:10 41:10 42:10 43:10 45:10 46:10 51:10 53:1000 54:1000 " ..
		       "57:10 58:10 85:1000 86:1000 90:10 102:10 106:10 170:10 ",
	},

	-- old type cavern ngb_min = 3, ngb_max = 5, connchance = 0 permil
	old35_0 = {
		desc = "7:1000 13:1000 14:1000 15:1000 21:1000 22:1000 23:1000 29:1000 " ..
		       "30:1000 31:1000 53:1000 54:1000 55:1000 61:1000 62:1000 " ..
		       "85:1000 86:1000 87:1000 91:1000 94:1000 103:1000 ",
	},

	-- old type cavern ngb_min = 3, ngb_max = 8, connchance = 0 permil
	old38_0 = {
		desc = "7:1000 13:1000 14:1000 15:1000 21:1000 22:1000 23:1000 29:1000 " ..
		       "30:1000 31:1000 53:1000 54:1000 55:1000 61:1000 62:1000 " ..
		       "63:1000 85:1000 86:1000 87:1000 91:1000 94:1000 95:1000 " ..
		       "103:1000 111:1000 119:1000 123:1000 126:1000 127:1000 191:1000 " ..
		       "255:1000 ",
	},

	-- old type cavern ngb_min = 3, ngb_max = 8, connchance = 2 permil
	old38_2 = {
		desc = "7:1000 11:2 13:1000 14:1000 15:1000 19:2 21:1000 22:1000 " ..
		       "23:1000 25:2 26:2 27:2 29:1000 30:1000 31:1000 35:2 37:2 38:2 " ..
		       "39:2 41:2 42:2 43:2 45:2 46:2 47:2 51:2 53:1000 54:1000 " ..
		       "55:1000 57:2 58:2 59:2 61:1000 62:1000 63:1000 85:1000 86:1000 " ..
		       "87:1000 90:2 91:1000 94:1000 95:1000 102:2 103:1000 106:2 " ..
		       "107:2 110:2 111:1000 119:1000 122:2 123:1000 126:1000 127:1000 " ..
		       "170:2 171:2 175:2 187:2 191:1000 255:1000 ",
	},
}

-- An example of the parameter form described above: a cavern of the "old"
-- kind that Kusigrosz did not ship a name for - between two and five floor
-- neighbours, and a one-in-ten chance of a passage that closes a loop.
-- Delete it freely; it is here to show the shape.
DELVE_PATTERNS.open_cavern = { ngb = { 2, 5 }, conmil = 100 }
