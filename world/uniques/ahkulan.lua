
	Monster.new("ahkulan")
		:View("Ahk-Ulan, great master of Darkness", 'p', xColor.xDARKGRAY, PersonType.NAMED_HE, CreatureTemplate.UNIQUE, CreatureClass.HUMAN)
		:Basic("1d30+120", "0d0+900", "0d0+900", CS_NORMAL, "1d200+1200")
		:Body("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, CreatureTemplate.SUPPRESS_INVIS + CreatureTemplate.SEE_INVIS)
		:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM + XStandardAI.COWARD + XStandardAI.PEACEFUL)
		:Stats("St 1d8+15 Dx 1d8+40 To 1d8+10 Le 1d5+45 Wi 1d5+45 Ma 1d5+45 Pe 5d6 Ch 7d5")
		:Resist("see_invisible:0d0+100")
		:Combat("1d3", "1d2")
		:Main("1d4", "1d1", "1d5+30", "5d5+50")
		:Description("Once a bright and promising sorcerer, Ahk-Ulan delved into dark magics and soon became the dark and twisted being he is today.  The aura of power and decay surrounds him as he seeks to control the destructive magics he has consumed.")
		:LearnSkill(SKT_HEALING, SKILL_MAX_LEVEL)
		:LearnSkill(SKT_FINDWEAKNESS, SKILL_MAX_LEVEL)
		:LearnSkill(SKT_CONCENTRATION, SKILL_MAX_LEVEL)
		:LearnSpell(SPELL_ACID_BOLT)
		:LearnSpell(SPELL_HEAL)
		:Unique()
		:Register()


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
