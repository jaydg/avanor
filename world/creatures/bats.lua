------------------------------ BATS --------------------------------------

Monster.new("bat")
	:View("bat", 'b', xColor.xBROWN, PersonType.IT, CreatureTemplate.VERY_LOW, "bat")
	:Basic("1d30+120", "0d0+1000", "1d200+800", CreatureSize.VERY_SMALL, "3d4")
	:Body("", 0)
	:AI(XStandardAI.COWARD + XStandardAI.RANDOM_MOVE)
	:Stats("St 1d1 Dx 1d4 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 3d10 Ch 1d1")
	:Resist{ fire = "5d5-50", see_invisible = true }
	:Combat("0d0", "1d2")
	:Main("1d4", "0d0", "1d2", "0d0")
	:Description("Flapping wings and squeaks in the darkness are a common "
		.. "sound to all who enter the caves of Avanor. Usually bats leave "
		.. "travlers alone, but sickness and magic sometimes cause them to "
		.. "attack.")
	:Register()

Monster.new("huge_bat", "bat")
	:View("huge bat", 'b', xColor.xLIGHTGRAY, PersonType.IT, CreatureTemplate.VERY_LOW, "bat")
	:Basic("1d30+150", "0d0+1000", "1d200+700", CreatureSize.VERY_SMALL, "10d4")
	:Stats("St 1d4 Dx 1d6 To 1d2 Pe 4d10")
	:Resist{ see_invisible = true }
	:Combat("1d2", "1d4")
	:Main("1d4", "1d1", "1d6", "0d0")
	:Description("With a wing span up to 10 feet, these bats can carry away "
		.. "much larger prey than their smaller cousins. They have been "
		.. "seen carrying creatures as large as a wolf away to feed their "
		.. "young in the dark corners of their cave. If they can they will "
		.. "take down any prey available and them dismember it with razor "
		.. "sharp teeth to make it easier to carry.")
	:Register()

-- Neither bat nor monkey and unpleasant as both. Where the ordinary bats
-- are a noise in the dark that mostly leaves you alone, these come down
-- the passage at you in a mob.
--
-- Not inherited from "bat": a mongbat has hands, a body worth putting
-- something on, and a temper, which is nearly everything the bat template
-- says. What it keeps is the echolocation - see_invisible - and it earns
-- that the same way its cousins do.
Monster.new("mongbat")
	:View("mongbat", 'b', xColor.xLIGHTRED, PersonType.IT, CreatureTemplate.LOW, "bat")
	:Basic("1d20+85", "0d0+900", "1d100+700", CreatureSize.SMALL, "1d50+300")
	:Body("head neck body hand hand", 5)
	-- A flock, and a cowardly one: alone it thinks better of it, and in a
	-- dozen it never has to. ALLOW_PACK is what makes the difference
	-- between a nuisance and a bad afternoon.
	:AI(XStandardAI.HI_ANIMAL + XStandardAI.ALLOW_PACK + XStandardAI.ALLOW_PICK_UP)
	-- Dexterity is the whole of its defence. It is small, quick and hard
	-- to land a blow on; everything else about it is slight.
	:Stats("St 2d3 Dx 6d4 To 1d3 Le 1d3 Wi 1d2 Ma 1d2 Pe 4d8 Ch 1d1")
	:Resist{ fire = "5d5-40", see_invisible = true }
	:Combat("3d4", "2d3")
	:Main("5d3", "0d0", "1d6+2", "0d0")
	:Description("A thing the size of a small boy with the face of a bat "
		.. "and the hands of a monkey, matted brown fur over all of it and "
		.. "the talons never sheathed. One is a nuisance. They do not come "
		.. "as one: they come down the passage in a chattering mob, and the "
		.. "ill-prepared are pulled down by sheer weight of them.")
	:CorpseTaste("aversive")
	:Register()

Monster.new("greater_mongbat", "mongbat")
	:View("greater mongbat", 'b', xColor.xRED, PersonType.IT, CreatureTemplate.ABOVE_LOW, "bat")
	:Basic("1d20+80", "0d0+900", "1d100+600", CreatureSize.NORMAL, "1d100+600")
	:Stats("St 4d4 Dx 7d4 To 2d4 Le 1d3 Wi 1d3 Ma 1d2 Pe 5d8 Ch 1d1")
	:Combat("4d4", "3d3")
	:Main("6d3", "1d1", "2d6+4", "0d0")
	:Description("Older, heavier and no better tempered, with a wingspan "
		.. "that fills the passage and scars enough to show it has been "
		.. "doing this a while. It leads, in the loose sense that a mob has "
		.. "a leader, and the rest come down behind it.")
	:Register()
