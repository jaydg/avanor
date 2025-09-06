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

#include "item.h"

#include "creature.h"


XItem::XItem()
{
    total_it++;

    ttm = 0;
    ttmb = 0;
    durability = 10;

    is_selected = 0;

    view = '*';
    color = xBLUE;
    it = IT_OTHER;
    im = IM_OTHER;
    wt = WSK_OTHER;
    bp = BP_OTHER;
    identify = 1;
    _DV = _PV = _HIT = RNG = 0;
    dice.Setup("0d0");
    special_number = -1;
    brt = BR_NONE;
    owner = NULL;
}

void XItem::Invalidate()
{
    total_it--;
    XBaseObject::Invalidate();
}


int XItem::BasicFill(ITEM_TYPE it, XItemBasicStructure * pData)
{
    _MAIN_ITEM_STRUCT * x_struct = pData->pFirstItem;

    if (it == IT_RANDOM) {
        int r_val = vRand(pData->total_prob);

        while (1) {
            r_val -= x_struct->probability;

            if (r_val < 0) {
                break;
            }

            x_struct++;
        }
    } else {
        for (int i = 0; i < pData->total_item; i++, x_struct++)
            if (x_struct->it == it) {
                break;
            }
    }

    if (x_struct) {
        MainFill(x_struct);
        PropFill(x_struct->iset);

        if (vRand() % 20 > 18) {
            SpecialFill();
        }

        return 1;
    } else {
        return 0;
    }
}

void XItem::MainFill(_MAIN_ITEM_STRUCT *is)
{
    strcpy(name, is->name);
    it = is->it;
    view = is->view;
    weight = is->valume;
    value = is->value;
    quality = is->iq;

    XDice * d;

    d = new XDice(is->dv);
    _DV = d->NThrow();

    d->Setup(is->pv);
    _PV = d->NThrow();

    d->Setup(is->hit);
    _HIT = d->NThrow();

    d->Setup(is->dice);
    int tx = d->X;
    int ty = d->Y;

    d->Setup(is->z);
    dice.Setup(tx, ty, d->NThrow());

    d->Setup(is->r);
    RNG = d->NThrow();
    delete d;
}

void XItem::PropFill(ITEM_SET is, int val)
{
    int tval = 0;

    for (int i = 0; i < DB_PROP_SZ; i++) {
        if (item_prop[i].iflag & is) {
            tval += item_prop[i].probability;
        }
    }

    assert(tval > 0);

    int trnd = vRand() % tval;
    int r_val = 0;

    while (1) {
        if (item_prop[r_val].iflag & is) {
            trnd -= item_prop[r_val].probability;
        }

        if (trnd < 0) {
            break;
        }

        r_val++;
    }

    material_index = r_val;
    char buf[100];
    sprintf(buf, "%s %s", item_prop[r_val].propname, name);
    strcpy(name, buf);

    color = item_prop[r_val].color;
    weight *= item_prop[r_val].density;
    value =	item_prop[r_val].value * value / 10;

    XDice d;

    if (_DV) {
        d.Setup(item_prop[r_val].dv);
        _DV += d.NThrow();
    }

    if (_PV) {
        d.Setup(item_prop[r_val].pv);
        _PV += d.NThrow();
    }

    d.Setup(item_prop[r_val].hit);
    _HIT += d.NThrow();

    d.Setup(item_prop[r_val].dice);
    int tx = dice.X + d.X;
    int ty = dice.Y + d.Y;
    d.Setup(item_prop[r_val].z);
    dice.Setup(tx, ty, dice.Z + d.NThrow());

    quality = (ITEM_QUALITY)(quality + item_prop[r_val].iq);
    assert(r == NULL);
    r = new XResistance(item_prop[r_val].resistance);
    assert(s == NULL);
    s = new XStats();
}

void XItem::SpecialFill()
{
    int r_val;

    if (im & IM_WEAPON) {
        int uu = 0;
    }

    r_val = vRand() % ENH_DB_SZ;

    if (!(ienh_db[r_val].val < vRand() % 101 && (im & ienh_db[r_val].im))) {
        return;
    }

    special_number = r_val;

    if (ienh_db[r_val].color) {
        color = ienh_db[r_val].color;
    }

    XDice * d;

    d = new XDice(ienh_db[r_val].dv);
    _DV += d->Throw();

    d->Setup(ienh_db[r_val].pv);
    _PV += d->Throw();

    d->Setup(ienh_db[r_val].hit);
    _HIT += d->Throw();

    d->Setup(ienh_db[r_val].dice);
    int tx = dice.X + d->X;
    int ty = dice.Y + d->Y;
    d->Setup(ienh_db[r_val].z);
    dice.Setup(tx, ty, dice.Z + d->Throw());
    delete d;

    XResistance xres(ienh_db[r_val].r);
    XStats xst(ienh_db[r_val].s);
    r->Add(&xres);
    s->Add(&xst);

    brt = ienh_db[r_val].brt;
}

XItem::XItem(XItem * copy) : XBaseObject((XBaseObject*)copy)
{
    total_it++;

    bp = copy->bp;
    durability = copy->durability;
    identify = copy->identify;
    is_selected = copy->is_selected;
    it = copy->it;
    material_index = copy->material_index;
    special_number = copy->special_number;
    special_property = copy->special_property;
    value = copy->value;
    wt = copy->wt;
    brt = copy->brt;
    owner = copy->owner.get();
}

int XItem::Compare(XObject * o)
{
    assert(dynamic_cast<XItem*>(o));
    XItem * tit = (XItem*)o;

    if (it == tit->it && tit->brt == brt && XBaseObject::Compare(o) == 0) {
        return 0;
    } else {
        return 1;
    }
}

void XItem::toString(char* buf)
{
    assert(0);
}

int XItem::ModifyDur(int val)
{
    if (durability == DUR_INFINITE) {
        return 0;
    }

    durability -= val;

    if (durability <= 0) {
        return 0;
    } else {
        return 1;
    }
}

int XItem::GetValue()
{
    int xdice = 0;
    int xdvpv = 0;
    int xhitdmg = 0;

    if (im & IM_VALUEDICE) {
        xdice = (dice.X * dice.Y + dice.X) * 3;
    }

    if (im & IM_VALUEDVPV) {
        xdvpv = (_DV + 6 * _PV) * 4;
    }

    if (im & IM_SHIELD) {
        xdvpv = xdvpv + _DV * 5;
    }

    if (im & IM_VALUEHITDMG) {
        xhitdmg = (_HIT + dice.Z * 3) * 3;
    }

    int xrng = RNG * (abs(RNG) + 5);

    int xstats = 0;
    int i;

    if (s)
        for (i = S_STR; i < S_EOF; i++) {
            xstats += s->Get((STATS)i);
        }

    int xresist = 0;

    if (r)
        for (i = R_WHITE; i < R_EOF; i++) {
            int tr = r->GetResistance((RESISTANCE)i);

            if (tr < 10) {
                xresist += tr * 2;
            } else if (tr < 30) {
                xresist += tr * 4;
            } else if (tr < 50) {
                xresist += tr * 8;
            } else if (tr < 75) {
                xresist += tr * 10;
            } else {
                xresist += tr * 20;
            }
        }

    int brtval = 0;

    if (brt & BR_FIRE) {
        brtval += 200;
    }

    if (brt & BR_COLD) {
        brtval += 150;
    }

    if (brt & BR_ORCSLAYER) {
        brtval += 300;
    }

    if (brt & BR_DEMONSLAYER) {
        brtval += 220;
    }

    int xval = brtval + value + xdice + xdvpv + xhitdmg + xstats * 150 + xresist + xrng;

    if (im & IM_MISSILE) {
        xval /= 7;
    }

    if (xval <= 0) {
        return 1;
    } else {
        return xval;
    }
}

void XItem::StatsToString(char* buf)
{
    char tb[256];
    strcpy(buf, "");

    if (s) {
        strcpy(tb, "{");
        int flag = 0;

        for (int i = S_STR; i < S_EOF; i++)
            if (s->Get((STATS)i) != 0) {
                if (flag) {
                    strcat(tb, " ");
                }

                strcat(tb, s->GetName((STATS)i));
                strcat(tb, ":");
                char tmpbuf[20];
                sprintf(tmpbuf, "%d", s->Get((STATS)i));
                strcat(tb, tmpbuf);
                flag++;
            }

        if (flag) {
            strcat(tb, "}");
            strcpy(buf, tb);
        }
    }
}

void XItem::GetFullName(char* buf, const char* templ)
{
    if (special_number >= 0) {
        if (quantity == 1) {
            sprintf(buf, ienh_db[special_number].name, name);
        } else {
            char tbuf[128];
            sprintf(buf, "heap of (%d)", quantity);

            if (im & (IM_BOOTS | IM_GLOVES)) {
                sprintf(tbuf, ienh_db[special_number].name, name);
                strcat(buf, tbuf);
            } else {
                char rbuf[128];
                strcpy(rbuf, name);
                strcat(rbuf, "s");
                sprintf(tbuf, ienh_db[special_number].name, rbuf);
                strcat(buf, tbuf);
            }
        }

        strcat(buf, " ");
    } else {
        if (quantity == 1) {
            sprintf(buf, "%s ", name);
        } else {
            if (im & (IM_BOOTS | IM_GLOVES)) {
                sprintf(buf, "heap of (%d) %s ", quantity, name);
            } else {
                sprintf(buf, "heap of (%d) %ss ", quantity, name);
            }
        }
    }
}

void XItem::GetArtifactName(char* buf, const char* real_name)
{
    if (isIdentifed()) {
        char tbuf[256];
        strcpy(buf, real_name);

        if (RNG != 0) {
            sprintf(tbuf, "<%+d>", RNG);
            strcat(buf, tbuf);
        }

        if (_DV != 0 || _PV != 0) {
            sprintf(tbuf, "[%+d, %+d]", _DV, _PV);
            strcat(buf, tbuf);
        }

        if (im & IM_WEAPON) {
            sprintf(tbuf, "(%+d, %dd%d%+d)", _HIT, dice.X, dice.Y, dice.Z);
            strcat(buf, tbuf);
        }

        StatsToString(tbuf);
        strcat(buf, tbuf);
    } else {
        strcpy(buf, name);
    }
}

void XItem::Store(XFile * f)
{
    XBaseObject::Store(f);

    f->Write(&bp, sizeof(BODYPART));
    f->Write(&brt, sizeof(BRAND_TYPE));
    f->Write(&durability, sizeof(int));
    f->Write(&identify, sizeof(int));

    f->Write(&is_selected, sizeof(int));
    f->Write(&it, sizeof(ITEM_TYPE));
    f->Write(&material_index, sizeof(int));

    f->Write(&special_number, sizeof(int));
    f->Write(&special_property, sizeof(SPECIAL_PROPERTY));

    f->Write(&value, sizeof(int));
    f->Write(&wt, sizeof(WSK_TYPE));
    f->Write(&quality, sizeof(ITEM_QUALITY));
    owner.Store(f);
}


void XItem::Restore(XFile * f)
{
    XBaseObject::Restore(f);

    f->Read(&bp, sizeof(BODYPART));
    f->Read(&brt, sizeof(BRAND_TYPE));
    f->Read(&durability, sizeof(int));
    f->Read(&identify, sizeof(int));

    f->Read(&is_selected, sizeof(int));
    f->Read(&it, sizeof(ITEM_TYPE));
    f->Read(&material_index, sizeof(int));

    f->Read(&special_number, sizeof(int));
    f->Read(&special_property, sizeof(SPECIAL_PROPERTY));

    f->Read(&value, sizeof(int));
    f->Read(&wt, sizeof(WSK_TYPE));
    f->Read(&quality, sizeof(ITEM_QUALITY));

    owner.Restore(f);
}

int XItem::onWear(XCreature * cr)
{
    cr->added_stats.Add(s); //modify stats;
    cr->added_resists.Add(r); //modify resist;

    if (im != IM_SHIELD) {
        cr->added_DV	+= _DV;
    }

    cr->added_PV	+= _PV;

    if (im & IM_TOHIT) {
        cr->added_HIT	+= _HIT;
    }

    if (!(im & (IM_WEAPON | IM_MISSILE | IM_MISSILEW))) {
        cr->added_DMG	+= dice.Z;
    }

    cr->added_RNG	+= RNG;

    cr->added_HP	+= _HP;
    cr->added_PP	+= _PP;

    return 1;
}

int XItem::onUnWear(XCreature * cr)
{
    cr->added_stats.Sub(s); //modify stats;
    cr->added_resists.Sub(r); //modify resist;

    if (im != IM_SHIELD) {
        cr->added_DV	-= _DV;
    }

    cr->added_PV	-= _PV;

    if (im & IM_TOHIT) {
        cr->added_HIT	-= _HIT;
    }

    if (!(im & IM_WEAPON | IM_MISSILE | IM_MISSILEW)) {
        cr->added_DMG	-= dice.Z;
    }

    cr->added_RNG	-= RNG;

    cr->added_HP	-= _HP;
    cr->added_PP	-= _PP;
    return 1;
}

int XItem::onPickUp(XCreature * cr)
{
    SetOwner(cr);
    return 1;
}

int XItem::onPutOn(XCreature * cr)
{
    return 1;
}

int XItem::onHit(XCreature * user, XCreature * target)
{
    if (ienh_db[special_number].brt & BR_FIRE) {
        //	user->MagicAttack(target, dice.Throw(), R_FIRE);
    }

    return 1;
}


_ITEMPROP* XItem::GetMaterial(int index)
{
    return &item_prop[index];
}

void XItem::Drop(XLocation * location, int _x, int _y)
{
    SetOwner(NULL);
    SetLocation(location);
    l->map->PutItem(_x, _y, this);
}

bool XItem::SetOwner(XCreature * new_owner)
{
    owner = new_owner;

    if (owner) {
        SetLocation(NULL);
    }

    return true;
}

void XItem::Pickup(XCreature * picker)
{
    assert(0);
}

void XItem::UnCarry()
{
    if (owner) {
        owner->UnCarryItem(this);
    }
}
