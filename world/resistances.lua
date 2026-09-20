-- Resistances: the ways a creature or an item can be hardened against harm.
--
-- Which ones exist is content. A world with radiation in it declares a
-- resistance to radiation here, and the engine never has to hear the word -
-- it only ever asks "how much of this does the target have".
--
-- The id is what a declaration spells. Anything that can be resistant -
-- Monster.new, Item.new, Enchantment.new, Material.new and
-- ArmourEnchantment.new - takes :Resist() in either of two forms:
--
--   :Resist{ fire = "5d5+25", see_invisible = true }   a table
--   :Resist("fire:5d5+25 see_invisible:0d0+1")         one string
--
-- The table is the one to write. It says true or false for the flags
-- below and dice for the rest, and it is checked as the world loads: a
-- name no row declares, dice given to a flag, or true given to a matter of
-- degree are each reported by the creature or item that said it. The
-- string form takes the same names and is checked not at all, so a typo in
-- one is simply a resistance you never get.
--
--   Resistance.new(id)
--       :Called(name)     what the Resistances screen shows
--       :Gained(text)     what the body notices on gaining some. Unsaid,
--                         nothing is said - which is how thirteen of these
--                         behaved when the text lived in C++
--       :Lost(text)       and on losing some
--       :Flag()           one held yes or no, with no degree to it. The
--                         screen names it rather than grading it, and
--                         anything asking about it asks only whether
--                         there is any
--       :Register()
--
-- Most of these are quantities: more resistance to fire is less harm from
-- fire, and the sheet grades what you have in words.
-- Two are not: invisibile and see_invisible.

Resistance.new("white")
	:Called("White magic")
	:Register()

Resistance.new("black")
	:Called("Black magic")
	:Register()

Resistance.new("fire")
	:Called("Fire magic")
	:Gained("Your blood cools down!")
	:Lost("Your blood warms up!")
	:Register()

Resistance.new("water")
	:Called("Water magic")
	:Register()

Resistance.new("air")
	:Called("Air magic")
	:Register()

Resistance.new("earth")
	:Called("Earth magic")
	:Register()

Resistance.new("acid")
	:Called("Acid")
	:Gained("Your stomach settles!")
	:Lost("You feel a pain in your stomach!")
	:Register()

Resistance.new("cold")
	:Called("Cold")
	:Gained("Your skin grows warm!")
	:Lost("Your skin grows cold!")
	:Register()

Resistance.new("poison")
	:Called("Poison")
	:Gained("Your flesh tingles!")
	:Lost("You feel vulnerable!")
	:Register()

Resistance.new("disease")
	:Called("Disease")
	:Register()

Resistance.new("paralyse")
	:Called("Paralyzation")
	:Gained("Your movements grow stronger!")
	:Lost("Your movements are unsure!")
	:Register()

Resistance.new("stun")
	:Called("Stun")
	:Register()

Resistance.new("confuse")
	:Called("Confusion")
	:Register()

Resistance.new("blind")
	:Called("Blindness")
	:Register()

Resistance.new("light")
	:Called("Light")
	:Register()

Resistance.new("darkness")
	:Called("Darkness")
	:Register()

Resistance.new("invisible")
	:Called("Invisible")
	:Flag()
	:Register()

Resistance.new("see_invisible")
	:Called("See Invisible")
	:Flag()
	:Register()
