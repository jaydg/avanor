-------------------------------------------------------------------------
-------------------- AHK_ULAN CASTLE AND WAY  ---------------------------

function MakeWizardDungeon()

	CreateLocation("WIZARD_DUNGEON1", "AD:1", "Ahk-Ulan's dungeons level 1", XLocation.DUNGEON)
		Way(XStairWay.UP, "MAIN")
		Way(XStairWay.DOWN, "WIZARD_DUNGEON2")
		Settle(CreatureClass.UNDEAD + CreatureClass.BLOB + CreatureClass.INSECT + CreatureClass.REPTILE + CreatureClass.RAT + CreatureClass.FELINE + CreatureClass.CANINE, CreatureTemplate.VERY_LOW)

	CreateLocation("WIZARD_DUNGEON2", "AD:2", "Ahk-Ulan's dungeons level 2", XLocation.DUNGEON)
		Way(XStairWay.UP, "WIZARD_DUNGEON1")
		Way(XStairWay.DOWN, "WIZARD_DUNGEON3")
		Settle(CreatureClass.UNDEAD + CreatureClass.BLOB + CreatureClass.INSECT + CreatureClass.REPTILE + CreatureClass.RAT + CreatureClass.FELINE + CreatureClass.CANINE, CreatureTemplate.VERY_LOW)

	CreateLocation("WIZARD_DUNGEON3", "AD:3", "Ahk-Ulan's dungeons level 3", XLocation.DUNGEON)
		Way(XStairWay.UP, "WIZARD_DUNGEON2")
		Way(XStairWay.DOWN, "WIZARD_DUNGEON4")
		Settle(CreatureClass.UNDEAD + CreatureClass.BLOB + CreatureClass.INSECT + CreatureClass.REPTILE + CreatureClass.RAT + CreatureClass.FELINE + CreatureClass.CANINE, CreatureTemplate.VERY_LOW)

	CreateLocation("WIZARD_DUNGEON4", "AD:4", "Ahk-Ulan's dungeons level 4", XLocation.DUNGEON)
		Way(XStairWay.UP, "WIZARD_DUNGEON3")
		Way(XStairWay.DOWN, "WIZARD_DUNGEON5")
		Settle(CreatureClass.UNDEAD + CreatureClass.BLOB + CreatureClass.INSECT + CreatureClass.REPTILE + CreatureClass.RAT + CreatureClass.FELINE + CreatureClass.CANINE, CreatureTemplate.VERY_LOW)

	CreateLocation("WIZARD_DUNGEON5", "AD:5", "Ahk-Ulan's dungeons level 5", XLocation.DUNGEON)
		Way(XStairWay.UP, "WIZARD_DUNGEON4")
		Way(XStairWay.DOWN, "AHKULAN_CASTLE")
		Settle(CreatureClass.UNDEAD + CreatureClass.BLOB + CreatureClass.INSECT + CreatureClass.REPTILE + CreatureClass.RAT + CreatureClass.FELINE + CreatureClass.CANINE, CreatureTemplate.VERY_LOW)

	CreateLocation("AHKULAN_CASTLE", "AC", "Ahk-Ulan's castle", XLocation.CAVE)
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
		AddTranslation("<", function(x, y) Way(XStairWay.UP, "WIZARD_DUNGEON5", x, y) end)
		AddTranslation("h", function(x, y) Guardian('death_knight', "ahkulan_guardian", x, y) end)
		AddTranslation("A", function(x, y) CreateAhkUlan(x, y) Furniture(x, y, xColor.xDARKGRAY, '~', 'the black throne from pure obsidian') end)
		AddTranslation("_", function(x, y) SetView(PlaceSpecial("XAltar", x, y), "_", xColor.xDARKGRAY) end)
		AddTranslation("r", function(x, y) Creature('huge_rat', x, y) end)
		DrawPattern(0, 0)

end
