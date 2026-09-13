-- What sorts of creature there are.
--
-- A creature is one sort and one only: a rat is a rat, a lich is undead.
-- The sorts matter because almost everything that wants to talk about a
-- group of creatures rather than a named one talks in these terms - a
-- sword that slays orcs, a guard posted to keep the goblins out, a
-- generator that fills a cave with vermin - and because a few of them
-- behave differently when they die.
--
--   CreatureClass.new(id)
--       :NoCorpse()      one of these leaves no body behind
--       :Slain(verb)     the verb for finishing one off. Unsaid, "kill"
--       :Enemy()         a creature that has been told nothing about whom
--                        to fight counts this sort an enemy
--       :Folk()          these are the people a guard is posted to
--                        protect rather than something for it to fight
--       :Register()
--
--
-- WHO IS WHAT
--
-- Every creature says its own sort, and must: one that names a class no
-- file below declares is reported by name while the world loads.
--
--   world/creatures/*.lua   :View(name, glyph, colour, person, level, id)
--   world/uniques/*.lua     - the last argument. Inherited, so a
--                             Monster.new("large_rat", "rat") is a rat too
--   world/hero.lua          the `class` field of a HERO_RACES entry, which
--                             InitHero() hands to SetCreatureClass().
--                             Unsaid, "human" - which is what every race
--                             was when the engine decided this itself
--
--
-- WHAT AIMS AT A SORT
--
-- All of these take an id, and most take either one id or a list of them:
--
--   world/brands.lua        :Slays(id)            triple damage to these
--   world/effects.lua       :Summons(id)          what summon_monster
--                                                 calls up
--   Settle(ids, level, ...) fills the current location with them over
--                           time. The ceiling counts each sort
--                           separately, so naming eight settles up to
--                           eight times as many creatures as naming one
--   GuardianClass(ids, ...) posts one of these sorts, drawn at random, to
--                           guard a patch
--   SetEnemy(cr, ids)       whom this one fights, replacing whatever it
--                           was told before
--   GetCreatureClass(cr)    what sort it is, to compare with "=="
--   SetCreatureClass(cr, id)
--   GetCreatureCount(loc, id)
--                           how many of that exact sort are in a location
--   the death hook          world/tally.lua is handed the dead creature's
--                           sort as its third argument
--
--
-- WHOM A CREATURE FIGHTS
--
-- Nothing in the engine decides this either. A creature that has been
-- given no orders treats every sort marked :Enemy() as an enemy - which
-- today is these ten:
--
--   rat  feline  canine  reptile  insect  human  kobold  undead  goblin
--   humanoid
--
-- and leaves orcs, giants, demons, blobs and whatever "other" covers out
-- of it: they are somebody else's quarrel, and whoever wants them fought
-- says so with SetEnemy() or GuardianClass().
--
-- Guardian() posts a guard on the same list minus the sorts marked
-- :Folk(), on the reasoning that a guard is there to protect people
-- rather than fight them. With human and humanoid marked, a guard fights:
--
--   rat  feline  canine  reptile  insect  kobold  undead  goblin
--
-- So :Enemy() widens both lists and :Folk() narrows the second one only.
-- Marking a new sort :Enemy() makes every leaderless monster in the world
-- hostile to it at once, which is rarely what a single quest wants -
-- SetEnemy() on the creatures that care is the smaller instrument.
--
-- Two exceptions the engine applies on top: a creature carrying the
-- PEACEFUL AI flag is set on nobody at all whatever this file says, and
-- a shopkeeper likewise.
--
--
-- DYING
--
-- :NoCorpse() and :Slain() are the whole of what a sort changes about
-- death. Undead use both - there is nothing left of one to eat or to
-- raise, and one is destroyed rather than killed, so the blow that
-- finishes one off reads "and destroys it." where a rat would read "and
-- kills it." Everything else leaves a body roughly one death in five.
--
-- Note that a corpse's own behaviour - how long it keeps, what eating it
-- does, how it tastes - is said per creature in world/creatures/ with
-- :Corpse(), :CorpseTaste() and the rest, not here. This file only
-- decides whether there is a corpse at all.

CreatureClass.new("rat")
	:Enemy()
	:Register()

CreatureClass.new("feline")
	:Enemy()
	:Register()

CreatureClass.new("canine")
	:Enemy()
	:Register()

CreatureClass.new("reptile")
	:Enemy()
	:Register()

CreatureClass.new("insect")
	:Enemy()
	:Register()

CreatureClass.new("human")
	:Enemy()
	:Folk()
	:Register()

-- Not :Enemy(): orcs, giants, demons and blobs have never been on the
-- list a monster fights by default. They are somebody else's quarrel,
-- and whoever wants them fought says so - see Guardian() and SetEnemy().
CreatureClass.new("orc")
	:Register()

CreatureClass.new("giant")
	:Register()

CreatureClass.new("kobold")
	:Enemy()
	:Register()

-- Nothing is left of one to eat or to raise, and it is destroyed rather
-- than killed.
CreatureClass.new("undead")
	:NoCorpse()
	:Slain("destroy")
	:Enemy()
	:Register()

CreatureClass.new("goblin")
	:Enemy()
	:Register()

CreatureClass.new("demon")
	:Register()

CreatureClass.new("humanoid")
	:Enemy()
	:Folk()
	:Register()

-- A warm mass, an ooze.
CreatureClass.new("blob")
	:Register()

CreatureClass.new("other")
	:Register()
