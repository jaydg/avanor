-- Beira, the dwarven cleric
--
-- She walks to the wounded and lays hands on them, then goes back to her
-- altar when there is nobody left to see to. Both halves are the engine's
-- doing rather than hers, which is why this file is as short as it is:
--
--   * Attending somebody is being their companion. XStandardAI::Move()
--     walks a creature to whoever it is escorting, and - this is the part
--     that matters - leaves a creature alone about its guard area while it
--     does. Her post does not pull her back mid-errand.
--   * The curing is XStandardAI::CastSpell(), which picks a caster's
--     HEALING spells for the worst hurt of its own standing beside it.
--   * Letting the patient go is all that "going home" takes: with nobody
--     to escort, the same GUARD_AREA machinery that keeps every other
--     sentry at its post walks her back to the altar.
--
-- So the only thing here that is hers alone is deciding who needs her.

-- Who counts as hers: the dwarves of the city, by the group the whole
-- garrison shares.
local CLERIC_GROUP = "dwarven_guardian"

-- Below this share of their strength, somebody is worth walking to. A
-- third, the same figure the engine uses when a creature decides to treat
-- its own wounds and when it decides to treat a neighbour's
-- (XStandardAI::CastSpell, XStandardAI::WoundedAlly), so she goes to
-- exactly those she will then be able to help.
local HURT = 3

-- And this far gone, badly enough to be worth heroism as well: a fifth of
-- their strength, which is somebody about to fall over.
local DIRE = 5


Monster.new("dwarf_cleric", "dwarf")
	:View("dwarven cleric", 'h', xColor.xWHITE, PersonType.NAMED_SHE, CreatureTemplate.LOW, "humanoid")
	:Never("invisible")
	:Always("see_invisible")
	-- Peaceful, like the rest of them. Not COWARD, though.
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM + XStandardAI.PEACEFUL)
	-- A little above the dwarves she tends (To 1d3+12, Wi 1d3, Ma 1d2):
	-- harder to kill, and enough mana to be worth calling a caster.
	:Stats("St 1d3+10 Dx 1d3+7 To 1d3+16 Le 2d4 Wi 2d4+4 Ma 2d5+4 Pe 1d6 Ch 2d3")
	-- Hardier than a townsman (2d3+1) and with the mana a caster needs
	-- (2d2); the swing and the rest of the build stay theirs.
	:Main("1d1", "0d0", "3d3+4", "3d3+6")
	:Description("A stout dwarven woman in the white of the life-givers, "
		.. "her beard braided with silver rings. She does not look up as "
		.. "you pass: her business is with the wounded they carry in from "
		.. "the stair, and there is always another.")
	:LearnSkill(XSkill.HEALING, 10)
	:LearnSpell("cure_serious_wounds")
	:LearnSpell("heroism")
	:Equip(ItemKind.WEAPON, "great_axe", 100)
	:Equip(ItemKind.SHIELD, "large_shield", 100)
	:Unique()
	:Register()


function CreateDwarfCleric(x, y)
	-- A small patch around her altar. It is what brings her back.
	local cleric = Guardian("dwarf_cleric", CLERIC_GROUP, x - 2, y - 1, 5, 3)

	if (cleric) then
		SetEventHandler(cleric, 'DwarfClericHandler')

		-- Without this her handler is never called at all: AI_TURN fires
		-- only for a creature that asked for it (XCreature::NewMove).
		EnableMoveHandler(cleric)
	end
end


-- Somebody still worth her time: alive, and under the share of their
-- strength the engine itself treats as hurt.
local function NeedsHer(cr)
	local c = AsCreature(cr)

	return c.hp > 0 and c.hp * HURT < c.max_hp
end


-- The worst hurt of her people on this level, or nil when nobody needs
-- her. Herself excepted: the engine already has her treat her own wounds
-- first, and a healer who walks to herself would never arrive.
local function WorstWounded(cleric)
	local where = GetLocationId(GetCreatureLocation(cleric))

	if (not where) then
		return nil
	end

	local worst, worst_share = nil, 1.0

	for _, cr in ipairs(FindCreatures(where, CLERIC_GROUP)) do
		local c = AsCreature(cr)
		local share = c.hp / c.max_hp

		if (cr ~= cleric and NeedsHer(cr) and share < worst_share) then
			worst, worst_share = cr, share
		end
	end

	return worst
end


-- What she adds to the mending by hand, once she is within reach of it.
--
-- The curing itself is not here: XStandardAI::CastSpell() casts her
-- cure_serious_wounds at whoever is worst hurt beside her, and does it
-- properly, out of her own mana. Heroism is not a HEALING spell - it names
-- no SpellUse in world/spells.lua, so no AI ever reaches for it - which
-- leaves the heart to keep standing while the mending takes hold as the
-- one thing she has to give herself.
local function LayOnHands(t, patient)
	local cx, cy = GetCreatureXY(t)
	local px, py = GetCreatureXY(patient)
	local dx, dy = cx - px, cy - py

	-- Still on her way. Nothing to do but keep walking, which the AI does
	-- for her.
	if (dx < -1 or dx > 1 or dy < -1 or dy > 1) then
		return
	end

	local hurt = AsCreature(patient)

	if (hurt.hp * DIRE < hurt.max_hp) then
		MakeEffect("heroism", t, GetCreatureLocation(t), cx, cy, patient, px, py, 20)
	end
end


function DwarfClericHandler(e, t)
	if (e ~= LuaEvent.AI_TURN) then
		return false
	end

	local hurt = WorstWounded(t)
	local cleric = AsCreature(t)

	-- Nobody to see to. Letting her patient go is the whole of going home:
	-- XStandardAI::Move() leaves a creature alone about its guard area only
	-- while it is escorting somebody, so with the escort dropped the same
	-- GUARD_AREA machinery that keeps every other sentry at its post walks
	-- her back to the altar.
	if (not hurt) then
		cleric.xai:SetCompanion(nil)

		return false
	end

	-- The worst first, reconsidered every turn: a dwarf who takes a bad hit
	-- while she is crossing the hall to a lighter case is the one she
	-- should be going to instead. Her feet are the AI's - being somebody's
	-- companion is what makes it walk her to them.
	cleric.xai:SetCompanion(AsCreature(hurt))

	LayOnHands(t, hurt)

	-- The turn is not hers to end: she still has to walk, or cast, and
	-- XStandardAI::Move() runs after this either way (XCreature::NewMove).
	return false
end
