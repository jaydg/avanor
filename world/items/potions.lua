-- Potions.
--
-- Most potions are simply an effect in a bottle: the row names the effect
-- and the engine makes it happen. The rest - the drinks, the ones that
-- shift a stat or lay a curse - say what they do in an :OnDrink() handler
-- below, because there is no effect that describes them.
--
--   Potion.new(id)
--       :Called(name)          what it reads as once somebody knows it
--       :Effect(id)            what drinking it does, from effects.lua.
--                              Unsaid, it wants an
--                              :OnDrink() handler instead
--       :Chance(rarity)        its weight in the draw against other potions
--       :Worth(value)
--       :Alchemy(power)        which rung of the alchemy ladder it sits on.
--                              Two potions of one rung mix into one of the
--                              rung above, and how deep the ladder goes is
--                              however deep these numbers go. Also how hard
--                              the alchemy set finds it to distil
--       :Looks(id)             its appearance, when it must be a particular
--                              one (water is clear) - a row from
--                              potion_colours.lua. Unsaid, it takes an
--                              appearance no other potion has this game
--       :OnDrink(function)     called as handler(id, drinker) when the
--                              potion names no effect. Answer false if
--                              nothing actually happened
--       :Register()
--
-- Until it is identified a potion reads by its colour alone, and the
-- colours are dealt out afresh each game.


-- What a stat potion moves, and by how much.
local STAT_CHANGE = {
	strength   = { XStats.STR,  1 },
	toughness  = { XStats.TOU,  1 },
	willpower  = { XStats.WIL,  1 },
	swiftness  = { XStats.DEX,  1 },
	mana       = { XStats.MAN,  1 },
	weakness   = { XStats.STR, -1 },
	clumsiness = { XStats.DEX, -1 },
}

-- What a curse-in-a-bottle lays on the drinker, and how strongly.
local MODIFIERS = {
	boost_speed = { Modifier.BOOST_SPEED, 100, nil,               "moves more quickly!" },
	slowness    = { Modifier.SLOWNESS,    100, nil,               "moves slowly!" },
	bleeding    = { Modifier.WOUND,        30, "You begin to bleed.", "starts to bleed." },
	disease     = { Modifier.DISEASE,      25, nil,               "looks ill." },
	poison      = { Modifier.POISON,       10, nil,               "is poisoned." },
}

function PotionQuench(id, drinker)
	if (isHero(drinker)) then
		AddMessage("You feel less thirsty.")
	elseif (isCreatureVisible(drinker)) then
		AddMessage(CreatureName(drinker, 0))
		AddMessage("looks less thirsty.")
	end

	return true
end

function PotionChangeStat(id, drinker)
	local change = STAT_CHANGE[id]

	-- A stat already at its floor or ceiling does not move, and a potion
	-- that moved nothing did nothing.
	return ChangeStats(drinker, change[1], change[2]) ~= 0
end

function PotionModifier(id, drinker)
	local m = MODIFIERS[id]

	AddModifier(drinker, m[1], m[2])

	if (isHero(drinker)) then
		if (m[3]) then
			AddMessage(m[3])
		end
	elseif (isCreatureVisible(drinker)) then
		AddMessage(CreatureName(drinker, 0))
		AddMessage(m[4])
	end

	return true
end

function PotionSatiation(id, drinker)
	ChangeNutrition(drinker, 7)

	if (isHero(drinker)) then
		AddMessage("You feel much fuller!")
	elseif (isCreatureVisible(drinker)) then
		AddMessage(CreatureName(drinker, 0))
		AddMessage("looks full!")
	end

	return true
end

function PotionStarvation(id, drinker)
	SetNutrition(drinker, 3)

	if (isHero(drinker)) then
		AddMessage("You feel hungrier!")
	elseif (isCreatureVisible(drinker)) then
		AddMessage(CreatureName(drinker, 0))
		AddMessage("looks very hungry!")
	end

	return true
end

-- This one kills anyone who drinks it, which is why it is the rarest thing
-- in the table. A creature with a name of its own is too wary to swallow
-- it and throws it away instead.
function PotionDeath(id, drinker)
	if (isHero(drinker)) then
		AddMessage("You feel your life draining away very rapidly!")
	elseif (isCreatureVisible(drinker)) then
		if (isUniqueCreature(drinker)) then
			AddMessage(CreatureName(drinker, 0))
			AddMessage("seems to change")
			AddMessage(CreatureName(drinker, 3))
			AddMessage("mind and throws the potion away!")
			return true
		end

		AddMessage(CreatureName(drinker, 0))
		AddMessage("seems to be dying!")
	end

	ChangeStats(drinker, XStats.STR, -1)  -- Weakness
	ChangeStats(drinker, XStats.DEX, -1)  -- Damage
	ChangeStats(drinker, XStats.TOU, -1)  -- Fatigue
	ChangeStats(drinker, XStats.LEN, -1)  -- Can't learn if you're dead
	ChangeStats(drinker, XStats.WIL, -1)  -- Lost the will to live
	ChangeStats(drinker, XStats.MAN, -1)  -- Out of touch with nature
	ChangeStats(drinker, XStats.PER, -1)  -- Senses are useless when dead
	ChangeStats(drinker, XStats.CHR, -1)  -- Rotting is ugly
	AddModifier(drinker, Modifier.WOUND, 100)  -- Ensure death

	return true
end


Potion.new("water")
	:Called("water")
	:Chance(100)
	:Worth(1)
	:Alchemy(1)
	:Looks("clear")
	:OnDrink("PotionQuench")
	:Register()

Potion.new("apple_juice")
	:Called("apple juice")
	:Chance(95)
	:Worth(2)
	:Alchemy(1)
	:Looks("yellow")
	:OnDrink("PotionQuench")
	:Register()

Potion.new("orange_juice")
	:Called("orange juice")
	:Chance(95)
	:Worth(3)
	:Alchemy(1)
	:Looks("orange")
	:OnDrink("PotionQuench")
	:Register()

Potion.new("healing")
	:Called("healing")
	:Effect("heal")
	:Chance(10)
	:Worth(200)
	:Alchemy(4)
	:Looks("white")
	:Register()

Potion.new("cure_light_wounds")
	:Called("cure light wounds")
	:Effect("cure_light_wounds")
	:Chance(80)
	:Worth(15)
	:Alchemy(2)
	:Register()

Potion.new("cure_serious_wounds")
	:Called("cure serious wounds")
	:Effect("cure_serious_wounds")
	:Chance(70)
	:Worth(40)
	:Alchemy(2)
	:Register()

Potion.new("cure_critical_wounds")
	:Called("cure critical wounds")
	:Effect("cure_critical_wounds")
	:Chance(40)
	:Worth(60)
	:Alchemy(3)
	:Register()

Potion.new("cure_mortal_wounds")
	:Called("cure mortal wounds")
	:Effect("cure_mortal_wounds")
	:Chance(20)
	:Worth(100)
	:Alchemy(3)
	:Register()

Potion.new("power")
	:Called("power")
	:Effect("power")
	:Chance(80)
	:Worth(15)
	:Alchemy(2)
	:Register()

Potion.new("restoration")
	:Called("restoration")
	:Effect("restoration")
	:Chance(5)
	:Worth(400)
	:Alchemy(5)
	:Register()

Potion.new("strength")
	:Called("strength")
	:Chance(10)
	:Worth(1000)
	:Alchemy(4)
	:OnDrink("PotionChangeStat")
	:Register()

Potion.new("willpower")
	:Called("willpower")
	:Chance(10)
	:Worth(1000)
	:Alchemy(4)
	:OnDrink("PotionChangeStat")
	:Register()

Potion.new("mana")
	:Called("mana")
	:Chance(10)
	:Worth(1000)
	:Alchemy(4)
	:OnDrink("PotionChangeStat")
	:Register()

Potion.new("toughness")
	:Called("toughness")
	:Chance(10)
	:Worth(1000)
	:Alchemy(4)
	:OnDrink("PotionChangeStat")
	:Register()

Potion.new("swiftness")
	:Called("swiftness")
	:Chance(10)
	:Worth(1000)
	:Alchemy(4)
	:OnDrink("PotionChangeStat")
	:Register()

Potion.new("poison")
	:Called("poison")
	:Chance(150)
	:Worth(5)
	:Alchemy(1)
	:OnDrink("PotionModifier")
	:Register()

Potion.new("cure_poison")
	:Called("cure poison")
	:Effect("cure_poison")
	:Chance(80)
	:Worth(25)
	:Alchemy(3)
	:Register()

Potion.new("bleeding")
	:Called("bleeding")
	:Chance(300)
	:Worth(1)
	:Alchemy(1)
	:OnDrink("PotionModifier")
	:Register()

Potion.new("disease")
	:Called("disease")
	:Chance(200)
	:Worth(1)
	:Alchemy(1)
	:OnDrink("PotionModifier")
	:Register()

Potion.new("cure_disease")
	:Called("cure disease")
	:Effect("cure_disease")
	:Chance(50)
	:Worth(50)
	:Alchemy(3)
	:Register()

Potion.new("heroism")
	:Called("heroism")
	:Effect("heroism")
	:Chance(75)
	:Worth(20)
	:Alchemy(2)
	:Register()

Potion.new("see_invisible")
	:Called("see invisible")
	:Effect("see_invisible")
	:Chance(30)
	:Worth(30)
	:Alchemy(3)
	:Register()

Potion.new("weakness")
	:Called("weakness")
	:Chance(70)
	:Worth(5)
	:Alchemy(1)
	:OnDrink("PotionChangeStat")
	:Register()

Potion.new("clumsiness")
	:Called("clumsiness")
	:Chance(70)
	:Worth(5)
	:Alchemy(1)
	:OnDrink("PotionChangeStat")
	:Register()

Potion.new("death")
	:Called("death")
	:Chance(1)
	:Worth(5)
	:Alchemy(2)
	:OnDrink("PotionDeath")
	:Register()

Potion.new("satiation")
	:Called("satiation")
	:Chance(50)
	:Worth(15)
	:Alchemy(2)
	:OnDrink("PotionSatiation")
	:Register()

Potion.new("starvation")
	:Called("starvation")
	:Chance(40)
	:Worth(15)
	:Alchemy(3)
	:OnDrink("PotionStarvation")
	:Register()

Potion.new("boost_speed")
	:Called("boost speed")
	:Chance(30)
	:Worth(100)
	:Alchemy(3)
	:OnDrink("PotionModifier")
	:Register()

Potion.new("slowness")
	:Called("slowness")
	:Chance(150)
	:Worth(2)
	:Alchemy(1)
	:OnDrink("PotionModifier")
	:Register()

Potion.new("acid_resistance")
	:Called("acid resistance")
	:Effect("acid_resistance")
	:Chance(35)
	:Worth(70)
	:Alchemy(3)
	:Register()

Potion.new("fire_resistance")
	:Called("fire resistance")
	:Effect("fire_resistance")
	:Chance(45)
	:Worth(50)
	:Alchemy(2)
	:Register()

Potion.new("cold_resistance")
	:Called("cold resistance")
	:Effect("cold_resistance")
	:Chance(45)
	:Worth(50)
	:Alchemy(2)
	:Register()

Potion.new("poison_resistance")
	:Called("poison resistance")
	:Effect("poison_resistance")
	:Chance(40)
	:Worth(50)
	:Alchemy(3)
	:Register()
