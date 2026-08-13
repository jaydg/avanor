
	Monster.new("beelzevile")
		:View("Beelzevile, the horned demon", '&', xColor.xLIGHTCYAN, PersonType.NAMED_HE, CreatureTemplate.UNIQUE, CreatureClass.DEMON)
		:Basic("1d30+50", "0d0+1500", "0d0+900", CS_LARGE, "1d400+4000")
		:Body("head body ring ring", 100, CreatureTemplate.SUPPRESS_INVIS + CreatureTemplate.SEE_INVIS)
		:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM + XStandardAI.COWARD)
		:Stats("St 5d5+50 Dx 3d3+10 To 2d15+20 Le 9d5 Wi 3d4 Ma 5d4 Pe 1d3 Ch 1d3")
		:Resist("see_invisible:0d0+70")
		:Combat("0d0+10", "3d15")
		:Main("0d0-10", "0d0+15", "5d5+100", "5d5+100")
		:Description("How he got to the mushroom caves is anyones guess but it is commonly believed that some kobold shaman got a little carried away with his magic and summoned Beelzevile from the netherworld.  Standing 10 feet high and carrying a reek of sulphur about him, he strides around the cavern, furious at being trapped in this cold place.  His massive claws can shred flesh and steel alike.  You catch a glimpse of a magnificent ring on one of his hands as he noisily chews on the haunch of a kobold who wandered a little too close.")
		:Melee(AttackEffectType.POISON, 80)
		:Melee(AttackEffectType.FIRE, 100)
		:Melee(AttackEffectType.COLD, 100)
		:Unique()
		:Register()


function CreateBeelzevile()
	local demon = Creature("beelzevile")
	SetEventHandler(demon, 'BeelzevileHandler')
	GiveObjectToCreature(CreateObject('XGreatElementalRing'), demon)
	-- no-op outside "-demo" mode - see XLocation::SetMainCreature
	SetMainCreature(demon)
end

function BeelzevileHandler(e, t, p, v)
	if (e == LuaEvent.DIE) then
		QuestModify(QUEST_ELDER, XQuest.COMPLETE)
	end
	return 0
end
