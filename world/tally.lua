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


-- The orc war-party tally behind the two "repulse an attack of orcs" lines
-- on the achievements screen (XHero::EndGame, which still reads these two
-- flags from C++).
--
-- orcs_killed counts only what the hero struck down personally, and is
-- what separates having taken part from having watched. total_orcs_killed
-- counts every orc that dies anywhere, by any hand - the royal guards do
-- most of the work if the hero is slow about it.
local function TallyOrcs(cr, killer, class)
	if (class ~= CreatureClass.ORC) then
		return
	end

	if (killer and isHero(killer)) then
		QuestState:SetFlag('orcs_killed', QuestState:GetFlag('orcs_killed') + 1)
	end

	QuestState:SetFlag('total_orcs_killed', QuestState:GetFlag('total_orcs_killed') + 1)
end


function OnCreatureDie(cr, killer, class)
	TallyOrcs(cr, killer, class)
end
