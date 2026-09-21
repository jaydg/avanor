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
--                           eight times as many creatures as naming one.
--                           A table in place of the ceiling says more:
--                           { max = .., refresh = .., area = {x=, y=,
--                           w=, h=}, on = .. }, where `area` confines it
--                           to a patch of the location and `on` names the
--                           ground, both as Guardian's do
--   GuardianClass(ids, ...) posts one of these sorts, drawn at random, to
--                           guard a patch
--
-- Guardian() and GuardianClass() both end with either the AI flags or a
-- table saying more about where one may stand:
--
--   Guardian("sheep", GID, x, y, 19, 9, { on = XTileType.GREEN_GRASS })
--   Guardian("guard", GID, x, y, 8, 8, { flags = XStandardAI.NO_SWAP,
--                                        on = { XTileType.PATH,
--                                               XTileType.ROAD } })
--
-- `on` is the ground the spot is drawn from, one tile or several. Without
-- it any ground inside the patch will do, which is what a walled garden
-- used to need two separate patches to work around - see Yohjishiro's
-- flock in world/valley.lua, which grazes the grass and keeps out of her
-- tower because of this and not because of where it was put.
--   SetEnemy(cr, ids)       whom this one fights, replacing whatever it
--                           was told before
--   OnSenseUnseen(cr, id)   what the hero feels when one of these stands
--                           in plain sight but cannot be made out
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

-- WHAT CANNOT BE SEEN
--
-- A creature can be invisible - the undead of world/creatures/undead.lua
-- are so by nature, and anything that wears what it finds can become so by
-- putting on a ring of invisibility. Whether the hero makes one out is a
-- question of their see_invisible against its invisible, and losing that
-- comparison used to mean no word of it at all: the first sign of a dread
-- was being struck by nothing.
--
-- So the engine (XHero::SenseUnseen) asks here instead, for every creature
-- standing on a lit tile within sight that the hero cannot make out, and
-- says whatever comes back. It asks about the nearest one, when one
-- arrives rather than every turn it stays, and it says nothing at all if
-- this returns nothing - what a hero notices is not the engine's to word.
--
-- Deliberately vague: it tells you that something is there and roughly
-- what sort of thing, not where it stands or what it is. That is enough to
-- reach for a potion of see invisible, which is the whole point of saying
-- it.
function OnSenseUnseen(cr, class)
	if (class == "undead") then
		return "You feel a chill run up your spine."
	end

	return "You sense someone or something nearby."
end


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
