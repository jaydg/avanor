
Monster.new("roderik")
	:View("Roderick, king of Avanor", 'p', xColor.xYELLOW, PersonType.NAMED_HE, CreatureTemplate.UNIQUE, CreatureClass.HUMAN)
	:Basic("1d30+150", "0d0+700", "0d0+700", CS_NORMAL, "1d200+1200")
	:Body("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, CreatureTemplate.SUPPRESS_INVIS + CreatureTemplate.SEE_INVIS)
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM + XStandardAI.COWARD + XStandardAI.PEACEFUL)
	:Stats("St 1d8+55 Dx 1d8+60 To 1d8+40 Le 1d5+35 Wi 1d5+20 Ma 1d5+20 Pe 8d6 Ch 12d5")
	:Resist("see_invisible:0d0+100")
	:Combat("1d6", "1d5")
	:Main("1d8", "1d3", "1d5+70", "1d5+30")
	:Description("The mystical crown of Avanor rests upon the head of this noble looking man.  Curls of red hair stick out from under it but don't detract from his noble bearing.  The sceptre of his rule lies in his hand looking like an ornament but it has been said it is a formidable weapon wrought with great magic in days of yore.  Rodrick's face is happy but you can see great concern in his eyes.  The responsibilty for this nation must truly weigh on him.")
	:LearnSkill(SKT_HEALING, SKILL_MAX_LEVEL)
	:LearnSkill(SKT_FINDWEAKNESS, SKILL_MAX_LEVEL)
	:Unique()
	:Register()


function CreateRoderik(x, y)
	local roderik = Guardian("roderik", "roderick_guardian", x, y, 1, 1, XStandardAI.NO_SWAP)
	SetEventHandler(roderik, 'RoderikHandler')
	GiveObjectToCreature(CreateObject(PotionName.HEALING), roderik)
	GiveObjectToCreature(CreateObject(PotionName.HEALING), roderik)
	GiveObjectToCreature(CreateObject(PotionName.HEALING), roderik)
	GiveObjectToCreature(CreateObject(PotionName.HEALING), roderik)
	GiveObjectToCreature(CreateObject('XAvanorCrown'), roderik)
	GiveObjectToCreature(CreateObject('XAvanorScepter'), roderik)
end

function RoderikHandler(e, t, p, v)
	local roderik = AsCreature(t)

	if (e == LuaEvent.CHAT) then
		local chatter = AsCreature(p)

		if (roderik.xai:isEnemy(chatter)) then
			AddMessage("No mercy!")
		elseif (IsWearingAvanorDefender(p)) then
			AddMessage("I recognize that sword in your hand. You have looted the tomb of my ancestors! Guards! Seize the traitor!")
			roderik.xai:AddPersonalEnemy(chatter)
			roderik.xai:SetGroupEnemy(chatter)
		else
			AddMessage("Hello, brave hero.")

			if (QuestState:GetFlag('roderick_quest2') == 0) then
				AddMessage("I have heard that my family crypt has been occupied by a group of undead. Clear the crypt and I will reward you. It lies to the south-west of the city.")
				QuestState:SetFlag('roderick_quest2', 1)
			elseif (QuestState:GetFlag('roderick_quest2') == 1) then
				if (GetCreatureCount(XLocation.UNDEADS_TOMB1, CreatureClass.UNDEAD) == 0) then
					AddMessage("Thank you for destroying the evil in our crypt. Please accept these coins and my gratitude for a job well done.")
					QuestState:SetFlag('roderick_quest2', 2)
					chatter:MoneyOp(1000)
				else
					AddMessage("You still have not cleansed my ancestor's crypt.")
				end
			elseif (QuestState:GetFlag('roderick_quest') == 0) then
				AddMessage("Some years ago one of my trusted servants stole a powerful artifact, the 'Eye of Raa' from me. He tried to hide it from me in one of the caves far south from here, but people say that he was killed while hiding it.  Could you return this artifact to me?")
				QuestState:SetFlag('roderick_quest', 1)
			elseif (QuestState:GetFlag('roderick_quest') == 1) then
				AddMessage("Please, return the 'Eye of Raa' to me.")
			end
		end

		return 1
	end

	if (e == LuaEvent.DIE) then
		QuestState:SetFlag('roderick_killed', 1)
		return 1
	end

	if (e == LuaEvent.GIVE_ITEM) then
		local item = AsItem(v)

		if (item.it == ItemType.EYEOFRAA) then
			AddMessage("Thank you for your great help. The citizens of Avanor never forget your exploits!")
			QuestState:SetFlag('roderick_quest', 2)
			roderik:ContainItem(item)
			return 1
		else
			AddMessage("I don't need this")
		end

		return 0
	end

	return 0
end
