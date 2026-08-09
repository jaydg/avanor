
function MakeAvanorValley()
	CreateLocation(XLocation.MAIN, "Valley", "Valley of Avanor", PLAIN)

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
		AddTranslation("P", function(x, y) for i = 1, 4 do SetEventHandler(Guardian('farmer', GROUP_ID.GID_SMALL_VILLAGE_FARMER, x, y, 20, 16), 'FarmerHandler') SetEventHandler(Guardian('goodwife', GROUP_ID.GID_SMALL_VILLAGE_FARMER, x, y, 20, 16), 'FarmerHandler') end end)
		AddTranslation("E", function(x, y) CreateElderGridor(x, y) end)
		AddTranslation("Y", function(x, y) CreateJorgus(x, y) end)
		AddTranslation("F", function(x, y) for i = 1, 5 do Guardian('bandit', GROUP_ID.GID_FOREST_BROTHER, x, y, 12, 8, XStandardAI.GUARD_AREA + XStandardAI.PROTECT_AREA + XStandardAI.RANDOM_MOVE) end end)
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
		AddTranslation("C", function(x, y) for i = 1, 5 do Guardian('citizen', GROUP_ID.GID_RODERICK_GUARDIAN, x, y, 30, 25) Guardian('fcitizen', GROUP_ID.GID_RODERICK_GUARDIAN, x, y, 30, 25) end end)
		AddTranslation("_", function(x, y) Altar(x, y, XDeity.LIFE) end)
		AddTranslation("E", function(x, y) CreateHighPriest(x, y) end)
		AddTranslation("F", function(x, y) Furniture(x, y, xColor.xBROWN, '~', 'pew') end)
		
		AddTranslation("R", function(x, y) CreateRoderik(x, y) Furniture(x, y, xColor.xYELLOW, '~', 'the throne of Avanor') end)
		AddTranslation("G", function(x, y) Guardian('royal_guard', GROUP_ID.GID_RODERICK_GUARDIAN, x, y) end)
		AddTranslation("H", function(x, y) Guardian('royal_guard', GROUP_ID.GID_RODERICK_GUARDIAN, x, y, 1, 1, XStandardAI.NO_SWAP) Way(DOWN, XLocation.KINGS_TREASURE, x, y) end)
		
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

function FarmerHandler(e, t, p, v)
	if (e == LUA_EVENT.LE_CHAT) then
		local qs = QuestStatus(QUEST_ELDER)
		if (qs == XQuest.COMPLETE or qs == XQuest.CLOSED) then
			AddMessage("'Thank you, great hero!'")
		else
			AddMessage("'Please speak with our elder. He lives in the stone house.'")
		end
	end
	return 1
end

function CreateElderGridor(x, y)
	local elder = Guardian("elder_gridor", GROUP_ID.GID_SMALL_VILLAGE_FARMER, x, y, 5, 1)
	SetEventHandler(elder, 'ElderGridorHandler')
end


function ElderGridorHandler(e, t, p, v)
	if (e == LUA_EVENT.LE_CHAT) then
		local qs = QuestStatus(QUEST_ELDER)
		if (qs == XQuest.UNKNOWN) then
			AddMessage('Good day, friend! All that we have are our mushrooms.')
			AddMessage('We collect them in the cave to the west of the village.')
			AddMessage('But a short time ago, an evil monster occupied the cave.')
			AddMessage('It looks like a demon and is very dangerous.')
			AddMessage('Maybe the people who live to the south of the lake can help.')
			QuestModify(QUEST_ELDER, XQuest.KNOWN)
		elseif (qs == XQuest.KNOWN) then
			AddMessage('The evil monster is still there.')
		elseif (qs == XQuest.COMPLETE) then
			AddMessage('Thank you for your great help! Now, our farmers can collect mushrooms.')
			QuestModify(QUEST_ELDER, XQuest.CLOSED)
			ExecuteAIScript()
		else
			AddMessage('Have a nice day,')
			if (Gender(p) == Gender.MALE) then
				AddMessage('sir!')
			else
				AddMessage("ma'am!")
			end
		end
	elseif (e == LUA_EVENT.LE_SAVE) then
	elseif (e == LUA_EVENT.LE_LOAD) then
	end
	return 1
end


function CreateJorgus(x, y)
	local jorgus = Guardian("jorgus", GROUP_ID.GID_FOREST_BROTHER, x, y, 3, 2)
	SetEventHandler(jorgus, 'JorgusHandler')
	GiveObjectToCreature(CreateObject('XForestBrotherCloak'), jorgus)
end

function JorgusHandler(e, t, p, v)
	if (e == LUA_EVENT.LE_CHAT) then
		if (GetSkill(p, SKT_STEALING) > 0) then
			if (Gender(p) == Gender.MALE) then
				AddMessage("Good day, brother!")
			else
				AddMessage("Good day, sister!")
			end
		else
			if (AskQuestion("'I can teach you the great art of theft for 1000gp. Would you like to learn?'", "y n", "yes", "no") == 'y') then
				if (MoneyOperation(p, -1000) >= 0) then
					MoneyOperation(t, 1000)
					LearnSkill(p, SKT_STEALING, 1)
					if (Gender(p) == Gender.MALE) then
						AddMessage("You're welcome, brother!")
					else
						AddMessage("You're welcome, sister!")
					end
				else
					AddMessage("You don't have enough money!")
				end
			else
				AddMessage("Don't waste my time!")
			end
		end
	end
	return 1
end


ozorik_award = 0
orcs_live = 50


function CreateOzorik(x, y)
	local ozorik = Guardian("ozorik", GROUP_ID.GID_GUARDIAN, x, y, 3, 2)
	SetEnemy(ozorik, CREATURE_CLASS.CR_ORC)
	SetEventHandler(ozorik, 'OzorikHandler')
	GiveObjectToCreature(CreateObject('XGlamdring'), ozorik)
	local death_hack = CreateObject('XDeathHack')
	ozorik_award = GetObjectGUID(death_hack)
	GiveObjectToCreature(death_hack, ozorik)
end


function OzorikHandler(e, t, p, v)
	if (e == LUA_EVENT.LE_CHAT) then
		local qs = QuestStatus(QUEST_OZORIK)
		local demon_quest = QuestStatus(QUEST_ELDER)
		if (orcs_live > 0 and qs < XQuest.COMPLETE) then
			if (qs == XQuest.UNKNOWN) then
				if (demon_quest == XQuest.KNOWN) then
					AddMessage("'Demons? We are mighty enough to slay them, but now another problem approaches from the south - an orc war-party!'")
				elseif (demon_quest == XQuest.UNKNOWN) then
					AddMessage("'Sorry, but I'm really busy now. The orc war-party will be here soon!'")
				end
				QuestModify(QUEST_OZORIK, XQuest.KNOWN)
			else
				AddMessage("'Sorry, but I'm really busy right now. The orc war-party will be here soon!'")
			end
		else
			if (qs < XQuest.CLOSED) then
				AddMessage("'You gained us victory!'")
				if (GiveAward(t, ozorik_award, p)) then
					AddMessage('Take this dagger as a reward!')
				end
				QuestModify(QUEST_OZORIK, XQuest.CLOSED)
			else
				AddMessage('Good day, hero!')
			end
		end
	elseif (e == LUA_EVENT.LE_GIVE_ITEM) then		
		local kind, brt, wt, it, count, name = GetItemParam(v)
		if (BinaryAND(kind, ItemKind.WEAPON) and BinaryAND(brt, AttackEffectType.ORCSLAYER) and wt == WSK_SWORD) then
			AddMessage("'Wow, you've probably saved our lives! Please, take this weapon to one of my guardians, then return to me!'")
		else
			AddMessage("'We are not looking for this.'")
			return 0
		end
	elseif (e == LUA_EVENT.LE_SAVE) then
		StoreInt(ozorik_award)
	elseif (e == LUA_EVENT.LE_LOAD) then
		ozorik_award = RestoreInt()
	end
	return 1
end


function CreateGuardians(x, y)
	for i = 1, 7 do 
		local g = Guardian("royal_guard", GROUP_ID.GID_GUARDIAN, x, y, 14, 5, XStandardAI.GUARD_AREA + XStandardAI.RANDOM_MOVE)
		SetEnemy(g, CREATURE_CLASS.CR_ORC)
		SetEventHandler(g, 'RoyalGuardHandler')
	end
end


function RoyalGuardHandler(e, t, p, v)
	if (e == LUA_EVENT.LE_CHAT) then
		AddMessage("'Don't bother me!'")
	elseif (e == LUA_EVENT.LE_GIVE_ITEM) then
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


function CreateGekta(x, y)
	local gekta = Guardian("gekta", GROUP_ID.GID_GUARDIAN, x, y, 14, 5)
	SetEnemy(gekta, CREATURE_CLASS.CR_ORC)
	SetEventHandler(gekta, 'GektaHandler')
end


function GektaHandler(e, t, p, v)
	if (e == LUA_EVENT.LE_CHAT) then
		AddMessage("'Woof! Woof! Woof'");
	elseif (e == LUA_EVENT.LE_GIVE_ITEM) then
		local kind, brt, wt, it, count, name = GetItemParam(v)
		if (kind == ItemKind.FOOD) then
			if (it == ITEM_TYPE.IT_BONE) then
				for i = 1, count do
					if (Rand(7) == 0) then
						 AddMessage("Gekta suddenly start to dig in the ground. She digs a pit. Gekta digs something up from the ground. After this, she puts a bone in the pit and buries it.")
						 DropItem(CreateObject(ItemKind.ITEM - ItemKind.FOOD, 20, 500), t)
					else
						AddMessage(string.format("Gekta eats the %s.", name))
					end
				end
			else
				AddMessage(string.format("'Gekta eats the %s.'", name))
				if (Rand(5) == 0) then
					SetCompanion(p, t, true)
					AddMessage("Gekta looks at you faithfully!");
				end
			end
			DestroyObject(v)
		else
			AddMessage('Woof?');
			return 0;
		end
	end
	return 1
end


function CreateYohji(x, y)
	local yohji = Guardian("yohjishiro", GID_NONE, x, y, 5, 5)
	SetEventHandler(yohji, 'YohjiHandler')
end


function YohjiHandler(e, t, p, v)
	if (e == LUA_EVENT.LE_CHAT) then
		local result
		if (GetSkill(p, SKT_LITERACY) == 0) then
			result = AskQuestion("What do you wish to speak about?", "esc q l", "quest", "learn")
		else
			result = 'q'
		end
		if (result == 'q') then
			if (QuestStatus(QUEST_YOHJI_BAT) ~= XQuest.KNOWN and QuestStatus(QUEST_YOHJI_RAT) ~= XQuest.KNOWN) then
				if (Rand(2) == 1) then
					AddMessage("'I can identify all items in your inventory, if you bring me a bat wing.'")
					QuestModify(QUEST_YOHJI_BAT, XQuest.KNOWN)
				else
					AddMessage("'I can identify all items in your inventory, if you bring me a rat tail.'")
					QuestModify(QUEST_YOHJI_RAT, XQuest.KNOWN)
				end
			else
				AddMessage("'Please, complete my last request first'")
			end
		elseif (result == 'l') then
			if (AskQuestion("'Do you want to learn literacy for 500gp?'", "y n", "yes", "no") == 'y') then
				if (MoneyOperation(p, -500) >= 0) then
					LearnSkill(p, SKT_LITERACY, 1)
					MoneyOperation(t, 500)
					AddMessage("Yohjishiro touches you. You feel more educated.")
				else
					AddMessage("'You don't have enough money!'")
				end
			else
				AddMessage("'As you wish.'")
			end
		end
	elseif (e == LUA_EVENT.LE_GIVE_ITEM) then
		local kind, brt, wt, it, count, name = GetItemParam(v)
		if (it == ITEM_TYPE.IT_RATTAIL or it == ITEM_TYPE.IT_BATWING) then
			if (it == ITEM_TYPE.IT_RATTAIL and QuestStatus(QUEST_YOHJI_RAT) == XQuest.KNOWN) then
				AddMessage("'Oh, thank you!' Yohjishiro touches you. Suddenly you know more about the items in your inventory.")
				QuestModify(QUEST_YOHJI_RAT, XQuest.UNKNOWN)
			elseif (it == ITEM_TYPE.IT_BATWING and QuestStatus(QUEST_YOHJI_BAT) == XQuest.KNOWN) then
				AddMessage("'Oh, thank you!'")
				MakeEffect(XEffect.GREAT_IDENTIFY, t, nil, 0, 0, p, 0, 0, 0, nil)
				QuestModify(QUEST_YOHJI_BAT, XQuest.UNKNOWN)
			else
				if (MoneyOperation(t, -50 * count) >= 0) then
					AddMessage(string.format("'I hope %d gp will be enough for this.'", 50 * count))
					MoneyOperation(p, 50 * count)
				else
					AddMessage("Sorry, I don't have enough money to buy this.")
				end
			end
		else
			AddMessage("'It is not of interest for me.'")
			return 0
		end
	end
	return 1
end


function CreateGefeon(x, y)
	local gefeon = Guardian("gefeon", GROUP_ID.GID_RODERICK_GUARDIAN, x, y, 3, 4)
	SetEventHandler(gefeon, '')
end

function CreateHighPriest(x, y)
	local hp = Guardian("highpriest", GROUP_ID.GID_RODERICK_GUARDIAN, x, y, 3, 4)
	SetEventHandler(hp, '')
end

function CreateRoderik(x, y)
	local hp = Guardian("roderik", GROUP_ID.GID_RODERICK_GUARDIAN, x, y, 1, 1, XStandardAI.NO_SWAP)
	SetEventHandler(hp, '')
end


function Grave(x, y, s, e)
	OuterObject(x, y, xColor.xLIGHTGRAY, "+", string.format("the grave signed '%s'", s), e)
end

function StandardGraveEvent(e, cr, o)	
	if (e == LUA_EVENT.LE_OUTER_USE) then
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
	if (e == LUA_EVENT.LE_OUTER_USE and GetView(o) == "+") then
		DropItem(CreateObject('XAvanorDefender'), o)
	end
	return StandardGraveEvent(e, cr, o)
end
