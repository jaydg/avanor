-- The gods, and what standing with one is worth.
--
-- Which gods exist is content. The engine knows only that a creature
-- stands in some relation to each of them, that the relation can be spent
-- on favours, and that something happens when a creature kills another -
-- it has no opinion at all about who deserves killing. That opinion lives
-- in the :OnKill() handlers below.
--
--   Deity.new(id)
--       :Called(name)          what worshippers call it
--       :OnKill(handler)       a Lua function called on every kill, with
--                              the killer and the victim, so the god can
--                              decide whether it approves
--       :Grants(name, rank, cost, effect [, alternative])
--                              one thing it will do for a follower who
--                              has reached that rank, and what it costs
--                              them in favour. Given an alternative, one
--                              of the two is picked afresh at each prayer
--       :Register()

-- The ladder of standing, shared by every god: how much favour amounts to
-- how much regard, what that regard is called, and what it is worth on
-- the final score. Which grants a rank opens is not said here - each
-- grant names the rank it needs, so grants may be reordered or added
-- without disturbing the ladder.
--
--   DeityRank.new(id)   the name a god's :Grants() rows point at
--       :Called(name)   what it reads as on screen
--       :From(favour)   the least favour that still counts as this rank
--       :Score(points)  what standing here with one god adds to the final
--                       score. A god is mentioned there at all only if
--                       the standing has opened at least one of its
--                       grants
--       :Register()

DeityRank.new("fallen_champion")
	:Called("<QUALITY_TERRIBLE>fallen champion")
	:From(-2147483648)
	:Register()

DeityRank.new("very_bad")
	:Called("<QUALITY_TERRIBLE>very bad")
	:From(-10000000)
	:Register()

DeityRank.new("bad")
	:Called("<QUALITY_TERRIBLE>bad")
	:From(-100)
	:Register()

DeityRank.new("normal")
	:Called("<QUALITY_NEUTRAL>normal")
	:From(0)
	:Register()

DeityRank.new("adept")
	:Called("<QUALITY_NEUTRAL>adept")
	:From(100)
	:Score(1200)
	:Register()

DeityRank.new("follower")
	:Called("<QUALITY_FAIR>follower")
	:From(1000)
	:Score(1500)
	:Register()

DeityRank.new("messiah")
	:Called("<QUALITY_GOOD>messiah")
	:From(3000)
	:Score(1800)
	:Register()

DeityRank.new("champion")
	:Called("<QUALITY_PERFECT>champion")
	:From(10000)
	:Score(2100)
	:Register()

Deity.new("life")
	:Called("Tiamat")
	:OnKill("TiamatWatches")
	:Grants("cure light wounds", "adept", 3, "cure_light_wounds")
	:Grants("minor divine intervention", "adept", 5, "magic_arrow")
	:Grants("cure poison", "follower", 10, "cure_poison")
	:Grants("heroism", "follower", 10, "heroism")
	:Grants("cure critical wounds", "messiah", 20, "cure_critical_wounds")
	:Grants("great knowledge", "messiah", 30, "identify")
	:Grants("divine restoration", "champion", 50, "restoration")
	:Register()

Deity.new("death")
	:Called("Marduk")
	:OnKill("MardukWatches")
	:Grants("cure light wounds", "adept", 5, "cure_light_wounds")
	:Grants("minor divine intervention", "adept", 5, "magic_arrow")
	:Grants("divine intervention", "follower", 5, "fire_bolt", "ice_bolt")
	:Grants("divine escape", "follower", 50, "teleport")
	:Grants("cure critical wounds", "messiah", 30, "cure_critical_wounds")
	:Grants("knowledge of insight", "messiah", 50, "self_knowledge")
	:Grants("major divine intervention", "champion", 5, "lightning_bolt", "acid_bolt")
	:Register()

-- What these two make of a kill.
--
-- Tiamat is the goddess of the living, and she does not begrudge the
-- living their quarrels: something that came at you and died fighting is
-- a death she accepts, and asks nothing for either way. What offends her
-- is a kill that was never a fight - one that had turned and run, or one
-- that would never have raised a hand against you. The undead are an
-- affront to life itself, and putting one down pleases her.
--
-- Marduk is the god of death, and of the cruelty in it. An honest fight
-- bores him; what he pays for is the blow struck at something already
-- running, and he pays double for it.
--
-- An ordinary kill credits neither of them, which is the whole of what
-- holds the two level. While killing anything at all was a devotional
-- act, the god pleased by ordinary killing was always going to run away
-- with it: nine tenths of everything that dies is a living thing that
-- fought back, so he collected on nine kills in ten and she was in debt
-- from her first.
--
-- How much a kill is worth still depends on how well the killer
-- understands what they are doing: a novice angers a god more than they
-- please one, and only real devotion makes the two balance.
local DEVOTION_GOOD = {5, 5, 5, 5, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10}
local DEVOTION_BAD = {-15, -14, -12, -10, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 0}

-- What Marduk pays for a cruel kill, as a multiple of what a kill is
-- otherwise worth. Two, because that is what makes the books balance:
-- measured over a world left to itself, a fleeing creature dies about
-- three times in a hundred and an undead about eight, so paying him twice
-- for the rarer act brings his takings and hers out within a tenth of
-- each other.
local CRUELTY_REWARD = 2

-- A kill that was not a fight: something running, or something that would
-- never have fought. Both are asked of the victim, not of the killer -
-- being attacked makes anything regard its attacker as an enemy, so what
-- the victim thought of the killer says nothing about who started it.
local function WasHelpless(victim)
	return IsFleeing(victim) or HasAIFlag(victim, XStandardAI.PEACEFUL)
end

local function Devotion(killer, table_)
	-- Lua tables count from one; the skill counts from zero.
	return table_[GetSkill(killer, XSkill.RELIGION) + 1] or 0
end

function TiamatWatches(killer, victim)
	if (WasHelpless(victim)) then
		ChangeFavour(killer, "life", Devotion(killer, DEVOTION_BAD))
	elseif (GetCreatureClass(victim) == "undead") then
		ChangeFavour(killer, "life", Devotion(killer, DEVOTION_GOOD))
	end
end

function MardukWatches(killer, victim)
	if (WasHelpless(victim)) then
		ChangeFavour(killer, "death",
			Devotion(killer, DEVOTION_GOOD) * CRUELTY_REWARD)
	end
end
