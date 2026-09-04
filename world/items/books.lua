-- Spell books.
--
-- A book teaches exactly one spell, so the spell's id names the book too -
-- there is no separate list of book names to keep in step with the spells.
-- Reading one takes as many turns as the reader's literacy and wit allow;
-- that part is engine.
--
--   Book.new(spell_id)
--       :Chance(rarity)    its weight in the draw against other books, and
--                          also what it is worth: a book nobody finds is a
--                          book worth a great deal
--       :Register()

-- What an unread book looks like. One of these is dealt to each book as
-- this file loads, and dealt afresh every game - so learning that the
-- "old tome" was fire bolt teaches you nothing about your next game.
-- There must be at least as many looks here as there are books below.
BookAppearances({
	"old tome", "small tome", "ancient tome", "dirty tome", "heavy tome",
	"old book", "small book", "ancient book", "dirty book", "heavy book",
	"wrapped tome", "pocket book", "leather-bound tome", "parchment book",
	"thin book", "gold decorated", "silver decorated",
})

Book.new("burning_hands")
	:Chance(100)
	:Register()

Book.new("ice_touch")
	:Chance(100)
	:Register()

Book.new("cure_light_wounds")
	:Chance(150)
	:Register()

Book.new("drain_life")
	:Chance(70)
	:Register()

Book.new("identify")
	:Chance(20)
	:Register()

Book.new("magic_arrow")
	:Chance(200)
	:Register()

Book.new("fire_bolt")
	:Chance(50)
	:Register()

Book.new("ice_bolt")
	:Chance(50)
	:Register()

Book.new("lightning_bolt")
	:Chance(20)
	:Register()

Book.new("acid_bolt")
	:Chance(10)
	:Register()

Book.new("cure_disease")
	:Chance(60)
	:Register()

Book.new("cure_poison")
	:Chance(80)
	:Register()

Book.new("blink")
	:Chance(15)
	:Register()

Book.new("self_knowledge")
	:Chance(5)
	:Register()
