function CreateElderGridor(x, y)
	local elder = Guardian("elder_gridor", "small_village_farmer", x, y, 5, 1)
	SetEventHandler(elder, 'ElderGridorHandler')
end


function ElderGridorHandler(e, t, p, v)
	if (e == LuaEvent.CHAT) then
		local qs = QuestStatus(QUEST_ELDER)
		if (qs == XQuest.UNKNOWN) then
			AddMessage('Good day, friend! All that we have are our mushrooms.')
			AddMessage('We collect them in the cave to the west of the village.')
			AddMessage('But a short time ago, an evil monster occupied the cave.')
			AddMessage('It looks like a demon and is very dangerous.')
			AddMessage('Maybe the people who live to the south of the lake can help.')
			QuestModify(QUEST_ELDER, XQuest.KNOWN)
		elseif (qs == XQuest.KNOWN) then
			AddMessage('The evil monster is still there.')
		elseif (qs == XQuest.COMPLETE) then
			AddMessage('Thank you for your great help! Now, our farmers can collect mushrooms.')
			QuestModify(QUEST_ELDER, XQuest.CLOSED)
			SendFarmersToCollectMushrooms()
		else
			AddMessage('Have a nice day,')
			if (Gender(p) == Gender.MALE) then
				AddMessage('sir!')
			else
				AddMessage("ma'am!")
			end
		end
	elseif (e == LuaEvent.SAVE) then
	elseif (e == LuaEvent.LOAD) then
	end
	return 1
end
