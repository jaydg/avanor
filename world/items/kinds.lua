-- What each kind of item does beyond being that kind.
--
-- The rules themselves are the engine's - how a price is summed, how a
-- to-hit bonus stacks when something is worn - but which kinds of item
-- each one applies to was buried in the ItemKind enum as a composite
-- member (TOHIT, VALUEDICE, VALUEDVPV, VALUEHITDMG), where content could
-- neither read it nor change it. A world that wants its wands priced by
-- their damage, or its cloaks to steady your hand, says so here.
--
--   ItemKindRules.new(kind)
--       :TakesToHit()      wearing one adds its to-hit bonus to the wearer
--       :PricedByDice()    its damage dice count towards its price
--       :PricedByArmour()  its DV and PV count towards its price
--       :PricedByDamage()  its to-hit and damage bonus count towards
--                          its price
--       :IsArmour()        it is armour - only the item dump asks
--       :Register()
--
-- `kind` may name several at once, added together or through a group like
-- ItemKind.ARMOUR. Naming the same kind again is fine: the rules add up.

-- Every piece of armour steadies the hand that holds the weapon, and is
-- worth what it shelters.
ItemKindRules.new(ItemKind.ARMOUR)
	:TakesToHit()
	:IsArmour()
	:PricedByArmour()
	:Register()

-- A shield is the one piece of armour whose own to-hit and damage do not
-- count towards its price - it is bought for what it stops. Kept as it
-- has always been, which is why this line names the other five rather
-- than saying ItemKind.ARMOUR again.
ItemKindRules.new(ItemKind.HAT + ItemKind.BODY + ItemKind.CLOAK
	+ ItemKind.GLOVES + ItemKind.BOOTS)
	:PricedByDamage()
	:Register()

-- A necklace and a ring steady the hand too, and are priced entirely by
-- what they do rather than by what they are.
ItemKindRules.new(ItemKind.NECK + ItemKind.RING)
	:TakesToHit()
	:Register()

-- A weapon is worth what it hits for, in every sense.
ItemKindRules.new(ItemKind.WEAPON)
	:TakesToHit()
	:PricedByDice()
	:PricedByArmour()
	:PricedByDamage()
	:Register()

-- A bow and its arrows are worth what they throw, but neither steadies
-- the hand the way worn armour does.
ItemKindRules.new(ItemKind.MISSILEW + ItemKind.MISSILE)
	:PricedByDice()
	:Register()
