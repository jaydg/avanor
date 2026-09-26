------------------------------ FIENDS ----------------------------------------
--
-- What is down there because it was called up, rather than because it
-- wandered in.
--
-- The mine's deep levels used to draw on the same pool as its first one, so
-- a hero six floors down met the dogs and cats that had strayed in off the
-- valley. These are the answer to that: creatures that belong under the
-- ground because they were never on it, for the rungs where a stray cat
-- stops making sense.
--
-- They take the "demon" class, which world/creature_classes.lua has always
-- declared and which until now held exactly one creature - Beelzevile, who
-- is UNIQUE and so never drawn by Settle(). The class was registered and
-- empty. Nothing new had to be invented to put them somewhere.
--
-- The glyphs follow the game's habit of a shape per sort: the lesser
-- fiends are 'u', the greater ones '&' as Beelzevile is, and the hell
-- hound keeps 'C' because it is a hound and reads as one - its colour is
-- what says otherwise.
--
-- On fire and cold: devils are creatures of the furnace and demons of the
-- pit, so all of them shrug off fire and none of them likes ice. That is
-- the one thread running through the whole file, and it is what a player
-- is meant to learn and then exploit.


-- THE LESSER SORT ---------------------------------------------------------
--
-- Met in numbers, on the rungs where the mine is still only unpleasant.

Monster.new("lemure")
	:View("lemure", 'u', xColor.xBROWN, PersonType.IT, CreatureTemplate.VERY_LOW, "demon")
	:Basic("0d0+100", "0d0+1000", CreatureSize.SMALL, "1d100+900")
	:Body("", 0)
	-- No COWARD: a lemure has nothing left to be frightened with. It comes
	-- on until it is cut down, which is the whole of what it is for.
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.FREE_WAY)
	:Stats("St 3d3 Dx 1d2 To 4d3 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d2 Ch 1d1")
	:Resist{ fire = "0d0+80", cold = "0d0-40" }
	:Combat("1d2", "1d4")
	:Main("0d0", "1d2", "1d8+2", "0d0")
	:Description("A mound of wet grey tallow in the rough shape of a man, "
		.. "running and re-forming as it comes. It has no face to read and "
		.. "makes no sound beyond the wet slap of itself against the floor. "
		.. "Whatever it was before it was sent here, nothing of that is "
		.. "left to appeal to.")
	:CorpseTaste("aversive")
	:Register()

Monster.new("dretch")
	:View("dretch", 'u', xColor.xLIGHTGREEN, PersonType.IT, CreatureTemplate.VERY_LOW, "demon")
	:Basic("0d0+100", "0d0+1000", CreatureSize.SMALL, "1d100+800")
	:AI(XStandardAI.CREATURE + XStandardAI.ALLOW_PACK)
	:Body("head neck body hand hand", 5)
	:Stats("St 4d3 Dx 2d3 To 3d3 Le 1d2 Wi 1d2 Ma 1d2 Pe 2d3 Ch 1d1")
	:Resist{ fire = "0d0+60", cold = "0d0-30" }
	:Combat("1d3", "1d5")
	:Main("1d2", "1d1", "1d8+3", "1d2")
	:Description("Squat, pot-bellied and altogether wretched, the dretch is "
		.. "what the pit makes when it is not paying attention. It is a "
		.. "coward alone and a menace in a dozen, and it is never alone. "
		.. "The smell arrives before it does.")
	:Melee("disease", 25)
	:CorpseTaste("aversive")
	:Register()

Monster.new("imp")
	:View("imp", 'u', xColor.xRED, PersonType.IT, CreatureTemplate.LOW, "demon")
	:Basic("0d0+111", "0d0+900", CreatureSize.VERY_SMALL, "1d50+200")
	:Body("head neck body", 2)
	:AI(XStandardAI.CREATURE)
	:Stats("St 2d3 Dx 5d4 To 2d3 Le 4d3 Wi 3d3 Ma 3d3 Pe 4d4 Ch 2d3")
	-- Unseen until it stings. The hero's own warning that something unseen
	-- is near is the only notice they get, which is the point of it.
	:Resist{ invisible = true, see_invisible = true, fire = "0d0+80", cold = "0d0-30" }
	:Combat("3d4", "1d4")
	:Main("4d3", "0d0", "2d4+4", "2d3")
	:Description("No larger than a cat and a great deal worse tempered, the "
		.. "imp is mostly tail and malice. It spends its time unseen and "
		.. "its patience is shorter than its attention, so what usually "
		.. "gives it away is the sting.")
	:Melee("poison", 60)
	:Register()

Monster.new("quasit")
	:View("quasit", 'u', xColor.xLIGHTMAGENTA, PersonType.IT, CreatureTemplate.LOW, "demon")
	:Basic("0d0+111", "0d0+900", CreatureSize.VERY_SMALL, "1d50+200")
	:Body("head neck body", 2)
	:AI(XStandardAI.CREATURE)
	:Stats("St 2d3 Dx 5d4 To 2d3 Le 3d3 Wi 3d3 Ma 4d3 Pe 4d4 Ch 1d3")
	:Resist{ invisible = true, see_invisible = true, fire = "0d0+50", cold = "0d0-30" }
	:Combat("3d4", "1d4")
	:Main("4d3", "0d0", "2d4+4", "2d4")
	:Description("The demons' answer to the imp, and no improvement on it. "
		.. "A knot of horns and claws that is rarely where you last saw it, "
		.. "the quasit prefers to make its victims afraid before it makes "
		.. "them bleed.")
	:Melee("poison", 40)
	:Melee("confuse", 30)
	:Register()


-- THE PACK ----------------------------------------------------------------

Monster.new("hell_hound")
	:View("hell hound", 'C', xColor.xRED, PersonType.IT, CreatureTemplate.ABOVE_LOW, "demon")
	:Basic("0d0+100", "1d100+700", CreatureSize.NORMAL, "1d200+1200")
	:Body("", 0)
	-- A hunting pack that does not break. The dogs upstairs flee when they
	-- are outmatched; these were bred somewhere that does not allow it.
	:AI(XStandardAI.FREE_WAY + XStandardAI.RANDOM_MOVE + XStandardAI.FIND_WAY
		+ XStandardAI.ALLOW_PACK)
	:Stats("St 5d4 Dx 4d3 To 4d4 Le 1d2 Wi 2d3 Ma 1d2 Pe 5d4 Ch 1d1")
	:Resist{ fire = "0d0+100", cold = "0d0-50" }
	:Combat("2d5", "2d5")
	:Main("2d4", "1d3", "2d8+4", "0d0")
	:Description("It hunts the way a dog hunts, in a line abreast and "
		.. "without hurry, and the heat comes off it in a wash you feel "
		.. "before you see the eyes. What it leaves behind it is not tracks "
		.. "but scorch.")
	:Melee("fire", 100)
	:CorpseTaste("aversive")
	:Register()


-- THE RANKS ---------------------------------------------------------------
--
-- Devils drill. These carry weapons and know how to use them, which since
-- the weapon-skill floor was made honest is something that now shows.

Monster.new("bearded_devil")
	:View("bearded devil", '&', xColor.xGREEN, PersonType.HE, CreatureTemplate.AVG, "demon")
	:Basic("0d0+100", "0d0+1000", CreatureSize.NORMAL, "1d200+1400")
	:Body("head neck body hand hand boots", 30)
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM
		+ XStandardAI.FREE_WAY + XStandardAI.FIND_WAY)
	:Stats("St 7d4 Dx 5d4 To 6d4 Le 3d3 Wi 4d3 Ma 2d3 Pe 4d3 Ch 1d3")
	:Resist{ fire = "0d0+100", cold = "0d0-40", poison = "0d0+60" }
	:Combat("3d5", "2d4")
	:Main("3d4", "2d2", "3d8+6", "1d4")
	:Description("Lean and grey and taller than a man, with a writhing mass "
		.. "of oiled snakes where a beard should be. It holds its glaive "
		.. "the way a soldier holds one, and it has clearly been shown how "
		.. "by somebody who minded whether it learned.")
	:LearnSkill(XSkill.FINDWEAKNESS, 8)
	:Equip(ItemKind.WEAPON, "halberd", 100)
	:Melee("disease", 40)
	:Register()

Monster.new("barbed_devil")
	:View("barbed devil", '&', xColor.xLIGHTRED, PersonType.HE, CreatureTemplate.AVG, "demon")
	:Basic("0d0+100", "0d0+1000", CreatureSize.NORMAL, "1d200+1600")
	-- Hands, so it can work a door, but nothing drawn: a barbed devil
	-- fights with what it is covered in.
	:Body("head neck body hand hand", 0)
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.FREE_WAY
		+ XStandardAI.FIND_WAY)
	:Stats("St 8d4 Dx 4d4 To 8d4 Le 3d3 Wi 4d3 Ma 4d3 Pe 5d4 Ch 1d2")
	:Resist{ fire = "0d0+100", cold = "0d0-40", poison = "0d0+60" }
	:Combat("3d5", "3d4")
	:Main("2d4", "3d3", "4d8+8", "3d4")
	:Description("Every inch of it is spines, from the crown of its head to "
		.. "the backs of its hands, and it holds still in a way that "
		.. "invites you to come closer and find out. When it tires of "
		.. "waiting it throws fire instead.")
	:LearnSkill(XSkill.CONCENTRATION, 6)
	:LearnSpell("fire_bolt")
	:Melee("fire", 70)
	:Register()


-- THE DEEP ----------------------------------------------------------------
--
-- The rungs where the mine had five creatures to draw on, three of them
-- undead.

Monster.new("vrock")
	:View("vrock", '&', xColor.xLIGHTGRAY, PersonType.IT, CreatureTemplate.AVG, "demon")
	:Basic("0d0+100", "1d100+900", CreatureSize.LARGE, "1d300+1800")
	:Body("head neck body hand hand", 0)
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.FREE_WAY + XStandardAI.FIND_WAY
		+ XStandardAI.ALLOW_PACK)
	:Stats("St 8d4 Dx 6d4 To 7d4 Le 2d3 Wi 3d3 Ma 3d3 Pe 6d4 Ch 1d2")
	:Resist{ fire = "0d0+70", cold = "0d0-30", poison = "0d0+80" }
	:Combat("3d5", "3d5")
	:Main("3d4", "2d2", "4d8+6", "2d4")
	:Description("A vulture the size of a man, if a vulture stood upright "
		.. "and had hands. It stinks of carrion and rot, and the noise it "
		.. "makes when it decides about you is not a cry so much as a "
		.. "physical blow.")
	:Melee("poison", 60)
	:Melee("stun", 30)
	:CorpseTaste("aversive")
	:CorpseModifier("poison", 40)
	:Register()

Monster.new("bone_devil")
	:View("bone devil", '&', xColor.xWHITE, PersonType.HE, CreatureTemplate.HI, "demon")
	:Basic("0d0+100", "0d0+1000", CreatureSize.LARGE, "1d300+1600")
	:Body("head neck body cloak hand hand ring ring", 40)
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM
		+ XStandardAI.FREE_WAY + XStandardAI.FIND_WAY)
	:Stats("St 9d5 Dx 7d4 To 8d5 Le 5d4 Wi 6d4 Ma 5d4 Pe 6d4 Ch 2d3")
	:Resist{ fire = "0d0+100", cold = "0d0-40", poison = "0d0+100" }
	:Combat("4d5", "3d5")
	:Main("5d4", "3d3", "4d9+10", "4d5")
	:Description("Nine feet of dry hide stretched over a frame that is "
		.. "mostly angles, with a hooked tail it carries arched over its "
		.. "own shoulder the way a scorpion does. It gives orders in a "
		.. "language you are glad not to understand.")
	:LearnSkill(XSkill.FINDWEAKNESS, 10)
	:LearnSkill(XSkill.HEALING, 8)
	:Equip(ItemKind.WEAPON, "halberd", 100)
	:Melee("poison", 100)
	:Register()

Monster.new("horned_devil")
	:View("horned devil", '&', xColor.xLIGHTCYAN, PersonType.HE, CreatureTemplate.HI, "demon")
	:Basic("1d10+90", "0d0+900", CreatureSize.LARGE, "1d400+2400")
	-- No random weapon: the tail and the fork are the point of it, and
	-- a horned devil turning up with a looted short sword is not.
	:Body("head neck body hand hand ring ring", 0)
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM
		+ XStandardAI.FREE_WAY + XStandardAI.FIND_WAY)
	:Stats("St 12d5 Dx 6d4 To 10d5 Le 5d4 Wi 6d4 Ma 6d4 Pe 6d4 Ch 3d3")
	:Resist{ fire = "0d0+100", cold = "0d0-40", poison = "0d0+100" }
	:Combat("5d5", "4d5")
	:Main("4d5", "4d3", "5d9+14", "5d5")
	:Description("Kin to the thing that got loose in the mushroom caves, "
		.. "and no happier to be here. Twelve feet of red hide and muscle "
		.. "under a rack of horns, with a barbed tail it uses first and a "
		.. "fork it uses when the tail has not been enough.")
	:LearnSkill(XSkill.FINDWEAKNESS, 10)
	:LearnSkill(XSkill.CONCENTRATION, 8)
	:LearnSpell("fire_bolt")
	:Melee("fire", 100)
	:Melee("poison", 50)
	:Register()


-- THE HUNGER --------------------------------------------------------------
--
-- Gluttony with a body, which is what a demon is.
--
-- The wendigo is worth a word, because it is easy to file wrongly. The
-- creature of ice and blizzards is a modern invention; Basil Johnston's
-- description is of something else entirely - "the ash-gray of death",
-- "suppuration of the flesh", "a strange and eerie odor of decay and
-- decomposition, of death and corruption". Not a frost spirit. A thing of
-- rot and appetite, never satisfied after consuming one victim and
-- constantly hunting the next, which is a demon by any reading and is
-- classed as one here.
--
-- That last part is not only flavour. A creature whose whole nature is
-- to keep looking for the next victim is what EXPLORER_MOVE was written
-- for, so these two get it: they work their way through a level rather
-- than milling about in a room waiting to be found.
--
-- They also carry the drain_life brand, which until now nothing in the
-- world did. The engine has always handled it - XCreature::
-- CausePostEffect(), half of what the victim loses the attacker gains -
-- and no creature or weapon had ever asked for it. A thing defined by its
-- hunger is what it was waiting for.

Monster.new("starveling")
	:View("starveling", 'W', xColor.xLIGHTGRAY, PersonType.IT, CreatureTemplate.AVG, "demon")
	:Basic("1d10+90", "0d0+1000", CreatureSize.NORMAL, "1d100+700")
	:Body("head neck body cloak hand hand boots", 20)
	-- No COWARD, and EXPLORER_MOVE: whatever judgement it had about odds
	-- went the way everything else did, and it does not wait to be found.
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM
		+ XStandardAI.FREE_WAY + XStandardAI.FIND_WAY + XStandardAI.EXPLORER_MOVE)
	:Stats("St 6d4 Dx 5d4 To 4d4 Le 2d3 Wi 1d2 Ma 1d2 Pe 5d4 Ch 1d1")
	:Resist{ fire = "0d0+40", cold = "0d0-20", poison = "0d0+80", disease = "0d0+100" }
	:Combat("3d4", "2d4")
	:Main("3d4", "0d0", "3d8+4", "1d3")
	:Description("The lesser hunger, and not lesser by much. Skin drawn "
		.. "tight enough to count the bones through it, the grey of three "
		.. "days dead, and the smell arrives well before it does. What it "
		.. "has for lips are torn and will not close. It watches your hands "
		.. "rather than your face.")
	:Melee("drain_life", 60)
	:Melee("disease", 30)
	:CorpseTaste("aversive")
	:Register()

Monster.new("wendigo")
	:View("wendigo", 'W', xColor.xWHITE, PersonType.IT, CreatureTemplate.HI, "demon")
	-- Quicker than anything else this deep. Outrunning one is not a plan.
	:Basic("0d0+111", "0d0+900", CreatureSize.LARGE, "1d200+1100")
	:Body("head neck body hand hand", 0)
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP
		+ XStandardAI.FREE_WAY + XStandardAI.FIND_WAY + XStandardAI.EXPLORER_MOVE)
	:Stats("St 10d5 Dx 9d4 To 8d5 Le 3d3 Wi 5d4 Ma 3d3 Pe 8d4 Ch 1d1")
	:Resist{ fire = "0d0+60", cold = "0d0-20", poison = "0d0+100", disease = "0d0+100" }
	:Combat("5d5", "4d4")
	:Main("6d4", "1d3", "4d9+10", "2d4")
	:Description("Gaunt past starvation, the skin desiccated and pulled so "
		.. "tight over the bones that they push back through it, and the "
		.. "complexion of it the ash-grey of death. The eyes have gone back "
		.. "deep into the skull. What lips it has are tattered and bloody. "
		.. "It gives off the smell of a thing that has been dead a while "
		.. "and has not stopped moving, and it has never once been full.")
	:Melee("drain_life", 100)
	:Melee("disease", 40)
	:CorpseTaste("aversive")
	:Register()
