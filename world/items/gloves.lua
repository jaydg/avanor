-- Gloves and gauntlets.
--
-- Template.new(kind, type) is documented in world/items/init.lua.
--

Template.new(ItemKind.GLOVES, "gloves")
	:View("gloves", ']')
	:Made("all_leather", ItemQuality.AVG)
	:Worth(2, 1)
	:Armour("1d2", "1d2")
	:Combat("1d3", "1d1", "1d1")
	:Range("1d2")
	:Chance(100)
	:Register()

Template.new(ItemKind.GLOVES, "gauntlets")
	:View("gauntlets", ']')
	:Made("all_metal", ItemQuality.FAIR)
	:Worth(3, 1)
	:Armour("1d3+1", "1d3+1")
	:Combat("1d3+1", "1d1", "1d2")
	:Range("1d3")
	:Chance(10)
	:Register()

Template.new(ItemKind.GLOVES, "knuckles")
	:View("knuckles", ']')
	:Made("hard_metal", ItemQuality.GOOD)
	:Worth(4, 1)
	:Armour("1d2+1", "1d2+1")
	:Combat("1d2", "1d1", "1d3")
	:Range("1d2+2")
	:Chance(10)
	:Register()
