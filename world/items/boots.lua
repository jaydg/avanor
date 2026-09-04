-- Boots.
--
-- Template.new(kind, type) is documented in world/items/init.lua.
--

Template.new(ItemKind.BOOTS, "sandals")
	:View("sandals", ']')
	:Made(ItemSet.ALLLEATHER, ItemQuality.POOR)
	:Worth(1, 1)
	:Armour("1d2", "0d0")
	:Combat("", "1d1", "")
	:Chance(150)
	:Register()

Template.new(ItemKind.BOOTS, "light_boots")
	:View("light boots", ']')
	:Made(ItemSet.ALLLEATHER, ItemQuality.AVG)
	:Worth(2, 2)
	:Armour("1d3", "1d1")
	:Combat("", "1d1", "")
	:Chance(80)
	:Register()

Template.new(ItemKind.BOOTS, "soft_boots")
	:View("soft boots", ']')
	:Made(ItemSet.ALLLEATHER, ItemQuality.FAIR)
	:Worth(3, 3)
	:Armour("1d4", "1d2")
	:Combat("", "1d1", "")
	:Chance(40)
	:Register()

Template.new(ItemKind.BOOTS, "hard_boots")
	:View("hard boots", ']')
	:Made(ItemSet.OBSIMETAL, ItemQuality.GOOD)
	:Worth(5, 5)
	:Armour("1d3", "1d2+1")
	:Combat("", "1d1", "")
	:Chance(10)
	:Register()
