
function MakeAvanorValley()
	CreateLocation(XLocation.MAIN, "Valley", "Valley of Avanor", PLAIN)

	-- Orc war party and the village<->town teleports.
	-- GuardianClass() picks a random ORC-class monster per spawn
	-- (7 templates - orc, large orc, hill orc, ...).
	for i = 1, 20 do
		GuardianClass(CreatureClass.ORC, "orcs_war_party", 10, 70, 20, 10, XStandardAI.GUARD_AREA)
	end

	Teleport(23, 20, XLocation.MAIN, 154, 13)
	Teleport(154, 13, XLocation.MAIN, 23, 20)

-- SMALL VILLAGE --
		SetPattern(55, 30,
		"                                                       " ..
		"                        =======                        " ..
		"       P  @@@@@@@  @@@@@@  9   ==                      " ..
		"        ..@;;;;;@..@....@........=                     " ..
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
		AddTranslation("S", function(x, y) BuildShop(x, y, 8, 2, ItemKind.FOOD, 'Nobel, the human shopkeeper') end)
		AddTranslation("P", function(x, y) for i = 1, 4 do SetEventHandler(Guardian('farmer', "small_village_farmer", x, y, 20, 16), 'FarmerHandler') SetEventHandler(Guardian('goodwife', "small_village_farmer", x, y, 20, 16), 'FarmerHandler') end end)
		AddTranslation("E", function(x, y) CreateElderGridor(x, y) end)
		AddTranslation("Y", function(x, y) CreateJorgus(x, y) end)
		AddTranslation("F", function(x, y) for i = 1, 5 do CreateBandit(x, y) end end)
		AddTranslation(">", function(x, y) Way(DOWN, XLocation.MUSHROOMS_CAVE1, x, y) end)
		AddTranslation("*", function(x, y) Way(DOWN, XLocation.DWARFCITYCAVE1, x, y) end)
		AddTranslation("9", function(x, y) Way(DOWN, L_SMALL_CAVE1, x, y) end)
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
		AddTranslation("S", function(x, y) BuildShop(x, y, 9, 3, ItemKind.ARMOUR + ItemKind.WEAPON + ItemKind.MISSILE + ItemKind.MISSILEW, 'Noberik, the human shopkeeper') end)
		AddTranslation(">", function(x, y) Way(DOWN, XLocation.RATCELLAR, x, y) end)
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
		AddTranslation("_", function(x, y) Altar(x, y, XDeity.LIFE) end)
		AddTranslation("E", function(x, y) CreateHighPriest(x, y) end)
		AddTranslation("F", function(x, y) Furniture(x, y, xColor.xBROWN, '~', 'pew') end)
		
		AddTranslation("R", function(x, y) CreateRoderik(x, y) Furniture(x, y, xColor.xYELLOW, '~', 'the throne of Avanor') end)
		AddTranslation("G", function(x, y) Guardian('royal_guard', "roderick_guardian", x, y) end)
		AddTranslation("H", function(x, y) Guardian('royal_guard', "roderick_guardian", x, y, 1, 1, XStandardAI.NO_SWAP) Way(DOWN, XLocation.KINGS_TREASURE, x, y) end)
		
		AddTranslation("Q", function(x, y) Furniture(x, y, xColor.xLIGHTRED, '~', 'royal bed') end)
		AddTranslation("O", function(x, y) Furniture(x, y, xColor.xBROWN, '~', 'dinner table') end)
		AddTranslation("U", function(x, y) Furniture(x, y, xColor.xBROWN, '~', 'round table') end)
		
		AddTranslation("K", function(x, y) Furniture(x, y, xColor.xBROWN, '~', 'table') end)
		AddTranslation("N", function(x, y) Furniture(x, y, xColor.xBROWN, '~', 'bed') end)
		
		
		AddTranslation("S", function(x, y) BuildShop(x, y, 4, 4, ItemKind.BOOK + ItemKind.SCROLL + ItemKind.POTION, 'Toberik, the human shopkeeper') end)
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
		AddTranslation(">", function(x, y) Way(DOWN, XLocation.EXTINCT_VOLCANO, x, y) end)
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
		AddTranslation(">", function(x, y) Way(DOWN, L_UNDEADS_TOMB1, x, y) end)
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
		AddTranslation(">", function(x, y) Way(DOWN, XLocation.WIZARD_DUNGEON1, x, y) end)
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
		AddTranslation("<", function(x, y) Way(UP, XLocation.WIZTOWER_TOP, x, y) end)
		DrawPattern(45, 25)

	-- Last thing done to the Valley itself, after every pattern is
	-- stamped onto it - restored alongside the orc war party/teleports
	-- above, see the comment there. Runs last so bushes don't end up
	-- under a village/town/city wall drawn afterward.
	ScatterHerbBushes()

-- KING'S TREASURE --
	CreateLocation(XLocation.KINGS_TREASURE, "RoyalTr", "Royal Treasure", PLAIN)
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
		AddTranslation("<", function(x, y) Way(UP, XLocation.MAIN, x, y) end)


-- YOHJI's TOWER SECOND FLOOR--
	CreateLocation(XLocation.WIZTOWER_TOP, "WzTwr", "Yohjishiro's Tower", PLAIN)
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
		AddTranslation(">", function(x, y) Way(DOWN, XLocation.MAIN, x, y) end)
		AddTranslation("W", function(x, y) CreateYohji(x, y) end)
		AddTranslation("B", function(x, y) Furniture(x, y, xColor.xLIGHTRED, '~', 'nice bed') end)
		DrawPattern(45, 25)


-- UNDEAD's THOMB Underground --
	CreateLocation(L_UNDEADS_TOMB1, "Tomb", "Tomb", CAVE)
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
		AddTranslation("<", function(x, y) Way(UP, XLocation.MAIN, x, y) end)
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

function SendFarmersToCollectMushrooms()
	local cave_x, cave_y = GetWayXY(XLocation.MUSHROOMS_CAVE5)
	local script = {
		{cmd = ScriptCommand.MOVE_POINT, pt_x = cave_x, pt_y = cave_y, ln = XLocation.MUSHROOMS_CAVE5},
		{cmd = ScriptCommand.COLLECT_MUSHROOM},
		{cmd = ScriptCommand.MOVE_POINT, pt_x = 13, pt_y = 8, ln = XLocation.MAIN},
		{cmd = ScriptCommand.DROP_ITEM, kind = ItemKind.FOOD},
	}

	for _, farmer in ipairs(FindCreatures(XLocation.MAIN, "small_village_farmer")) do
		ExecuteCreatureScript(farmer, script)
	end
end

function FarmerHandler(e, t, p, v)
	if (e == LuaEvent.CHAT) then
		local qs = QuestStatus(QUEST_ELDER)
		if (qs == XQuest.COMPLETE or qs == XQuest.CLOSED) then
			AddMessage("'Thank you, great hero!'")
		else
			AddMessage("'Please speak with our elder. He lives in the stone house.'")
		end
	end
	return 1
end

function CreateGuardians(x, y)
	for i = 1, 7 do 
		local g = Guardian("royal_guard", "guardian", x, y, 14, 5, XStandardAI.GUARD_AREA + XStandardAI.RANDOM_MOVE)
		SetEnemy(g, CreatureClass.ORC)
		SetEventHandler(g, 'RoyalGuardHandler')
	end
end


function RoyalGuardHandler(e, t, p, v)
	if (e == LuaEvent.CHAT) then
		AddMessage("'Don't bother me!'")
	elseif (e == LuaEvent.GIVE_ITEM) then
		local kind, brt, wt, it, count, name = GetItemParam(v)
		if (BinaryAND(kind, ItemKind.WEAPON) and BinaryAND(brt, AttackEffectType.ORCSLAYER) and wt == WSK_SWORD) then
			AddMessage("'Thank you!'")
			if (QuestStatus(QUEST_OZORIK) < XQuest.COMPLETE) then
				QuestModify(QUEST_OZORIK, XQuest.COMPLETE)
			end
		else
			AddMessage("'I do not need this!'")
			return 0;
		end
	end
	return 1
end


-- Recognizes fellow forest-brotherhood members by their cloak and never
-- treats them as enemies, regardless of the usual class-based hostility
-- rules (see BanditAI.isEnemy below). Guardian() already applies
-- GUARD_AREA/PROTECT_AREA/RANDOM_MOVE and the enemy_class every other
-- Guardian-created creature gets, so this only adds what used to be
-- XBandit-specific: the cloak swap and the isEnemy override.
function CreateBandit(x, y)
	local bandit = Guardian('bandit', "forest_brother", x, y, 12, 8, XStandardAI.GUARD_AREA + XStandardAI.PROTECT_AREA + XStandardAI.RANDOM_MOVE)
	AsCreature(bandit):PutOnBody(BodyPart.CLOAK, 0, CreateObject('XForestBrotherCloak'))
	SetCreatureAI(bandit, 'BanditAI')
end

-- Note: does not re-check personal-enemy status before the cloak check the
-- way the original C++ XBanditAI::isEnemy did (isPersonalEnemy isn't
-- exposed to Lua) - XStandardAI::isEnemy() still checks it as its own
-- final fallback when this returns nil, so the only behavior gap is a
-- forest-brother-cloaked creature that's *also* a declared personal enemy
-- getting treated as non-hostile instead of hostile. Narrow edge case,
-- accepted rather than adding new binding surface for it.
BanditAI = {}
function BanditAI.isEnemy(self, cr)
	if cr:IsWearingItemType(BodyPart.CLOAK, 0, ItemType.FORESTBROTHERCLOAK) then
		return false
	end

	return nil
end

function Grave(x, y, s, e)
	OuterObject(x, y, xColor.xLIGHTGRAY, "+", string.format("the grave signed '%s'", s), e)
end

function StandardGraveEvent(e, cr, o)	
	if (e == LuaEvent.OUTER_USE) then
		if (GetView(o) == "+") then
			AddMessage('You move the headstone. Something glimmers in the dark.')
			SetView(o, '-', xColor.xLIGHTGRAY);
			SetName(o, 'the desecrated grave');
			for i = 1, Rand(3) + 2 do 
				DropItem(CreateObject('XBone'), o)
			end
		else
			AddMessage('This grave has already been looted.')
		end
	end
	return 1
end

function AvanorDefenderGraveEvent(e, cr, o)
	if (e == LuaEvent.OUTER_USE and GetView(o) == "+") then
		DropItem(CreateObject('XAvanorDefender'), o)
	end
	return StandardGraveEvent(e, cr, o)
end
