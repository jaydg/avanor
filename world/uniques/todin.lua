function CreateTodin(x, y)
	local todin = Guardian("todin", "dwarven_guardian", x, y, 6, 4)
	SetEventHandler(todin, 'TodinHandler')
end


function TodinHandler(e, t, p, v)
	if (e == LuaEvent.CHAT) then
		AddMessage("'Give me your weapon, and I'll make it the best!'")
	elseif (e == LuaEvent.GIVE_ITEM) then
		local kind, brt, wt, it, count, name = GetItemParam(v)
		if (BinaryAND(kind, ItemKind.WEAPON)) then
			if (BinaryAND(brt, AttackEffectType.COLD + AttackEffectType.FIRE + AttackEffectType.ORCSLAYER)) then
				AddMessage("'This weapon's good enough!'")
			else
				if (AskQuestion("'I need 450 gp to improve this weapon. Do you agree?'", "esc y n", "yes", "no") == 'y') then
					if (MoneyOperation(p, -450) >= 0) then
						MoneyOperation(t, 450)
						local res = Rand(3)
						if ( res == 0) then
							SetItemBrand(v, AttackEffectType.COLD)
						elseif (res == 1) then
							SetItemBrand(v, AttackEffectType.FIRE)
						else
							SetItemBrand(v, AttackEffectType.ORCSLAYER)
						end
						AddMessage("'Thank you!'")
					else
						AddMessage("'But you haven't enough money!'")
					end
				else
					AddMessage("'Don't waste my time!'")
				end
			end
		else
			AddMessage("'Sorry, I don't need this.'")
		end
	end
	return 0
end
