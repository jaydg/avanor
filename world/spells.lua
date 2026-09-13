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
--       :School(id)               which school it belongs to, and so which
--                                 rank a caster is judged by - one of the
--                                 rows declared just below
--       :Cost(mana)               its price at rank 0; a caster grows
--                                 cheaper at it with practice
--       :Use(SpellUse.X)          what a caster reaches for it FOR, so the
--                                 AI can pick without naming spells one at
--                                 a time. ATTACK or HEALING; unsaid, it is
--                                 OTHER and no monster will spend a turn
--                                 on it
--       :Register()

-- THE SCHOOLS
--
-- How the scholars of old divided the Power. Every spell belongs to
-- exactly one, and a caster is ranked in each separately: casting a spell
-- of a school trains that school, and the rank a caster holds in it adds
-- to the power and reach of every spell in it. Learning any spell of a
-- school a caster has never touched starts them at its first rank.
--
-- They live here rather than in a file of their own because a school
-- with no spells in it is nothing at all - the character sheet lists
-- them in the order below, and only the ones a caster has begun.
--
--   MagicSchool.new(id)
--       :Called(name)   what the character sheet calls it
--       :Register()

MagicSchool.new("elemental")
	:Called("Elemental")
	:Register()

MagicSchool.new("body")
	:Called("Body and Spirit")
	:Register()

MagicSchool.new("protection")
	:Called("Protection and Resistance")
	:Register()

MagicSchool.new("death")
	:Called("Death and Devastation")
	:Register()

MagicSchool.new("surviving")
	:Called("Surviving and Enlightenment")
	:Register()


-- THE SPELLS

Spell.new("cure_light_wounds")
	:Called("cure light wounds")
	:Effect("cure_light_wounds")
	:School("body")
	:Cost(5)
	:Use(SpellUse.HEALING)
	:Register()

Spell.new("cure_serious_wounds")
	:Called("cure serious wounds")
	:Effect("cure_serious_wounds")
	:School("body")
	:Cost(10)
	:Use(SpellUse.HEALING)
	:Register()

Spell.new("cure_critical_wounds")
	:Called("cure critical wounds")
	:Effect("cure_critical_wounds")
	:School("body")
	:Cost(15)
	:Use(SpellUse.HEALING)
	:Register()

Spell.new("cure_mortal_wounds")
	:Called("cure mortal wounds")
	:Effect("cure_mortal_wounds")
	:School("body")
	:Cost(20)
	:Use(SpellUse.HEALING)
	:Register()

Spell.new("heal")
	:Called("heal")
	:Effect("heal")
	:School("body")
	:Cost(30)
	:Use(SpellUse.HEALING)
	:Register()

Spell.new("heroism")
	:Called("heroism")
	:Effect("heroism")
	:School("body")
	:Cost(5)
	:Register()

Spell.new("cure_poison")
	:Called("cure poison")
	:Effect("cure_poison")
	:School("body")
	:Cost(15)
	:Register()

Spell.new("cure_disease")
	:Called("cure disease")
	:Effect("cure_disease")
	:School("body")
	:Cost(20)
	:Register()

Spell.new("burning_hands")
	:Called("burning hands")
	:Effect("burning_hands")
	:School("elemental")
	:Cost(7)
	:Use(SpellUse.ATTACK)
	:Register()

Spell.new("ice_touch")
	:Called("ice touch")
	:Effect("ice_touch")
	:School("elemental")
	:Cost(7)
	:Use(SpellUse.ATTACK)
	:Register()

Spell.new("magic_arrow")
	:Called("magic arrow")
	:Effect("magic_arrow")
	:School("elemental")
	:Cost(5)
	:Use(SpellUse.ATTACK)
	:Register()

Spell.new("fire_bolt")
	:Called("fire bolt")
	:Effect("fire_bolt")
	:School("elemental")
	:Cost(12)
	:Use(SpellUse.ATTACK)
	:Register()

Spell.new("ice_bolt")
	:Called("ice bolt")
	:Effect("ice_bolt")
	:School("elemental")
	:Cost(12)
	:Use(SpellUse.ATTACK)
	:Register()

Spell.new("lightning_bolt")
	:Called("lightning bolt")
	:Effect("lightning_bolt")
	:School("elemental")
	:Cost(18)
	:Use(SpellUse.ATTACK)
	:Register()

Spell.new("drain_life")
	:Called("drain life")
	:Effect("drain_life")
	:School("death")
	:Cost(7)
	:Use(SpellUse.ATTACK)
	:Register()

Spell.new("acid_bolt")
	:Called("acid bolt")
	:Effect("acid_bolt")
	:School("death")
	:Cost(25)
	:Use(SpellUse.ATTACK)
	:Register()

Spell.new("identify")
	:Called("identify")
	:Effect("identify")
	:School("surviving")
	:Cost(25)
	:Register()

Spell.new("summon_monster")
	:Called("summon monster")
	:Effect("summon_monster")
	:School("surviving")
	:Cost(15)
	:Register()

Spell.new("create_item")
	:Called("create item")
	:Effect("create_item")
	:School("surviving")
	:Cost(100)
	:Register()

Spell.new("blink")
	:Called("blink")
	:Effect("blink")
	:School("surviving")
	:Cost(15)
	:Register()

Spell.new("self_knowledge")
	:Called("self knowledge")
	:Effect("self_knowledge")
	:School("surviving")
	:Cost(25)
	:Register()

Spell.new("see_invisible")
	:Called("see invisible")
	:Effect("see_invisible")
	:School("protection")
	:Cost(15)
	:Register()

Spell.new("acid_resistance")
	:Called("acid resistance")
	:Effect("acid_resistance")
	:School("protection")
	:Cost(20)
	:Register()

Spell.new("fire_resistance")
	:Called("fire resistance")
	:Effect("fire_resistance")
	:School("protection")
	:Cost(15)
	:Register()

Spell.new("cold_resistance")
	:Called("cold resistance")
	:Effect("cold_resistance")
	:School("protection")
	:Cost(15)
	:Register()

Spell.new("poison_resistance")
	:Called("poison resistance")
	:Effect("poison_resistance")
	:School("protection")
	:Cost(15)
	:Register()

