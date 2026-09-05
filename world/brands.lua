-- Brands: what an attack carries besides the blow itself. A weapon of
-- fire, a spectre's touch that drains, a sword that slays orcs.
--
-- Which brands exist is content. The engine only asks "what does this
-- attack carry", so a world may declare a brand of radiation or of holy
-- water without the C++ side hearing about it.
--
--   Brand.new(id)
--       :Called(template)   the weapon name, {} standing for the plain
--                           name: "{} of Fire" gives "sword of Fire".
--                           Unsaid, the brand does not rename its weapon
--       :Group(g)           how the name combines with other brands -
--                           ELEMENTAL and SLAYER names join, BLACK ones
--                           suppress the name entirely
--       :Element(resist)    magic damage, resisted by this resistance
--       :Slays(class)       triple damage against that creature class
--       :Inflicts(modifier) laid on the victim after a blow that landed
--       :Value(n)           what carrying it adds to the price
--       :Register()

-- The elements. Fire and cold are the two the game leans on; earth and
-- lightning arrive from spells rather than from weapons.
Brand.new("fire")
	:Called("{} of Fire")
	:Group(BrandGroup.ELEMENTAL)
	:Element("fire")
	:Value(200)
	:Register()

Brand.new("cold")
	:Called("{} of Cold")
	:Group(BrandGroup.ELEMENTAL)
	:Element("cold")
	:Value(150)
	:Register()

Brand.new("lightning")
	:Called("{} of Lightning")
	:Group(BrandGroup.ELEMENTAL)
	:Element("air")
	:Register()

-- Earth damage is dealt by the small arrow spell. It has never had a
-- weapon name of its own, so an earth-branded weapon is named plainly.
Brand.new("earth")
	:Group(BrandGroup.ELEMENTAL)
	:Element("earth")
	:Register()

-- Named by the old weapon tables and never given an effect: these two
-- rename a weapon and do nothing else.
Brand.new("hellfire")
	:Called("{} of Hell Fire")
	:Group(BrandGroup.ELEMENTAL)
	:Register()

Brand.new("ultimate_cold")
	:Called("{} of Ultimate Cold")
	:Group(BrandGroup.ELEMENTAL)
	:Register()

-- The black brands. A weapon carrying any of these is named plainly - the
-- naming rules give a name only when no black brand is present - so the
-- templates below are carried for the day that rule changes.
Brand.new("acid")
	:Called("{} of Acid")
	:Group(BrandGroup.BLACK)
	:Element("acid")
	:Register()

Brand.new("poison")
	:Called("{} of Poison")
	:Group(BrandGroup.BLACK)
	:Inflicts(Modifier.POISON)
	:Register()

Brand.new("disease")
	:Group(BrandGroup.BLACK)
	:Inflicts(Modifier.DISEASE)
	:Register()

Brand.new("paralyse")
	:Group(BrandGroup.BLACK)
	:Inflicts(Modifier.PARALYSE)
	:Register()

Brand.new("stun")
	:Group(BrandGroup.BLACK)
	:Inflicts(Modifier.STUN)
	:Register()

Brand.new("confuse")
	:Group(BrandGroup.BLACK)
	:Inflicts(Modifier.CONFUSE)
	:Register()

-- Half of what the victim loses, the attacker gains. The engine does this
-- one itself: it is the only brand that touches the attacker.
Brand.new("drain_life")
	:Group(BrandGroup.BLACK)
	:Register()

-- Named, never implemented: it was meant to treble the chance of a
-- critical hit.
Brand.new("death")
	:Called("{} of Death")
	:Group(BrandGroup.BLACK)
	:Register()

-- The slayers. Only orcs and demons are actually slain; the other six
-- name a weapon and leave it at that, as they always have.
Brand.new("orc_slayer")
	:Called("{} of Slay Orcs")
	:Group(BrandGroup.SLAYER)
	:Slays(CreatureClass.ORC)
	:Value(300)
	:Register()

Brand.new("demon_slayer")
	:Called("{} of Slay Demons")
	:Group(BrandGroup.SLAYER)
	:Slays(CreatureClass.DEMON)
	:Value(220)
	:Register()

Brand.new("undead_slayer")
	:Called("{} of Slay Undead")
	:Group(BrandGroup.SLAYER)
	:Register()

Brand.new("humanoid_slayer")
	:Called("{} of Slay Humanoids")
	:Group(BrandGroup.SLAYER)
	:Register()

Brand.new("animal_slayer")
	:Called("{} of Slay Animals")
	:Group(BrandGroup.SLAYER)
	:Register()

Brand.new("dragon_slayer")
	:Called("{} of Dragon Slaying")
	:Group(BrandGroup.SLAYER)
	:Register()

Brand.new("giant_slayer")
	:Called("{} of Giant Slaying")
	:Group(BrandGroup.SLAYER)
	:Register()

Brand.new("troll_slayer")
	:Called("{} of Slay Trolls")
	:Group(BrandGroup.SLAYER)
	:Register()
