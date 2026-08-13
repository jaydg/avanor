----------------------------------------------------------------
-------------------- EXTINCT VULCANO ---------------------------

Monster.new("xshee_voo")
	:View("Xshee-Voo, the Cyclope", 'H', xColor.xLIGHTMAGENTA, PersonType.NAMED_HE, CreatureTemplate.UNIQUE, CreatureClass.GIANT)
	:Basic("1d30+120", "0d0+900", "0d0+900", CS_LARGE, "1d400+3000")
	:Body("head neck body cloak hand hand boots", 50, 0)
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM + XStandardAI.COWARD)
	:Stats("St 5d5+150 Dx 1d10+10 To 1d10+80 Le 1d5+5 Wi 1d5+5 Ma 1d5+5 Pe 1d6 Ch 1d5")
	:Resist("")
	:Combat("1d5", "2d5")
	:Main("0d0-10", "0d0+15", "1d5+70", "1d5+5")
	:Description("Xshee-Voo has lived in his mountain cave for as long as anyone can remember.  He never shows himself outside, and the few that have been in his cave and returned speak of piles of bones and armor slowly decaying.  They also speak of his enormous club which looks to have been carved from the rock of the mountain and is written over with Runes of great power.")
	:LearnSkill(SKT_HEALING, SKILL_MAX_LEVEL)
	:LearnSkill(SKT_FINDWEAKNESS, SKILL_MAX_LEVEL)
	:Unique()
	:Register()

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
