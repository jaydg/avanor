-- What things are made of.
--
-- Every ordinary item is made of one of these, drawn from the set its
-- template allows: a long sword says :Made("obsimetal", ...) and gets
-- steel, mithril, adamantium or obsidian. The sets are at the foot of
-- this file. The material decides what the
-- item is called, what colour it is, what it weighs, what it is worth, and
-- how much it adds to protection and damage.
--
--   Material.new(id)
--       :Called(name)        the word that goes in front - "an iron sword".
--                            Unsaid, the id itself
--       :Looks(xColor.X)
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

-- A note on the first number of :Body(), which is how heavy the material
-- is. These are real densities, scaled so that iron sits at 30 - iron
-- being the commonest thing in the world, and the one worth leaving where
-- it was so that the weight of everything else does not shift underneath
-- the burden rules.
--
-- At that scale one point is about a quarter of a gram per cubic
-- centimetre. Steel used to weigh half what iron did, when the two are in
-- truth within one per cent of each other; gold used to weigh less than
-- iron, when it is two and a half times heavier than anything else here;
-- and stone used to weigh two thirds of iron rather than a third. The
-- invented metals are the exception and stay far below everything, which
-- is what they are famous for.

Material.new("cloth")
	:Looks(xColor.xWHITE)
	:Chance(200)
	:Quality(ItemQuality.POOR)
	:Body(3, 8)
	:Armour("1d1", "1d1")
	:Combat("", 0, "")
	:Register()

Material.new("leather")
	:Looks(xColor.xBROWN)
	:Chance(200)
	:Quality(ItemQuality.POOR)
	:Body(4, 10)
	:Armour("1d2", "1d1")
	:Combat("", 0, "")
	:Register()

Material.new("studded_leather")
	:Called("studded leather")
	:Looks(xColor.xBROWN)
	:Chance(100)
	:Quality(ItemQuality.AVG)
	:Body(6, 12)
	:Armour("1d3", "1d2")
	:Combat("", 0, "")
	:Register()

Material.new("wooden")
	:Looks(xColor.xBROWN)
	:Chance(200)
	:Quality(ItemQuality.POOR)
	:Body(3, 7)
	:Armour("1d2", "1d3")
	:Combat("1d2", 0, "1d2")
	:Register()

-- Horn: springy, tough and light, and it takes an enormous squeeze and
-- gives all of it back. Cut into scales and sewn onto a backing it makes
-- a good coat, a little better than studded leather.
Material.new("horn")
	:Looks(xColor.xBROWN)
	:Chance(40)
	:Quality(ItemQuality.AVG)
	:Body(5, 14)
	:Armour("1d2", "1d2+1")
	:Combat("1d3+1", 0, "1d3+1")
	:Register()

-- Bone is hard and it is brittle, and it is poor in tension. It makes a
-- serviceable point, a club and a coat of plates.
Material.new("bone")
	:Looks(xColor.xWHITE)
	:Chance(60)
	:Quality(ItemQuality.POOR)
	:Body(7, 6)
	:Armour("1d2", "1d2")
	:Combat("1d1", 0, "1d2")
	:Register()

Material.new("stone")
	:Looks(xColor.xLIGHTGRAY)
	:Chance(200)
	:Quality(ItemQuality.POOR)
	:Body(10, 5)
	:Armour("1d3", "1d4+1")
	:Combat("1d3", 0, "1d2")
	:Resist{ earth = 10 }
	:Register()

Material.new("crystal")
	:Looks(xColor.xLIGHTMAGENTA)
	:Chance(50)
	:Quality(ItemQuality.AVG)
	:Body(10, 14)
	:Armour("1d3+3", "1d5+2")
	:Combat("1d5+1", 0, "1d5+1")
	:Resist{ water = 10 }
	:Register()

Material.new("obsidian")
	:Looks(xColor.xDARKGRAY)
	:Chance(50)
	:Quality(ItemQuality.FAIR)
	:Body(10, 20)
	:Armour("1d3+3", "1d6+2")
	:Combat("1d6+1", 0, "1d6+1")
	:Resist{ fire = 15 }
	:Register()

-- The two soft metals: silver yields at about a quarter of what
-- wrought iron does and gold at a tenth, and neither work-hardens usefully.
-- Gold is also twice the weight of steel for the same bulk. So they sit
-- below brass, gold at the very bottom, and what makes them worth having
-- is what they already carry: they do not corrode, and they are worth a
-- great deal.
Material.new("silver")
	:Looks(xColor.xLIGHTGRAY)
	:Chance(30)
	:Quality(ItemQuality.FAIR)
	:Body(40, 30)
	:Armour("1d2", "1d2")
	:Combat("1d4+0", 0, "1d4+0")
	:Resist{ acid = 10 }
	:Register()

Material.new("gold")
	:Looks(xColor.xYELLOW)
	:Chance(15)
	:Quality(ItemQuality.GOOD)
	:Body(74, 50)
	:Armour("1d1", "1d1")
	:Combat("1d5", 0, "1d5+0")
	:Resist{ acid = 20 }
	:Property(SpecialProperty.REGENERATION + SpecialProperty.FAST_DIGESTION)
	:Register()

-- The armour metals, in the order a smith would rank them.
--
-- Protection (pv) is what the metal does, and it is where these differ.
-- Defence (dv) is how easy you still are to hit, and no amount of metal
-- makes a man harder to hit - so the metals barely move it.

-- Copper and zinc: soft, low yield, and it stays soft. Real brass armour
-- was for parades and fittings, never for a battlefield - which is why
-- this is the worst thing here to be wearing.
Material.new("brass")
	:Looks(xColor.xBROWN)
	:Chance(60)
	:Quality(ItemQuality.POOR)
	:Body(32, 30)
	:Armour("1d2", "1d3")
	:Combat("1d3", 0, "1d3+0")
	:Resist{ stun = 10 }
	:Property(SpecialProperty.SLOW_DIGESTION)
	:Register()

-- Copper and tin, and it work-hardens as it is beaten - which is why it
-- armoured the Greeks for centuries. Better than brass by a wide margin
-- and still softer than iron, and heavy for what it gives back.
Material.new("bronze")
	:Looks(xColor.xBROWN)
	:Chance(60)
	:Quality(ItemQuality.AVG)
	:Body(34, 17)
	:Armour("1d2", "1d4+1")
	:Combat("1d4", 0, "1d3+0")
	:Resist{ stun = 5 }
	:Register()

-- Wrought iron: not hard, but tough and ductile, which is the quality a
-- mail ring wants - it bends round a point rather than shattering at it.
-- The standard armour metal of the world for a thousand years.
Material.new("iron")
	:Looks(xColor.xDARKGRAY)
	:Chance(120)
	:Quality(ItemQuality.AVG)
	:Body(30, 15)
	:Armour("1d3", "1d5+1")
	:Combat("1d2", 0, "1d3+0")
	:Register()

-- Iron with the carbon in it and the heat treatment done. Hardened plate
-- is the best armour anybody ever actually wore: a clear step above iron,
-- and the last rung with any physics behind it.
Material.new("steel")
	:Looks(xColor.xLIGHTBLUE)
	:Chance(50)
	:Quality(ItemQuality.FAIR)
	:Body(30, 20)
	:Armour("1d3+1", "1d6+2")
	:Combat("1d6+1", 0, "1d5+1")
	:Resist{ stun = 15 }
	:Register()

-- Past here the metals are invented, so they answer to the ladder rather
-- than to metallurgy: better than steel, and each other's better, by a
-- margin that stays in proportion instead of running away.
Material.new("mithril")
	:Looks(xColor.xLIGHTCYAN)
	:Chance(5)
	:Quality(ItemQuality.GOOD)
	:Body(8, 100)
	:Armour("1d3+3", "2d3+3")
	:Combat("2d4+4", "0d1", "2d4+3")
	:Resist{ poison = 10, stun = 10, confuse = 20 }
	:Register()

Material.new("adamantium")
	:Looks(xColor.xLIGHTGREEN)
	:Chance(2)
	:Quality(ItemQuality.EXCELLENT)
	:Body(6, 300)
	:Armour("1d3+5", "2d4+5")
	:Combat("2d6+6", "1d0", "2d6+3")
	:Resist{ paralyse = 20, stun = 15, confuse = 30, blind = 30 }
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

-- Names for groups of materials, so a template can say what it may be made
-- of without listing them every time. A set may be built out of other
-- sets, which are spliced in as it is read - so they must be declared
-- before they are used.
--
-- A template may also name a single material outright, :Made("steel", ...),
-- so only the groups need naming here.
--
--   MaterialSet.new(id)
--       :Of{ ... }      the materials it allows, or other sets of them
--       :Register()

-- The two leathers, for armour that may be either.
MaterialSet.new("all_leather")
	:Of{ "leather", "studded_leather" }
	:Register()

-- Anything that bends.
MaterialSet.new("soft")
	:Of{ "leather", "studded_leather", "cloth" }
	:Register()

-- Things quarried rather than smelted.
MaterialSet.new("stone_from")
	:Of{ "stone", "crystal", "obsidian" }
	:Register()

-- The soft metals.
MaterialSet.new("soft_metal")
	:Of{ "silver", "gold" }
	:Register()

-- The ordinary metals.
MaterialSet.new("metal")
	:Of{ "bronze", "brass", "iron" }
	:Register()

-- What a blade worth carrying is made of.
MaterialSet.new("hard_metal")
	:Of{ "steel", "mithril", "adamantium" }
	:Register()

MaterialSet.new("all_metal")
	:Of{ "soft_metal", "metal", "hard_metal" }
	:Register()

-- Metals an armour is made of.
MaterialSet.new("armour_metal")
	:Of{ "metal", "hard_metal" }
	:Register()

MaterialSet.new("obsimetal")
	:Of{ "obsidian", "hard_metal" }
	:Register()

-- What each sort of thing is made of.
MaterialSet.new("shield")
	:Of{ "all_leather", "armour_metal", "wooden" }
	:Register()

MaterialSet.new("bow")
	:Of{ "wooden" }
	:Register()

-- Scales sewn onto a backing, which is the oldest armour there is and can
-- be made of whatever is hard and comes in small pieces.
MaterialSet.new("scale")
	:Of{ "all_leather", "horn", "bone" }
	:Register()

-- Bone makes a good point and always has.
MaterialSet.new("missile")
	:Of{ "all_metal", "wooden", "stone", "bone" }
	:Register()

MaterialSet.new("wood_stone")
	:Of{ "stone_from", "wooden" }
	:Register()

MaterialSet.new("weapon")
	:Of{ "stone_from", "hard_metal" }
	:Register()

MaterialSet.new("simple_weapon")
	:Of{ "iron", "steel" }
	:Register()

MaterialSet.new("black_metal")
	:Of{ "steel", "iron" }
	:Register()
