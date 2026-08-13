
Monster.new("rotmoth", "bandit")
	:View("Rotmoth", 'p', xColor.xDARKGRAY, PersonType.NAMED_HE, CreatureTemplate.UNIQUE, CreatureClass.HUMAN)
	:Description("A wiry, hollow-eyed man with a bandit's cunning and a coward's cruelty, Rotmoth long ago traded whatever honor he once had for easier profits.  He holds his captive in this cave not through any real strength of his own, but because he knows precisely what a frightened hostage is worth in gold - and precisely who to threaten if you come at him with steel drawn instead of coin.")
	:Unique()
	:Register()

Monster.new("giana", "goodwife")
	:View("Giana", 'p', xColor.xLIGHTRED, PersonType.NAMED_SHE, CreatureTemplate.UNIQUE, CreatureClass.HUMAN)
	:Description("Giana's dress is torn at the hem and her eyes dart toward the cave entrance at every sound, but there's a stubborn set to her jaw that captivity hasn't managed to break.  Snatched from the valley for ransom, she has spent her time here watching for any chance at rescue - and once freed, she intends to walk every step of the way home herself.")
	:Unique()
	:Register()

function SmallCaveQuestPersons(x, y)
	local giana = Guardian("giana", "giana", x + 1, y, 8, 4)
	SetEventHandler(giana, 'GianaHandler')
	-- Rotmoth's kidnap victim
	QuestState:SetCreatureRef('kidnapped_girl', AsCreature(giana))

	-- Cheap per-turn check (see GianaHandler's AI_TURN case) for whether
	-- she's made it home yet, once ransomed - only does real work (the
	-- FindCreature map scan) once rotmoth_status reaches 1, and turns
	-- itself back off via DisableMoveHandler() the moment she arrives, so
	-- this isn't paid for the rest of the game.
	EnableMoveHandler(giana)

	local rotmoth = Guardian("rotmoth", "rotmoth", x + 1, y, 8, 4)
	SetEventHandler(rotmoth, 'RotmothHandler')
	SetCreatureAI(rotmoth, 'RotmothAI')

	-- Giana is PEACEFUL, and PEACEFUL creatures proactively defend
	-- themselves against anything non-PEACEFUL they can see (isEnemy(),
	-- creature/std_ai.cpp) - Rotmoth is right there in the same small
	-- room from the moment this location is built, so without this she
	-- attacks her own captor before the hero ever arrives (that
	-- self-defense clause is suppressed while a companion is set, see
	-- the escort-gate fix). Marking Rotmoth as her "companion" here isn't
	-- really about following him - it reuses that same suppression to
	-- mean "captive, not fighting anyone right now". RotmothHandler's
	-- ransom payment already overwrites this with SetCompanion(chatter),
	-- which is the correct transition from captive to escorted anyway.
	AsCreature(giana).xai:SetCompanion(AsCreature(rotmoth))

	EventPlace(x, y, 5, 2, 'SmallCaveEvent')
end

-- Redirects retaliation for an attack onto the kidnapped girl
-- instead of Rotmoth himself, as long as she's still around.
RotmothAI = {}
function RotmothAI.onWasAttacked(self, attacker)
	if attacker:isHero() then
		local girl = QuestState:GetCreatureRef('kidnapped_girl')

		if girl then
			self.xai:ReactToAttacker(girl)
			self.xai:AddPersonalEnemy(attacker)
			return true
		end
	end

	return false
end

function RotmothHandler(e, t, p, v)
	if (e ~= LuaEvent.CHAT) then
		return 0
	end

	local rotmoth = AsCreature(t)
	local chatter = AsCreature(p)

	if (rotmoth.xai:isEnemy(chatter)) then
		AddMessage("You will be rewarded for your stupidness!")
		return 1
	end

	if (QuestState:GetFlag('rotmoth_status') ~= 0) then
		AddMessage("Run away quickly before I change my mind!")
		return 1
	end

	local girl = QuestState:GetCreatureRef('kidnapped_girl')

	if (not girl or not rotmoth:IsCreatureVisible(girl)) then
		AddMessage("I dont know what you are asking about.")
		return 1
	end

	AddMessage("I hope you'll bring 100 gold coins, otherwise this girl will die.")

	if (chatter:MoneyOp(0) >= 100) then
		if (AskQuestion("Pay him?", "y n", "yes", "no") == 'y') then
			chatter:MoneyOp(-100)
			rotmoth:MoneyOp(100)

			if (chatter:IsMale()) then
				AddMessage("Thank you, boy!")
			else
				AddMessage("Thank you, girl!")
			end

			girl.xai:SetCompanion(chatter)
			QuestState:SetFlag('rotmoth_status', 1)
		end
	end

	return 1
end

function GianaHandler(e, t, p, v)
	if (e == LuaEvent.AI_TURN) then
		-- Once ransomed (rotmoth_status == 1, see RotmothHandler) she's
		-- following whoever paid it - stop here and settle in the valley
		-- the moment she's made it out of the cave and back home, rather
		-- than trailing the hero around forever.
		--
		-- FindCreature() returns a raw void* (nullptr when nothing
		-- matches) - unlike a typed pointer return (e.g. AsCreature's own
		-- XCreature*), sol2 pushes that as a light userdata that Lua
		-- treats as truthy even when it wraps a null pointer, so testing
		-- it directly here was always true the instant rotmoth_status hit
		-- 1, regardless of whether she'd actually reached the village -
		-- wrap it in AsCreature() so a real miss becomes proper Lua nil.
		if (QuestState:GetFlag('rotmoth_status') == 1 and AsCreature(FindCreature(XLocation.MAIN, "giana"))) then
			AsCreature(t).xai:SetCompanion(nil)
			QuestState:SetFlag('rotmoth_status', 2)
			DisableMoveHandler(t)
		end

		return 0
	end

	if (e ~= LuaEvent.CHAT) then
		return 0
	end

	local giana = AsCreature(t)
	local chatter = AsCreature(p)

	if (giana.xai:isEnemy(chatter)) then
		AddMessage("Don't touch me!")
	elseif (QuestState:GetFlag('rotmoth_status') < 2) then
		AddMessage("Please, save me.")
	else
		AddMessage("Thank you again for saving me. I'm happy to be back home.")
	end

	return 1
end

small_cave_first_visit = 0

function SmallCaveEvent(e, p)

	if (not isHero(p)) then
		return
	end

	local bandit = FindCreature(L_SMALL_CAVE2, "rotmoth")
	local giana = FindCreature(L_SMALL_CAVE2, "giana")

	if (e == LuaEvent.MOVE and isHero(p)) then
--[[		local c = FindCreature(XLocation.DWARFCITY, "dwarven_guardian", 75, 1, 4, 2)
		if (not isEnemy(c, p)) then
			DvCityEventMoveCount = DvCityEventMoveCount + 1
			if (DvCityEventMoveCount < 3) then
				AddMessage("'Leave here immediately!'")
			elseif (DvCityEventMoveCount == 3) then
				AddMessage("'This is your last chance to leave!'")
			elseif (DvCityEventMoveCount > 3) then
				SetItEnemyFor(p, c)
			end
		end
]]--
	elseif (e == LuaEvent.MOVE_IN and isHero(p)) then
		if (small_cave_first_visit == 0) then
			AddMessage("Halt! Don't move anymore or I'll kill her!")
			AddMessage("Bring me 150 golden coins, run away quikly and I probably give her a mercy!")
			small_cave_first_visit = 1
			if (MoneyOperation(p, 0) >= 150) then
				if (AskQuestion("Pay him right now?", "esc y n", "yes", "no") == 'y') then
					MoneyOperation(p, -150)
				end
			end
		else

		end

	elseif (e == LuaEvent.MOVE_OUT and isHero(p)) then
		if (small_cave_first_visit == 1) then
			AddMessage("Remember! 150 golden coins!")
		else
		end
	elseif (e == LuaEvent.SAVE) then
		StoreInt(small_cave_first_visit)
	elseif (e == LuaEvent.LOAD) then
		small_cave_first_visit = RestoreInt()
	end
end
