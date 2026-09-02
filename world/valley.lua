
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

-- The group the valley's own people belong to - the farmers and goodwives
-- of the small village, and nobody else. Global rather than local: the
-- mushroom caves need to recognise them when they come down on the Elder's
-- errand (see MushroomCaveEvent), the same way VILLAGE_GUARD_AREA below is
-- shared with the rescued girl's homecoming.
VILLAGE_GROUP = "small_village_farmer"

-- The village's guard area - fixed by the "P" tile's spot in the ASCII
-- pattern below (DrawPattern places it at the map's origin, so this never
-- varies between games) and given here, so this stays set regardless for
-- an new game or a restored one. If the "P" tile in the pattern ever moves,
-- update this to match.
VILLAGE_GUARD_AREA = {x = 7, y = 2, w = 20, h = 16}

-- The orc war party musters in the southern hills and, after a long
-- while, marches on the small town (the pattern drawn at 10,40 below).
--
-- The group id and the size are global rather than local: the world-level
-- death tally (world/tally.lua) has to recognise a war-party orc when one
-- dies, and has to know how many came in order to say so once they are all
-- dead. Change the size here and the achievements line follows.
ORC_WAR_PARTY = "orcs_war_party"
ORC_WAR_PARTY_SIZE = 20

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

	return false
end

function MakeAvanorValley()
	CreateLocation("MAIN", "Valley", "Valley of Avanor", XLocation.PLAIN, PLAIN)

	-- The hero starts on the green outside the village.
	SetStartLocation("MAIN", 26, 4, 6, 5)

	-- Dungeon monsters stay in their dungeons: only creatures with
	-- ALLOW_MOVE_OUT (the hero's dog, escorts) follow a stairway up here.
	SetWanderingAllowed("MAIN", false)

	-- Orcish war party.
	-- GuardianClass() picks a random ORC-class monster per spawn
	-- (7 templates - orc, large orc, hill orc, ...).
	for i = 1, ORC_WAR_PARTY_SIZE do
		GuardianClass(CreatureClass.ORC, ORC_WAR_PARTY, ORC_MUSTER_AREA.x, ORC_MUSTER_AREA.y,
			ORC_MUSTER_AREA.w, ORC_MUSTER_AREA.h, XStandardAI.GUARD_AREA)
	end

	CreateTimerEvent('OrcWarPartyAttack', ORC_ATTACK_DELAY)

	-- The village <-> town teleports.
	Teleport(23, 20, "MAIN", 153, 13)
	Teleport(153, 13, "MAIN", 23, 20)

-- SMALL VILLAGE --
		SetPattern(55, 31,
		"                       =                               " ..
		"                       ========                       9" ..
		"       P  @@@@@@@  @@@@@@G.....==                      " ..
		"        ..@A;B;A@..@...A@......X.=                     " ..
		"        ..@@@/@@@..@+@@@@XXXXXXX..=                    " ..
		"        .....1......1.............=                    " ..
		"   >111111111111111111111111111...=F                   " ..
		"       .#####/####1...####....11..=                    " ..
		"       .#S;;;;;;;#1##+#;A#.....111211111               " ..
		"       .#;;;;;;;;#1#;E;;;#........=     11111111111111 " ..
		"       .##########1#######.......==       ##;##        " ..
		"      ............1.............===       #Y;;;        " ..
		"     ...@@@@@@@@@@1....... ==========     #;;.#        " ..
		"      ..@......+.+111111==============    ;####        " ..
		"      ..@.A.A.@@@@.====2===============                " ..
		"     ...@@@@@@@...=====2==================             " ..
		"      ..........=======2====================           " ..
		"       .===============2======================         " ..
		"       .===============2=======================        " ..
		"      ================222======================        " ..
		"       .==============222====================          " ..
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
			-- Where the farmers themselves guard - also the exact spot
			-- VILLAGE_GUARD_AREA above is fixed to (this is the "P" tile
			-- it's derived from), so this and that constant had better
			-- agree.
			local area = {x = x, y = y, w = 20, h = 16}

			for i = 1, 4 do
				SetEventHandler(Guardian('farmer', VILLAGE_GROUP, area.x, area.y, area.w, area.h), 'FarmerHandler')
				SetEventHandler(Guardian('goodwife', VILLAGE_GROUP, area.x, area.y, area.w, area.h), 'FarmerHandler')
			end
		end)
		AddTranslation("B", function(x, y) CreateBrida(x, y) end)
		AddTranslation("E", function(x, y) CreateElderGridor(x, y) end)
		AddTranslation("X", XTileType.FENCE)
		-- The goats of the fenced pen behind the houses. The marker sits in
		-- its north-west corner and the rect covers the whole enclosure
		-- (x 25-30, y 2-3), so they are scattered through it rather than
		-- stacked on one tile.
		AddTranslation("G", function(x, y) for i = 1, 3 do Creature("goat", x, y, 6, 2) end end)
		AddTranslation("Y", function(x, y) CreateJorgus(x, y) end)
		AddTranslation("F", function(x, y) for i = 1, 5 do CreateBandit(x, y) end end)
		AddTranslation(">", function(x, y) Way(XStairWay.DOWN, "MUSHROOMS_CAVE1", x, y) end)
		AddTranslation("*", function(x, y) Way(XStairWay.DOWN, "DWARFCITYCAVE1", x, y) end)
		AddTranslation("9", function(x, y) Way(XStairWay.DOWN, "SMALL_CAVE_1", x, y) end)
		DrawPattern(0, 0)

		-- The bandits shout across the water as the hero reaches the bridge
		-- (see BanditBridgeEvent). The tiles it covers are named once, in
		-- BANDIT_BRIDGE, because the handler needs them too - it compares the
		-- hero's position against them to tell a crossing from a homecoming.
		EventPlace(BANDIT_BRIDGE.x, BANDIT_BRIDGE.y,
			BANDIT_BRIDGE.w, BANDIT_BRIDGE.h, 'BanditBridgeEvent')

		-- Connect Rotmoth's hideout with Jorgus' HQ
		for _, c in ipairs(WindingRoad(53, 2, 43, 10, 9)) do
			local t = GetTile(c.x, c.y)
			SetTile(c.x, c.y, XTileType.ROAD)
		end


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
		-- min_value keeps the junk off the armourer's floor. GetValue()
		-- weights a point of protection six times as heavily as a point of
		-- defence, so a price floor is the bluntest way to ask for armour
		-- that actually protects: at 20 it takes the protective share of his
		-- stock from 42% to 59%, and it is still low enough that a hero who
		-- has only just arrived can afford part of the shelf. Raising it much
		-- past 25 starts selecting expensive weapons instead and prices the
		-- shop out of the early game entirely.
		AddTranslation("S", function(x, y)
			BuildShop(x, y, 9, 3,
				ItemKind.ARMOUR + ItemKind.WEAPON + ItemKind.MISSILE + ItemKind.MISSILEW,
				'Noberik, the human shopkeeper',
				{ wall = SHOP.wall, floor = SHOP.floor, min_value = 20 })
		end)
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


-- UNDEAD's TOMB --
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


-- THE FORLORN MINE --
--
-- The head-house of a gem mine, far south of the valley, and the apron of
-- spoil and dressed stone the miners left spread around it. The roof is
-- down in two places and there are saplings in what was the yard, but the
-- doorway still stands and so does the shaft inside it. See
-- world/locations/forlorn.lua for what is below.
--
-- It sits where the three random caves of the original game had their
-- entrances - x 115..180, y 60..80 of the plain - which is the ground
-- Roderick means when he says the Eye of Raa was hidden in the caves far
-- south of here.
		SetPattern(21, 11,
		"        ;;;;;        " ..
		"     ;;;;;;;;; ;;    " ..
		"   ;;;;##### ###;;;  " ..
		"  ;;;;;#;;;;;;;#;;;  " ..
		" ;; ;;;+;;>;;;;#;;;; " ..
		"  ;;;;;#;;;;;;;# ;;  " ..
		"   ;;;;###;; ###;;;  " ..
		"    ;;;;;;;;;;;;;;   " ..
		"  &   ;;  ;;;;;   &  " ..
		"       ;;;; ;;       " ..
		"         ;;;         ")
		AddTranslation(">", function(x, y) Way(XStairWay.DOWN, "FORLORN1", x, y) end)
		DrawPattern(140, 66)


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
		"Xq..............q...X" ..
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

		-- Yohjishiro's flock, grazing the grass either side of her
		-- tower. Each mark is the corner of the strip its four sheep
		-- keep to, so they stay inside the fence and out of the tower.
		-- They share her group: harm one and she knows (see YOHJI's
		-- TOWER SECOND FLOOR below, and XStandardAI::SetGroupEnemy).
		AddTranslation("q", function(x, y)
			for i = 1, 4 do
				Guardian('sheep', "yohji_flock", x, y, 4, 8)
			end
		end)
		DrawPattern(45, 25)

	-- The road east out of the village has always stopped dead in open country,
	-- and the city's western gate has always been reached over a bridge with
	-- nothing on the near side of it. Lay the track that ought to have joined
	-- them, wandering the way a road worn by use wanders rather than ruled
	-- straight across the plain. Placed here because it has to come after every
	--  pattern that is stamped onto the Valley.
	for _, c in ipairs(WindingRoad(53, 9, 128, 13, 14)) do
		local t = GetTile(c.x, c.y)

		if (t == XTileType.WATER or t == XTileType.DEEP_WATER) then
			SetTile(c.x, c.y, XTileType.BRIDGE)
		else
			SetTile(c.x, c.y, XTileType.ROAD)
		end
	end

	-- Last thing done to the Valley itself, after every pattern is
	-- stamped onto it - restored alongside the orc war party/teleports
	-- above, see the comment there. Runs last so bushes don't end up
	-- under a village/town/city wall drawn afterward.
	ScatterHerbBushes()

-- KING'S TREASURE --
	CreateLocation("KINGS_TREASURE", "RoyalTr", "Royal Treasure", XLocation.PATTERN, Drawn(21, 11))
		SetPattern(21, 11,
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

		AddTranslation("[", function(x, y) DropItem(CreateObject('ancient_machine_part'), x, y) end)
		AddTranslation("$", function(x, y) Treasure(x, y, 250) end)
		AddTranslation("~", function(x, y) Chest(x, y) end)
		AddTranslation("<", function(x, y) Way(XStairWay.UP, "MAIN", x, y) end)
		DrawPattern(0, 0)


-- YOHJI's TOWER SECOND FLOOR--
	CreateLocation("WIZTOWER_TOP", "WzTwr", "Yohjishiro's Tower", XLocation.PATTERN, Above("MAIN", 45, 25, 21, 11))
		-- Only the tower itself is up here. Every blank is a hole in
		-- this floor: what shows through it is the valley below, drawn
		-- from the same place it is drawn when you walk it.
		SetPattern(21, 11,
		"                     " ..
		"       ##333##       " ..
		"      #3W22223#      " ..
		"     ##2222222##     " ..
		"    #####222#####    " ..
		"    3B22#222#2223    " ..
		"    ##22/2>2/22##    " ..
		"     3#2#222#2#3     " ..
		"      ###222###      " ..
		"       ##333##       " ..
		"                     ")
		AddTranslation("2", XTileType.GOLDEN_FLOOR)
		AddTranslation("3", XTileType.WINDOW)
		AddTranslation("#", XTileType.MARBLE_WALL)
		AddTranslation(">", function(x, y) Way(XStairWay.DOWN, "MAIN", x, y) end)
		AddTranslation("W", function(x, y) CreateYohji(x, y) end)
		AddTranslation("B", function(x, y) Furniture(x, y, xColor.xLIGHTRED, '~', 'nice bed') end)
		DrawPattern(45, 25)


-- UNDEAD's THOMB Underground --
	CreateLocation("UNDEADS_TOMB1", "Tomb", "Tomb", XLocation.PATTERN, Drawn())
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
