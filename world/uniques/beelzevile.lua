function CreateBeelzevile()
	local demon = Creature("beelzevile")
	SetEventHandler(demon, 'BeelzevileHandler')
	GiveObjectToCreature(CreateObject('XGreatElementalRing'), demon)
	-- no-op outside "-demo" mode - see XLocation::SetMainCreature
	SetMainCreature(demon)
end

function BeelzevileHandler(e, t, p, v)
	if (e == LuaEvent.DIE) then
		QuestModify(QUEST_ELDER, XQuest.COMPLETE)
	end
	return 0
end
