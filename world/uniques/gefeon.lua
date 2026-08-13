function CreateGefeon(x, y)
	local gefeon = Guardian("gefeon", "roderick_guardian", x, y, 3, 4)
	SetEventHandler(gefeon, 'GefeonHandler')
end

function GefeonHandler(e, t, p, v)
	if (e ~= LuaEvent.CHAT) then
		return 0
	end

	if (QuestState:GetFlag('ahk_ulan_ordered') == 0 and QuestState:GetFlag('ahk_ulan_killed') == 0) then
		AddMessage("Ahk-Ulan, the evil wizard and master of black magic, lives in the dungeon beneath the ruins of his magic tower. The ruins are to the south-east of town. He causes great evil, and he should be eliminated.")
		QuestState:SetFlag('ahk_ulan_ordered', 1)
		return 1
	end

	if (QuestState:GetFlag('ahk_ulan_ordered') == 1 and QuestState:GetFlag('ahk_ulan_killed') == 0) then
		AddMessage("And how is Ahk-Ulan? Still alive? That is very bad.")
		return 1
	end

	if (QuestState:GetFlag('ahk_ulan_killed') == 1) then
		if (QuestState:GetFlag('roderick_killed') == 1) then
			AddMessage("Well, you killed the pretender and the King, I guess that makes you the new ruler!")
		else
			AddMessage("You did a great thing! You truly are the best!")
		end

		QuestState:WinGame()
	end

	return 1
end
