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

#include <algorithm>
#include <fmt/format.h>
#include <cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>
#include <sol/sol.hpp>

#include "creature/std_ai.h"
#include "engine/xapi.h"
#include "game/game.h"
#include "helpers/msgwin.h"
#include "map/map_objects.h"

void XStandardAI::RegisterLua(sol::state_view& lua)
{
    lua.new_enum("XStandardAI",
        "ALLOW_PICK_UP", XStandardAI::ALLOW_PICK_UP,
        "ALLOW_MOVE_WAY_UP", XStandardAI::ALLOW_MOVE_WAY_UP,
        "ALLOW_MOVE_WAY_DOWN", XStandardAI::ALLOW_MOVE_WAY_DOWN,
        "FREE_WAY", XStandardAI::FREE_WAY,
        "ALLOW_MOVE_OUT", XStandardAI::ALLOW_MOVE_OUT,
        "FREE_MOVE", XStandardAI::FREE_MOVE,
        "FIND_WAY", XStandardAI::FIND_WAY,
        "PEACEFUL", XStandardAI::PEACEFUL,
        "COWARD", XStandardAI::COWARD,
        "ALLOW_PACK", XStandardAI::ALLOW_PACK,
        "ALLOW_WEAR_ITEM", XStandardAI::ALLOW_WEAR_ITEM,
        "GUARD_AREA", XStandardAI::GUARD_AREA,
        "PROTECT_AREA", XStandardAI::PROTECT_AREA,
        "RANDOM_MOVE", XStandardAI::RANDOM_MOVE,
        "EXPLORER_MOVE", XStandardAI::EXPLORER_MOVE,
        "EXECUTE_SCRIPT", XStandardAI::EXECUTE_SCRIPT,
        "NO_SWAP", XStandardAI::NO_SWAP,
        "INSECT", XStandardAI::INSECT,
        "LO_ANIMAL", XStandardAI::LO_ANIMAL,
        "HI_ANIMAL", XStandardAI::HI_ANIMAL,
        "CREATURE", XStandardAI::CREATURE,
        "HUMAN", XStandardAI::HUMAN,
        "GHOST", XStandardAI::GHOST
    );

    lua.new_enum("ScriptCommand",
        "NONE", SCC_NONE,
        "MOVE_POINT", SCC_MOVE_POINT,
        "MOVE_LOCATION", SCC_MOVE_LOCATION,
        "COLLECT_MUSHROOM", SCC_COLLECT_MUSHROOM,
        "DROP_ITEM", SCC_DROP_ITEM
    );

    // Named "CreatureAI" rather than "XStandardAI" - that name is already
    // taken above by the AI-flag enum table. Reachable from Lua as
    // AsCreature(cr).xai.
    lua.new_usertype<XStandardAI>("CreatureAI",
        "AddPersonalEnemy", &XStandardAI::AddPersonalEnemy,
        "SetGroupEnemy", &XStandardAI::SetGroupEnemy,
        "isEnemy", &XStandardAI::isEnemy,
        "SetCompanion", &XStandardAI::SetCompanion,
        "ReactToAttacker", &XStandardAI::ReactToAttacker,
        "ResAIFlag", &XStandardAI::ResAIFlag,
        "SetGuardArea", &XStandardAI::SetGuardArea
    );

}

// Directly instantiable (not just an abstract base for XShopKeeperAI),
// but still polymorphic (virtual destructor/methods), so Cereal's
// polymorphic dispatch applies even when the concrete object really is
// just a plain XStandardAI - needs its own registration, not only the
// base/derived relation in skeep_ai.cpp.
CEREAL_REGISTER_TYPE(XStandardAI);

XStandardAI::XStandardAI(XCreature* _cr) : guard_area(1, 1, 2, 3),
                                           enemy(nullptr), enemy_dist(0),
                                           friend_avg_x(0), friend_avg_y(0),
                                           friends_count(0),
                                           item_dist(0), item_x(0), item_y(0),
                                           way_dist(0), way_x(0), way_y(0)
{
    ai_owner = _cr;
    ai_flag = XStandardAI::NONE; //(XStandardAI::Flag)(XStandardAI::RANDOM_MOVE | XStandardAI::ALLOW_PICK_UP);

    enemy_class = CreatureClass::ALL;
    last_moved_way = nullptr;

    companion_command = CC_NONE;
    invisible_hunting_mode = 0;
    invisible_x = -1;
    invisible_y = -1;
    sleep_well = 0;
}

XStandardAI::~XStandardAI()
{
    ai_owner = nullptr;

    for (auto & i : personal_enemy) {
        i.reset();
    }
}

void XStandardAI::AnalyzeGrid(int j, int i, int w)
{
    //test for monsters
    XCreature * tgt = ai_owner->l->map->GetMonster(j, i);

    if (tgt && !ai_owner->isCreatureVisible(tgt)) {
        tgt = nullptr;
    }

    if (tgt && w < enemy_dist && w > 0 && isEnemy(tgt)) {
        enemy = tgt;
        enemy_dist = w;
    }

    //test for friends if
    if (ai_flag & XStandardAI::ALLOW_PACK && tgt && !isEnemy(tgt) && w > 0) {
        friends_count++;
        //make summ of all friend coord, then div it on friend count
        //so we got center of the pack
        friend_avg_x += tgt->x;
        friend_avg_y += tgt->y;
    }

    //test for items
    if (ai_flag & XStandardAI::ALLOW_PICK_UP &&
        (ai_owner->l->map->GetItemCount(j, i) > 0) && (w < item_dist)) {
        XAnyPlace * pl = ai_owner->l->map->GetPlace(j, i);

        if (!pl) {
            item_dist = w;
            item_x = j;
            item_y = i;
        }
    }

    //test for ways
    XMapObject * spec = ai_owner->l->map->GetSpecial(j, i);
    XStairWay * way = dynamic_cast<XStairWay *>(spec);

    if (way && (w < way_dist) && spec != last_moved_way &&
        (((spec->view == '>') && (ai_flag & XStandardAI::ALLOW_MOVE_WAY_DOWN)) ||
        ((spec->view == '<') && (ai_flag & XStandardAI::ALLOW_MOVE_WAY_UP)))
    ) {
        if (way->ln != XLocation::IdToKey(XLocation::MAIN) || ai_flag & XStandardAI::ALLOW_MOVE_OUT) {
            way_dist = w;
            way_x = j;
            way_y = i;
        }
    }
}

void XStandardAI::Move()
{
    // initializing variables
    enemy = nullptr;
    enemy_dist = 10000;
    item_dist = 10000;
    item_x = 0;
    item_y = 0;
    way_dist = 10000;
    way_x = 0;
    way_y = 0;

    //  process grids if no enemy can be attacked
    if (sleep_well <= 0) {
        friends_count = 1;
        friend_avg_x = ai_owner->x;
        friend_avg_y = ai_owner->y;

        // process all visible grids
        AnalyzeView(ai_owner->GetVisibleRadius());

        // calculate average coordinates for XStandardAI::ALLOW_PACK
        friend_avg_x = static_cast<int>(std::lround(static_cast<float>(friend_avg_x) / static_cast<float>(friends_count)));
        friend_avg_y = static_cast<int>(std::lround(static_cast<float>(friend_avg_y) / static_cast<float>(friends_count)));

        if (enemy_dist > 100) {
            sleep_well = 3;
        }
    } else {
        sleep_well--;
    }

    // trying to wear some item
    if (ai_flag & XStandardAI::ALLOW_WEAR_ITEM && enemy_dist > 1 && Wear()) {
        ai_owner->nx = ai_owner->x;
        ai_owner->ny = ai_owner->y;
        return;
    }

    int was_attack = 0;
    int was_item_pick = 0;

    // companion may have died independently since we last checked; lock once
    // and reuse for the rest of this function rather than re-locking per use.
    auto companion_sp = companion.lock();

    // first of all, execute order of companion to attack
    if (companion_sp && companion_command == CC_ATTACK) {
        auto ordered_enemy_sp = ordered_enemy.lock();

        if (ordered_enemy_sp && ordered_enemy_sp->isValid()) {
            enemy = ordered_enemy_sp.get();
        } else {
            ordered_enemy.reset();
            companion_command = CC_NONE;
        }
    }

    assert(ai_owner->isValid());
    assert(enemy != ai_owner);

    if (enemy) {
        // Keep enemy allocated across AttackEnemy(): it can die from that
        // very attack, and XCreature::Die()'s self-keepalive only covers
        // its own call, not this caller (see the matching fix in
        // XCreature::Attack(), creature2.cpp) - grab this reference now,
        // while enemy is still known-alive, so the ToWeakPtr() call below
        // has a real object to read even if enemy died in the meantime.
        auto enemy_keepalive = XCreature::ToWeakPtr(enemy).lock();

        //second try to attack enemy
        was_attack = AttackEnemy(enemy->x, enemy->y);

        // A PEACEFUL creature fights whatever's currently in front of it
        // (see the isEnemy() clause above) but doesn't give chase once
        // that's no longer true - remembering last_enemy is what drives
        // the MoveTo() pursuit below once `enemy` next comes up empty
        // (out of sight/range), so skip it here for PEACEFUL creatures.
        // Reactive pursuit after actually being attacked is skipped the
        // same way in ReactToAttacker().
        if (was_attack && !(ai_flag & XStandardAI::PEACEFUL)) {
            // enemy may have died from that very attack - only remember it
            // as last_enemy if it's still around.
            last_enemy = XCreature::ToWeakPtr(enemy);
        }
    } else if (invisible_hunting_mode > 0) {
        was_attack = AttackEnemy(invisible_x, invisible_y);

        if (!was_attack || (ai_owner->x == ai_owner->nx && ai_owner->y == ai_owner->ny)) {
            invisible_x = -1;
            invisible_y = -1;
            invisible_hunting_mode = 0;
        }
    } else if (companion_sp && (companion_command == CC_FOLLOW || companion_command == CC_NONE)
        && MoveTo(companion_sp->x, companion_sp->y, companion_sp->l))
    {
        //do nothing....
    } else if (auto last_enemy_sp = last_enemy.lock()) {
        if (!MoveTo(last_enemy_sp->x, last_enemy_sp->y, last_enemy_sp->l)) {
            last_enemy.reset();
        }
    } else if (ai_flag & XStandardAI::EXECUTE_SCRIPT) {
        //execute script when nothing to do
        RunScript();
    } else if (ai_flag & XStandardAI::ALLOW_PICK_UP &&
        !(ai_owner->l->map->GetItemList(ai_owner->x, ai_owner->y))->empty() &&
        !ai_owner->l->map->GetPlace(ai_owner->x, ai_owner->y))
    {
        if (PickUpItems()) {
            return;
        }
    } else if (ai_flag & XStandardAI::ALLOW_PICK_UP && item_dist < 10000) {
        MoveTo(item_x, item_y);
        was_item_pick = 1;
    } else if (ai_flag & (XStandardAI::ALLOW_MOVE_WAY_DOWN | XStandardAI::ALLOW_MOVE_WAY_UP)
        && way_dist < 10000 && !(ai_flag & XStandardAI::GUARD_AREA))
    {
        auto spec = ai_owner->l->map->GetSpecial(ai_owner->x, ai_owner->y);

        if (dynamic_cast<XStairWay *>(spec) &&
            (((spec->view == '>') && (ai_flag & XStandardAI::ALLOW_MOVE_WAY_DOWN)) ||
            ((spec->view == '<') && (ai_flag & XStandardAI::ALLOW_MOVE_WAY_UP)))) {
            ai_owner->MoveStairWay();
            last_moved_way = ai_owner->l->map->GetSpecial(ai_owner->x, ai_owner->y);
        } else {
            MoveTo(way_x, way_y);
        }
    } else if (ai_flag & XStandardAI::ALLOW_PACK) {
        // Allow to create packs....
        XPoint direction_point;
        XPoint target_point(friend_avg_x, friend_avg_y);
        GetRandDirection(&target_point, &direction_point);
        ai_owner->nx = ai_owner->x + direction_point.x;
        ai_owner->ny = ai_owner->y + direction_point.y;

        if (ai_owner->l->map->XGetMovability(ai_owner->nx, ai_owner->ny) != 0) {
            ai_owner->nx = ai_owner->x + vRand(3) - 1;
            ai_owner->ny = ai_owner->y + vRand(3) - 1;
        }
    } else if (ai_flag & XStandardAI::RANDOM_MOVE) {
        ai_owner->nx = ai_owner->x + vRand(3) - 1;
        ai_owner->ny = ai_owner->y + vRand(3) - 1;
    }

    // we can leave the area only to pursuit enemies, otherwise come back
    if (!companion_sp && !was_attack && !was_item_pick && (ai_flag & XStandardAI::GUARD_AREA)) {
        if (guard_area_location != ai_owner->l->id || !guard_area.PointIn(ai_owner->nx, ai_owner->ny)) {
            MoveTo((guard_area.left + guard_area.right) / 2, (guard_area.top + guard_area.bottom) / 2, Game.Location(guard_area_location).get());
        }
    }

    //Prevents from attacking friends...
    const auto tgt = ai_owner->l->map->GetMonster(ai_owner->nx, ai_owner->ny);

    if (tgt) {
        if (!isEnemy(tgt)) {
            ai_owner->nx = ai_owner->x;
            ai_owner->ny = ai_owner->y;
        } else if (!ai_owner->isCreatureVisible(tgt) && ai_owner != tgt) {
            invisible_x = tgt->x;
            invisible_y = tgt->y;
            invisible_hunting_mode = 1;
        }
    }
}

constexpr int find_path_deep = 200;

int XStandardAI::FindPath(const XPoint* target, XPoint * direction) const
{
    int dist_x = abs(target->x - ai_owner->x);
    int dist_y = abs(target->y - ai_owner->y);

    if (dist_x > find_path_deep || dist_y > find_path_deep) {
        direction->x = 0;
        direction->y = 0;
        return 0;
    }

    int center_x = (target->x + ai_owner->x) / 2;
    int center_y = (target->y + ai_owner->y) / 2;

    int path_flags[find_path_deep + 4][find_path_deep + 4] = {};

    int map_x = center_x - find_path_deep / 2 + 2;
    int map_y = center_y - find_path_deep / 2 + 2;
    XRect map_rect(map_x, map_y, map_x + find_path_deep, map_y + find_path_deep);

    XPoint pa[8 * find_path_deep];
    XPoint pb[8 * find_path_deep];

    path_flags[target->x - map_x][target->y - map_y] = 1;
    pa[0].x = target->x;
    pa[0].y = target->y;
    int stop_flag = 1;
    XPoint *pc = pa;
    int list_len_pc = 1;
    XPoint *pd = pb;

    for (int i = 2; i < find_path_deep + 2 && stop_flag; i++) {
        int list_len_pd = 0;

        for (int j = 0; j < list_len_pc; j++) {
            XPoint *cpt = &pc[j];

            if (map_rect.PointIn(cpt->x - 1, cpt->y - 1) &&
                path_flags[cpt->x - map_x - 1][cpt->y - map_y - 1] == 0) {
                if (cpt->x - 1 == ai_owner->x &&
                    cpt->y - 1 == ai_owner->y) {
                    stop_flag = 0;
                    direction->x = 1;
                    direction->y = 1;
                    break;
                }

                if (ai_owner->l->map->XGetMovability(cpt->x - 1, cpt->y - 1) == 0) {
                    path_flags[cpt->x - map_x - 1][cpt->y - map_y - 1] = i;
                    pd[list_len_pd].x = cpt->x - 1;
                    pd[list_len_pd].y = cpt->y - 1;
                    list_len_pd++;
                }
            }

            if (map_rect.PointIn(cpt->x - 0, cpt->y - 1) &&
                path_flags[cpt->x - map_x - 0][cpt->y - map_y - 1] == 0) {
                if (cpt->x - 0 == ai_owner->x &&
                    cpt->y - 1 == ai_owner->y) {
                    stop_flag = 0;
                    direction->x = 0;
                    direction->y = 1;
                    break;
                }

                if (ai_owner->l->map->XGetMovability(cpt->x - 0, cpt->y - 1) == 0) {
                    path_flags[cpt->x - map_x - 0][cpt->y - map_y - 1] = i;
                    pd[list_len_pd].x = cpt->x - 0;
                    pd[list_len_pd].y = cpt->y - 1;
                    list_len_pd++;
                }
            }

            if (map_rect.PointIn(cpt->x + 1, cpt->y - 1) &&
                path_flags[cpt->x - map_x + 1][cpt->y - map_y - 1] == 0) {
                if (cpt->x + 1 == ai_owner->x &&
                    cpt->y - 1 == ai_owner->y) {
                    stop_flag = 0;
                    direction->x = -1;
                    direction->y = 1;
                    break;
                }

                if (ai_owner->l->map->XGetMovability(cpt->x + 1, cpt->y - 1) == 0) {
                    path_flags[cpt->x - map_x + 1][cpt->y - map_y - 1] = i;
                    pd[list_len_pd].x = cpt->x + 1;
                    pd[list_len_pd].y = cpt->y - 1;
                    list_len_pd++;
                }
            }

            if (map_rect.PointIn(cpt->x + 1, cpt->y + 0) &&
                path_flags[cpt->x - map_x + 1][cpt->y - map_y + 0] == 0) {
                if (cpt->x + 1 == ai_owner->x &&
                    cpt->y + 0 == ai_owner->y) {
                    stop_flag = 0;
                    direction->x = -1;
                    direction->y = 0;
                    break;
                }

                if (ai_owner->l->map->XGetMovability(cpt->x + 1, cpt->y + 0) == 0) {
                    path_flags[cpt->x - map_x + 1][cpt->y - map_y + 0] = i;
                    pd[list_len_pd].x = cpt->x + 1;
                    pd[list_len_pd].y = cpt->y + 0;
                    list_len_pd++;
                }
            }

            if (map_rect.PointIn(cpt->x - 1, cpt->y + 0) &&
                path_flags[cpt->x - map_x - 1][cpt->y - map_y + 0] == 0) {
                if (cpt->x - 1 == ai_owner->x &&
                    cpt->y + 0 == ai_owner->y) {
                    stop_flag = 0;
                    direction->x = +1;
                    direction->y = 0;
                    break;
                }

                if (ai_owner->l->map->XGetMovability(cpt->x - 1, cpt->y + 0) == 0) {
                    path_flags[cpt->x - map_x - 1][cpt->y - map_y + 0] = i;
                    pd[list_len_pd].x = cpt->x - 1;
                    pd[list_len_pd].y = cpt->y + 0;
                    list_len_pd++;
                }

            }

            if (map_rect.PointIn(cpt->x - 1, cpt->y + 1) &&
                path_flags[cpt->x - map_x - 1][cpt->y - map_y + 1] == 0) {
                if (cpt->x - 1 == ai_owner->x &&
                    cpt->y + 1 == ai_owner->y) {
                    stop_flag = 0;
                    direction->x = +1;
                    direction->y = -1;
                    break;
                }

                if (ai_owner->l->map->XGetMovability(cpt->x - 1, cpt->y + 1) == 0) {
                    path_flags[cpt->x - map_x - 1][cpt->y - map_y + 1] = i;
                    pd[list_len_pd].x = cpt->x - 1;
                    pd[list_len_pd].y = cpt->y + 1;
                    list_len_pd++;
                }
            }

            if (map_rect.PointIn(cpt->x + 0, cpt->y + 1) &&
                path_flags[cpt->x - map_x + 0][cpt->y - map_y + 1] == 0) {
                if (cpt->x + 0 == ai_owner->x &&
                    cpt->y + 1 == ai_owner->y) {
                    stop_flag = 0;
                    direction->x = 0;
                    direction->y = -1;
                    break;
                }

                if (ai_owner->l->map->XGetMovability(cpt->x + 0, cpt->y + 1) == 0) {
                    path_flags[cpt->x - map_x + 0][cpt->y - map_y + 1] = i;
                    pd[list_len_pd].x = cpt->x + 0;
                    pd[list_len_pd].y = cpt->y + 1;
                    list_len_pd++;
                }
            }

            if (map_rect.PointIn(cpt->x + 1, cpt->y + 1) &&
                path_flags[cpt->x - map_x + 1][cpt->y - map_y + 1] == 0) {
                if (cpt->x + 1 == ai_owner->x &&
                    cpt->y + 1 == ai_owner->y) {
                    stop_flag = 0;
                    direction->x = -1;
                    direction->y = -1;
                    break;
                }

                if (ai_owner->l->map->XGetMovability(cpt->x + 1, cpt->y + 1) == 0) {
                    path_flags[cpt->x - map_x + 1][cpt->y - map_y + 1] = i;
                    pd[list_len_pd].x = cpt->x + 1;
                    pd[list_len_pd].y = cpt->y + 1;
                    list_len_pd++;
                }
            }
        }

        XPoint* pt = pd;
        pd = pc;
        pc = pt;
        list_len_pc = list_len_pd;
    }

    return 1;
};

void XStandardAI::GetDirection(const XPoint* target, XPoint * direction) const
{
    int dx = sgn(target->x - ai_owner->x);
    int dy = sgn(target->y - ai_owner->y);

    if (ai_owner->x + dx == target->x && ai_owner->y + dy == target->y) {
        direction->x = dx;
        direction->y = dy;
        return;
    }

    FindPath(target, direction);
}

void XStandardAI::GetRandDirection(const XPoint* target, XPoint * direction) const
{
    int dx = sgn(target->x - ai_owner->x);
    int dy = sgn(target->y - ai_owner->y);

    if (ai_owner->x + dx == target->x && ai_owner->y + dy == target->y) {
        direction->x = dx;
        direction->y = dy;
        return;
    }

    if (ai_owner->l->map->XGetMovability(ai_owner->x + dx, ai_owner->y + dy) != 0) {
        int tx = vRand(3) - 1;
        int ty = vRand(3) - 1;
        dx += tx;
        dy += ty;
        direction->x = sgn(dx);
        direction->y = sgn(dy);
    } else {
        direction->x = dx;
        direction->y = dy;
    }
}

void XStandardAI::GetExactDirection(const XPoint* target, XPoint* direction) const
{
    direction->x = sgn(target->x - ai_owner->x);
    direction->y = sgn(target->y - ai_owner->y);
}

bool XStandardAI::isEnemy(XCreature *cr)
{
    if (cr == companion.lock().get()
        || (ai_flag & XStandardAI::GUARD_AREA && cr->groupID() == ai_owner->groupID())) {
        return false;
    }

    if (enemy_class & cr->creature_class && ai_owner->view != cr->view) {
        return true;
    }

    if (ai_flag & XStandardAI::PROTECT_AREA
        && cr->groupID() != ai_owner->groupID()
        && cr->x >= guard_area.left
        && cr->x < guard_area.right
        && cr->y >= guard_area.top
        && cr->y < guard_area.bottom) {
        return true;
    }

    // A PEACEFUL creature still defends itself against anything that is
    // itself not peaceful - PEACEFUL means "won't start a fight with
    // civilized/harmless beings" (see enemy_class == NONE in
    // XLocation::NewCreature()), not "will never fight back against real
    // danger". Keying off the intruder's own disposition instead of its
    // class means this works the same whether the intruder is a bandit
    // invading the village or a monster encountered mid-journey (e.g. the
    // mushroom-cave route) - no fixed "these are the monster classes"
    // list to maintain, and it correctly leaves other non-monster,
    // non-PEACEFUL classes (a hostile human, say) as a threat too. The
    // hero is explicitly exempted so a PEACEFUL creature doesn't
    // pre-emptively attack the player on sight - XStandardAI::
    // ReactToAttacker()/isPersonalEnemy() below already covers the case
    // where the hero attacks first.
    //
    // Skipped entirely while under escort (an active companion set, e.g.
    // Giana following whoever ransomed her): picking a proactive fight
    // with someone merely visible-and-hostile takes priority over
    // MoveTo(companion) in Move()'s branch order, so without this guard
    // an escortee that's still in sight of a non-PEACEFUL creature (their
    // own former captor, say) gets stuck trying to fight instead of
    // following their rescuer out. Reactive self-defense if actually
    // attacked still works while escorted, via isPersonalEnemy() below.
    if (ai_flag & XStandardAI::PEACEFUL
        && !companion.lock()
        && !cr->isHero()
        && cr->groupID() != ai_owner->groupID()
        && !(cr->xai->GetAIFlag() & XStandardAI::PEACEFUL)) {
        return true;
    }

    return isPersonalEnemy(cr);
}

bool XStandardAI::isPersonalEnemy(XCreature *cr)
{
    for (const auto & i : personal_enemy)
        if (i.lock().get() == cr) {
            return true;
        }

    return false;
}

void XStandardAI::SetAIFlag(XStandardAI::Flag aif)
{
    ai_flag = static_cast<XStandardAI::Flag>(ai_flag | aif);
}

void XStandardAI::ResAIFlag(XStandardAI::Flag aif)
{
    ai_flag = static_cast<XStandardAI::Flag>((ai_flag | aif) ^ aif);
}

void XStandardAI::SetEnemyClass(CreatureClass cr_class)
{
    enemy_class = cr_class;
}

int XStandardAI::Wear() const
{

    for (auto item: ai_owner->contain) {
        XBodyPart * xbp = ai_owner->GetBodyPart(item->bp);

        if (!xbp) {
            continue;
        }

        XItem * old_item = ai_owner->GetItem(item->bp);

        if (old_item) {
            if (auto xbp1 = ai_owner->GetBodyPart(item->bp, 1)) {
                auto old_item1 = ai_owner->GetItem(item->bp, 1);

                if (!old_item1 || (old_item1->GetValue() < old_item->GetValue())) {
                    xbp = xbp1;
                    old_item = old_item1;
                }
            }
        }

        int new_item_val = item->GetValue(), old_item_val = 0;

        if (old_item) {
            old_item_val = old_item->GetValue();
        }

        if ((old_item_val >= new_item_val) || !(xbp->GetProperKind() & item->kind)) {
            continue;
        }

        if (old_item) {
            // Worn items stay resident in contain the whole time they're
            // worn (see XBodyPart::Wear()) - nothing to put back.
            xbp->UnWear();
        }

        // item stays in contain - it's simply also worn now.
        xbp->Wear(item.get());

        if (ai_owner->isVisible()) {
            std::string str;

            switch (item->kind) {
                case ItemKind::WEAPON :
                case ItemKind::MISSILEW :
                    str = fmt::format("{} has wielded {}.", ai_owner->name, item->toString());
                    break;

                case ItemKind::MISSILE :
                    str = fmt::format("{} has armed {}.", ai_owner->name, item->toString());
                    break;

                default :
                    str = fmt::format("{} puts on {}.", ai_owner->name, item->toString());
                    break;
            }

            msgwin.Add(str);
        }

        return 1;
    }

    // Sacrifice useless items
    for (const auto item: ai_owner->contain) {
        assert(item->isValid());

        // Worn items stay resident in contain the whole time they're worn
        // (see XBodyPart::Wear()) - without this check, a creature whose
        // only body armor happens to be cheap (e.g. any "HUMAN"-flagged
        // creature's starting dress/clothes) sacrifices its own equipped
        // gear the very first turn nothing better is found to wear.
        if (ai_owner->IsWorn(item.get())) {
            continue;
        }

        if (item->GetValue() > 800) {
            continue;
        }

        if (item->kind & ItemKind::FOOD && item->it != ItemType::CORPSE) {
            continue;
        }

        if (item->kind & (ItemKind::SCROLL | ItemKind::BOOK | ItemKind::POTION | ItemKind::MISSILE | ItemKind::MONEY)) {
            continue;
        }

        ai_owner->contain.erase(item);
        ai_owner->Sacrifice(item.get());
        break;
    }

    return 0;
}

XStairWay* RecursiveWayFound(XLocation * tl, XLocation * tgt_l)
{
    tl->way_found_flag = false;

    for (const auto it: tl->ways_list) {
        const auto way = dynamic_cast<XStairWay*>(it);

        if (way->ln == tgt_l->id) {
            return way;
        }

        if (const auto way_loc = Game.Location(way->ln); way_loc && way_loc->way_found_flag) {
            if (RecursiveWayFound(way_loc.get(), tgt_l))
            {
                // we need to find only the top (i.e. the closest) way
                return way;
            }
        }
    }

    return nullptr;
}

XStairWay* RWayFound(XLocation * tl, XLocation * tgt_l)
{
    for (const auto& [key, location] : Game.locations) {
        if (location) {
            location->way_found_flag = true;
        }
    }

    return RecursiveWayFound(tl, tgt_l);
}

int XStandardAI::MoveTo(int x, int y, XLocation * l) const
{
    // if it is not this location, then try the way to the nearest location
    if (l && l->id != ai_owner->l->id) {
        if (!(ai_flag & XStandardAI::FIND_WAY)) {
            return 0;
        }

        // try to find StairWay to creature...
        XStairWay * way = RWayFound(ai_owner->l, l);

        if (!way) {
            return 0;
        }

        if (ai_owner->x == way->x && ai_owner->y == way->y) {
            ai_owner->MoveStairWay();
            return 1;
        } else {
            XPoint direction_point;
            XPoint target_point(way->x, way->y);
            GetDirection(&target_point, &direction_point);
            ai_owner->nx = ai_owner->x + direction_point.x;
            ai_owner->ny = ai_owner->y + direction_point.y;
            return 1;
        }
    } else {
        // if it is this location than move to...
        XPoint direction_point;
        XPoint target_point(x, y);
        GetDirection(&target_point, &direction_point);
        ai_owner->nx = ai_owner->x + direction_point.x;
        ai_owner->ny = ai_owner->y + direction_point.y;
        return 1;
    }
}

// from enemy
int XStandardAI::TryToRunAway() const
{
    assert(enemy);
    int dx = sgn(ai_owner->x - enemy->x);
    int dy = sgn(ai_owner->y - enemy->y);

    ai_owner->nx = ai_owner->x + dx;
    ai_owner->ny = ai_owner->y + dy;

    if (ai_owner->l->map->XGetMovability(ai_owner->nx, ai_owner->ny) == 0) {
        return 1;
    }

    dx += vRand(3) - 1;
    dy += vRand(3) - 1;
    dx = sgn(dx);
    dy = sgn(dy);
    ai_owner->nx = ai_owner->x + dx;
    ai_owner->ny = ai_owner->y + dy;

    if (ai_owner->l->map->XGetMovability(ai_owner->nx, ai_owner->ny) == 0) {
        return 1;
    }

    dx += vRand(3) - 1;
    dy += vRand(3) - 1;
    dx = sgn(dx);
    dy = sgn(dy);
    ai_owner->nx = ai_owner->x + dx;
    ai_owner->ny = ai_owner->y + dy;

    if (ai_owner->l->map->XGetMovability(ai_owner->nx, ai_owner->ny) == 0) {
        return 1;
    }

    ai_owner->nx = ai_owner->x;
    ai_owner->ny = ai_owner->y;

    if (abs(ai_owner->x - enemy->x) > 1 || abs(ai_owner->y - enemy->y) > 1) {
        return 1;
    }

    return 0;
}

int XStandardAI::AttackEnemy(int ex, int ey) const
{
    // try to run away if we must or can
    if (ai_flag & XStandardAI::COWARD && enemy &&
        (enemy->GetExp() / 10 > ai_owner->GetExp() * friends_count // creature is more powerful
        || ai_owner->GetMaxHP() / ai_owner->HP > 4) // less than 25% of HP
        && TryToRunAway()) {
        return 0;
    }

    XPoint direction_point;
    XPoint target_point(ex, ey);
    GetDirection(&target_point, &direction_point);

    if (CastSpell() || Shoot() || ReadScroll() || DrinkPotion()) {
        ai_owner->nx = ai_owner->x;
        ai_owner->ny = ai_owner->y;
    } else {
        ai_owner->nx = ai_owner->x + direction_point.x;
        ai_owner->ny = ai_owner->y + direction_point.y;
    }

    return 1;

}

int XStandardAI::CastSpell() const {
    if (ai_owner->m->spells.empty()) {
        return 0;
    }

    //	try to heal self
    if (ai_owner->HP < ai_owner->GetMaxHP() / 3) {

        for (const auto& spell: ai_owner->m->spells) {
            if ((spell->GetSpellName() == SPELL_CURE_LIGHT_WOUNDS ||
                spell->GetSpellName() == SPELL_CURE_SERIOUS_WOUNDS ||
                spell->GetSpellName() == SPELL_CURE_CRITICAL_WOUNDS ||
                spell->GetSpellName() == SPELL_CURE_MORTAL_WOUNDS ||
                spell->GetSpellName() == SPELL_HEAL) &&
                spell->GetManaCost() <= ai_owner->PP) {
                ai_owner->m->Cast(spell.get(), ai_owner);

                return 1;
            }
        }
    }

    // try to attack
    if (enemy) {
        int r_enemy = ai_owner->Distance(enemy);

        assert(r_enemy > 0);

        for (const auto& spell: ai_owner->m->spells) {
            if ((spell->GetSpellName() == SPELL_MAGIC_ARROW ||
                spell->GetSpellName() == SPELL_FIRE_BOLT ||
                spell->GetSpellName() == SPELL_ICE_BOLT ||
                spell->GetSpellName() == SPELL_LIGHTNING_BOLT ||
                spell->GetSpellName() == SPELL_ACID_BOLT)
                && spell->GetManaCost() <= ai_owner->PP) {
                ai_owner->m->Cast(spell.get(), ai_owner);

                return 1;
            }
        }
    }

    return 0;
}

int XStandardAI::ReadScroll() const
{
    for (auto item: ai_owner->contain) {
        if (!(item->kind & ItemKind::SCROLL)) {
            continue;
        }

        auto scroll = dynamic_cast<XScroll*>(item.get());

        if (scroll->sc_name == ScrollName::MAGIC_ARROW ||
            scroll->sc_name == ScrollName::FIRE_BOLT ||
            scroll->sc_name == ScrollName::ICE_BOLT ||
            scroll->sc_name == ScrollName::LIGHTNING_BOLT ||
            scroll->sc_name == ScrollName::ACID_BOLT) {
            scroll->onRead(ai_owner);

            if (--scroll->quantity <= 0) {
                ai_owner->contain.erase(item);
                scroll->Invalidate();
            }

            return 1;
        }
    }

    return 0;
}

int XStandardAI::DrinkPotion() const
{
    if (ai_owner->HP < ai_owner->GetMaxHP() / 3) {
        for (const auto it: ai_owner->contain) {
            if (it->kind & ItemKind::POTION) {
                auto pot = dynamic_cast<XPotion *>(it.get());

                if (pot->pn == PotionName::HEALING ||
                    pot->pn == PotionName::CURE_LIGHT_WOUNDS ||
                    pot->pn == PotionName::CURE_SERIOUS_WOUNDS ||
                    pot->pn == PotionName::CURE_CRITICAL_WOUNDS ||
                    pot->pn == PotionName::CURE_MORTAL_WOUNDS) {
                    // FIXME: why the copy?
                    auto np = dynamic_cast<XPotion *>(pot->MakeCopy());
                    np->onDrink(ai_owner);

                    if (pot->quantity > 1) {
                        pot->quantity--;
                    } else {
                        ai_owner->contain.erase(it);
                        pot->Invalidate();
                    }

                    return 1;
                }
            }
        }
    }

    return 0;
}

int XStandardAI::Shoot() const
{
    int hit;
    int range;
    XDice dmg;
    ai_owner->GetRangeAttackInfo(&range, &hit, &dmg);

    if (enemy) {
        if (ai_owner->Distance(enemy) <= range) {
            ai_owner->Shoot(enemy->x, enemy->y);
            return 1;
        }
    }

    return 0;
}

int XStandardAI::PickUpItems() const
{
    XItemList *item_list = ai_owner->l->map->GetItemList(ai_owner->x, ai_owner->y);
    bool item_picked = false;

    for (auto it = item_list->begin(); it != item_list->end(); ) {
        if ((*it)->kind & ItemKind::CHEST) {
            break;
        }

        // Hold a live shared_ptr across the erase - item_list can be this
        // item's only reference, and PickUpItem() failing needs to hand it
        // right back, so it must not be destroyed in between.
        std::shared_ptr<XItem> current_item = *it;
        it = item_list->erase(it);

        if (ai_owner->PickUpItem(current_item.get())) {
            item_picked = true;
        } else {
            it = item_list->insert(it, current_item);
            break;
        }
    }

    if (!item_picked) {
        return 0;
    }

    ai_owner->nx = ai_owner->x;
    ai_owner->ny = ai_owner->y;

    if (ai_owner->isVisible()) {
        msgwin.Add(fmt::format("{} picks something up from the ground.",
            ai_owner->name));
    }

    return 1;
}

void XStandardAI::SetArea(XRect & area, const std::string& ln)
{
    guard_area.Setup(area);
    guard_area_location = ln;
}

void XStandardAI::SetCompanion(XCreature * cr)
{
    companion = XCreature::ToWeakPtr(cr);
}

void XStandardAI::SetOrderedEnemy(XCreature * cr)
{
    ordered_enemy = XCreature::ToWeakPtr(cr);
}

void XStandardAI::onWasAttacked(XCreature * attacker)
{
    ReactToAttacker(attacker);
}

void XStandardAI::ReactToAttacker(XCreature * attacker)
{
    assert(attacker != ai_owner);
    AddPersonalEnemy(attacker);

    if (ai_owner->groupID() != GID_NONE) {
        SetGroupEnemy(attacker);
    }

    // A PEACEFUL creature still remembers the attacker as a personal/group
    // enemy above (so it'll fight back if it runs into them again), but it
    // shouldn't gain long-distance pursuit capability just from having been
    // attacked - that's what last_enemy/invisible_hunting_mode drive in
    // Move(). See the matching skip there.
    if (ai_flag & XStandardAI::PEACEFUL) {
        return;
    }

    if (ai_owner->isCreatureVisible(attacker)) {
        last_enemy = XCreature::ToWeakPtr(attacker);
    }

    invisible_x = attacker->x;
    invisible_y = attacker->y;
    invisible_hunting_mode = 1;
}

void XStandardAI::onDie(XCreature * killer)
{
    if (killer) {
        SetGroupEnemy(killer);
    }
}

void XStandardAI::SetGroupEnemy(XCreature* cr) const
{
    if (ai_owner->groupID() != GID_NONE && cr) {
        for (const auto& buddy : ai_owner->getGroupMembers()) {
            buddy->xai->AddPersonalEnemy(cr);
            buddy->xai->ResAIFlag(XStandardAI::GUARD_AREA);
            buddy->xai->enemy = cr;
        }
    }
}

void XStandardAI::onSteal(XCreature * rogue)
{
    AddPersonalEnemy(rogue);
}

void XStandardAI::AddPersonalEnemy(XCreature * cr)
{
    int i;
    sleep_well = 0;

    auto cr_sp = XCreature::ToWeakPtr(cr).lock();

    for (i = 0; i < ENEMY_LIST_SIZE; i++) {
        if (personal_enemy[i].expired()) {
            personal_enemy[i] = cr_sp;
            return;
        }
    }

    for (i = 1; i < ENEMY_LIST_SIZE; i++) {
        personal_enemy[i - 1] = personal_enemy[i];
    }

    personal_enemy[ENEMY_LIST_SIZE - 1] = cr_sp;
}

void XStandardAI::RemovePersonalEnemy(const XCreature* cr)
{
    for (auto & i : personal_enemy) {
        if (i.lock().get() == cr) {
            i.reset();

            return;
        }
    }
}

int XStandardAI::Chat(XCreature* chatter, const char* msg)
{
    if (!ai_owner->Chat(chatter, msg)) {
        msgwin.Add(ai_owner->StdAnswer());
    }

    return 1;
}

int XStandardAI::onGiveItem(XCreature * giver, XItem * item)
{
    return ai_owner->onGiveItem(giver, item);
}

int XStandardAI::GetTargetPos(XPoint * pt)
{
    if (enemy) {
        pt->x = enemy->x;
        pt->y = enemy->y;
        return 1;

    } else {
        return 0;
    }
}

bool XStandardAI::CanMoveHere(int px, int py)
{
    if (ai_owner->l->map->XGetMovability(px, py) != 0) {
        XCreature * tgt = ai_owner->l->map->GetMonster(px, py);

        if (tgt && isEnemy(tgt)) {
            return true;
        } else {
            return false;
        }
    } else {
        return true;
    }
}

/////////////// scripting support
void XStandardAI::ExecuteScript(const std::vector<SCRIPT_CMD> &scr)
{
    script.clear();

    for (auto it: scr) {
        script.push_back(it);
    }

    SetAIFlag(XStandardAI::EXECUTE_SCRIPT);
    ResAIFlag(XStandardAI::GUARD_AREA);
}

void XStandardAI::RunScript()
{
    SCRIPT_CMD cmd = *script.begin();

    bool flag = false;

    switch (cmd.cmd) {
        case SCC_NONE:
            break;

        case SCC_MOVE_POINT:
            MoveTo(cmd.pt_x, cmd.pt_y, Game.Location(cmd.ln).get());

            if (cmd.pt_x == ai_owner->nx && cmd.pt_y == ai_owner->ny && cmd.ln == ai_owner->l->id) {
                flag = true;
            }

            break;

        case SCC_COLLECT_MUSHROOM: {
            XMapObject* obj = ai_owner->l->map->GetSpecial(ai_owner->x, ai_owner->y);

            // Pick() may destroy obj (self-eviction, kept alive through
            // this call by the deferred-release graveyard).
            auto* tit = (obj && obj->isValid()) ? dynamic_cast<XItem *>(obj->Pick(ai_owner)) : nullptr;

            if (tit) {
                if (ai_owner->PickUpItem(tit)) {
                    if (ai_owner->isVisible()) {
                        msgwin.Add(fmt::format("{} collects {}.",
                            ai_owner->GetNameEx(CRN_T1), tit->toString()));
                    }

                    if (vRand(2) == 0) {
                        flag = true;
                    }
                } else {
                    tit->Invalidate();
                }
            } else {
                ai_owner->nx = ai_owner->x + vRand(3) - 1;
                ai_owner->ny = ai_owner->y + vRand(3) - 1;
            }
        }
        break;

        case SCC_DROP_ITEM: {
            // Pre-existing bug, fixed in passing: erasing from contain
            // while range-for iterating it invalidated the iterator on the
            // very next increment. Use the erase-returns-next-iterator form
            // instead.
            for (auto it = ai_owner->contain.begin(); it != ai_owner->contain.end();) {
                if ((*it)->kind & cmd.kind) {
                    auto item = *it;
                    it = ai_owner->contain.erase(it);
                    ai_owner->DropItem(item.get());
                } else {
                    ++it;
                }
            }

            flag = true;
        }
        break;
    }

    if (flag) {
        script.push_back(cmd);
        script.pop_front();
    }
}

void XStandardAI::LearnTraps()
{
    for (int i = guard_area.left; i < guard_area.right; i++)
        for (int j = guard_area.top; j < guard_area.bottom; j++) {
            XMapObject * pO = ai_owner->l->map->GetSpecial(i, j);

            if (dynamic_cast<XTrap *>(pO)) {
                known_traps.push_back(pO);
            }
        }
}

bool XStandardAI::isKnowThisTrap(const XMapObject* trap)
{
    return std::any_of(
        known_traps.begin(),
        known_traps.end(),
        [trap](const XMapObject* t) { return trap == t; });
}
