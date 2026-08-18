/*
This file is part of "Avanor, the Land of Mystery" roguelike game
Home page: http://www.avanor.com/
Copyright (C) 2000-2003 Vadim Gaidukevich

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <sol/sol.hpp>

#include "creature/anycr.h"
#include "creature/lua_ai.h"
#include "creature/xhero.h"
#include "engine/xlua.h"
#include "game/game.h"
#include "game/location.h"
#include "game/quest.h"
#include "game/setting.h"
#include "helpers/msgwin.h"
#include "lua/api_actor.h"
#include "magic/effect.h"
#include "map/map_objects.h"

namespace lua_api
{

//AskQuestion("Are you sure?", "yn", "Yes", "No")
struct ASK_QUESTION_REC {
    std::string val;
    int key;
};

int GetSkill(void* cr, int skill)
{
    XSkill * sk = ((XCreature*)cr)->sk->GetSkill((XSkill::Skill)skill);
    return sk ? sk->GetLevel() : 0;
}

void LearnSkill(void* cr, int skill, int val)
{
    XCreature * p = (XCreature*)cr;

    if (!p->sk->GetSkill((XSkill::Skill)skill)) {
        p->sk->Learn((XSkill::Skill)skill, val);
    }
}

int MoneyOperation(void* cr, int val)
{
    return ((XCreature*)cr)->MoneyOp(val);
}

//InflictDamage (target, dmg, RESISTANCE_TYPE, creature ["msg"])
void InflictDamage(void* target, int dmg, int resist, sol::optional<std::string> msg)
{
    XCreature * p = (XCreature*)target;
    dmg = p->onMagicDamage(dmg, (XResistance::Id)resist);
    p->HP -= dmg;

    if (p->HP < 0 && msg) {
        XFakeCreature * tcr = new XFakeCreature((char*)msg->c_str());
        p->Die(tcr);
        tcr->Invalidate();
    }
}

void ChangeStats(void* cr, int st, int val)
{
    ((XCreature*)cr)->GainAttr((XStats::Id)st, val);
}

int GetStats(void* cr, int st)
{
    return ((XCreature*)cr)->GetStats((XStats::Id)st);
}

int Rand(int val)
{
    return vRand(val);
}

bool isHero(void* cr)
{
    return ((XCreature*)cr)->isHero();
}

XCreature* AsCreature(void* p)
{
    return (XCreature*)p;
}

XItem* AsItem(void* p)
{
    return (XItem*)p;
}

void SetCreatureAI(void* cr, const std::string& lua_class)
{
    XCreature* p = (XCreature*)cr;
    auto new_ai = std::make_unique<XLuaAI>(p, lua_class);
    p->xai->CopyBaseStateTo(*new_ai);
    p->xai = std::move(new_ai);
}

int CreatureCountInLocation(int l_id, CreatureClass cc)
{
    return Game.locations[l_id]->GetCreatureCount(cc);
}

bool isEnemy(void* cr1, void* cr2)
{
    XCreature * p1 = (XCreature*)cr1;
    XCreature * p2 = (XCreature*)cr2;
    return p1 && p2 && p1->xai->isEnemy(p2);
}

void* FindCreature(int l_id, const std::string& gid, sol::optional<int> x, sol::optional<int> y, sol::optional<int> w, sol::optional<int> h)
{
    XRect rect(0, 0, Game.locations[l_id]->map->len, Game.locations[l_id]->map->hgt);

    if (x) {
        rect.left = *x;
        rect.top = *y;
        rect.right = rect.left + *w;
        rect.bottom = rect.top + *h;
    }

    for (int i = rect.left; i < rect.right; i++)
        for (int j = rect.top; j < rect.bottom; j++) {
            XCreature* cr = Game.locations[l_id]->map->GetMonster(i, j);

            if (cr && cr->groupID() == gid) {
                return cr;
            }
        }

    return nullptr;
}

std::vector<void*> FindCreatures(int l_id, const std::string& gid, sol::optional<int> x, sol::optional<int> y, sol::optional<int> w, sol::optional<int> h)
{
    XRect rect(0, 0, Game.locations[l_id]->map->len, Game.locations[l_id]->map->hgt);

    if (x) {
        rect.left = *x;
        rect.top = *y;
        rect.right = rect.left + *w;
        rect.bottom = rect.top + *h;
    }

    std::vector<void*> result;

    for (int i = rect.left; i < rect.right; i++)
        for (int j = rect.top; j < rect.bottom; j++) {
            XCreature* cr = Game.locations[l_id]->map->GetMonster(i, j);

            if (cr && cr->groupID() == gid) {
                result.push_back(cr);
            }
        }

    return result;
}

void ExecuteCreatureScript(void* cr, sol::table script)
{
    std::vector<SCRIPT_CMD> cmds;

    for (auto& [key, value] : script) {
        sol::table row = value;
        SCRIPT_CMD cmd{};
        cmd.cmd = row.get_or("cmd", SCC_NONE);
        cmd.pt_x = row.get_or("pt_x", 0);
        cmd.pt_y = row.get_or("pt_y", 0);
        cmd.ln = row.get_or("ln", XLocation::UNKNOWN);
        cmd.kind = row.get_or("kind", ItemKind::UNKNOWN);
        cmds.push_back(cmd);
    }

    ((XCreature*)cr)->xai->ExecuteScript(cmds);
}

std::tuple<int, int> GetWayXY(int l_id)
{
    XStairWay* way = (XStairWay*)*Game.locations[l_id]->ways_list.begin();
    return {way->x, way->y};
}

void SetItEnemyFor(void* cr1, void* cr2)
{
    XCreature * p1 = (XCreature*)cr1;
    XCreature * p2 = (XCreature*)cr2;

    if (p1 && p2) {
        p2->xai->AddPersonalEnemy(p1);
        p2->xai->SetGroupEnemy(p1);
    }
}

void SetEnemy(void* cr, int cr_class)
{
    ((XCreature*)cr)->xai->SetEnemyClass((CreatureClass)cr_class);
}

int Gender(void* cr)
{
    return ((XCreature*)cr)->GetGender();
}

void SetName(void* obj, const std::string& name)
{
    ((XMapObject*)obj)->SetName(name.c_str());
}

void SetView(void* obj, const std::string& view, int color)
{
    ((XMapObject*)obj)->SetView(view[0], color);
}

std::string GetView(void* obj)
{
    return std::string(1, ((XMapObject*)obj)->view);
}

void AddMessage(const std::string& str)
{
    msgwin.Add(str);
}

std::string AskQuestion(const std::string& msg, const std::string& key, sol::variadic_args va)
{
    msgwin.Add(msg);

    std::string out = "[";

    int index = 0;

    std::vector<ASK_QUESTION_REC> keys;

    // `key` is a whitespace-separated list of accepted keys, e.g. "esc y n".
    static constexpr char WHITESPACE[] = " \t\n\r";

    for (std::string::size_type pos = 0;
         (pos = key.find_first_not_of(WHITESPACE, pos)) != std::string::npos; ) {
        const auto token_end = key.find_first_of(WHITESPACE, pos);
        const std::string token = key.substr(pos, token_end == std::string::npos
            ? std::string::npos
            : token_end - pos);
        pos = (token_end == std::string::npos) ? key.size() : token_end;

        ASK_QUESTION_REC aqr;
        std::string variant;

        if (stricmp(token.c_str(), "esc") == 0) {
            variant = MSG_CYAN "ESC" MSG_LIGHTGRAY;
            aqr.key = KEY_ESC;
            aqr.val = "esc";
        } else if (stricmp(token.c_str(), "return") == 0) {
            variant = MSG_CYAN "Enter" MSG_LIGHTGRAY;
            aqr.key = KEY_ENTER;
            aqr.val = "enter";
        } else {
            variant = va[index].get<std::string>();
            index++;

            // Highlight the answer's first letter with the key that picks
            // it - which is the key token's letter, not necessarily the
            // one the answer text starts with.
            variant.replace(0, 1, std::string(MSG_CYAN) + token[0] + MSG_LIGHTGRAY);
            aqr.key = token[0];
            aqr.val = std::string(1, token[0]);
        }

        if (!keys.empty()) {
            out += ", ";
        }

        keys.push_back(aqr);
        out += variant;
    }

    out += "]";

    msgwin.Add(out);
    vRefresh();
    int ch = vGetch();
    msgwin.ClrMsg();

    for (auto it: keys) {
        if (ch == it.key) {
            return it.val;
        }
    }

    // An empty or all-whitespace `key` yields no accepted answers at all,
    // and front() on the empty vector would be undefined - reachable from
    // a script calling AskQuestion(msg, "").
    if (keys.empty()) {
        return {};
    }

    return keys.front().val;
}

void SetEventHandler(void* cr, const std::string& event)
{
    ((XCreature*)cr)->SetEventHandler(event);
}

void EnableMoveHandler(void* cr)
{
    ((XCreature*)cr)->EnableMoveHandler();
}

void DisableMoveHandler(void* cr)
{
    ((XCreature*)cr)->DisableMoveHandler();
}

void SetMainCreature(void* cr)
{
    if (XSettings::isDemo) {
        XCreature::main_creature = (XCreature*)cr;
    }
}

XGUID GetObjectGUID(void* obj)
{
    return ((XObject*)obj)->guid();
}

std::tuple<int, int, int, int, int, std::string> GetItemParam(void* item)
{
    XItem * p = (XItem*)item;
    return {static_cast<int>(p->kind), static_cast<int>(p->aet), p->wt, static_cast<int>(p->it), p->quantity, p->name};
}

void SetItemBrand(void* item, int br)
{
    ((XItem*)item)->aet = (AttackEffectType)br;
}

int MakeEffect(int effect, void* caller, void* location, int call_x, int call_y, void* target, int target_x, int target_y, int power)
{
    EFFECT_DATA ed;
    ed.effect = (XEffect::Id)effect;
    ed.caller = (XCreature*)caller;
    ed.l = (XLocation*)location;
    ed.call_x = call_x;
    ed.call_y = call_y;
    ed.target = (XCreature*)target;
    ed.target_x = target_x;
    ed.target_y = target_y;
    ed.power = power;

    return XEffect::Make(&ed);
}

void DestroyObject(void* item)
{
    ((XItem*)item)->Invalidate();
}

void SetCompanion(void* owner, void* slave, bool flag)
{
    XCreature * pOwner = (XCreature*)owner;
    XCreature * pSlave = (XCreature*)slave;

    if (flag) {
        pSlave->xai->SetCompanion(pOwner);
        pSlave->xai->companion_command = CC_FOLLOW;
    } else {
        pSlave->xai->SetCompanion(nullptr);
    }
}

void GiveObjectToCreature(void* item, void* cr)
{
    ((XCreature*)cr)->ContainItem((XItem*)item);
}

bool GiveAward(void* owner_ptr, XGUID aguid, void* target_ptr)
{
    const auto owner = (XCreature*)owner_ptr;
    const auto target = (XCreature*)target_ptr;
    auto item = dynamic_cast<XItem *>(XObject::GetObject(aguid));

    // Keep a live shared_ptr across the whole transfer below - owner's
    // contain can be item's only reference, and erasing it here (before
    // target's contain takes it over) would run Own()'s deleter on a
    // still-valid item and invalidate it outright instead of just handing
    // it to target.
    std::shared_ptr<XItem> item_sp;

    // Worn items are still resident in contain the whole time (see
    // XBodyPart::Wear()), so unwear first if needed - UnWear() no longer
    // needs a matching contain.insert(), it was never removed.
    for (const auto& bp: owner->components) {
        if (bp->Item() && bp->Item() == item) {
            bp->UnWear();
            break;
        }
    }

    const auto it = owner->contain.find(item);
    if (it != owner->contain.end()) {
        item_sp = *it;
        owner->contain.erase(it);
    }

    if (item) {
        owner->UnCarryItem(item);

        if (target->CarryItem(item)) {
            target->contain.insert(item_sp);
        } else {
            owner->DropItem(item);
        }

        return true;
    }

    return false;
}

void Quest(int quest_id, int status, const std::string& know, const std::string& complete, const std::string& closed)
{
    auto qr = std::make_unique<XQuestRec>();
    qr->quest_id = quest_id;
    qr->status = (XQuest::Id)status;
    qr->know = know;
    qr->complete = complete;
    qr->closed = closed;
    XQuest::quest.quests.push_back(std::move(qr));
}

void QuestModify(int id, int status)
{
    XQuestRec * qr = XQuest::quest.Find(id);

    if (qr) {
        qr->status = (XQuest::Id)status;
    }
}

int QuestStatus(int id)
{
    XQuestRec * qr = XQuest::quest.Find(id);
    return qr ? qr->status : XQuest::UNKNOWN;
}

int StoreInt(lua_State * L)
{
    int tx = lua_tonumber(L, 1);
    XLocation::lua_int_buffer->push_back(tx);

    return 0;
}

int RestoreInt(lua_State * L)
{
    lua_pushnumber(L, (*XLocation::lua_int_buffer)[XLocation::lua_int_index++]);

    return 1;
}

bool BinaryAND(int v1, int v2)
{
    return v1 & v2;
}

// The item worn in a body part slot, or nil when the slot is empty.
void* GetWornItem(void* cr, const int bodypart, const int slot)
{
    XBodyPart* bp = ((XCreature*)cr)->GetBodyPart((BODY_PART)bodypart, slot);

    return (bp && bp->Item()) ? static_cast<void*>(bp->Item()) : nullptr;
}

// An object's registered class name - the same identity CreateObject() and
// PlaceSpecial() take, so script can recognise a specific kind of object
// rather than only its broad ItemType.
std::string GetObjectClass(void* obj)
{
    return ((XObject*)obj)->GetClassName();
}

void RegisterActorApi(sol::state_view& lua)
{
    lua_register(lua.lua_state(), "StoreInt", lua_api::StoreInt);
    lua_register(lua.lua_state(), "RestoreInt", lua_api::RestoreInt);
        lua.set_function("isHero", &lua_api::isHero);
        lua.set_function("isEnemy", &lua_api::isEnemy);
        lua.set_function("SetCreatureAI", &lua_api::SetCreatureAI);
        lua.set_function("AsCreature", &lua_api::AsCreature);
        lua.set_function("AsItem", &lua_api::AsItem);
        lua.set_function("GetWornItem", &lua_api::GetWornItem);
        lua.set_function("GetObjectClass", &lua_api::GetObjectClass);
        lua.set_function("GetCreatureCount", &lua_api::CreatureCountInLocation);
        lua.set_function("FindCreature", &lua_api::FindCreature);
        lua.set_function("FindCreatures", &lua_api::FindCreatures);
        lua.set_function("ExecuteCreatureScript", &lua_api::ExecuteCreatureScript);
        lua.set_function("GetWayXY", &lua_api::GetWayXY);
        lua.set_function("AddMessage", &lua_api::AddMessage);
        lua.set_function("AskQuestion", &lua_api::AskQuestion);
        lua.set_function("SetItEnemyFor", &lua_api::SetItEnemyFor);
        lua.set_function("SetEnemy", &lua_api::SetEnemy);
        lua.set_function("ChangeStats", &lua_api::ChangeStats);
        lua.set_function("GetStats", &lua_api::GetStats);
        lua.set_function("InflictDamage", &lua_api::InflictDamage);
        lua.set_function("Rand", &lua_api::Rand);
        lua.set_function("SetEventHandler", &lua_api::SetEventHandler);
        lua.set_function("EnableMoveHandler", &lua_api::EnableMoveHandler);
        lua.set_function("DisableMoveHandler", &lua_api::DisableMoveHandler);
        lua.set_function("SetMainCreature", &lua_api::SetMainCreature);
        lua.set_function("GetSkill", &lua_api::GetSkill);
        lua.set_function("LearnSkill", &lua_api::LearnSkill);
        lua.set_function("MoneyOperation", &lua_api::MoneyOperation);
        lua.set_function("SetName", &lua_api::SetName);
        lua.set_function("SetView", &lua_api::SetView);
        lua.set_function("GetView", &lua_api::GetView);
        lua.set_function("GetObjectGUID", &lua_api::GetObjectGUID);
        lua.set_function("GetItemParam", &lua_api::GetItemParam);
        lua.set_function("SetItemBrand", &lua_api::SetItemBrand);
        lua.set_function("GiveObjectToCreature", &lua_api::GiveObjectToCreature);
        lua.set_function("GiveAward", &lua_api::GiveAward);
        lua.set_function("MakeEffect", &lua_api::MakeEffect);
        lua.set_function("DestroyObject", &lua_api::DestroyObject);
        lua.set_function("SetCompanion", &lua_api::SetCompanion);
        lua.set_function("Quest", &lua_api::Quest);
        lua.set_function("QuestModify", &lua_api::QuestModify);
        lua.set_function("QuestStatus", &lua_api::QuestStatus);
        lua.set_function("Gender", &lua_api::Gender);
        lua.set_function("BinaryAND", &lua_api::BinaryAND);
}

} // namespace lua_api
