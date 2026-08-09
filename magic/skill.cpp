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

#include <fmt/format.h>
#include <sol/sol.hpp>

#include "creature/creature.h"
#include "helpers/msgwin.h"
#include "helpers/xgui.h"
#include "magic/skill.h"
#include "map/map_objects.h"

void XSkill::RegisterLua(sol::state_view& lua)
{
    lua.new_enum("XSkill",
        "ARCHERY", XSkill::ARCHERY,
        "FINDWEAKNESS", XSkill::FINDWEAKNESS,
        "HEALING", XSkill::HEALING,
        "CONCENTRATION", XSkill::CONCENTRATION,
        "DODGE", XSkill::DODGE,
        "TRADING", XSkill::TRADING,
        "STEALING", XSkill::STEALING,
        "LITERACY", XSkill::LITERACY,
        "DETECTTRAP", XSkill::DETECTTRAP,
        "DISARMTRAP", XSkill::DISARMTRAP,
        "COOKING", XSkill::COOKING,
        "MINING", XSkill::MINING,
        "HERBALISM", XSkill::HERBALISM,
        "RELIGION", XSkill::RELIGION,
        "BACKSTABBING", XSkill::BACKSTABBING,
        "FIRST_AID", XSkill::FIRST_AID,
        "TACTICS", XSkill::TACTICS,
        "ALCHEMY", XSkill::ALCHEMY,
        "WOODCRAFT", XSkill::WOODCRAFT,
        "CREATETRAP", XSkill::CREATETRAP,
        "NECROMANCY", XSkill::NECROMANCY,
        "ATHLETICS", XSkill::ATHLETICS,
        "CLIMBING", XSkill::CLIMBING
    );
}

SKILL_DB skill_db[] = {
    {"Archery", 1},
    {"Find Weakness", 1},
    {"Healing", 2},
    {"Concentration", 3},
    {"Dodge", 1},
    {"Trading", 1},
    {"Stealing", 1},
    {"Literacy", 1},
    {"Detect trap", 1},
    {"Disarm trap", 1},
    {"Cooking", 1},
    {"Mining", 5},
    {"Herbalism", 1},
    {"Religion", 1},
    {"Backstabbing", 1},
    {"First aid",	2},
    {"Tactics",	1},
    {"Alchemy",	1},
    {"Woodcraft",	1},
    {"Create trap",	1},
    {"Necromancy",	1},
    {"Athletics",	1},
    {"Climbing",	1},
};

const char* skill_level_name[16] = {
    MSG_LIGHTGRAY "NONE",
    MSG_LIGHTGREEN "Basic",
    MSG_LIGHTGREEN "Basic",
    MSG_LIGHTGREEN "Basic",
    MSG_GREEN "Skilled",
    MSG_GREEN "Skilled",
    MSG_GREEN "Skilled",
    MSG_GREEN "Skilled",
    MSG_YELLOW "Expert",
    MSG_YELLOW "Expert",
    MSG_YELLOW "Expert",
    MSG_YELLOW "Expert",
    MSG_LIGHTRED "Master",
    MSG_LIGHTRED "Master",
    MSG_LIGHTRED "Master",
    MSG_DARKGRAY "Grand Master"
};

XSkill::XSkill(XSkill::Skill _skt, int _level)
{
    skt = _skt;
    level = _level;

    if (_level < 4) {
        used_time = skill_db[skt].use_per_level * (4 * 4 - 1) * 2;
    } else {
        used_time = skill_db[skt].use_per_level * (level * level - 1) * 2;
    }
}

const char* XSkill::GetName()
{
    return skill_db[skt].name;
}

const char* XSkill::GetSkillLevel()
{
    assert(level <= SKILL_MAX_LEVEL);
    return skill_level_name[level];
}

int XSkill::GetMaxLevel()
{
    int xlevel = (int)(sqrt((float)(used_time / (2 * skill_db[skt].use_per_level) + 1)));
    return xlevel < 15 ? xlevel : 15;
}

int XSkill::IncLevel()
{
    if (level < GetMaxLevel()) {
        level++;
        return 1;
    } else {
        return 0;
    }
}

SKILL_MASTERY XSkill::GetMastery()
{
    switch (level) {
        case 1 :
        case 2 :
        case 3 :
            return SM_BASIC;
            break;

        case 4 :
        case 5 :
        case 6 :
        case 7 :
            return SM_SKILLED;
            break;

        case 8 :
        case 9 :
        case 10:
        case 11:
            return SM_EXPERT;
            break;

        case 12:
        case 13:
        case 14:
            return SM_MASTER;
            break;

        case 15:
            return SM_GRANDMASTER;
            break;
    }

    return SM_BASIC;
}

int XSkill::Use(XCreature * user)
{
    switch (skt) {
        case XSkill::Skill::STEALING :
            return UseSteal(user);

        case XSkill::Skill::DISARMTRAP :
            return UseDisarm(user);

        case XSkill::Skill::CREATETRAP:
            return UseCreate(user);

    };

    return 1;
}

int XSkill::isUseable() const
{
    if (skt == XSkill::Skill::STEALING || skt == XSkill::Skill::DISARMTRAP || skt == XSkill::Skill::CREATETRAP) {
        return 1;
    } else {
        return 0;
    }
}

int XSkill::UseSteal(XCreature * user)
{
    XPoint pt;
    XItem * object;

    if (user->GetTarget(TR_STEAL_ITEM, &pt, 0, (XObject**)&object)) {
        if (pt.x == 0 && pt.y == 0) {
            msgwin.Add("Stealing from yourself? You are successful!");
            user->contain.insert(XItem::Own(object));
        } else {
            XCreature * cr = user->l->map->GetMonster(user->x + pt.x, user->y + pt.y);
            int flag = 0;

            if (cr == nullptr) {
                XAnyPlace * pl = user->l->map->GetPlace(user->x + pt.x, user->y + pt.y);

                if (pl) {
                    cr = pl->GetOwner().lock().get();
                    flag = 1;
                }
            }

            assert(cr);

            if (!flag && cr->IsWorn(object)) {
                if (user->isVisible()) {
                    msgwin.Add("You can't steal that - it's currently equipped.");
                }

                return 1;
            }

            double perception = 1 + cr->stats->Get(XStats::PER);
            double stealing = 1 + user->sk->GetLevel(XSkill::Skill::STEALING);
            int p = (int)((stealing * 300) / perception);

            if (vRand() % 100 < p || !user->isVisible()) {
                if (user->isVisible()) {
                    msgwin.Add(fmt::format("You steal {}.", object->toString()));
                }

                cr->UnCarryItem(object);
                user->CarryItem(object);

                // object is still sitting wherever GetTarget(TR_STEAL_ITEM,
                // ...) found it (IF_NO_ERASE - it only had a raw pointer to
                // hand back through *back, so it couldn't safely erase it
                // there). Establish user->contain's ownership first, then
                // erase from the source - never the other way around, or
                // the erase could run Own()'s deleter on a still-valid item.
                user->contain.insert(XItem::Own(object));

                if (flag) {
                    auto* src = user->l->map->GetItemList(user->x + pt.x, user->y + pt.y);

                    if (auto it = src->find(object); it != src->end()) {
                        src->erase(it);
                    }
                } else {
                    if (auto it = cr->contain.find(object); it != cr->contain.end()) {
                        cr->contain.erase(it);
                    }
                }

                UseSkill(6);
            } else {
                if (user->isVisible()) {
                    msgwin.Add(cr->name);
                    msgwin.Add("notices your efforts and becomes angry.");
                }

                cr->xai->onSteal(user);

                // Theft failed - object was never erased from its source
                // (see the IF_NO_ERASE comment above), so it's already
                // right where it should stay. Nothing to do.
            }
        }
    }

    return 1;
}

int XSkill::UseDisarm(XCreature * user)
{
    XMapObject * obj = user->l->map->GetSpecial(user->x, user->y);

    if (auto* trap = dynamic_cast<XTrap *>(obj)) {
        if (trap->Disarm(user)) {
            UseSkill(1);
        }
    } else {
        msgwin.Add("There is no trap here.");
    }

    return 1;
}

struct TRAP_CREATE_REC {
    const char* name;
    bool isMagic;
    int level;
    unsigned int var;   // neccessary item or spell
    unsigned int var2;  // tools
} trap_create_rec[] = {

    {"Arrow trap",	false,	0, static_cast<unsigned int>(ItemType::ARROW),	0},
    {"Spear trap",	false,	2, static_cast<unsigned int>(ItemType::SHORTSPEAR),	0},
    {"Magic Arrow trap",	true,	4, SPELL_MAGIC_ARROW,	0},
    {"Fire Bolt trap",	true,	6, SPELL_FIRE_BOLT,	0},
    {"Pit",	false,	8, 0,	static_cast<unsigned int>(ItemType::PICKAXE)},
    {"Acid Bolt trap",	true,	10, SPELL_ACID_BOLT,	0},
    {"Spear Pit",	false,	12, static_cast<unsigned int>(ItemType::SHORTSPEAR),	static_cast<unsigned int>(ItemType::PICKAXE)},
    {nullptr, false, 1000, 0, 0}
};


int TrapArrowsFiltr(XItem * item)
{
    if (item->kind & ItemKind::MISSILE && (item->it == ItemType::ARROW || item->it == ItemType::QUARREL)) {
        return 1;
    } else {
        return 0;
    }
}

int TrapSpearsFiltr(XItem * item)
{
    if (item->kind & ItemKind::WEAPON && (item->it == ItemType::SHORTSPEAR || item->it == ItemType::LONGSPEAR)) {
        return 1;
    } else {
        return 0;
    }
}

int XSkill::UseCreate(XCreature * user)
{
    if (user->l->map->GetSpecial(user->x, user->y)) {
        msgwin.Add("You can not create trap here.");
        return 0;
    }

    XGuiList list;
    list.SetCaption(MSG_BROWN "###" MSG_LIGHTGRAY " Create Trap " MSG_BROWN "###");
    int i = 0;

    while (trap_create_rec[i].name && trap_create_rec[i].level < level) {
        list.AddItem(new XGuiItem_SimpleSelect(trap_create_rec[i].name), 0);
        i++;
    }

    int ch = list.Run();

    if (trap_create_rec[ch].isMagic) {
        XSpell * sp = user->m->GetSpell((SPELL_NAME)trap_create_rec[ch].var);

        if (sp) {
            int count = user->_PP / (sp->GetManaCost() * 2);

            if (count == 0) {
                msgwin.Add("You don't have enough mana!");
            } else {
                switch (trap_create_rec[ch].var) {
                    case SPELL_MAGIC_ARROW:
                        (new XTrap(user->x, user->y, user->l, TL_RANDOM, TT_MAGICARROW, user))->activation_count = count;
                        break;

                    case SPELL_FIRE_BOLT:
                        (new XTrap(user->x, user->y, user->l, TL_RANDOM, TT_FIREBOLT, user))->activation_count = count;
                        break;

                    case SPELL_ACID_BOLT:
                        (new XTrap(user->x, user->y, user->l, TL_RANDOM, TT_ACIDBOLT, user))->activation_count = count;
                        break;
                }

                user->_PP -= sp->GetManaCost() * 2 * count;
                user->sk->UseSkill(XSkill::Skill::CREATETRAP, 10);
                msgwin.Add("You have successfuly created a trap!");
            }
        } else {
            msgwin.Add("You have to learn spell first!");
        }
    } else {
        // this trap created from items...
        std::shared_ptr<XItem> item;

        if (trap_create_rec[ch].var == static_cast<unsigned int>(ItemType::ARROW)) {
            item = user->SelectItem(&TrapArrowsFiltr, true);
        } else if (trap_create_rec[ch].var == static_cast<unsigned int>(ItemType::SHORTSPEAR)) {
            item = user->SelectItem(&TrapSpearsFiltr, true);
        }

        if (trap_create_rec[ch].var2 == static_cast<unsigned int>(ItemType::PICKAXE)) {
            if (user->GetBodyPart(BP_TOOL, 0)->Item()->it == ItemType::PICKAXE) {
                if (item && trap_create_rec[ch].var > 0) {
                    new XTrap(user->x, user->y, user->l, TL_RANDOM, TT_SPEAR_PIT, user, item.get());
                    user->sk->UseSkill(XSkill::Skill::CREATETRAP, 20);
                    msgwin.Add("You have successfuly created a trap!");
                } else if (trap_create_rec[ch].var == 0) {
                    new XTrap(user->x, user->y, user->l, TL_RANDOM, TT_PIT, user, nullptr);
                    user->sk->UseSkill(XSkill::Skill::CREATETRAP, 10);
                    msgwin.Add("You have successfuly created a trap!");
                }
            } else {
                msgwin.Add("You should wield a pickaxe!");
            }
        } else if (item) {
            new XTrap(user->x, user->y, user->l, TL_RANDOM, TT_ARROW, user, item.get());
            user->sk->UseSkill(XSkill::Skill::CREATETRAP, 15);
            msgwin.Add("You have successfuly created a trap!");
        }
    }

    return 1;
}
