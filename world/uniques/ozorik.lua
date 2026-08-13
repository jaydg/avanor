ozorik_award = 0
orcs_live = 50


function CreateOzorik(x, y)
	local ozorik = Guardian("ozorik", "guardian", x, y, 3, 2)
	SetEnemy(ozorik, CreatureClass.ORC)
	SetEventHandler(ozorik, 'OzorikHandler')
	GiveObjectToCreature(CreateObject('XGlamdring'), ozorik)
	local death_hack = CreateObject('XDeathHack')
	ozorik_award = GetObjectGUID(death_hack)
	GiveObjectToCreature(death_hack, ozorik)
end


function OzorikHandler(e, t, p, v)
	if (e == LuaEvent.CHAT) then
		local qs = QuestStatus(QUEST_OZORIK)
		local demon_quest = QuestStatus(QUEST_ELDER)
		if (orcs_live > 0 and qs < XQuest.COMPLETE) then
			if (qs == XQuest.UNKNOWN) then
				if (demon_quest == XQuest.KNOWN) then
					AddMessage("'Demons? We are mighty enough to slay them, but now another problem approaches from the south - an orc war-party!'")
				elseif (demon_quest == XQuest.UNKNOWN) then
					AddMessage("'Sorry, but I'm really busy now. The orc war-party will be here soon!'")
				end
				QuestModify(QUEST_OZORIK, XQuest.KNOWN)
			else
				AddMessage("'Sorry, but I'm really busy right now. The orc war-party will be here soon!'")
			end
		else
			if (qs < XQuest.CLOSED) then
				AddMessage("'You gained us victory!'")
				if (GiveAward(t, ozorik_award, p)) then
					AddMessage('Take this dagger as a reward!')
				end
				QuestModify(QUEST_OZORIK, XQuest.CLOSED)
			else
				AddMessage('Good day, hero!')
			end
		end
	elseif (e == LuaEvent.GIVE_ITEM) then
		local kind, brt, wt, it, count, name = GetItemParam(v)
		if (BinaryAND(kind, ItemKind.WEAPON) and BinaryAND(brt, AttackEffectType.ORCSLAYER) and wt == WSK_SWORD) then
			AddMessage("'Wow, you've probably saved our lives! Please, take this weapon to one of my guardians, then return to me!'")
		else
			AddMessage("'We are not looking for this.'")
			return 0
		end
	elseif (e == LuaEvent.SAVE) then
		StoreInt(ozorik_award)
	elseif (e == LuaEvent.LOAD) then
		ozorik_award = RestoreInt()
	end
	return 1
end
