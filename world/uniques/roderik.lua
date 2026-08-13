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
