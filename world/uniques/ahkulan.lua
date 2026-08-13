function CreateAhkUlan(x, y)
	local ahkulan = Guardian("ahkulan", "ahkulan_guardian", x, y)
	SetEventHandler(ahkulan, 'AhkUlanHandler')
end


function AhkUlanHandler(e, t, p, v)
	if (e == LuaEvent.CHAT) then
		local qs = QuestStatus(QUEST_ANCIENT_PART)
		if (qs == XQuest.UNKNOWN) then
			AddMessage("Hello, brave hero.")
			AddMessage("Some years ago, some evil wizards destroyed my tower.")
			AddMessage("Now I wait here gaining strength and planning my revenge.")
			AddMessage("I am searching for 3 parts to an ancient machine.")
			AddMessage("Bring them to me and I will reward you well.")
			QuestModify(QUEST_ANCIENT_PART, XQuest.KNOWN)
		elseif (qs == XQuest.KNOWN) then
			AddMessage("Don't disturb me before completing my quest, puny mortal!")
		end
	elseif (e == LuaEvent.GIVE_ITEM) then
		local kind, brt, wt, it, count, name = GetItemParam(v)
		if (it == ItemType.ANCIENTMACHINEPART) then
			if (count == 3) then
				AddMessage("Very nice job, servant!")
				QuestModify(QUEST_ANCIENT_PART, XQuest.CLOSED)
				return 1
			else
				AddMessage("PLEASE! Return with THREE... THREE parts of an ancient machine!")
				return 0
			end
		else
			AddMessage("Are you jeering at me?")
			return 0
		end
	end
	return 1
end
