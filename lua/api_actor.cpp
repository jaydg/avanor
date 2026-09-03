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

// Character building, for InitHero (world/hero.lua). Free functions rather
// than methods on the XCreature usertype: adding names there is the one
// operation this codebase has caught corrupting the Lua state (see the note
// at XCreature::RegisterLua).
void SetStats(void* cr, const std::string& dice)
{
    // Replaces, and does not add to, whatever the creature had: XStats::Set
    // accumulates (stats[i] += ...), so setting a character's figures has to
    // start from a fresh object or a second call would double them.
    ((XCreature*)cr)->stats = std::make_unique<XStats>(dice.c_str());
}

void AddStats(void* cr, const std::string& dice)
{
    const XStats extra(dice.c_str());
    ((XCreature*)cr)->stats->Add(&extra);
}

void SetMaxStats(void* cr, const std::string& dice)
{
    ((XCreature*)cr)->max_stats.Set(dice.c_str());
}

// No stat may end below 1, however unkind the race and profession were to
// each other.
void ClampStats(void* cr)
{
    auto* c = (XCreature*)cr;

    for (int i = XStats::STR; i < XStats::COUNT; i++) {
        if (c->stats->Get(static_cast<XStats::Id>(i)) < 1) {
            c->stats->SetStat(static_cast<XStats::Id>(i), 1);
        }
    }
}

// How long a turn takes this character - "0d0+1000" is the ordinary pace.
void SetMoveEnergy(void* cr, const std::string& dice)
{
    XDice d(dice.c_str());
    ((XCreature*)cr)->SetMoveEnergy(d.Throw());
}

void SetFoodFeeling(void* cr, int ff)
{
    ((XCreature*)cr)->food_feeling = static_cast<FOOD_FEELING>(ff);
}

int GetStats(void* cr, int st)
{
    return ((XCreature*)cr)->GetStats((XStats::Id)st);
}

int Rand(int val)
{
    return vRand(val);
}

// The no-argument draw. Not the same as Rand(n) and not interchangeable
// with it: vRand() advances the generator twice and combines both halves,
// where vRand(n) advances it once. Content that has to reproduce a
// sequence the engine used to make - the starting kit, which the old C++
// wrote as vRand() % 10 + 10 - needs this exact form.
// Returns the draw whole. vRand() combines two 32-bit halves of a 64-bit
// generator and can answer with far more than an int holds, so narrowing it
// here would change what "Rand() % 10" means - which is exactly what the
// starting kit does with it.
double RandRaw()
{
    return static_cast<double>(vRand());
}

void SetWarSkill(void* cr, int wt, int level)
{
    ((XCreature*)cr)->wsk->SetLevel(static_cast<XWarSkills::Type>(wt), level);
}

// Which war skill an item trains, for "you begin competent with whatever
// you were handed".
int GetItemWarSkill(void* item)
{
    return item ? static_cast<int>(((XItem*)item)->wt) : XWarSkills::OTHER;
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

int CreatureCountInLocation(const std::string& l_id, CreatureClass cc)
{
    return Game.Location(l_id)->GetCreatureCount(cc);
}

bool isEnemy(void* cr1, void* cr2)
{
    XCreature * p1 = (XCreature*)cr1;
    XCreature * p2 = (XCreature*)cr2;
    return p1 && p2 && p1->xai->isEnemy(p2);
}

std::string GetGroupID(void* cr)
{
    XCreature* p = (XCreature*)cr;
    return p ? p->groupID() : std::string();
}

std::tuple<int, int> GetCreatureXY(void* cr)
{
    XCreature* p = (XCreature*)cr;
    return p ? std::tuple<int, int>{p->x, p->y} : std::tuple<int, int>{-1, -1};
}

sol::optional<void*> FindCreature(const std::string& l_id, const std::string& gid, sol::optional<int> x, sol::optional<int> y, sol::optional<int> w, sol::optional<int> h)
{
    XRect rect(0, 0, Game.Location(l_id)->map->len, Game.Location(l_id)->map->hgt);

    if (x) {
        rect.left = *x;
        rect.top = *y;
        rect.right = rect.left + *w;
        rect.bottom = rect.top + *h;
    }

    for (int i = rect.left; i < rect.right; i++)
        for (int j = rect.top; j < rect.bottom; j++) {
            XCreature* cr = Game.Location(l_id)->map->GetMonster(i, j);

            if (cr && cr->groupID() == gid) {
                return cr;
            }
        }

    // Nothing found - nil, not a null pointer dressed as a value. See
    // GetWornItem().
    return sol::nullopt;
}

std::vector<void*> FindCreatures(const std::string& l_id, const std::string& gid, sol::optional<int> x, sol::optional<int> y, sol::optional<int> w, sol::optional<int> h)
{
    XRect rect(0, 0, Game.Location(l_id)->map->len, Game.Location(l_id)->map->hgt);

    if (x) {
        rect.left = *x;
        rect.top = *y;
        rect.right = rect.left + *w;
        rect.bottom = rect.top + *h;
    }

    std::vector<void*> result;

    for (int i = rect.left; i < rect.right; i++)
        for (int j = rect.top; j < rect.bottom; j++) {
            XCreature* cr = Game.Location(l_id)->map->GetMonster(i, j);

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
        cmd.ln = row.get_or<std::string>("ln", "");
        cmd.kind = row.get_or("kind", ItemKind::UNKNOWN);
        cmds.push_back(cmd);
    }

    ((XCreature*)cr)->xai->ExecuteScript(cmds);
}

std::tuple<int, int> GetWayXY(const std::string& l_id)
{
    XStairWay* way = (XStairWay*)*Game.Location(l_id)->ways_list.begin();
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

// Puts a new creature on the ground beside an existing one, in that one's
// own location. Creature() cannot serve here: it builds into whatever
// location the world was last laying out, which is a world-construction
// notion and says nothing about where a creature is standing now.
//
// nil when all eight neighbouring cells are taken - the caller decides
// whether that matters.
sol::optional<void*> CreatureNear(void* who, const std::string& name)
{
    auto* beside = (XCreature*)who;

    if (!beside || !beside->l) {
        return sol::nullopt;
    }

    XRect around(beside->x - 1, beside->y - 1, beside->x + 1, beside->y + 1);

    if (const auto spot = beside->l->GetFreeXY(&around)) {
        if (XCreature* made = beside->l->NewCreature(name, spot->x, spot->y)) {
            return static_cast<void*>(made);
        }
    }

    return sol::nullopt;
}

void SetAIFlag(void* cr, unsigned int flags)
{
    ((XCreature*)cr)->xai->SetAIFlag(static_cast<XStandardAI::Flag>(flags));
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
            variant = "<KEY>ESC<TEXT>";
            aqr.key = KEY_ESC;
            aqr.val = "esc";
        } else if (stricmp(token.c_str(), "return") == 0) {
            variant = "<KEY>Enter<TEXT>";
            aqr.key = KEY_ENTER;
            aqr.val = "enter";
        } else {
            variant = va[index].get<std::string>();
            index++;

            // Highlight the answer's first letter with the key that picks
            // it - which is the key token's letter, not necessarily the
            // one the answer text starts with.
            variant.replace(0, 1, std::string("<KEY>") + token[0] + "<TEXT>");
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

// Reading and changing a finished item from an :OnCreate() handler.
// Naming it is a plain set; the three numbers are always adjustments,
// because what the template rolled is the starting point, not a draft.
// The sort of missile a launcher fires, for content that has to produce
// ammunition to go with a weapon. Answers from the missiles' own
// :Launcher() declarations, so the relationship is stated once, on the
// missile rows, rather than a second time wherever ammo is handed out.
sol::optional<int> MissileForLauncher(void* weapon)
{
    const XItem* w = (XItem*)weapon;

    if (!w || w->wt == XWarSkills::OTHER) {
        return sol::nullopt;
    }

    for (int i = 0; i < gi_missile.total_item; i++) {
        const ItemTemplate& row = gi_missile.pFirstItem[i];

        if (row.launcher != XWarSkills::OTHER && row.launcher == w->wt) {
            return static_cast<int>(row.it);
        }
    }

    return sol::nullopt;
}

std::string GetItemName(void* item)
{
    return ((XItem*)item)->name;
}

void SetItemName(void* item, const std::string& name)
{
    ((XItem*)item)->name = name;
}

void AddItemToHit(void* item, int bonus)
{
    ((XItem*)item)->to_hit += bonus;
}

// A missile's own contribution to how far its wielder can shoot.
void AddItemRange(void* item, int bonus)
{
    ((XItem*)item)->RNG += bonus;
}

void AddItemDice(void* item, int count, int sides, int bonus)
{
    ((XItem*)item)->dice.Add(count, sides, bonus);
}

// The caster's own form of an effect: unlike MakeEffect() below, this is
// the one that asks the player where to aim when the effect needs a
// direction or a target, and answers ABORT if they change their mind. What
// an item that casts something wants.
int CastEffect(void* caster, int effect, int power)
{
    if (!caster) {
        return ABORT;
    }

    return XEffect::Make((XCreature*)caster, (XEffect::Id)effect, power);
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

void Quest(int quest_id, int status, const std::string& know, const std::string& complete, const std::string& closed, sol::optional<int> score)
{
    auto qr = std::make_unique<XQuestRec>();
    qr->quest_id = quest_id;
    qr->status = (XQuest::Id)status;
    qr->know = know;
    qr->complete = complete;
    qr->closed = closed;
    qr->score = score.value_or(0);
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
//
// sol::optional and not a plain void*, because that is the only way to
// say "nothing" to Lua: a null void* is pushed as light userdata, which
// is not nil and is perfectly true, so `if (item)` let an empty slot
// straight through and whatever came next was handed a null pointer.
sol::optional<void*> GetWornItem(void* cr, const int bodypart, const int slot)
{
    XBodyPart* bp = ((XCreature*)cr)->GetBodyPart((BODY_PART)bodypart, slot);

    if (!bp || !bp->Item()) {
        return sol::nullopt;
    }

    return static_cast<void*>(bp->Item());
}

// An object's registered class name - the same identity CreateObject() and
// PlaceSpecial() take, so script can recognise a specific kind of object
// rather than only its broad ItemType.
// The id world/ defined this item under, or "" for one that is still a
// C++ class. What GetObjectClass() used to be asked for - "is this that
// exact item" - now that many different items share one carrier class.
std::string GetItemId(void* item)
{
    if (!item) {
        return {};
    }

    return ((XItem*)item)->GetContentId();
}

std::string GetObjectClass(void* obj)
{
    if (!obj) {
        std::cerr << "lua: GetObjectClass() was given nothing to look at" << std::endl;

        throw std::invalid_argument("GetObjectClass: null object");
    }

    return ((XObject*)obj)->GetClassName();
}

void RegisterActorApi(sol::state_view& lua)
{
    lua_register(lua.lua_state(), "StoreInt", lua_api::StoreInt);
    lua_register(lua.lua_state(), "RestoreInt", lua_api::RestoreInt);
        lua.set_function("isHero", &lua_api::isHero);
        lua.set_function("GetGroupID", &lua_api::GetGroupID);
        lua.set_function("GetCreatureXY", &lua_api::GetCreatureXY);
        lua.set_function("isEnemy", &lua_api::isEnemy);
        lua.set_function("SetCreatureAI", &lua_api::SetCreatureAI);
        lua.set_function("AsCreature", &lua_api::AsCreature);
        lua.set_function("AsItem", &lua_api::AsItem);
        lua.set_function("GetWornItem", &lua_api::GetWornItem);
        lua.set_function("GetObjectClass", &lua_api::GetObjectClass);
        lua.set_function("GetItemId", &lua_api::GetItemId);
        lua.set_function("GetCreatureCount", &lua_api::CreatureCountInLocation);
        lua.set_function("FindCreature", &lua_api::FindCreature);
        lua.set_function("FindCreatures", &lua_api::FindCreatures);
        lua.set_function("ExecuteCreatureScript", &lua_api::ExecuteCreatureScript);
        lua.set_function("GetWayXY", &lua_api::GetWayXY);
        lua.set_function("AddMessage", &lua_api::AddMessage);
        lua.set_function("AskQuestion", &lua_api::AskQuestion);
        lua.set_function("SetItEnemyFor", &lua_api::SetItEnemyFor);
        lua.set_function("SetEnemy", &lua_api::SetEnemy);
        lua.set_function("SetAIFlag", &lua_api::SetAIFlag);
        lua.set_function("CreatureNear", &lua_api::CreatureNear);
        lua.set_function("ChangeStats", &lua_api::ChangeStats);
        lua.set_function("GetStats", &lua_api::GetStats);
        lua.set_function("SetStats", &lua_api::SetStats);
        lua.set_function("AddStats", &lua_api::AddStats);
        lua.set_function("SetMaxStats", &lua_api::SetMaxStats);
        lua.set_function("ClampStats", &lua_api::ClampStats);
        lua.set_function("SetMoveEnergy", &lua_api::SetMoveEnergy);
        lua.set_function("SetFoodFeeling", &lua_api::SetFoodFeeling);
        lua.set_function("InflictDamage", &lua_api::InflictDamage);
        lua.set_function("Rand", sol::overload(&lua_api::Rand, &lua_api::RandRaw));
        lua.set_function("SetWarSkill", &lua_api::SetWarSkill);
        lua.set_function("GetItemWarSkill", &lua_api::GetItemWarSkill);
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
        lua.set_function("GetItemName", &lua_api::GetItemName);
        lua.set_function("MissileForLauncher", &lua_api::MissileForLauncher);
        lua.set_function("SetItemName", &lua_api::SetItemName);
        lua.set_function("AddItemToHit", &lua_api::AddItemToHit);
        lua.set_function("AddItemRange", &lua_api::AddItemRange);
        lua.set_function("AddItemDice", &lua_api::AddItemDice);
        lua.set_function("GiveObjectToCreature", &lua_api::GiveObjectToCreature);
        lua.set_function("GiveAward", &lua_api::GiveAward);
        lua.set_function("MakeEffect", &lua_api::MakeEffect);
        lua.set_function("CastEffect", &lua_api::CastEffect);
        lua.set_function("DestroyObject", &lua_api::DestroyObject);
        lua.set_function("SetCompanion", &lua_api::SetCompanion);
        lua.set_function("Quest", &lua_api::Quest);
        lua.set_function("QuestModify", &lua_api::QuestModify);
        lua.set_function("QuestStatus", &lua_api::QuestStatus);
        lua.set_function("Gender", &lua_api::Gender);
        lua.set_function("BinaryAND", &lua_api::BinaryAND);
}

} // namespace lua_api
