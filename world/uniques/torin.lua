torin_award = 0;

function CreateTorin(x, y)
	local torin = Guardian("torin", "dwarven_guardian", x, y)
	SetEventHandler(torin, 'TorinHandler')
	local pickaxe = CreateObject('XPickAxe')
	torin_award = GetObjectGUID(pickaxe)
	GiveObjectToCreature(pickaxe, torin)
	GiveObjectToCreature(CreateObject('XDwarfCrown'), torin)
	GiveObjectToCreature(CreateObject('XTorinShield'), torin)
	GiveObjectToCreature(CreateObject('XTorinAxe'), torin)
end

function TorinHandler(e, t, p, v)
	if (e == LuaEvent.CHAT) then
		local qs = QuestStatus(QUEST_TORIN)
		if (qs == XQuest.UNKNOWN) then
			AddMessage("Hello, brave hero. As you know, we dwarves mine our treasures deep from the ground. Some time ago, one of our mine was filled by a mysterious gas, which slowly kills all living things. It is oozing from the rocks. We have gas pump there, but there is no one who can switch this pump on. Please solve this problem.")
			QuestModify(QUEST_TORIN, XQuest.KNOWN)
		elseif (qs == XQuest.KNOWN) then
			AddMessage("You haven\'t completed my previous request? Hmm... GET OUT OF HERE!")
		elseif (qs == XQuest.COMPLETE) then
			AddMessage("Thank you for your great help.")
			QuestModify(QUEST_TORIN, XQuest.CLOSED)
			if (GiveAward(t, torin_award, p)) then
				AddMessage('Take this tool as a reward.')
			end
		else
			AddMessage("Thank you for your help.")
		end
	elseif (e == LuaEvent.SAVE) then
		StoreInt(torin_award)
	elseif (e == LuaEvent.LOAD) then
		torin_award = RestoreInt()
	end
	return 1
end
