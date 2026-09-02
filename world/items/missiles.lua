-- What they shoot.
--
-- Template.new(kind, type) is documented in world/items/init.lua.
--

Template.new(ItemKind.MISSILE, ItemType.ARROW)
	:View("arrow", '\\')
	:Made(ItemSet.MISSILE, ItemQuality.AVG)
	:Worth(1, 1)
	:Combat("1d2", "1d4", "1d3")
	:Range("1d2+3")
	:Chance(100)
	:Register()

Template.new(ItemKind.MISSILE, ItemType.QUARREL)
	:View("quarrel", '\\')
	:Made(ItemSet.MISSILE, ItemQuality.AVG)
	:Worth(1, 1)
	:Combat("1d2", "1d6", "1d3")
	:Range("1d2+3")
	:Chance(100)
	:Register()

Template.new(ItemKind.MISSILE, ItemType.SLINGBULLET)
	:View("sling bullet", '\\')
	:Made(ItemSet.MISSILE, ItemQuality.FAIR)
	:Worth(1, 1)
	:Combat("1d2", "1d5", "1d2")
	:Range("1d2+3")
	:Chance(30)
	:Register()

Template.new(ItemKind.MISSILE, ItemType.ROCK)
	:View("rock", '*')
	:Made(ItemSet.STONE, ItemQuality.POOR)
	:Worth(1, 1)
	:Combat("1d1", "1d3", "1d1")
	:Range("1d2+2")
	:Chance(300)
	:Register()

Template.new(ItemKind.MISSILE, ItemType.SHURIKEN)
	:View("shuriken", '*')
	:Made(ItemSet.METAL, ItemQuality.FAIR)
	:Worth(1, 1)
	:Combat("1d2", "1d6", "1d4")
	:Range("1d2+2")
	:Chance(50)
	:Register()
