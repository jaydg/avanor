-- Item definitions
--
-- Food.new(id) and Item.new(id) register under that id; CreateObject(id)
-- then makes one, the same call that takes a C++ class name.
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

-- What a skeleton leaves behind, and what is under a headstone. Dropped by
-- the world tally (world/tally.lua) and by the graves in the valley.

Food.new("bone")
	:View("bone", '%', xColor.xWHITE)
	:Basic(ItemType.BONE, 1, 5)
	:Nutrition(10, 10)
	:Register()


-- Ordinary fare, and the only foods the game hands out on its own: when
-- something asks for food without saying which, one of these four is drawn.
-- They were a four-row table in C++ (rations_db) behind a class whose only
-- method forwarded straight to its base - data wearing a class.
--
-- The per-turn figures are what that table computed as (nutrition * 20) /
-- weight, worked out once and written down: heavier food is chewed through
-- more slowly.

Food.new("large_ration")
	:View("large ration", '%', xColor.xLIGHTGRAY)
	:Basic(ItemType.LARGERATION, 5, 100)
	:Nutrition(400, 80)
	:Random(100)
	:Register()

Food.new("ration")
	:View("ration", '%', xColor.xBROWN)
	:Basic(ItemType.RATION, 3, 70)
	:Nutrition(250, 71)
	:Random(100)
	:Register()

Food.new("small_ration")
	:View("small ration", '%', xColor.xBROWN)
	:Basic(ItemType.SMALLRATION, 1, 40)
	:Nutrition(150, 75)
	:Random(100)
	:Register()

Food.new("elvish_waybread")
	:View("elvish waybread", '%', xColor.xYELLOW)
	:Basic(ItemType.ELVISHWAYBREAD, 15, 15)
	:Nutrition(700, 933)
	:Taste(FoodType.GOOD)
	:Random(100)
	:Register()


-- The three parts Ahk-Ulan wants brought to him, and the only reason any
-- of them exists. Scattered by hand where they lie (world/valley.lua and
-- world/locations/dwarven_city.lua) and never generated at random - a shop
-- selling one would break the errand.

Item.new("ancient_machine_part")
	:Plain(ItemType.ANCIENTMACHINEPART, ItemKind.TOOL)
	:View("ancient machine part", ']', xColor.xDARKGRAY)
	:Basic(1000, 15)
	:Register()
