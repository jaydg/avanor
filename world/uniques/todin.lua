--
-- The king's brother and the best hand with a hammer in the mountain.
--
Monster.new("todin", "dwarf")
	:View("Todin, dwarven weaponsmith", 'h', xColor.xBROWN, PersonType.NAMED_HE, CreatureTemplate.UNIQUE, "humanoid")
	:Body("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100)
	:Never("invisible")
	:Always("see_invisible")
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM + XStandardAI.PEACEFUL)
	:Stats("St 1d8+30 Dx 1d8+30 To 1d8+15 Le 1d5+15 Wi 1d4+5 Ma 1d4+5 Pe 3d6 Ch 5d5")
	:Combat("1d8", "2d2")
	:Main("4d2", "1d3", "1d5+15", "1d5+5")
	:Description("Squat, sturdy and built like a boulder here stands the "
		.. "King's twin brother and master smith. Todin stands at his forge "
		.. "and works the bellows with one hand while nonchalantly shaping "
		.. "a sword with the hammer in his other hand. The ruddy glow of "
		.. "the forge glimmers on his sweat drenched skin. Truly he is a "
		.. "master smith as the weapons hanging about the room display his "
		.. "craft.")
	:LearnSkill(XSkill.HEALING, 6)
	:LearnSkill(XSkill.FINDWEAKNESS, 6)
	:Unique()
	:Register()


function CreateTodin(x, y)
	local todin = Guardian("todin", "dwarven_guardian", x, y, 6, 4)
	SetEventHandler(todin, 'TodinHandler')
end


-- What Todin can forge into a blade, by the key that picks it. The answer
-- AskQuestion() hands back is the key's own letter, so this doubles as the
-- list of what he will take for an answer.
--
-- He used to choose for you, at random. A player who wanted one particular
-- brand had no way to ask for it: each attempt cost 450 gp and a weapon,
-- and once a weapon carried any of the three he would not touch it again
-- (he replaces what a weapon carries rather than adding to it - see
-- SetItemBrand), so the wrong answer meant finding another blade as well.
local TODIN_BRANDS = {
	c = "cold",
	f = "fire",
	o = "orc_slayer",
}


-- What the runes on the black club say, once somebody who can read them
-- has it in his hands. It matters that they are a gift and not an
-- enchantment: a hero who took the club off a body learns here what he
-- took, and there is nothing Todin can do about it afterwards.
local WARD_RUNES = "'Given, freely, to the hand that holds it.'"


-- Todin's side of the errand. The club is his family's work, so he knows
-- it by its brand and its weight rather than by its name.
local function IsTheWard(item)
	return GetItemId(item) == "black_club"
end


-- Whether the hero has been up there and seen what is holding it. Set
-- when Xshee-Voo is spoken to or handed something, so that "I have found
-- your slab, and it is not lying in the ash" is only sayable by somebody
-- who actually knows.
TODIN_WARD_SEEN = false

-- Whether the cyclops was left alive and warded. Todin can tell, because
-- an honest trade leaves the runes quiet and a killing does not - see
-- WARD_RUNES.
TODIN_WARD_HONOURED = false


function TodinHandler(e, t, p, v)
	if (e == LuaEvent.CHAT) then
		local ward = QuestStatus("todin_ward")

		-- His brother's errand first. Until the gas pump is running, the
		-- hero is not somebody Todin sends up a mountain.
		if (ward == XQuest.UNKNOWN and QuestStatus("torin") == XQuest.CLOSED) then
			AddMessage("'You did my brother a good turn down in the mine, so "
				.. "I'll ask you something for myself. My great-grandfather "
				.. "cut a slab of black obsidian and wrote the deep fire "
				.. "into it, so a smith could stand where no man stands. It "
				.. "went up the mountain with the cyclopes, and none of them "
				.. "came back down. Bring me the slab. I would see his hand "
				.. "again before I die.'")
			QuestModify("todin_ward", XQuest.KNOWN)
			return true
		elseif (ward == XQuest.KNOWN) then
			if (TODIN_WARD_SEEN) then
				-- Ending (c): the hero found it, found what is holding it,
				-- and came back to say so rather than bring it down.
				AddMessage("'Still up there, is it. And something still "
					.. "holding it.' He is quiet for a while. 'Then it is "
					.. "where my great-grandfather meant it to be, and I am "
					.. "too old to argue with that. Here - for the walk.'")
				QuestModify("todin_ward", XQuest.CLOSED)

				if (MoneyOperation(t, -300) >= 0) then
					MoneyOperation(p, 300)
				end

				return true
			end

			AddMessage("'The crater in the mountains, south and east. Black "
				.. "rock, and hot. You will know the slab when you see it.'")
			return true
		elseif (ward == XQuest.CLOSED) then
			AddMessage("'My great-grandfather's hand. I am glad to have seen "
				.. "it.'")
			return true
		end

		AddMessage("'Give me your weapon, and I'll make it the best!'")
		return true
	elseif (e == LuaEvent.GIVE_ITEM) then
		local kind, brt, wt, it, count, name = GetItemParam(v)

		-- The ward, before the fire-brand test below: the black club
		-- already carries fire, so without this branch Todin would answer
		-- his family's own work with "this weapon's good enough".
		if (IsTheWard(v)) then
			if (QuestStatus("todin_ward") ~= XQuest.KNOWN) then
				AddMessage("'That is a fine thing, but it is not mine to "
					.. "ask for.'")
				return false
			end

			QuestModify("todin_ward", XQuest.CLOSED)

			if (TODIN_WARD_HONOURED) then
				AddMessage("'His hand. After all this while.' He turns it "
					.. "over and reads what is cut into it. " .. WARD_RUNES
					.. " 'Given. Well. You left the fellow something to "
					.. "stand behind, and that is the whole of the thing.'")
				AddMessage("'Sit down. The forge is hot and the runes are "
					.. "only sleeping.'")
				WakeTheWard(v)
				AddMessage("You could swing it now.")
			else
				AddMessage("'His hand. After all this while.' He turns it "
					.. "over and reads what is cut into it. " .. WARD_RUNES
					.. " He sets it down and does not look at you. 'Freely. "
					.. "Take your money.'")
			end

			if (MoneyOperation(t, -1200) >= 0) then
				MoneyOperation(p, 1200)
			end

			-- Answered true: he keeps the slab. It is what he asked for.
			return true
		end

		if (IsKind(kind, ItemKind.WEAPON)) then
			if (HasBrand(brt, "cold fire orc_slayer")) then
				AddMessage("'This weapon's good enough!'")
			else
				-- Asked before any money changes hands, so that walking away
				-- from the price costs nothing.
				local answer = AskQuestion(
					"'I need 450 gp to improve this weapon. What shall I forge into it?'",
					"esc c f o", "cold", "fire", "orc slaying")

				local brand = TODIN_BRANDS[answer]

				if (brand) then
					if (MoneyOperation(p, -450) >= 0) then
						MoneyOperation(t, 450)
						SetItemBrand(v, brand)
						AddMessage("'Thank you!'")
					else
						AddMessage("'But you haven't enough money!'")
					end
				else
					AddMessage("'Don't waste my time!'")
				end
			end
		else
			AddMessage("'Sorry, I don't need this.'")
		end
	elseif (e == LuaEvent.SAVE) then
		StoreInt(TODIN_WARD_SEEN and 1 or 0)
		StoreInt(TODIN_WARD_HONOURED and 1 or 0)
	elseif (e == LuaEvent.LOAD) then
		TODIN_WARD_SEEN = RestoreInt() ~= 0
		TODIN_WARD_HONOURED = RestoreInt() ~= 0
	end
	-- The give-item path ends here, and must answer false: Todin brands the
	-- weapon where it lies and the player walks off with it, while a true
	-- answer would mean he kept it (see XHero::GiveItem).
	return false
end
