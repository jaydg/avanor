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
	return 1
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
		AddMessage("'Don't bother me!'")
	elseif (e == LuaEvent.GIVE_ITEM) then
		local kind, brt, wt, it, count, name = GetItemParam(v)
		if (BinaryAND(kind, ItemKind.WEAPON) and BinaryAND(brt, AttackEffectType.ORCSLAYER) and wt == XWarSkills.SWORD) then
			AddMessage("'Thank you!'")
			if (QuestStatus(QUEST_OZORIK) < XQuest.COMPLETE) then
				QuestModify(QUEST_OZORIK, XQuest.COMPLETE)
			end
		else
			AddMessage("'I do not need this!'")
			return 0;
		end
	end
	return 1
end


local BANDIT_GROUP = "forest_brother"

-- Recognizes fellow forest-brotherhood members by their cloak and never
-- treats them as enemies, regardless of the usual class-based hostility
-- rules.
function CreateBandit(x, y)
	local bandit = Guardian('bandit', BANDIT_GROUP, x, y, 12, 8, XStandardAI.GUARD_AREA + XStandardAI.PROTECT_AREA + XStandardAI.RANDOM_MOVE)
	AsCreature(bandit):PutOnBody(BodyPart.CLOAK, 0, CreateObject('XForestBrotherCloak'))
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
	if (not FindCreature("MAIN", BANDIT_GROUP)) then
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
				DropItem(CreateObject('XBone'), o)
			end
		else
			AddMessage('This grave has already been looted.')
		end
	end
	return 1
end

function AvanorDefenderGraveEvent(e, cr, o)
	if (e == LuaEvent.OUTER_USE and GetView(o) == "+") then
		DropItem(CreateObject('XAvanorDefender'), o)
	end
	return StandardGraveEvent(e, cr, o)
end
