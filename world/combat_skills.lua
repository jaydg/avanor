-- Combat skills: how good somebody is with a class of weapon.
--
-- Which classes exist follows entirely from what there is to fight with,
-- which is why this is content. A world of blasters and light sabers would
-- list those and no crossbow at all.
--
-- Weapons say which skill they train, in world/items/weapons.lua and
-- missile_weapons.lua (`:Skill(...)`), and missiles say which launcher
-- fires them (`:Launcher(...)`). The engine only ever passes those along -
-- it names no skill of its own.
--
--   CombatSkill.new(id)
--       :Called(name)        what the skill screen shows
--       :Group(CombatGroup.X)
--                            where the skill screen lists it: MELEE,
--                            MISSILE or SHIELD. Unsaid, MELEE
--       :Role(CombatRole.X)  the job this skill can answer for when the
--                            engine needs one and no weapon names it -
--                            fighting bare-handed, blocking, throwing by
--                            hand. Unsaid, it answers for nothing and is
--                            only ever trained by a weapon that names it
--       :Defence({...})      bonus per level, 0 upwards. A short row is
--       :ToHit({...})        padded with its last value, so a row of one
--       :Damage({...})       number means "and no better after that"
--       :Register()
--
-- More than one skill may take the same role. The engine asks for the best
-- one the fighter has, so a world can offer several ways of fighting
-- bare-handed - wrestling, boxing, a martial art - and whoever has come
-- furthest with any of them fights with that.

CombatSkill.new("unarmed")
	:Called("Unarmed fight")
	:Role(CombatRole.UNARMED)
	:Defence({ 0, 0, 0, 1, 1, 1, 2, 2, 3, 4, 5, 6, 7, 8, 10, 12 })
	:ToHit({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 })
	:Damage({ 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 4, 5, 7, 9, 12 })
	:Register()

CombatSkill.new("dagger")
	:Called("Daggers & Knives")
	:Defence({ 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 4, 5, 6 })
	:ToHit({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 16, 19 })
	:Damage({ 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 8 })
	:Register()

CombatSkill.new("sword")
	:Called("Swords")
	:Defence({ 0, 0, 1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 12 })
	:ToHit({ 0, 1, 1, 1, 2, 2, 2, 3, 3, 4, 4, 5, 6, 8, 10, 12 })
	:Damage({ 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 8, 10, 12, 14 })
	:Register()

CombatSkill.new("club")
	:Called("Clubs & Hammers")
	:Defence({ 0, 0, 1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 12 })
	:ToHit({ 0, 1, 1, 1, 2, 2, 2, 3, 3, 4, 4, 5, 6, 8, 10, 12 })
	:Damage({ 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 8, 10, 12, 14 })
	:Register()

CombatSkill.new("mace")
	:Called("Maces & Flails")
	:Defence({ 0, 0, 1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 12 })
	:ToHit({ 0, 1, 1, 1, 2, 2, 2, 3, 3, 4, 4, 5, 6, 8, 10, 12 })
	:Damage({ 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 8, 10, 12, 14 })
	:Register()

CombatSkill.new("polearm")
	:Called("Pole Arms")
	:Defence({ 0, 0, 1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 12 })
	:ToHit({ 0, 1, 1, 1, 2, 2, 2, 3, 3, 4, 4, 5, 6, 8, 10, 12 })
	:Damage({ 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 8, 10, 12, 14 })
	:Register()

CombatSkill.new("axe")
	:Called("Axes")
	:Defence({ 0, 0, 1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 12 })
	:ToHit({ 0, 1, 1, 1, 2, 2, 2, 3, 3, 4, 4, 5, 6, 8, 10, 12 })
	:Damage({ 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 8, 10, 12, 14 })
	:Register()

CombatSkill.new("stave")
	:Called("Staves")
	:Defence({ 0, 0, 1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 12 })
	:ToHit({ 0, 1, 1, 1, 2, 2, 2, 3, 3, 4, 4, 5, 6, 8, 10, 12 })
	:Damage({ 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 8, 10, 12, 14 })
	:Register()

CombatSkill.new("shield")
	:Called("Shields")
	:Group(CombatGroup.SHIELD)
	:Role(CombatRole.SHIELD)
	:Defence({ 0, 1, 2, 3, 5, 7, 9, 11, 12, 14, 16, 18, 20, 23, 26, 30 })
	:ToHit({ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 })
	:Damage({ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 })
	:Register()

CombatSkill.new("bow")
	:Called("Bows")
	:Group(CombatGroup.MISSILE)
	:Defence({ 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 5 })
	:ToHit({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 16, 18, 22 })
	:Damage({ 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 8, 10, 12, 14 })
	:Register()

CombatSkill.new("crossbow")
	:Called("Crossbows")
	:Group(CombatGroup.MISSILE)
	:Defence({ 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 3 })
	:ToHit({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 16, 18, 22 })
	:Damage({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16 })
	:Register()

CombatSkill.new("sling")
	:Called("Slings")
	:Group(CombatGroup.MISSILE)
	:Defence({ 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6 })
	:ToHit({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 16, 18, 22 })
	:Damage({ 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 3, 3, 4, 4, 6 })
	:Register()

CombatSkill.new("throw")
	:Called("Throwing items")
	:Group(CombatGroup.MISSILE)
	:Role(CombatRole.THROW)
	:Defence({ 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 5 })
	:ToHit({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 16, 18, 22 })
	:Damage({ 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 3, 3, 4, 4, 6 })
	:Register()
