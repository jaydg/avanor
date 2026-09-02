-- The sword under the Avanor Defender's headstone, and the cloak that
-- marks a forest brother. Both belong to what happens in this file - the
-- grave that gives up the one, and the brotherhood that wears the other.

Item.new("avanor_defender")
	:Weapon(ItemType.LONGSWORD)
	:View("rune-covered long sword")
	:Basic(3000, 110)
	:Armour(0, 0)
	:Combat(18, 4, 4, 15)
	:Resist("poison:0d0+100 stun:0d0+100 confuse:0d0+100 fire:0d0+100 cold:0d0+100")
	:Stats("St:0d0+10 To:0d0+10")
	:Brand(AttackEffectType.COLD + AttackEffectType.FIRE + AttackEffectType.DEMONSLAYER)
	:Called('long sword named "Avanor\'s Defender"')
	:Unique()
	:Register()

-- No :Called - it is meant to pass for an ordinary cloak, which is the
-- whole point of wearing one past the bridge.
Item.new("forest_brother_cloak")
	:Cloak(ItemType.CLOAK)
	:View("forest brother's cloak", nil, xColor.xGREEN)
	:Type(ItemType.FORESTBROTHERCLOAK)
	:Basic(100, 20)
	:Armour(3, 1)
	:Combat(0, 1, 2, 0)
	:Resist("poison:0d0+20 stun:0d0+20")
	:Stats("")
	:Unique()
	:Register()


function SendFarmersToCollectMushrooms()
	local cave_x, cave_y = GetWayXY("MUSHROOMS_CAVE5")
	local script = {
		{cmd = ScriptCommand.MOVE_POINT, pt_x = cave_x, pt_y = cave_y, ln = "MUSHROOMS_CAVE5"},
		{cmd = ScriptCommand.COLLECT_MUSHROOM},
		{cmd = ScriptCommand.MOVE_POINT, pt_x = 13, pt_y = 8, ln = "MAIN"},
		{cmd = ScriptCommand.DROP_ITEM, kind = ItemKind.FOOD},
	}

	-- Everyone who lives in the village shares VILLAGE_GROUP - it is what
	-- gives them their patrol area and what keeps the mushroom caves from
	-- draining them (see MushroomCaveEvent). It is not a list of gatherers.
	-- The goodwives keep the cottages and do not go below, and the Elder and
	-- Brida have names, dialogue and quests of their own; sending those two
	-- down a dungeon can end with the Elder's own errand or Giana's rescue
	-- unfinishable. Only the farmers make the trip.
	for _, villager in ipairs(FindCreatures("MAIN", VILLAGE_GROUP)) do
		if (AsCreature(villager).name == "farmer") then
			ExecuteCreatureScript(villager, script)
		end
	end
end

function FarmerHandler(e, t, p, v)
	if (e == LuaEvent.CHAT) then
		local qs = QuestStatus(QUEST_ELDER)

		if (qs == XQuest.COMPLETE or qs == XQuest.CLOSED) then
			AddMessage("'Thank you, great hero!'")
		elseif (Rand(2) == 0) then
			-- The other half of the time, the thing a newcomer most needs
			-- to hear. Said here as well as shouted at the bridge, because
			-- a player who never crosses the river still ought to learn
			-- that east is not the way out of the valley.
			AddMessage("'You'll not be going east, will you? The forest brothers hold the bridge.'")
			AddMessage("'There's five of them and they don't ask twice. Better men than us have gone that way and not come back.'")
		else
			AddMessage("'Please speak with our elder. He lives in the stone house.'")
		end
	end
	return true
end

function CreateGuardians(x, y)
	for i = 1, 7 do
		local g = Guardian("royal_guard", "guardian", x, y, 14, 5, XStandardAI.GUARD_AREA + XStandardAI.RANDOM_MOVE)
		SetEnemy(g, CreatureClass.ORC)
		SetEventHandler(g, 'RoyalGuardHandler')
	end
end


function RoyalGuardHandler(e, t, p, v)
	if (e == LuaEvent.CHAT) then
		-- The captain died with his errand unfinished (OzorikHandler's DIE case).
		-- His guards are where the hero learns of it - the quest itself is just
		-- stops appearing in the log.
		if (QuestStatus(QUEST_OZORIK) == XQuest.FAIL) then
			AddMessage("'The captain is dead. We hold this ground because it is ours to hold, not because anyone is left to order it.'")
		else
			AddMessage("'Don't bother me!'")
		end
	elseif (e == LuaEvent.GIVE_ITEM) then
		local kind, brt, wt, it, count, name = GetItemParam(v)
		if (BinaryAND(kind, ItemKind.WEAPON) and BinaryAND(brt, AttackEffectType.ORCSLAYER) and wt == XWarSkills.SWORD) then
			-- A dead captain cannot judge the blade or pay for it. The guard still
			-- takes it - it is exactly what he wants against the orcs.
			if (QuestStatus(QUEST_OZORIK) == XQuest.FAIL) then
				AddMessage("'A fine blade. It comes a little late.'")
			else
				AddMessage("'Thank you!'")
			end

			if (QuestStatus(QUEST_OZORIK) < XQuest.COMPLETE) then
				QuestModify(QUEST_OZORIK, XQuest.COMPLETE)
			end
		else
			AddMessage("'I do not need this!'")
			return false;
		end
	end
	return true
end


-- The brotherhood that holds the bridge. Global rather than local: the
-- Elder's second quest is to clear them off the road, so his handler has to
-- be able to ask whether any are left (see ElderGridorHandler).
BANDIT_GROUP = "forest_brother"

-- Recognizes fellow forest-brotherhood members by their cloak and never
-- treats them as enemies, regardless of the usual class-based hostility
-- rules.
function CreateBandit(x, y)
	local bandit = Guardian('bandit', BANDIT_GROUP, x, y, 12, 8, XStandardAI.GUARD_AREA + XStandardAI.PROTECT_AREA + XStandardAI.RANDOM_MOVE)
	AsCreature(bandit):PutOnBody(BodyPart.CLOAK, 0, CreateObject('forest_brother_cloak'))
	SetCreatureAI(bandit, 'BanditAI')

end

-- Note: does not re-check personal-enemy status before the cloak check the
-- way the original C++ XBanditAI::isEnemy did (isPersonalEnemy isn't
-- exposed to Lua) - XStandardAI::isEnemy() still checks it as its own
-- final fallback when this returns nil, so the only behavior gap is a
-- forest-brother-cloaked creature that's *also* a declared personal enemy
-- getting treated as non-hostile instead of hostile. Narrow edge case,
-- accepted rather than adding new binding surface for it.
BanditAI = {}
function BanditAI.isEnemy(self, cr)
	if cr:IsWearingItemType(BodyPart.CLOAK, 0, ItemType.FORESTBROTHERCLOAK) then
		return false
	end

	-- Whoever walks with a brother walks under his colours. The cloak is easy to
	-- come by (one lies in the rat cellar), so a hero who has been down there
	-- crosses the bridge untouched - but the camp's guard areas cover the whole
	-- east bank, and anything of another group standing inside one is an enemy
	-- by PROTECT_AREA alone (XStandardAI::isEnemy).
	local escort = cr.xai:GetCompanion()

	if (escort and escort:IsWearingItemType(BodyPart.CLOAK, 0, ItemType.FORESTBROTHERCLOAK)) then
		return false
	end

	return nil
end

-- The forest brothers hold the only bridge across the river, and a hero who
-- walks into them at level 1 dies.
--
-- So they call out as the hero sets foot on the span. The bridge is the one
-- tile at (34, 8) and their guard area only begins at x 35, so this is the
-- last square from which turning back is still free - they have not decided
-- to kill anyone yet.
-- The one crossing on the river, and the road tile just short of it. The
-- brothers' guard area begins at x 35, on the far bank, so a hero standing
-- here has not yet been decided about.
BANDIT_BRIDGE = {x = 33, y = 8, w = 2, h = 1}

-- How many of the five still hold the road. Counted by name, not by group:
-- Jorgus shares their group id because he leads them (see CreateJorgus),
-- but he sits peacefully in his own house behind the camp and is the only
-- person in the valley who will teach stealing. What the Elder wants
-- cleared is the road, not the man.
function BanditsOnTheRoad()
	local n = 0

	for _, b in ipairs(FindCreatures("MAIN", BANDIT_GROUP)) do
		if (AsCreature(b).name == "bandit") then
			n = n + 1
		end
	end

	return n
end

function BanditBridgeEvent(e, p)
	if (e ~= LuaEvent.MOVE_IN or not isHero(p)) then
		return
	end

	-- Only warn somebody about to cross, not somebody coming home. MOVE_IN
	-- fires before the step commits (XCreature::NewMove places the creature
	-- afterwards), so this is still the tile being stepped off: east of the
	-- bridge means the hero is arriving from the brothers' own bank, has
	-- already met them, and needs no telling.
	local from_x = GetCreatureXY(p)

	if (from_x >= BANDIT_BRIDGE.x + BANDIT_BRIDGE.w) then
		return
	end

	-- Nobody left to shout: the road is the player's, and silence says so.
	if (BanditsOnTheRoad() == 0) then
		return
	end

	if (QuestState:GetFlag('bandit_bridge_warned') == 0) then
		QuestState:SetFlag('bandit_bridge_warned', 1)
		AddMessage("A voice comes out of the trees on the far bank. 'Far enough!'")
		AddMessage("'This road is ours now. Set one foot past the water and we will have everything you carry - and your life to finish with.'")
	else
		AddMessage("From the trees on the far bank: 'Still breathing? Come across, then.'")
	end
end

function Grave(x, y, s, e)
	OuterObject(x, y, xColor.xLIGHTGRAY, "+", string.format("the grave signed '%s'", s), e)
end

function StandardGraveEvent(e, cr, o)
	if (e == LuaEvent.OUTER_USE) then
		if (GetView(o) == "+") then
			AddMessage('You move the headstone. Something glimmers in the dark.')
			SetView(o, '-', xColor.xLIGHTGRAY);
			SetName(o, 'the desecrated grave');
			for i = 1, Rand(3) + 2 do
				DropItem(CreateObject('bone'), o)
			end
		else
			AddMessage('This grave has already been looted.')
		end
	end
	return true
end

function AvanorDefenderGraveEvent(e, cr, o)
	if (e == LuaEvent.OUTER_USE and GetView(o) == "+") then
		DropItem(CreateObject('avanor_defender'), o)
	end
	return StandardGraveEvent(e, cr, o)
end
