-- What things are made of.
--
-- Every ordinary item is made of one of these, drawn from the set its
-- template allows: a long sword says :Made(ItemSet.OBSIMETAL, ...) and gets
-- steel, mithril, adamantium or obsidian. The material decides what the
-- item is called, what colour it is, what it weighs, what it is worth, and
-- how much it adds to protection and damage.
--
--   Material.new(id)
--       :Called(name)        the word that goes in front - "an iron sword".
--                            Unsaid, the id itself
--       :Looks(xColor.X)
--       :Sets(ItemSet.X)     which sets this material belongs to, so a
--                            template asking for a set can be given it
--       :Chance(n)           its weight in the draw against other materials
--                            of the same set
--       :Quality(ItemQuality.X)
--                            what it does to the item's quality
--       :Body(density, worth)
--                            multiplies the item's weight, and its value in
--                            tenths - 10 leaves the price alone
--       :Armour(dv, pv)      dice strings, added to what the item already has
--       :Combat(hit, dice, extra)
--       :Resist(text)        what wearing it resists
--       :Property(...)       declared but inert - see the note at the foot
--                            of this file
--       :Register()

Material.new("cloth")
	:Looks(xColor.xWHITE)
	:Sets(ItemSet.CLOTH)
	:Chance(200)
	:Quality(ItemQuality.POOR)
	:Body(4, 8)
	:Armour("1d1", "1d1")
	:Combat("", "0d0", "")
	:Register()

Material.new("leather")
	:Looks(xColor.xBROWN)
	:Sets(ItemSet.LEATHER)
	:Chance(200)
	:Quality(ItemQuality.POOR)
	:Body(6, 10)
	:Armour("1d2", "1d1")
	:Combat("", "0d0", "")
	:Register()

Material.new("studded_leather")
	:Called("studded leather")
	:Looks(xColor.xBROWN)
	:Sets(ItemSet.STUDEDLEATHER)
	:Chance(100)
	:Quality(ItemQuality.AVG)
	:Body(8, 12)
	:Armour("1d3", "1d2")
	:Combat("", "0d0", "")
	:Register()

Material.new("wooden")
	:Looks(xColor.xBROWN)
	:Sets(ItemSet.WOOD)
	:Chance(200)
	:Quality(ItemQuality.POOR)
	:Body(8, 7)
	:Armour("1d2", "1d3")
	:Combat("1d2", "0d0", "1d2")
	:Register()

Material.new("stone")
	:Looks(xColor.xLIGHTGRAY)
	:Sets(ItemSet.STONE)
	:Chance(200)
	:Quality(ItemQuality.POOR)
	:Body(20, 5)
	:Armour("1d3", "1d4+1")
	:Combat("1d3", "0d0", "1d2")
	:Resist("earth:0d0+10")
	:Register()

Material.new("iron")
	:Looks(xColor.xDARKGRAY)
	:Sets(ItemSet.IRON)
	:Chance(120)
	:Quality(ItemQuality.POOR)
	:Body(30, 15)
	:Armour("1d3", "1d5+1")
	:Combat("1d2", "0d0", "1d3+0")
	:Register()

Material.new("bronze")
	:Looks(xColor.xBROWN)
	:Sets(ItemSet.BRONZE)
	:Chance(60)
	:Quality(ItemQuality.AVG)
	:Body(26, 17)
	:Armour("1d3", "1d5+1")
	:Combat("1d4", "0d0", "1d3+0")
	:Resist("stun:0d0+5")
	:Register()

Material.new("brass")
	:Looks(xColor.xBROWN)
	:Sets(ItemSet.BRASS)
	:Chance(60)
	:Quality(ItemQuality.AVG)
	:Body(30, 30)
	:Armour("1d3", "1d5+1")
	:Combat("1d3", "0d0", "1d3+0")
	:Resist("stun:0d0+10")
	:Property(SpecialProperty.SLOW_DIGESTION)
	:Register()

Material.new("silver")
	:Looks(xColor.xLIGHTGRAY)
	:Sets(ItemSet.SILVER)
	:Chance(30)
	:Quality(ItemQuality.FAIR)
	:Body(20, 30)
	:Armour("1d3", "1d5+2")
	:Combat("1d4+0", "0d0", "1d4+0")
	:Resist("acid:0d0+10")
	:Register()

Material.new("golden")
	:Looks(xColor.xYELLOW)
	:Sets(ItemSet.GOLD)
	:Chance(15)
	:Quality(ItemQuality.GOOD)
	:Body(22, 50)
	:Armour("1d3+1", "1d5+2")
	:Combat("1d5", "0d0", "1d5+0")
	:Resist("acid:0d0+20")
	:Property(SpecialProperty.REGENERATION + SpecialProperty.FAST_DIGESTION)
	:Register()

Material.new("crystal")
	:Looks(xColor.xLIGHTMAGENTA)
	:Sets(ItemSet.CRYSTAL)
	:Chance(50)
	:Quality(ItemQuality.AVG)
	:Body(25, 14)
	:Armour("1d3+3", "1d5+2")
	:Combat("1d5+1", "0d0", "1d5+1")
	:Resist("water:0d0+10")
	:Register()

Material.new("steel")
	:Looks(xColor.xLIGHTBLUE)
	:Sets(ItemSet.STEEL)
	:Chance(50)
	:Quality(ItemQuality.FAIR)
	:Body(15, 20)
	:Armour("1d3+3", "1d5+2")
	:Combat("1d6+1", "0d0", "1d5+1")
	:Resist("stun:0d0+15")
	:Register()

Material.new("obsidian")
	:Looks(xColor.xDARKGRAY)
	:Sets(ItemSet.OBSIDIAN)
	:Chance(50)
	:Quality(ItemQuality.FAIR)
	:Body(13, 20)
	:Armour("1d3+3", "1d6+2")
	:Combat("1d6+1", "0d0", "1d6+1")
	:Resist("fire:0d0+15")
	:Register()

Material.new("mithril")
	:Looks(xColor.xLIGHTCYAN)
	:Sets(ItemSet.MITHRIL)
	:Chance(5)
	:Quality(ItemQuality.GOOD)
	:Body(11, 100)
	:Armour("1d3+6", "2d3+3")
	:Combat("2d4+4", "0d1", "2d4+3")
	:Resist("poison:0d0+10 stun:0d0+10 confuse:0d0+20")
	:Register()

Material.new("adamantium")
	:Looks(xColor.xLIGHTGREEN)
	:Sets(ItemSet.ADAMANTIUM)
	:Chance(2)
	:Quality(ItemQuality.EXCELLENT)
	:Body(9, 300)
	:Armour("1d3+12", "2d4+5")
	:Combat("2d6+6", "1d0", "2d6+3")
	:Resist("paralyse:0d0+20 stun:0d0+15 confuse:0d0+30 blind:0d0+30")
	:Property(SpecialProperty.REGENERATION)
	:Register()

-- A note on :Property(). Three materials above declare one - brass slows
-- digestion, gold and adamantium regenerate the wearer - and none of them
-- has ever done anything: PropFill() has never copied the property from
-- the material onto the item, in any version of the game. The declarations
-- are kept because they are somebody's design, not because they work.
--
-- The armour enchantments in world/items/armour_enchantments.lua use the
-- same field, and their side of it does work now.
