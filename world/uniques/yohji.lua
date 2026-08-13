function CreateYohji(x, y)
	local yohji = Guardian("yohjishiro", "", x, y, 5, 5)
	SetEventHandler(yohji, 'YohjiHandler')
end


function YohjiHandler(e, t, p, v)
	if (e == LuaEvent.CHAT) then
		local result
		if (GetSkill(p, SKT_LITERACY) == 0) then
			result = AskQuestion("What do you wish to speak about?", "esc q l", "quest", "learn")
		else
			result = 'q'
		end
		if (result == 'q') then
			if (QuestStatus(QUEST_YOHJI_BAT) ~= XQuest.KNOWN and QuestStatus(QUEST_YOHJI_RAT) ~= XQuest.KNOWN) then
				if (Rand(2) == 1) then
					AddMessage("'I can identify all items in your inventory, if you bring me a bat wing.'")
					QuestModify(QUEST_YOHJI_BAT, XQuest.KNOWN)
				else
					AddMessage("'I can identify all items in your inventory, if you bring me a rat tail.'")
					QuestModify(QUEST_YOHJI_RAT, XQuest.KNOWN)
				end
			else
				AddMessage("'Please, complete my last request first'")
			end
		elseif (result == 'l') then
			if (AskQuestion("'Do you want to learn literacy for 500gp?'", "y n", "yes", "no") == 'y') then
				if (MoneyOperation(p, -500) >= 0) then
					LearnSkill(p, SKT_LITERACY, 1)
					MoneyOperation(t, 500)
					AddMessage("Yohjishiro touches you. You feel more educated.")
				else
					AddMessage("'You don't have enough money!'")
				end
			else
				AddMessage("'As you wish.'")
			end
		end
	elseif (e == LuaEvent.GIVE_ITEM) then
		local kind, brt, wt, it, count, name = GetItemParam(v)
		if (it == ItemType.RATTAIL or it == ItemType.BATWING) then
			if (it == ItemType.RATTAIL and QuestStatus(QUEST_YOHJI_RAT) == XQuest.KNOWN) then
				AddMessage("'Oh, thank you!' Yohjishiro touches you. Suddenly you know more about the items in your inventory.")
				QuestModify(QUEST_YOHJI_RAT, XQuest.UNKNOWN)
			elseif (it == ItemType.BATWING and QuestStatus(QUEST_YOHJI_BAT) == XQuest.KNOWN) then
				AddMessage("'Oh, thank you!'")
				MakeEffect(XEffect.GREAT_IDENTIFY, t, nil, 0, 0, p, 0, 0, 0, nil)
				QuestModify(QUEST_YOHJI_BAT, XQuest.UNKNOWN)
			else
				if (MoneyOperation(t, -50 * count) >= 0) then
					AddMessage(string.format("'I hope %d gp will be enough for this.'", 50 * count))
					MoneyOperation(p, 50 * count)
				else
					AddMessage("Sorry, I don't have enough money to buy this.")
				end
			end
		else
			AddMessage("'It is not of interest for me.'")
			return 0
		end
	end
	return 1
end
