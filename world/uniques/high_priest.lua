
Monster.new("highpriest")
	:View("Aphilius, the high priest of Avanor", 'p', xColor.xWHITE, PersonType.NAMED_HE, CreatureTemplate.UNIQUE, CreatureClass.HUMANOID)
	:Basic("1d10+100", "0d0+800", "0d0+800", CreatureSize.NORMAL, "1d200+1200")
	:Body("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, CreatureTemplate.SUPPRESS_INVIS + CreatureTemplate.SEE_INVIS)
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM + XStandardAI.COWARD + XStandardAI.PEACEFUL)
	:Stats("St 1d8+10 Dx 1d8+15 To 1d8+10 Le 1d5+25 Wi 1d5+25 Ma 1d5+10 Pe 5d6 Ch 6d5")
	:Resist("see_invisible:0d0+100")
	:Combat("1d3", "1d2")
	:Main("1d4", "1d2", "1d5+30", "1d5+10")
	:Description("This compassionate soul gives his time and devotion to maintaining the temple.  He is dressed in the vestments of his position and bears the mitre of the priesthood...")
	:LearnSkill(SKT_HEALING, SKILL_MAX_LEVEL)
	:LearnSkill(SKT_RELIGION, SKILL_MAX_LEVEL)
	:LearnSpell(SPELL_HEAL)
	:Equip(ItemKind.BODY, ItemType.ROBE, 100)
	:Unique()
	:Register()


function CreateHighPriest(x, y)
	local hp = Guardian("highpriest", "roderick_guardian", x, y, 3, 4)
	SetEventHandler(hp, 'HighPriestHandler')
	GiveObjectToCreature(CreateObject(PotionName.HEALING), hp)
	GiveObjectToCreature(CreateObject(PotionName.HEALING), hp)
	GiveObjectToCreature(CreateObject(PotionName.HEALING), hp)
	GiveObjectToCreature(CreateObject(PotionName.HEALING), hp)
	GiveObjectToCreature(CreateObject('XAvanorMitre'), hp)
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

		return 1
	end

	if (e == LuaEvent.DIE) then
		local killer = AsCreature(p)

		if (killer:isHero()) then
			AddMessage(XReligion.GetDeityName(XDeity.LIFE) .. " will not be pleased about this...")
		else
			AddMessage(killer.name .. " seems to be trying to anger " .. XReligion.GetDeityName(XDeity.LIFE) .. "...")
		end

		killer.religion.life_act = killer.religion.life_act - 50
		return 1
	end

	if (e == LuaEvent.GIVE_ITEM) then
		local giver = AsCreature(p)
		local item = AsItem(v)

		AddMessage("Thank you for your charitable donation!")
		giver.religion:SacrificeItem(giver, item, XDeity.LIFE)
		return 1
	end

	return 0
end
