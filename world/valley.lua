
function MakeAvanorValley()
	CreateLocation(L_MAIN, "Valley", "Valley of Avanor", PLAIN)

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
		AddTranslation("@", M_WOODWALL)
		AddTranslation("1", M_ROAD)
		AddTranslation("2", M_BRIDGE)
		AddTranslation("3", M_SAND)
		AddTranslation("A", "Furniture(x, y, xBROWN, '~', 'plain bed')")
		AddTranslation("S", "BuildShop(x, y, 8, 2, IM_FOOD, 'Nobel, the human shopkeeper')")
		AddTranslation("P", "for i = 1, 4 do SetEventHandler(Guardian(CN_FARMER, GID_SMALL_VILLAGE_FARMER, x, y, 20, 16), 'FarmerHandler') SetEventHandler(Guardian(CN_GOODWIFE, GID_SMALL_VILLAGE_FARMER, x, y, 20, 16), 'FarmerHandler') end")
		AddTranslation("E", "CreateElderGridor(x, y)")
		AddTranslation("Y", "CreateJorgus(x, y)")
		AddTranslation("F", "for i = 1, 5 do Guardian(CN_BANDIT, GID_FOREST_BROTHER, x, y, 12, 8, AIF_GUARD_AREA + AIF_PROTECT_AREA + AIF_RANDOM_MOVE) end")
		AddTranslation(">", "Way(DOWN, L_MUSHROOMS_CAVE1, x, y)")
		AddTranslation("*", "Way(DOWN, L_DWARFCITYCAVE1, x, y)")
		AddTranslation("9", "Way(DOWN, L_SMALL_CAVE1, x, y)")
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
		
		AddTranslation("1", M_ROAD)
		AddTranslation("A", "Furniture(x, y, xBROWN, '~', 'plain bed')")
		AddTranslation("B", "Furniture(x, y, xBROWN, '~', 'table')")
		AddTranslation("C", "Furniture(y, y, xBROWN, '~', 'desk')")
		AddTranslation("D", "Furniture(x, y, xLIGHTRED, '~', 'bed')")
		AddTranslation("E", "CreateOzorik(x, y)")
		AddTranslation("F", "CreateGuardians(x, y)")
		AddTranslation("G", "CreateGekta(x, y)")
		AddTranslation("S", "BuildShop(x, y, 9, 3, IM_ARMOUR + IM_WEAPON + IM_MISSILE + IM_MISSILEW, 'Noberik, the human shopkeeper')")
		AddTranslation(">", "Way(DOWN, L_RATCELLAR, x, y)")
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

		AddTranslation("1", M_BRIDGE)
		AddTranslation("2", M_WINDOW)
		AddTranslation("3", M_GOLDENFLOOR)
		AddTranslation("M", M_MOUNTAIN)
		AddTranslation("W", M_MARBLEWALL)
		AddTranslation("X", M_GOLDENFENCE)
		
		AddTranslation("A", "Furniture(x, y, xLIGHTRED, '~', 'eternal flame')")
		AddTranslation("B", "CreateGefeon(x, y)")
		AddTranslation("C", "for i = 1, 5 do Guardian(CN_CITIZEN, GID_RODERICK_GUARDIAN, x, y, 30, 25) Guardian(CN_FCITIZEN, GID_RODERICK_GUARDIAN, x, y, 30, 25) end")
		AddTranslation("_", "Altar(x, y, D_LIFE)")
		AddTranslation("E", "CreateHighPriest(x, y)")
		AddTranslation("F", "Furniture(x, y, xBROWN, '~', 'pew')")
		
		AddTranslation("R", "CreateRoderik(x, y) Furniture(x, y, xYELLOW, '~', 'the throne of Avanor')")
		AddTranslation("G", "Guardian(CN_ROYAL_GUARD, GID_RODERICK_GUARDIAN, x, y)")
		AddTranslation("H", "Guardian(CN_ROYAL_GUARD, GID_RODERICK_GUARDIAN, x, y, 1, 1, AIF_NO_SWAP) Way(DOWN, L_KINGS_TREASURE, x, y)")
		
		AddTranslation("Q", "Furniture(x, y, xLIGHTRED, '~', 'royal bed')")
		AddTranslation("O", "Furniture(x, y, xBROWN, '~', 'dinner table')")
		AddTranslation("U", "Furniture(x, y, xBROWN, '~', 'round table')")
		
		AddTranslation("K", "Furniture(x, y, xBROWN, '~', 'table')")
		AddTranslation("N", "Furniture(x, y, xBROWN, '~', 'bed')")
		
		
		AddTranslation("S", "BuildShop(x, y, 4, 4, IM_BOOK + IM_SCROLL + IM_POTION, 'Toberik, the human shopkeeper')")
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
		AddTranslation(",", M_HILL)
		AddTranslation(":", M_LOWMOUNTAIN)
		AddTranslation(";", M_MOUNTAIN)
		AddTranslation("^", M_HIGHMOUNTAIN)
		AddTranslation(">", "Way(DOWN, L_EXTINCT_VOLCANO, x, y)")
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
		AddTranslation(".", M_CAVEFLOOR)
		AddTranslation(">", "Way(DOWN, L_UNDEADS_TOMB1, x, y)")
		AddTranslation("U", "for i = 1, 20 do Creature(CN_SKELETON, x, y, 14, 4) end")
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
		AddTranslation("#", M_BLACKMARBLEWALL)
		AddTranslation(">", "Way(DOWN, L_WIZARD_DUNGEON1, x, y)")
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
		AddTranslation("1", M_ROAD)
		AddTranslation("2", M_GOLDENFLOOR)
		AddTranslation("#", M_MARBLEWALL)
		AddTranslation("<", "Way(UP, L_WIZTOWER_TOP, x, y)")
		DrawPattern(45, 25)


-- KING'S TREASURE --
	CreateLocation(L_KINGS_TREASURE, "RoyalTr", "Royal Treasure", PLAIN)
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

		AddTranslation("[", "DropItem(CreateObject('XAncientMachinePart'), x, y)")
		AddTranslation("$", "Treasure(x, y, 250)")
		AddTranslation("~", "Chest(x, y)")
		AddTranslation("<", "Way(UP, L_MAIN, x, y)")


-- YOHJI's TOWER SECOND FLOOR--
	CreateLocation(L_WIZTOWER_TOP, "WzTwr", "Yohjishiro's Tower", PLAIN)
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
		AddTranslation("1", M_ROAD)
		AddTranslation("2", M_GOLDENFLOOR)
		AddTranslation("3", M_WINDOW)
		AddTranslation("#", M_MARBLEWALL)
		AddTranslation(">", "Way(DOWN, L_MAIN, x, y)")
		AddTranslation("W", "CreateYohji(x, y)")
		AddTranslation("B", "Furniture(x, y, xLIGHTRED, '~', 'nice bed')")
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
		AddTranslation("<", "Way(UP, L_MAIN, x, y)")
		AddTranslation("A", "Grave(x, y, 'Frederick III (760-805)', 'StandardGraveEvent')")
		AddTranslation("B", "Grave(x, y, 'Frederick II (710-790)', 'StandardGraveEvent')")
		AddTranslation("C", "Grave(x, y, 'Arthur IV (796-854)', 'StandardGraveEvent')")
		AddTranslation("D", "Grave(x, y, 'Rodomir III (821-894)', 'AvanorDefenderGraveEvent')")
		
		DrawPattern(0, 0)
		for i = 1, 20 do Creature(CN_GHOST) end
		for i = 1, 10 do 
			Creature(CN_SPECTRE) 
			Creature(CN_DREAD)
		end
end

function FarmerHandler(e, t, p, v)
	if (e == LE_CHAT) then
		local qs = QuestStatus(QUEST_ELDER)
		if (qs == Q_COMPLETE or qs == Q_CLOSED) then
			AddMessage("'Thank you, great hero!'")
		else
			AddMessage("'Please speak with our elder. He lives in the stone house.'")
		end
	end
	return 1
end

function CreateElderGridor(x, y)
	local elder = Guardian(CN_ELDER_GRIDOR, GID_SMALL_VILLAGE_FARMER, x, y, 5, 1)
	SetEventHandler(elder, 'ElderGridorHandler')
end


function ElderGridorHandler(e, t, p, v)
	if (e == LE_CHAT) then
		local qs = QuestStatus(QUEST_ELDER)
		if (qs == Q_UNKNOWN) then
			AddMessage('Good day, friend! All that we have are our mushrooms.')
			AddMessage('We collect them in the cave to the west of the village.')
			AddMessage('But a short time ago, an evil monster occupied the cave.')
			AddMessage('It looks like a demon and is very dangerous.')
			AddMessage('Maybe the people who live to the south of the lake can help.')
			QuestModify(QUEST_ELDER, Q_KNOWN)
		elseif (qs == Q_KNOWN) then
			AddMessage('The evil monster is still there.')
		elseif (qs == Q_COMPLETE) then
			AddMessage('Thank you for your great help! Now, our farmers can collect mushrooms.')
			QuestModify(QUEST_ELDER, Q_CLOSED)
			ExecuteAIScript()
		else
			AddMessage('Have a nice day,')
			if (Gender(p) == GEN_MALE) then
				AddMessage('sir!')
			else
				AddMessage("ma'am!")
			end
		end
	elseif (e == LE_SAVE) then
	elseif (e == LE_LOAD) then
	end
	return 1
end


function CreateJorgus(x, y)
	local jorgus = Guardian(CN_JORGUS, GID_FOREST_BROTHER, x, y, 3, 2)
	SetEventHandler(jorgus, 'JorgusHandler')
	GiveObjectToCreature(CreateObject('XForestBrotherCloak'), jorgus)
end

function JorgusHandler(e, t, p, v)
	if (e == LE_CHAT) then
		if (GetSkill(p, SKT_STEALING) > 0) then
			if (Gender(p) == GEN_MALE) then
				AddMessage("Good day, brother!")
			else
				AddMessage("Good day, sister!")
			end
		else
			if (AskQuestion("'I can teach you the great art of theft for 1000gp. Would you like to learn?'", "y n", "yes", "no") == 'y') then
				if (MoneyOperation(p, -1000) >= 0) then
					MoneyOperation(t, 1000)
					LearnSkill(p, SKT_STEALING, 1)
					if (Gender(p) == GEN_MALE) then
						AddMessage("You're welcome, brother!")
					else
						AddMessage("You're welcome, sister!")
					end
				else
					AddMessage("You haven't enough money!")
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
	local ozorik = Guardian(CN_OZORIK, GID_GUARDIAN, x, y, 3, 2)
	SetEnemy(ozorik, CR_ORC)
	SetEventHandler(ozorik, 'OzorikHandler')
	GiveObjectToCreature(CreateObject('XGlamdring'), ozorik)
	local death_hack = CreateObject('XDeathHack')
	ozorik_award = GetObjectGUID(death_hack)
	GiveObjectToCreature(death_hack, ozorik)
end


function OzorikHandler(e, t, p, v)
	if (e == LE_CHAT) then
		local qs = QuestStatus(QUEST_OZORIK)
		local demon_quest = QuestStatus(QUEST_ELDER)
		if (orcs_live > 0 and qs < Q_COMPLETE) then
			if (qs == Q_UNKNOWN) then
				if (demon_quest == Q_KNOWN) then
					AddMessage("'Demons? We are mighty enough to slay them, but now another problem approaches from the south - an orc war-party!'")
				elseif (demon_quest == Q_UNKNOWN) then
					AddMessage("'Sorry, but I'm really busy now. The orc war-party will be here soon!'")
				end
				QuestModify(QUEST_OZORIK, Q_KNOWN)
			else
				AddMessage("'Sorry, but I'm really busy now. The orc war-party will be here soon!'")
			end
		else
			if (qs < Q_CLOSED) then
				AddMessage("'You gained us victory!'")
				if (GiveAward(t, ozorik_award, p)) then
					AddMessage('Take this dagger as a reward!')
				end
				QuestModify(QUEST_OZORIK, Q_CLOSED)
			else
				AddMessage('Good day, hero!')
			end
		end
	elseif (e == LE_GIVE_ITEM) then		
		local im, brt, wt, it, count, name = GetItemParam(v)
		if (BinaryAND(im, IM_WEAPON) and BinaryAND(brt, BR_ORCSLAYER) and wt == WSK_SWORD) then
			AddMessage("'Wow, you've probably saved our lives! Please, take this weapon to one of my guardians, than return to me!'")
		else
			AddMessage("'We are not looking for this.'")
			return 0
		end
	elseif (e == LE_SAVE) then
		StoreInt(ozorik_award)
	elseif (e == LE_LOAD) then
		ozorik_award = RestoreInt()
	end
	return 1
end


function CreateGuardians(x, y)
	for i = 1, 7 do 
		local g = Guardian(CN_ROYAL_GUARD, GID_GUARDIAN, x, y, 14, 5, AIF_GUARD_AREA + AIF_RANDOM_MOVE)
		SetEnemy(g, CR_ORC)
		SetEventHandler(g, 'RoyalGuardHandler')
	end
end


function RoyalGuardHandler(e, t, p, v)
	if (e == LE_CHAT) then
		AddMessage("'Don't bothering me!'")
	elseif (e == LE_GIVE_ITEM) then
		local im, brt, wt, it, count, name = GetItemParam(v)
		if (BinaryAND(im, IM_WEAPON) and BinaryAND(brt, BR_ORCSLAYER) and wt == WSK_SWORD) then
			AddMessage("'Thank you!'")
			if (QuestStatus(QUEST_OZORIK) < Q_COMPLETE) then
				QuestModify(QUEST_OZORIK, Q_COMPLETE)
			end
		else
			AddMessage("'I do not need this!'")
			return 0;
		end
	end
	return 1
end


function CreateGekta(x, y)
	local gekta = Guardian(CN_GEKTA, GID_GUARDIAN, x, y, 14, 5)
	SetEnemy(gekta, CR_ORC)
	SetEventHandler(gekta, 'GektaHandler')
end


function GektaHandler(e, t, p, v)
	if (e == LE_CHAT) then
		AddMessage("'Woof! Woof! Woof'");
	elseif (e == LE_GIVE_ITEM) then
		local im, brt, wt, it, count, name = GetItemParam(v)
		if (im == IM_FOOD) then
			if (it == IT_BONE) then
				for i = 1, count do
					if (Rand(7) == 0) then
						 AddMessage("Gekta suddenly start to dig in the ground. She digs a pit. Gekta digs something up from the ground. After this, she puts a bone in the pit and buries it.")
						 DropItem(CreateObject(IM_ITEM - IM_FOOD, 20, 500), t)
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
	local yohji = Guardian(CN_YOHJISHIRO, GID_NONE, x, y, 5, 5)
	SetEventHandler(yohji, 'YohjiHandler')
end


function YohjiHandler(e, t, p, v)
	if (e == LE_CHAT) then
		local result
		if (GetSkill(p, SKT_LITERACY) == 0) then
			result = AskQuestion("What do you wish to speak about?", "esc q l", "quest", "learn")
		else
			result = 'q'
		end
		if (result == 'q') then
			if (QuestStatus(QUEST_YOHJI_BAT) ~= Q_KNOWN and QuestStatus(QUEST_YOHJI_RAT) ~= Q_KNOWN) then
				if (Rand(2) == 1) then
					AddMessage("'I can identify all your inventory, if you bring me a bat wing.'")
					QuestModify(QUEST_YOHJI_BAT, Q_KNOWN)
				else
					AddMessage("'I can identify all your inventory, if you bring me a rat tail.'")
					QuestModify(QUEST_YOHJI_RAT, Q_KNOWN)
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
					AddMessage("'You haven't enough money!'")
				end
			else
				AddMessage("'As you wish.'")
			end
		end
	elseif (e == LE_GIVE_ITEM) then
		local im, brt, wt, it, count, name = GetItemParam(v)
		if (it == IT_RATTAIL or it == IT_BATWING) then
			if (it == IT_RATTAIL and QuestStatus(QUEST_YOHJI_RAT) == Q_KNOWN) then
				AddMessage("'Oh, thank you!' Yohjishiro touches you. Suddenly you know more about your inventory.")
				QuestModify(QUEST_YOHJI_RAT, Q_UNKNOWN)
			elseif (it == IT_BATWING and QuestStatus(QUEST_YOHJI_BAT) == Q_KNOWN) then
				AddMessage("'Oh, thank you!'")
				MakeEffect(E_GREAT_IDENTIFY, t, nil, 0, 0, p, 0, 0, 0, nil)
				QuestModify(QUEST_YOHJI_BAT, Q_UNKNOWN)
			else
				if (MoneyOperation(t, -50 * count) >= 0) then
					AddMessage(string.format("'I hope %d gp will be enough for this.'", 50 * count))
					MoneyOperation(p, 50 * count)
				else
					AddMessage("Sorry, I haven't enough money to by this.")
				end
			end
		else
			AddMessage("'It is of no interest to me.'")
			return 0
		end
	end
	return 1
end


function CreateGefeon(x, y)
	local gefeon = Guardian(CN_GEFEON, GID_RODERICK_GUARDIAN, x, y, 3, 4)
	SetEventHandler(gefeon, '')
end

function CreateHighPriest(x, y)
	local hp = Guardian(CN_HIGHPRIEST, GID_RODERICK_GUARDIAN, x, y, 3, 4)
	SetEventHandler(hp, '')
end

function CreateRoderik(x, y)
	local hp = Guardian(CN_RODERIK, GID_RODERICK_GUARDIAN, x, y, 1, 1, AIF_NO_SWAP)
	SetEventHandler(hp, '')
end


function Grave(x, y, s, e)
	OuterObject(x, y, xLIGHTGRAY, "+", string.format("the grave signed '%s'", s), e)
end

function StandardGraveEvent(e, cr, o)	
	if (e == LE_OUTER_USE) then
		if (GetView(o) == "+") then
			AddMessage('You move the headstone. Something glimmers in the dark.')
			SetView(o, '-', xLIGHTGRAY);
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
	if (e == LE_OUTER_USE and GetView(o) == "+") then
		DropItem(CreateObject('XAvanorDefender'), o)
	end
	return StandardGraveEvent(e, cr, o)
end
