
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

Monster.new("brida", "goodwife")
	:View("Brida", 'p', xColor.xLIGHTCYAN, PersonType.NAMED_SHE, CreatureTemplate.UNIQUE, CreatureClass.HUMAN)
	:Description("Brida's eyes are red-rimmed and her hands won't sit still, wringing a scrap of cloth over and over as she paces the small room.  Her daughter Giana went out to the valley days ago and never came home - and every hour since has worn another line into her face.")
	:Unique()
	:Register()

-- Quest-hook NPC: gives the player the reason to ever go looking for
-- Rotmoth's cave in the first place, and closes the loop once Giana is
-- safely home. Doesn't gate anything mechanically (the cave and its
-- inhabitants already exist regardless of whether she's been spoken to -
-- same as every other hand-placed location in this game), purely the
-- narrative entry and exit point for the quest.
function CreateBrida(x, y)
	-- Her house is a single-row 5-tile room (see world/valley.lua's "B"
	-- placement) - x,y is the exact spawn tile, so center the guard rect
	-- on the room rather than just the one tile.
	local brida = Guardian("brida", "small_village_farmer", x - 2, y, 5, 1)
	SetEventHandler(brida, 'BridaHandler')
end

function BridaHandler(e, t, p, v)
	if (e ~= LuaEvent.CHAT) then
		return 0
	end

	local status = QuestState:GetFlag('rotmoth_status')

	if (status == 0) then
		AddMessage("'My Giana... she never came back from the valley!' Brida sobs.")
		AddMessage("'A bandit - Rotmoth, he calls himself - is holding her in a cave east of here, past where the bandits lurk near the bridge. He wants 150 gold coins for her, but I have nothing left to give him.'")
		AddMessage("'Please - if you have the coin, or the courage - bring my daughter home!'")
		QuestModify(QUEST_GIANA, XQuest.KNOWN)
	elseif (status == 1) then
		AddMessage("'Is she safe? Please, hurry her home to me!'")
	elseif (status == 2) then
		if (QuestState:GetFlag('brida_reward_given') ~= 1) then
			AddMessage("Brida pulls you into a fierce hug, tears streaming down her face.")
			AddMessage("'Thank you, thank you! I have little to offer, but... here, take these. I brewed them myself, from what I still remember of my own mother's craft.'")

			for i = 1, 3 do
				GiveObjectToCreature(CreateObject(PotionName.CURE_LIGHT_WOUNDS), p)
			end

			QuestState:SetFlag('brida_reward_given', 1)
			QuestModify(QUEST_GIANA, XQuest.CLOSED)
		else
			AddMessage("'Bless you, again and again, for bringing my Giana home.'")
		end
	end

	return 1
end

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

	-- Rotmoth's private quarters (bedroom, then treasure room) sit through
	-- the door directly south of the starting room - previously nothing
	-- stopped the hero from walking in and looting his chest before ever
	-- paying (or even hearing) the ransom demand. Covers the door itself
	-- plus everything behind it, so opening it (MOVE_IN) warns, and taking
	-- even one more step deeper rather than backing out (MOVE) turns him
	-- hostile.
	EventPlace(x + 1, y + 4, 13, 10, 'RotmothGuardEvent')
end

function RotmothGuardEvent(e, p)
	if (not isHero(p)) then
		return
	end

	-- p arrives as a raw void* light userdata (this is an XAnyPlace/
	-- event_handler-style dispatch, not an XLuaAI hook) - wrap it before
	-- passing it to any usertype method (isEnemy/ReactToAttacker below).
	-- Skipping this makes the call fail sol2's argument type check and get
	-- silently swallowed by the protected_function_result in
	-- XAnyPlace::onCreatureEnter/Move - no crash, no message, nothing.
	-- p arrives as a raw void* light userdata (this is an XAnyPlace/
	-- event_handler-style dispatch, not an XLuaAI hook) - wrap it before
	-- passing it to any usertype method (isEnemy/ReactToAttacker below).
	-- Skipping this doesn't throw - sol2 silently accepts it and the calls
	-- below become no-ops - so this failure mode is invisible unless you
	-- specifically check whether isEnemy() actually flips afterward.
	local chatter = AsCreature(p)
	local rotmoth = AsCreature(FindCreature(L_SMALL_CAVE2, "rotmoth"))

	-- Nothing to guard with once he's dead, and no point re-warning someone
	-- he's already fighting (isEnemy() is what ReactToAttacker below puts
	-- them on, via the personal-enemy fallback in isEnemy()).
	if (not rotmoth or rotmoth.xai:isEnemy(chatter)) then
		return
	end

	if (e == LuaEvent.MOVE_IN) then
		AddMessage("'Stay out of my quarters!' Rotmoth shouts from the next room.")
	elseif (e == LuaEvent.MOVE) then
		AddMessage("Rotmoth comes at you, furious!")
		rotmoth.xai:ReactToAttacker(chatter)
	end
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

-- If Rotmoth dies before ransom was ever paid, Giana is still his captive
-- (rotmoth_status == 0) - killing her captor is at least as good as paying
-- him, so free her the same way RotmothHandler's ransom branch does rather
-- than leaving her stuck waiting for a payment that can no longer happen.
function RotmothAI.onDie(self, killer)
	if (QuestState:GetFlag('rotmoth_status') == 0 and killer and killer:isHero()) then
		local girl = QuestState:GetCreatureRef('kidnapped_girl')

		if (girl) then
			AddMessage("'Thank you for saving me!' Giana cries out.")
			girl.xai:SetCompanion(killer)
			QuestState:SetFlag('rotmoth_status', 1)
		end
	end
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

	AddMessage("Bring me 150 gold coins, or the girl dies.")

	if (chatter:MoneyOp(0) >= 150) then
		if (AskQuestion("Pay him?", "y n", "yes", "no") == 'y') then
			chatter:MoneyOp(-150)
			rotmoth:MoneyOp(150)

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
			local giana = AsCreature(t)
			giana.xai:SetCompanion(nil)

			-- Re-home her to the exact same area the farmers themselves patrol
			-- (VILLAGE_GUARD_AREA, set in world/valley.lua where they're
			-- spawned) rather than just clearing GUARD_AREA outright, as
			-- GUARD_AREA is ALSO the only thing suppressing a second, unrelated
			-- behaviour: she inherits goodwife's HUMAN flag set, which includes
			-- FREE_WAY (ALLOW_MOVE_WAY_UP|DOWN).
			giana.xai:SetGuardArea(VILLAGE_GUARD_AREA.x, VILLAGE_GUARD_AREA.y,
				VILLAGE_GUARD_AREA.w, VILLAGE_GUARD_AREA.h, XLocation.MAIN)

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
		if (QuestState:GetFlag('rotmoth_status') == 0) then
			AddMessage("Halt! Don't move anymore or I'll kill her!")
			AddMessage("Back off now, and bring me 150 golden coins - do that, and I might just show her mercy!")

			if (MoneyOperation(p, 0) >= 150) then
				if (AskQuestion("Pay him right now?", "esc y n", "yes", "no") == 'y') then
					MoneyOperation(p, -150)
				end
			end
		else

		end

	elseif (e == LuaEvent.MOVE_OUT and isHero(p)) then
		if (QuestState:GetFlag('rotmoth_status') == 0) then
			AddMessage("Hurry! 150 golden coins!")
		end
	end
end
