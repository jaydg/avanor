-- The ring Beelzevile wears, and the only one of its kind.
--
-- It is a plain item rather than one of the ordinary sorts of ring: those
-- are their enchantment, drawn at random and discovered by wearing, and
-- this is neither. It says what it does and what it is called outright.
Item.new("great_elemental_ring")
	:Plain("ring", ItemKind.RING)
	:Wear(BodyPart.RING)
	:View("Great Elemental Ring", '=', xColor.xLIGHTCYAN)
	:Basic(500, 3)
	:Resist("fire:1d1+79 cold:1d1+79 acid:1d1+79 see_invisible:0d0+20")
	:Stats("Wi:1d1+4")
	:Unique()
	:Register()

Monster.new("beelzevile")
	:View("Beelzevile, the horned demon", '&', xColor.xLIGHTCYAN, PersonType.NAMED_HE, CreatureTemplate.UNIQUE, "demon")
	:Basic("1d30+50", "0d0+1500", "0d0+900", CreatureSize.LARGE, "1d400+4000")
	:Body("head body ring ring", 100)
	:Never("invisible")
	:Always("see_invisible")
	:AI(XStandardAI.RANDOM_MOVE + XStandardAI.ALLOW_PICK_UP + XStandardAI.ALLOW_WEAR_ITEM + XStandardAI.COWARD)
	:Stats("St 5d5+50 Dx 3d3+10 To 2d5+30 Le 9d5 Wi 3d4 Ma 5d4 Pe 1d3 Ch 1d3")
	:Resist("see_invisible:0d0+70")
	:Combat("0d0+10", "2d8")
	:Main("0d0-5", "0d0+2", "1d6+22", "5d5+100")
	:Description("How he got to the mushroom caves is anyones guess but it is commonly believed that some kobold shaman got a little carried away with his magic and summoned Beelzevile from the netherworld.  Standing 10 feet high and carrying a reek of sulphur about him, he strides around the cavern, furious at being trapped in this cold place.  His massive claws can shred flesh and steel alike.  You catch a glimpse of a magnificent ring on one of his hands as he noisily chews on the haunch of a kobold who wandered a little too close.")
	:Melee("poison", 80)
	:Melee("fire", 100)
	:Melee("cold", 100)
	:Unique()
	:Register()


function CreateBeelzevile()
	local demon = Creature("beelzevile")
	SetEventHandler(demon, 'BeelzevileHandler')
	GiveObjectToCreature(CreateObject('great_elemental_ring'), demon)
	-- no-op outside "-demo" mode - see XLocation::SetMainCreature
	SetMainCreature(demon)
end

function BeelzevileHandler(e, t, p, v)
	if (e == LuaEvent.DIE) then
		QuestModify("elder", XQuest.COMPLETE)
	end
	return false
end
