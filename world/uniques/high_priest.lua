
Monster.new("highpriest")
	:View("Aphilius, the high priest of Avanor", 'p', xColor.xWHITE, PersonType.NAMED_HE, CreatureTemplate.UNIQUE, "humanoid")
	:Basic(125, 800, CreatureSize.NORMAL, "1d200+1200")
	:Body("head neck body cloak hand hand ring ring gloves boots missile_weapon missile", 100)
	:Never("invisible")
	:Always("see_invisible")
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM + XStandardAI.COWARD + XStandardAI.PEACEFUL)
	:Stats("St 1d8+10 Dx 1d8+15 To 1d8+10 Le 1d5+25 Wi 1d5+25 Ma 1d5+10 Pe 5d6 Ch 6d5")
	:Combat("1d3", "1d2")
	:Main("1d4", "1d2", "1d5+30", "1d5+10")
	:Description("This compassionate soul gives his time and devotion to "
		.. "maintaining the temple. He is dressed in the vestments of his "
		.. "position and bears the mitre of the priesthood...")
	:LearnSkill(XSkill.HEALING, XSkill.MAX_LEVEL)
	:LearnSkill(XSkill.RELIGION, XSkill.MAX_LEVEL)
	:LearnSpell("heal")
	:Equip(ItemKind.BODY, "robe", 100)
	:Unique()
	:Register()


-- The high priest's mitre.

Item.new("avanor_mitre")
	:Cap("cap")
	:View("holy mitre", '[', xColor.xWHITE)
	:Basic(8000, 100)
	:Armour(3, 2)
	:Combat(0, 0, 0, 0)
	:Resist{ stun = "1d1+99", confuse = "1d1+99", fire = "1d1+99", cold = "1d1+99", acid = "1d1+99", see_invisible = true }
	:Stats("Wi:0d0+10")
	:Called("holy mitre of Avanor")
	:Unique()
	:Register()


function CreateHighPriest(x, y)
	local hp = Guardian("highpriest", "roderick_guardian", x, y, 3, 4)
	SetEventHandler(hp, 'HighPriestHandler')
	GiveObjectToCreature(CreatePotion("healing"), hp)
	GiveObjectToCreature(CreatePotion("healing"), hp)
	GiveObjectToCreature(CreatePotion("healing"), hp)
	GiveObjectToCreature(CreatePotion("healing"), hp)
	GiveObjectToCreature(CreateObject('avanor_mitre'), hp)
end

-- What the temple asks for teaching the skill it is built around. The
-- same as Yohjishiro's price for reading: those two are the game's
-- teachers of the things a character cannot really do without, and there
-- is no reason for one of them to be dearer than the other.
local HEALING_TUITION = 500

-- Healing is the one skill a character can walk into the dungeons without
-- and not walk back out of, and the ranger is the profession that starts
-- without it. A priest of life is the obvious person to learn it from, so
-- he offers it to anyone who lacks it, and never mentions it to anyone
-- who has it.
local function TeachHealing(t, p)
	if (GetSkill(p, XSkill.HEALING) > 0) then
		return
	end

	local answer = AskQuestion(
		string.format("'You walk into the dark not knowing how to bind a "
			.. "wound. The temple will teach you, for a donation of %d "
			.. "gold. Shall I?'", HEALING_TUITION),
		"esc y n", "yes", "no")

	if (answer ~= 'y') then
		AddMessage("'As you wish. The offer keeps.'")

		return
	end

	-- MoneyOperation() answers with what is left over and takes nothing
	-- at all when there is not enough, so the asking is the paying and
	-- a refusal costs the player nothing.
	if (MoneyOperation(p, -HEALING_TUITION) < 0) then
		AddMessage("'You do not carry that much. Come back when you do - "
			.. "the teaching will keep.'")

		return
	end

	MoneyOperation(t, HEALING_TUITION)
	LearnSkill(p, XSkill.HEALING, 1)
	AddMessage("Aphilius shows you how to close a wound and slow a bleed.")
	AddMessage("'Go carefully. A hurt tended early is a life kept.'")
end

function HighPriestHandler(e, t, p, v)
	local hp = AsCreature(t)

	if (e == LuaEvent.CHAT) then
		local chatter = AsCreature(p)

		if (hp.xai:isEnemy(chatter)) then
			AddMessage("'Defiler, you must be punished!'")

			return true
		end

		AddMessage("'Blessings on you.'")
		TeachHealing(t, p)

		return true
	end

	if (e == LuaEvent.DIE) then
		local killer = AsCreature(p)

		if (killer:isHero()) then
			AddMessage(GetDeityName("life")
				.. " will not be pleased about this...")
		else
			AddMessage(killer.name .. " seems to be trying to anger "
				.. GetDeityName("life") .. "...")
		end

		ChangeFavour(p, "life", -50)
		return true
	end

	if (e == LuaEvent.GIVE_ITEM) then
		local giver = AsCreature(p)
		local item = AsItem(v)

		AddMessage("'Thank you for your charitable donation!'")
		Sacrifice(p, v, "life")
		return true
	end

	return false
end
