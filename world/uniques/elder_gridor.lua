
Monster.new("elder_gridor")
	:View("Elder Gridor", 'p', xColor.xWHITE, PersonType.NAMED_HE, CreatureTemplate.UNIQUE, "human")
	:Basic("1d10+95", "0d0+1000", "0d0+1000", CreatureSize.NORMAL, "1d200+1200")
	:Body("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 0)
	:Never("invisible")
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM + XStandardAI.COWARD + XStandardAI.PEACEFUL)
	:Stats("St 1d3+5 Dx 1d3+5 To 1d3+4 Le 9d5 Wi 1d4+5 Ma 1d4+5 Pe 3d6 Ch 4d4")
	:Resist("")
	:Combat("1d1", "1d1")
	:Main("1d1", "0d0", "1d5+5", "1d5+5")
	:Description("This kind looking old man leans on a cane as he gazes towards you.  As leader of this small farming community, he is responsible for their well being.  He gazes at you and asks for help to a serious problem that has recently arisen and is out of his power to control.")
	:LearnSkill(XSkill.HEALING, 6)
	:EquipCount(ItemKind.SCROLL + ItemKind.POTION, 3, 30)
	:Equip(ItemKind.BODY, "clothes", 100)
	:Equip(ItemKind.WEAPON, "staff", 100)
	:Unique()
	:Register()


function CreateElderGridor(x, y)
	local elder = Guardian("elder_gridor", VILLAGE_GROUP, x, y, 5, 1)
	SetEventHandler(elder, 'ElderGridorHandler')
end


-- The Elder's second errand: the forest brothers on the old road east.
-- Split out of ElderGridorHandler only because it has four states of its
-- own and would otherwise bury the demon quest above it.
function ElderBanditQuest(p)
	local bq = QuestStatus("bandits")

	if (bq >= XQuest.CLOSED) then
		AddMessage("'The road is open and the carts come through. Have a nice day,'")

		if (Gender(p) == Gender.MALE) then
			AddMessage('sir!')
		else
			AddMessage("ma'am!")
		end

		return
	end

	-- The road clear - whether he asked for it or not. Counted by
	-- BanditsOnTheRoad(), which leaves Jorgus out of it: he leads them, but
	-- killing a peaceful man in his house is not what was asked, and he is
	-- the valley's only teacher of stealing.
	if (BanditsOnTheRoad() == 0) then
		if (bq == XQuest.UNKNOWN) then
			AddMessage("'You have been east already, have you? We wondered why the smoke over the bridge went out.'")
		else
			AddMessage("'The road is open! We have not walked it in years.'")
		end

		AddMessage("'This is what the village can spare, and it is given gladly.'")
		MoneyOperation(p, 250)
		QuestModify("bandits", XQuest.CLOSED)
		return
	end

	if (bq == XQuest.UNKNOWN) then
		AddMessage("'There is one thing more, if you have the stomach for it.'")
		AddMessage("'The old road east is ours by right and we have not used it in years. Five of them hold the bridge - forest brothers, they call themselves, as if robbery were a family trade.'")
		AddMessage("'And one of them took one of ours. Brida's girl. Go and speak to her; she will tell it better than I can.'")
		AddMessage("'Clear the road for us and the valley will not forget it.'")
		QuestModify("bandits", XQuest.KNOWN)
	else
		AddMessage("'They are still out there. We hear them at night, on the far bank.'")
	end
end

function ElderGridorHandler(e, t, p, v)
	if (e == LuaEvent.CHAT) then
		local qs = QuestStatus("elder")
		if (qs == XQuest.UNKNOWN) then
			AddMessage('Good day, friend! All that we have are our mushrooms.')
			AddMessage('We collect them in the cave to the west of the village.')
			AddMessage('But a short time ago, an evil monster occupied the cave.')
			AddMessage('It looks like a demon and is very dangerous.')
			AddMessage('Maybe the people who live to the south of the lake can help.')
			QuestModify("elder", XQuest.KNOWN)
		elseif (qs == XQuest.KNOWN) then
			AddMessage('The evil monster is still there.')
		elseif (qs == XQuest.COMPLETE) then
			AddMessage('Thank you for your great help! Now, our farmers can collect mushrooms.')
			QuestModify("elder", XQuest.CLOSED)
			SendFarmersToCollectMushrooms()
		else
			-- The demon is dealt with and the farmers are back at work. The
			-- road east is the village's other old grievance, and this is
			-- the point in the game where it becomes a fight the player can
			-- have: one of the brothers loses to a level-5 hero about seven
			-- times in eight, where at level 1 it is the other way round.
			ElderBanditQuest(p)
		end
	elseif (e == LuaEvent.SAVE) then
	elseif (e == LuaEvent.LOAD) then
	end
	return true
end
