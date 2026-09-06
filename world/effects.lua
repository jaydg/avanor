-- What magic can do.
--
-- An effect is a list of things it does, tried in order until one of them
-- has something to do - so a potion that heals wounds and mends bleeding
-- does whichever the drinker actually needs, never both.
--
-- Amounts are dice rolled against the power the effect was cast with:
-- Heals(1, 2, 3) rolls 1d(power/2)+3. A divisor of 1 uses the full power.
--
--   Effect.new(id)
--       :Heals(count, divisor, bonus)      hit points back
--       :Cures(count, divisor, bonus)      bleeding staunched
--       :Restores(count, divisor, bonus)   power points back
--       :Inflicts(modifier, c, d, b)       laid on the causer
--       :Relieves(modifier, c, d, b)       taken off the causer
--       :Sustains(modifier)                laid on for `power` itself, with
--                                          no dice - a heroism or a
--                                          resistance held while it lasts
--       :Touches(c, d, b, colour, brand, message)   where they face
--       :Throws(c, d, b, colour, brand, message)    at a place in range
--       :Engine(name)                      one of the seven the engine
--                                          does itself: identify,
--                                          great_identify, self_knowledge,
--                                          summon_monster, create_item,
--                                          blink, teleport
--       :Targets(EffectTarget.X)           what it needs pointed at
--       :Range(divisor, bonus)             how far: power/divisor + bonus
--       :Register()

-- Healing and restoration. Each tries to heal first and falls back to
-- mending wounds when there is nothing left to heal.
Effect.new("cure_light_wounds")
	:Heals(1, 2, 3)
	:Cures(1, 10, 1)
	:Register()

Effect.new("cure_serious_wounds")
	:Heals(1, 1, 5)
	:Cures(1, 5, 2)
	:Register()

Effect.new("cure_critical_wounds")
	:Heals(2, 1, 5)
	:Cures(1, 2, 3)
	:Register()

Effect.new("cure_mortal_wounds")
	:Heals(3, 1, 10)
	:Cures(3, 1, 10)
	:Register()

Effect.new("heal")
	:Heals(5, 1, 20)
	:Cures(5, 1, 20)
	:Register()

Effect.new("ultraheal")
	:Heals(7, 1, 20)
	:Cures(7, 1, 20)
	:Register()

Effect.new("power")
	:Restores(3, 1, 20)
	:Register()

Effect.new("ultrapower")
	:Restores(5, 1, 20)
	:Register()

Effect.new("restoration")
	:Heals(5, 1, 20)
	:Cures(5, 1, 20)
	:Restores(5, 1, 20)
	:Register()

Effect.new("cure_poison")
	:Relieves("poison", 1, 1, 5)
	:Register()

Effect.new("cure_disease")
	:Relieves("disease", 1, 1, 3)
	:Register()

-- Touch: it lands on the square the caster faces.
Effect.new("burning_hands")
	:Touches(1, 1, 5, xColor.xRED, "fire", "the ball of fire")
	:Targets(EffectTarget.DIRECTION)
	:Register()

Effect.new("ice_touch")
	:Touches(1, 1, 7, xColor.xWHITE, "cold", "the cone of ice")
	:Targets(EffectTarget.DIRECTION)
	:Register()

Effect.new("drain_life")
	:Touches(1, 1, 9, xColor.xDARKGRAY, "drain_life", "the black sphere")
	:Targets(EffectTarget.DIRECTION)
	:Register()

-- Bolts: they fly to a place within range.
Effect.new("magic_arrow")
	:Throws(1, 2, 0, xColor.xBROWN, "earth", "the small arrow")
	:Targets(EffectTarget.TARGET)
	:Range(4, 2)
	:Register()

Effect.new("fire_bolt")
	:Throws(1, 1, 3, xColor.xRED, "fire", "the small ball of fire")
	:Targets(EffectTarget.TARGET)
	:Range(6, 2)
	:Register()

Effect.new("ice_bolt")
	:Throws(1, 1, 5, xColor.xWHITE, "cold", "the small cone of ice")
	:Targets(EffectTarget.TARGET)
	:Range(6, 2)
	:Register()

Effect.new("lightning_bolt")
	:Throws(2, 1, 10, xColor.xLIGHTBLUE, "lightning", "the bright spark")
	:Targets(EffectTarget.TARGET)
	:Range(7, 2)
	:Register()

Effect.new("acid_bolt")
	:Throws(3, 1, 15, xColor.xGREEN, "acid", "the small ball of viscous liquid")
	:Targets(EffectTarget.TARGET)
	:Range(8, 2)
	:Register()

-- Blessings that last a while.
Effect.new("heroism")
	:Inflicts("heroism", 2, 1, 5)
	:Register()

Effect.new("see_invisible")
	:Sustains("see_invisible")
	:Register()

Effect.new("acid_resistance")
	:Sustains("resist_acid")
	:Register()

Effect.new("fire_resistance")
	:Sustains("resist_fire")
	:Register()

Effect.new("cold_resistance")
	:Sustains("resist_cold")
	:Register()

Effect.new("poison_resistance")
	:Sustains("resist_poison")
	:Register()

-- The seven the engine does itself.
Effect.new("identify")
	:Engine("identify")
	:Targets(EffectTarget.ITEM)
	:Register()

Effect.new("great_identify")
	:Engine("great_identify")
	:Register()

Effect.new("self_knowledge")
	:Engine("self_knowledge")
	:Register()

Effect.new("summon_monster")
	:Engine("summon_monster")
	:Register()

Effect.new("create_item")
	:Engine("create_item")
	:Register()

Effect.new("blink")
	:Engine("blink")
	:Register()

Effect.new("teleport")
	:Engine("teleport")
	:Targets(EffectTarget.TARGET)
	:Range(0, 1)
	:Register()
