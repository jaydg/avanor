
Monster.new("yohjishiro")
	:View("Yohjishiro, the elven wizard", 'h', xColor.xWHITE, PersonType.NAMED_SHE, CreatureTemplate.UNIQUE, CreatureClass.HUMANOID)
	:Basic("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200")
	:Body("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, CreatureTemplate.SUPPRESS_INVIS + CreatureTemplate.SEE_INVIS)
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM + XStandardAI.COWARD + XStandardAI.PEACEFUL)
	:Stats("St 1d8+15 Dx 1d8+40 To 1d8+10 Le 1d5+45 Wi 1d5+45 Ma 1d5+45 Pe 5d6 Ch 7d5")
	:Resist("see_invisible:0d0+100")
	:Combat("1d3", "1d2")
	:Main("1d4", "1d1", "1d5+10", "5d5+50")
	:Description("Last of the elder wizards to live in the vale of Avanor, Yohjishiro quietly tends her herbs and gardens. Her white hair hangs down her back as she kneels down to look at one of her bushes.  Her pointed ears stick out from under a floppy hat that keeps the sun from her eyes.  As she walks through the garden, you notice plants sprouting wherever her feet touch the soil.  Truly she is a power of life.  Frail but intelligent, she remains apart from most of the happenings of Avanor.  Occasionally she will accept a pupil or give aid to those in need.")
	:LearnSkill(SKT_HEALING, SKILL_MAX_LEVEL)
	:LearnSpell(SPELL_LIGHTNING_BOLT)
	:LearnSpell(SPELL_HEAL)
	:Equip(ItemKind.HAT, ItemType.HAT, 100)
	:Equip(ItemKind.WEAPON, ItemType.STAFF, 100)
	:Unique()
	:Register()


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
