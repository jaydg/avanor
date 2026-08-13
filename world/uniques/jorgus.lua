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
