-- Spells.
--
-- A spell is an effect somebody learned to call up at will: it names the
-- effect, the school it belongs to, and what it costs in mana. Which
-- spells exist is content; how they are learned, ranked and cast is not.
--
--   Spell.new(id)
--       :Called(name)             what a caster sees it as
--       :Effect(id)               what casting it does - a row from
--                                 world/effects.lua
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
	:Effect("cure_light_wounds")
	:School(MagicSchool.BODY)
	:Cost(5)
	:Use(SpellUse.HEALING)
	:Register()

Spell.new("cure_serious_wounds")
	:Called("cure serious wounds")
	:Effect("cure_serious_wounds")
	:School(MagicSchool.BODY)
	:Cost(10)
	:Use(SpellUse.HEALING)
	:Register()

Spell.new("cure_critical_wounds")
	:Called("cure critical wounds")
	:Effect("cure_critical_wounds")
	:School(MagicSchool.BODY)
	:Cost(15)
	:Use(SpellUse.HEALING)
	:Register()

Spell.new("cure_mortal_wounds")
	:Called("cure mortal wounds")
	:Effect("cure_mortal_wounds")
	:School(MagicSchool.BODY)
	:Cost(20)
	:Use(SpellUse.HEALING)
	:Register()

Spell.new("heal")
	:Called("heal")
	:Effect("heal")
	:School(MagicSchool.BODY)
	:Cost(30)
	:Use(SpellUse.HEALING)
	:Register()

Spell.new("heroism")
	:Called("heroism")
	:Effect("heroism")
	:School(MagicSchool.BODY)
	:Cost(5)
	:Register()

Spell.new("cure_poison")
	:Called("cure poison")
	:Effect("cure_poison")
	:School(MagicSchool.BODY)
	:Cost(15)
	:Register()

Spell.new("cure_disease")
	:Called("cure disease")
	:Effect("cure_disease")
	:School(MagicSchool.BODY)
	:Cost(20)
	:Register()

Spell.new("burning_hands")
	:Called("burning hands")
	:Effect("burning_hands")
	:School(MagicSchool.ELEMENTAL)
	:Cost(7)
	:Use(SpellUse.ATTACK)
	:Register()

Spell.new("ice_touch")
	:Called("ice touch")
	:Effect("ice_touch")
	:School(MagicSchool.ELEMENTAL)
	:Cost(7)
	:Use(SpellUse.ATTACK)
	:Register()

Spell.new("magic_arrow")
	:Called("magic arrow")
	:Effect("magic_arrow")
	:School(MagicSchool.ELEMENTAL)
	:Cost(5)
	:Use(SpellUse.ATTACK)
	:Register()

Spell.new("fire_bolt")
	:Called("fire bolt")
	:Effect("fire_bolt")
	:School(MagicSchool.ELEMENTAL)
	:Cost(12)
	:Use(SpellUse.ATTACK)
	:Register()

Spell.new("ice_bolt")
	:Called("ice bolt")
	:Effect("ice_bolt")
	:School(MagicSchool.ELEMENTAL)
	:Cost(12)
	:Use(SpellUse.ATTACK)
	:Register()

Spell.new("lightning_bolt")
	:Called("lightning bolt")
	:Effect("lightning_bolt")
	:School(MagicSchool.ELEMENTAL)
	:Cost(18)
	:Use(SpellUse.ATTACK)
	:Register()

Spell.new("drain_life")
	:Called("drain life")
	:Effect("drain_life")
	:School(MagicSchool.DEATH)
	:Cost(7)
	:Use(SpellUse.ATTACK)
	:Register()

Spell.new("acid_bolt")
	:Called("acid bolt")
	:Effect("acid_bolt")
	:School(MagicSchool.DEATH)
	:Cost(25)
	:Use(SpellUse.ATTACK)
	:Register()

Spell.new("identify")
	:Called("identify")
	:Effect("identify")
	:School(MagicSchool.SURVIVING)
	:Cost(25)
	:Register()

Spell.new("summon_monster")
	:Called("summon monster")
	:Effect("summon_monster")
	:School(MagicSchool.SURVIVING)
	:Cost(15)
	:Register()

Spell.new("create_item")
	:Called("create item")
	:Effect("create_item")
	:School(MagicSchool.SURVIVING)
	:Cost(100)
	:Register()

Spell.new("blink")
	:Called("blink")
	:Effect("blink")
	:School(MagicSchool.SURVIVING)
	:Cost(15)
	:Register()

Spell.new("self_knowledge")
	:Called("self knowledge")
	:Effect("self_knowledge")
	:School(MagicSchool.SURVIVING)
	:Cost(25)
	:Register()

Spell.new("see_invisible")
	:Called("see invisible")
	:Effect("see_invisible")
	:School(MagicSchool.PROTECTION)
	:Cost(15)
	:Register()

Spell.new("acid_resistance")
	:Called("acid resistance")
	:Effect("acid_resistance")
	:School(MagicSchool.PROTECTION)
	:Cost(20)
	:Register()

Spell.new("fire_resistance")
	:Called("fire resistance")
	:Effect("fire_resistance")
	:School(MagicSchool.PROTECTION)
	:Cost(15)
	:Register()

Spell.new("cold_resistance")
	:Called("cold resistance")
	:Effect("cold_resistance")
	:School(MagicSchool.PROTECTION)
	:Cost(15)
	:Register()

Spell.new("poison_resistance")
	:Called("poison resistance")
	:Effect("poison_resistance")
	:School(MagicSchool.PROTECTION)
	:Cost(15)
	:Register()

