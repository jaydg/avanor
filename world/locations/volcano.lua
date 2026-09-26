----------------------------------------------------------------
-------------------- EXTINCT VULCANO ---------------------------

Monster.new("xshee_voo")
	:View("Xshee-Voo, the Cyclope", 'H', xColor.xLIGHTMAGENTA, PersonType.NAMED_HE, CreatureTemplate.UNIQUE, "giant")
	:Basic("0d0+111", "0d0+900", CreatureSize.LARGE, "1d400+3000")
	:Body("head neck body cloak hand hand boots", 50)
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM + XStandardAI.COWARD)
	:Stats("St 5d5+150 Dx 1d10+10 To 1d10+80 Le 1d5+5 Wi 1d5+5 Ma 1d5+5 Pe 1d6 Ch 1d5")
	:Combat("1d5", "2d5")
	:Main("0d0-10", "0d0+15", "5d5+150", "1d5+5")
	:Description("Xshee-Voo has lived in his mountain cave for as long as "
		.. "anyone can remember. He never shows himself outside, and the "
		.. "few that have been in his cave and returned speak of piles of "
		.. "bones and armour slowly decaying. They also speak of his "
		.. "enormous club which looks to have been carved from the rock of "
		.. "the mountain and is written over with Runes of great power.")
	:LearnSkill(XSkill.HEALING, XSkill.MAX_LEVEL)
	:LearnSkill(XSkill.FINDWEAKNESS, XSkill.MAX_LEVEL)
	:Unique()
	:Register()


-- What Xshee-Voo swings.

Item.new("black_club")
	:Weapon("club")
	:View("black club", '/', xColor.xDARKGRAY)
	:Basic(10000, 8000)
	:Armour(-10, 0)
	:Combat(-15, 2, 20, 0)
	:Resist{ fire = "0d0+50" }
	:Stats("")
	:Brand("fire")
	:Called("club of black obsidian")
	:Unique()
	:Register()

function MakeVulcano()
	CreateLocation("EXTINCT_VOLCANO", "Volcano", "Crater of an Extinct Volcano", XLocation.PATTERN, Drawn())
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
		AddTranslation("<", function(x, y) Way(XStairWay.UP, "MAIN", x, y) end)
		AddTranslation("=", XTileType.LAVA)
		DrawPattern(0, 0)

		local cyclops = Creature("xshee_voo")
		SetEventHandler(cyclops, 'XsheeVooHandler')
		GiveObjectToCreature(CreateObject('black_club'), cyclops)
end


-- What he says when spoken to. He is not a riddle to be solved and there
-- is nothing here to trade for yet: the point of talking to him is to
-- learn that the thing in the crater is frightened, that it does not want
-- the fight, and that whatever the club is, it is not treasure to him.
--
-- His grammar is his own. He has an intelligence of six and nobody to
-- practise on, so he drops his articles, muddles his tenses and falls
-- back on his own name when "I" deserts him. That is characterisation
-- and not an oversight - if these lines are ever tidied up, tidy them
-- into better broken speech rather than into good English.
local XSHEE_VOO_LINES = {
	"'Go up. Go up the stair. Nothing here for little ones.'",
	"'Not the stone. Stone is Xshee-Voo's. You not touch it.'",
	"'Others came for it. They are still here, under the ash.'",
	"'Cold out there. In here is warm. Xshee-Voo stays where warm is.'",
	"'Xshee-Voo not want to fight you. You go now, yes? You go.'",
}


-- Once he is losing he stops warning and starts pleading, and says the
-- thing the warnings only circle: the club is not loot, it is the reason
-- he is still alive. A player who is most of the way through killing him
-- is exactly the player who should hear it.
local XSHEE_VOO_HURT = {
	"'Stop. Stop, please. Xshee-Voo did nothing to you.'",
	"'Take the bones. Take all the bones. Not the stone.'",
	"'Without stone it burns. It burns, it burns. You do not know.'",
}


-- He begs at exactly the point the engine has him run. XStandardAI's
-- COWARD branch flees once GetMaxHP() / HP > 4, and because that is
-- integer division the real threshold is a fifth of his hit points and
-- not the quarter the comment beside it claims.
--
-- Written as a multiplication rather than the same division, because
-- Lua's / is floating point: max_hp / hp > 4 would have him begging at a
-- quarter and running at a fifth. hp * 5 <= max_hp is exactly the
-- engine's test for whole numbers, and it cannot divide by zero.
--
-- The two have to agree. A fleeing creature is one Tiamat counts as
-- helpless, so the moment he starts pleading is the moment killing him
-- stops being a fight and starts being a cruelty - Tiamat takes it
-- badly, Marduk pays double. If the engine's threshold moves, move this
-- with it.
local function IsDesperate(him)
	return him.hp * 5 <= him.max_hp
end


function XsheeVooHandler(e, t, p, v)
	if (e == LuaEvent.CHAT) then
		local him = AsCreature(t)

		if (IsDesperate(him)) then
			AddMessage(XSHEE_VOO_HURT[Rand(#XSHEE_VOO_HURT) + 1])
		else
			AddMessage(XSHEE_VOO_LINES[Rand(#XSHEE_VOO_LINES) + 1])
		end

		return true
	end

	return false
end
