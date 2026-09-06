-- What a trap does when it goes off.
--
-- The engine knows only the shapes below: a trap either brings about an
-- effect, or shoots what it is loaded with, or hurts whoever fell in. What
-- sorts of trap a world has is content.
--
--   TrapType.new(id)
--       :Looks(xColor.X)      the colour it is drawn in
--       :Casts(effect)        a magic trap: the effect it brings about
--       :Fires(to_hit)        shoots one charge of what it is loaded with
--       :Impales(to_hit, message)
--                             everything it holds, at once
--       :Hurts(count, sides, bonus, to_hit, message)
--                             plain dice, holding nothing
--       :Pit()                a hole: whoever falls in must climb out, and
--                             may fall in again trying
--       :Register()

TrapType.new("magic_arrow")
	:Looks(xColor.xBROWN)
	:Casts("magic_arrow")
	:Register()

TrapType.new("fire_bolt")
	:Looks(xColor.xRED)
	:Casts("fire_bolt")
	:Register()

TrapType.new("acid_bolt")
	:Looks(xColor.xGREEN)
	:Casts("acid_bolt")
	:Register()

TrapType.new("teleport")
	:Looks(xColor.xLIGHTGREEN)
	:Casts("teleport")
	:Register()

TrapType.new("arrow")
	:Looks(xColor.xBROWN)
	:Fires(30)
	:Register()

TrapType.new("pit")
	:Looks(xColor.xDARKGRAY)
	:Hurts(1, 30, 1, 10000, "the bottom of the pit")
	:Pit()
	:Register()

TrapType.new("spear_pit")
	:Looks(xColor.xDARKGRAY)
	:Impales(10000, "the spears in the pit")
	:Pit()
	:Register()


-- What a character with the Create Trap skill can build.
--
-- The skill itself is engine: the menu, the mana arithmetic, placing the
-- trap and crediting the practice. What lives here is the list of what can
-- be built, in the order the menu offers it.
--
--   TrapRecipe.new(name, type)   the id of a trap type above
--       :Level(n)          the CREATETRAP level it becomes available at
--       :Spell(id)         a magic trap: cast instead of loaded, and paid
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

TrapRecipe.new("Arrow trap", "arrow")
	:Level(0)
	:Loads(ItemKind.MISSILE, { "arrow", "quarrel" })
	:Practice(15)
	:Fills(5, 9)
	:Register()

-- The same machine as the arrow trap, loaded with something heavier.
TrapRecipe.new("Spear trap", "arrow")
	:Level(2)
	:Loads(ItemKind.WEAPON, SPEARS)
	:Practice(15)
	:Register()

TrapRecipe.new("Magic Arrow trap", "magic_arrow")
	:Level(4)
	:Spell("magic_arrow")
	:Practice(10)
	:Register()

TrapRecipe.new("Fire Bolt trap", "fire_bolt")
	:Level(6)
	:Spell("fire_bolt")
	:Practice(10)
	:Register()

TrapRecipe.new("Pit", "pit")
	:Level(8)
	:Tool("pickaxe", "pickaxe")
	:Practice(10)
	:Register()

TrapRecipe.new("Acid Bolt trap", "acid_bolt")
	:Level(10)
	:Spell("acid_bolt")
	:Practice(10)
	:Register()

TrapRecipe.new("Spear Pit", "spear_pit")
	:Level(12)
	:Loads(ItemKind.WEAPON, SPEARS)
	:Tool("pickaxe", "pickaxe")
	:Practice(20)
	:Fills(2, 4)
	:Register()
