
Monster.new("torin")
	:View("Torin the Great, dwarven king", 'h', xColor.xLIGHTGREEN, PersonType.NAMED_HE, CreatureTemplate.UNIQUE, CreatureClass.HUMANOID)
	:Basic("1d10+100", "0d0+800", "0d0+800", CreatureSize.NORMAL, "1d200+1200")
	:Body("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, CreatureTemplate.SUPPRESS_INVIS + CreatureTemplate.SEE_INVIS)
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM + XStandardAI.COWARD + XStandardAI.PEACEFUL)
	:Stats("St 1d8+35 Dx 1d8+20 To 1d8+30 Le 1d5+25 Wi 1d5+25 Ma 1d5+25 Pe 5d6 Ch 4d5")
	:Resist("see_invisible:0d0+100")
	:Combat("1d3", "1d2")
	:Main("1d4", "1d2", "1d5+30", "1d5+10")
	:Description("Twin brother to the master smith, Torin's obvious strength and coloring are all that own the relationship.  The crown on his head glitters and his great golden beard flows over his expansive chest.  The hand he rests on the hammer at his belt belies his prowess in battle.  As your eyes drift to the door at the back of his throne room he clears his throat as if to address you, and draw your attention.")
	:LearnSkill(XSkill.HEALING, XSkill.MAX_LEVEL)
	:LearnSkill(XSkill.FINDWEAKNESS, XSkill.MAX_LEVEL)
	:Unique()
	:Register()


-- The dwarven king's own gear.

Item.new("torin_axe")
	:Weapon("great_axe")
	:View("ancient axe", '\\')
	:Basic(12000, 1200)
	:Armour(0, 0)
	:Combat(8, 2, 8, 5)
	:Resist("stun:1d1+99 confuse:1d1+99 see_invisible:0d0+20")
	:Stats("To:1d1+10")
	:Brand("cold")
	:Called("Axe of Torin")
	:Unique()
	:Register()

Item.new("torin_shield")
	:Shield("medium_shield")
	:View("ancient shield", ')')
	:Basic(8000, 100)
	:Armour(12, 2)
	:Combat(0, 0, 0, 0)
	:Resist("stun:1d1+99 confuse:1d1+99 fire:1d1+99 cold:1d1+99 acid:1d1+99 see_invisible:0d0+20")
	:Stats("To:0d0+5")
	:Called("Shield of Torin")
	:Unique()
	:Register()

Item.new("dwarf_crown")
	:Cap("cap")
	:View("mithril crown", '[', xColor.xLIGHTCYAN)
	:Basic(8000, 100)
	:Armour(2, 2)
	:Combat(0, 0, 0, 0)
	:Resist("stun:1d1+99 confuse:1d1+99 fire:1d1+99 cold:1d1+99 acid:1d1+99 see_invisible:0d0+20")
	:Stats("St:0d0+5 Dx:0d0+5 To:0d0+5 Wi:0d0+5 Ma:0d0+5")
	:Called("dwarven crown")
	:Unique()
	:Register()


torin_award = 0;

function CreateTorin(x, y)
	local torin = Guardian("torin", "dwarven_guardian", x, y)
	SetEventHandler(torin, 'TorinHandler')
	local pickaxe = CreateObject("pickaxe")
	torin_award = GetObjectGUID(pickaxe)
	GiveObjectToCreature(pickaxe, torin)
	GiveObjectToCreature(CreateObject('dwarf_crown'), torin)
	GiveObjectToCreature(CreateObject('torin_shield'), torin)
	GiveObjectToCreature(CreateObject('torin_axe'), torin)
end

function TorinHandler(e, t, p, v)
	if (e == LuaEvent.CHAT) then
		local qs = QuestStatus(QUEST_TORIN)
		if (qs == XQuest.UNKNOWN) then
			AddMessage("Hello, brave hero. As you know, we dwarves mine our treasures deep from the ground. Some time ago, one of our mine was filled by a mysterious gas, which slowly kills all living things. It is oozing from the rocks. We have gas pump there, but there is no one who can switch this pump on. Please solve this problem.")
			QuestModify(QUEST_TORIN, XQuest.KNOWN)
		elseif (qs == XQuest.KNOWN) then
			AddMessage("You haven\'t completed my previous request? Hmm... GET OUT OF HERE!")
		elseif (qs == XQuest.COMPLETE) then
			AddMessage("Thank you for your great help.")
			QuestModify(QUEST_TORIN, XQuest.CLOSED)
			if (GiveAward(t, torin_award, p)) then
				AddMessage('Take this tool as a reward.')
			end
		else
			AddMessage("Thank you for your help.")
		end
	elseif (e == LuaEvent.SAVE) then
		StoreInt(torin_award)
	elseif (e == LuaEvent.LOAD) then
		torin_award = RestoreInt()
	end
	return true
end
