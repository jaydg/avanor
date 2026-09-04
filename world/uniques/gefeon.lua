
Monster.new("gefeon")
	:View("Gefeon, great master of Fire", 'p', xColor.xRED, PersonType.NAMED_HE, CreatureTemplate.UNIQUE, CreatureClass.HUMAN)
	:Basic("1d10+110", "0d0+1000", "0d0+1000", CreatureSize.NORMAL, "1d200+1200")
	:Body("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, CreatureTemplate.SUPPRESS_INVIS + CreatureTemplate.SEE_INVIS)
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM + XStandardAI.COWARD + XStandardAI.PEACEFUL)
	:Stats("St 1d8+15 Dx 1d8+40 To 1d8+10 Le 1d5+45 Wi 1d5+45 Ma 1d5+45 Pe 5d6 Ch 7d5")
	:Resist("see_invisible:0d0+100")
	:Combat("1d3", "1d2")
	:Main("1d4", "1d1", "1d5+10", "5d5+50")
	:Description("As master of the eternal flame of Avanor, Gefeon advises the king in all matters arcane.  Clad only in robes and wearing rings of power, he sits and meditates on the state of the universe.  As you watch a furrow creases on his brow as if he is greatly worried by something.")
	:LearnSkill(XSkill.HEALING, XSkill.MAX_LEVEL)
	:LearnSpell("fire_bolt")
	:LearnSpell("heal")
	:Unique()
	:Register()


function CreateGefeon(x, y)
	local gefeon = Guardian("gefeon", "roderick_guardian", x, y, 3, 4)
	SetEventHandler(gefeon, 'GefeonHandler')
end

function GefeonHandler(e, t, p, v)
	if (e ~= LuaEvent.CHAT) then
		return false
	end

	-- A hero who went down there and finished Ahk-Ulan off before ever
	-- meeting Gefeon is welcome all the same: the errand is judged by the
	-- wizard being dead, not by who asked for it.
	if (QuestState:GetFlag('ahk_ulan_killed') == 1) then
		local ending

		if (QuestState:GetFlag('roderick_killed') == 1) then
			-- Both of them dead and the throne of Avanor standing empty.
			-- Gefeon is the last man in the kingdom whose word carries, and
			-- he gives it to the hero.
			AddMessage("Well, you killed the pretender and the King, I guess that makes you the new ruler!")
			ending = string.format(
				"You killed Ahk-Ulan and the King of Avanor and became the new %s of Avanor.",
				AsCreature(p):IsMale() and "King" or "Queen")
		else
			AddMessage("You did a great thing! You truly are the best!")
			ending = "You killed evil Ahk-Ulan and saved the Kingdom of Avanor from Ahk-Ulan's deadly plans."
		end

		QuestModify(QUEST_GEFEON, XQuest.CLOSED)
		QuestState:WinGame(ending)
		return true
	end

	if (QuestStatus(QUEST_GEFEON) == XQuest.UNKNOWN) then
		AddMessage("Ahk-Ulan, the evil wizard and master of black magic, lives in the dungeon beneath the ruins of his magic tower. The ruins are to the south-east of town. He causes great evil, and he should be eliminated.")
		QuestModify(QUEST_GEFEON, XQuest.KNOWN)
		return true
	end

	AddMessage("And how is Ahk-Ulan? Still alive? That is very bad.")

	return true
end
