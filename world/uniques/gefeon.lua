
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
	:LearnSkill(SKT_HEALING, SKILL_MAX_LEVEL)
	:LearnSpell(SPELL_FIRE_BOLT)
	:LearnSpell(SPELL_HEAL)
	:Unique()
	:Register()


function CreateGefeon(x, y)
	local gefeon = Guardian("gefeon", "roderick_guardian", x, y, 3, 4)
	SetEventHandler(gefeon, 'GefeonHandler')
end

function GefeonHandler(e, t, p, v)
	if (e ~= LuaEvent.CHAT) then
		return 0
	end

	if (QuestState:GetFlag('ahk_ulan_ordered') == 0 and QuestState:GetFlag('ahk_ulan_killed') == 0) then
		AddMessage("Ahk-Ulan, the evil wizard and master of black magic, lives in the dungeon beneath the ruins of his magic tower. The ruins are to the south-east of town. He causes great evil, and he should be eliminated.")
		QuestState:SetFlag('ahk_ulan_ordered', 1)
		return 1
	end

	if (QuestState:GetFlag('ahk_ulan_ordered') == 1 and QuestState:GetFlag('ahk_ulan_killed') == 0) then
		AddMessage("And how is Ahk-Ulan? Still alive? That is very bad.")
		return 1
	end

	if (QuestState:GetFlag('ahk_ulan_killed') == 1) then
		if (QuestState:GetFlag('roderick_killed') == 1) then
			AddMessage("Well, you killed the pretender and the King, I guess that makes you the new ruler!")
		else
			AddMessage("You did a great thing! You truly are the best!")
		end

		QuestState:WinGame()
	end

	return 1
end
