-- What a ring or an amulet can be.
--
-- A ring is nothing but its enchantment, so the sorts below are the whole
-- list of rings and amulets in the game. Both wear the same list: a ring of
-- Strength and an amulet of Strength are the same discovery.
--
--   Enchantment.new(id)
--       :Called(name)         what it reads as once somebody knows it
--       :Armour(dv, pv)       dice strings; omitted when it protects nothing
--       :Combat(hit, dice, extra)
--       :Range(dice)
--       :Resist(text)         the same dice strings Monster.new takes
--       :Stats(text)
--       :Worth(value)
--       :Register()
--
-- Until it is identified, one reads by its look alone - "a ruby ring". The
-- looks are dealt out afresh each game, one to a sort, so learning that the
-- white ring was protection tells you nothing about your next game.

EnchantmentLooks({
	{ "white", xColor.xWHITE },
	{ "blue", xColor.xBLUE },
	{ "black", xColor.xDARKGRAY },
	{ "golden", xColor.xYELLOW },
	{ "copper", xColor.xRED },
	{ "wooden", xColor.xBROWN },
	{ "red", xColor.xRED },
	{ "marble", xColor.xWHITE },
	{ "pearl", xColor.xYELLOW },
	{ "sapphire", xColor.xLIGHTGREEN },
	{ "diamond", xColor.xCYAN },
	{ "ruby", xColor.xRED },
	{ "steel", xColor.xBLUE },
	{ "glass", xColor.xLIGHTGRAY },
	{ "obsidian", xColor.xDARKGRAY },
})

Enchantment.new("protection")
	:Called("of protection")
	:Armour("", "1d6-2")
	:Worth(150)
	:Register()

Enchantment.new("damage")
	:Called("of damage")
	:Combat("", "", "1d6-2")
	:Worth(150)
	:Register()

Enchantment.new("slaying")
	:Called("of slaying")
	:Combat("1d6-2", "", "1d6-2")
	:Worth(300)
	:Register()

Enchantment.new("free_action")
	:Called("of free action")
	:Resist("stun:8d5+50 confuse:8d5+50")
	:Worth(200)
	:Register()

Enchantment.new("invisibility")
	:Called("of invisibility")
	:Resist("invisible:0d0+10")
	:Worth(500)
	:Register()

Enchantment.new("see_invisible")
	:Called("of see invisible")
	:Resist("see_invisible:0d0+10")
	:Worth(300)
	:Register()

Enchantment.new("fire_resistance")
	:Called("of fire resistance")
	:Resist("fire:8d5+30")
	:Worth(250)
	:Register()

Enchantment.new("acid_resistance")
	:Called("of acid resistance")
	:Resist("acid:8d5+25")
	:Worth(300)
	:Register()

Enchantment.new("poison_resistance")
	:Called("of poison resistance")
	:Resist("poison:8d5+25")
	:Worth(300)
	:Register()

Enchantment.new("strength")
	:Called("of Strength")
	:Stats("St:1d4")
	:Worth(400)
	:Register()

Enchantment.new("power")
	:Called("of Power")
	:Stats("Wi:1d4")
	:Worth(400)
	:Register()
