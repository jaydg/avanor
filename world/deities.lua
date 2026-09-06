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

-- What these two make of a kill. Tiamat is the goddess of the living and
-- is pleased by the destruction of the undead; Marduk is pleased by a
-- death of any other kind. Killing to please one is how you displease the
-- other, which is the whole of the tension between them.
--
-- How much a kill is worth depends on how well the killer understands
-- what they are doing: a novice angers a god more than they please one,
-- and only real devotion makes the two balance.
local DEVOTION_GOOD = {5, 5, 5, 5, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10}
local DEVOTION_BAD = {-15, -14, -12, -10, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 0}

local function JudgeKill(killer, victim, pleased_by_undead)
	-- Lua tables count from one; the skill counts from zero.
	local devotion = GetSkill(killer, XSkill.RELIGION) + 1
	local good = DEVOTION_GOOD[devotion] or 0
	local bad = DEVOTION_BAD[devotion] or 0
	local undead = BinaryAND(GetCreatureClass(victim), CreatureClass.UNDEAD)

	if (undead == pleased_by_undead) then
		return good
	end

	return bad
end

function TiamatWatches(killer, victim)
	ChangeFavour(killer, "life", JudgeKill(killer, victim, true))
end

function MardukWatches(killer, victim)
	ChangeFavour(killer, "death", JudgeKill(killer, victim, false))
end
