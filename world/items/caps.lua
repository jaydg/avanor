-- Hats and helmets.
--
-- Template.new(kind, type) is documented in world/items/init.lua.
--

Template.new(ItemKind.HAT, "hat")
	:View("hat", '[')
	:Made("soft", ItemQuality.POOR)
	:Worth(1, 1)
	:Armour("1d1", 0)
	:Combat("", "1d1", "")
	:Chance(100)
	:Register()

Template.new(ItemKind.HAT, "cap")
	:View("cap", '[')
	:Made("all_leather", ItemQuality.AVG)
	:Worth(2, 2)
	:Armour("1d2+1", "1d2")
	:Combat("", "1d1", "")
	:Chance(50)
	:Register()

Template.new(ItemKind.HAT, "helmet")
	:View("helmet", '[')
	:Made("armour_metal", ItemQuality.FAIR)
	:Worth(3, 3)
	:Armour("1d2", "1d3+1")
	:Combat("", "1d2", "")
	:Chance(25)
	:Register()

Template.new(ItemKind.HAT, "great_helm")
	:View("great helm", '[')
	:Made("hard_metal", ItemQuality.FAIR)
	:Worth(6, 4)
	:Armour("1d2", "1d4+3")
	:Combat("", "1d3", "")
	:Chance(5)
	:Register()
