----------------------------------------------------------------
-------------------- EXTINCT VULCANO ---------------------------

Monster.new("xshee_voo")
	:View("Xshee-Voo, the Cyclope", 'H', xColor.xLIGHTMAGENTA, PersonType.NAMED_HE, CreatureTemplate.UNIQUE, "giant")
	:Basic(111, 900, CreatureSize.LARGE, "1d400+3000")
	:Body("head neck body cloak hand hand ring ring boots", 50)
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM + XStandardAI.COWARD)
	:Stats("St 5d5+150 Dx 1d10+10 To 1d10+80 Le 1d5+5 Wi 1d5+5 Ma 1d5+5 Pe 1d6 Ch 1d5")
	:Combat("1d5", "2d5")
	:Main(-10, 15, "5d5+150", "1d5+5")
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
	:Resist{ fire = 50 }
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

		local club = CreateObject('black_club')
		XSHEE_VOO_WARD = GetObjectGUID(club)
		GiveObjectToCreature(club, cyclops)
end


-- The club he is holding, so that he can hand it over rather than only be
-- robbed of it. Saved with him: a guid is the only handle GiveAward()
-- takes, and it has to survive a reload like any other.
XSHEE_VOO_WARD = 0


-- What Todin does to the slab at his own forge, once he has it honestly.
-- The runes were cut to let a hand hold the thing where no hand can go,
-- and waking them is what makes it a weapon a person can actually swing:
-- eight thousand of dead rock becomes something a strong arm manages, and
-- the miserable -15 it forged with goes away.
--
-- The dice are left alone. It was never a weapon and 2d20 is already more
-- than anything else in the game; making it wieldable is the whole of the
-- reward.
function WakeTheWard(item)
	SetItemWeight(item, 2500)
	AddItemToHit(item, 15)
	SetItemName(item, "club of the deep fire")
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


-- What he says once he has been given something to stand behind. He does
-- not understand that he has made a bargain; he understands that the
-- little one gave him a thing and that the burning stopped mattering.
local XSHEE_VOO_TRADED = {
	"'Warm. Still warm. Xshee-Voo keeps this one now.'",
	"'You gave. Nobody gives. Go, little one, go safe.'",
	"'The stone is yours. Xshee-Voo does not need two.'",
}


function XsheeVooHandler(e, t, p, v)
	if (e == LuaEvent.CHAT) then
		local him = AsCreature(t)

		-- Todin cannot be told what is up here by somebody who has not
		-- been up here. Set on any word with him, so that finding him is
		-- what unlocks the honest ending rather than finding the crater.
		TODIN_WARD_SEEN = true

		if (XSHEE_VOO_WARD == 0) then
			AddMessage(XSHEE_VOO_TRADED[Rand(#XSHEE_VOO_TRADED) + 1])
		elseif (IsDesperate(him)) then
			AddMessage(XSHEE_VOO_HURT[Rand(#XSHEE_VOO_HURT) + 1])
		else
			AddMessage(XSHEE_VOO_LINES[Rand(#XSHEE_VOO_LINES) + 1])
		end

		return true
	elseif (e == LuaEvent.GIVE_ITEM) then
		TODIN_WARD_SEEN = true

		-- He is not stupid, only frightened and slow. Offer him something
		-- that answers the fire and he will let the slab go, because the
		-- slab was never a weapon to him and never treasure: it was the
		-- only thing standing between him and the crater.
		--
		-- Judged by what the thing does and not by what it is called: an
		-- unidentified ring of fire resistance is a ruby ring, and he can
		-- tell the difference even if the hero cannot yet.
		if (XSHEE_VOO_WARD ~= 0 and GetItemResistance(v, "fire") > 0) then
			AddMessage("The cyclops turns it over in a hand the size of "
				.. "your chest, and something in his face gives way.")
			AddMessage("'It is warm. It is warm and it is small.'")

			-- Said rather than left to be noticed, because what he does
			-- with it is the point: a ring or an amulet he can put on and
			-- keep on, and anything else he can only hold. He has the
			-- slots for both now, and his own AI does the wearing.
			local kind = GetItemParam(v)

			if (IsKind(kind, ItemKind.RING)) then
				AddMessage("He works it onto a finger, as far as the first "
					.. "knuckle, which is as far as it goes.")
			elseif (IsKind(kind, ItemKind.NECK)) then
				AddMessage("He hangs it round his neck, where it looks like "
					.. "a bead on a rope.")
			else
				AddMessage("He closes his fist round it and will not open "
					.. "it again.")
			end

			if (GiveAward(t, XSHEE_VOO_WARD, p)) then
				AddMessage("He puts the black slab down in front of you, "
					.. "very carefully, and steps back from it.")
				XSHEE_VOO_WARD = 0
				TODIN_WARD_HONOURED = true
			end

			-- Answered true: he keeps what he was given, which is the
			-- whole of the bargain.
			return true
		end

		if (XSHEE_VOO_WARD == 0) then
			AddMessage("'Xshee-Voo has a warm thing. Does not need more.'")
		else
			AddMessage("'What is that? No. No use. Cold.'")
		end

		return false
	elseif (e == LuaEvent.SAVE) then
		StoreInt(XSHEE_VOO_WARD)
	elseif (e == LuaEvent.LOAD) then
		XSHEE_VOO_WARD = RestoreInt()
	end

	return false
end
