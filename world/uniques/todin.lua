
Monster.new("todin")
	:View("Todin, dwarven weaponsmith", 'h', xColor.xBROWN, PersonType.NAMED_HE, CreatureTemplate.UNIQUE, CreatureClass.HUMANOID)
	:Basic("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200")
	:Body("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, CreatureTemplate.SUPPRESS_INVIS + CreatureTemplate.SEE_INVIS)
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM + XStandardAI.COWARD + XStandardAI.PEACEFUL)
	:Stats("St 1d8+30 Dx 1d8+30 To 1d8+15 Le 1d5+15 Wi 1d4+5 Ma 1d4+5 Pe 3d6 Ch 5d5")
	:Resist("see_invisible:0d0+20")
	:Combat("1d8", "2d2")
	:Main("4d2", "1d3", "1d5+15", "1d5+5")
	:Description("Squat, sturdy and built like a boulder here stands the King's twin brother and master smith.  Todin stands at his forge and works the bellows with one hand while nonchalantly shaping a sword with the hammer in his other hand.  The ruddy glow of the forge glimmers on his sweat drenched skin.  Truly he is a master smith as the weapons hanging about the room display his craft.")
	:LearnSkill(SKT_HEALING, 6)
	:LearnSkill(SKT_FINDWEAKNESS, 6)
	:Unique()
	:Register()


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
