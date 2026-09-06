-- What they shoot.
--
-- Template.new(kind, type) is documented in world/items/init.lua.
--

-- Something on the head. One missile in twenty carries one.
local MISSILE_BRANDS = {
	"poison",
	"fire",
	"undead_slayer",
	"orc_slayer",
}

-- Better made than the rest. Another one in twenty is, and the two can
-- meet: a poisoned seeker arrow is rare, but nothing forbids it.
local MISSILE_CRAFT = {
	{ name = "seeker", range = 1, dice = { 2, 2, 0 } },
	{ name = "hunter", range = 2, to_hit = 10 },
	{ name = "sharp", range = 2, dice = { 1, 1, 10 } },
}

-- Called by the engine on every finished missile of a sort that asked for
-- it. Anything special halves the heap it was found in: the better the
-- arrow, the fewer of them lying about.
--
-- The rock asks for none of this. A rock is a rock.
function FinishMissile(item)
	local heap = nil

	if (Rand(20) == 0) then
		SetItemBrand(item, MISSILE_BRANDS[Rand(4) + 1])
		heap = 10
	end

	if (Rand(20) == 0) then
		local craft = MISSILE_CRAFT[Rand(3) + 1]

		SetItemName(item, craft.name .. " " .. GetItemName(item))
		AddItemRange(item, craft.range)

		if (craft.to_hit) then
			AddItemToHit(item, craft.to_hit)
		end

		if (craft.dice) then
			AddItemDice(item, craft.dice[1], craft.dice[2], craft.dice[3])
		end

		heap = (heap or 20) / 2
	end

	if (heap) then
		AsItem(item).quantity = 3 + Rand(heap)
	end
end

Template.new(ItemKind.MISSILE, "arrow")
	:View("arrow", '\\')
	:Made("missile", ItemQuality.AVG)
	:Launcher("bow")
	:Worth(1, 1)
	:Combat("1d2", "1d4", "1d3")
	:Range("1d2+3")
	:Chance(100)
	:OnCreate("FinishMissile")
	:Register()

Template.new(ItemKind.MISSILE, "quarrel")
	:View("quarrel", '\\')
	:Made("missile", ItemQuality.AVG)
	:Launcher("crossbow")
	:Worth(1, 1)
	:Combat("1d2", "1d6", "1d3")
	:Range("1d2+3")
	:Chance(100)
	:OnCreate("FinishMissile")
	:Register()

Template.new(ItemKind.MISSILE, "sling_bullet")
	:View("sling bullet", '\\')
	:Made("missile", ItemQuality.FAIR)
	:Launcher("sling")
	:Worth(1, 1)
	:Combat("1d2", "1d5", "1d2")
	:Range("1d2+3")
	:Chance(30)
	:OnCreate("FinishMissile")
	:Register()

Template.new(ItemKind.MISSILE, "rock")
	:View("rock", '*')
	:Made("stone", ItemQuality.POOR)
	:Launcher("sling")
	:Worth(1, 1)
	:Combat("1d1", "1d3", "1d1")
	:Range("1d2+2")
	:Chance(300)
	:Register()

Template.new(ItemKind.MISSILE, "shuriken")
	:View("shuriken", '*')
	:Made("metal", ItemQuality.FAIR)
	:Worth(1, 1)
	:Combat("1d2", "1d6", "1d4")
	:Range("1d2+2")
	:Chance(50)
	:OnCreate("FinishMissile")
	:Register()
