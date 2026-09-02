-- Hats and helmets.
--
-- Template.new(kind, type) is documented in world/items/init.lua.
--

Template.new(ItemKind.HAT, ItemType.HAT)
	:View("hat", '[')
	:Made(ItemSet.SOFT, ItemQuality.POOR)
	:Worth(1, 1)
	:Armour("1d1", "0d0")
	:Combat("", "1d1", "")
	:Chance(100)
	:Register()

Template.new(ItemKind.HAT, ItemType.CAP)
	:View("cap", '[')
	:Made(ItemSet.ALLLEATHER, ItemQuality.AVG)
	:Worth(2, 2)
	:Armour("1d2+1", "1d2")
	:Combat("", "1d1", "")
	:Chance(50)
	:Register()

Template.new(ItemKind.HAT, ItemType.HELMET)
	:View("helmet", '[')
	:Made(ItemSet.HARDMETAL, ItemQuality.FAIR)
	:Worth(3, 3)
	:Armour("1d3+2", "1d3+1")
	:Combat("", "1d3", "")
	:Chance(10)
	:Register()
