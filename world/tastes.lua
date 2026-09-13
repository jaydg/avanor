-- How well what you eat sits.
--
-- A taste is nothing but what the eater says of it afterwards - "You find
-- that the apple is delicious." - and never touches how much good the meal
-- does. A corpse that makes the hero sick says so through
-- :CorpseEffect(CorpseEffectType.VOMIT, ...) in world/creatures/, quite
-- separately from how it tastes; the two are declared together only
-- because it would be odd to be sick on something delicious.
--
--   Taste.new(id)
--       :Called(text)   what the eater says of it
--       :Ordinary()     the middling one - see below. Exactly one taste
--                       should be marked, and everything nobody has said
--                       a word about tastes of it
--       :Register()
--
-- Who says what tastes of what:
--
--   world/items/food.lua      :Taste(id)        on a food template
--   world/creatures/*.lua     :CorpseTaste(id)  on a monster, for its
--                                               corpse. Inherited, so a
--                                               Monster.new("large_rat",
--                                               "rat") tastes as a rat
--                                               does unless it says
--                                               otherwise
--
--
-- THE ORDER MATTERS
--
-- Declare them from the best down to the worst. The engine never names a
-- taste, but it does step along this list, because the same meal is not
-- the same meal to every stomach.
--
-- What sort of stomach the hero has comes from the race they chose - the
-- `food` field of each entry in HERO_RACES, world/hero.lua:
--
--   FoodFeeling.SENSITIVE   everything tastes one step worse than it is,
--                           and the worst there is cannot get worse.
--                           Halflings
--   FoodFeeling.NORMAL      what the row says, unshifted. Humans, both
--                           kinds of elf, dwarves, gnomes
--   FoodFeeling.TOLERANT    nothing tastes worse than the ordinary one -
--                           and, oddly but as it has always been, nothing
--                           tastes better than it either. Half-orcs
--
-- Which is why :Ordinary() has to be marked: it is the floor and the
-- ceiling a tolerant stomach settles on. So the six below come out as:
--
--            declared     tolerant     normal       sensitive
--   best     delicious    tasty        delicious    very tasty
--   good     very tasty   tasty        very tasty   tasty
--   normal   tasty        tasty        tasty        unpleasant
--   bad      unpleasant   tasty        unpleasant   repulsive
--   aversive repulsive    unpleasant   repulsive    nauseating
--   emetic   nauseating   repulsive    nauseating   nauseating
--
-- Note what a half-orc gets away with: a bad ration is merely tasty to
-- one, while a halfling finds the same ration repulsive. Adding a taste
-- at either end shifts that for everyone, which is the whole of what
-- declaring one does.
--
-- One exception: a corpse tastes of what it is, whatever stomach is
-- receiving it - XCorpse::postEat overrides the shift, as it always has.
-- So the table above applies in full to food from world/items/food.lua,
-- and only its "declared" column to corpses: an emetic rat is nauseating
-- to a half-orc and a halfling alike.

Taste.new("best")
	:Called("delicious")
	:Register()

Taste.new("good")
	:Called("very tasty")
	:Register()

Taste.new("normal")
	:Called("tasty")
	:Ordinary()
	:Register()

Taste.new("bad")
	:Called("unpleasant")
	:Register()

Taste.new("aversive")
	:Called("repulsive")
	:Register()

Taste.new("emetic")
	:Called("nauseating")
	:Register()
