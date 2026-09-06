-- Cloaks.
--
-- Template.new(kind, type) is documented in world/items/init.lua.
--

Template.new(ItemKind.CLOAK, "light_cloak")
	:View("light cloak", '(')
	:Made("all_leather", ItemQuality.AVG)
	:Worth(1, 2)
	:Armour("1d2+1", "1d2")
	:Combat("", "1d1", "")
	:Chance(100)
	:Register()

Template.new(ItemKind.CLOAK, "cloak")
	:View("cloak", '(')
	:Made("all_leather", ItemQuality.AVG)
	:Worth(2, 3)
	:Armour("1d2", "1d2+1")
	:Combat("", "1d1", "")
	:Chance(60)
	:Register()

Template.new(ItemKind.CLOAK, "shadow_cloak")
	:View("shadow cloak", '(')
	:Made("all_leather", ItemQuality.FAIR)
	:Worth(15, 8)
	:Armour("1d2+2", "1d5+3")
	:Combat("", "1d1", "")
	:Chance(10)
	:Register()

Template.new(ItemKind.CLOAK, "cape")
	:View("cape", '(')
	:Made("soft", ItemQuality.AVG)
	:Worth(2, 2)
	:Armour("1d2", "1d1")
	:Combat("", "1d1", "")
	:Chance(120)
	:Register()
