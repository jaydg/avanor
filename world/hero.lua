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


-- Who the character may be, in menu order. Each names the pronoun the game
-- should use for them - not the engine assuming that the first entry in the
-- menu means male - so this list can be reordered, or added to, freely.
--
-- Only the pronoun: that the player is spoken to as "you" is a fact about
-- playing the game rather than about the world, so the engine adds it.
HERO_GENDERS = {
	{ key = "male", name = "male", pronoun = PersonType.HE,
	  stats = "St:0d0+1 Dx:0d0+0 To:0d0+0 Le:0d0+0 Wi:0d0+0 Ma:0d0+0 Pe:0d0+0 Ch:0d0+0" },
	{ key = "female", name = "female", pronoun = PersonType.SHE,
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
		out[i] = { key = gender.key, name = gender.name, pronoun = gender.pronoun }
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

	local who = AsCreature(hero)
	local kit = HERO_KITS[profession_key]

	if (kit) then
		kit(hero, who, race_key)
	end

	-- Whatever ended up in the hero's hand, they begin competent with it.
	-- After the kit, necessarily: it reads what the kit put there.
	local weapon = GetWornItem(hero, BodyPart.HAND, 0)

	if (weapon and BinaryAND(AsItem(weapon).kind, ItemKind.WEAPON)) then
		SetWarSkill(hero, GetItemWarSkill(weapon), 2)
	end

	-- Nobody sets out unable to spot a tripwire, lay one, or climb out of
	-- a pit.
	LearnSkill(hero, XSkill.DETECTTRAP, 1)
	LearnSkill(hero, XSkill.CREATETRAP, 1)
	LearnSkill(hero, XSkill.CLIMBING, 1)

	-- And nobody sets out naked.
	if (Rand(2) == 0) then
		who:PutOnBody(BodyPart.BODY, 0,
			CreateObject(ItemKind.BODY, "clothes", 1, 100))
	else
		who:PutOnBody(BodyPart.BODY, 0,
			CreateObject(ItemKind.BODY, "robe", 1, 100))
	end

	-- A little money and a day's food. The purse is topped up again in
	-- OnHeroPlaced, which is deliberate for now: that second, larger sum is
	-- there to make testing comfortable and goes away before release, and
	-- keeping the two apart means removing it will not disturb this one.
	MoneyOperation(hero, 15 + Rand(10))

	local ration = CreateObject("ration")

	if (ration) then
		who:ContainItem(AsItem(ration))
	end
end


-- What each calling begins with, by race where it differs. Translated from
-- the switch this used to be in creature/xhero2.cpp; the order of what it
-- creates is preserved exactly, because the material and the enchantment of
-- every piece are rolled as it is made.
HERO_KITS = {}

HERO_KITS["warrior"] = function(hero, who, race)
	local item, potion, scroll, book, tool
	if (race == "human") then
		item = CreateObject(ItemKind.WEAPON, "long_sword", 1, 100)
		who:PutOnBody(BodyPart.HAND, 0, item)
		item = CreateObject(ItemKind.SHIELD, "small_shield", 1, 100)
		who:PutOnBody(BodyPart.HAND, 1, item)
	elseif (race == "half_elf") then
		item = CreateObject(ItemKind.WEAPON, "rapier", 1, 100)
		who:PutOnBody(BodyPart.HAND, 0, item)
		item = CreateObject(ItemKind.BOOTS, "sandals", 1, 20)
		who:PutOnBody(BodyPart.BOOTS, 0, item)
	elseif (race == "high_elf") then
		item = CreateObject(ItemKind.WEAPON, "rapier", 1, 100)
		who:PutOnBody(BodyPart.HAND, 0, item)
		item = CreateObject(ItemKind.CLOAK, "light_cloak", 1, 50)
		who:PutOnBody(BodyPart.CLOAK, 0, item)
		item = CreateObject(ItemKind.BOOTS, "sandals", 1, 20)
		who:PutOnBody(BodyPart.BOOTS, 0, item)
	elseif (race == "halfling") then
		item = CreateObject(ItemKind.WEAPON, "short_sword", 1, 100)
		who:PutOnBody(BodyPart.HAND, 0, item)
		item = CreateObject(ItemKind.CLOAK, "light_cloak", 1, 50)
		who:PutOnBody(BodyPart.CLOAK, 0, item)
		item = CreateObject(ItemKind.BOOTS, "light_boots", 1, 40)
		who:PutOnBody(BodyPart.BOOTS, 0, item)
		LearnSkill(hero, XSkill.DISARMTRAP, 1)
	elseif (race == "half_orc") then
		item = CreateObject(ItemKind.WEAPON, "orcish_axe", 1, 100)
		who:PutOnBody(BodyPart.HAND, 0, item)
		item = CreateObject(ItemKind.SHIELD, "medium_shield", 1, 100)
		who:PutOnBody(BodyPart.HAND, 1, item)
	elseif (race == "dwarf") then
		item = CreateObject(ItemKind.WEAPON, "battle_axe", 1, 100)
		who:PutOnBody(BodyPart.HAND, 0, item)
		item = CreateObject(ItemKind.SHIELD, "medium_shield", 1, 100)
		who:PutOnBody(BodyPart.HAND, 1, item)
	elseif (race == "gnome") then
		item = CreateObject(ItemKind.WEAPON, "war_axe", 1, 100)
		who:PutOnBody(BodyPart.HAND, 0, item)
		item = CreateObject(ItemKind.SHIELD, "small_shield", 1, 100)
		who:PutOnBody(BodyPart.HAND, 1, item)
	end
	potion = CreateObject(PotionName.CURE_LIGHT_WOUNDS)
	Identify(potion)
	who:ContainItem(AsItem(potion))
	potion = CreateObject(PotionName.HEROISM)
	Identify(potion)
	who:ContainItem(AsItem(potion))
	LearnSkill(hero, XSkill.FINDWEAKNESS, 1)
	LearnSkill(hero, XSkill.HEALING, 1)
	LearnSkill(hero, XSkill.TACTICS, 1)
	LearnSkill(hero, XSkill.ATHLETICS, 1)
end

HERO_KITS["wizard"] = function(hero, who, race)
	local item, potion, scroll, book, tool
	item = CreateObject(ItemKind.WEAPON, "staff", 1, 100)
	who:PutOnBody(BodyPart.HAND, 0, item)
	potion = CreateObject(PotionName.POWER)
	Identify(potion)
	who:ContainItem(AsItem(potion))
	for t = 1, 2 do
		if (Rand() % 2 == 1) then
			scroll = CreateScroll("fire_bolt")
		else
			scroll = CreateScroll("ice_bolt")
		end
		Identify(scroll)
		who:ContainItem(AsItem(scroll))
	end
	if (Rand() % 2 == 1) then
		book = CreateBook(BookName.FIRE_BOLT)
	else
		book = CreateBook(BookName.ICE_BOLT)
	end
	Identify(book)
	who:ContainItem(AsItem(book))
	book = CreateObject(ItemKind.BOOK, 0, 10000)
	Identify(book)
	who:ContainItem(AsItem(book))
	LearnSkill(hero, XSkill.HEALING, 1)
	LearnSkill(hero, XSkill.LITERACY, 1)
	LearnSkill(hero, XSkill.CONCENTRATION, 1)
	LearnSkill(hero, XSkill.HERBALISM, 1)
end

HERO_KITS["archer"] = function(hero, who, race)
	local item, potion, scroll, book, tool
	if (race == "human") then
		item = CreateObject(ItemKind.MISSILEW, "crossbow", 1, 100)
		who:PutOnBody(BodyPart.MISSILE_WEAPON, 0, item)
		item = CreateObject(ItemKind.MISSILE, "quarrel", 1, 100)
		who:PutOnBody(BodyPart.MISSILE, 0, item)
		AsItem(item).quantity = Rand() % 10 + 10
		item = CreateObject(ItemKind.WEAPON, "dagger", 1, 100)
		who:PutOnBody(BodyPart.HAND, 0, item)
	elseif (race == "half_elf") then
		item = CreateObject(ItemKind.MISSILEW, "long_bow", 1, 100)
		who:PutOnBody(BodyPart.MISSILE_WEAPON, 0, item)
		item = CreateObject(ItemKind.MISSILE, "arrow", 1, 100)
		who:PutOnBody(BodyPart.MISSILE, 0, item)
		AsItem(item).quantity = Rand() % 10 + 10
		item = CreateObject(ItemKind.WEAPON, "dagger", 1, 100)
		who:PutOnBody(BodyPart.HAND, 0, item)
		item = CreateObject(ItemKind.BOOTS, "sandals", 1, 20)
		who:PutOnBody(BodyPart.BOOTS, 0, item)
	elseif (race == "high_elf") then
		item = CreateObject(ItemKind.MISSILEW, "long_bow", 1, 100)
		who:PutOnBody(BodyPart.MISSILE_WEAPON, 0, item)
		item = CreateObject(ItemKind.MISSILE, "arrow", 1, 100)
		who:PutOnBody(BodyPart.MISSILE, 0, item)
		AsItem(item).quantity = Rand() % 10 + 10
		item = CreateObject(ItemKind.WEAPON, "rapier", 1, 100)
		who:PutOnBody(BodyPart.HAND, 0, item)
		item = CreateObject(ItemKind.CLOAK, "light_cloak", 1, 50)
		who:PutOnBody(BodyPart.CLOAK, 0, item)
		item = CreateObject(ItemKind.BOOTS, "sandals", 1, 20)
		who:PutOnBody(BodyPart.BOOTS, 0, item)
	elseif (race == "halfling") then
		item = CreateObject(ItemKind.MISSILEW, "sling", 1, 100)
		who:PutOnBody(BodyPart.MISSILE_WEAPON, 0, item)
		item = CreateObject(ItemKind.MISSILE, "sling_bullet", 1, 100)
		who:PutOnBody(BodyPart.MISSILE, 0, item)
		AsItem(item).quantity = Rand() % 10 + 10
		item = CreateObject(ItemKind.CLOAK, "light_cloak", 1, 50)
		who:PutOnBody(BodyPart.CLOAK, 0, item)
		item = CreateObject(ItemKind.BOOTS, "light_boots", 1, 40)
		who:PutOnBody(BodyPart.BOOTS, 0, item)
	elseif (race == "half_orc") then
		item = CreateObject(ItemKind.MISSILEW, "heavy_crossbow", 1, 100)
		who:PutOnBody(BodyPart.MISSILE_WEAPON, 0, item)
		item = CreateObject(ItemKind.MISSILE, "quarrel", 1, 100)
		who:PutOnBody(BodyPart.MISSILE, 0, item)
		AsItem(item).quantity = Rand() % 10 + 10
		item = CreateObject(ItemKind.WEAPON, "orcish_dagger", 1, 100)
		who:PutOnBody(BodyPart.HAND, 0, item)
	elseif (race == "dwarf") then
		item = CreateObject(ItemKind.MISSILEW, "heavy_crossbow", 1, 100)
		who:PutOnBody(BodyPart.MISSILE_WEAPON, 0, item)
		item = CreateObject(ItemKind.MISSILE, "quarrel", 1, 100)
		who:PutOnBody(BodyPart.MISSILE, 0, item)
		AsItem(item).quantity = Rand() % 10 + 10
		item = CreateObject(ItemKind.WEAPON, "war_axe", 1, 100)
		who:PutOnBody(BodyPart.HAND, 0, item)
	elseif (race == "gnome") then
		item = CreateObject(ItemKind.MISSILEW, "light_crossbow", 1, 100)
		who:PutOnBody(BodyPart.MISSILE_WEAPON, 0, item)
		item = CreateObject(ItemKind.MISSILE, "quarrel", 1, 100)
		who:PutOnBody(BodyPart.MISSILE, 0, item)
		AsItem(item).quantity = Rand() % 10 + 10
		item = CreateObject(ItemKind.WEAPON, "war_axe", 1, 100)
		who:PutOnBody(BodyPart.HAND, 0, item)
	end
	potion = CreateObject(PotionName.CURE_LIGHT_WOUNDS)
	Identify(potion)
	who:ContainItem(AsItem(potion))
	LearnSkill(hero, XSkill.FINDWEAKNESS, 1)
	LearnSkill(hero, XSkill.HEALING, 1)
	LearnSkill(hero, XSkill.ARCHERY, 1)
	LearnSkill(hero, XSkill.ATHLETICS, 1)
end

HERO_KITS["ranger"] = function(hero, who, race)
	local item, potion, scroll, book, tool
	if (race == "human") then
		item = CreateObject(ItemKind.MISSILEW, "crossbow", 1, 100)
		who:PutOnBody(BodyPart.MISSILE_WEAPON, 0, item)
		item = CreateObject(ItemKind.MISSILE, "quarrel", 1, 100)
		who:PutOnBody(BodyPart.MISSILE, 0, item)
		AsItem(item).quantity = Rand() % 10 + 10
		item = CreateObject(ItemKind.WEAPON, "dagger", 1, 100)
		who:PutOnBody(BodyPart.HAND, 0, item)
	elseif (race == "half_elf") then
		item = CreateObject(ItemKind.MISSILEW, "long_bow", 1, 100)
		who:PutOnBody(BodyPart.MISSILE_WEAPON, 0, item)
		item = CreateObject(ItemKind.MISSILE, "arrow", 1, 100)
		who:PutOnBody(BodyPart.MISSILE, 0, item)
		AsItem(item).quantity = Rand() % 10 + 10
		item = CreateObject(ItemKind.WEAPON, "dagger", 1, 100)
		who:PutOnBody(BodyPart.HAND, 0, item)
		item = CreateObject(ItemKind.BOOTS, "sandals", 1, 20)
		who:PutOnBody(BodyPart.BOOTS, 0, item)
	elseif (race == "high_elf") then
		item = CreateObject(ItemKind.MISSILEW, "long_bow", 1, 100)
		who:PutOnBody(BodyPart.MISSILE_WEAPON, 0, item)
		item = CreateObject(ItemKind.MISSILE, "arrow", 1, 100)
		who:PutOnBody(BodyPart.MISSILE, 0, item)
		AsItem(item).quantity = Rand() % 10 + 10
		item = CreateObject(ItemKind.WEAPON, "rapier", 1, 100)
		who:PutOnBody(BodyPart.HAND, 0, item)
		item = CreateObject(ItemKind.CLOAK, "light_cloak", 1, 50)
		who:PutOnBody(BodyPart.CLOAK, 0, item)
		item = CreateObject(ItemKind.BOOTS, "sandals", 1, 20)
		who:PutOnBody(BodyPart.BOOTS, 0, item)
	elseif (race == "halfling") then
		item = CreateObject(ItemKind.MISSILEW, "sling", 1, 100)
		who:PutOnBody(BodyPart.MISSILE_WEAPON, 0, item)
		item = CreateObject(ItemKind.MISSILE, "sling_bullet", 1, 100)
		who:PutOnBody(BodyPart.MISSILE, 0, item)
		AsItem(item).quantity = Rand() % 10 + 10
		item = CreateObject(ItemKind.WEAPON, "long_dagger", 1, 100)
		who:PutOnBody(BodyPart.HAND, 0, item)
		item = CreateObject(ItemKind.CLOAK, "light_cloak", 1, 50)
		who:PutOnBody(BodyPart.CLOAK, 0, item)
		item = CreateObject(ItemKind.BOOTS, "light_boots", 1, 40)
		who:PutOnBody(BodyPart.BOOTS, 0, item)
	elseif (race == "half_orc") then
		item = CreateObject(ItemKind.MISSILEW, "heavy_crossbow", 1, 100)
		who:PutOnBody(BodyPart.MISSILE_WEAPON, 0, item)
		item = CreateObject(ItemKind.MISSILE, "quarrel", 1, 100)
		who:PutOnBody(BodyPart.MISSILE, 0, item)
		AsItem(item).quantity = Rand() % 10 + 10
		item = CreateObject(ItemKind.WEAPON, "orcish_dagger", 1, 100)
		who:PutOnBody(BodyPart.HAND, 0, item)
	elseif (race == "dwarf") then
		item = CreateObject(ItemKind.MISSILEW, "heavy_crossbow", 1, 100)
		who:PutOnBody(BodyPart.MISSILE_WEAPON, 0, item)
		item = CreateObject(ItemKind.MISSILE, "quarrel", 1, 100)
		who:PutOnBody(BodyPart.MISSILE, 0, item)
		AsItem(item).quantity = Rand() % 10 + 10
		item = CreateObject(ItemKind.WEAPON, "war_axe", 1, 100)
		who:PutOnBody(BodyPart.HAND, 0, item)
	elseif (race == "gnome") then
		item = CreateObject(ItemKind.MISSILEW, "light_crossbow", 1, 100)
		who:PutOnBody(BodyPart.MISSILE_WEAPON, 0, item)
		item = CreateObject(ItemKind.MISSILE, "quarrel", 1, 100)
		who:PutOnBody(BodyPart.MISSILE, 0, item)
		AsItem(item).quantity = Rand() % 10 + 10
		item = CreateObject(ItemKind.WEAPON, "war_axe", 1, 100)
		who:PutOnBody(BodyPart.HAND, 0, item)
	end
	potion = CreateObject(PotionName.CURE_LIGHT_WOUNDS)
	Identify(potion)
	who:ContainItem(AsItem(potion))
	book = CreateBook(BookName.MAGIC_ARROW)
	Identify(book)
	who:ContainItem(AsItem(book))
	LearnSkill(hero, XSkill.FINDWEAKNESS, 1)
	LearnSkill(hero, XSkill.ARCHERY, 1)
	LearnSkill(hero, XSkill.CONCENTRATION, 1)
	LearnSkill(hero, XSkill.LITERACY, 1)
	LearnSkill(hero, XSkill.COOKING, 1)
	LearnSkill(hero, XSkill.ATHLETICS, 1)
end

HERO_KITS["cleric"] = function(hero, who, race)
	local item, potion, scroll, book, tool
	item = CreateObject(ItemKind.WEAPON, "mace", 10, 150)
	who:PutOnBody(BodyPart.HAND, 0, item)
	item = CreateObject(ItemKind.SHIELD, "small_shield", 10, 150)
	who:PutOnBody(BodyPart.HAND, 1, item)
	scroll = CreateScroll("blink")
	Identify(scroll)
	who:ContainItem(AsItem(scroll))
	scroll = CreateScroll("heroism")
	Identify(scroll)
	who:ContainItem(AsItem(scroll))
	potion = CreateObject(PotionName.CURE_LIGHT_WOUNDS)
	Identify(potion)
	who:ContainItem(AsItem(potion))
	LearnSkill(hero, XSkill.HEALING, 1)
	LearnSkill(hero, XSkill.LITERACY, 1)
	LearnSkill(hero, XSkill.HERBALISM, 1)
	LearnSkill(hero, XSkill.RELIGION, 1)
	if (race == "human") then
	elseif (race == "half_orc") then
		who.religion.death_act = 200
	elseif (race == "half_elf") then
	elseif (race == "high_elf") then
	elseif (race == "halfling") then
	elseif (race == "dwarf") then
	elseif (race == "gnome") then
		who.religion.life_act = 200
	end
end

HERO_KITS["paladin"] = function(hero, who, race)
	local item, potion, scroll, book, tool
	item = CreateObject(ItemKind.WEAPON, "mace", 10, 150)
	who:PutOnBody(BodyPart.HAND, 0, item)
	item = CreateObject(ItemKind.SHIELD, "large_shield", 10, 150)
	who:PutOnBody(BodyPart.HAND, 1, item)
	scroll = CreateScroll("blink")
	Identify(scroll)
	who:ContainItem(AsItem(scroll))
	scroll = CreateScroll("heroism")
	Identify(scroll)
	who:ContainItem(AsItem(scroll))
	potion = CreateObject(PotionName.CURE_LIGHT_WOUNDS)
	Identify(potion)
	who:ContainItem(AsItem(potion))
	LearnSkill(hero, XSkill.HEALING, 1)
	LearnSkill(hero, XSkill.LITERACY, 1)
	LearnSkill(hero, XSkill.RELIGION, 1)
	LearnSkill(hero, XSkill.ATHLETICS, 1)
	if (race == "human") then
	elseif (race == "half_orc") then
		who.religion.death_act = 100
	elseif (race == "half_elf") then
	elseif (race == "high_elf") then
	elseif (race == "halfling") then
	elseif (race == "dwarf") then
	elseif (race == "gnome") then
		who.religion.life_act = 100
	end
end

HERO_KITS["alchemist"] = function(hero, who, race)
	local item, potion, scroll, book, tool
	item = CreateObject(ItemKind.WEAPON, "dagger", 10, 150)
	who:PutOnBody(BodyPart.HAND, 0, item)
	scroll = CreateScroll("blink")
	Identify(scroll)
	who:ContainItem(AsItem(scroll))
	scroll = CreateScroll("recipe")
	Identify(scroll)
	who:ContainItem(AsItem(scroll))
	scroll = CreateScroll("recipe")
	Identify(scroll)
	who:ContainItem(AsItem(scroll))
	potion = CreateObject(PotionName.CURE_LIGHT_WOUNDS)
	Identify(potion)
	who:ContainItem(AsItem(potion))
	potion = CreateObject(PotionName.ORANGEJUCE)
	Identify(potion)
	who:ContainItem(AsItem(potion))
	potion = CreateObject(PotionName.APPLEJUCE)
	Identify(potion)
	who:ContainItem(AsItem(potion))
	potion = CreateObject(PotionName.WATER)
	Identify(potion)
	who:ContainItem(AsItem(potion))
	tool = CreateObject('XAlchemySet')
	who:ContainItem(AsItem(tool))
	LearnSkill(hero, XSkill.HEALING, 1)
	LearnSkill(hero, XSkill.LITERACY, 1)
	LearnSkill(hero, XSkill.HERBALISM, 1)
	LearnSkill(hero, XSkill.ALCHEMY, 1)
end

HERO_KITS["bard"] = function(hero, who, race)
	local item, potion, scroll, book, tool
	item = CreateObject(ItemKind.WEAPON, "club", 10, 150)
	who:PutOnBody(BodyPart.HAND, 0, item)
	potion = CreateObject(PotionName.CURE_LIGHT_WOUNDS)
	Identify(potion)
	who:ContainItem(AsItem(potion))
	LearnSkill(hero, XSkill.HEALING, 1)
	LearnSkill(hero, XSkill.LITERACY, 1)
	LearnSkill(hero, XSkill.HERBALISM, 1)
	LearnSkill(hero, XSkill.ALCHEMY, 1)
end


-- Called once the hero is standing in the world, which InitHero cannot wait
-- for: it runs while the character is still being made and there is nowhere
-- to put anything yet.
--
--   hero        the new hero, now on the map
--   race        the race key they were built with
--   profession  the profession key
function OnHeroPlaced(hero, race, profession)
	-- Everyone sets out with a purse.
	MoneyOperation(hero, 2000)

	if (profession ~= "bard") then
		return
	end

	-- A bard travels with a dog. Only the eight cells around the hero are
	-- considered, and they can legitimately all be taken - then the bard
	-- simply starts without it.
	local dog = CreatureNear(hero, "dog")

	if (not dog) then
		return
	end

	AsCreature(dog).xai:SetCompanion(AsCreature(hero))
	SetAIFlag(dog, XStandardAI.ALLOW_MOVE_OUT + XStandardAI.PEACEFUL)
	SetEnemy(dog, CreatureClass.KOBOLD + CreatureClass.GOBLIN + CreatureClass.UNDEAD
		+ CreatureClass.INSECT + CreatureClass.BLOB + CreatureClass.CANINE
		+ CreatureClass.FELINE + CreatureClass.RAT + CreatureClass.REPTILE
		+ CreatureClass.ORC)
end
