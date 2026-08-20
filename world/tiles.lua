------------------------------- THE TILES ---------------------------------
--
-- What every kind of ground looks like and how it behaves. The engine
-- keeps no tiles of its own: it knows only that a tile has a character,
-- a colour, a name, and how hard it is to cross and to see through.
--
--         type                     char  colour                    name                 movability             visibility

DefineTile(XTileType.UNKNOWN,           ' ',  xColor.xBLACK,           "unknown",           Movability.UNKNOWN,    Visibility.UNKNOWN)
DefineTile(XTileType.GREEN_GRASS,       '.',  xColor.xGREEN,           "green grass",       Movability.NORMAL,     Visibility.NORMAL)
DefineTile(XTileType.TREE,              'T',  xColor.xGREEN,           "large tree",        Movability.HARD,       Visibility.HARD)
DefineTile(XTileType.SAND,              '.',  xColor.xYELLOW,          "sand",              Movability.SHARD,      Visibility.NORMAL)
DefineTile(XTileType.WINDOW,            '#',  xColor.xCYAN,            "window",            Movability.WALL,       Visibility.NORMAL)
DefineTile(XTileType.MAGMA,             '#',  xColor.xDARKGRAY,        "magma",             Movability.WALL,       Visibility.WALL)
DefineTile(XTileType.QUARTZ,            '#',  xColor.xLIGHTGRAY,       "quartz",            Movability.WALL,       Visibility.WALL)
DefineTile(XTileType.CAVE_FLOOR,        '.',  xColor.xLIGHTGRAY,       "cave floor",        Movability.NORMAL,     Visibility.NORMAL)
DefineTile(XTileType.STONE_FLOOR,       '.',  xColor.xLIGHTGRAY,       "stone floor",       Movability.NORMAL,     Visibility.NORMAL)
DefineTile(XTileType.PATH,              '.',  xColor.xBROWN,           "path",              Movability.NORMAL,     Visibility.NORMAL)
DefineTile(XTileType.WOOD_WALL,         '#',  xColor.xBROWN,           "wooden wall",       Movability.WALL,       Visibility.WALL)
DefineTile(XTileType.STONE_WALL,        '#',  xColor.xLIGHTGRAY,       "stone wall",        Movability.WALL,       Visibility.WALL)
DefineTile(XTileType.WATER,             '=',  xColor.xLIGHTBLUE,       "water",             Movability.WATER,      Visibility.NORMAL)
DefineTile(XTileType.DEEP_WATER,        '=',  xColor.xBLUE,            "deep water",        Movability.DEEPWATER,  Visibility.NORMAL)
DefineTile(XTileType.LAVA,              '=',  xColor.xRED,             "lava",              Movability.WATER,      Visibility.NORMAL)
DefineTile(XTileType.HILL,              '^',  xColor.xGREEN,           "hill",              Movability.NORMAL,     Visibility.NORMAL)
DefineTile(XTileType.LOW_MOUNTAIN,      '^',  xColor.xBROWN,           "low mountains",     Movability.VHARD,      Visibility.AHARD)
DefineTile(XTileType.MOUNTAIN,          '^',  xColor.xLIGHTGRAY,       "mountains",         Movability.MOUNTAIN,   Visibility.HARD)
DefineTile(XTileType.HIGH_MOUNTAIN,     '^',  xColor.xWHITE,           "high mountains",    Movability.WALL,       Visibility.VHARD)
DefineTile(XTileType.BRIDGE,            '=',  xColor.xBROWN,           "bridge",            Movability.NORMAL,     Visibility.NORMAL)
DefineTile(XTileType.ROAD,              '.',  xColor.xYELLOW,          "road",              Movability.NORMAL,     Visibility.NORMAL)
DefineTile(XTileType.OBSIDIAN_FLOOR,    '.',  xColor.xDARKGRAY,        "obsidian floor",    Movability.NORMAL,     Visibility.NORMAL)
DefineTile(XTileType.FENCE,             'X',  xColor.xBROWN,           "fence",             Movability.WALL,       Visibility.NORMAL)
DefineTile(XTileType.GOLDEN_FLOOR,      '.',  xColor.xYELLOW,          "golden floor",      Movability.NORMAL,     Visibility.NORMAL)
DefineTile(XTileType.MARBLE_WALL,       '#',  xColor.xWHITE,           "marble wall",       Movability.WALL,       Visibility.WALL)
DefineTile(XTileType.BLACK_MARBLE_WALL, '#',  xColor.xDARKGRAY,        "black marble wall", Movability.WALL,       Visibility.WALL)
DefineTile(XTileType.GOLDEN_FENCE,      'X',  xColor.xYELLOW,          "golden fence",      Movability.WALL,       Visibility.NORMAL)
DefineTile(XTileType.TELEPORT_WHITE,    '0',  xColor.xWHITE,           "teleport circle",   Movability.NORMAL,     Visibility.NORMAL)
