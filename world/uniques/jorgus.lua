
Monster.new("jorgus")
	:View("Jorgus, the master thief", 'p', xColor.xWHITE, PersonType.NAMED_HE, CreatureTemplate.UNIQUE, CreatureClass.HUMAN)
	:Basic("1d10+95", "0d0+1000", "0d0+1000", CreatureSize.NORMAL, "1d200+1200")
	:Body("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 50, CreatureTemplate.SUPPRESS_INVIS + CreatureTemplate.SEE_INVIS)
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM + XStandardAI.COWARD + XStandardAI.PEACEFUL)
	:Stats("St 1d8+30 Dx 1d8+30 To 1d8+15 Le 1d5+15 Wi 1d4+5 Ma 1d4+5 Pe 3d6 Ch 5d5")
	:Resist("")
	:Combat("1d8", "2d2")
	:Main("4d2", "1d3", "1d5+15", "1d5+5")
	:Description("As you enter his dwelling you keep a tight hold on your purse.  The shifty eyes and too quick smile of the owner makes you nervous.  Jorgush is the leader of his outlaw band.  They are known for stealing from the rich and keeping it for themselves.  He appears to evaluate you and the weapons you bear and then gestures to the cahir at the table.  Perhaps he has a proposition for you...")
	:LearnSkill(SKT_STEALING, 15)
	:EquipCount(ItemKind.SCROLL + ItemKind.POTION, 3, 30)
	:Equip(ItemKind.BODY, ItemType.CLOTHES, 100)
	:Unique()
	:Register()


function CreateJorgus(x, y)
	local jorgus = Guardian("jorgus", "forest_brother", x, y, 3, 2)
	SetEventHandler(jorgus, 'JorgusHandler')
	GiveObjectToCreature(CreateObject('XForestBrotherCloak'), jorgus)
end

function JorgusHandler(e, t, p, v)
	if (e == LuaEvent.CHAT) then
		if (GetSkill(p, SKT_STEALING) > 0) then
			if (Gender(p) == Gender.MALE) then
				AddMessage("Good day, brother!")
			else
				AddMessage("Good day, sister!")
			end
		else
			if (AskQuestion("'I can teach you the great art of theft for 1000gp. Would you like to learn?'", "y n", "yes", "no") == 'y') then
				if (MoneyOperation(p, -1000) >= 0) then
					MoneyOperation(t, 1000)
					LearnSkill(p, SKT_STEALING, 1)
					if (Gender(p) == Gender.MALE) then
						AddMessage("You're welcome, brother!")
					else
						AddMessage("You're welcome, sister!")
					end
				else
					AddMessage("You don't have enough money!")
				end
			else
				AddMessage("Don't waste my time!")
			end
		end
	end
	return 1
end
