-- Shields.
--
-- Template.new(kind, type) is documented in world/items/init.lua.
--

Template.new(ItemKind.SHIELD, "small_shield")
	:View("small shield", ')')
	:Made("shield", ItemQuality.POOR)
	:Skill("shield")
	:Worth(3, 3)
	:Armour("0d0+2", "1d2")
	:Combat("", "1d2", "")
	:Chance(200)
	:Register()

Template.new(ItemKind.SHIELD, "medium_shield")
	:View("medium shield", ')')
	:Made("shield", ItemQuality.AVG)
	:Skill("shield")
	:Worth(5, 5)
	:Armour("0d0+3", "1d2")
	:Combat("", "1d3", "")
	:Chance(100)
	:Register()

Template.new(ItemKind.SHIELD, "large_shield")
	:View("large shield", ')')
	:Made("shield", ItemQuality.AVG)
	:Skill("shield")
	:Worth(10, 8)
	:Armour("0d0+4", "1d2")
	:Combat("", "1d4", "")
	:Chance(50)
	:Register()

Template.new(ItemKind.SHIELD, "tower_shield")
	:View("tower shield", ')')
	:Made("shield", ItemQuality.FAIR)
	:Skill("shield")
	:Worth(15, 15)
	:Armour("0d0+6", "1d2")
	:Combat("", "1d4", "")
	:Chance(10)
	:Register()
