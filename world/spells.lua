-- Spells.
--
-- A spell is an effect somebody learned to call up at will: it names the
-- effect, the school it belongs to, and what it costs in mana. Which
-- spells exist is content; how they are learned, ranked and cast is not.
--
--   Spell.new(id)
--       :Called(name)             what a caster sees it as
--       :Effect(XEffect.X)        what casting it does
--       :School(MagicSchool.X)    which school it belongs to, and so which
--                                 rank a caster is judged by
--       :Cost(mana)               its price at rank 0; a caster grows
--                                 cheaper at it with practice
--       :Use(SpellUse.X)          what a caster reaches for it FOR, so the
--                                 AI can pick without naming spells one at
--                                 a time. ATTACK or HEALING; unsaid, it is
--                                 OTHER and no monster will spend a turn
--                                 on it
--       :Register()

Spell.new("cure_light_wounds")
	:Called("cure light wounds")
	:Effect(XEffect.CURE_LIGHT_WOUNDS)
	:School(MagicSchool.BODY)
	:Cost(5)
	:Use(SpellUse.HEALING)
	:Register()

Spell.new("cure_serious_wounds")
	:Called("cure serious wounds")
	:Effect(XEffect.CURE_SERIOUS_WOUNDS)
	:School(MagicSchool.BODY)
	:Cost(10)
	:Use(SpellUse.HEALING)
	:Register()

Spell.new("cure_critical_wounds")
	:Called("cure critical wounds")
	:Effect(XEffect.CURE_CRITICAL_WOUNDS)
	:School(MagicSchool.BODY)
	:Cost(15)
	:Use(SpellUse.HEALING)
	:Register()

Spell.new("cure_mortal_wounds")
	:Called("cure mortal wounds")
	:Effect(XEffect.CURE_MORTAL_WOUNDS)
	:School(MagicSchool.BODY)
	:Cost(20)
	:Use(SpellUse.HEALING)
	:Register()

Spell.new("heal")
	:Called("heal")
	:Effect(XEffect.HEAL)
	:School(MagicSchool.BODY)
	:Cost(30)
	:Use(SpellUse.HEALING)
	:Register()

Spell.new("heroism")
	:Called("heroism")
	:Effect(XEffect.HEROISM)
	:School(MagicSchool.BODY)
	:Cost(5)
	:Register()

Spell.new("cure_poison")
	:Called("cure poison")
	:Effect(XEffect.CURE_POISON)
	:School(MagicSchool.BODY)
	:Cost(15)
	:Register()

Spell.new("cure_disease")
	:Called("cure disease")
	:Effect(XEffect.CURE_DISEASE)
	:School(MagicSchool.BODY)
	:Cost(20)
	:Register()

Spell.new("burning_hands")
	:Called("burning hands")
	:Effect(XEffect.BURNING_HANDS)
	:School(MagicSchool.ELEMENTAL)
	:Cost(7)
	:Use(SpellUse.ATTACK)
	:Register()

Spell.new("ice_touch")
	:Called("ice touch")
	:Effect(XEffect.ICE_TOUCH)
	:School(MagicSchool.ELEMENTAL)
	:Cost(7)
	:Use(SpellUse.ATTACK)
	:Register()

Spell.new("magic_arrow")
	:Called("magic arrow")
	:Effect(XEffect.MAGIC_ARROW)
	:School(MagicSchool.ELEMENTAL)
	:Cost(5)
	:Use(SpellUse.ATTACK)
	:Register()

Spell.new("fire_bolt")
	:Called("fire bolt")
	:Effect(XEffect.FIRE_BOLT)
	:School(MagicSchool.ELEMENTAL)
	:Cost(12)
	:Use(SpellUse.ATTACK)
	:Register()

Spell.new("ice_bolt")
	:Called("ice bolt")
	:Effect(XEffect.ICE_BOLT)
	:School(MagicSchool.ELEMENTAL)
	:Cost(12)
	:Use(SpellUse.ATTACK)
	:Register()

Spell.new("lightning_bolt")
	:Called("lightning bolt")
	:Effect(XEffect.LIGHTNING_BOLT)
	:School(MagicSchool.ELEMENTAL)
	:Cost(18)
	:Use(SpellUse.ATTACK)
	:Register()

Spell.new("drain_life")
	:Called("drain life")
	:Effect(XEffect.DRAIN_LIFE)
	:School(MagicSchool.DEATH)
	:Cost(7)
	:Use(SpellUse.ATTACK)
	:Register()

Spell.new("acid_bolt")
	:Called("acid bolt")
	:Effect(XEffect.ACID_BOLT)
	:School(MagicSchool.DEATH)
	:Cost(25)
	:Use(SpellUse.ATTACK)
	:Register()

Spell.new("identify")
	:Called("identify")
	:Effect(XEffect.IDENTIFY)
	:School(MagicSchool.SURVIVING)
	:Cost(25)
	:Register()

Spell.new("summon_monster")
	:Called("summon monster")
	:Effect(XEffect.SUMMON_MONSTER)
	:School(MagicSchool.SURVIVING)
	:Cost(15)
	:Register()

Spell.new("create_item")
	:Called("create item")
	:Effect(XEffect.CREATE_ITEM)
	:School(MagicSchool.SURVIVING)
	:Cost(100)
	:Register()

Spell.new("blink")
	:Called("blink")
	:Effect(XEffect.BLINK)
	:School(MagicSchool.SURVIVING)
	:Cost(15)
	:Register()

Spell.new("self_knowledge")
	:Called("self knowledge")
	:Effect(XEffect.SELF_KNOWLEDGE)
	:School(MagicSchool.SURVIVING)
	:Cost(25)
	:Register()

Spell.new("see_invisible")
	:Called("see invisible")
	:Effect(XEffect.SEE_INVISIBLE)
	:School(MagicSchool.PROTECTION)
	:Cost(15)
	:Register()

Spell.new("acid_resistance")
	:Called("acid resistance")
	:Effect(XEffect.ACID_RESISTANCE)
	:School(MagicSchool.PROTECTION)
	:Cost(20)
	:Register()

Spell.new("fire_resistance")
	:Called("fire resistance")
	:Effect(XEffect.FIRE_RESISTANCE)
	:School(MagicSchool.PROTECTION)
	:Cost(15)
	:Register()

Spell.new("cold_resistance")
	:Called("cold resistance")
	:Effect(XEffect.COLD_RESISTANCE)
	:School(MagicSchool.PROTECTION)
	:Cost(15)
	:Register()

Spell.new("poison_resistance")
	:Called("poison resistance")
	:Effect(XEffect.POISON_RESISTANCE)
	:School(MagicSchool.PROTECTION)
	:Cost(15)
	:Register()

