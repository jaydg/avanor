-- Questions about this world, asked by playing them out.
--
-- A simulation arranges something, lets the turns run, and says how it
-- went. Nothing is drawn and nothing is saved; the report is one line on
-- stdout, so a shell loop can collect a few hundred of them and take an
-- average.
--
--   avanor --simulate orc_attack --arg 3 --seed 7
--
--   Simulation.new(id)
--       :Called(text)      what the question is, shown when --simulate is
--                          given a name nothing declares
--       :Setup(fn)         arranges it. Handed --arg as a number, which
--                          each scenario reads however it likes
--       :Finished(fn)      asked now and then whether anything is left to
--                          decide; true ends the run early. Unsaid, it
--                          runs to the turn limit
--       :Report(fn)        answers with the line to print
--       :Turns(n)          how long to allow. Unsaid, 100000
--       :AskEvery(n)       how often to ask :Finished(). Unsaid, 500
--       :Register()
--
-- The engine knows none of what follows: who fights, what counts as
-- finished and what is worth reporting are this world's business.


-- THE ORC WAR PARTY'S ATTACK ------------------------------------------
--
-- Twenty orcs muster in the southern hills and march on the town. Waiting
-- for them are seven royal guardians, their captain Ozorick, and Gekta the
-- sheep dog, who fights but cannot hold a sword.
--
-- Ozorick's errand has the hero bring the watch a sword of orc slaying,
-- which is worth four times its damage against an orc. --arg says how
-- many of the defenders get one, so that "does the sword matter?" can be
-- answered with a number rather than an impression.

local ORC_BATTLE_GROUP = "guardian"

-- Who the errand's sword is actually for.
local GUARD_NAME = "royal guardian"

-- Counted at the start so the report can say "of how many".
local orcs_at_start = 0
local guards_at_start = 0
local armed = 0

local function Defenders()
	return FindCreatures("MAIN", ORC_BATTLE_GROUP)
end

local function WarParty()
	return FindCreatures("MAIN", ORC_WAR_PARTY)
end

function OrcAttackSetup(swords)
	orcs_at_start = #WarParty()
	guards_at_start = #Defenders()

	-- Hand out the blades, to the same people the errand does: Ozorick
	-- tells the hero to take the sword to one of his guardians, so the
	-- captain is not among them. That is not only faithful - he already
	-- carries Glamdring and Death Hack, and putting an ordinary blade in
	-- his hand would be taking something away rather than adding to it.
	--
	-- Gekta the sheep dog fights alongside the watch but has no hand to
	-- hold a sword in, so she is passed over rather than counted among
	-- the armed.
	armed = 0

	for _, guard in ipairs(Defenders()) do
		if (armed >= swords) then
			break
		end

		if (AsCreature(guard).name == GUARD_NAME
			and HasBodyPart(guard, BodyPart.HAND, 0)) then
			local sword = CreateObject(ItemKind.WEAPON, "long_sword", 1, 1000000)

			if (sword) then
				SetItemBrand(sword, "orc_slayer")
				AsCreature(guard):PutOnBody(BodyPart.HAND, 0, sword)
				armed = armed + 1
			end
		end
	end

	-- The march, which the location's own timer would otherwise order a
	-- very long while from now.
	OrcWarPartyAttack(nil)
end

function OrcAttackDone()
	return #WarParty() == 0 or #Defenders() == 0
end

function OrcAttackReport()
	return string.format("swords=%d orcs=%d/%d guards=%d/%d",
		armed, #WarParty(), orcs_at_start, #Defenders(), guards_at_start)
end

Simulation.new("orc_attack")
	:Called("how the orc war party's attack on the royal guard ends")
	:Setup("OrcAttackSetup")
	:Finished("OrcAttackDone")
	:Report("OrcAttackReport")
	:Turns(300000)
	:Register()

-- How many of the village's farmers come back from the mushroom caves.
--
-- The Elder sends them down once his quest closes, and they keep going:
-- the errand is a loop, not one trip, so what it costs the village is
-- attrition rather than a single walk. They go in a robe and a long
-- spear and nothing else, and die faster than the village can spare.
--
-- Beelzevile is killed before anyone sets out, because that is the only
-- state of the world in which they ever set out: the Elder sends them
-- when his quest closes, and it closes when the hero brings him word
-- that the demon is dead. Leaving Beelzevile alive down there measures a
-- walk the farmers never actually take.
--
-- --arg says what they are sent down in:
--
--   0  as they are - a robe and a long spear
--   1  and soft boots, for the empty slot
--   2  scale mail instead of the robe
--   3  both
--
-- The report is how many of them are still alive after the errand has
-- been running a while, so the arms can be compared as attrition rather
-- than as one trip.

local FARMER_NAME = "farmer"

local farmers_at_start = 0

local function Farmers()
	local out = {}

	for _, cr in ipairs(FindCreatures("MAIN", VILLAGE_GROUP)) do
		if (AsCreature(cr).name == FARMER_NAME) then
			table.insert(out, cr)
		end
	end

	-- They spend most of the errand underground, so the valley alone is
	-- not where to count them.
	for _, id in ipairs({"MUSHROOMS_CAVE1", "MUSHROOMS_CAVE2", "MUSHROOMS_CAVE5"}) do
		for _, cr in ipairs(FindCreatures(id, VILLAGE_GROUP)) do
			if (AsCreature(cr).name == FARMER_NAME) then
				table.insert(out, cr)
			end
		end
	end

	return out
end

local function Dress(farmer, kind, id, slot)
	local it = CreateObject(kind, id, 1, 1000000)

	if (it and HasBodyPart(farmer, slot, 0)) then
		AsCreature(farmer):PutOnBody(slot, 0, it)
	end
end

-- What the hero did before the Elder would send anyone down. Named by the
-- blow so that, if it ever shows on screen, it reads as something rather
-- than as nothing in particular.
local function KillTheDemon()
	for _, cr in ipairs(FindCreatures("MUSHROOMS_CAVE2", "")) do
		if (AsCreature(cr).name:find("Beelzevile")) then
			InflictDamage(cr, 100000, "", "the hero's errand")

			return true
		end
	end

	return false
end

function MushroomErrandSetup(outfit)
	SIM_OUTFIT = outfit

	if (not KillTheDemon()) then
		print("NOTE: no demon found on MUSHROOMS_CAVE2")
	end

	local all = Farmers()
	farmers_at_start = #all

	for _, farmer in ipairs(all) do
		if (outfit == 1 or outfit == 3) then
			Dress(farmer, ItemKind.BOOTS, "soft_boots", BodyPart.BOOTS)
		end

		if (outfit == 2 or outfit == 3) then
			Dress(farmer, ItemKind.BODY, "light_mail", BodyPart.BODY)
		end
	end

	SendFarmersToCollectMushrooms()
end

function MushroomErrandDone()
	return #Farmers() == 0
end

function MushroomErrandReport()
	return string.format("outfit=%d farmers=%d/%d", SIM_OUTFIT or 0,
		#Farmers(), farmers_at_start)
end

Simulation.new("mushroom_errand")
	:Called("how many of the village's farmers survive the errand underground")
	:Setup("MushroomErrandSetup")
	:Finished("MushroomErrandDone")
	:Report("MushroomErrandReport")
	:Turns(1000000)
	:Register()
