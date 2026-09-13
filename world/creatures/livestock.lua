---------------------------- LIVESTOCK ------------------------------------
--
-- Beasts nobody in the valley considers game: they belong to someone, and
-- that someone tends to notice.

Monster.new("sheep")
	:View("sheep", 'q', xColor.xWHITE, PersonType.IT, CreatureTemplate.VERY_LOW, "other")
	:Basic("1d10+115", "0d0+1000", "0d0+1000", CreatureSize.SMALL, "1d200+900")
	:Body("", 0)
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PACK + XStandardAI.COWARD + XStandardAI.PEACEFUL)
	:Stats("St 1d3+2 Dx 1d3+1 To 1d4+2 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d4+2 Ch 1d4")
	:Resist("cold:2d10+20")
	:Combat("0d0", "1d2")
	:Main("1d2", "0d0", "2d4+4", "0d0")
	:Description("A ewe in a thick winter fleece, cropping the grass with the single-mindedness of a creature that has never once been asked to think. She watches you approach, decides you are neither grass nor wolf, and goes back to grazing. Whoever keeps her keeps her well.")
	:Register()

Monster.new("goat")
	:View("goat", 'q', xColor.xDARKGRAY, PersonType.IT, CreatureTemplate.VERY_LOW, "other")
	:Basic("1d10+110", "0d0+1000", "0d0+1000", CreatureSize.SMALL, "1d200+800")
	:Body("", 0)
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PACK + XStandardAI.PEACEFUL)
	:Stats("St 1d3+3 Dx 1d3+2 To 1d4+3 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d4+3 Ch 1d3")
	:Resist("cold:2d10+10")
	:Combat("1d2", "1d3")
	:Main("1d2+1", "0d0", "2d4+4", "0d0")
	:Description("Lean where the sheep is round, and a good deal less agreeable about it. She has already eaten everything in the pen worth eating and is presently working on the fence. When you come close she stops, turns those slotted yellow eyes on you, and holds them there a moment too long before going back to the rail.")
	:Register()
