
-----------------------------------------------------------
-------------------- DWARVEN CITY -------------------------

function MakeDwarvenCity()
	CreateLocation(L_DWARFCITYCAVE1, "PDC:1", "Path to the Dwarven City Level 1", DUNGEON)
		Way(UP, L_MAIN)
		Way(DOWN, L_DWARFCITYCAVE2)
		Settle(CR_UNDEAD + CR_BLOB + CR_INSECT + CR_REPTILE + CR_RAT + CR_FELINE + CR_CANINE, CRL_VERY_LOW)

	CreateLocation(L_DWARFCITYCAVE2, "PDC:2", "Path to the Dwarven City Level 2", DUNGEON)
		Way(UP, L_DWARFCITYCAVE1)
		Way(DOWN, L_DWARFCITYCAVE3)
		Settle(CR_UNDEAD + CR_BLOB + CR_INSECT + CR_REPTILE + CR_RAT + CR_FELINE + CR_CANINE, CRL_LOW)

	CreateLocation(L_DWARFCITYCAVE3, "PDC:3", "Path to the Dwarven City Level 3", DUNGEON)
		Way(UP, L_DWARFCITYCAVE2)
		Way(DOWN, L_DWARFCITYCAVE4)
		Settle(CR_UNDEAD + CR_BLOB + CR_INSECT + CR_REPTILE + CR_RAT + CR_FELINE + CR_CANINE, CRL_LOW)

	CreateLocation(L_DWARFCITYCAVE4, "PDC:4", "Path to the Dwarven City Level 4", DUNGEON)
		Way(UP, L_DWARFCITYCAVE3)
		Way(DOWN, L_DWARFCITYCAVE5)
		Settle(CR_UNDEAD + CR_BLOB + CR_INSECT + CR_REPTILE + CR_RAT + CR_FELINE + CR_CANINE, CRL_LOW)

	CreateLocation(L_DWARFCITYCAVE5, "PDC:5", "Path to the Dwarven City Level 5", DUNGEON)
		Way(UP, L_DWARFCITYCAVE4)
		Way(DOWN, L_DWARFCITYCAVE6)
		Settle(CR_UNDEAD + CR_BLOB + CR_INSECT + CR_REPTILE + CR_RAT + CR_FELINE + CR_CANINE, CRL_LOW)

	CreateLocation(L_DWARFCITYCAVE6, "PDC:6", "Path to the Dwarven City Level 6", DUNGEON)
		Way(UP, L_DWARFCITYCAVE5)
		Way(DOWN, L_DWARFCITY)
		Settle(CR_UNDEAD + CR_BLOB + CR_INSECT + CR_REPTILE + CR_RAT + CR_FELINE + CR_CANINE + CR_GOBLIN + CR_KOBOLD, CRL_LOW)
		DropItem(CreateObject("XCookingSet"))


	CreateLocation(L_DWARFCITY, "DvCty", "Dwarven City", CAVE)
		SetPattern(80, 20,
		"################################################################################" ..
		"###############,,,,,,,############,,,,,,,####G####################,,,,##F++E,,A#" ..
		"##,,,,,,,,,,,,,,,,,,,,,,,,,,,,,P,,,,,,,,,,,,,;,,,,,,,,,,,,,,,#####,,,,#####,,,A#" ..
		"###,,,#########################,,,,,,,,,,,,,,;,,,,,,,,,,,,,,,,,,####+########+##" ..
		"##,,,,#,,,,,#,,,,,#,,,,,#,,,,,#,,,,,,,,,,,,,,;,,,,,,,,,,,,,,,,,,,#,,,,,#,#,,,,,#" ..
		"###,,,###+#####+#####+#####+###,,,,,,,,,,,,,,;,,,,,,,,,,,,,,,,,,,#,,,,,+,#,,,,,#" ..
		"####,,;;;;;;;;;;;;;;;;;;;;;;;;;;,,,,,,,,,,,,,;,,,,,,,,,,,,,,,,,,,#,,B,,#,#,,C,,#" ..
		"##,,,,###+#####+#####+#####+###;,,,,,,,,,;;;;;;;;;,,,,,,,,,,,,,,,#,,,,,#,+,,,,,#" ..
		"###,,,#,,,,,#,,,,,#,,,,,#,,,,,#;,,,,,,,,,;;;;;;;;;,,,,,,,,,,,,,,,#######,#,,####" ..
		"##,,,,#########################;,,,,,,,,,;;==:==;;,,,,,,,,,,,,,,,#,,,,,,,####,,#" ..
		"###,,,,,,,,,###################;,,,,,,,,,;;==:==;;,,,,,,,,,,,,,,,#,,,,,,A#A,,A,#" ..
		"######,,,,,,#S,,,,,,,,##T,,,,,#;;;;;;;;;;;;::_::;;;;;;;;;;;;;;;;;+1111111+1111X#" ..
		"#######,,,,,#,,,,,,,,,##,,,,,,#;,,,,,,,,,;;==:==;;,,,,,,,,,,,,,,,#,,,,,,A#A,,A,#" ..
		"##########,,#,,,,,,,,,##,,,,,,+;,,,,,,,,,;;==:==;;,,,,,,,,,,,,,,,#,,,,,,,####,,#" ..
		"###,,##,,##,#####/######,,,,,,#;,,,,,,,,,;;;;;;;;;,,,,,,,,,,,,,,,#######,#,,####" ..
		"##,,,,,,,,#,,,,,,;,,,,,########;,,,,,,,,,;;;;;;;;;,,,,,,,,,,,,,,,#,,,,,#,+,,,,,#" ..
		"#,,,A,A,,,+;;;;;;;;;;;;;;;;;;;;;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,#,,,,,#,#,,,D,#" ..
		"#,,,,<,,,,#,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,#,,,,,+,#,,,,,#" ..
		"##,,A,A,,,#,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,#,,,,,#,#,,,,,#" ..
		"################################################################################")
		AddTranslation("1", GOLDEN_FLOOR)
		AddTranslation(";", OBSIDIAN_FLOOR)
		AddTranslation(":", BRIDGE)
		AddTranslation("<", "Way(UP, L_DWARFCITYCAVE6, x, y)")
		AddTranslation("A", "Guardian(CN_DWARF_GUARD, GID_DWARVEN_GUARDIAN, x, y)")
		AddTranslation("S", "BuildShop(x, y, 9, 3, IM_ARMOUR + IM_WEAPON + IM_POTION + IM_BOOK + IM_SCROLL + IM_NECK + IM_MISSILE + IM_MISSILEW, 'Toberin, the dwarwen shopkeeper')")
		AddTranslation("B", "Furniture(x, y, xBROWN, '~', 'a dinner table')")
		AddTranslation("C", "Furniture(x, y, xBROWN, '~', 'a round table')")
		AddTranslation("D", "Furniture(x, y, xLIGHTRED, '~', 'a royal bed')")
		AddTranslation("X", "CreateTorin(x, y) Furniture(x, y, xYELLOW, '~', 'the throne of the Dwarven Kingdom')")
		AddTranslation("_", "Altar(x, y, D_LIFE)")
		AddTranslation("P", "for i = 1, 6 do Guardian(CN_DWARF, GID_DWARVEN_GUARDIAN, x, y, 32, 16) end")
		AddTranslation("T", "CreateTodin(x, y)")
		AddTranslation("E", "EventPlace(x - 1, y, 5, 2, 'DvCityEvent1')")
		AddTranslation("F", "Way(DOWN, L_DWARFTREASURE, x, y)")
		AddTranslation("G", "Way(DOWN, L_GASMINE1, x, y)")
		DrawPattern(0, 0)

	CreateLocation(L_DWARFTREASURE, "DvTr", "Dwarven Treasure", CAVE)
		SetPattern(80, 20,
		"################################################################################" ..
		"################################################################################" ..
		"################################################################################" ..
		"################################################################################" ..
		"#######################.................................########################" ..
		"#######################.###############.###############.########################" ..
		"#######################.#.........#####^#####.........#.########################" ..
		"#######################.#.#######.#####.#####.#######.#.########################" ..
		"#######################.#.#$$$$$#.####...####.#~.~..#.#.########################" ..
		"#######################.#.+^...$#^+....<....+^#~.A.^+.#.########################" ..
		"#######################.#.#$$$$$#.####...####.#~.~..#.#.########################" ..
		"#######################.#.#######.#####.#####.#######.#.########################" ..
		"#######################.#.........#####^#####.........#.########################" ..
		"#######################.###############.###############.########################" ..
		"#######################.................................########################" ..
		"################################################################################" ..
		"################################################################################" ..
		"################################################################################" ..
		"################################################################################" ..
		"################################################################################")
		AddTranslation(".", GOLDEN_FLOOR)
		AddTranslation("<", "Way(UP, L_DWARFCITY, x, y)")
		AddTranslation("$", "Treasure(x, y, 500)")
		AddTranslation("~", "Chest(x, y)")
		AddTranslation("A", "DropItem(CreateObject('XAncientMachinePart'), x, y)")
		DrawPattern(0, 0)
		

	CreateLocation(L_GASMINE1, "GM:1", "Gassed Mine level 1", DUNGEON)
		Way(UP, L_DWARFCITY)
		Way(DOWN, L_GASMINE2)
--		Settle(CR_BLOB, CRL_VERY_LOW)
		EventPlace('GasMineEvent')

	CreateLocation(L_GASMINE2, "GM:2", "Gassed Mine level 2", DUNGEON)
		Way(UP, L_GASMINE1)
		Way(DOWN, L_GASMINE3)
--		Settle(CR_BLOB, CRL_VERY_LOW)
		EventPlace('GasMineEvent')

	CreateLocation(L_GASMINE3, "GM:3", "Gassed Mine level 3", DUNGEON)
		Way(UP, L_GASMINE2)
--		Settle(CR_BLOB, CRL_VERY_LOW)
		EventPlace('GasMineEvent')
		OuterObject(xBLUE, '#', 'a gas pump', 'GasPumpEvent')
end
	

DvCityEventMoveCount = 0

function DvCityEvent1(e, p)
	if (e == LE_MOVE and isHero(p)) then
		local c = FindCreature(L_DWARFCITY, GID_DWARVEN_GUARDIAN, 75, 1, 4, 2)
		if (not isEnemy(c, p)) then
			DvCityEventMoveCount = DvCityEventMoveCount + 1
			if (DvCityEventMoveCount < 3) then
				AddMessage("'Leave here immediately!'")
			elseif (DvCityEventMoveCount == 3) then
				AddMessage("'This is your last chance to leave!'")
			elseif (DvCityEventMoveCount > 3) then
				SetItEnemyFor(p, c)
			end
		end
	elseif (e == LE_MOVE_IN and isHero(p)) then
		local c = FindCreature(L_DWARFCITY, GID_DWARVEN_GUARDIAN, 75, 1, 4, 2)
		if (not isEnemy(c, p)) then
			AddMessage("'Halt! You aren't allowed to enter.'")
		end
		DvCityEventMoveCount = 1
	elseif (e == LE_MOVE_OUT and isHero(p)) then
		DvCityEventMoveCount = 0
	elseif (e == LE_SAVE) then
		StoreInt(DvCityEventMoveCount)
	elseif (e == LE_LOAD) then
		DvCityEventMoveCount = RestoreInt()
	end

end

torin_award = 0;

function GasMineEvent(e, p)
	if (e == LE_MOVE) then
		if (QuestStatus(QUEST_TORIN) < Q_COMPLETE and Rand(3) == 0) then
			InflictDamage(p, Rand(5) + 2, R_POISON, "mysterious gas")
		end
	end
end


function GasPumpEvent(e, p)	
	if (e == LE_OUTER_USE) then
		if (QuestStatus(QUEST_TORIN) < Q_COMPLETE) then
			AddMessage('You hear hollow rumble as the pump starts.')
			QuestModify(QUEST_TORIN, Q_COMPLETE)
		else
			AddMessage('You can\'t seem to stop the pump.')
		end
	end
	return 1
end


function CreateTodin(x, y)
	local todin = Guardian(CN_TODIN, GID_DWARVEN_GUARDIAN, x, y, 6, 4)
	SetEventHandler(todin, 'TodinHandler')
end


function TodinHandler(e, t, p, v)
	if (e == LE_CHAT) then
		AddMessage("'Give me your weapon, and I'll make it the best!'")
	elseif (e == LE_GIVE_ITEM) then
		local im, brt, wt, it, count, name = GetItemParam(v)
		if (BinaryAND(im, IM_WEAPON)) then
			if (BinaryAND(brt, BR_COLD + BR_FIRE + BR_ORCSLAYER)) then
				AddMessage("'This weapon's good enough!'")
			else
				if (AskQuestion("'I need 450 gp to improve this weapon. Do you agree?'", "esc y n", "yes", "no") == 'y') then
					if (MoneyOperation(p, -450) >= 0) then
						MoneyOperation(t, 450)
						local res = Rand(3)
						if ( res == 0) then
							SetItemBrand(v, BR_COLD)
						elseif (res == 1) then
							SetItemBrand(v, BR_FIRE)
						else 
							SetItemBrand(v, BR_ORCSLAYER)
						end
						AddMessage("'Thank you!'")
					else
						AddMessage("'But you haven't enough money!'")
					end
				else
					AddMessage("'Don't waste my time!'")
				end
			end
		else
			AddMessage("'Sorry, I don't need this.'")
		end
	end
	return 0
end

function CreateTorin(x, y)
	local torin = Guardian(CN_TORIN, GID_DWARVEN_GUARDIAN, x, y)
	SetEventHandler(torin, 'TorinHandler')
	local pickaxe = CreateObject('XPickAxe')
	torin_award = GetObjectGUID(pickaxe)
	GiveObjectToCreature(pickaxe, torin)
	GiveObjectToCreature(CreateObject('XDwarfCrown'), torin)
	GiveObjectToCreature(CreateObject('XTorinShield'), torin)
	GiveObjectToCreature(CreateObject('XTorinAxe'), torin)
end

function TorinHandler(e, t, p, v)
	if (e == LE_CHAT) then
		local qs = QuestStatus(QUEST_TORIN)	
		if (qs == Q_UNKNOWN) then
			AddMessage("Hello, brave hero. As you know, we dwarves mine our treasures deep from the ground. Some time ago, one of our mine was filled by a mysterious gas, which slowly kills all living things. It is oozing from the rocks. We have gas pump there, but there is no one who can switch this pump on. Please solve this problem.")
			QuestModify(QUEST_TORIN, Q_KNOWN)
		elseif (qs == Q_KNOWN) then
			AddMessage("You haven\'t completed my previous request? Hmm... GET OUT OF HERE!")
		elseif (qs == Q_COMPLETE) then
			AddMessage("Thank you for your great help.")
			QuestModify(QUEST_TORIN, Q_CLOSED)
			if (GiveAward(t, torin_award, p)) then
				AddMessage('Take this tool as a reward.')
			end
		else
			AddMessage("Thank you for your help.")
		end
	elseif (e == LE_SAVE) then
		StoreInt(torin_award)
	elseif (e == LE_LOAD) then
		torin_award = RestoreInt()
	end
	return 1
end


-----------------------------------------------------------
-------------------- MUSHROOM CAVE ------------------------

--           000  entr cave(1)
--            |
--           000  demon cave (2)
--          / |  
-- misc(3)000 | 
--         | 000 mushroom cave (5)
--         |
--        000 kobold cave (4)

function MakeMushroomCave()
	CreateLocation(L_MUSHROOMS_CAVE1, "MC:1", "Mushroom Caves Level 1", CAVE)
		Way(UP, L_MAIN)
		Way(DOWN, L_MUSHROOMS_CAVE2)
		Settle(CR_INSECT + CR_REPTILE, CRL_VERY_LOW)
		Creature(CN_SMALL_SNAKE)
		Creature(CN_SMALL_SNAKE)
		Creature(CN_SMALL_SNAKE)
		Creature(CN_SMALL_SNAKE)
		Creature(CN_SPIDER)
		Creature(CN_SPIDER)
		Creature(CN_SPIDER)				

	CreateLocation(L_MUSHROOMS_CAVE2, "MC:2", "Mushroom Caves Level 2", CAVE)
		Way(UP, L_MUSHROOMS_CAVE1)
		Way(DOWN, L_MUSHROOMS_CAVE3)
		Way(DOWN, L_MUSHROOMS_CAVE5)
		CreateBeelzevile()
		Settle(CR_INSECT + CR_REPTILE, CRL_VERY_LOW)

	CreateLocation(L_MUSHROOMS_CAVE3, "KC:1", "Kobold Cavern Level 1", CAVE)
		Way(UP, L_MUSHROOMS_CAVE2)
		Way(DOWN, L_MUSHROOMS_CAVE4)
		Settle(CR_KOBOLD + CR_INSECT + CR_REPTILE, CRL_VERY_LOW)

	CreateLocation(L_MUSHROOMS_CAVE4, "KC:2", "Kobold Cavern Level 2", CAVE)
		Way(UP, L_MUSHROOMS_CAVE3)
		Settle(CR_KOBOLD, CRL_LOW + CRL_VERY_LOW)
		Creature(CN_MAGNUSH)

	CreateLocation(L_MUSHROOMS_CAVE5, "MC:3", "Mushroom Caves Level 3", CAVE)
		Way(UP, L_MUSHROOMS_CAVE2)
		EventPlace('MushroomCaveEvent')	
		CreateTimerEvent('SpawnMushroomEvent', 40000)
end

function MushroomCaveEvent(e, p)
	if (e == LE_MOVE) then
		if (isHero(p) and Rand(3) == 0) then
			AddMessage("You feel power swirling in the air...")
		end
		if (Rand(30) == 0) then
			local st = Rand(S_EOF)
			if (GetStats(p, st) > 5) then
				ChangeStats(p, st, -1)
			end
		end
	end
end

function SpawnMushroomEvent(l)
	CreateMushroom(l)
	return 1
end

function CreateBeelzevile()
	local demon = Creature(CN_BEELZEVILE)
	SetEventHandler(demon, 'BeelzevileHandler')
	GiveObjectToCreature(CreateObject('XGreatElementalRing'), demon)
end

function BeelzevileHandler(e, t, p, v)
	if (e == LE_DIE) then
		QuestModify(QUEST_ELDER, Q_COMPLETE)
	end
	return 0
end

-----------------------------------------------------------
-------------------- SMALL CAVE ---------------------------
function MakeSmallCave()
	CreateLocation(L_SMALL_CAVE1, "SmCv:1", "Small Cave Level 1", CAVE)
		Way(UP, L_MAIN)
		Way(DOWN, L_SMALL_CAVE2)
--		Settle(CR_RAT + CR_FELINE + CR_INSECT, CRL_VERY_LOW)

	CreateLocation(L_SMALL_CAVE2, "SmCv:2", "Small Cave Level 2", CAVE)
		SetPattern(80, 20,
		"################################################################################" ..
		"################################################################################" ..
		"######################,,,,,,,###################################################" ..
		"###################,,,,,,<,,,,,,###########################S,,,,,,,,############" ..
		"####################,,,,,,,,,,##############,,,,,,,,,,,,,,+,,,,,,,,,############" ..
		"####################,###,,,,################,##############,,,,,,,,,############" ..
		"####################,#######################,##############,,,,,,,,,############" ..
		"####################,#######################,###################+###############" ..
		"################,,,,,#######################,###################,###############" ..
		"##############,,,,,,,##############,,,,,,,,,,,##################+###############" ..
		"############,,,,,,,,,,,#################,,,,,,,,#############,,,,,,,############" ..
		"##############,,,,,,,################,,,,,,,,,,,,,,##########,,,,A,B############" ..
		"#############,,,,,,,,,############,,,,,,,,,,,,,,#############,,,,,,,############" ..
		"############,,,,,,,,############,,,,,,,,,,,,,,,,,,##############+###############" ..
		"#############,,,,,,################,,,,,,,,,,,,#################,###############" ..
		"##################,,,,,,#############,##########################,#####$$########" ..
		"#######################,,,,,,,,,,,,,,,##########################,,,,,+$~########" ..
		"################################################################################" ..
		"################################################################################" ..
		"################################################################################" )
	
		AddTranslation("A", "Furniture(x, y, xBROWN, '~', 'a table')")
		AddTranslation("B", "Furniture(x, y, xBROWN, '~', 'a bed')")
		AddTranslation("$", "Treasure(x, y, 20)")
		AddTranslation("~", "Chest(x, y)")
		AddTranslation("<", "Way(UP, L_SMALL_CAVE1, x, y)")
		AddTranslation("S", "SmallCaveQuestPersons(x, y)")
		DrawPattern(0, 0)			
--		Creature(CN_ROTMOTH)
end

function SmallCaveQuestPersons(x, y)
	Guardian(CN_GIANA, GID_GIANA, x + 1, y, 8, 4)
	Guardian(CN_ROTMOTH, GID_ROTMOTH, x + 1, y, 8, 4)
	EventPlace(x, y, 5, 2, 'SmallCaveEvent')
end

small_cave_first_visit = 0

function SmallCaveEvent(e, p)

	if (not isHero(p)) then 
		return 
	end
	
	local bandit = FindCreature(L_SMALL_CAVE2, GID_ROTMOTH)
	local giana = FindCreature(L_SMALL_CAVE2, GID_GIANA)
	
	if (e == LE_MOVE and isHero(p)) then
--[[		local c = FindCreature(L_DWARFCITY, GID_DWARVEN_GUARDIAN, 75, 1, 4, 2)
		if (not isEnemy(c, p)) then
			DvCityEventMoveCount = DvCityEventMoveCount + 1
			if (DvCityEventMoveCount < 3) then
				AddMessage("'Leave here immediately!'")
			elseif (DvCityEventMoveCount == 3) then
				AddMessage("'This is your last chance to leave!'")
			elseif (DvCityEventMoveCount > 3) then
				SetItEnemyFor(p, c)
			end
		end
]]--		
	elseif (e == LE_MOVE_IN and isHero(p)) then
		if (small_cave_first_visit == 0) then
			AddMessage("Halt! Don't move anymore or I'll kill her!")
			AddMessage("Bring me 150 golden coins, run away quikly and I probably give her a mercy!")
			small_cave_first_visit = 1
			if (MoneyOperation(p, 0) >= 150) then
				if (AskQuestion("Pay him right now?", "esc y n", "yes", "no") == 'y') then
					MoneyOperation(p, -150)
				end
			end
		else
			
		end
		
	elseif (e == LE_MOVE_OUT and isHero(p)) then
		if (small_cave_first_visit == 1) then
			AddMessage("Remember! 150 golden coins!")
		else
		end
	elseif (e == LE_SAVE) then
		StoreInt(small_cave_first_visit)
	elseif (e == LE_LOAD) then
		small_cave_first_visit = RestoreInt()
	end
end

-----------------------------------------------------------
-------------------- RAT'S CELLAR ---------------------------
function MakeRatCellar()
	CreateLocation(L_RATCELLAR, "RC:1", "Rat's cellar", CAVE)
		SetPattern(80, 20,
		"################################################################################" ..
		"#A,,,,,,,,,,,######################################################A,,,,,,,,,,,#" ..
		"#,,,,,,,,,,,,+,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,+,,,,,,,,,,,,#" ..
		"#,,C,,,,,,,,,###########################,##########################,,,,,,,,,D,,#" ..
		"#,,,,,,,,,,,,##,,,,,,,,,,###########,,,#,#,,,,,,,,,,,,,,,,,,,,,,,,#,,,,,,,,,,,,#" ..
		"#,,,,,,,,,,,,##,###,########,,,,##,,,,,#,#,,,,,,,,,##############,#,,,,,,,,,,,,#" ..
		"###############,###,####,,,,,,,,##,#,,,#,#,,,,,,,,,####,,,,,,,~##,##############" ..
		"##B,,,##B,,,###,###,####,###,,,,##,#,,,#,#,,,,,,,,,####,,,,######,,,,,,,,,,,,,##" ..
		"##,,,,##~,,,+,,,###,####,####,####,#####,##############,,,,,,,,,#############,##" ..
		"##,,,,##,,,,#######,####,####,####,###,,,,,####################,########,,###,##" ..
		"##,,,,##~,,,#######,####,####,###,,,,,,,<,,######,,,,,,,,####,,,,,,,,,,,,,###,##" ..
		"##,################,####,####,####,###,,,,,,#####,###,##,####,###,###########,##" ..
		"##,,,,,,,,,,,,,,,,,,####,####,####,#####,##,#####,###,#,,,,,#,##B,,,,,,,,,,,,,##" ..
		"###################,,,,,,####,###,,,####,##,#####,###,#,,,,,#,##,,,#############" ..
		"#A,,,,,,,,,,,################,,,,,,,####,##,#####,###,#######,##,,,#A,,,,,,,,,,#" ..
		"#,,,,,,,,,,,,####################,,,####,##,,,,,,,###,,,,,,,,,##,,,#,,,,,,,,,,,#" ..
		"#,,F,,,,,,,,,###########################,###########################,,,,,,,,E,,#" ..
		"#,,,,,,,,,,,,+,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,+,,,,,,,,,,,#" ..
		"#,,,,,,,,,,,,#######################################################,,,,,,,,,,,#" ..
		"################################################################################")
		AddTranslation("<", "Way(UP, L_MAIN, x, y)")
		AddTranslation("~", "Chest(x, y)")
		AddTranslation("A", "for i = 1, 8 do Creature(CN_RAT, x, y, 12, 4) end")
		AddTranslation("B", "for i = 1, 2 do Creature(CN_GHOST, x, y, 4, 4) end")
		local trnd = Rand(4)
		if (trnd == 0) then 
			AddTranslation("C", "DropItem(CreateObject('XForestBrotherCloak'), x, y)")
		elseif (trnd == 1) then
			AddTranslation("D", "DropItem(CreateObject('XForestBrotherCloak'), x, y)")
		elseif (trnd == 2) then
			AddTranslation("E", "DropItem(CreateObject('XForestBrotherCloak'), x, y)")
		elseif (trnd == 3) then
			AddTranslation("F", "DropItem(CreateObject('XForestBrotherCloak'), x, y)")
		end
		DrawPattern(0, 0)
		Settle(CR_RAT, CRL_VERY_LOW)
		
end



----------------------------------------------------------------
-------------------- EXTINCT VULCANO ---------------------------
function MakeVulcano()
	CreateLocation(L_EXTINCT_VOLCANO, "Volcano", "Crater of an Extinct Volcano", CAVE)
		SetPattern(80, 20,
		"################################################################################" ..
		"########################,#######################################################" ..
		"####,,,,,###############,,,####################################,,,##############" ..
		"########,,,################,,################################,,###,########,,###" ..
		"##########,,,,,,#############,#########,,,,###############,,,######,####,,,#####" ..
		"################,,###########,,#####,,,,,,,,,,#########,,,##########,,,,########" ..
		"##################,,,########,,,,,,,,,,,,,,,,,,,,###,,,#########################" ..
		"#################,##,,#######,,,,,,,,,,,,,,,,,,,,,,,,,,#########################" ..
		"#########,,,###,,#####,,,,#,,,,,,,,===,,,===,,,,,,,,,,,#########################" ..
		"########,###,,,########,,,,,,,,,,,,,,,======,,,,,,,,,,,,########################" ..
		"########################,,,,,,,,,,,,,,======,,,,,,,,,,,,,###,###################" ..
		"#########################,,,,,,,,,,,===========,,,,,,,,,##,,,,############<#####" ..
		"##########################,,,,,,,,,,,,==========,,,,,,,,,,,,,,,##########,######" ..
		"#######################,,,,,,,,,,,,,============,,,,,,,,,,#####,########,#######" ..
		"#############,#######,,,,,,,,,,,,,,,,,=========,,,,,,,,,########,,####,,########" ..
		"##########,,,,,,,,,,,,,,,,,,,,,,,,,,===,,,,,,,,,,,,,,,,###########,,,,#,########" ..
		"#######,,,####################,,,,,,,,,,,,,,,,,,,,,,,,##################,#######" ..
		"#####,,#######################,,,,,,,,,,,,,,,,,,,,,,,####################,,#####" ..
		"####,################################,,,,,,,,,,,,,,,#######################,,,,#" ..
		"################################################################################")
		AddTranslation("<", "Way(UP, L_MAIN, x, y)")
		AddTranslation("=", LAVA)
		DrawPattern(0, 0)
		GiveObjectToCreature(CreateObject('XBlackClub'), Creature(CN_XSHEE_VOO))
end



-------------------------------------------------------------------------
-------------------- AHK_ULAN CASTLE AND WAY  ---------------------------
function MakeWizardDungeon()

	CreateLocation(L_WIZARD_DUNGEON1, "AD:1", "Ahk-Ulan's dungeons level 1", DUNGEON)
		Way(UP, L_MAIN)
		Way(DOWN, L_WIZARD_DUNGEON2)
		Settle(CR_UNDEAD + CR_BLOB + CR_INSECT + CR_REPTILE + CR_RAT + CR_FELINE + CR_CANINE, CRL_VERY_LOW)

	CreateLocation(L_WIZARD_DUNGEON2, "AD:2", "Ahk-Ulan's dungeons level 2", DUNGEON)
		Way(UP, L_WIZARD_DUNGEON1)
		Way(DOWN, L_WIZARD_DUNGEON3)
		Settle(CR_UNDEAD + CR_BLOB + CR_INSECT + CR_REPTILE + CR_RAT + CR_FELINE + CR_CANINE, CRL_VERY_LOW)

	CreateLocation(L_WIZARD_DUNGEON3, "AD:3", "Ahk-Ulan's dungeons level 3", DUNGEON)
		Way(UP, L_WIZARD_DUNGEON2)
		Way(DOWN, L_WIZARD_DUNGEON4)
		Settle(CR_UNDEAD + CR_BLOB + CR_INSECT + CR_REPTILE + CR_RAT + CR_FELINE + CR_CANINE, CRL_VERY_LOW)

	CreateLocation(L_WIZARD_DUNGEON4, "AD:4", "Ahk-Ulan's dungeons level 4", DUNGEON)
		Way(UP, L_WIZARD_DUNGEON3)
		Way(DOWN, L_WIZARD_DUNGEON5)
		Settle(CR_UNDEAD + CR_BLOB + CR_INSECT + CR_REPTILE + CR_RAT + CR_FELINE + CR_CANINE, CRL_VERY_LOW)

	CreateLocation(L_WIZARD_DUNGEON5, "AD:5", "Ahk-Ulan's dungeons level 5", DUNGEON)
		Way(UP, L_WIZARD_DUNGEON4)
		Way(DOWN, L_AHKULAN_CASTLE)
		Settle(CR_UNDEAD + CR_BLOB + CR_INSECT + CR_REPTILE + CR_RAT + CR_FELINE + CR_CANINE, CRL_VERY_LOW)

	CreateLocation(L_AHKULAN_CASTLE, "AC", "Ahk-Ulan's castle", CAVE)
		SetPattern(80, 20,
		"################################################################################" ..
		"##################==############################################################" ..
		"##,,,,,,,,#####,,,==############################################################" ..
		"##,,,,,,,,,,,,,,,==#############################################################" ..
		"#,,,,,,,,,,,,,,,,==#############################################################" ..
		"#,,,,,,,,,,,,,,,,==#############################################################" ..
		"#,,,,,,,,,,,,,,,==#####################h;;;;;;##################################" ..
		"#,,,rrrrrrrrr,,,==#;;##################;;;;;;;;#################################" ..
		"#,,,,,,,,,,,,,,h==#;h##################h;;;;;;;h;;h#############################" ..
		"#,,<;;;;;;;;;;;;;;+;;+;;;;;;;;;;;;;;;;;;;;_;;;;;;A;#############################" ..
		"#,,,,,,,,,,,,,,h==#;h##################h;;;;;;;h;;h#############################" ..
		"#,,,,,,,,,,,,,,,==#;;##################;;;;;;;;#################################" ..
		"#,,,,,,,,,,,,,,,==#####################h;;;;;;##################################" ..
		"#,,,,,,,,,,,,,,,,==#############################################################" ..
		"#,,,,,,,,,,,,,,,,==#############################################################" ..
		"#,,,,,,,,,,,,,,,,==#############################################################" ..
		"#,,,,,,,#,,,,,,,,,==############################################################" ..
		"##,,,,####,,###,,,==############################################################" ..
		"################################################################################" ..
		"################################################################################" )
		AddTranslation("<", "Way(UP, L_WIZARD_DUNGEON5, x, y)")
		AddTranslation("h", "Guardian(CN_DEATH_KNIGHT, GID_AHKULAN_GUARDIAN, x, y)")
		AddTranslation("A", "CreateAhkUlan(x, y) Furniture(x, y, xDARKGRAY, '~', 'the black throne from pure obsidian')")
		AddTranslation("_", "Altar(x, y, D_DEATH)")
		AddTranslation("r", "Creature(CN_HUGE_RAT, x, y)")
		DrawPattern(0, 0)
		
end

function CreateAhkUlan(x, y)
	local ahkulan = Guardian(CN_AHKULAN, GID_AHKULAN_GUARDIAN, x, y)
	SetEventHandler(ahkulan, 'AhkUlanHandler')
end


function AhkUlanHandler(e, t, p, v)
	if (e == LE_CHAT) then
		local qs = QuestStatus(QUEST_ANCIENT_PART)
		if (qs == Q_UNKNOWN) then
			AddMessage("Hello, brave hero.")
			AddMessage("Some years ago, some evil wizards destroyed my tower.")
			AddMessage("Now I wait here gaining strength and planning my revenge.")
			AddMessage("I am searching for 3 parts to an ancient machine.")
			AddMessage("Bring them to me and I will reward you well.")
			QuestModify(QUEST_ANCIENT_PART, Q_KNOWN)
		elseif (qs == Q_KNOWN) then
			AddMessage("Don't disturb me before completing my quest, puny mortal!")
		end
	elseif (e == LE_GIVE_ITEM) then
		local im, brt, wt, it, count, name = GetItemParam(v)
		if (it == IT_ANCIENTMACHINEPART) then
			if (count == 3) then
				AddMessage("Very nice job, servant!")
				QuestModify(QUEST_ANCIENT_PART, Q_CLOSED)
				return 1
			else
				AddMessage("PLEASE! Return with THREE... THREE parts of an ancient machine!")
				return 0
			end
		else
			AddMessage("Are you jeering at me?")
			return 0
		end
	end
	return 1
end
