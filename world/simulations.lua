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

-- THE ASSAULT ON AHK-ULAN ----------------------------------------------
--
-- A capable fighter comes up out of the dungeons into Ahk-Ulan's castle
-- and goes for the throne. Eight death knights stand between the stairs
-- and the sorcerer. The question is whether they are anything more than a
-- speed bump, and what would make them matter.
--
--   avanor --simulate ahkulan_assault --arg 0
--
-- --arg says how many death knights are left standing between him and the
-- throne, because "how many of them can a tenth-level character walk
-- through" is the difficulty dial stated in the plainest terms there are.
-- 0 leaves all twelve. The castle keeps them spread along the hall, so a
-- small number is the corridor fight a player actually picks, and twelve
-- is the throne room all at once.
--
-- THE FIGHTER IS A STAND-IN, NOT A HERO. Nothing headless has a real
-- hero in it - XHero::PlayerSetup() asks the player who they are - so
-- this is a creature built to the numbers a tenth-level human warrior
-- reaches, registered when the simulation starts rather than with the
-- rest of the world, so that it is never drawn into a dungeon and never
-- shifts a seed:
--
--   stats   human 1d4+8 plus the warrior's +4/+4/+4/-3/-3/-6
--   hit pts XHero's TOU/2+3 at first level, then nine rounds of
--           IncLevel()'s vRand(TOU/5+1)+1 - that is 9d3+10
--   kit     the warrior's long sword and small shield, and a full set of
--           found armour, which is what five dungeon levels buy
local ASSAULT_GROUP = "ahkulan_guardian"
local FIGHTER = "simulated_fighter"

-- Turn on to watch a fight tick by tick rather than read its result.
local TRACE = false
local knights_at_start = 0
local fighter, ahkulan
local turns = 0

local function DefineFighter()
	Monster.new(FIGHTER)
		:View("adventurer", '@', xColor.xWHITE, PersonType.NAMED_HE, CreatureTemplate.UNIQUE, "human")
		:Basic(100, 1000, CreatureSize.NORMAL, "1d200+1200")
		:Body("head neck body cloak hand hand ring ring gloves boots missile_weapon missile", 100)
		:Never("invisible")
		:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM + XStandardAI.FIND_WAY)
		:Stats("St 1d4+12 Dx 1d4+12 To 1d4+12 Le 1d4+5 Wi 1d4+5 Ma 1d4+2 Pe 1d4+8 Ch 1d4+8")
		:Combat("1d6", "1d2")
		:Main("1d4", 0, "9d3+10", "9d3+3")
		:Description("A stand-in for the player, built to a tenth-level "
			.. "warrior's numbers.")
		:LearnSkill(XSkill.HEALING, 6)
		:LearnSkill(XSkill.FINDWEAKNESS, 6)
		:Equip(ItemKind.WEAPON, "long_sword", 100)
		:Equip(ItemKind.SHIELD, "small_shield", 100)
		:Equip(ItemKind.BODY, "chain_mail", 100)
		:Unique()
		:Register()
end

local function Knights()
	local all = {}
	for _, cr in ipairs(FindCreatures("AHKULAN_CASTLE", ASSAULT_GROUP)) do
		if (AsCreature(cr).name == "death knight") then all[#all + 1] = cr end
	end
	return all
end

-- What is being tried on the defenders, as --arg's hundreds digit:
--
--   0  the death knights as they stand
--   1  tougher: +8 Toughness, which is hit points by way of GetMaxHP()
--   2  better trained: war skill raised to expert with what they carry
--   3  better armed: a great sword in place of the long sword
--
-- The units digit is how many of them there are. So --arg 203 is three
-- knights with the training tweak.
local function Tweak(knights, which)
	for _, k in ipairs(knights) do
		if (which == 1) then
			AddStats(k, "To 0d0+8")
		elseif (which == 2) then
			local w = GetWornItem(k, BodyPart.HAND, 0)

			if (w) then
				SetWarSkill(k, GetItemWarSkill(w), 8)
			end
		elseif (which == 3) then
			local blade = CreateObject(ItemKind.WEAPON, "halberd", 1, 1000000)

			if (blade) then
				AsCreature(k):PutOnBody(BodyPart.HAND, 0, blade)
				SetWarSkill(k, GetItemWarSkill(blade), 6)
			end
		end
	end
end


function AhkUlanAssaultSetup(arg)
	local knob = arg % 100
	local tweak = (arg - knob) / 100

	DefineFighter()

	for _, cr in ipairs(FindCreatures("AHKULAN_CASTLE", ASSAULT_GROUP)) do
		if (AsCreature(cr).name == "Ahk-Ulan, great master of Darkness") then ahkulan = cr end
	end

	if (not ahkulan) then print("PROBE: no Ahk-Ulan") return end

	-- The castle's huge rats are not part of the question and they are in
	-- the way of it: the fighter fought nothing else for three thousand
	-- turns the first time this was run.
	for _, cr in ipairs(FindCreatures("AHKULAN_CASTLE", "")) do
		if (AsCreature(cr).name == "huge rat") then
			InflictDamage(cr, 10000, "physical", "the simulation")
		end
	end

	-- Thin the guard down to the number asked for, keeping the first of
	-- them: that one is the anchor the fighter is put beside, and it has
	-- to be one that is certainly still alive - a creature killed this
	-- turn is still on the map until the graveyard is drained.
	local all = Knights()
	local anchor = all[1]

	if (knob > 0) then
		for i = knob + 1, #all do
			InflictDamage(all[i], 10000, "physical", "the simulation")
		end
	end

	knights_at_start = knob > 0 and knob or #all

	-- Put him in among them, so the fight happens rather than being waited
	-- for: the guard stands its posts and does not come looking, and half
	-- the castle between them is not the question being asked.
	Tweak(Knights(), tweak)

	fighter = anchor and CreatureNear(anchor, FIGHTER) or CreatureNear(ahkulan, FIGHTER)

	if (not fighter) then print("PROBE: nowhere to put the fighter") return end

	SetWarSkill(fighter, GetItemWarSkill(GetWornItem(fighter, BodyPart.HAND, 0)), 6)

	if (not TRACE) then
		return
	end

	local f = AsCreature(fighter)
	local w = GetWornItem(fighter, BodyPart.HAND, 0)
	local b = GetWornItem(fighter, BodyPart.BODY, 0)
	local fx, fy = GetCreatureXY(fighter)
	local ax, ay = GetCreatureXY(ahkulan)
	print(string.format("SETUP fighter hp=%d St=%d Dx=%d To=%d at %d,%d  weapon=%s body=%s | ahkulan hp=%d at %d,%d | knights=%d",
		f.max_hp, GetStats(fighter, XStats.STR), GetStats(fighter, XStats.DEX), GetStats(fighter, XStats.TOU),
		fx, fy, w and GetItemName(w) or "-", b and GetItemName(b) or "-",
		AsCreature(ahkulan).max_hp, ax, ay, knights_at_start))

	-- He is here for the sorcerer, and the household takes exception.
	SetItEnemyFor(ahkulan, fighter)
	SetItEnemyFor(fighter, ahkulan)
end

-- The last readable state of the two who matter, kept from turn to turn:
-- once one of them is dead its fields are no longer worth reading.
local f_hp, f_max, a_hp, a_max, done = 0, 0, 0, 0, nil

function AhkUlanAssaultDone()
	turns = turns + 10
	if (not fighter or not ahkulan) then return true end

	local f, a = AsCreature(fighter), AsCreature(ahkulan)

	if (f.hp > 0) then f_hp, f_max = f.hp, f.max_hp end
	if (a.hp > 0) then a_hp, a_max = a.hp, a.max_hp end

	if (TRACE) then
		local fx, fy = GetCreatureXY(fighter)
		local near, nd = nil, 9999
		for _, k in ipairs(Knights()) do
			local kx, ky = GetCreatureXY(k)
			local dx, dy = kx - fx, ky - fy
			if (dx < 0) then dx = -dx end
			if (dy < 0) then dy = -dy end
			local d = dx > dy and dx or dy
			if (d < nd) then near, nd = k, d end
		end
		print(string.format("T t=%d fighter %d/%d at %d,%d  nearest knight d=%d hp=%d  knights=%d",
			turns, f.hp, f.max_hp, fx, fy, nd,
			near and AsCreature(near).hp or -1, #Knights()))
	end

	-- The guard is down and the sorcerer is across the castle behind a
	-- door the fighter has no reason to walk through: the question has
	-- been answered, so do not spend another two hundred thousand turns
	-- watching him stand in the hall.
	if (#Knights() == 0) then done = "guard wiped" return true end

	if (f.hp <= 0) then done = "fighter died" return true end
	if (a.hp <= 0) then done = "ahkulan died" return true end

	return false
end

function AhkUlanAssaultReport()
	if (not fighter or not ahkulan) then return "setup failed" end

	return string.format("outcome=%-13s knights=%2d/%2d fighter=%3d/%3d ahkulan=%2d/%2d ticks=%d",
		done or "stalemate", #Knights(), knights_at_start,
		f_hp, f_max, a_hp, a_max, turns)
end

Simulation.new("ahkulan_assault")
	:Called("whether the death knights are more than a speed bump")
	:Setup("AhkUlanAssaultSetup")
	:Finished("AhkUlanAssaultDone")
	:Report("AhkUlanAssaultReport")
	:Turns(200000)
	:AskEvery(10)
	:Register()
