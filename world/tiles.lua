------------------------------- THE TILES ---------------------------------
--
-- Every kind of ground the game knows. The engine ships none: it knows
-- only that a tile has a name, a character, a colour, and how hard it is
-- to cross and to see through.
--
-- A last table may carry what else is true of it: `fertile` for ground
-- things grow on, `diggable_into` for what a pickaxe leaves behind.
--
-- The order matters twice over. The first tile defined is the one the
-- engine falls back on for "nothing here", and the ids handed out here
-- are what a saved game stores - a save records the names alongside them,
-- so reordering this file will not corrupt one, but do keep UNKNOWN
-- first.
--
--         id                    name                  char  colour                  movability             visibility

DefineTile("UNKNOWN",           "unknown",           ' ',  xColor.xBLACK,     Movability.UNKNOWN,   Visibility.UNKNOWN)
DefineTile("GREEN_GRASS",       "green grass",       '.',  xColor.xGREEN,     Movability.NORMAL,    Visibility.NORMAL,  { fertile = true })
DefineTile("TREE",              "large tree",        'T',  xColor.xGREEN,     Movability.HARD,      Visibility.HARD)
DefineTile("SAND",              "sand",              '.',  xColor.xYELLOW,    Movability.SHARD,     Visibility.NORMAL)
DefineTile("WINDOW",            "window",            '#',  xColor.xCYAN,      Movability.WALL,      Visibility.NORMAL)
DefineTile("MAGMA",             "magma",             '#',  xColor.xDARKGRAY,  Movability.WALL,      Visibility.WALL,    { diggable_into = "STONE_FLOOR" })
DefineTile("QUARTZ",            "quartz",            '#',  xColor.xLIGHTGRAY, Movability.WALL,      Visibility.WALL)
DefineTile("CAVE_FLOOR",        "cave floor",        '.',  xColor.xLIGHTGRAY, Movability.NORMAL,    Visibility.NORMAL)
DefineTile("STONE_FLOOR",       "stone floor",       '.',  xColor.xLIGHTGRAY, Movability.NORMAL,    Visibility.NORMAL)
DefineTile("PATH",              "path",              '.',  xColor.xBROWN,     Movability.NORMAL,    Visibility.NORMAL)
DefineTile("WOOD_WALL",         "wooden wall",       '#',  xColor.xBROWN,     Movability.WALL,      Visibility.WALL)
DefineTile("STONE_WALL",        "stone wall",        '#',  xColor.xLIGHTGRAY, Movability.WALL,      Visibility.WALL,    { diggable_into = "STONE_FLOOR" })
DefineTile("WATER",             "water",             '=',  xColor.xLIGHTBLUE, Movability.WATER,     Visibility.NORMAL)
DefineTile("DEEP_WATER",        "deep water",        '=',  xColor.xBLUE,      Movability.DEEPWATER, Visibility.NORMAL)
DefineTile("LAVA",              "lava",              '=',  xColor.xRED,       Movability.WATER,     Visibility.NORMAL)
DefineTile("HILL",              "hill",              '^',  xColor.xGREEN,     Movability.NORMAL,    Visibility.NORMAL)
DefineTile("LOW_MOUNTAIN",      "low mountains",     '^',  xColor.xBROWN,     Movability.VHARD,     Visibility.AHARD)
DefineTile("MOUNTAIN",          "mountains",         '^',  xColor.xLIGHTGRAY, Movability.MOUNTAIN,  Visibility.HARD)
DefineTile("HIGH_MOUNTAIN",     "high mountains",    '^',  xColor.xWHITE,     Movability.WALL,      Visibility.VHARD)
DefineTile("BRIDGE",            "bridge",            '=',  xColor.xBROWN,     Movability.NORMAL,    Visibility.NORMAL)
DefineTile("ROAD",              "road",              '.',  xColor.xYELLOW,    Movability.NORMAL,    Visibility.NORMAL)
DefineTile("OBSIDIAN_FLOOR",    "obsidian floor",    '.',  xColor.xDARKGRAY,  Movability.NORMAL,    Visibility.NORMAL)
DefineTile("FENCE",             "fence",             'X',  xColor.xBROWN,     Movability.WALL,      Visibility.NORMAL)
DefineTile("GOLDEN_FLOOR",      "golden floor",      '.',  xColor.xYELLOW,    Movability.NORMAL,    Visibility.NORMAL)
DefineTile("MARBLE_WALL",       "marble wall",       '#',  xColor.xWHITE,     Movability.WALL,      Visibility.WALL)
DefineTile("BLACK_MARBLE_WALL", "black marble wall", '#',  xColor.xDARKGRAY,  Movability.WALL,      Visibility.WALL)
DefineTile("GOLDEN_FENCE",      "golden fence",      'X',  xColor.xYELLOW,    Movability.WALL,      Visibility.NORMAL)
DefineTile("TELEPORT_WHITE",    "teleport circle",   '0',  xColor.xWHITE,     Movability.NORMAL,    Visibility.NORMAL)
