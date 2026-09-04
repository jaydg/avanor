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

		if (not TileDiggableInto(GetTile(x, y))) then
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

	AddMessage(CreatureName(digger, 0) .. " " .. CreatureVerb(digger, "smash") .. " the stone to pieces.")
	SetTile(x, y, TileDiggableInto(GetTile(x, y)))
	UseSkill(digger, XSkill.MINING)

	if (Rand(3) == 0) then
		AddMessage("There was some gold in ore.")
		DropMoney(digger, Rand(100) + 10, x, y)
	end

	return Result.SUCCESS
end
