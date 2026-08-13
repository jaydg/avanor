-------------------------------------------------------------------------
-------------------- AHK_ULAN CASTLE AND WAY  ---------------------------

function MakeWizardDungeon()

	CreateLocation(XLocation.WIZARD_DUNGEON1, "AD:1", "Ahk-Ulan's dungeons level 1", DUNGEON)
		Way(UP, XLocation.MAIN)
		Way(DOWN, XLocation.WIZARD_DUNGEON2)
		Settle(CreatureClass.UNDEAD + CreatureClass.BLOB + CreatureClass.INSECT + CreatureClass.REPTILE + CreatureClass.RAT + CreatureClass.FELINE + CreatureClass.CANINE, CreatureTemplate.VERY_LOW)

	CreateLocation(XLocation.WIZARD_DUNGEON2, "AD:2", "Ahk-Ulan's dungeons level 2", DUNGEON)
		Way(UP, XLocation.WIZARD_DUNGEON1)
		Way(DOWN, XLocation.WIZARD_DUNGEON3)
		Settle(CreatureClass.UNDEAD + CreatureClass.BLOB + CreatureClass.INSECT + CreatureClass.REPTILE + CreatureClass.RAT + CreatureClass.FELINE + CreatureClass.CANINE, CreatureTemplate.VERY_LOW)

	CreateLocation(XLocation.WIZARD_DUNGEON3, "AD:3", "Ahk-Ulan's dungeons level 3", DUNGEON)
		Way(UP, XLocation.WIZARD_DUNGEON2)
		Way(DOWN, XLocation.WIZARD_DUNGEON4)
		Settle(CreatureClass.UNDEAD + CreatureClass.BLOB + CreatureClass.INSECT + CreatureClass.REPTILE + CreatureClass.RAT + CreatureClass.FELINE + CreatureClass.CANINE, CreatureTemplate.VERY_LOW)

	CreateLocation(XLocation.WIZARD_DUNGEON4, "AD:4", "Ahk-Ulan's dungeons level 4", DUNGEON)
		Way(UP, XLocation.WIZARD_DUNGEON3)
		Way(DOWN, XLocation.WIZARD_DUNGEON5)
		Settle(CreatureClass.UNDEAD + CreatureClass.BLOB + CreatureClass.INSECT + CreatureClass.REPTILE + CreatureClass.RAT + CreatureClass.FELINE + CreatureClass.CANINE, CreatureTemplate.VERY_LOW)

	CreateLocation(XLocation.WIZARD_DUNGEON5, "AD:5", "Ahk-Ulan's dungeons level 5", DUNGEON)
		Way(UP, XLocation.WIZARD_DUNGEON4)
		Way(DOWN, XLocation.AHKULAN_CASTLE)
		Settle(CreatureClass.UNDEAD + CreatureClass.BLOB + CreatureClass.INSECT + CreatureClass.REPTILE + CreatureClass.RAT + CreatureClass.FELINE + CreatureClass.CANINE, CreatureTemplate.VERY_LOW)

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
		AddTranslation("h", function(x, y) Guardian('death_knight', "ahkulan_guardian", x, y) end)
		AddTranslation("A", function(x, y) CreateAhkUlan(x, y) Furniture(x, y, xColor.xDARKGRAY, '~', 'the black throne from pure obsidian') end)
		AddTranslation("_", function(x, y) Altar(x, y, XDeity.DEATH) end)
		AddTranslation("r", function(x, y) Creature('huge_rat', x, y) end)
		DrawPattern(0, 0)

end
