
Monster.new("ozorik")
	:View("Ozorick, the royal guard captain", 'p', xColor.xLIGHTBLUE, PersonType.NAMED_HE, CreatureTemplate.UNIQUE, "human")
	:Basic("1d10+95", "0d0+1000", "0d0+1000", CreatureSize.NORMAL, "1d200+1200")
	:Body("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100)
	:Never("invisible")
	:Always("see_invisible")
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM + XStandardAI.COWARD + XStandardAI.PEACEFUL)
	:Stats("St 1d8+30 Dx 1d8+30 To 1d8+15 Le 1d5+15 Wi 1d4+5 Ma 1d4+5 Pe 3d6 Ch 5d5")
	:Resist("see_invisible:0d0+50")
	:Combat("1d8", "2d2")
	:Main("4d2", "1d3", "1d5+15", "1d3+5")
	:Description("Standing just over five feet tall, Ozorick doesn't seem like such an imposing figure but the air of command about him is complete.  His thickly corded neck and arms lightly balance a huge broadsword covered with runes.  He swings it down as you enter and you can't help but gasp as the point cuts right through a stone in the floor and barely even slows.  No one who has seen him wield his mighty sword in battle would dream of opposing him.")
	:LearnSkill(XSkill.HEALING, 10)
	:LearnSkill(XSkill.FINDWEAKNESS, 10)
	:Unique()
	:Register()


-- Ozorick's unique weapons.

Item.new("glamdring")
	:Weapon("broad_sword")
	:View("rune-covered broadsword", '|')
	:Basic(2000, 700)
	:Armour(0, 0)
	:Combat(12, 4, 5, 10)
	:Resist("poison:1d1+99 stun:1d1+99 confuse:1d1+99 fire:1d1+99 cold:1d1+99")
	:Stats("St:1d1+9 Dx:1d1+9 To:1d1+9")
	:Brand("fire")
	:Brand("cold")
	:Brand("demon_slayer")
	:Called('broadsword named "Glamdring"')
	:Unique()
	:Register()

Item.new("death_hack")
	:Weapon("long_dagger")
	:View("rune-covered long dagger")
	:Basic(500, 70)
	:Armour(0, 0)
	:Combat(6, 2, 3, 12)
	:Resist("")
	:Stats("")
	:Brand("cold")
	:Brand("demon_slayer")
	:Called('long dagger named "Death Hack"')
	:Unique()
	:Register()


ozorik_award = 0
orcs_live = 50


function CreateOzorik(x, y)
	local ozorik = Guardian("ozorik", "guardian", x, y, 3, 2)
	SetEnemy(ozorik, "orc")
	SetEventHandler(ozorik, 'OzorikHandler')
	GiveObjectToCreature(CreateObject('glamdring'), ozorik)
	local death_hack = CreateObject('death_hack')
	ozorik_award = GetObjectGUID(death_hack)
	GiveObjectToCreature(death_hack, ozorik)
end


function OzorikHandler(e, t, p, v)
	if (e == LuaEvent.CHAT) then
		local qs = QuestStatus("ozorik")
		local demon_quest = QuestStatus("elder")
		if (orcs_live > 0 and qs < XQuest.COMPLETE) then
			if (qs == XQuest.UNKNOWN) then
				if (demon_quest == XQuest.KNOWN) then
					AddMessage("'Demons? We are mighty enough to slay them, but now another problem approaches from the south - an orc war-party!'")
				elseif (demon_quest == XQuest.UNKNOWN) then
					AddMessage("'Sorry, but I'm really busy now. The orc war-party will be here soon!'")
				end
				QuestModify("ozorik", XQuest.KNOWN)
			else
				AddMessage("'Sorry, but I'm really busy right now. The orc war-party will be here soon!'")
			end
		else
			if (qs < XQuest.CLOSED) then
				AddMessage("'You gained us victory!'")
				if (GiveAward(t, ozorik_award, p)) then
					AddMessage('Take this dagger as a reward!')
				end
				QuestModify("ozorik", XQuest.CLOSED)
			else
				AddMessage('Good day, hero!')
			end
		end
	elseif (e == LuaEvent.GIVE_ITEM) then
		local kind, brt, wt, it, count, name = GetItemParam(v)
		if (IsKind(kind, ItemKind.WEAPON) and HasBrand(brt, "orc_slayer") and wt == "sword") then
			AddMessage("'Wow, you've probably saved our lives! Please, take this weapon to one of my guardians, then return to me!'")
		else
			AddMessage("'We are not looking for this.'")
			return false
		end
	elseif (e == LuaEvent.DIE) then
		-- The captain is the quest. He is the only one who judges the orc-slaying
		-- sword, the only one who pays for it, and the only one his guards take
		-- orders from - his men fight on, but there is nothing left to finish. Fail
		-- it silently: he may well go down while the hero is far away, and the news
		-- belongs to whichever of his guards the hero speaks to next.
		if (QuestStatus("ozorik") < XQuest.CLOSED) then
			QuestModify("ozorik", XQuest.FAIL)
		end
	elseif (e == LuaEvent.SAVE) then
		StoreInt(ozorik_award)
	elseif (e == LuaEvent.LOAD) then
		ozorik_award = RestoreInt()
	end
	return true
end
