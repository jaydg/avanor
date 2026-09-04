-- What a piece of armour can turn out to be, beyond plain.
--
-- These are the "of Strength", "of the Titans" that a helmet or a shield
-- can be found with. Rings and amulets have their own, separate list in
-- world/items/enchantments.lua - the two have never shared a table.
--
--   ArmourEnchantment.new(id)
--       :Called(text)        how the enchanted item reads. A format string,
--                            because the words go on either side: "{} of
--                            Strength", but "clean {}"
--       :Looks(xColor.X)     its colour. Unsaid, it keeps the colour its
--                            material gave it
--       :Rarity(n)           how rare - the HIGHER this is, the fewer come
--                            out this way. A threshold a roll must beat,
--                            not a weight in a draw
--       :Weight(n)           how many places it takes in the draw. Unsaid,
--                            one, like everything else
--       :Fits(ItemKind.X)    which sorts of item it can appear on
--       :Brand(AttackEffectType.X)
--       :Stats(text) / :Resist(text)
--                            the same dice strings Monster.new takes
--       :Property(...)       declared but inert - see "of slow digestion"
--       :Register()

ArmourEnchantment.new("fire_resistance")
	:Called("{} of fire resistance")
	:Looks(xColor.xRED)
	:Rarity(1)
	:Fits(ItemKind.ARMOUR)
	:Resist("fire:5d5+25")
	:Register()

ArmourEnchantment.new("cold_resistance")
	:Called("{} of cold resistance")
	:Looks(xColor.xWHITE)
	:Rarity(1)
	:Fits(ItemKind.ARMOUR)
	:Resist("cold:5d5+25")
	:Register()

ArmourEnchantment.new("air_resistance")
	:Called("{} of air resistance")
	:Looks(xColor.xLIGHTBLUE)
	:Rarity(1)
	:Fits(ItemKind.ARMOUR)
	:Resist("air:5d5+25")
	:Register()

ArmourEnchantment.new("acid_resistance")
	:Called("{} of acid resistance")
	:Looks(xColor.xDARKGRAY)
	:Rarity(5)
	:Fits(ItemKind.ARMOUR)
	:Resist("acid:5d5+25")
	:Register()

ArmourEnchantment.new("poison_resistance")
	:Called("{} of poison resistance")
	:Rarity(2)
	:Fits(ItemKind.ARMOUR)
	:Resist("poison:5d5+25")
	:Register()

ArmourEnchantment.new("resistance")
	:Called("{} of Resistance")
	:Looks(xColor.xDARKGRAY)
	:Rarity(80)
	:Fits(ItemKind.ARMOUR)
	:Property(SpecialProperty.FAST_DIGESTION)
	:Resist("poison:5d5+25 fire:5d5+25 cold:5d5+25 water:5d5+25 acid:5d5+25 earth:5d5+25")
	:Register()

ArmourEnchantment.new("magi")
	:Called("{} of the Magi")
	:Rarity(50)
	:Fits(ItemKind.ARMOUR)
	:Stats("Le:1d5 Wi:1d5 Ma:1d5")
	:Resist("fire:5d5+25 cold:5d5+25")
	:Register()

ArmourEnchantment.new("free_action")
	:Called("{} of free action")
	:Rarity(50)
	:Fits(ItemKind.ARMOUR)
	:Resist("confuse:5d5+25 stun:5d5+25")
	:Register()

-- Its whole effect is the property below: the wearer grows hungry at half
-- the usual rate. The property is declared here and acted on by the engine
-- (XCreature::DecNutrio).
ArmourEnchantment.new("slow_digestion")
	:Called("{} of slow digestion")
	:Rarity(1)
	:Fits(ItemKind.ARMOUR)
	:Property(SpecialProperty.SLOW_DIGESTION)
	:Register()

ArmourEnchantment.new("strength")
	:Called("{} of Strength")
	:Rarity(1)
	:Fits(ItemKind.ARMOUR)
	:Stats("St:1d4")
	:Register()

ArmourEnchantment.new("swiftness")
	:Called("{} of Swiftness")
	:Rarity(1)
	:Fits(ItemKind.ARMOUR)
	:Stats("Dx:1d4")
	:Register()

ArmourEnchantment.new("toughness")
	:Called("{} of Toughness")
	:Rarity(1)
	:Fits(ItemKind.ARMOUR)
	:Stats("To:1d4")
	:Register()

ArmourEnchantment.new("giants")
	:Called("{} of the Giants")
	:Rarity(30)
	:Fits(ItemKind.ARMOUR)
	:Stats("St:1d5+5")
	:Register()

ArmourEnchantment.new("titans")
	:Called("{} of the Titans")
	:Looks(xColor.xCYAN)
	:Rarity(60)
	:Fits(ItemKind.ARMOUR)
	:Stats("St:1d10+10")
	:Register()

-- The old table held four "of the High Kings" rows, two of them identical
-- word for word - which simply meant the strength one came up twice as
-- often. That is said here with :Weight(2) instead of by writing the row
-- out twice.
ArmourEnchantment.new("high_kings_strength")
	:Called("{} of the High Kings")
	:Looks(xColor.xWHITE)
	:Rarity(90)
	:Weight(2)
	:Fits(ItemKind.ARMOUR)
	:Stats("St:1d1+24")
	:Register()

ArmourEnchantment.new("high_kings_toughness")
	:Called("{} of the High Kings")
	:Looks(xColor.xWHITE)
	:Rarity(90)
	:Fits(ItemKind.ARMOUR)
	:Stats("To:1d1+24")
	:Register()

ArmourEnchantment.new("high_kings_dexterity")
	:Called("{} of the High Kings")
	:Looks(xColor.xWHITE)
	:Rarity(90)
	:Fits(ItemKind.ARMOUR)
	:Stats("Dx:1d1+24")
	:Register()

ArmourEnchantment.new("elven_kings")
	:Called("{} of the Elven Kings")
	:Looks(xColor.xWHITE)
	:Rarity(90)
	:Fits(ItemKind.ARMOUR)
	:Stats("Le:1d1+24")
	:Register()

ArmourEnchantment.new("clean")
	:Called("clean {}")
	:Rarity(1)
	:Fits(ItemKind.ARMOUR)
	:Stats("ch:1d6+0")
	:Register()
