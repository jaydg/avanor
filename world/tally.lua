-- World-level bookkeeping: things the game counts that belong to no single
-- actor, place or quest.
--
-- OnCreatureDie() is called by the engine (XCreature::Die) for every
-- creature that dies anywhere in the world, after that creature's own
-- event handler has had its LuaEvent.DIE. It is optional - an engine with
-- no world/ script defining it simply counts nothing - and it is called
-- protected, so a mistake in here costs a message on stderr rather than
-- the running game.
--
--   cr      the creature that died
--   killer  whoever killed it, or nil (starvation, poison, a collapsing
--           dungeon - see XCreature::DecNutrio)
--   class   the dead creature's CreatureClass


-- The orc war-party tally behind the "repulse an attack of orcs" line on the
-- achievements screen.
--
-- Only the war party is counted. An orc that dies anywhere else has nothing
-- to do with the attack on the town, so asking the dead orc's group - rather
-- than trusting that the only orcs alive are theirs - is what keeps this
-- honest the day orcs are put somewhere else in the world. Without it, an
-- orc dying in some future cave would be read as the last of the war party
-- the moment the valley happened to hold exactly one, and the hero could
-- earn the credit for a battle never fought.
--
-- war_party_killed is the hero's own share of them, and is what separates
-- having taken part from having watched - the royal guards do most of the
-- work if the hero is slow about it.
local function TallyOrcs(cr, killer, class)
	if (class ~= CreatureClass.ORC or GetGroupID(cr) ~= ORC_WAR_PARTY) then
		return
	end

	if (killer and isHero(killer)) then
		QuestState:SetFlag('war_party_killed', QuestState:GetFlag('war_party_killed') + 1)
	end

	-- The war party is the only reason there are orcs in the valley, and
	-- nothing wanders up into it (SetWanderingAllowed), so once MAIN holds
	-- no more of them the attack has been beaten.
	--
	-- One, not none: the orc dying right now is still standing on its map
	-- cell. XCreature::Die drops the cell's reference in LastStep(), which
	-- runs long after this hook.
	if (GetCreatureCount("MAIN", CreatureClass.ORC) ~= 1) then
		return
	end

	-- Somebody has to have taken part to have helped. If the royal guards
	-- did all of it while the hero was elsewhere, there is nothing to claim.
	if (QuestState:GetFlag('war_party_killed') == 0) then
		return
	end

	-- Added once, and only when earned: a quest that exists solely to put a
	-- line on the achievements screen, so it is born CLOSED with nothing to
	-- say in any other state. QuestStatus() answers UNKNOWN for a record
	-- that was never added, which is what keeps this from firing twice.
	if (QuestStatus(QUEST_ORC_WAR_PARTY) ~= XQuest.UNKNOWN) then
		return
	end

	Quest(QUEST_ORC_WAR_PARTY, XQuest.CLOSED,
		"",
		"",
		string.format(
			"You helped to repulse an attack of orcs - all %d of the war party that came down on the town lie dead.",
			ORC_WAR_PARTY_SIZE),
		0)
end


function OnCreatureDie(cr, killer, class)
	TallyOrcs(cr, killer, class)
end
