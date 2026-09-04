-- What a character with the Create Trap skill can build.
--
-- The skill itself is engine: the menu, the mana arithmetic, placing the
-- trap and crediting the practice. What lives here is the list of what can
-- be built, in the order the menu offers it.
--
--   TrapRecipe.new(name, XTrap.TYPE)
--       :Level(n)          the CREATETRAP level it becomes available at
--       :Spell(Spell.X)   a magic trap: cast instead of loaded, and paid
--                          for in mana rather than in things
--       :Loads(kind, {types})
--                          what may go into it - any one of the types will
--                          do. Unsaid, it holds nothing, like a plain pit
--       :Tool(type, name)  what must be wielded to build it, and what to
--                          call it when the character is not holding one
--       :Practice(n)       how much building one exercises the skill
--       :Fills(min, max)   how many charges one found in a dungeon arrives
--                          with. Unsaid, the world never places a loaded
--                          one, which is what every built-only trap wants
--

local SPEARS = { "short_spear", "long_spear" }

TrapRecipe.new("Arrow trap", XTrap.ARROW)
	:Level(0)
	:Loads(ItemKind.MISSILE, { "arrow", "quarrel" })
	:Practice(15)
	:Fills(5, 9)
	:Register()

-- The same machine as the arrow trap, loaded with something heavier.
TrapRecipe.new("Spear trap", XTrap.ARROW)
	:Level(2)
	:Loads(ItemKind.WEAPON, SPEARS)
	:Practice(15)
	:Register()

TrapRecipe.new("Magic Arrow trap", XTrap.MAGICARROW)
	:Level(4)
	:Spell(Spell.MAGIC_ARROW)
	:Practice(10)
	:Register()

TrapRecipe.new("Fire Bolt trap", XTrap.FIREBOLT)
	:Level(6)
	:Spell(Spell.FIRE_BOLT)
	:Practice(10)
	:Register()

TrapRecipe.new("Pit", XTrap.PIT)
	:Level(8)
	:Tool("pickaxe", "pickaxe")
	:Practice(10)
	:Register()

TrapRecipe.new("Acid Bolt trap", XTrap.ACIDBOLT)
	:Level(10)
	:Spell(Spell.ACID_BOLT)
	:Practice(10)
	:Register()

TrapRecipe.new("Spear Pit", XTrap.SPEAR_PIT)
	:Level(12)
	:Loads(ItemKind.WEAPON, SPEARS)
	:Tool("pickaxe", "pickaxe")
	:Practice(20)
	:Fills(2, 4)
	:Register()
