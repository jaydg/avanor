-- What the shops of this world are built of.
local SHOP = { wall = XTileType.STONE_WALL, floor = XTileType.STONE_FLOOR }

-- What the caves and dungeons of this world are cut out of.
local CAVE = { wall = XTileType.MAGMA, floor = XTileType.CAVE_FLOOR }

-----------------------------------------------------------
-------------------- DWARVEN CITY -------------------------

function MakeDwarvenCity()
	CreateLocation("DWARFCITYCAVE1", "PDC:1", "Path to the Dwarven City Level 1", XLocation.DUNGEON, CAVE)
		Way(XStairWay.UP, "MAIN")
		Way(XStairWay.DOWN, "DWARFCITYCAVE2")
		Settle(CreatureClass.UNDEAD + CreatureClass.BLOB + CreatureClass.INSECT + CreatureClass.REPTILE + CreatureClass.RAT + CreatureClass.FELINE + CreatureClass.CANINE, CreatureTemplate.VERY_LOW)

	CreateLocation("DWARFCITYCAVE2", "PDC:2", "Path to the Dwarven City Level 2", XLocation.DUNGEON, { wall = CAVE.wall, floor = CAVE.floor, room_chance = 40 })
		Way(XStairWay.UP, "DWARFCITYCAVE1")
		Way(XStairWay.DOWN, "DWARFCITYCAVE3")
		Settle(CreatureClass.UNDEAD + CreatureClass.BLOB + CreatureClass.INSECT + CreatureClass.REPTILE + CreatureClass.RAT + CreatureClass.FELINE + CreatureClass.CANINE, CreatureTemplate.LOW)

	CreateLocation("DWARFCITYCAVE3", "PDC:3", "Path to the Dwarven City Level 3", XLocation.DUNGEON, { wall = CAVE.wall, floor = CAVE.floor, room_chance = 40 })
		Way(XStairWay.UP, "DWARFCITYCAVE2")
		Way(XStairWay.DOWN, "DWARFCITYCAVE4")
		Settle(CreatureClass.UNDEAD + CreatureClass.BLOB + CreatureClass.INSECT + CreatureClass.REPTILE + CreatureClass.RAT + CreatureClass.FELINE + CreatureClass.CANINE, CreatureTemplate.LOW)

	CreateLocation("DWARFCITYCAVE4", "PDC:4", "Path to the Dwarven City Level 4", XLocation.DUNGEON, { wall = CAVE.wall, floor = CAVE.floor, room_chance = 40 })
		Way(XStairWay.UP, "DWARFCITYCAVE3")
		Way(XStairWay.DOWN, "DWARFCITYCAVE5")
		Settle(CreatureClass.UNDEAD + CreatureClass.BLOB + CreatureClass.INSECT + CreatureClass.REPTILE + CreatureClass.RAT + CreatureClass.FELINE + CreatureClass.CANINE, CreatureTemplate.LOW)

	CreateLocation("DWARFCITYCAVE5", "PDC:5", "Path to the Dwarven City Level 5", XLocation.DUNGEON, { wall = CAVE.wall, floor = CAVE.floor, room_chance = 40 })
		Way(XStairWay.UP, "DWARFCITYCAVE4")
		Way(XStairWay.DOWN, "DWARFCITYCAVE6")
		Settle(CreatureClass.UNDEAD + CreatureClass.BLOB + CreatureClass.INSECT + CreatureClass.REPTILE + CreatureClass.RAT + CreatureClass.FELINE + CreatureClass.CANINE, CreatureTemplate.LOW)

	CreateLocation("DWARFCITYCAVE6", "PDC:6", "Path to the Dwarven City Level 6", XLocation.DUNGEON, { wall = CAVE.wall, floor = CAVE.floor, room_chance = 40 })
		Way(XStairWay.UP, "DWARFCITYCAVE5")
		Way(XStairWay.DOWN, "DWARFCITY")
		Settle(CreatureClass.UNDEAD + CreatureClass.BLOB + CreatureClass.INSECT + CreatureClass.REPTILE + CreatureClass.RAT + CreatureClass.FELINE + CreatureClass.CANINE + CreatureClass.GOBLIN + CreatureClass.KOBOLD, CreatureTemplate.LOW)
		DropItem(CreateObject("XCookingSet"))


	CreateLocation("DWARFCITY", "DvCty", "Dwarven City", XLocation.CAVE, CAVE)
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
		AddTranslation("<", function(x, y) Way(XStairWay.UP, "DWARFCITYCAVE6", x, y) end)
		AddTranslation("A", function(x, y) Guardian('dwarf_guard', "dwarven_guardian", x, y) end)
		AddTranslation("S", function(x, y) BuildShop(x, y, 9, 3, ItemKind.ARMOUR + ItemKind.WEAPON + ItemKind.POTION + ItemKind.BOOK + ItemKind.SCROLL + ItemKind.NECK + ItemKind.MISSILE + ItemKind.MISSILEW, 'Toberin, the dwarwen shopkeeper', SHOP) end)
		AddTranslation("B", function(x, y) Furniture(x, y, xColor.xBROWN, '~', 'a dinner table') end)
		AddTranslation("C", function(x, y) Furniture(x, y, xColor.xBROWN, '~', 'a round table') end)
		AddTranslation("D", function(x, y) Furniture(x, y, xColor.xLIGHTRED, '~', 'a royal bed') end)
		AddTranslation("X", function(x, y) CreateTorin(x, y) Furniture(x, y, xColor.xYELLOW, '~', 'the throne of the Dwarven Kingdom') end)
		AddTranslation("_", function(x, y) SetView(PlaceSpecial("XAltar", x, y), "_", xColor.xWHITE) end)
		AddTranslation("P", function(x, y) for i = 1, 6 do Guardian('dwarf', "dwarven_guardian", x, y, 32, 16) end end)
		AddTranslation("T", function(x, y) CreateTodin(x, y) end)
		AddTranslation("E", function(x, y) EventPlace(x - 1, y, 5, 2, 'DvCityEvent1') end)
		AddTranslation("F", function(x, y) Way(XStairWay.DOWN, "DWARFTREASURE", x, y) end)
		AddTranslation("G", function(x, y) Way(XStairWay.DOWN, "GASMINE1", x, y) end)
		DrawPattern(0, 0)

	CreateLocation("DWARFTREASURE", "DvTr", "Dwarven Treasure", XLocation.CAVE, CAVE)
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
		AddTranslation("<", function(x, y) Way(XStairWay.UP, "DWARFCITY", x, y) end)
		AddTranslation("$", function(x, y) Treasure(x, y, 500) end)
		AddTranslation("~", function(x, y) Chest(x, y) end)
		AddTranslation("A", function(x, y) DropItem(CreateObject('XAncientMachinePart'), x, y) end)
		DrawPattern(0, 0)


	CreateLocation("GASMINE1", "GM:1", "Gassed Mine level 1", XLocation.DUNGEON, CAVE)
		Way(XStairWay.UP, "DWARFCITY")
		Way(XStairWay.DOWN, "GASMINE2")
--		Settle(CreatureClass.BLOB, CreatureTemplate.VERY_LOW)
		EventPlace('GasMineEvent')

	CreateLocation("GASMINE2", "GM:2", "Gassed Mine level 2", XLocation.DUNGEON, { wall = CAVE.wall, floor = CAVE.floor, room_chance = 40 })
		Way(XStairWay.UP, "GASMINE1")
		Way(XStairWay.DOWN, "GASMINE3")
--		Settle(CreatureClass.BLOB, CreatureTemplate.VERY_LOW)
		EventPlace('GasMineEvent')

	CreateLocation("GASMINE3", "GM:3", "Gassed Mine level 3", XLocation.DUNGEON, { wall = CAVE.wall, floor = CAVE.floor, room_chance = 40 })
		Way(XStairWay.UP, "GASMINE2")
--		Settle(CreatureClass.BLOB, CreatureTemplate.VERY_LOW)
		EventPlace('GasMineEvent')
		OuterObject(xColor.xBLUE, '#', 'a gas pump', 'GasPumpEvent')
end


DvCityEventMoveCount = 0

function DvCityEvent1(e, p)
	if (e == LuaEvent.MOVE and isHero(p)) then
		local c = FindCreature("DWARFCITY", "dwarven_guardian", 75, 1, 4, 2)
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
	elseif (e == LuaEvent.MOVE_IN and isHero(p)) then
		local c = FindCreature("DWARFCITY", "dwarven_guardian", 75, 1, 4, 2)
		if (not isEnemy(c, p)) then
			AddMessage("'Halt! You aren't allowed to enter.'")
		end
		DvCityEventMoveCount = 1
	elseif (e == LuaEvent.MOVE_OUT and isHero(p)) then
		DvCityEventMoveCount = 0
	elseif (e == LuaEvent.SAVE) then
		StoreInt(DvCityEventMoveCount)
	elseif (e == LuaEvent.LOAD) then
		DvCityEventMoveCount = RestoreInt()
	end

end

function GasMineEvent(e, p)
	if (e == LuaEvent.MOVE) then
		if (QuestStatus(QUEST_TORIN) < XQuest.COMPLETE and Rand(3) == 0) then
			InflictDamage(p, Rand(5) + 2, XResistance.POISON, "mysterious gas")
		end
	end
end


function GasPumpEvent(e, p)
	if (e == LuaEvent.OUTER_USE) then
		if (QuestStatus(QUEST_TORIN) < XQuest.COMPLETE) then
			AddMessage('You hear hollow rumble as the pump starts.')
			QuestModify(QUEST_TORIN, XQuest.COMPLETE)
		else
			AddMessage('You can\'t seem to stop the pump.')
		end
	end
	return 1
end
