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

#include "creature/std_ai.h"
#include "engine/xapi.h"
#include "game/game.h"
#include "map/map_objects.h"

REGISTER_CLASS(XStandardAI);

XStandardAI::XStandardAI(XCreature * _cr) : guard_area(1, 1, 2, 3)
{
    ai_owner = _cr;
    ai_flag = AIF_NONE; //(AI_FLAG)(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP);

    enemy_class = CR_ALL;
    last_moved_way = NULL;

    companion_command = CC_NONE;
    invisible_hunting_mode = 0;
    invisible_x = -1;
    invisible_y = -1;
    sleep_well = 0;
}

void XStandardAI::Invalidate()
{
    ai_owner = NULL;

    for (int i = 0; i < ENEMY_LIST_SIZE; i++) {
        personal_enemy[i] = NULL;
    }

    XObject::Invalidate();
}

void XStandardAI::AnalyzeGrid(int j, int i, int w)
{
    //test for monsters
    XCreature * tgt = ai_owner->l->map->GetMonster(j, i);

    if (tgt && !ai_owner->isCreatureVisible(tgt)) {
        tgt = NULL;
    }

    if (tgt && w < enemy_dist && w > 0 && isEnemy(tgt)) {
        enemy = tgt;
        enemy_dist = w;
    }

    //test for friends if
    if (ai_flag & AIF_ALLOW_PACK && tgt && !isEnemy(tgt) && w > 0) {
        friends_count++;
        //make summ of all friend coord, then div it on friend count
        //so we got center of the pack
        friend_avg_x += tgt->x;
        friend_avg_y += tgt->y;
    }


    //test for items
    if (ai_flag & AIF_ALLOW_PICK_UP &&
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

    if (spec && spec->im & IM_WAY && (w < way_dist) && spec != last_moved_way &&
        (((spec->view == '>') && (ai_flag & AIF_ALLOW_MOVE_WAY_DOWN)) ||
        ((spec->view == '<') && (ai_flag & AIF_ALLOW_MOVE_WAY_UP)))
    ) {
        if (((XStairWay*)spec)->ln != L_MAIN || ai_flag & AIF_ALLOW_MOVE_OUT) {
            way_dist = w;
            way_x = j;
            way_y = i;
        }
    }
}


void XStandardAI::Move()
{
    // initializing variables
    enemy = NULL;
    enemy_dist = 10000;
    item_dist = 10000;
    item_x = 0;
    item_y = 0;
    way_dist = 10000;
    way_x = 0;
    way_y = 0;

    //if no last enemy to attack only than process grids...
    if (sleep_well <= 0) {
        friends_count = 1;
        friend_avg_x = ai_owner->x;
        friend_avg_y = ai_owner->y;

        // processing all visible grids
        AnalyzeView(ai_owner->GetVisibleRadius());

        //calculate avg coordinats for AIF_ALLOW_PACK
        friend_avg_x = std::lround((float)friend_avg_x / (float)friends_count);
        friend_avg_y = std::lround((float)friend_avg_y / (float)friends_count);

        if (enemy_dist > 100) {
            sleep_well = 3;
        }
    } else {
        sleep_well--;
    }

    // trying to wear some item
    if (ai_flag & AIF_ALLOW_WEAR_ITEM && enemy_dist > 1 && Wear()) {
        ai_owner->nx = ai_owner->x;
        ai_owner->ny = ai_owner->y;
        return;
    }

    int was_attack = 0;
    int was_item_pick = 0;
    int was_stair_way = 0;

    // first of all, execute order of companion to attack
    bool order_executing = false;

    if (companion && companion_command == CC_ATTACK) {
        if (ordered_enemy && ordered_enemy->isValid()) {
            enemy = ordered_enemy;
        } else {
            ordered_enemy = NULL;
            companion_command = CC_NONE;
        }
    }

    assert(ai_owner->isValid());
    assert(enemy != ai_owner.get());

    if (enemy) {
        //second try to attack enemy
        was_attack = AttackEnemy(enemy->x, enemy->y);

        if (was_attack) {
            last_enemy = enemy;
        }
    } else if (invisible_hunting_mode > 0) {
        was_attack = AttackEnemy(invisible_x, invisible_y);

        if (!was_attack || (ai_owner->x == ai_owner->nx && ai_owner->y == ai_owner->ny)) {
            invisible_x = -1;
            invisible_y = -1;
            invisible_hunting_mode = 0;
        }
    } else if (companion && (companion_command == CC_FOLLOW || companion_command == CC_NONE)
        && MoveTo(companion->x, companion->y, companion->l)) {
        //do nothing....
    } else if (last_enemy) {
        if (!MoveTo(last_enemy->x, last_enemy->y, last_enemy->l)) {
            last_enemy = NULL;
        }
    } else if (ai_flag & AIF_EXECUTE_SCRIPT) {
        //execute script when nothing to do
        RunScript();
    } else if (ai_flag & AIF_ALLOW_PICK_UP && !(ai_owner->l->map->GetItemList(ai_owner->x, ai_owner->y))->empty() && !ai_owner->l->map->GetPlace(ai_owner->x, ai_owner->y)) {
        if (PickUpItems()) {
            return;
        }
    } else if (ai_flag & AIF_ALLOW_PICK_UP && item_dist < 10000) {
        MoveTo(item_x, item_y);
        was_item_pick = 1;
    } else if (ai_flag & (AIF_ALLOW_MOVE_WAY_DOWN | AIF_ALLOW_MOVE_WAY_UP) && way_dist < 10000 && !(ai_flag && AIF_GUARD_AREA)) {

        XMapObject * spec = ai_owner->l->map->GetSpecial(ai_owner->x, ai_owner->y);

        if (spec && spec->im & IM_WAY &&
            (((spec->view == '>') && (ai_flag & AIF_ALLOW_MOVE_WAY_DOWN)) ||
            ((spec->view == '<') && (ai_flag & AIF_ALLOW_MOVE_WAY_UP)))) {
            ai_owner->MoveStairWay();
            last_moved_way = ai_owner->l->map->GetSpecial(ai_owner->x, ai_owner->y);
        } else {
            MoveTo(way_x, way_y);
        }

        was_stair_way = 1;
    } else if (ai_flag & AIF_ALLOW_PACK) {
        //Allow to create packs....
        XPoint direction_point;
        XPoint target_point(friend_avg_x, friend_avg_y);
        GetRandDirection(&target_point, &direction_point);
        ai_owner->nx = ai_owner->x + direction_point.x;
        ai_owner->ny = ai_owner->y + direction_point.y;

        if (ai_owner->l->map->XGetMovability(ai_owner->nx, ai_owner->ny) != 0) {
            ai_owner->nx = ai_owner->x + vRand(3) - 1;
            ai_owner->ny = ai_owner->y + vRand(3) - 1;
        }
    } else if (ai_flag & AIF_RANDOM_MOVE) {
        ai_owner->nx = ai_owner->x + vRand(3) - 1;
        ai_owner->ny = ai_owner->y + vRand(3) - 1;
    }

    //we can leave area only to pursuit enemy, other wise - comeback
    if (!companion && !was_attack && !was_item_pick && (ai_flag & AIF_GUARD_AREA)) {
        if (guard_area_location != ai_owner->l->ln || !guard_area.PointIn(ai_owner->nx, ai_owner->ny)) {
            MoveTo((guard_area.left + guard_area.right) / 2, (guard_area.top + guard_area.bottom) / 2, Game.locations[guard_area_location]);
        }
    }

    //Prevents from attacking friends...
    XCreature * tgt = ai_owner->l->map->GetMonster(ai_owner->nx, ai_owner->ny);

    if (tgt) {
        if (!isEnemy(tgt)) {
            ai_owner->nx = ai_owner->x;
            ai_owner->ny = ai_owner->y;
        } else if (!ai_owner->isCreatureVisible(tgt) && ai_owner.get() != tgt) {
            invisible_x = tgt->x;
            invisible_y = tgt->y;
            invisible_hunting_mode = 1;
        }
    }
}

const int find_path_deep = 200;

int XStandardAI::FindPath(XPoint * target, XPoint * direction)
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

    int path_flags[find_path_deep + 4][find_path_deep + 4];
    memset(path_flags, 0, (find_path_deep + 4) * (find_path_deep + 4) * sizeof(int));

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

void XStandardAI::GetDirection(XPoint * target, XPoint * direction)
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


void XStandardAI::GetRandDirection(XPoint * target, XPoint * direction)
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

void XStandardAI::GetExactDirection(XPoint * target, XPoint * direction)
{
    direction->x = sgn(target->x - ai_owner->x);
    direction->y = sgn(target->y - ai_owner->y);
}

int XStandardAI::isEnemy(XCreature * cr)
{
    if (cr == companion || (ai_flag & AIF_GUARD_AREA && cr->group_id == ai_owner->group_id)) {
        return 0;
    }

    if (enemy_class & cr->creature_class && ai_owner->view != cr->view) {
        return 1;
    }

    if (ai_flag & AIF_PROTECT_AREA && cr->group_id != ai_owner->group_id && cr->x >= guard_area.left && cr->x < guard_area.right && cr->y >= guard_area.top && cr->y < guard_area.bottom) {
        return 1;
    }

    return isPersonalEnemy(cr);
}

int XStandardAI::isPersonalEnemy(XCreature * cr)
{
    for (int i = 0; i < ENEMY_LIST_SIZE; i++)
        if (personal_enemy[i] == cr) {
            return 1;
        }

    return 0;
}

void XStandardAI::SetAIFlag(AI_FLAG aif)
{
    ai_flag = (AI_FLAG)(ai_flag | aif);
}

void XStandardAI::ResAIFlag(AI_FLAG aif)
{
    ai_flag = (AI_FLAG)((ai_flag | aif) ^ aif);
}

void XStandardAI::SetEnemyClass(CREATURE_CLASS cr_class)
{
    enemy_class = cr_class;
}

int XStandardAI::Wear()
{
    it_iterator it;

    for (it = ai_owner->contain.begin(); it != ai_owner->contain.end(); it++) {
        XItem * item = static_cast<XItem*>(static_cast<XObject*>(it));

        XBodyPart * xbp = ai_owner->GetBodyPart(item->bp);

        if (!xbp) {
            continue;
        }

        XItem * old_item = ai_owner->GetItem(item->bp);

        if (old_item) {
            XBodyPart * xbp1 = ai_owner->GetBodyPart(item->bp, 1);

            if (xbp1) {
                XItem * old_item1 = ai_owner->GetItem(item->bp, 1);

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

        if ((old_item_val >= new_item_val) || !(xbp->GetProperIM() & item->im)) {
            continue;
        }

        if (old_item) {
            ai_owner->contain.Add(xbp->UnWear());
        }

        xbp->Wear(item);
        ai_owner->contain.Remove(it);

        if (ai_owner->isVisible()) {
            char xbuf[256];
            char tbuf[256];
            item->toString(tbuf);

            switch (item->im) {
                case IM_WEAPON :
                case IM_MISSILEW :
                    sprintf(xbuf, "%s has wielded %s.", ai_owner->name, tbuf);
                    break;

                case IM_MISSILE :
                    sprintf(xbuf, "%s has armed %s.", ai_owner->name, tbuf);
                    break;

                default :
                    sprintf(xbuf, "%s puts on %s.", ai_owner->name, tbuf);
                    break;
            }

            msgwin.Add(xbuf);
        }

        return 1;
    }

    // Sacrifice useless items
    int s_flag = 0;

    it = ai_owner->contain.begin();

    while (it != ai_owner->contain.end()) {
        XItem * item = it;
        it++;
        assert(item->isValid());

        if (item->GetValue() > 800) {
            continue;
        }

        if (item->im & IM_FOOD && item->it != IT_CORPSE) {
            continue;
        }

        if (item->im & (IM_SCROLL | IM_BOOK | IM_POTION | IM_MISSILE | IM_MONEY)) {
            continue;
        }

        it--;
        it = ai_owner->contain.erase(it);
        ai_owner->Sacrifice(item);
        s_flag = 1;
        break;
    }

    return 0;
}

XStairWay* RecursiveWayFound(XLocation * tl, XLocation * tgt_l)
{
    tl->way_found_flag = false;

    for (XQList<XObject* >::iterator it = tl->ways_list.begin(); it != tl->ways_list.end(); it++) {
        XStairWay * way = (XStairWay*)(*it);

        if (way->ln == tgt_l->ln) {
            return way;
        }

        if (Game.locations[way->ln] && Game.locations[way->ln]->way_found_flag) {
            XStairWay * tway = RecursiveWayFound(Game.locations[way->ln], tgt_l);

            if (tway) {
                // we need to find only top(closest) WAY at this time
                return way;
            }
        }
    }

    return NULL;
}

XStairWay* RWayFound(XLocation * tl, XLocation * tgt_l)
{
    for (int i = 0; i < L_EOF; i++) {
        if (Game.locations[i]) {
            Game.locations[i]->way_found_flag = true;
        }
    }

    return RecursiveWayFound(tl, tgt_l);
}


int XStandardAI::MoveTo(int x, int y, XLocation * l)
{
    // if it is not this location, than try to way to nearest location
    if (l && l->ln != ai_owner->l->ln) {
        if (!(ai_flag & AIF_FIND_WAY)) {
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

int XStandardAI::TryToRunAway() // from enemy
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

int XStandardAI::AttackEnemy(int ex, int ey)
{
    assert(isValid());

    //try to run away if we must or can
    if (ai_flag & AIF_COWARD && enemy &&
        (enemy->GetExp() / 10 > ai_owner->GetExp() * friends_count // creature is more powerfull
        || ai_owner->GetMaxHP() / ai_owner->_HP > 4) // less than 25% of _HP
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

    assert(isValid());
    return 1;

}

int XStandardAI::CastSpell()
{
    if (ai_owner->m->spells.empty()) {
        return 0;
    }

    int flag = 0;

    //	try to heal self
    if (ai_owner->_HP < ai_owner->GetMaxHP() / 3) {
        XList<XSpell*>::iterator spell = ai_owner->m->spells.begin();

        for (; spell != ai_owner->m->spells.end(); spell++) {
            if ((spell->spell_name == SPELL_CURE_LIGHT_WOUNDS ||
                spell->spell_name == SPELL_CURE_SERIOUS_WOUNDS ||
                spell->spell_name == SPELL_CURE_CRITICAL_WOUNDS ||
                spell->spell_name == SPELL_CURE_MORTAL_WOUNDS ||
                spell->spell_name == SPELL_HEAL) &&
                spell->GetManaCost() <= ai_owner->_PP) {
                ai_owner->m->Cast(spell, ai_owner);
                flag = 1;
                break;
            }
        }
    }

    // try to attack
    if (flag == 0 && enemy) {
        int r_enemy = (int)sqrt((float)(enemy->x - ai_owner->x) * (enemy->x - ai_owner->x) +
            (enemy->y - ai_owner->y) * (enemy->y - ai_owner->y));

        assert(r_enemy > 0);
        XList<XSpell*>::iterator spell = ai_owner->m->spells.begin();

        for (; spell != ai_owner->m->spells.end(); spell++) {
            if ((spell->spell_name == SPELL_MAGIC_ARROW ||
                spell->spell_name == SPELL_FIRE_BOLT ||
                spell->spell_name == SPELL_ICE_BOLT ||
                spell->spell_name == SPELL_LIGHTNING_BOLT ||
                spell->spell_name == SPELL_ACID_BOLT)
                && spell->GetManaCost() <= ai_owner->_PP) {
                ai_owner->m->Cast(spell, ai_owner);
                flag = 1;
                break;
            }
        }
    }

    return flag;
}

int XStandardAI::ReadScroll()
{
    for (it_iterator it = ai_owner->contain.begin(); it != ai_owner->contain.end(); it++) {
        if (!(it->im & IM_SCROLL)) {
            continue;
        }

        XScroll * scroll = static_cast<XScroll*>(static_cast<XObject*>(it));

        if (scroll->sc_name == SCROLL_MAGIC_ARROW ||
            scroll->sc_name == SCROLL_FIRE_BOLT ||
            scroll->sc_name == SCROLL_ICE_BOLT ||
            scroll->sc_name == SCROLL_LIGHTNING_BOLT ||
            scroll->sc_name == SCROLL_ACID_BOLT) {
            scroll->onRead(ai_owner);

            if (--scroll->quantity <= 0) {
                scroll->Invalidate();
            }

            return 1;
        }
    }

    return 0;
}

int XStandardAI::DrinkPotion()
{
    if (ai_owner->_HP < ai_owner->GetMaxHP() / 3) {
        for (it_iterator i = ai_owner->contain.begin(); i != ai_owner->contain.end(); i++) {
            XItem * it = i;

            if (it->im & IM_POTION) {
                XPotion * pot = (XPotion*)it;

                if (pot->pn == PN_HEALING ||
                    pot->pn == PN_CURE_LIGHT_WOUNDS ||
                    pot->pn == PN_CURE_SERIOUS_WOUNDS ||
                    pot->pn == PN_CURE_CRITICAL_WOUNDS ||
                    pot->pn == PN_CURE_MORTAL_WOUNDS) {
                    XPotion * np = (XPotion*)pot->MakeCopy();
                    np->onDrink(ai_owner);

                    if (pot->quantity > 1) {
                        pot->quantity--;
                    } else {
                        XObject * obj = ai_owner->contain.Remove(i);
                        assert(obj->guid() == pot->guid());
                        pot->Invalidate();
                    }

                    return 1;
                }
            }
        }
    }

    return 0;
}

int XStandardAI::Shoot()
{
    int hit;
    int range;
    XDice dmg;
    ai_owner->GetRangeAttackInfo(&range, &hit, &dmg);

    if (enemy) {
        int r = (int)sqrt((float)(enemy->x - ai_owner->x) * (enemy->x - ai_owner->x) +
            (enemy->y - ai_owner->y) * (enemy->y - ai_owner->y));

        if (r <= range) {
            ai_owner->Shoot(enemy->x, enemy->y);
            return 1;
        }
    }

    return 0;
}


int XStandardAI::PickUpItems()
{
    XItemList * item_list = ai_owner->l->map->GetItemList(ai_owner->x, ai_owner->y);
    bool item_picked = false;

    it_iterator it = item_list->begin();

    while (it != item_list->end()) {
        if (it->im & IM_CHEST) {
            break;
        }

        XItem * tit = it;
        it = item_list->erase(it);

        if (ai_owner->PickUpItem(tit)) {
            item_picked = true;
            continue;
        } else {
            item_list->Add(tit);
            break;
        }
    }

    if (!item_picked) {
        return 0;
    }

    ai_owner->nx = ai_owner->x;
    ai_owner->ny = ai_owner->y;

    if (ai_owner->isVisible()) {
        msgwin.Add(ai_owner->name);
        msgwin.Add("picks something up from the ground.");
    }

    return 1;
}

void XStandardAI::SetArea(XRect & area, LOCATION ln)
{
    guard_area.Setup(area);
    guard_area_location = ln;
}

void XStandardAI::onWasAttacked(XCreature * attacker)
{
    assert(attacker != ai_owner.get());
    AddPersonalEnemy(attacker);

    if (ai_owner->group_id != GID_NONE) {
        SetGroupEnemy(attacker);
    }

    if (ai_owner->isCreatureVisible(attacker)) {
        last_enemy = attacker;
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

void XStandardAI::SetGroupEnemy(XCreature * enemy)
{
    if (ai_owner->group_id != GID_NONE && enemy) {
        for (const auto& [key, obj] : objects) {
            if (obj->im & IM_CREATURE && ((XCreature*)obj)->group_id == ai_owner->group_id) {
                ((XCreature*)obj)->xai->AddPersonalEnemy(enemy);
                //((XCreature *)obj)->xai->SetLastEnemy(enemy->x, enemy->y);
                ((XCreature*)obj)->xai->ResAIFlag(AIF_GUARD_AREA);
                ((XCreature*)obj)->xai->enemy = (XCreature*)obj;
            }
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

    for (i = 0; i < ENEMY_LIST_SIZE; i++) {
        if (!personal_enemy[i]) {
            personal_enemy[i] = cr;
            return;
        }
    }

    for (i = 1; i < ENEMY_LIST_SIZE; i++) {
        personal_enemy[i - 1] = personal_enemy[i].get();
    }

    personal_enemy[ENEMY_LIST_SIZE - 1] = cr;
}

void XStandardAI::RemovePersonalEnemy(XCreature * cr)
{
    for (int i = 0; i < ENEMY_LIST_SIZE; i++) {
        if (personal_enemy[i].get() == cr) {
            personal_enemy[i] = NULL;
            return;
        }
    }
}

int XStandardAI::Chat(XCreature * chatter, const char* msg)
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


void XStandardAI::Store(XFile * f)
{
    XObject::Store(f);
    f->Write(&ai_flag, sizeof(AI_FLAG));
    f->Write(&enemy_class, sizeof(CREATURE_CLASS));
    f->Write(&invisible_x, sizeof(int));
    f->Write(&invisible_y, sizeof(int));
    f->Write(&invisible_hunting_mode, sizeof(int));
    XObject::StorePointer(f, last_moved_way);

    last_enemy.Store(f);
    companion.Store(f);
    ordered_enemy.Store(f);
    f->Write(&companion_command, sizeof(COMPANION_COMMAND));

    for (int i = 0; i < ENEMY_LIST_SIZE; i++) {
        personal_enemy[i].Store(f);
    }

    ai_owner.Store(f);
    guard_area.Store(f);
    f->Write(&guard_area_location, sizeof(LOCATION));
    known_traps.StoreList(f);
}

void XStandardAI::Restore(XFile * f)
{
    XObject::Restore(f);
    f->Read(&ai_flag, sizeof(AI_FLAG));
    f->Read(&enemy_class, sizeof(CREATURE_CLASS));
    f->Read(&invisible_x, sizeof(int));
    f->Read(&invisible_y, sizeof(int));
    f->Read(&invisible_hunting_mode, sizeof(int));
    last_moved_way = (XMapObject*)RestorePointer(f, this);
    last_enemy.Restore(f);
    companion.Restore(f);
    ordered_enemy.Restore(f);
    f->Read(&companion_command, sizeof(COMPANION_COMMAND));

    for (int i = 0; i < ENEMY_LIST_SIZE; i++) {
        personal_enemy[i].Restore(f);
    }

    ai_owner.Restore(f);
    guard_area.Restore(f);
    f->Read(&guard_area_location, sizeof(LOCATION));
    known_traps.RestoreList(f);
}

/////////////// scripting support
void XStandardAI::ExecuteScript(XQList<SCRIPT_CMD>* scr)
{
    script.clear();

    for (XQList<SCRIPT_CMD>::iterator it = scr->begin(); it != scr->end(); it++) {
        script.push_back(*it);
    }

    SetAIFlag(AIF_EXECUTE_SCRIPT);
    ResAIFlag(AIF_GUARD_AREA);
}

void XStandardAI::RunScript()
{
    SCRIPT_CMD cmd = *script.begin();

    bool flag = false;

    switch (cmd.cmd) {
        case SCC_NONE:
            break;

        case SCC_MOVE_POINT:
            MoveTo(cmd.pt_x, cmd.pt_y, Game.locations[cmd.ln]);

            if (cmd.pt_x == ai_owner->nx && cmd.pt_y == ai_owner->ny && cmd.ln == ai_owner->l->ln) {
                flag = true;
            }

            break;

        case SCC_COLLECT_MUSHROOM: {
            XMapObject * obj = ai_owner->l->map->GetSpecial(ai_owner->x, ai_owner->y);

            if (obj && obj->isValid() && obj->im == IM_OTHER) {
                XItem * tit = (XItem*)(obj->Pick(ai_owner));
                char buf[256];
                char buf2[256];
                tit->toString(buf);

                if (ai_owner->PickUpItem(tit)) {
                    if (ai_owner->isVisible()) {
                        sprintf(buf2, "%s collects %s.", ai_owner->GetNameEx(CRN_T1), buf);
                        msgwin.Add(buf2);
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
            XItemList::iterator it = ai_owner->contain.begin();

            while (it != ai_owner->contain.end()) {
                if (it->im & cmd.im) {
                    XItem * item = it;
                    it = ai_owner->contain.erase(it);
                    ai_owner->DropItem(item);
                } else {
                    it++;
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

            if (pO && pO->im & IM_TRAP) {
                known_traps.push_back(pO);
            }
        }
}

bool XStandardAI::isKnowThisTrap(XMapObject * trap)
{
    for (XList<XMapObject*>::iterator it = known_traps.begin(); it != known_traps.end(); it++) {
        if (it == trap) {
            return true;
        }
    }

    return false;
}
