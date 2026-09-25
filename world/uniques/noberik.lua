-- Noberik, the armourer of the small town, and the errand he hands out.
--
-- The valley's first two errands - the Elder's demon and Roderick's crypt -
-- both want a hero who has already been in a fight, and a new character has
-- nowhere to learn that. The rat cellar under Noberik's shop is exactly the
-- right size for it, and until now nothing pointed anyone at it: the stair
-- was simply there, in the corner of a shop, with no reason to take it.
--
-- So Noberik asks, the moment somebody capable-looking walks in.
--
-- BuildShop() was given this script's name as its `handler`, which hands it
-- to both halves of a shop: to the shop itself, which is a place and so
-- says MOVE_IN when someone walks in - the same event EventPlace() sends -
-- and to the keeper, who is a creature and so says CHAT when spoken to.
-- The two arrive with different arguments, as they do everywhere else, and
-- nothing here needs either creature: the errand is about the cellar.


-- What counts as cleared. The cellar's rats are settled by a generator that
-- never stops making more (Settle in world/locations/rat_cellar.lua), so
-- "no creatures left" is a promise the level cannot keep. The zombies are
-- finite, they are what makes the place dangerous, and they are what the
-- errand is really about.
local function CellarIsCleared()
	return GetCreatureCount("RATCELLAR", "undead") == 0
end

-- Said once on the way in, and it is the whole point of the errand: it
-- names the cellar, says where the way down is, and promises the loot.
local function OfferTheErrand()
	AddMessage("'Finally! You look like you could give me a hand! Clear out "
		.. "whatever's nesting in my cellar - rodents, restless dead, or "
		.. "anything in between, and you may keep everything you find.'")
	QuestModify("noberik_cellar", XQuest.KNOWN)
end

-- WHERE TO GET A SWORD THAT KILLS ORCS
--
-- Ozorick wants a blade branded for orc-slaying (world/uniques/ozorik.lua)
-- and there is exactly one place in the world that will make one: Todin,
-- under the mountain, who forges a brand into a weapon for 450 gp
-- (world/uniques/todin.lua). An armourer is the one man in the valley who
--  would know of another smith, and would know what the road to him is like.
--
-- Said while Ozorick's errand is open and not yet done, so that it arrives
-- when it is worth something rather than as scenery.
local function MentionTodin()
	AddMessage("'Orcs, is it? Then you want better steel than mine. Todin "
		.. "keeps a forge under the mountain, east of the village - he'll "
		.. "work a blade so that it bites orcs like kindling, and he asks "
		.. "about four hundred and fifty gold for it.'")
	AddMessage("'Mind the way down, though. Six floors of old dwarven "
		.. "delving, and it has not been empty in years.'")
end

-- Whether that is worth saying: he has the captain's errand and has not
-- yet finished it.
local function OzorikNeedsASword()
	return QuestStatus("ozorik") == XQuest.KNOWN
end


-- Paid in the only coin he promised: whatever came up the stair.
local function PayOff()
	AddMessage("'You cleared them out! The place is mine again - and "
		.. "what you carried up is yours, as I said.'")
	QuestModify("noberik_cellar", XQuest.CLOSED)
end

function NoberikHandler(e, a, b)
	-- Who this is about. MOVE_IN names the creature that walked in - the
	-- shop is a place, and a place tells its script about anyone at all -
	-- while CHAT names the keeper first and then whoever is speaking to
	-- him. Either way only the hero is worth answering: Noberik wanders out
	-- of his own shop and back in all day (he is a RANDOM_MOVE creature),
	-- and every one of those steps used to hand the errand out again, to a
	-- player who might be standing in the village at the time.
	local visitor = (e == LuaEvent.MOVE_IN) and a or b

	if (not isHero(visitor)) then
		return false
	end

	local status = QuestStatus("noberik_cellar")

	if (e == LuaEvent.MOVE_IN) then
		if (status == XQuest.UNKNOWN) then
			OfferTheErrand()
		elseif (status == XQuest.COMPLETE) then
			PayOff()
		end

		-- His own errand first - it is why he calls out at all - and the
		-- smith after it, once, so that walking past his door every day is
		-- not a lecture.
		if (OzorikNeedsASword() and QuestState:GetFlag('noberik_told_of_todin') ~= 1) then
			QuestState:SetFlag('noberik_told_of_todin', 1)
			MentionTodin()
		end

		return true
	end

	if (e == LuaEvent.CHAT) then
		if (status == XQuest.UNKNOWN) then
			OfferTheErrand()
		elseif (status == XQuest.KNOWN) then
			if (CellarIsCleared()) then
				-- Cleared while nobody was counting: the tally only
				-- notices a death it is told about, and a cellar emptied
				-- before the errand was taken is emptied all the same.
				QuestModify("noberik_cellar", XQuest.COMPLETE)
				PayOff()
			else
				AddMessage("'They're still down there. The stairs are in "
					.. "the shed on the opposite side of the square.'")
			end
		elseif (status == XQuest.COMPLETE) then
			PayOff()
		elseif (not OzorikNeedsASword()) then
			AddMessage("'Good hunting to you.'")
		end

		-- Asked for rather than volunteered, and so repeated as often as it
		-- is wanted: a player who has forgotten the name has nowhere else
		-- to get it.
		if (OzorikNeedsASword()) then
			QuestState:SetFlag('noberik_told_of_todin', 1)
			MentionTodin()
		end

		return true
	end

	return false
end
