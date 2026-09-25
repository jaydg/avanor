------------------------------ GOBLINS --------------------------------------
--
-- "Goblins normally prefer secrecy and stealing to open confrontation."
-- That line has been in the goblin's description since the beginning and
-- nothing ever acted on it. Now they do: every goblin carries
-- EXPLORER_MOVE, so it works its way through a level rather than milling
-- about, and CREATURE already gives it the hands to pick up what it finds.
--
-- A goblin met alone keeps what it knows to itself - a creature with no
-- group has its own record of where it has been (XStandardAI::
-- ExplorerKey). A camp shares one, so the pack sweeps the level between
-- them instead of each re-walking the same passages.

Monster.new("goblin")
	:View("goblin", 'g', xColor.xLIGHTGREEN, PersonType.HE, CreatureTemplate.VERY_LOW, "goblin")
	:Basic("1d10+90", "0d0+1000", "0d0+1000", CreatureSize.SMALL, "1d200+800")
	:Body("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 3)
	:AI(XStandardAI.CREATURE + XStandardAI.EXPLORER_MOVE)
	:Stats("St 3d3 Dx 2d3 To 2d3 Le 1d4 Wi 1d4 Ma 1d4 Pe 3d3 Ch 1d5")
	:Combat("1d3", "1d3")
	:Main("1d3", "1d1", "1d5+6", "1d5")
	:Description("This scruffy looking humanoid glances at you with fear in "
		.. "its face. The stench it carries indicates that it has probably "
		.. "never been clean in its life. Goblins normally prefer secrecy "
		.. "and stealing to open confrontation but have been known to "
		.. "attack travellers.")
	:LearnSkill(XSkill.HEALING, 4)
	:LearnSkill(XSkill.FINDWEAKNESS, 4)
	:EquipCount(ItemKind.SCROLL + ItemKind.POTION, 1, 10)
	:Equip(ItemKind.WEAPON, "short_sword", 100)
	:Register()

Monster.new("goblin_warrior")
	:View("goblin warrior", 'g', xColor.xGREEN, PersonType.HE, CreatureTemplate.LOW, "goblin")
	:Basic("1d10+95", "0d0+1000", "0d0+1000", CreatureSize.SMALL, "1d200+800")
	:Body("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 5)
	:AI(XStandardAI.CREATURE + XStandardAI.EXPLORER_MOVE)
	:Stats("St 3d4 Dx 3d3 To 3d3 Le 1d4 Wi 1d4 Ma 1d4 Pe 3d3 Ch 2d3")
	:Combat("1d4", "1d3")
	:Main("1d4", "1d1", "1d5+10", "1d5")
	:Description("This goblin carries a nasty looking knife, almost a sword "
		.. "for one as short as he is. His posture and multiple scars "
		.. "indicate that he is more than a novice at handling them as "
		.. "well.")
	:LearnSkill(XSkill.HEALING, 5)
	:LearnSkill(XSkill.FINDWEAKNESS, 5)
	:EquipCount(ItemKind.SCROLL + ItemKind.POTION, 2, 10)
	:Equip(ItemKind.WEAPON, "short_sword", 100)
	:Register()

Monster.new("goblin_warmaster")
	:View("goblin warmaster", 'g', xColor.xBLUE, PersonType.HE, CreatureTemplate.LOW, "goblin")
	:Basic("1d10+100", "0d0+1000", "0d0+1000", CreatureSize.SMALL, "1d200+800")
	:Body("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 7)
	:AI(XStandardAI.CREATURE + XStandardAI.EXPLORER_MOVE)
	:Stats("St 5d4 Dx 6d3 To 5d5 Le 2d4 Wi 2d4 Ma 2d4 Pe 5d5 Ch 3d3")
	:Combat("1d8", "2d3")
	:Main("1d8", "1d3", "2d5+20", "2d5")
	:Description("Arms criss-crossed with scars and bulging with muscle, "
		.. "this goblin has a hint of white hair. Not many goblins can live "
		.. "so long but the worn weapons at his side indicate that he is "
		.. "capable of keeping his place in the earth.")
	:LearnSkill(XSkill.HEALING, 6)
	:LearnSkill(XSkill.FINDWEAKNESS, 6)
	:EquipCount(ItemKind.SCROLL + ItemKind.POTION, 3, 10)
	:Equip(ItemKind.WEAPON, "short_sword", 100)
	:Register()

Monster.new("goblin_chieftain")
	:View("goblin chieftain", 'g', xColor.xRED, PersonType.HE, CreatureTemplate.AVG, "goblin")
	:Basic("1d10+100", "0d0+1000", "0d0+1000", CreatureSize.SMALL, "1d200+800")
	:Body("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 10)
	:AI(XStandardAI.CREATURE + XStandardAI.EXPLORER_MOVE)
	:Stats("St 5d4 Dx 6d3 To 5d5 Le 2d4 Wi 2d4 Ma 2d4 Pe 5d5 Ch 3d3")
	:Combat("1d8", "2d4")
	:Main("1d10", "2d2", "2d5+25", "2d5")
	:Description("Stronger and smarter than the other goblins. This goblin "
		.. "has become the chief and now commands all the troops. His shock "
		.. "of white hair is the only part of him that shows his age and "
		.. "his muscular body is toned from battles past.")
	:LearnSkill(XSkill.HEALING, 6)
	:LearnSkill(XSkill.FINDWEAKNESS, 6)
	:EquipCount(ItemKind.SCROLL + ItemKind.POTION, 4, 10)
	:Equip(ItemKind.WEAPON, "short_sword", 100)
	:Register()


-- THE GOBLIN CAMP ------------------------------------------------------
--
-- A pack with somewhere to put what it takes.
--
-- The chieftain sits on the hoard and does not leave it; the rest range
-- over the level, and when one is carrying enough it comes back and drops
-- the lot at his feet. None of this is in the engine. The engine offers
-- three things and content decides what to make of them: explore
-- (EXPLORER_MOVE), pick up (ALLOW_PICK_UP), and go to somebody
-- (SetCompanion). The thieving is what this file adds.
--
-- One thing worth knowing about the chieftain: he keeps a guard area over
-- the hoard, but the engine skips that pull on a turn he attacked, so a
-- fight can draw him off it for a while. Loot handed over while he is
-- away lands outside the hoard, where it is ordinary floor loot again and
-- gets picked up and brought back. Untidy, and it settles.

GOBLIN_PACK = "goblin_pack"

-- How much one of them will carry before taking it home. Two, which is
-- not much until you count what a mine actually has lying about: a whole
-- level of Dungeon Forlorn rarely holds more than a handful of loose
-- items, and a goblin that waited for a proper sackful would never go
-- home at all. This counts only what it is carrying, not the sword in
-- its hand (see CarriedCount).
local HAUL = 1

-- How wide the hoard lies around the chieftain.
local HOARD = 3


function CreateGoblinCamp(w, h)
	-- Somewhere on the level, wherever there is room: these levels are
	-- dug out fresh each game and have no fixed corner to put a camp in.
	local chief = Guardian("goblin_chieftain", GOBLIN_PACK, 1, 1, w - 2, h - 2)

	if (not chief) then
		return
	end

	local cx, cy = GetCreatureXY(chief)

	-- Guardian() posted him to the whole level, which was only ever a way
	-- of saying "anywhere there is room". Re-home him to the hoard itself
	-- now that it is known where that is, or he wanders off with the rest
	-- and the others come back to an empty floor.
	AsCreature(chief).xai:SetGuardArea(cx - 1, cy - 1, HOARD, HOARD,
		GetLocationId(GetCreatureLocation(chief)))

	-- What makes the hoard a hoard: a cell belonging to a place is passed
	-- over by XStandardAI::PickUpItems() and by the loot-remembering in
	-- AnalyzeGrid(), so the pack does not steal its own plunder back.
	EventPlace(cx - 1, cy - 1, HOARD, HOARD, 'GoblinHoardEvent')

	-- Something to start it off, so a camp found early is worth finding.
	for _ = 1, 3 do
		local loot = CreateObject(ItemKind.ITEM, 1, 400)

		if (loot) then
			DropItem(loot, chief)
		end
	end

	-- The raiders. They share the chieftain's group, so they share what
	-- the pack has seen - and they lose GUARD_AREA, because a creature
	-- sent back to its post every turn can never go looking (see the
	-- EXPLORER_MOVE branch in XStandardAI::Move).
	for i = 1, 5 do
		local sort = i <= 3 and "goblin" or "goblin_warrior"
		local raider = Guardian(sort, GOBLIN_PACK, cx - 3, cy - 3, 7, 7)

		if (raider) then
			AsCreature(raider).xai:ResAIFlag(XStandardAI.GUARD_AREA)
			SetEventHandler(raider, 'GoblinRaiderHandler')
			EnableMoveHandler(raider)
		end
	end
end


-- The hoard itself minds nobody: it exists to be a place, not to do
-- anything. Declared because EventPlace names a handler and an unnamed
-- one would be complained about.
function GoblinHoardEvent(e, p)
	return false
end


-- The chieftain of this one's own pack, found afresh each time: a
-- creature pointer kept from turn to turn goes stale the moment its owner
-- dies, and a goblin chieftain is not hard to kill.
local function Chieftain(raider)
	local where = GetLocationId(GetCreatureLocation(raider))

	if (not where) then
		return nil
	end

	for _, cr in ipairs(FindCreatures(where, GOBLIN_PACK)) do
		local c = AsCreature(cr)

		if (c.name == "goblin chieftain" and c.hp > 0) then
			return cr
		end
	end

	return nil
end


function GoblinRaiderHandler(e, t)
	if (e ~= LuaEvent.AI_TURN) then
		return false
	end

	local raider = AsCreature(t)

	-- Not yet worth the walk: let go of the chieftain and the engine goes
	-- back to exploring, which is what XStandardAI::Move() does with
	-- nobody to follow.
	if (CarriedCount(t) < HAUL) then
		raider.xai:SetCompanion(nil)

		return false
	end

	local chief = Chieftain(t)

	-- Nobody left to take it to. A pack whose chieftain is dead keeps
	-- what it has and carries on.
	if (not chief) then
		raider.xai:SetCompanion(nil)

		return false
	end

	-- Following him is what walks it home - and it also stops the pack's
	-- own guard area pulling it back mid-errand, since Move() leaves a
	-- creature alone about its post while it is escorting somebody.
	raider.xai:SetCompanion(AsCreature(chief))

	local rx, ry = GetCreatureXY(t)
	local hx, hy = GetCreatureXY(chief)
	local dx, dy = rx - hx, ry - hy

	if (dx >= -1 and dx <= 1 and dy >= -1 and dy <= 1) then
		DropCarried(t)
		raider.xai:SetCompanion(nil)
	end

	return false
end
