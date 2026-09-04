-- Bows, crossbows and slings.
--
-- Template.new(kind, type) is documented in world/items/init.lua.
--

-- A launcher handed to a creature comes with something to shoot. Which
-- missile that is is not restated here: MissileForLauncher() reads it back
-- from the missiles' own :Launcher() declarations in missiles.lua, so a new
-- launcher needs nothing but its :Skill(), and a new kind of ammunition
-- needs nothing but its :Launcher().
function ArmLauncher(weapon, who)
	local kind = MissileForLauncher(weapon)

	if (not kind) then
		return
	end

	local ammo = CreateObject(ItemKind.MISSILE, kind, 0, 10000000)

	if (ammo) then
		AsCreature(who):ContainItem(AsItem(ammo))
	end
end

Template.new(ItemKind.MISSILEW, "long_bow")
	:View("long bow", '{')
	:Made(ItemSet.BOW, ItemQuality.FAIR)
	:Skill(XWarSkills.BOW)
	:Worth(20, 10)
	:Combat("1d3+2", "1d6", "1d3+2")
	:Range("1d1+2")
	:Chance(30)
	:OnOutfit("ArmLauncher")
	:Register()

Template.new(ItemKind.MISSILEW, "short_bow")
	:View("short bow", '{')
	:Made(ItemSet.BOW, ItemQuality.AVG)
	:Skill(XWarSkills.BOW)
	:Worth(10, 5)
	:Combat("1d2", "1d4", "1d2")
	:Range("1d2")
	:Chance(150)
	:OnOutfit("ArmLauncher")
	:Register()

Template.new(ItemKind.MISSILEW, "light_crossbow")
	:View("light crossbow", '{')
	:Made(ItemSet.BOW, ItemQuality.AVG)
	:Skill(XWarSkills.CROSSBOW)
	:Worth(10, 5)
	:Combat("1d2", "1d5", "1d3")
	:Range("1d2")
	:Chance(120)
	:OnOutfit("ArmLauncher")
	:Register()

Template.new(ItemKind.MISSILEW, "crossbow")
	:View("crossbow", '{')
	:Made(ItemSet.BOW, ItemQuality.AVG)
	:Skill(XWarSkills.CROSSBOW)
	:Worth(15, 5)
	:Combat("1d4", "1d6", "1d3+1")
	:Range("1d2")
	:Chance(70)
	:OnOutfit("ArmLauncher")
	:Register()

Template.new(ItemKind.MISSILEW, "heavy_crossbow")
	:View("heavy crossbow", '{')
	:Made(ItemSet.BOW, ItemQuality.GOOD)
	:Skill(XWarSkills.CROSSBOW)
	:Worth(35, 30)
	:Combat("1d3", "1d8", "1d4+2")
	:Range("1d2+1")
	:Chance(10)
	:OnOutfit("ArmLauncher")
	:Register()

Template.new(ItemKind.MISSILEW, "sling")
	:View("sling", '{')
	:Made(ItemSet.ALLLEATHER, ItemQuality.POOR)
	:Skill(XWarSkills.SLING)
	:Worth(5, 2)
	:Combat("1d2", "1d3+1", "1d3")
	:Range("1d1+1")
	:Chance(200)
	:OnOutfit("ArmLauncher")
	:Register()
