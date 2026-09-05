-- Scrolls.
--
-- A scroll is a spell somebody wrote down: it names an effect, and reading
-- it makes that effect happen to whoever or whatever the reader picks. All
-- a row has to say is which effect, what the scroll is worth, and how often
-- one turns up.
--
--   Scroll.new(id)
--       :Called(name)       what it reads as once somebody knows it
--       :Effect(XEffect.X)  what reading it does. Unsaid, it does nothing
--                           by itself - see "recipe" below
--       :OnRead(handler)    for a scroll whose reading is not an effect:
--                           the Lua function it hands over to, called with
--                           the reader. It answers 1 if the reading was
--                           worth something, 0 if it was not
--       :Worth(value)
--       :Chance(rarity)     its weight in the draw against other scrolls
--       :ReadInCombat()     a monster holding one will read it at an enemy.
--                           Unsaid, it saves it for something else
--       :Register()
--
-- Until it is identified a scroll reads as a label of nonsense syllables,
-- rolled fresh for each row as this file loads - so the labels differ from
-- one game to the next, and knowing one in this game tells you nothing
-- about the next.

Scroll.new("healing")
	:Called("healing")
	:Effect(XEffect.HEAL)
	:Worth(200)
	:Chance(10)
	:Register()

Scroll.new("burning_hands")
	:Called("burning hands")
	:Effect(XEffect.BURNING_HANDS)
	:Worth(20)
	:Chance(100)
	:Register()

Scroll.new("ice_touch")
	:Called("ice touch")
	:Effect(XEffect.ICE_TOUCH)
	:Worth(20)
	:Chance(100)
	:Register()

Scroll.new("heroism")
	:Called("heroism")
	:Effect(XEffect.HEROISM)
	:Worth(25)
	:Chance(100)
	:Register()

Scroll.new("power")
	:Called("power")
	:Effect(XEffect.POWER)
	:Worth(15)
	:Chance(100)
	:Register()

Scroll.new("identify")
	:Called("identify")
	:Effect(XEffect.IDENTIFY)
	:Worth(100)
	:Chance(30)
	:Register()

Scroll.new("magic_arrow")
	:Called("magic arrow")
	:Effect(XEffect.MAGIC_ARROW)
	:Worth(15)
	:Chance(200)
	:ReadInCombat()
	:Register()

Scroll.new("fire_bolt")
	:Called("fire bolt")
	:Effect(XEffect.FIRE_BOLT)
	:Worth(50)
	:Chance(60)
	:ReadInCombat()
	:Register()

Scroll.new("ice_bolt")
	:Called("ice bolt")
	:Effect(XEffect.ICE_BOLT)
	:Worth(50)
	:Chance(60)
	:ReadInCombat()
	:Register()

Scroll.new("lightning_bolt")
	:Called("lightning bolt")
	:Effect(XEffect.LIGHTNING_BOLT)
	:Worth(100)
	:Chance(20)
	:ReadInCombat()
	:Register()

Scroll.new("acid_bolt")
	:Called("acid bolt")
	:Effect(XEffect.ACID_BOLT)
	:Worth(150)
	:Chance(15)
	:ReadInCombat()
	:Register()

Scroll.new("summon_monster")
	:Called("summon monsters")
	:Effect(XEffect.SUMMON_MONSTER)
	:Worth(10)
	:Chance(100)
	:Register()

Scroll.new("create_item")
	:Called("create item")
	:Effect(XEffect.CREATE_ITEM)
	:Worth(200)
	:Chance(25)
	:Register()

Scroll.new("cure_disease")
	:Called("cure disease")
	:Effect(XEffect.CURE_DISEASE)
	:Worth(40)
	:Chance(100)
	:Register()

Scroll.new("cure_poison")
	:Called("cure poison")
	:Effect(XEffect.CURE_POISON)
	:Worth(40)
	:Chance(100)
	:Register()

Scroll.new("blink")
	:Called("blink")
	:Effect(XEffect.BLINK)
	:Worth(70)
	:Chance(30)
	:Register()

Scroll.new("self_knowledge")
	:Called("self knowledge")
	:Effect(XEffect.SELF_KNOWLEDGE)
	:Worth(150)
	:Chance(10)
	:Register()

Scroll.new("see_invisible")
	:Called("see invisible")
	:Effect(XEffect.SEE_INVISIBLE)
	:Worth(40)
	:Chance(50)
	:Register()

-- The one scroll that is not an effect: it teaches an alchemy recipe.
-- Which recipes exist is not declared anywhere in world/ - they are dealt
-- out afresh each game from the potions' own :Alchemy() levels - so this
-- asks for one rather than naming it.
Scroll.new("recipe")
	:Called("recipe")
	:Worth(30)
	:Chance(25)
	:OnRead("TeachAlchemyRecipe")
	:Register()

-- Answers 1 when the reader is better off for having read it, 0 when the
-- scroll told them nothing they did not know - which is what makes the
-- engine say "You feel nothing special."
function TeachAlchemyRecipe(reader)
	local count = AlchemyRecipeCount()

	if (count < 1) then
		return 0
	end

	local first, second, result = AlchemyRecipe(Rand(count))

	if (not first) then
		return 0
	end

	if (LearnAlchemyRecipe(reader, first, second, result)) then
		return 1
	end

	return 0
end
