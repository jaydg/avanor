-- DUNGEON FORLORN ---------------------------------------------------------
--
-- A gem mine, once. The dwarves who sank it followed the seams down and
-- then followed them further, and somewhere below the last gallery they
-- broke into something that was not a seam. What poured up through that
-- breach took the mine in a night, and the miners who got out did not go
-- back for the tools, let alone the stones. The head-house on the plain
-- south of the valley still stands, more or less, over a shaft nobody has
-- climbed down in living memory.
--
-- Later, and much smaller: a servant of King Roderick who had stolen the
-- Eye of Raa needed somewhere nobody would follow him, and got as far as
-- the bottom before whatever lives there found him. He may have gone down
-- any of the three workings; see world/locations/mines.lua for what the
-- three have in common and for the Eye's placing.
--
-- Forlorn's one speciality is the goblin raiders. They are not a feature
-- of abandoned mines in general - the other two have none.

-- Which digging pattern the mine takes this game. All five are caverns
-- rather than mazes - galleries and halls, which is what a mine collapses
-- into - and each gives the whole mine its character, so every level of a
-- given game looks like it belongs to the same workings.
DUNGEON_FORLORN_PRESETS = {
	"old34_10", "old35_0", "old23_50", "old24_200", "old34_0"
}

-- The artifact Roderick wants back, waiting at the bottom of the mine.
--
-- The first content item with behaviour of its own: using it throws a
-- lightning bolt, and CastEffect() is the form that asks the wielder which
-- way to aim it - MakeEffect() would fire it in no direction at all.
--
-- :Artifact() is what keeps it where it was put: every undead template can
-- pick things up, and one that pocketed this would leave the player
-- guessing which of them to hunt (XStandardAI::PickUpItems).
Item.new("eye_of_raa")
	:Tool("eye_of_raa")
	:View("Eye of Raa", '*', xColor.xCYAN)
	:Basic(150, 100)
	:Combat(0, 1, 10, 0)
	:Stats("Ma:0d0+10 Wi:0d0+10")
	:Resist{ air = "0d0+100" }
	:Artifact()
	:Use('EyeOfRaaUse')
	:Register()


-- Only the hero gets the lightning; a monster that somehow used it just
-- wastes the turn, exactly as before.
function EyeOfRaaUse(state, item, user)
	if (state ~= ItemUse.START or not isHero(user)) then
		return Result.SUCCESS
	end

	CastEffect(user, "lightning_bolt", 30)

	return Result.SUCCESS
end


-- The head-house and the apron of spoil and dressed stone the miners left
-- spread around it. The roof is down in two places and there are saplings
-- in what was the yard, but the doorway still stands and so does the shaft
-- inside it.
local function ForlornRuin(x, y, mine)
	SetPattern(21, 11,
	"        ;;;;;        " ..
	"     ;;;;;;;;; ;;    " ..
	"   ;;;;##### ###;;;  " ..
	"  ;;;;;#;;;;;;;#;;;  " ..
	" ;; ;;;+;;>;;;;#;;;; " ..
	"  ;;;;;#;;;;;;;# ;;  " ..
	"   ;;;;###;; ###;;;  " ..
	"    ;;;;;;;;;;;;;;   " ..
	"  &   ;;  ;;;;;   &  " ..
	"       ;;;; ;;       " ..
	"         ;;;         ")
	AddTranslation(">", function(sx, sy)
		Way(XStairWay.DOWN, mine.id .. "1", sx, sy)
	end)
	DrawPattern(x, y)
end


-- One goblin camp in the mine, never on the first level - the pack has
-- come up from below, not in through the front door - and never the last,
-- where the Eye may be: what is down there is meant to be guarded by what
-- killed the thief, not carried off to a hoard before the player arrives.
local goblins_at = nil

local function ForlornLevel(level, depth, workings)
	-- Chosen on the way past the first level rather than once ever, so a
	-- second world built in the same process does not inherit the first
	-- one's camp.
	if (level == 1) then
		goblins_at = Rand(depth - 2) + 2
	end

	if (level == goblins_at) then
		CreateGoblinCamp(workings.width, workings.height)
	end
end


table.insert(MINES, {
	id = "FORLORN",
	brief = "Frl",
	name = "Forlorn",
	presets = DUNGEON_FORLORN_PRESETS,
	ruin = ForlornRuin,
	on_level = ForlornLevel,
})
