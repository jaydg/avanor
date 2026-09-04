-- Herbs and mushrooms.
--
-- What grows, what the picked part is called, and how it tastes. What each
-- species distils into is NOT here: the alchemy is dealt out afresh every
-- game, so learning that valeriana made healing last time tells you nothing
-- this time. That is why a herb reads as "unknown herb" until somebody
-- works it out.
--
--   Herb.new(id)
--       :Called(name)      what the picked part is called
--       :Growing(name)     what the plant it grows on is called. Unsaid, it
--                          is its own plant - see :Mushroom() below
--       :Mushroom()        a mushroom rather than a herb. Mushrooms yield
--                          the harder potions and stand under their own name
--       :Taste(text)       what eating it is like
--       :Looks(xColor.X)
--       :Register()


Herb.new("valeriana_root")
	:Called("valeriana root")
	:Growing("valeriana")
	:Taste("sedative")
	:Looks(xColor.xGREEN)
	:Register()

Herb.new("stellaria_leave")
	:Called("stellaria leave")
	:Growing("stellaria")
	:Taste("strange")
	:Looks(xColor.xGREEN)
	:Register()

Herb.new("trifolium_leave")
	:Called("trifolium leave")
	:Growing("trifolium")
	:Taste("grassy")
	:Looks(xColor.xGREEN)
	:Register()

Herb.new("trifolium_flower")
	:Called("trifolium flower")
	:Growing("trifolium")
	:Taste("sweet")
	:Looks(xColor.xGREEN)
	:Register()

Herb.new("urtica_leave")
	:Called("urtica leave")
	:Growing("urtica")
	:Taste("bitter")
	:Looks(xColor.xGREEN)
	:Register()

Herb.new("convallaria_flower")
	:Called("convallaria flower")
	:Growing("convallaria")
	:Taste("sweet")
	:Looks(xColor.xGREEN)
	:Register()

Herb.new("tussilago_leave")
	:Called("tussilago farfara leave")
	:Growing("tussilago farfara")
	:Taste("bitter")
	:Looks(xColor.xGREEN)
	:Register()

Herb.new("melissa_leave")
	:Called("melissa leave")
	:Growing("melissa")
	:Taste("delicate")
	:Looks(xColor.xGREEN)
	:Register()

Herb.new("mentha_leave")
	:Called("mentha leave")
	:Growing("mentha")
	:Taste("mint")
	:Looks(xColor.xGREEN)
	:Register()

Herb.new("taraxacum_flower")
	:Called("taraxacum flower")
	:Growing("taraxacum")
	:Taste("sweet")
	:Looks(xColor.xGREEN)
	:Register()

Herb.new("paeonia_root")
	:Called("paeonia root")
	:Growing("paeonia")
	:Taste("bitter")
	:Looks(xColor.xGREEN)
	:Register()

Herb.new("plantago_leave")
	:Called("plantago leave")
	:Growing("plantago")
	:Taste("bitter")
	:Looks(xColor.xGREEN)
	:Register()

Herb.new("chamomilla_flower")
	:Called("chamomilla flower")
	:Growing("chamomilla")
	:Taste("sweet")
	:Looks(xColor.xGREEN)
	:Register()

-- The mushrooms, which yield the harder potions.
Herb.new("red_mushroom")
	:Called("red mushroom")
	:Mushroom()
	:Taste("bitter")
	:Looks(xColor.xRED)
	:Register()

Herb.new("green_mushroom")
	:Called("green mushroom")
	:Mushroom()
	:Taste("bitter")
	:Looks(xColor.xGREEN)
	:Register()

Herb.new("blue_mushroom")
	:Called("blue mushroom")
	:Mushroom()
	:Taste("bitter")
	:Looks(xColor.xBLUE)
	:Register()

Herb.new("yellow_mushroom")
	:Called("yellow mushroom")
	:Mushroom()
	:Taste("bitter")
	:Looks(xColor.xYELLOW)
	:Register()

Herb.new("white_mushroom")
	:Called("white mushroom")
	:Mushroom()
	:Taste("bitter")
	:Looks(xColor.xWHITE)
	:Register()
