-- What the shops of this world are built of.
local SHOP = { wall = XTileType.STONE_WALL, floor = XTileType.STONE_FLOOR }

-- What the caves and dungeons of this world are cut out of.
local CAVE = { wall = XTileType.MAGMA, floor = XTileType.CAVE_FLOOR }


-- Scatters herb bushes across the current location.
--
local HERB_BUSH_ODDS = 18
local HERB_BUSH_TERRAIN = XTileType.GREEN_GRASS
local HERB_BUSH_CLASS = "XHerbBush"

function ScatterHerbBushes()
	local w, h = GetMapSize()

	for y = 0, h - 1 do
		for x = 0, w - 1 do
			if Rand(HERB_BUSH_ODDS) == 0
				and GetTile(x, y) == HERB_BUSH_TERRAIN
				and not HasSpecial(x, y) then
				PlaceSpecial(HERB_BUSH_CLASS, x, y)
			end
		end
	end
end

-- The orc war party musters in the southern hills and, after a long
-- while, marches on the small town (the pattern drawn at 10,40 below).
local ORC_WAR_PARTY = "orcs_war_party"
local ORC_MUSTER_AREA = {x = 10, y = 70, w = 20, h = 10}
local ORC_TARGET_AREA = {x = 20, y = 42, w = 8, h = 6}
local ORC_ATTACK_DELAY = 10000 * 1000

-- Location timer event: re-guards every orc still alive onto the town.
-- Returns 0 so the scheduler drops the timer afterwards - the march
-- happens once, exactly as the old generator removed itself after its
-- one strike.
function OrcWarPartyAttack(l)
	for _, orc in ipairs(FindCreatures("MAIN", ORC_WAR_PARTY)) do
		AsCreature(orc).xai:SetGuardArea(ORC_TARGET_AREA.x, ORC_TARGET_AREA.y,
			ORC_TARGET_AREA.w, ORC_TARGET_AREA.h, "MAIN")
	end

	return 0
end

function MakeAvanorValley()
	CreateLocation("MAIN", "Valley", "Valley of Avanor", XLocation.PLAIN, {
		ground = XTileType.GREEN_GRASS,
		cover = XTileType.TREE,
		-- The border, from the inside out: hills give way to foothills,
		-- then mountains, then the impassable peaks at the map's edge.
		slope = { XTileType.HILL, XTileType.LOW_MOUNTAIN, XTileType.MOUNTAIN, XTileType.HIGH_MOUNTAIN },
		width = 200, height = 90,
	})

	-- The hero starts on the green outside the village.
	SetStartLocation("MAIN", 26, 4, 6, 5)

	-- Dungeon monsters stay in their dungeons: only creatures with
	-- ALLOW_MOVE_OUT (the hero's dog, escorts) follow a stairway up here.
	SetWanderingAllowed("MAIN", false)

	-- Orc war party and the village<->town teleports.
	-- GuardianClass() picks a random ORC-class monster per spawn
	-- (7 templates - orc, large orc, hill orc, ...).
	for i = 1, 20 do
		GuardianClass(CreatureClass.ORC, ORC_WAR_PARTY, ORC_MUSTER_AREA.x, ORC_MUSTER_AREA.y,
			ORC_MUSTER_AREA.w, ORC_MUSTER_AREA.h, XStandardAI.GUARD_AREA)
	end

	CreateTimerEvent('OrcWarPartyAttack', ORC_ATTACK_DELAY)

	Teleport(23, 20, "MAIN", 154, 13)
	Teleport(154, 13, "MAIN", 23, 20)

-- SMALL VILLAGE --
		SetPattern(55, 30,
		"                                                       " ..
		"                        =======                        " ..
		"       P  @@@@@@@  @@@@@@      ==        9             " ..
		"        ..@;;B;;@..@....@........=                     " ..
		"        ..@@@/@@@..@+@@@@.........=                    " ..
		"        .....1......1.............=                    " ..
		"   >111111111111111111111111111...=F                   " ..
		"       .#####/####1...####....11..=                    " ..
		"       .#S.......#1##+#;A#.....111211111               " ..
		"       .#........#1#;E;;;#........=     11111111111111 " ..
		"       .##########1#######.......==       ##;##        " ..
		"     .............1.............===       #Y;;;        " ..
		"     ...@@@@@@@@@@1....... ==========     #;;.#        " ..
		"     ...@......+.+111111==============    ;####        " ..
		"     ...@.....@@@@.====2===============                " ..
		"     ...@@@@@@@. ======2==================             " ..
		"     ........==========2====================           " ..
		"        ===============2======================         " ..
		"        ===============2=======================        " ..
		"      ================222======================        " ..
		"        ==============222====================          " ..
		"     =================222==================            " ..
		"      ==================================               " ..
		"       ===================================             " ..
		"    *   ================================               " ..
		"         ============================                  " ..
		"        ==============================                 " ..
		"          ==========================                   " ..
		"             =======================                   " ..
		"                   ============                        " ..
		"                                                       ")
		AddTranslation("@", XTileType.WOOD_WALL)
		AddTranslation("1", XTileType.ROAD)
		AddTranslation("2", XTileType.BRIDGE)
		AddTranslation("3", XTileType.SAND)
		AddTranslation("A", function(x, y) Furniture(x, y, xColor.xBROWN, '~', 'plain bed') end)
		AddTranslation("S", function(x, y) BuildShop(x, y, 8, 2, ItemKind.FOOD, 'Nobel, the human shopkeeper', SHOP) end)
		AddTranslation("P", function(x, y)
			-- Single source of truth for the village's guard area - also
			-- used to re-home Giana here once rescued (see GianaHandler,
			-- world/uniques/rotmoth.lua), so she settles into exactly the
			-- same area the farmers.
			VILLAGE_GUARD_AREA = {x = x, y = y, w = 20, h = 16}
			local area = VILLAGE_GUARD_AREA

			for i = 1, 4 do
				SetEventHandler(Guardian('farmer', "small_village_farmer", area.x, area.y, area.w, area.h), 'FarmerHandler')
				SetEventHandler(Guardian('goodwife', "small_village_farmer", area.x, area.y, area.w, area.h), 'FarmerHandler')
			end
		end)
		AddTranslation("B", function(x, y) CreateBrida(x, y) end)
		AddTranslation("E", function(x, y) CreateElderGridor(x, y) end)
		AddTranslation("Y", function(x, y) CreateJorgus(x, y) end)
		AddTranslation("F", function(x, y) for i = 1, 5 do CreateBandit(x, y) end end)
		AddTranslation(">", function(x, y) Way(XStairWay.DOWN, "MUSHROOMS_CAVE1", x, y) end)
		AddTranslation("*", function(x, y) Way(XStairWay.DOWN, "DWARFCITYCAVE1", x, y) end)
		AddTranslation("9", function(x, y) Way(XStairWay.DOWN, "SMALL_CAVE_1", x, y) end)
		DrawPattern(0, 0)

-- SMALL TOWN --
		SetPattern(46, 15,
		".............................................." ..
		".###########.###########.#####.#####.#####...." ..
		".#B;;;A#;>;#.#B;;;;;;;A#.#A;A#.#A;A#.#A;A#...." ..
		".##+#####+##.##+########.##+##.##+##.##+##...." ..
		".11111G1111111111111111111111111111111111111.." ..
		".##+##F.....11&=&11.###########1#####+######.." ..
		".#B;A#......11=&=11.#B;B;B;B;B#1#B;+;;;#B;A#.." ..
		".#####......11&=&111+;;;;;;;;;+1+;;#;B;+;;;#.." ..
		"1111111111111111111.#B;B;B;B;B#1########+###.." ..
		".#####/#####.##/##1.###########1.......#;;A#.." ..
		".#S........#.#E;;#11111111111111.......#####.." ..
		".#.........#.#C;D#............................" ..
		".#.........#.#####............................" ..
		".###########.................................." ..
		"..............................................")

		AddTranslation("1", XTileType.ROAD)
		AddTranslation("A", function(x, y) Furniture(x, y, xColor.xBROWN, '~', 'plain bed') end)
		AddTranslation("B", function(x, y) Furniture(x, y, xColor.xBROWN, '~', 'table') end)
		AddTranslation("C", function(x, y) Furniture(y, y, xColor.xBROWN, '~', 'desk') end)
		AddTranslation("D", function(x, y) Furniture(x, y, xColor.xLIGHTRED, '~', 'bed') end)
		AddTranslation("E", function(x, y) CreateOzorik(x, y) end)
		AddTranslation("F", function(x, y) CreateGuardians(x, y) end)
		AddTranslation("G", function(x, y) CreateGekta(x, y) end)
		AddTranslation("S", function(x, y) BuildShop(x, y, 9, 3, ItemKind.ARMOUR + ItemKind.WEAPON + ItemKind.MISSILE + ItemKind.MISSILEW, 'Noberik, the human shopkeeper', SHOP) end)
		AddTranslation(">", function(x, y) Way(XStairWay.DOWN, "RATCELLAR", x, y) end)
		DrawPattern(10, 40)



-- LARGE CITY --
		SetPattern(68, 30,
		"  =   ###C;;;;;;;;;;;;;;;;;;;;;;.MM  ##2##.....................##2##" ..
		"  ==  #;#.;#####;...WWWWWWWWW..;;.MM #;;;#......#########......#;;;#" ..
		"  ==  #;#.;#K;N#;...WE;;_;;;W...;..MM2;;;########;;WRW;;########;;;2" ..
		"  ==  #;#.;#;;;+;...WWFF;FFWW...;...M#;;;+;;;;#;#;;;3;;;+;;;;#;+;;;#" ..
		"  ==  #;#.;#####;....WFF;FFW...;;...M###+####;+;##G;3;G##;;Q;#;#+###" ..
		"  ==  #;#.;#N;N#;###.WW;;;WW.;;;....M..#;#;;#;#;;#G;3;G#;;;;;#;#;#.." ..
		"  ==  #;#.;#;;;#;#;##.WW/WW.;;......M..#;#;;#;#####;3;########;#;#.." ..
		"   =  #;#.;#;K;+;#;;##.;;;;;;WWWWWWWM..#;#O;+;;;;;+;3;+;;;;;;;;#;#.." ..
		"   =  #;#.;#;;;#;#K;;#.;;;...W;W===WMM.#;#;;#;######+#######+###;#.." ..
		"   = ##+##;#####;##+##;;;;;..W;W=XXWWM.#;#;;#;#.....;.....#;;;;#;#.." ..
		";;;1;#;;;#;;;;;;;;;;;;;=;=;;;W;W=XB3WM.#;####;#....;;;....#;;U;#;#.." ..
		";;;1;+;;;+;;;;;;;;;;;;;;;;;;;/33333AWM.#;#;;+;+;;;;;=;;;;;+;;;;#;#.." ..
		";;;1;#;;;#;;;;;;;;;;;;;=;=;;;W;W=X33WM.#;#G;###....;;;....###+##;#.." ..
		"   = ##+##;#####;##+##;;;;;..W;W=XXWWM.#;#G;#H#.....;.....#;;;;#;#.." ..
		"   =  #;#.;#N;;#;#;;;##;;;...W;W===WMM.#;#;;+;#..###+###..#;;;;#;#.." ..
		"  ==  #;#.;#;;;#;#N;;N#;;;...WWWWWWWM###+#########;;3;;#########+###" ..
		"  ==  #;#.;#K;;+;######;;;..........M#;;;+;;;;;;;+;;3;;+;;;;;;;+;;;#" ..
		"  ==  #;#.;#####;#S;;;#;;;#########MM2;;;#########;;3;;#########;;;2" ..
		"  ==  #;#.;#N;N#;#;;;;#;;;#;;#N;;N#M.#;;;#.......###+###T.T.T.T#;;;#" ..
		"   == #;#.;#;;;#;#;;;;/;;;+;K#;;;;#MM##2##........M;;;MMMMM.TT.##2##" ..
		"   == #;#.;#;;K#;#;;;;#;;;######+##.MMMMMMMMMMMMMMM;;;####MMMM.TT.T." ..
		"   == #;#.;##+##;######;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;+;;####MMM.T.T" ..
		"   ==##+##;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;.#;;+;;#..MMMMM" ..
		"   =##;;;##...........##+##...........................#;;#;;#......." ..
		"  ==#;;;;;#############;;;#############################+############" ..
		"  ==2;;;;;+;;;;;;;;;;;+;;;+;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;#" ..
		"  ==#;;;;;#############;;;##########################################" ..
		"   =##;;;##    =======##+##               ====     ========         " ..
		"   ==##2##  ===========111====         =============================" ..
		"    ==========         ;;;=================    ==           =====   ")

		AddTranslation("1", XTileType.BRIDGE)
		AddTranslation("2", XTileType.WINDOW)
		AddTranslation("3", XTileType.GOLDEN_FLOOR)
		AddTranslation("M", XTileType.MOUNTAIN)
		AddTranslation("W", XTileType.MARBLE_WALL)
		AddTranslation("X", XTileType.GOLDEN_FENCE)

		AddTranslation("A", function(x, y) Furniture(x, y, xColor.xLIGHTRED, '~', 'eternal flame') end)
		AddTranslation("B", function(x, y) CreateGefeon(x, y) end)
		AddTranslation("C", function(x, y) for i = 1, 5 do Guardian('citizen', "roderick_guardian", x, y, 30, 25) Guardian('fcitizen', "roderick_guardian", x, y, 30, 25) end end)
		AddTranslation("_", function(x, y) SetView(PlaceSpecial("XAltar", x, y), "_", xColor.xWHITE) end)
		AddTranslation("E", function(x, y) CreateHighPriest(x, y) end)
		AddTranslation("F", function(x, y) Furniture(x, y, xColor.xBROWN, '~', 'pew') end)

		AddTranslation("R", function(x, y) CreateRoderik(x, y) Furniture(x, y, xColor.xYELLOW, '~', 'the throne of Avanor') end)
		AddTranslation("G", function(x, y) Guardian('royal_guard', "roderick_guardian", x, y) end)
		AddTranslation("H", function(x, y) Guardian('royal_guard', "roderick_guardian", x, y, 1, 1, XStandardAI.NO_SWAP) Way(XStairWay.DOWN, "KINGS_TREASURE", x, y) end)

		AddTranslation("Q", function(x, y) Furniture(x, y, xColor.xLIGHTRED, '~', 'royal bed') end)
		AddTranslation("O", function(x, y) Furniture(x, y, xColor.xBROWN, '~', 'dinner table') end)
		AddTranslation("U", function(x, y) Furniture(x, y, xColor.xBROWN, '~', 'round table') end)

		AddTranslation("K", function(x, y) Furniture(x, y, xColor.xBROWN, '~', 'table') end)
		AddTranslation("N", function(x, y) Furniture(x, y, xColor.xBROWN, '~', 'bed') end)


		AddTranslation("S", function(x, y) BuildShop(x, y, 4, 4, ItemKind.BOOK + ItemKind.SCROLL + ItemKind.POTION, 'Toberik, the human shopkeeper', SHOP) end)
		DrawPattern(129, 2)


-- EXTINCT VOLCANO --
		SetPattern(16, 7,
		"      :    ,,,  " ..
		"  :: :;:  ,:::, " ..
		"    :;^;:,  ::, " ..
		"   :;^>^;:,,,,  " ..
		" ,,,:;^::  :::::" ..
		"   ,,:;:::::    " ..
		"      :,,,      " )
		AddTranslation(",", XTileType.HILL)
		AddTranslation(":", XTileType.LOW_MOUNTAIN)
		AddTranslation(";", XTileType.MOUNTAIN)
		AddTranslation("^", XTileType.HIGH_MOUNTAIN)
		AddTranslation(">", function(x, y) Way(XStairWay.DOWN, "EXTINCT_VOLCANO", x, y) end)
		DrawPattern(46, 75)


-- UNDEAD's THOMB --
		SetPattern(22, 9,
		"              ########" ..
		"    ###########,,,,,,#" ..
		"    #U,,,,,,,,,,,,,,##" ..
		"#######,,,,,,,,,,##,# " ..
		"+,,+,,+,,,,,,,,,,#>,# " ..
		"#######,,,,,,,,,,##,# " ..
		"    #,,,,,,,,,,,,,,,##" ..
		"    ###########,,,,,,#" ..
		"              ########")
		AddTranslation(".", XTileType.CAVE_FLOOR)
		AddTranslation(">", function(x, y) Way(XStairWay.DOWN, "UNDEADS_TOMB1", x, y) end)
		AddTranslation("U", function(x, y) for i = 1, 20 do Creature('skeleton', x, y, 14, 4) end end)
		DrawPattern(100, 50)



-- BLACK TOWER RUINS --
		SetPattern(21, 11,
		"....X..XX.XX..XX....X" ..
		"X...................." ..
		"........##.##.......X" ..
		"........;..;;.#......" ..
		".....#;;;;>...;#....." ..
		".....#;...;;;;;#....." ..
		"X......#;;..;........" ..
		"........../##.......X" ..
		"X...................." ..
		"X...................X" ..
		"....XX..X...X.....X..")
		AddTranslation("#", XTileType.BLACK_MARBLE_WALL)
		AddTranslation(">", function(x, y) Way(XStairWay.DOWN, "WIZARD_DUNGEON1", x, y) end)
		DrawPattern(155, 44)



-- YOHJI's TOWER --
		SetPattern(21, 11,
		"XXXXXXXXXXXXXXXXXXXXX" ..
		"X...................X" ..
		"X......#######......X" ..
		"X.....##22#22##.....X" ..
		"X....##222#222##....X" ..
		"X....#22#####22#....X" ..
		"X....##2/2<2/2##....X" ..
		"X.....###222###.....X" ..
		"X......###/###......X" ..
		"X........111........X" ..
		"XXXXXXXXX111XXXXXXXXX")
		AddTranslation("1", XTileType.ROAD)
		AddTranslation("2", XTileType.GOLDEN_FLOOR)
		AddTranslation("#", XTileType.MARBLE_WALL)
		AddTranslation("<", function(x, y) Way(XStairWay.UP, "WIZTOWER_TOP", x, y) end)
		DrawPattern(45, 25)

	-- Last thing done to the Valley itself, after every pattern is
	-- stamped onto it - restored alongside the orc war party/teleports
	-- above, see the comment there. Runs last so bushes don't end up
	-- under a village/town/city wall drawn afterward.
	ScatterHerbBushes()

-- KING'S TREASURE --
	CreateLocation("KINGS_TREASURE", "RoyalTr", "Royal Treasure", XLocation.PLAIN, {
		ground = XTileType.GREEN_GRASS,
		cover = XTileType.TREE,
		-- The border, from the inside out: hills give way to foothills,
		-- then mountains, then the impassable peaks at the map's edge.
		slope = { XTileType.HILL, XTileType.LOW_MOUNTAIN, XTileType.MOUNTAIN, XTileType.HIGH_MOUNTAIN },
		width = 200, height = 90,
	})
		SetPattern(21, 10,
		"#####################" ..
		"#######[~~~~[########" ..
		"#####$$$$$$$$$$######" ..
		"##$$$$$$$$$$$$$$$$$##" ..
		"##$$$$$$$$$$$$$$$$$##" ..
		"####$$$$$$$$$$$$$####" ..
		"#######$$$$$$$#######" ..
		"##########^##########" ..
		"##########+##########" ..
		"#########,<,#########" ..
		"#####################" )

		AddTranslation("[", function(x, y) DropItem(CreateObject('XAncientMachinePart'), x, y) end)
		AddTranslation("$", function(x, y) Treasure(x, y, 250) end)
		AddTranslation("~", function(x, y) Chest(x, y) end)
		AddTranslation("<", function(x, y) Way(XStairWay.UP, "MAIN", x, y) end)


-- YOHJI's TOWER SECOND FLOOR--
	CreateLocation("WIZTOWER_TOP", "WzTwr", "Yohjishiro's Tower", XLocation.PLAIN, {
		ground = XTileType.GREEN_GRASS,
		cover = XTileType.TREE,
		-- The border, from the inside out: hills give way to foothills,
		-- then mountains, then the impassable peaks at the map's edge.
		slope = { XTileType.HILL, XTileType.LOW_MOUNTAIN, XTileType.MOUNTAIN, XTileType.HIGH_MOUNTAIN },
		width = 200, height = 90,
	})
		SetPattern(21, 11,
		"XXXXXXXXXXXXXXXXXXXXX" ..
		"X......##333##......X" ..
		"X.....#3W22223#.....X" ..
		"X....##2222222##....X" ..
		"X...#####222#####...X" ..
		"X...3B22#222#2223...X" ..
		"X...##22/2>2/22##...X" ..
		"X....3#2#222#2#3....X" ..
		"X.....###222###.....X" ..
		"X......##333##......X" ..
		"XXXXXXXXX111XXXXXXXXX")
		AddTranslation("1", XTileType.ROAD)
		AddTranslation("2", XTileType.GOLDEN_FLOOR)
		AddTranslation("3", XTileType.WINDOW)
		AddTranslation("#", XTileType.MARBLE_WALL)
		AddTranslation(">", function(x, y) Way(XStairWay.DOWN, "MAIN", x, y) end)
		AddTranslation("W", function(x, y) CreateYohji(x, y) end)
		AddTranslation("B", function(x, y) Furniture(x, y, xColor.xLIGHTRED, '~', 'nice bed') end)
		DrawPattern(45, 25)


-- UNDEAD's THOMB Underground --
	CreateLocation("UNDEADS_TOMB1", "Tomb", "Tomb", XLocation.CAVE, CAVE)
		SetPattern(80, 20,
		"################################################################################" ..
		"################################################################################" ..
		"################################################################################" ..
		"###################################################A,,,,,,,,####################" ..
		"###################################################,,,,,,,,,+,,,,,,,,,,,########" ..
		"#############################,,,,,,################B,,,,,,,,###########,########" ..
		"#########################,,,,,,,,,,####################+###############,########" ..
		"#######################,,,,,,,,,,,,##########,,,,,,####,####,,,,,,#####,########" ..
		"###,,,,,##############,,,,,,,,,,,,,#########,,,,,,,,###,###,,,,,,,,####,########" ..
		"###,,,,,##############,,,,,,,,,,,,,########,,######,,##+##,,######,,###+########" ..
		"###,,<,,+,,,,,,,,,,,,+,,,,,,,,,,,,,+,,,,,,+,########,,+,+,,########,+,+,########" ..
		"###,,,,,##############,,,,,,,,,,,,,########,,######,,##+##,,######,,###+########" ..
		"###,,,,,##############,,,,,,,,,,,,,#########,,,,,,,,###,###,,,,,,,,####,########" ..
		"#######################,,,,,,,,,,,,##########,,,,,,####,####,,,,,,#####,########" ..
		"#########################,,,,,,,,,,####################+###############,########" ..
		"#############################,,,,,,################C,,,,,,,,###########,########" ..
		"###################################################,,,,,,,,,+,,,,,,,,,,,########" ..
		"###################################################D,,,,,,,,####################" ..
		"################################################################################" ..
		"################################################################################")
		AddTranslation("<", function(x, y) Way(XStairWay.UP, "MAIN", x, y) end)
		AddTranslation("A", function(x, y) Grave(x, y, 'Frederick III (760-805)', 'StandardGraveEvent') end)
		AddTranslation("B", function(x, y) Grave(x, y, 'Frederick II (710-790)', 'StandardGraveEvent') end)
		AddTranslation("C", function(x, y) Grave(x, y, 'Arthur IV (796-854)', 'StandardGraveEvent') end)
		AddTranslation("D", function(x, y) Grave(x, y, 'Rodomir III (821-894)', 'AvanorDefenderGraveEvent') end)

		DrawPattern(0, 0)
		for i = 1, 20 do Creature("ghost") end
		for i = 1, 10 do
			Creature("spectre")
			Creature("dread")
		end
end
