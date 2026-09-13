
Monster.new("highpriest")
	:View("Aphilius, the high priest of Avanor", 'p', xColor.xWHITE, PersonType.NAMED_HE, CreatureTemplate.UNIQUE, "humanoid")
	:Basic("1d10+100", "0d0+800", "0d0+800", CreatureSize.NORMAL, "1d200+1200")
	:Body("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, CreatureTemplate.SUPPRESS_INVIS + CreatureTemplate.SEE_INVIS)
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM + XStandardAI.COWARD + XStandardAI.PEACEFUL)
	:Stats("St 1d8+10 Dx 1d8+15 To 1d8+10 Le 1d5+25 Wi 1d5+25 Ma 1d5+10 Pe 5d6 Ch 6d5")
	:Resist("see_invisible:0d0+100")
	:Combat("1d3", "1d2")
	:Main("1d4", "1d2", "1d5+30", "1d5+10")
	:Description("This compassionate soul gives his time and devotion to maintaining the temple.  He is dressed in the vestments of his position and bears the mitre of the priesthood...")
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
	:Resist("stun:1d1+99 confuse:1d1+99 fire:1d1+99 cold:1d1+99 acid:1d1+99 see_invisible:0d0+20")
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

function HighPriestHandler(e, t, p, v)
	local hp = AsCreature(t)

	if (e == LuaEvent.CHAT) then
		local chatter = AsCreature(p)

		if (hp.xai:isEnemy(chatter)) then
			AddMessage("Defiler, you must be punished!")
		else
			AddMessage("Blessings on you.")
		end

		return true
	end

	if (e == LuaEvent.DIE) then
		local killer = AsCreature(p)

		if (killer:isHero()) then
			AddMessage(GetDeityName("life") .. " will not be pleased about this...")
		else
			AddMessage(killer.name .. " seems to be trying to anger " .. GetDeityName("life") .. "...")
		end

		ChangeFavour(p, "life", -50)
		return true
	end

	if (e == LuaEvent.GIVE_ITEM) then
		local giver = AsCreature(p)
		local item = AsItem(v)

		AddMessage("Thank you for your charitable donation!")
		Sacrifice(p, v, "life")
		return true
	end

	return false
end
