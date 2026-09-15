-- The things that grow: a herb bush and a patch of mushrooms.
--
-- Both are ordinary map objects that take turns and do their thinking
-- here. What they have in common is only that they grow a species from
-- world/items/herbs.lua and can be picked.

local HERB_REFRESH = 1000000
local MUSH_LIFETIME = 1000000

-- A bush lives by Conway's rules: it withers alone and withers in a
-- crowd, thrives with exactly three neighbours, and seeds any empty
-- fertile ground that has three bushes around it.
MapObject.new("herb_bush")
	:View("bush", '"', xColor.xGREEN)
	:OnRun("BushTurn")
	:OnPick("BushPick")
	:OnName("BushName")
	:FirstDelay(1)
	:Register()

-- Mushrooms simply stand until their time is up.
MapObject.new("mushroom")
	:View("mushroom", '`', xColor.xLIGHTGRAY)
	:OnRun("MushroomTurn")
	:OnPick("MushroomPick")
	:OnName("MushroomName")
	:FirstDelay(1, MUSH_LIFETIME)
	:Register()

-- Which species a plant is growing. Settled the moment it is planted, so
-- it reads correctly to anybody who looks at it before its first turn;
-- asked again later it simply says what it already decided.
function Species(plant, kind)
	local species = RecallText(plant, "species")

	if (species == "") then
		species = RandomPlantSpecies(kind) or ""
		MemoriseText(plant, "species", species)

		if (species ~= "") then
			SetObjectView(plant, PlantName(species, true), nil, PlantColour(species))
		end
	end

	return species
end

local function CountBushes(x, y, here)
	local n = 0

	for i = x - 1, x + 1 do
		for j = y - 1, y + 1 do
			if (not (i == x and j == y) and GetSpecialId(i, j, here) == "herb_bush") then
				n = n + 1
			end
		end
	end

	return n
end

-- Which map object grows which sort of plant. A world adding a third kind
-- of plant adds a PlantKind in world/items/herbs.lua, a MapObject here,
-- and a line to this table.
local GROWS = {
	herb     = "herb_bush",
	mushroom = "mushroom",
}

-- Puts one down and settles what it is growing.
function Plant.Place(kind, x, y, here)
	local plant = PlaceObject(GROWS[kind], x, y, here)

	if (plant) then
		Species(plant, kind)
	end

	return plant
end

function BushTurn(bush)
	Species(bush, "herb")

	local x, y = GetObjectXY(bush)
	local here = GetObjectLocation(bush)
	local n = CountBushes(x, y, here)
	local strength = Recall(bush, "strength")

	if (strength == 0) then
		strength = 1
	end

	-- Too lonely or too crowded: it loses heart, and dies once it has
	-- none left.
	if (n < 2 or n > 3) then
		strength = strength - 1

		if (strength <= 0) then
			return nil
		end
	end

	if (n == 3 and strength < 3) then
		strength = strength + 1
	end

	Memorise(bush, "strength", strength)

	-- Seed the ground around it, wherever three bushes meet.
	for i = x - 1, x + 1 do
		for j = y - 1, y + 1 do
			if (not HasSpecial(i, j, here) and TileFertile(i, j, here)
				and CountBushes(i, j, here) == 3) then
				Plant.Place("herb", i, j, here)
			end
		end
	end

	return Rand(HERB_REFRESH)
end

function BushPick(bush, picker)
	local species = Species(bush, "herb")

	UseSkill(picker, XSkill.HERBALISM)

	local strength = Recall(bush, "strength") - 1
	Memorise(bush, "strength", strength)

	if (strength <= 0) then
		DestroyMapObject(bush)
	end

	-- Unidentified: knowing a bush when you see one is not the same as
	-- knowing what you have picked.
	return CreateHerb(species)
end

function BushName(bush, viewer)
	local species = Species(bush, "herb")
	local size = ({ "small", "medium", "large" })[Recall(bush, "strength")] or "small"

	if (PlantDifficulty(species) > GetSkill(viewer, XSkill.HERBALISM)
		and not PlantKnown(species)) then
		return size .. " bush of unknown herbs"
	end

	LearnPlant(species)

	return size .. " bush of " .. (PlantName(species, true) or "herbs")
end

function MushroomTurn(mushroom)
	Species(mushroom, "mushroom")

	if (isObjectVisible(mushroom)) then
		AddMessage("Suddenly mushroom dissapered in the small cloud of spores.")
	end

	return nil
end

function MushroomPick(mushroom, picker)
	local species = Species(mushroom, "mushroom")

	UseSkill(picker, XSkill.HERBALISM)
	DestroyMapObject(mushroom)

	-- A mushroom is unmistakable: whoever picks one knows what it is.
	return CreateHerb(species, true)
end

-- Gathering, as an errand step: given to a creature with
--
--   {cmd = ScriptCommand.CALL, fn = "CollectMushroom"}
--
-- and asked once a turn until it answers true, which ends the step and
-- sends the gatherer on to the next thing in its script.
--
-- Nothing here is about walking: a gatherer with RANDOM_MOVE in its AI
-- flags wanders the cave of its own accord on the turns this finds
-- nothing to pick, which is exactly what the engine used to do by hand.
local COLLECT_ENOUGH_ODDS = 2

function CollectMushroom(gatherer)
	local x, y = GetCreatureXY(gatherer)
	local here = GetSpecial(x, y)

	-- Whatever is underfoot answers for itself, through the same handler
	-- the hero's own picking goes through. Standing on nothing, or on
	-- something that is not for picking, is not a reason to give up - the
	-- cave is large and the gatherer keeps looking.
	if (not here or GetSpecialId(x, y) ~= "mushroom") then
		return false
	end

	local picked = MushroomPick(here, gatherer)

	if (not picked) then
		return false
	end

	local who = AsCreature(gatherer)
	who:ContainItem(AsItem(picked))

	if (isCreatureVisible(gatherer)) then
		-- 0 is CRN_T1, the subject form - "the farmer", not "him". The
		-- bare number is how the rest of world/ spells it.
		AddMessage(string.format("%s collects %s.",
			CreatureName(gatherer, 0), DescribeItem(picked)))
	end

	-- An armful is enough. Which armful is nobody's decision in
	-- particular, so it is the same coin the engine used to toss.
	return Rand(COLLECT_ENOUGH_ODDS) == 0
end

function MushroomName(mushroom, viewer)
	return PlantName(Species(mushroom, "mushroom"), true) or "mushroom"
end
