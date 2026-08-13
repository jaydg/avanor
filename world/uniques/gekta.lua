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
