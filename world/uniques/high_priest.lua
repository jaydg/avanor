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
