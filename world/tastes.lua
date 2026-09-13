-- How well what you eat sits.
--
-- Nothing more than what the eater says of it afterwards - "You find that
-- the apple is delicious." - but the order matters: a delicate stomach
-- finds everything one step worse than it is, and a stomach that takes
-- anything settles on the ordinary one. Declare them from the best down
-- to the worst, and the engine steps along that order without ever
-- naming a taste of its own.
--
--   Taste.new(id)
--       :Called(text)   what the eater says of it
--       :Ordinary()     the one a stomach that takes anything settles
--                       on. Exactly one taste should be marked
--       :Register()

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
	:Called("tasteless")
	:Register()

Taste.new("very_bad")
	:Called("taste like a slops")
	:Register()

Taste.new("vomit")
	:Called("vomit")
	:Register()
