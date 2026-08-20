
Monster.new("ozorik")
	:View("Ozorick, the royal guard captain", 'p', xColor.xLIGHTBLUE, PersonType.NAMED_HE, CreatureTemplate.UNIQUE, CreatureClass.HUMAN)
	:Basic("1d10+95", "0d0+1000", "0d0+1000", CreatureSize.NORMAL, "1d200+1200")
	:Body("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, CreatureTemplate.SUPPRESS_INVIS + CreatureTemplate.SEE_INVIS)
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM + XStandardAI.COWARD + XStandardAI.PEACEFUL)
	:Stats("St 1d8+30 Dx 1d8+30 To 1d8+15 Le 1d5+15 Wi 1d4+5 Ma 1d4+5 Pe 3d6 Ch 5d5")
	:Resist("see_invisible:0d0+50")
	:Combat("1d8", "2d2")
	:Main("4d2", "1d3", "1d5+15", "1d3+5")
	:Description("Standing just over five feet tall, Ozorick doesn't seem like such an imposing figure but the air of command about him is complete.  His thickly corded neck and arms lightly balance a huge broadsword covered with runes.  He swings it down as you enter and you can't help but gasp as the point cuts right through a stone in the floor and barely even slows.  No one who has seen him wield his mighty sword in battle would dream of opposing him.")
	:LearnSkill(SKT_HEALING, 10)
	:LearnSkill(SKT_FINDWEAKNESS, 10)
	:Unique()
	:Register()


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
