
Monster.new("ahkulan")
	:View("Ahk-Ulan, great master of Darkness", 'p', xColor.xDARKGRAY, PersonType.NAMED_HE, CreatureTemplate.UNIQUE, CreatureClass.HUMAN)
	:Basic("1d30+120", "0d0+900", "0d0+900", CreatureSize.NORMAL, "1d200+1200")
	:Body("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, CreatureTemplate.SUPPRESS_INVIS + CreatureTemplate.SEE_INVIS)
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM + XStandardAI.COWARD + XStandardAI.PEACEFUL)
	:Stats("St 1d8+15 Dx 1d8+40 To 1d8+10 Le 1d5+45 Wi 1d5+45 Ma 1d5+45 Pe 5d6 Ch 7d5")
	:Resist("see_invisible:0d0+100")
	:Combat("1d3", "1d2")
	:Main("1d4", "1d1", "1d5+30", "5d5+50")
	:Description("Once a bright and promising sorcerer, Ahk-Ulan delved into dark magics and soon became the dark and twisted being he is today.  The aura of power and decay surrounds him as he seeks to control the destructive magics he has consumed.")
	:LearnSkill(XSkill.HEALING, XSkill.MAX_LEVEL)
	:LearnSkill(XSkill.FINDWEAKNESS, XSkill.MAX_LEVEL)
	:LearnSkill(XSkill.CONCENTRATION, XSkill.MAX_LEVEL)
	:LearnSpell(Spell.ACID_BOLT)
	:LearnSpell(Spell.HEAL)
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
		elseif (QuestStatus(QUEST_USURPER) == XQuest.COMPLETE) then
			-- The parts are his and the King is dead. He has no further use
			-- for the hero, and no reason left to pretend otherwise - this
			-- is the game's second ending (XHero::EndGame scores a win with
			-- Ahk-Ulan still alive as the usurper's).
			AddMessage("'The King is dead.' Ahk-Ulan does not smile. The air around him simply grows colder.")
			AddMessage("'There is no one left in Avanor who can stand between me and the throne, and that is as much your doing as mine.'")
			AddMessage("'Go. Take what you can carry, and be far from here when I come to claim what is mine.'")
			QuestModify(QUEST_USURPER, XQuest.CLOSED)
			QuestState:WinGame("You killed the King of Avanor and helped Ahk-Ulan to become Usurper of Avanor.")
		else
			AddMessage("Don't disturb me before completing my quest, puny mortal!")
		end
	elseif (e == LuaEvent.GIVE_ITEM) then
		local kind, brt, wt, it, count, name = GetItemParam(v)
		if (it == "ancient_machine_part") then
			if (count == 3) then
				AddMessage("Very nice job, servant!")
				QuestModify(QUEST_ANCIENT_PART, XQuest.CLOSED)

				-- The machine was never the point. With it in his hands the
				-- only thing still standing between Ahk-Ulan and Avanor is
				-- the man wearing its crown, and he asks the hero to remove
				-- him. A hero who has already killed Roderick for reasons of
				-- his own skips straight to the reckoning.
				if (QuestState:GetFlag('roderick_killed') == 1) then
					AddMessage("'And Roderick already lies dead. You have been busier than I asked.'")
					AddMessage("'Come to me again when you have caught your breath. We have a throne to speak of.'")
					QuestModify(QUEST_USURPER, XQuest.COMPLETE)
				else
					AddMessage("'And now, my last request: kill Roderick, for he is the only one who can stop me now.'")
					QuestModify(QUEST_USURPER, XQuest.KNOWN)
				end

				return true
			else
				AddMessage("PLEASE! Return with THREE... THREE parts of an ancient machine!")
				return false
			end
		else
			AddMessage("Are you jeering at me?")
			return false
		end
	elseif (e == LuaEvent.DIE) then
		QuestState:SetFlag('ahk_ulan_killed', 1)

		-- He was the only buyer for the parts of the ancient machine, and there is
		-- no one else in the world to hand them to. The errand ends here, and the
		-- record should say that it ended badly rather than leave it open.
		local qs = QuestStatus(QUEST_ANCIENT_PART)

		if (qs < XQuest.CLOSED) then
			if (qs == XQuest.KNOWN) then
				AddMessage("Whatever Ahk-Ulan wanted with the ancient machine, he will not be asking for its parts now.")
			end

			QuestModify(QUEST_ANCIENT_PART, XQuest.FAIL)
		end

		-- Gefeon wanted him dead and now he is. The errand is done the
		-- moment he falls; going back to say so is what closes it
		-- (GefeonHandler), and until then it stops cluttering the log.
		if (QuestStatus(QUEST_GEFEON) == XQuest.KNOWN) then
			QuestModify(QUEST_GEFEON, XQuest.COMPLETE)
		end

		-- The throne he wanted taken for him goes the same way, but only while the
		-- King still stands. If Roderick is already dead the hero did exactly what
		-- was asked, so it stays COMPLETE.
		if (QuestStatus(QUEST_USURPER) == XQuest.KNOWN) then
			QuestModify(QUEST_USURPER, XQuest.FAIL)
		end
	end
	return true
end
