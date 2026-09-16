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
