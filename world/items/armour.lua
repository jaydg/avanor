-- Body armour.
--
-- Template.new(kind, type) is documented in world/items/init.lua.
--

Template.new(ItemKind.BODY, "clothes")
	:View("clothes", '[')
	:Made("soft", ItemQuality.POOR)
	:Worth(3, 5)
	:Armour("1d2", "1d1")
	:Combat("", "1d1", "")
	:Chance(100)
	:Register()

-- No :Chance, so nothing ever draws one at random. A dress is worn by
-- whoever content dresses in one - the goodwife and the female citizen ask
-- for it by name, which works whether or not the row is in the draw - and
-- by nobody else.
Template.new(ItemKind.BODY, "dress")
	:View("dress", '[')
	:Made("soft", ItemQuality.POOR)
	:Worth(3, 5)
	:Armour("1d2", "1d1")
	:Combat("", "1d1", "")
	:Register()

Template.new(ItemKind.BODY, "robe")
	:View("robe", '[')
	:Made("soft", ItemQuality.POOR)
	:Worth(5, 9)
	:Armour("1d2", "1d1")
	:Combat("", "1d1", "")
	:Chance(150)
	:Register()

Template.new(ItemKind.BODY, "light_mail")
	:View("scale mail", '[')
	:Made("all_leather", ItemQuality.AVG)
	:Worth(15, 15)
	:Armour("1d3+1", "1d2")
	:Combat("", "1d1", "")
	:Chance(70)
	:Register()

Template.new(ItemKind.BODY, "scale_mail")
	:View("scale mail", '[')
	:Made("all_metal", ItemQuality.FAIR)
	:Worth(30, 30)
	:Armour("1d3", "1d2+2")
	:Combat("1d2", "1d4", "")
	:Chance(20)
	:Register()

Template.new(ItemKind.BODY, "plate_mail")
	:View("plate mail", '[')
	:Made("all_metal", ItemQuality.FAIR)
	:Worth(45, 30)
	:Armour("1d3", "1d2+3")
	:Combat("1d2", "1d4", "")
	:Chance(15)
	:Register()

Template.new(ItemKind.BODY, "chain_mail")
	:View("chain mail", '[')
	:Made("all_metal", ItemQuality.GOOD)
	:Worth(45, 30)
	:Armour("1d3", "1d2+4")
	:Combat("1d2", "1d4", "")
	:Chance(10)
	:Register()

Template.new(ItemKind.BODY, "ring_mail")
	:View("ring mail", '[')
	:Made("all_metal", ItemQuality.GOOD)
	:Worth(50, 30)
	:Armour("1d3", "1d2+4")
	:Combat("1d2", "1d4", "")
	:Chance(10)
	:Register()
