-- Item definitions.
--
-- This file documents the three builders and loads the rest of the
-- directory; the definitions themselves live one kind to a file beside it.
--
-- There are three ways to define an item, for three different things:
--
--   Template.new(kind, type)  one sort of ordinary item - "a long sword",
--                             "a pair of boots" - and how often the game
--                             hands one out. What comes back is made of a
--                             material picked at random from the set the
--                             template allows, so no two are quite alike.
--                             These are the nine tables below.
--
--   Item.new(id)              one particular item, exactly as described.
--                             A quest piece, or an artifact.
--
--   Food.new(id)              the same, for something that can be eaten.
--
-- Food.new(id) and Item.new(id) register under that id; CreateObject(id)
-- then makes one, the same call that takes a C++ class name.
--
-- Template.new takes:
--
--   :View(name, view)         what it is called and how it looks
--   :Made(ItemSet, quality)   which materials it may be made from, and how
--                             well made it is
--   :Worth(value, weight)
--   :Armour(dv, pv)           dice strings; omitted when it protects nothing
--   :Combat(hit, dice, extra) dice strings; omitted when it strikes nothing
--   :Range(dice)              for bows and what they throw
--   :Skill(id)                which combat skill wielding one exercises,
--                             from world/combat_skills.lua
--   :Verb(word)               what one does to somebody - an axe hacks, a
--                             mace smashes. Unsaid, it hits
--   :Launcher(id)             for a missile: the skill of the weapon that
--                             fires it, so an arrow asks for a bow. Unsaid
--                             means nothing launches it and it can only be
--                             thrown - which anything can be anyway
--   :Chance(probability)      its weight in the draw against others of its
--                             kind
--   :OnCreate(function_name)  a Lua function the engine calls with each
--                             finished item of this sort, so content can
--                             have the last word on what came out of the
--                             pool: rename it, sharpen it, poison it, or
--                             hand out fewer of them. It receives the item
--                             and may change anything about it - see
--                             FinishMissile in missiles.lua. Unsaid, the
--                             plain item is the finished item
--   :OnOutfit(function_name)  a Lua function called with (item, creature)
--                             when a creature is being kitted out with one
--                             of these, so content can supply whatever the
--                             item needs to be worth carrying - the arrows
--                             that make a bow a weapon. See ArmLauncher in
--                             missile_weapons.lua. Unsaid, the item stands
--                             on its own, which is what almost all do
--
-- Only the things that belong to nobody in particular live in this file.
-- An item with a single owner is defined beside that owner instead, so
-- that everything about an NPC is in one place: Glamdring and the Death
-- Hack with Ozorick, the regalia with Roderick, the dwarven gear with
-- Torin, the mitre with the high priest, the black club with Xshee-Voo,
-- and Yohjishiro's two trophies with the errand that wants them. The
-- forest brother's cloak and the Avanor Defender sit in valley_extras.lua,
-- with the brotherhood and the grave they come from.
--
-- Item.new is for things that are nothing but a name, a look and some
-- numbers:
--
--   :View(name, view, color)
--   :Basic(ItemType, ItemKind, value, weight)
--   :Random(probability)          as below - unsaid means never drawn
--
-- Food.new adds eating to that:
--
--   :View(name, view, color)      what it is called and how it looks
--   :Basic(ItemType, value, wt)   its type, its price, its weight
--   :Nutrition(food, per_turn)    how much food it holds, how fast it goes
--   :Taste(FoodType)              how well it sits; NORMAL if unsaid
--   :Random(probability)          its weight in the draw when the game
--                                 wants "some food" and does not say which.
--                                 Left unsaid it is never drawn, which is
--                                 what a quest trophy wants - no shop
--                                 should ever stock a rat tail.
--
-- The ItemType must be distinct per food: XAnyFood::Compare() stacks on
-- nutrition and type together, so two trophies sharing a type would merge
-- into one heap however differently they are described. Register() says so
-- if two ever collide.

-- ARTIFACTS ----------------------------------------------------------------
--
-- One of a kind, placed by hand, and never generated at random.
--
--   :Weapon/:Cap/:Shield/:Cloak(ItemType)
--                        which kind it is, and which row of that kind's
--                        table it is built from - that decides the war
--                        skill it trains and the material it is made of,
--                        the two things an artifact does not restate
--   :View(name, view, color)
--                        what it looks like to someone who does not know
--                        it; view and color are optional, and without them
--                        it keeps its material's look
--   :Type(ItemType)      optional; only if it differs from the base's
--   :Basic(value, weight)
--   :Armour(dv, pv)
--   :Combat(to_hit, count, sides, bonus)
--   :Stats / :Resist     the same dice strings Monster.new takes
--   :Brand(AttackEffectType)
--   :Called(name)        what it is once known. Left unsaid, it reads as an
--                        ordinary item of its kind - which is what a
--                        disguise wants.
--   :Unique()            never merges with anything, not even another of
--                        itself
--
-- An artifact is not a kind of its own: it is an item of its base type that
-- says what it is Called, declares itself Unique, and never asks for a place
-- in the random draw. An ordinary sword will be the same definition with
-- :Random() instead of those last two. Artefacts shall be defined next to the
-- NPC/quests/locations where they are used.

function LoadItems()
	dofile("./world/items/weapons.lua")
	dofile("./world/items/missile_weapons.lua")
	dofile("./world/items/missiles.lua")
	dofile("./world/items/armour.lua")
	dofile("./world/items/shields.lua")
	dofile("./world/items/caps.lua")
	dofile("./world/items/cloaks.lua")
	dofile("./world/items/boots.lua")
	dofile("./world/items/gloves.lua")
	dofile("./world/items/tools.lua")
	dofile("./world/items/materials.lua")
	dofile("./world/items/armour_enchantments.lua")
	dofile("./world/items/enchantments.lua")
	dofile("./world/items/herbs.lua")
	dofile("./world/items/books.lua")
	dofile("./world/items/potions.lua")
	dofile("./world/items/scrolls.lua")
	dofile("./world/items/food.lua")
end
