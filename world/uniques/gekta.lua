
	Monster.new("gekta")
		:View("Gekta, the sheep dog", 'C', xColor.xDARKGRAY, PersonType.NAMED_IT, CreatureTemplate.UNIQUE, CreatureClass.CANINE)
		:Basic("1d10+200", "0d0+1000", "1d100+300", CS_SMALL, "1d200+400")
		:Body("neck", 100, 0)
		:AI(XStandardAI.HI_ANIMAL + XStandardAI.PEACEFUL)
		:Stats("St 1d3+10 Dx 1d5+20 To 1d3+8 Le 1d3+3 Wi 1d3+3 Ma 1d3+3 Pe 5d6 Ch 4d5")
		:Resist("see_invisible:0d0+30")
		:Combat("5d5", "2d6")
		:Main("4d5", "2d1", "1d5+10", "1d1+1")
		:Description("The sheep dog is the mascot for the royal guardians.  It must represent strength, intelligence and unswerving loyalty.  Gekta is the current favorite.  She bounds towards you and knocks you flat, covering you with slobbery dog kisses.")
		:LearnSkill(SKT_HEALING, 15)
		:Unique()
		:Register()


function CreateGekta(x, y)
	local gekta = Guardian("gekta", "guardian", x, y, 14, 5)
	SetEnemy(gekta, CreatureClass.ORC)
	SetEventHandler(gekta, 'GektaHandler')
end


function GektaHandler(e, t, p, v)
	if (e == LuaEvent.CHAT) then
		AddMessage("'Woof! Woof! Woof'");
	elseif (e == LuaEvent.GIVE_ITEM) then
		local kind, brt, wt, it, count, name = GetItemParam(v)
		if (kind == ItemKind.FOOD) then
			if (it == ItemType.BONE) then
				for i = 1, count do
					if (Rand(7) == 0) then
						 AddMessage("Gekta suddenly start to dig in the ground. She digs a pit. Gekta digs something up from the ground. After this, she puts a bone in the pit and buries it.")
						 DropItem(CreateObject(ItemKind.ITEM - ItemKind.FOOD, 20, 500), t)
					else
						AddMessage(string.format("Gekta eats the %s.", name))
					end
				end
			else
				AddMessage(string.format("'Gekta eats the %s.'", name))
				if (Rand(5) == 0) then
					SetCompanion(p, t, true)
					AddMessage("Gekta looks at you faithfully!");
				end
			end
			DestroyObject(v)
		else
			AddMessage('Woof?');
			return 0;
		end
	end
	return 1
end
