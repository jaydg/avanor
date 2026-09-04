-- Food: everything that can be eaten and belongs to nobody in particular.
-- Yohjishiro's two trophies are defined with her instead, beside the errand
-- that wants them.


-- What a skeleton leaves behind, and what is under a headstone. Dropped by
-- the world tally (world/tally.lua) and by the graves in the valley.

Food.new("bone")
	:View("bone", '%', xColor.xWHITE)
	:Basic("bone", 1, 5)
	:Nutrition(10, 10)
	:Register()


-- Ordinary fare, and the only foods the game hands out on its own: when
-- something asks for food without saying which, one of these four is drawn.
-- They were a four-row table in C++ (rations_db) behind a class whose only
-- method forwarded straight to its base - data wearing a class.
--
-- The per-turn figures are what that table computed as (nutrition * 20) /
-- weight, worked out once and written down: heavier food is chewed through
-- more slowly.

Food.new("large_ration")
	:View("large ration", '%', xColor.xLIGHTGRAY)
	:Basic("large_ration", 5, 100)
	:Nutrition(400, 80)
	:Random(100)
	:Register()

Food.new("ration")
	:View("ration", '%', xColor.xBROWN)
	:Basic("ration", 3, 70)
	:Nutrition(250, 71)
	:Random(100)
	:Register()

Food.new("small_ration")
	:View("small ration", '%', xColor.xBROWN)
	:Basic("small_ration", 1, 40)
	:Nutrition(150, 75)
	:Random(100)
	:Register()

Food.new("elvish_waybread")
	:View("elvish waybread", '%', xColor.xYELLOW)
	:Basic("elvish_waybread", 15, 15)
	:Nutrition(700, 933)
	:Taste(FoodType.GOOD)
	:Random(100)
	:Register()
