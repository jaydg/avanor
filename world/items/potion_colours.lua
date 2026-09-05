-- What a potion looks like before anyone knows what it is.
--
-- Each sort of potion takes an appearance nothing else has, dealt out
-- afresh every game, so knowing that the smoky one was healing last time
-- tells you nothing this time. A potion may insist on a particular one
-- (water is always clear); the rest are handed out at random.
--
-- There must be at least as many of these as there are sorts of potion,
-- or some potion will have nothing to be told apart by.
--
--   PotionColour.new(id)
--       :Called(name)    what it reads as, when that differs from the id
--       :Looks(xColor.X) the colour it is drawn in
--       :Register()

PotionColour.new("clear")
	:Looks(xColor.xLIGHTGRAY)
	:Register()

PotionColour.new("smoky")
	:Looks(xColor.xLIGHTGRAY)
	:Register()

PotionColour.new("green")
	:Looks(xColor.xGREEN)
	:Register()

PotionColour.new("orange")
	:Looks(xColor.xYELLOW)
	:Register()

PotionColour.new("yellow")
	:Looks(xColor.xYELLOW)
	:Register()

PotionColour.new("black")
	:Looks(xColor.xDARKGRAY)
	:Register()

PotionColour.new("blue")
	:Looks(xColor.xBLUE)
	:Register()

PotionColour.new("white")
	:Looks(xColor.xWHITE)
	:Register()

PotionColour.new("cyan")
	:Looks(xColor.xCYAN)
	:Register()

PotionColour.new("purple")
	:Looks(xColor.xLIGHTMAGENTA)
	:Register()

PotionColour.new("haze")
	:Looks(xColor.xLIGHTGRAY)
	:Register()

PotionColour.new("golden")
	:Looks(xColor.xYELLOW)
	:Register()

PotionColour.new("silver")
	:Looks(xColor.xLIGHTGRAY)
	:Register()

PotionColour.new("azure")
	:Looks(xColor.xLIGHTCYAN)
	:Register()

PotionColour.new("murky")
	:Looks(xColor.xDARKGRAY)
	:Register()

PotionColour.new("red")
	:Looks(xColor.xRED)
	:Register()

PotionColour.new("glowing")
	:Looks(xColor.xYELLOW)
	:Register()

PotionColour.new("mottled")
	:Looks(xColor.xLIGHTRED)
	:Register()

PotionColour.new("blobby")
	:Looks(xColor.xBROWN)
	:Register()

PotionColour.new("pink")
	:Looks(xColor.xLIGHTMAGENTA)
	:Register()

PotionColour.new("mouldy")
	:Looks(xColor.xLIGHTCYAN)
	:Register()

PotionColour.new("gray")
	:Looks(xColor.xLIGHTGRAY)
	:Register()

PotionColour.new("mercury")
	:Looks(xColor.xLIGHTGRAY)
	:Register()

PotionColour.new("oily")
	:Looks(xColor.xDARKGRAY)
	:Register()

PotionColour.new("viscous")
	:Looks(xColor.xLIGHTCYAN)
	:Register()

PotionColour.new("dark_red")
	:Called("dark red")
	:Looks(xColor.xRED)
	:Register()

PotionColour.new("light_red")
	:Called("light red")
	:Looks(xColor.xLIGHTRED)
	:Register()

PotionColour.new("dark_blue")
	:Called("dark blue")
	:Looks(xColor.xBLUE)
	:Register()

PotionColour.new("light_blue")
	:Called("light blue")
	:Looks(xColor.xLIGHTBLUE)
	:Register()

PotionColour.new("brown")
	:Looks(xColor.xBROWN)
	:Register()

PotionColour.new("light_gray")
	:Called("light gray")
	:Looks(xColor.xLIGHTGRAY)
	:Register()

PotionColour.new("dark_gray")
	:Called("dark gray")
	:Looks(xColor.xDARKGRAY)
	:Register()

PotionColour.new("dark_green")
	:Called("dark green")
	:Looks(xColor.xGREEN)
	:Register()

PotionColour.new("light_green")
	:Called("light green")
	:Looks(xColor.xLIGHTGREEN)
	:Register()

PotionColour.new("beige")
	:Looks(xColor.xLIGHTGRAY)
	:Register()

PotionColour.new("aquamarine")
	:Looks(xColor.xCYAN)
	:Register()

PotionColour.new("coral")
	:Looks(xColor.xGREEN)
	:Register()

PotionColour.new("ivory")
	:Looks(xColor.xYELLOW)
	:Register()

PotionColour.new("maroon")
	:Looks(xColor.xRED)
	:Register()

PotionColour.new("tan")
	:Looks(xColor.xBROWN)
	:Register()

PotionColour.new("turquoise")
	:Looks(xColor.xCYAN)
	:Register()

PotionColour.new("violet")
	:Looks(xColor.xMAGENTA)
	:Register()
