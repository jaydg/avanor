function SendFarmersToCollectMushrooms()
	local cave_x, cave_y = GetWayXY(XLocation.MUSHROOMS_CAVE5)
	local script = {
		{cmd = ScriptCommand.MOVE_POINT, pt_x = cave_x, pt_y = cave_y, ln = XLocation.MUSHROOMS_CAVE5},
		{cmd = ScriptCommand.COLLECT_MUSHROOM},
		{cmd = ScriptCommand.MOVE_POINT, pt_x = 13, pt_y = 8, ln = XLocation.MAIN},
		{cmd = ScriptCommand.DROP_ITEM, kind = ItemKind.FOOD},
	}

	for _, farmer in ipairs(FindCreatures(XLocation.MAIN, "small_village_farmer")) do
		ExecuteCreatureScript(farmer, script)
	end
end

function FarmerHandler(e, t, p, v)
	if (e == LuaEvent.CHAT) then
		local qs = QuestStatus(QUEST_ELDER)
		if (qs == XQuest.COMPLETE or qs == XQuest.CLOSED) then
			AddMessage("'Thank you, great hero!'")
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
		if (BinaryAND(kind, ItemKind.WEAPON) and BinaryAND(brt, AttackEffectType.ORCSLAYER) and wt == WSK_SWORD) then
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


-- Recognizes fellow forest-brotherhood members by their cloak and never
-- treats them as enemies, regardless of the usual class-based hostility
-- rules.
function CreateBandit(x, y)
	local bandit = Guardian('bandit', "forest_brother", x, y, 12, 8, XStandardAI.GUARD_AREA + XStandardAI.PROTECT_AREA + XStandardAI.RANDOM_MOVE)
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

	return nil
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
