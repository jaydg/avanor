CAVE = 0
DUNGEON = 1
L_SMALL_CAVE1 = 55
L_SMALL_CAVE2 = 56
UP = 1
DOWN = 2



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
		AddTranslation("1", M_GOLDENFLOOR)
		AddTranslation(";", M_OBSIDIANFLOOR)
		AddTranslation(":", M_BRIDGE)
		AddTranslation("<", "Way(UP, L_DWARFCITYCAVE6, x, y)")
		AddTranslation("A", "Guardian(CN_DWARF_GUARD, GID_DWARVEN_GUARDIAN, x, y)")
		AddTranslation("S", "BuildShop(x, y, 9, 3, IM_ARMOUR + IM_WEAPON + IM_POTION + IM_BOOK + IM_SCROLL + IM_NECK + IM_MISSILE + IM_MISSILEW, 'Toberin, the dwarwen shopkeeper')")
		AddTranslation("B", "Furniture(x, y, xBROWN, '~', 'a dinner table')")
		AddTranslation("C", "Furniture(x, y, xBROWN, '~', 'a round table')")
		AddTranslation("D", "Furniture(x, y, xLIGHTRED, '~', 'a royal bad')")
		AddTranslation("X", "CreateTorin(x, y) Furniture(x, y, xYELLOW, '~', 'the throne of the Dwarven Kingdom')")
		AddTranslation("_", "Altar(x, y, D_LIFE)")
		AddTranslation("P", "for i = 1, 6 do Guardian(CN_DWARF, GID_DWARVEN_GUARDIAN, x, y, 32, 16) end")
		AddTranslation("T", "Guardian(CN_TODIN, GID_DWARVEN_GUARDIAN, x, y, 6, 4)")
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
		AddTranslation(".", M_GOLDENFLOOR)
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
		OuterObject(x, y, xBLUE, '#', 'a gas pump', 'GasPumpEvent')
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


gas_pump_quest = Q_UNKNOWN;
--gas_pump_quest = Q_COMPLETE;
torin_award = 0;

function GasMineEvent(e, p)
	if (e == LE_MOVE) then
		if (gas_pump_quest < Q_COMPLETE and Rand(3) == 0) then
			InflictDamage(p, Rand(5) + 2, R_POISON, "mysterious gas")
		end
	end
end


function GasPumpEvent(e, p)	
	if (e == LE_OUTER_USE) then
		if (gas_pump_quest < Q_COMPLETE) then
			AddMessage('You hear hollow rumble as the pump starts.')
			gas_pump_quest = Q_COMPLETE
		else
			AddMessage('You can\'t seem to stop the pump.')
		end
	end
	return 1
end

function CreateTorin(x, y)
	local torin = Guardian(CN_TORIN, GID_DWARVEN_GUARDIAN, x, y)
	SetEventHandler(torin, 'TorinHandler')
	local pickaxe = CreateObject('XPickAxe')
	torin_award = GetObjectGUID(pickaxe)
	GiveObjectToCreature(pickaxe, torin)
end

function TorinHandler(e, t, p, v)
	if (e == LE_CHAT) then
		if (gas_pump_quest == Q_KNOWN) then
			AddMessage("You haven\'t completed my previous request? Hmm... GET OUT OF HERE!")
		elseif (gas_pump_quest == Q_UNKNOWN) then
			AddMessage("Hello, brave hero. As you know, we dwarves mine our treasures deep from the ground. Some time ago, one of our mine was filled by a mysterious gas, which slowly kills all living things. It is oozing from the rocks. We have gas pump there, but there is no one who can switch this pump on. Please solve this problem.")
			gas_pump_quest = Q_KNOWN
		elseif (gas_pump_quest == Q_COMPLETE) then
			AddMessage("Thank you for your great help.")
			gas_pump_quest = Q_CLOSED
			if (GiveAward(t, torin_award, p)) then
				AddMessage('Take this tool as a reward.')
			end
		else
			AddMessage("Thank you for your help.")
		end
	elseif (e == LE_SAVE) then
		StoreInt(gas_pump_quest)
		StoreInt(torin_award)
	elseif (e == LE_LOAD) then
		gas_pump_quest = RestoreInt()
		torin_award = RestoreInt()
	end
	return 1
end


-----------------------------------------------------------
-------------------- MUSHROOM CAVE ------------------------
function MakeMushroomCave()

end

-----------------------------------------------------------
-------------------- SMALL CAVE ---------------------------
function MakeSmallCave()
	CreateLocation(L_SMALL_CAVE1, "SmCv:1", "Small Cave Level 1", CAVE)
		Way(UP, L_MAIN)
		Way(DOWN, L_SMALL_CAVE2)
		Settle(CR_RAT + CR_FELINE + CR_INSECT, CRL_VERY_LOW)

	CreateLocation(L_SMALL_CAVE2, "SmCv:2", "Small Cave Level 2", CAVE)
		Way(UP, L_SMALL_CAVE1)
		Creature(CN_ROTMOTH)
end


-----------------------------------------------------------
-------------------- RAT'S CELLAR ---------------------------
function MakeRatCellar()
	CreateLocation(L_RATCELLAR, "RC:1", "Rat's cellar", DUNGEON)
		SetPattern(80, 20,
		"################################################################################" ..
		"#A,,,,,,,,,,,######################################################A,,,,,,,,,,,#" ..
		"#,,,,,,,,,,,,+,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,+,,,,,,,,,,,,#" ..
		"#,,C,,,,,,,,,###########################,##########################,,,,,,,,,D,,#" ..
		"#,,,,,,,,,,,,###########################,###,,,,###################,,,,,,,,,,,,#" ..
		"#,,,,,,,,,,,,###########################,###,,,,######,############,,,,,,,,,,,,#" ..
		"######################,,,,,,,,##########,###,,,,######,#########################" ..
		"########B,,,##########,######,##########,####+########+#########################" ..
		"########~,,,+,,,,,,,##,#,,,,#,##########,####,,,,,,,,,,,,,,,,,,,,,+,,###########" ..
		"########,,,,#######,##,#,##,#,##########,####,#####################,,###########" ..
		"########~,,,#######,##,#,##,#,,,,,,,,,,,<,,,,,,,,,,,,,,,,,,,,,,,,##,,###########" ..
		"###################,##,#,##,############,#######################,###############" ..
		"###################,##,,,##,############,#######################,###############" ..
		"###################,#######,,,,,,,,,,###,#########,,############,###############" ..
		"#A,,,,,,,,,,,######,################,###,#########,,+,,,,,,,,,,,,###A,,,,,,,,,,#" ..
		"#,,,,,,,,,,,,######,,,,,,,,,,,,,,,,,,###,#########,,################,,,,,,,,,,,#" ..
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
