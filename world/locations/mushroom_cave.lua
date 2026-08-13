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

function MakeMushroomCave()
	CreateLocation(XLocation.MUSHROOMS_CAVE1, "MC:1", "Mushroom Caves Level 1", CAVE)
		Way(UP, XLocation.MAIN)
		Way(DOWN, XLocation.MUSHROOMS_CAVE2)
		Settle(CreatureClass.INSECT + CreatureClass.REPTILE, CreatureTemplate.VERY_LOW)
		Creature("small_snake")
		Creature("small_snake")
		Creature("small_snake")
		Creature("small_snake")
		Creature("spider")
		Creature("spider")
		Creature("spider")

	CreateLocation(XLocation.MUSHROOMS_CAVE2, "MC:2", "Mushroom Caves Level 2", CAVE)
		Way(UP, XLocation.MUSHROOMS_CAVE1)
		Way(DOWN, XLocation.MUSHROOMS_CAVE3)
		Way(DOWN, XLocation.MUSHROOMS_CAVE5)
		CreateBeelzevile()
		Settle(CreatureClass.INSECT + CreatureClass.REPTILE, CreatureTemplate.VERY_LOW)

	CreateLocation(XLocation.MUSHROOMS_CAVE3, "KC:1", "Kobold Cavern Level 1", CAVE)
		Way(UP, XLocation.MUSHROOMS_CAVE2)
		Way(DOWN, XLocation.MUSHROOMS_CAVE4)
		Settle(CreatureClass.KOBOLD + CreatureClass.INSECT + CreatureClass.REPTILE, CreatureTemplate.VERY_LOW)

	CreateLocation(XLocation.MUSHROOMS_CAVE4, "KC:2", "Kobold Cavern Level 2", CAVE)
		Way(UP, XLocation.MUSHROOMS_CAVE3)
		Settle(CreatureClass.KOBOLD, CreatureTemplate.LOW + CreatureTemplate.VERY_LOW)
		Creature("magnush")

	CreateLocation(XLocation.MUSHROOMS_CAVE5, "MC:3", "Mushroom Caves Level 3", CAVE)
		Way(UP, XLocation.MUSHROOMS_CAVE2)
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

function SpawnMushroomEvent(l)
	CreateMushroom(l)
	return 1
end
