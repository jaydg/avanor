-- Herbs and mushrooms.
--
-- What grows, what the picked part is called, and how it tastes. What each
-- species distils into is NOT here: the alchemy is dealt out afresh every
-- game, so learning that valeriana made healing last time tells you nothing
-- this time. That is why a herb reads as "unknown herb" until somebody
-- works it out.
--
-- What sorts of plant there are. A world may have as many as it likes:
-- each says what an unrecognised one is called and which grades of potion
-- it distils into, and the first declared is what a species is if it does
-- not say otherwise.
--
--   PlantKind.new(id)
--       :Unknown(name)          what one reads as before anybody knows it
--       :Distils(power, dice)   it yields potions of that alchemy grade,
--                               recognised at that difficulty
--       :Register()

PlantKind.new("herb")
	:Unknown("unknown herb")
	:Distils(1, "1d4")
	:Distils(2, "1d4+3")
	:Register()

PlantKind.new("mushroom")
	:Unknown("unknown mushroom")
	:Distils(3, "1d4+6")
	:Register()


--   Plant.new(id)
--       :Called(name)      what the picked part is called
--       :Growing(name)     what the plant it grows on is called. Unsaid, it
--                          is its own plant - see :Kind() below
--       :Kind(id)          which sort of plant it is, from the kinds above.
--                          Unsaid, the first kind declared - a plain herb
--       :Taste(text)       what eating it is like
--       :Looks(xColor.X)
--       :Register()


Plant.new("valeriana_root")
	:Called("valeriana root")
	:Growing("valeriana")
	:Taste("sedative")
	:Looks(xColor.xGREEN)
	:Register()

Plant.new("stellaria_leave")
	:Called("stellaria leave")
	:Growing("stellaria")
	:Taste("strange")
	:Looks(xColor.xGREEN)
	:Register()

Plant.new("trifolium_leave")
	:Called("trifolium leave")
	:Growing("trifolium")
	:Taste("grassy")
	:Looks(xColor.xGREEN)
	:Register()

Plant.new("trifolium_flower")
	:Called("trifolium flower")
	:Growing("trifolium")
	:Taste("sweet")
	:Looks(xColor.xGREEN)
	:Register()

Plant.new("urtica_leave")
	:Called("urtica leave")
	:Growing("urtica")
	:Taste("bitter")
	:Looks(xColor.xGREEN)
	:Register()

Plant.new("convallaria_flower")
	:Called("convallaria flower")
	:Growing("convallaria")
	:Taste("sweet")
	:Looks(xColor.xGREEN)
	:Register()

Plant.new("tussilago_leave")
	:Called("tussilago farfara leave")
	:Growing("tussilago farfara")
	:Taste("bitter")
	:Looks(xColor.xGREEN)
	:Register()

Plant.new("melissa_leave")
	:Called("melissa leave")
	:Growing("melissa")
	:Taste("delicate")
	:Looks(xColor.xGREEN)
	:Register()

Plant.new("mentha_leave")
	:Called("mentha leave")
	:Growing("mentha")
	:Taste("mint")
	:Looks(xColor.xGREEN)
	:Register()

Plant.new("taraxacum_flower")
	:Called("taraxacum flower")
	:Growing("taraxacum")
	:Taste("sweet")
	:Looks(xColor.xGREEN)
	:Register()

Plant.new("paeonia_root")
	:Called("paeonia root")
	:Growing("paeonia")
	:Taste("bitter")
	:Looks(xColor.xGREEN)
	:Register()

Plant.new("plantago_leave")
	:Called("plantago leave")
	:Growing("plantago")
	:Taste("bitter")
	:Looks(xColor.xGREEN)
	:Register()

Plant.new("chamomilla_flower")
	:Called("chamomilla flower")
	:Growing("chamomilla")
	:Taste("sweet")
	:Looks(xColor.xGREEN)
	:Register()

-- The mushrooms, which yield the harder potions.
Plant.new("red_mushroom")
	:Called("red mushroom")
	:Kind("mushroom")
	:Taste("bitter")
	:Looks(xColor.xRED)
	:Register()

Plant.new("green_mushroom")
	:Called("green mushroom")
	:Kind("mushroom")
	:Taste("bitter")
	:Looks(xColor.xGREEN)
	:Register()

Plant.new("blue_mushroom")
	:Called("blue mushroom")
	:Kind("mushroom")
	:Taste("bitter")
	:Looks(xColor.xBLUE)
	:Register()

Plant.new("yellow_mushroom")
	:Called("yellow mushroom")
	:Kind("mushroom")
	:Taste("bitter")
	:Looks(xColor.xYELLOW)
	:Register()

Plant.new("white_mushroom")
	:Called("white mushroom")
	:Kind("mushroom")
	:Taste("bitter")
	:Looks(xColor.xWHITE)
	:Register()
