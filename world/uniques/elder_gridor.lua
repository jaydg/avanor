
Monster.new("elder_gridor")
	:View("Elder Gridor", 'p', xColor.xWHITE, PersonType.NAMED_HE, CreatureTemplate.UNIQUE, CreatureClass.HUMAN)
	:Basic("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200")
	:Body("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 0, CreatureTemplate.SUPPRESS_INVIS)
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM + XStandardAI.COWARD + XStandardAI.PEACEFUL)
	:Stats("St 1d3+5 Dx 1d3+5 To 1d3+4 Le 9d5 Wi 1d4+5 Ma 1d4+5 Pe 3d6 Ch 4d4")
	:Resist("")
	:Combat("1d1", "1d1")
	:Main("1d1", "0d0", "1d5+5", "1d5+5")
	:Description("This kind looking old man leans on a cane as he gazes towards you.  As leader of this small farming community, he is responsible for their well being.  He gazes at you and asks for help to a serious problem that has recently arisen and is out of his power to control.")
	:LearnSkill(SKT_HEALING, 6)
	:EquipCount(ItemKind.SCROLL + ItemKind.POTION, 3, 30)
	:Equip(ItemKind.BODY, ItemType.CLOTHES, 100)
	:Equip(ItemKind.WEAPON, ItemType.STAFF, 100)
	:Unique()
	:Register()


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
