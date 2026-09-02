-- What the player begins the game with.
--
-- InitHero() is called by the engine (XHero::PlayerSetup) once the player
-- has chosen who they are, and before the game asks their name. Race and
-- profession arrive as the names shown in the creation menus - "half elf",
-- "wizard" - so a definition reads the way the choice did.
--
-- It is optional: an engine whose world defines no InitHero starts its
-- heroes with nothing at all. It is called protected, so a mistake here
-- costs a message on stderr rather than the character.
--
--   hero        the new hero
--   race        "human", "half elf", "high elf", "halfling", "half orc",
--               "dwarf", "gnome"
--   profession  "warrior", "wizard", "archer", "ranger", "cleric",
--               "paladin", "alchemist", "bard"

-- Every profession, by key, in menu order. A race names the list it offers;
-- today they all offer the same one.
ALL_PROFESSIONS = {"warrior", "wizard", "archer", "ranger", "cleric",
	"paladin", "alchemist", "bard"}


-- The races a character may be, what each is made of, and which
-- professions are open to it. The engine asks for this list to build
-- the creation menus and never decides any of it for itself.
--
--   key         what content matches on, and what is written into the
--               saved character
--   name        what the menu and the character sheet show
--   stats       starting figures
--   max_stats   how far they can ever be trained
--   speed       how long a turn takes; "0d0+1000" is ordinary
--   food        how kindly the stomach takes to what it is given
--   skills      what the race simply knows
--   professions which callings it may take up, in menu order
HERO_RACES = {
	{
		key = "human", name = "human",
		stats = "St:1d4+8 Dx:1d4+8 To:1d4+8 Le:1d4+8 Wi:1d4+8 Ma:1d4+8 Pe:1d4+8 Ch:1d4+8",
		max_stats = "St:1d8+18 Dx:1d8+18 To:1d8+18 Le:1d8+18 Wi:1d8+18 Ma:1d8+18 Pe:1d8+18 Ch:1d8+18",
		speed = "0d0+1000",
		food = FoodFeeling.NORMAL,
		skills = {XSkill.COOKING, XSkill.BACKSTABBING},
		professions = ALL_PROFESSIONS,
	},
	{
		key = "half_elf", name = "half elf",
		stats = "St:1d4+6 Dx:1d4+10 To:1d4+6 Le:1d4+10 Wi:1d4+8 Ma:1d4+8 Pe:1d4+8 Ch:1d4+8",
		max_stats = "St:1d8+16 Dx:1d8+20 To:1d8+16 Le:1d8+20 Wi:1d8+18 Ma:1d8+18 Pe:1d8+18 Ch:1d8+18",
		speed = "0d0+1000",
		food = FoodFeeling.NORMAL,
		skills = {},
		professions = ALL_PROFESSIONS,
	},
	{
		key = "high_elf", name = "high elf",
		stats = "St:1d4+5 Dx:1d4+12 To:1d4+4 Le:1d4+12 Wi:1d4+8 Ma:1d4+8 Pe:1d4+8 Ch:1d4+8",
		max_stats = "St:1d8+15 Dx:1d8+22 To:1d8+14 Le:1d8+22 Wi:1d8+18 Ma:1d8+18 Pe:1d8+18 Ch:1d8+18",
		speed = "0d0+1000",
		food = FoodFeeling.NORMAL,
		skills = {XSkill.COOKING},
		professions = ALL_PROFESSIONS,
	},
	{
		key = "halfling", name = "halfling",
		stats = "St:1d4+5 Dx:1d4+12 To:1d4+4 Le:1d4+8 Wi:1d4+8 Ma:1d4+8 Pe:1d4+10 Ch:1d4+10",
		max_stats = "St:1d8+15 Dx:1d8+22 To:1d8+14 Le:1d8+18 Wi:1d8+18 Ma:1d8+18 Pe:1d8+20 Ch:1d8+20",
		speed = "0d0+1000",
		food = FoodFeeling.SENSITIVE,
		skills = {XSkill.DODGE},
		professions = ALL_PROFESSIONS,
	},
	{
		key = "half_orc", name = "half orc",
		stats = "St:1d4+14 Dx:1d4+10 To:1d4+12 Le:1d4+4 Wi:1d4+4 Ma:1d4+4 Pe:1d4+10 Ch:1d4+6",
		max_stats = "St:1d8+24 Dx:1d8+20 To:1d8+22 Le:1d8+14 Wi:1d4+14 Ma:1d8+14 Pe:1d8+20 Ch:1d8+16",
		speed = "0d0+1000",
		food = FoodFeeling.TOLERANT,
		skills = {XSkill.ATHLETICS},
		professions = ALL_PROFESSIONS,
	},
	{
		key = "dwarf", name = "dwarf",
		stats = "St:1d4+10 Dx:1d4+4 To:1d4+14 Le:1d4+8 Wi:1d4+10 Ma:1d4+6 Pe:1d4+6 Ch:1d4+6",
		max_stats = "St:1d8+20 Dx:1d8+14 To:1d8+24 Le:1d8+18 Wi:1d8+20 Ma:1d8+16 Pe:1d8+16 Ch:1d8+16",
		speed = "0d0+1000",
		food = FoodFeeling.NORMAL,
		skills = {XSkill.MINING},
		professions = ALL_PROFESSIONS,
	},
	{
		key = "gnome", name = "gnome",
		stats = "St:1d4+5 Dx:1d4+8 To:1d4+5 Le:1d4+12 Wi:1d4+12 Ma:1d4+8 Pe:1d4+8 Ch:1d4+6",
		max_stats = "St:1d8+15 Dx:1d8+18 To:1d8+15 Le:1d8+22 Wi:1d8+22 Ma:1d8+18 Pe:1d8+18 Ch:1d8+16",
		speed = "0d0+1000",
		food = FoodFeeling.NORMAL,
		skills = {XSkill.MINING, XSkill.FINDWEAKNESS},
		professions = ALL_PROFESSIONS,
	},
}


-- The callings. Every race may take any of them today; a race that
-- wanted a shorter list would simply name one.
HERO_PROFESSIONS = {
	{ key = "warrior", name = "warrior",
	  stats = "St:0d0+4 Dx:0d0+4 To:0d0+4 Le:0d0-3 Wi:0d0-3 Ma:0d0-6 Pe:0d0+0 Ch:0d0+0" },
	{ key = "wizard", name = "wizard",
	  stats = "St:0d0-3 Dx:0d0-3 To:0d0-3 Le:0d0+3 Wi:0d0+3 Ma:0d0+3 Pe:0d0+0 Ch:0d0+0" },
	{ key = "archer", name = "archer",
	  stats = "St:0d0+1 Dx:0d0+4 To:0d0+2 Le:0d0-2 Wi:0d0-2 Ma:0d0-4 Pe:0d0+1 Ch:0d0+0" },
	{ key = "ranger", name = "ranger",
	  stats = "St:0d0+0 Dx:0d0+2 To:0d0+2 Le:0d0-2 Wi:0d0-1 Ma:0d0-0 Pe:0d0+0 Ch:0d0+0" },
	{ key = "cleric", name = "cleric",
	  stats = "St:0d0-2 Dx:0d0-2 To:0d0-1 Le:0d0+2 Wi:0d0+2 Ma:0d0-2 Pe:0d0+3 Ch:0d0+0" },
	{ key = "paladin", name = "paladin",
	  stats = "St:0d0+2 Dx:0d0+0 To:0d0+2 Le:0d0-2 Wi:0d0+1 Ma:0d0-3 Pe:0d0+0 Ch:0d0+0" },
	{ key = "alchemist", name = "alchemist",
	  stats = "St:0d0-2 Dx:0d0-2 To:0d0-2 Le:0d0+3 Wi:0d0+1 Ma:0d0-2 Pe:0d0+2 Ch:0d0+0" },
	{ key = "bard", name = "bard",
	  stats = "St:0d0+1 Dx:0d0+1 To:0d0 Le:0d0-2 Wi:0d0 Ma:0d0 Pe:0d0 Ch:0d0+0" },
}


HERO_GENDERS = {
	{ key = "male", name = "male",
	  stats = "St:0d0+1 Dx:0d0+0 To:0d0+0 Le:0d0+0 Wi:0d0+0 Ma:0d0+0 Pe:0d0+0 Ch:0d0+0" },
	{ key = "female", name = "female",
	  stats = "St:0d0+0 Dx:0d0+1 To:0d0+0 Le:0d0+0 Wi:0d0+0 Ma:0d0+0 Pe:0d0+0 Ch:0d0+0" },
}


local function Find(list, key)
	for _, entry in ipairs(list) do
		if (entry.key == key) then
			return entry
		end
	end
end


-- What the engine asks for to build the creation menus. Each race carries
-- the professions open to it, so the whole of the offer arrives in one
-- answer and the engine never has to ask a second time.
function HeroRaces()
	local out = {}

	for i, race in ipairs(HERO_RACES) do
		local professions = {}

		for j, key in ipairs(race.professions) do
			local profession = Find(HERO_PROFESSIONS, key)
			professions[j] = { key = profession.key, name = profession.name }
		end

		out[i] = { key = race.key, name = race.name, professions = professions }
	end

	return out
end


function HeroGenders()
	local out = {}

	for i, gender in ipairs(HERO_GENDERS) do
		out[i] = { key = gender.key, name = gender.name }
	end

	return out
end


-- Builds the character the player asked for. Called by the engine
-- (XHero::PlayerSetup) once all three choices are made, and responsible for
-- the whole of what those choices mean - the figures, the pace, the
-- stomach, what the race simply knows.
--
-- The three stat blocks are added in the order they were chosen, and only
-- then clamped: a race and a profession that disagree sharply can drive a
-- figure below one between them, and nothing may start below one.
function InitHero(hero, race_key, gender_key, profession_key)
	local race = Find(HERO_RACES, race_key)
	local gender = Find(HERO_GENDERS, gender_key)
	local profession = Find(HERO_PROFESSIONS, profession_key)

	SetMoveEnergy(hero, race.speed)
	SetStats(hero, race.stats)
	SetMaxStats(hero, race.max_stats)
	SetFoodFeeling(hero, race.food)

	for _, skill in ipairs(race.skills) do
		LearnSkill(hero, skill, 1)
	end

	-- Everyone can bind a wound.
	LearnSkill(hero, XSkill.FIRST_AID, 1)

	AddStats(hero, gender.stats)
	AddStats(hero, profession.stats)
	ClampStats(hero)
end
