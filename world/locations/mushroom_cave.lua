-----------------------------------------------------------
-------------------- MUSHROOM CAVE ------------------------

--           000  entr cave(1)
--            |
--           000  demon cave (2)
--          / |
-- misc(3)000 |
--         | 000 mushroom cave (5)
--         |
--        000 kobold cave (4)

Monster.new("magnush")
	:View("Magnush, the Kobold Lord", 'k', xColor.xRED, PersonType.NAMED_HE, CreatureTemplate.UNIQUE, CreatureClass.KOBOLD)
	:Basic("1d10+120", "0d0+1000", "0d0+1000", CreatureSize.SMALL, "1d200+900")
	:Body("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 5, 0)
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM)
	:Stats("St 4d5 Dx 3d5 To 5d3 Le 2d4 Wi 3d5 Ma 3d3 Pe 1d8 Ch 1d4")
	:Resist("")
	:Combat("1d4", "1d3")
	:Main("1d4", "1d2", "2d6", "2d3")
	:Description("Magnush the kobold lord stands before you with all the majesty he can muster.  Bigger than most kobolds (some say his father was a gnoll), he has managed to bully his way to the prime position in his pack.  His mangy fur and crooked legs give him an almost laughable appearance though and you have difficulty hiding your mirth from this overgrown puppy.")
	:LearnSkill(SKT_HEALING, 10)
	:LearnSkill(SKT_FINDWEAKNESS, 10)
	:EquipCount(ItemKind.SCROLL + ItemKind.POTION + ItemKind.BOOK + ItemKind.FOOD, 6, 20)
	:Equip(ItemKind.WEAPON, ItemType.DAGGER, 100)
	:CorpseEffect(CET_DISEASE, 30)
	:LearnSpell(SPELL_CURE_SERIOUS_WOUNDS)
	:Unique()
	:Register()

function MakeMushroomCave()
	CreateLocation("MUSHROOMS_CAVE1", "MC:1", "Mushroom Caves Level 1", XLocation.CAVE, CAVE)
		Way(XStairWay.UP, "MAIN")
		Way(XStairWay.DOWN, "MUSHROOMS_CAVE2")
		Settle(CreatureClass.INSECT + CreatureClass.REPTILE, CreatureTemplate.VERY_LOW)
		Creature("small_snake")
		Creature("small_snake")
		Creature("small_snake")
		Creature("small_snake")
		Creature("spider")
		Creature("spider")
		Creature("spider")

	CreateLocation("MUSHROOMS_CAVE2", "MC:2", "Mushroom Caves Level 2", XLocation.CAVE, CAVE)
		Way(XStairWay.UP, "MUSHROOMS_CAVE1")
		Way(XStairWay.DOWN, "MUSHROOMS_CAVE3")
		Way(XStairWay.DOWN, "MUSHROOMS_CAVE5")
		CreateBeelzevile()
		Settle(CreatureClass.INSECT + CreatureClass.REPTILE, CreatureTemplate.VERY_LOW)

	CreateLocation("MUSHROOMS_CAVE3", "KC:1", "Kobold Cavern Level 1", XLocation.CAVE, CAVE)
		Way(XStairWay.UP, "MUSHROOMS_CAVE2")
		Way(XStairWay.DOWN, "MUSHROOMS_CAVE4")
		Settle(CreatureClass.KOBOLD + CreatureClass.INSECT + CreatureClass.REPTILE, CreatureTemplate.VERY_LOW)

	CreateLocation("MUSHROOMS_CAVE4", "KC:2", "Kobold Cavern Level 2", XLocation.CAVE, CAVE)
		Way(XStairWay.UP, "MUSHROOMS_CAVE3")
		Settle(CreatureClass.KOBOLD, CreatureTemplate.LOW + CreatureTemplate.VERY_LOW)
		Creature("magnush")

	CreateLocation("MUSHROOMS_CAVE5", "MC:3", "Mushroom Caves Level 3", XLocation.CAVE, CAVE)
		Way(XStairWay.UP, "MUSHROOMS_CAVE2")
		EventPlace('MushroomCaveEvent')
		CreateTimerEvent('SpawnMushroomEvent', 40000)
end

function MushroomCaveEvent(e, p)
	if (e == LuaEvent.MOVE) then
		if (isHero(p) and Rand(3) == 0) then
			AddMessage("You feel power swirling in the air...")
		end
		if (Rand(30) == 0) then
			local st = Rand(XStats.COUNT)
			if (GetStats(p, st) > 5) then
				ChangeStats(p, st, -1)
			end
		end
	end
end

local MUSHROOM_CLASS = "XMushSpawn"

function SpawnMushroomEvent(l)
	local x, y = GetFreeXY(l)

	if (x) then
		PlaceSpecial(MUSHROOM_CLASS, x, y, l)
	end

	return 1
end
