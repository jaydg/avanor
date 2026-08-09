
-----------------------------------------------------------
-------------------- DWARVEN CITY -------------------------

function MakeDwarvenCity()
	CreateLocation(XLocation.DWARFCITYCAVE1, "PDC:1", "Path to the Dwarven City Level 1", DUNGEON)
		Way(UP, XLocation.MAIN)
		Way(DOWN, XLocation.DWARFCITYCAVE2)
		Settle(CREATURE_CLASS.CR_UNDEAD + CREATURE_CLASS.CR_BLOB + CREATURE_CLASS.CR_INSECT + CREATURE_CLASS.CR_REPTILE + CREATURE_CLASS.CR_RAT + CREATURE_CLASS.CR_FELINE + CREATURE_CLASS.CR_CANINE, CREATURE_LEVEL.CRL_VERY_LOW)

	CreateLocation(XLocation.DWARFCITYCAVE2, "PDC:2", "Path to the Dwarven City Level 2", DUNGEON)
		Way(UP, XLocation.DWARFCITYCAVE1)
		Way(DOWN, XLocation.DWARFCITYCAVE3)
		Settle(CREATURE_CLASS.CR_UNDEAD + CREATURE_CLASS.CR_BLOB + CREATURE_CLASS.CR_INSECT + CREATURE_CLASS.CR_REPTILE + CREATURE_CLASS.CR_RAT + CREATURE_CLASS.CR_FELINE + CREATURE_CLASS.CR_CANINE, CREATURE_LEVEL.CRL_LOW)

	CreateLocation(XLocation.DWARFCITYCAVE3, "PDC:3", "Path to the Dwarven City Level 3", DUNGEON)
		Way(UP, XLocation.DWARFCITYCAVE2)
		Way(DOWN, XLocation.DWARFCITYCAVE4)
		Settle(CREATURE_CLASS.CR_UNDEAD + CREATURE_CLASS.CR_BLOB + CREATURE_CLASS.CR_INSECT + CREATURE_CLASS.CR_REPTILE + CREATURE_CLASS.CR_RAT + CREATURE_CLASS.CR_FELINE + CREATURE_CLASS.CR_CANINE, CREATURE_LEVEL.CRL_LOW)

	CreateLocation(XLocation.DWARFCITYCAVE4, "PDC:4", "Path to the Dwarven City Level 4", DUNGEON)
		Way(UP, XLocation.DWARFCITYCAVE3)
		Way(DOWN, XLocation.DWARFCITYCAVE5)
		Settle(CREATURE_CLASS.CR_UNDEAD + CREATURE_CLASS.CR_BLOB + CREATURE_CLASS.CR_INSECT + CREATURE_CLASS.CR_REPTILE + CREATURE_CLASS.CR_RAT + CREATURE_CLASS.CR_FELINE + CREATURE_CLASS.CR_CANINE, CREATURE_LEVEL.CRL_LOW)

	CreateLocation(XLocation.DWARFCITYCAVE5, "PDC:5", "Path to the Dwarven City Level 5", DUNGEON)
		Way(UP, XLocation.DWARFCITYCAVE4)
		Way(DOWN, XLocation.DWARFCITYCAVE6)
		Settle(CREATURE_CLASS.CR_UNDEAD + CREATURE_CLASS.CR_BLOB + CREATURE_CLASS.CR_INSECT + CREATURE_CLASS.CR_REPTILE + CREATURE_CLASS.CR_RAT + CREATURE_CLASS.CR_FELINE + CREATURE_CLASS.CR_CANINE, CREATURE_LEVEL.CRL_LOW)

	CreateLocation(XLocation.DWARFCITYCAVE6, "PDC:6", "Path to the Dwarven City Level 6", DUNGEON)
		Way(UP, XLocation.DWARFCITYCAVE5)
		Way(DOWN, XLocation.DWARFCITY)
		Settle(CREATURE_CLASS.CR_UNDEAD + CREATURE_CLASS.CR_BLOB + CREATURE_CLASS.CR_INSECT + CREATURE_CLASS.CR_REPTILE + CREATURE_CLASS.CR_RAT + CREATURE_CLASS.CR_FELINE + CREATURE_CLASS.CR_CANINE + CREATURE_CLASS.CR_GOBLIN + CREATURE_CLASS.CR_KOBOLD, CREATURE_LEVEL.CRL_LOW)
		DropItem(CreateObject("XCookingSet"))


	CreateLocation(XLocation.DWARFCITY, "DvCty", "Dwarven City", CAVE)
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
		AddTranslation("1", XTileType.GOLDEN_FLOOR)
		AddTranslation(";", XTileType.OBSIDIAN_FLOOR)
		AddTranslation(":", XTileType.BRIDGE)
		AddTranslation("<", function(x, y) Way(UP, XLocation.DWARFCITYCAVE6, x, y) end)
		AddTranslation("A", function(x, y) Guardian('dwarf_guard', GROUP_ID.GID_DWARVEN_GUARDIAN, x, y) end)
		AddTranslation("S", function(x, y) BuildShop(x, y, 9, 3, ITEM_MASK.IM_ARMOUR + ITEM_MASK.IM_WEAPON + ITEM_MASK.IM_POTION + ITEM_MASK.IM_BOOK + ITEM_MASK.IM_SCROLL + ITEM_MASK.IM_NECK + ITEM_MASK.IM_MISSILE + ITEM_MASK.IM_MISSILEW, 'Toberin, the dwarwen shopkeeper') end)
		AddTranslation("B", function(x, y) Furniture(x, y, xColor.xBROWN, '~', 'a dinner table') end)
		AddTranslation("C", function(x, y) Furniture(x, y, xColor.xBROWN, '~', 'a round table') end)
		AddTranslation("D", function(x, y) Furniture(x, y, xColor.xLIGHTRED, '~', 'a royal bed') end)
		AddTranslation("X", function(x, y) CreateTorin(x, y) Furniture(x, y, xColor.xYELLOW, '~', 'the throne of the Dwarven Kingdom') end)
		AddTranslation("_", function(x, y) Altar(x, y, XDeity.LIFE) end)
		AddTranslation("P", function(x, y) for i = 1, 6 do Guardian('dwarf', GROUP_ID.GID_DWARVEN_GUARDIAN, x, y, 32, 16) end end)
		AddTranslation("T", function(x, y) CreateTodin(x, y) end)
		AddTranslation("E", function(x, y) EventPlace(x - 1, y, 5, 2, 'DvCityEvent1') end)
		AddTranslation("F", function(x, y) Way(DOWN, XLocation.DWARFTREASURE, x, y) end)
		AddTranslation("G", function(x, y) Way(DOWN, XLocation.GASMINE1, x, y) end)
		DrawPattern(0, 0)

	CreateLocation(XLocation.DWARFTREASURE, "DvTr", "Dwarven Treasure", CAVE)
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
		AddTranslation(".", XTileType.GOLDEN_FLOOR)
		AddTranslation("<", function(x, y) Way(UP, XLocation.DWARFCITY, x, y) end)
		AddTranslation("$", function(x, y) Treasure(x, y, 500) end)
		AddTranslation("~", function(x, y) Chest(x, y) end)
		AddTranslation("A", function(x, y) DropItem(CreateObject('XAncientMachinePart'), x, y) end)
		DrawPattern(0, 0)
		

	CreateLocation(XLocation.GASMINE1, "GM:1", "Gassed Mine level 1", DUNGEON)
		Way(UP, XLocation.DWARFCITY)
		Way(DOWN, XLocation.GASMINE2)
--		Settle(CREATURE_CLASS.CR_BLOB, CREATURE_LEVEL.CRL_VERY_LOW)
		EventPlace('GasMineEvent')

	CreateLocation(XLocation.GASMINE2, "GM:2", "Gassed Mine level 2", DUNGEON)
		Way(UP, XLocation.GASMINE1)
		Way(DOWN, XLocation.GASMINE3)
--		Settle(CREATURE_CLASS.CR_BLOB, CREATURE_LEVEL.CRL_VERY_LOW)
		EventPlace('GasMineEvent')

	CreateLocation(XLocation.GASMINE3, "GM:3", "Gassed Mine level 3", DUNGEON)
		Way(UP, XLocation.GASMINE2)
--		Settle(CREATURE_CLASS.CR_BLOB, CREATURE_LEVEL.CRL_VERY_LOW)
		EventPlace('GasMineEvent')
		OuterObject(xColor.xBLUE, '#', 'a gas pump', 'GasPumpEvent')
end
	

DvCityEventMoveCount = 0

function DvCityEvent1(e, p)
	if (e == LUA_EVENT.LE_MOVE and isHero(p)) then
		local c = FindCreature(XLocation.DWARFCITY, GROUP_ID.GID_DWARVEN_GUARDIAN, 75, 1, 4, 2)
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
	elseif (e == LUA_EVENT.LE_MOVE_IN and isHero(p)) then
		local c = FindCreature(XLocation.DWARFCITY, GROUP_ID.GID_DWARVEN_GUARDIAN, 75, 1, 4, 2)
		if (not isEnemy(c, p)) then
			AddMessage("'Halt! You aren't allowed to enter.'")
		end
		DvCityEventMoveCount = 1
	elseif (e == LUA_EVENT.LE_MOVE_OUT and isHero(p)) then
		DvCityEventMoveCount = 0
	elseif (e == LUA_EVENT.LE_SAVE) then
		StoreInt(DvCityEventMoveCount)
	elseif (e == LUA_EVENT.LE_LOAD) then
		DvCityEventMoveCount = RestoreInt()
	end

end

torin_award = 0;

function GasMineEvent(e, p)
	if (e == LUA_EVENT.LE_MOVE) then
		if (QuestStatus(QUEST_TORIN) < QUEST.Q_COMPLETE and Rand(3) == 0) then
			InflictDamage(p, Rand(5) + 2, RESISTANCE.R_POISON, "mysterious gas")
		end
	end
end


function GasPumpEvent(e, p)	
	if (e == LUA_EVENT.LE_OUTER_USE) then
		if (QuestStatus(QUEST_TORIN) < QUEST.Q_COMPLETE) then
			AddMessage('You hear hollow rumble as the pump starts.')
			QuestModify(QUEST_TORIN, QUEST.Q_COMPLETE)
		else
			AddMessage('You can\'t seem to stop the pump.')
		end
	end
	return 1
end


function CreateTodin(x, y)
	local todin = Guardian("todin", GROUP_ID.GID_DWARVEN_GUARDIAN, x, y, 6, 4)
	SetEventHandler(todin, 'TodinHandler')
end


function TodinHandler(e, t, p, v)
	if (e == LUA_EVENT.LE_CHAT) then
		AddMessage("'Give me your weapon, and I'll make it the best!'")
	elseif (e == LUA_EVENT.LE_GIVE_ITEM) then
		local im, brt, wt, it, count, name = GetItemParam(v)
		if (BinaryAND(im, ITEM_MASK.IM_WEAPON)) then
			if (BinaryAND(brt, BRAND_TYPE.BR_COLD + BRAND_TYPE.BR_FIRE + BRAND_TYPE.BR_ORCSLAYER)) then
				AddMessage("'This weapon's good enough!'")
			else
				if (AskQuestion("'I need 450 gp to improve this weapon. Do you agree?'", "esc y n", "yes", "no") == 'y') then
					if (MoneyOperation(p, -450) >= 0) then
						MoneyOperation(t, 450)
						local res = Rand(3)
						if ( res == 0) then
							SetItemBrand(v, BRAND_TYPE.BR_COLD)
						elseif (res == 1) then
							SetItemBrand(v, BRAND_TYPE.BR_FIRE)
						else 
							SetItemBrand(v, BRAND_TYPE.BR_ORCSLAYER)
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
	local torin = Guardian("torin", GROUP_ID.GID_DWARVEN_GUARDIAN, x, y)
	SetEventHandler(torin, 'TorinHandler')
	local pickaxe = CreateObject('XPickAxe')
	torin_award = GetObjectGUID(pickaxe)
	GiveObjectToCreature(pickaxe, torin)
	GiveObjectToCreature(CreateObject('XDwarfCrown'), torin)
	GiveObjectToCreature(CreateObject('XTorinShield'), torin)
	GiveObjectToCreature(CreateObject('XTorinAxe'), torin)
end

function TorinHandler(e, t, p, v)
	if (e == LUA_EVENT.LE_CHAT) then
		local qs = QuestStatus(QUEST_TORIN)	
		if (qs == QUEST.Q_UNKNOWN) then
			AddMessage("Hello, brave hero. As you know, we dwarves mine our treasures deep from the ground. Some time ago, one of our mine was filled by a mysterious gas, which slowly kills all living things. It is oozing from the rocks. We have gas pump there, but there is no one who can switch this pump on. Please solve this problem.")
			QuestModify(QUEST_TORIN, QUEST.Q_KNOWN)
		elseif (qs == QUEST.Q_KNOWN) then
			AddMessage("You haven\'t completed my previous request? Hmm... GET OUT OF HERE!")
		elseif (qs == QUEST.Q_COMPLETE) then
			AddMessage("Thank you for your great help.")
			QuestModify(QUEST_TORIN, QUEST.Q_CLOSED)
			if (GiveAward(t, torin_award, p)) then
				AddMessage('Take this tool as a reward.')
			end
		else
			AddMessage("Thank you for your help.")
		end
	elseif (e == LUA_EVENT.LE_SAVE) then
		StoreInt(torin_award)
	elseif (e == LUA_EVENT.LE_LOAD) then
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
	CreateLocation(XLocation.MUSHROOMS_CAVE1, "MC:1", "Mushroom Caves Level 1", CAVE)
		Way(UP, XLocation.MAIN)
		Way(DOWN, XLocation.MUSHROOMS_CAVE2)
		Settle(CREATURE_CLASS.CR_INSECT + CREATURE_CLASS.CR_REPTILE, CREATURE_LEVEL.CRL_VERY_LOW)
		Creature("small_snake")
		Creature("small_snake")
		Creature("small_snake")
		Creature("small_snake")
		Creature("spider")
		Creature("spider")
		Creature("spider")				

	CreateLocation(XLocation.MUSHROOMS_CAVE2, "MC:2", "Mushroom Caves Level 2", CAVE)
		Way(UP, XLocation.MUSHROOMS_CAVE1)
		Way(DOWN, XLocation.MUSHROOMS_CAVE3)
		Way(DOWN, XLocation.MUSHROOMS_CAVE5)
		CreateBeelzevile()
		Settle(CREATURE_CLASS.CR_INSECT + CREATURE_CLASS.CR_REPTILE, CREATURE_LEVEL.CRL_VERY_LOW)

	CreateLocation(XLocation.MUSHROOMS_CAVE3, "KC:1", "Kobold Cavern Level 1", CAVE)
		Way(UP, XLocation.MUSHROOMS_CAVE2)
		Way(DOWN, XLocation.MUSHROOMS_CAVE4)
		Settle(CREATURE_CLASS.CR_KOBOLD + CREATURE_CLASS.CR_INSECT + CREATURE_CLASS.CR_REPTILE, CREATURE_LEVEL.CRL_VERY_LOW)

	CreateLocation(XLocation.MUSHROOMS_CAVE4, "KC:2", "Kobold Cavern Level 2", CAVE)
		Way(UP, XLocation.MUSHROOMS_CAVE3)
		Settle(CREATURE_CLASS.CR_KOBOLD, CREATURE_LEVEL.CRL_LOW + CREATURE_LEVEL.CRL_VERY_LOW)
		Creature("magnush")

	CreateLocation(XLocation.MUSHROOMS_CAVE5, "MC:3", "Mushroom Caves Level 3", CAVE)
		Way(UP, XLocation.MUSHROOMS_CAVE2)
		EventPlace('MushroomCaveEvent')	
		CreateTimerEvent('SpawnMushroomEvent', 40000)
end

function MushroomCaveEvent(e, p)
	if (e == LUA_EVENT.LE_MOVE) then
		if (isHero(p) and Rand(3) == 0) then
			AddMessage("You feel power swirling in the air...")
		end
		if (Rand(30) == 0) then
			local st = Rand(XStats.COUNT)
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
	local demon = Creature("beelzevile")
	SetEventHandler(demon, 'BeelzevileHandler')
	GiveObjectToCreature(CreateObject('XGreatElementalRing'), demon)
end

function BeelzevileHandler(e, t, p, v)
	if (e == LUA_EVENT.LE_DIE) then
		QuestModify(QUEST_ELDER, QUEST.Q_COMPLETE)
	end
	return 0
end

-----------------------------------------------------------
-------------------- SMALL CAVE ---------------------------
function MakeSmallCave()
	CreateLocation(L_SMALL_CAVE1, "SmCv:1", "Small Cave Level 1", CAVE)
		Way(UP, XLocation.MAIN)
		Way(DOWN, L_SMALL_CAVE2)
--		Settle(CREATURE_CLASS.CR_RAT + CREATURE_CLASS.CR_FELINE + CREATURE_CLASS.CR_INSECT, CREATURE_LEVEL.CRL_VERY_LOW)

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
	
		AddTranslation("A", function(x, y) Furniture(x, y, xColor.xBROWN, '~', 'a table') end)
		AddTranslation("B", function(x, y) Furniture(x, y, xColor.xBROWN, '~', 'a bed') end)
		AddTranslation("$", function(x, y) Treasure(x, y, 20) end)
		AddTranslation("~", function(x, y) Chest(x, y) end)
		AddTranslation("<", function(x, y) Way(UP, L_SMALL_CAVE1, x, y) end)
		AddTranslation("S", function(x, y) SmallCaveQuestPersons(x, y) end)
		DrawPattern(0, 0)			
--		Creature("rotmoth")
end

function SmallCaveQuestPersons(x, y)
	Guardian("giana", GID_GIANA, x + 1, y, 8, 4)
	Guardian("rotmoth", GID_ROTMOTH, x + 1, y, 8, 4)
	EventPlace(x, y, 5, 2, 'SmallCaveEvent')
end

small_cave_first_visit = 0

function SmallCaveEvent(e, p)

	if (not isHero(p)) then 
		return 
	end
	
	local bandit = FindCreature(L_SMALL_CAVE2, GID_ROTMOTH)
	local giana = FindCreature(L_SMALL_CAVE2, GID_GIANA)
	
	if (e == LUA_EVENT.LE_MOVE and isHero(p)) then
--[[		local c = FindCreature(XLocation.DWARFCITY, GROUP_ID.GID_DWARVEN_GUARDIAN, 75, 1, 4, 2)
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
	elseif (e == LUA_EVENT.LE_MOVE_IN and isHero(p)) then
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
		
	elseif (e == LUA_EVENT.LE_MOVE_OUT and isHero(p)) then
		if (small_cave_first_visit == 1) then
			AddMessage("Remember! 150 golden coins!")
		else
		end
	elseif (e == LUA_EVENT.LE_SAVE) then
		StoreInt(small_cave_first_visit)
	elseif (e == LUA_EVENT.LE_LOAD) then
		small_cave_first_visit = RestoreInt()
	end
end

-----------------------------------------------------------
-------------------- RAT'S CELLAR ---------------------------
function MakeRatCellar()
	CreateLocation(XLocation.RATCELLAR, "RC:1", "Rat's cellar", CAVE)
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
		AddTranslation("<", function(x, y) Way(UP, XLocation.MAIN, x, y) end)
		AddTranslation("~", function(x, y) Chest(x, y) end)
		AddTranslation("A", function(x, y) for i = 1, 8 do Creature('rat', x, y, 12, 4) end end)
		AddTranslation("B", function(x, y) for i = 1, 2 do Creature('ghost', x, y, 4, 4) end end)
		local trnd = Rand(4)
		if (trnd == 0) then 
			AddTranslation("C", function(x, y) DropItem(CreateObject('XForestBrotherCloak'), x, y) end)
		elseif (trnd == 1) then
			AddTranslation("D", function(x, y) DropItem(CreateObject('XForestBrotherCloak'), x, y) end)
		elseif (trnd == 2) then
			AddTranslation("E", function(x, y) DropItem(CreateObject('XForestBrotherCloak'), x, y) end)
		elseif (trnd == 3) then
			AddTranslation("F", function(x, y) DropItem(CreateObject('XForestBrotherCloak'), x, y) end)
		end
		DrawPattern(0, 0)
		Settle(CREATURE_CLASS.CR_RAT, CREATURE_LEVEL.CRL_VERY_LOW)
		
end



----------------------------------------------------------------
-------------------- EXTINCT VULCANO ---------------------------
function MakeVulcano()
	CreateLocation(XLocation.EXTINCT_VOLCANO, "Volcano", "Crater of an Extinct Volcano", CAVE)
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
		AddTranslation("<", function(x, y) Way(UP, XLocation.MAIN, x, y) end)
		AddTranslation("=", XTileType.LAVA)
		DrawPattern(0, 0)
		GiveObjectToCreature(CreateObject('XBlackClub'), Creature("xshee_voo"))
end



-------------------------------------------------------------------------
-------------------- AHK_ULAN CASTLE AND WAY  ---------------------------
function MakeWizardDungeon()

	CreateLocation(XLocation.WIZARD_DUNGEON1, "AD:1", "Ahk-Ulan's dungeons level 1", DUNGEON)
		Way(UP, XLocation.MAIN)
		Way(DOWN, XLocation.WIZARD_DUNGEON2)
		Settle(CREATURE_CLASS.CR_UNDEAD + CREATURE_CLASS.CR_BLOB + CREATURE_CLASS.CR_INSECT + CREATURE_CLASS.CR_REPTILE + CREATURE_CLASS.CR_RAT + CREATURE_CLASS.CR_FELINE + CREATURE_CLASS.CR_CANINE, CREATURE_LEVEL.CRL_VERY_LOW)

	CreateLocation(XLocation.WIZARD_DUNGEON2, "AD:2", "Ahk-Ulan's dungeons level 2", DUNGEON)
		Way(UP, XLocation.WIZARD_DUNGEON1)
		Way(DOWN, XLocation.WIZARD_DUNGEON3)
		Settle(CREATURE_CLASS.CR_UNDEAD + CREATURE_CLASS.CR_BLOB + CREATURE_CLASS.CR_INSECT + CREATURE_CLASS.CR_REPTILE + CREATURE_CLASS.CR_RAT + CREATURE_CLASS.CR_FELINE + CREATURE_CLASS.CR_CANINE, CREATURE_LEVEL.CRL_VERY_LOW)

	CreateLocation(XLocation.WIZARD_DUNGEON3, "AD:3", "Ahk-Ulan's dungeons level 3", DUNGEON)
		Way(UP, XLocation.WIZARD_DUNGEON2)
		Way(DOWN, XLocation.WIZARD_DUNGEON4)
		Settle(CREATURE_CLASS.CR_UNDEAD + CREATURE_CLASS.CR_BLOB + CREATURE_CLASS.CR_INSECT + CREATURE_CLASS.CR_REPTILE + CREATURE_CLASS.CR_RAT + CREATURE_CLASS.CR_FELINE + CREATURE_CLASS.CR_CANINE, CREATURE_LEVEL.CRL_VERY_LOW)

	CreateLocation(XLocation.WIZARD_DUNGEON4, "AD:4", "Ahk-Ulan's dungeons level 4", DUNGEON)
		Way(UP, XLocation.WIZARD_DUNGEON3)
		Way(DOWN, XLocation.WIZARD_DUNGEON5)
		Settle(CREATURE_CLASS.CR_UNDEAD + CREATURE_CLASS.CR_BLOB + CREATURE_CLASS.CR_INSECT + CREATURE_CLASS.CR_REPTILE + CREATURE_CLASS.CR_RAT + CREATURE_CLASS.CR_FELINE + CREATURE_CLASS.CR_CANINE, CREATURE_LEVEL.CRL_VERY_LOW)

	CreateLocation(XLocation.WIZARD_DUNGEON5, "AD:5", "Ahk-Ulan's dungeons level 5", DUNGEON)
		Way(UP, XLocation.WIZARD_DUNGEON4)
		Way(DOWN, XLocation.AHKULAN_CASTLE)
		Settle(CREATURE_CLASS.CR_UNDEAD + CREATURE_CLASS.CR_BLOB + CREATURE_CLASS.CR_INSECT + CREATURE_CLASS.CR_REPTILE + CREATURE_CLASS.CR_RAT + CREATURE_CLASS.CR_FELINE + CREATURE_CLASS.CR_CANINE, CREATURE_LEVEL.CRL_VERY_LOW)

	CreateLocation(XLocation.AHKULAN_CASTLE, "AC", "Ahk-Ulan's castle", CAVE)
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
		AddTranslation("<", function(x, y) Way(UP, XLocation.WIZARD_DUNGEON5, x, y) end)
		AddTranslation("h", function(x, y) Guardian('death_knight', GROUP_ID.GID_AHKULAN_GUARDIAN, x, y) end)
		AddTranslation("A", function(x, y) CreateAhkUlan(x, y) Furniture(x, y, xColor.xDARKGRAY, '~', 'the black throne from pure obsidian') end)
		AddTranslation("_", function(x, y) Altar(x, y, XDeity.DEATH) end)
		AddTranslation("r", function(x, y) Creature('huge_rat', x, y) end)
		DrawPattern(0, 0)
		
end

function CreateAhkUlan(x, y)
	local ahkulan = Guardian("ahkulan", GROUP_ID.GID_AHKULAN_GUARDIAN, x, y)
	SetEventHandler(ahkulan, 'AhkUlanHandler')
end


function AhkUlanHandler(e, t, p, v)
	if (e == LUA_EVENT.LE_CHAT) then
		local qs = QuestStatus(QUEST_ANCIENT_PART)
		if (qs == QUEST.Q_UNKNOWN) then
			AddMessage("Hello, brave hero.")
			AddMessage("Some years ago, some evil wizards destroyed my tower.")
			AddMessage("Now I wait here gaining strength and planning my revenge.")
			AddMessage("I am searching for 3 parts to an ancient machine.")
			AddMessage("Bring them to me and I will reward you well.")
			QuestModify(QUEST_ANCIENT_PART, QUEST.Q_KNOWN)
		elseif (qs == QUEST.Q_KNOWN) then
			AddMessage("Don't disturb me before completing my quest, puny mortal!")
		end
	elseif (e == LUA_EVENT.LE_GIVE_ITEM) then
		local im, brt, wt, it, count, name = GetItemParam(v)
		if (it == ITEM_TYPE.IT_ANCIENTMACHINEPART) then
			if (count == 3) then
				AddMessage("Very nice job, servant!")
				QuestModify(QUEST_ANCIENT_PART, QUEST.Q_CLOSED)
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
