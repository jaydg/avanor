-- Tools: what the miscellany drawer of the pack holds. ItemKind.TOOL is
-- the category; an XTool that the use command can be pointed at is a
-- different thing, and needs a :Use() handler (see the Eye of Raa, in
-- world/locations/forlorn.lua).

Item.new("ancient_machine_part")
	:Plain("ancient_machine_part", ItemKind.TOOL)
	:View("ancient machine part", ']', xColor.xDARKGRAY)
	:Basic(1000, 15)
	:Register()


-- The pickaxe. Digging is content: which walls give way is declared per
-- tile in world/tiles.lua (`diggable_into`), and how long each swing takes
-- off the rock is the sum below. The engine only carries the turns.
--
-- Every wall has the same 1000 points of resistance to get through. A
-- swing takes off the pickaxe's own dice (1d10), five more for each rank
-- of mining, and one for every two points of strength - so a strong dwarf
-- who knows the work is through a wall in a few turns where a scholar
-- would still be chipping at it.
--
-- A dig spans many turns, so what is left of the rock and where it is are
-- kept on the pickaxe itself (ToolRemember/ToolRecall) and saved with it -
-- the old C++ pickaxe held them in plain members it never serialized, so
-- saving mid-dig quietly lost the progress.
Item.new("pickaxe")
	:Tool("pickaxe")
	:View("pickaxe", '\\', xColor.xLIGHTGRAY)
	:Basic(150, 100)
	:Combat(0, 1, 10, 0)
	:Use("DigWithPickaxe")
	:Register()

function DigWithPickaxe(state, item, digger)
	if (not isHero(digger)) then
		return Result.FAIL
	end

	if (state == ItemUse.START) then
		local dx, dy = AskDirection(digger)

		if (not dx) then
			return Result.FAIL
		end

		local x, y = GetCreatureXY(digger)
		x = x + dx
		y = y + dy

		if (not TileDiggableInto(GetTile(x, y, GetCreatureLocation(digger)))) then
			AddMessage("You can't dig something other than walls.")
			return Result.FAIL
		end

		ToolRemember(item, "x", x)
		ToolRemember(item, "y", y)
		ToolRemember(item, "rock", 1000)
		AddMessage(CreatureName(digger, 0) .. " " .. CreatureVerb(digger, "start") .. " to dig.")

		return Result.CONTINUE
	end

	if (state ~= ItemUse.PROGRESS) then
		return Result.FAIL
	end

	-- One swing: the tool's own bite, plus what the digger knows and how
	-- strong they are.
	local rock = ToolRecall(item, "rock")
		- ThrowItemDice(item)
		- GetSkill(digger, XSkill.MINING) * 5
		- GetStats(digger, XStats.STR) / 2

	if (rock >= 0) then
		ToolRemember(item, "rock", rock)
		return Result.CONTINUE
	end

	local x = ToolRecall(item, "x")
	local y = ToolRecall(item, "y")

	-- The digger's own level, named rather than left to default.
	local level = GetCreatureLocation(digger)

	AddMessage(CreatureName(digger, 0) .. " " .. CreatureVerb(digger, "smash") .. " the stone to pieces.")
	SetTile(x, y, TileDiggableInto(GetTile(x, y, level)), level)
	UseSkill(digger, XSkill.MINING)

	if (Rand(3) == 0) then
		AddMessage("There was some gold in ore.")
		DropMoney(digger, Rand(100) + 10, x, y)
	end

	return Result.SUCCESS
end


-- The alchemy set: distils a herb into the potion that species yields.
--
-- Which potion that is, and how hard it is to get, are not declared
-- anywhere in world/ - they are dealt out afresh each game (see
-- world/items/herbs.lua), so this asks rather than looks them up.
Item.new("alchemy_set")
	:Tool("alchemy_set")
	:View("alchemy set", '[', xColor.xLIGHTGRAY)
	:Basic(150, 100)
	:Use("DistilHerb")
	:Register()

function DistilHerb(state, item, alchemist)
	-- One turn's work: the set answers SUCCESS at once, so START is the
	-- only state it is ever asked about.
	if (state ~= ItemUse.START) then
		return Result.SUCCESS
	end

	local herb = SelectItem(alchemist, function(candidate)
		local c = AsItem(candidate)
		return IsKind(c.kind, ItemKind.FOOD) and c.it == "herb"
	end)

	if (not herb) then
		return Result.FAIL
	end

	local pn = HerbPotion(herb)

	if (not pn) then
		return Result.FAIL
	end

	-- The better the alchemist and the plainer the potion, the likelier.
	local chance = GetSkill(alchemist, XSkill.ALCHEMY) * 8 + 30
		- PotionAlchemyPower(pn) * 10

	if (Rand(100) < chance) then
		local potion = CreatePotion(pn)

		AddMessage(AsCreature(alchemist).name .. " managed to create a "
			.. DescribeItem(potion) .. ".")
		GiveObjectToCreature(potion, alchemist)
		UseSkill(alchemist, XSkill.ALCHEMY)
	else
		AddMessage(AsCreature(alchemist).name .. " failed to create a potion.")
	end

	DestroyObject(herb)

	return Result.SUCCESS
end


-- The cooking set: puts a raw corpse over the fire for a while, and if the
-- cook knows their trade, turns it into something worth eating.
--
-- The corpse comes out of the pack while it cooks - the set holds it, and
-- holds the only reference to it, which is why an abandoned cooking hands
-- it back rather than dropping it on the floor.
Item.new("cooking_set")
	:Tool("cooking_set")
	:View("cooking set", '[', xColor.xLIGHTGRAY)
	:Basic(150, 100)
	:Use("CookCorpseOverFire")
	:Register()

function CookCorpseOverFire(state, item, cook)
	if (state == ItemUse.START) then
		if (not isHero(cook)) then
			return Result.FAIL
		end

		local corpse = SelectItem(cook, function(candidate)
			return isRawCorpse(candidate)
		end)

		if (not corpse) then
			return Result.FAIL
		end

		-- Out of the pack and onto the fire: it stops rotting while it
		-- cooks, and the set is now the only thing holding it.
		ToolHold(item, corpse)
		StopCorpseRotting(corpse, true)
		ToolRemember(item, "left", 50 - GetSkill(cook, XSkill.COOKING) * 2)

		if (isCreatureVisible(cook)) then
			AddMessage(CreatureName(cook, 0) .. " " .. CreatureVerb(cook, "start")
				.. " to cook " .. GetItemName(corpse) .. ".")
		end

		return Result.CONTINUE
	end

	local corpse = ToolHeld(item)

	if (not corpse) then
		return Result.FAIL
	end

	-- Given up on, or interrupted: hand it back raw and let it rot again.
	if (state == ItemUse.FINISH) then
		StopCorpseRotting(corpse, false)
		GiveObjectToCreature(corpse, cook)
		ToolRelease(item)

		return Result.SUCCESS
	end

	if (state ~= ItemUse.PROGRESS) then
		return Result.FAIL
	end

	local left = ToolRecall(item, "left") - 1

	if (left > 0) then
		ToolRemember(item, "left", left)
		return Result.CONTINUE
	end

	UnCarryItem(corpse)

	if (Rand(100) < GetSkill(cook, XSkill.COOKING) * 4 + 30) then
		-- Cooked meat goes twice as far and weighs a tenth of the carcass.
		CookCorpse(corpse)
		SetItemNutrition(corpse, GetItemNutrition(corpse) * 2)
		SetItemWeight(corpse, GetItemWeight(corpse) / 10)
		GiveObjectToCreature(corpse, cook)
		UseSkill(cook, XSkill.COOKING, 3)

		if (isCreatureVisible(cook)) then
			AddMessage(CreatureName(cook, 0) .. " " .. CreatureVerb(cook, "cook")
				.. " " .. GetItemName(corpse) .. ".")
		end
	elseif (isCreatureVisible(cook)) then
		AddMessage(CreatureName(cook, 0) .. " " .. CreatureVerb(cook, "fail")
			.. " to cook " .. GetItemName(corpse) .. ".")
	end

	ToolRelease(item)

	return Result.SUCCESS
end
