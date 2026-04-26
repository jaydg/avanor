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

#include "creature/anycr.h"
#include "creature/unique.h"
#include "engine/xapi.h"
#include "game/quest.h"
#include "item/item_misc.h"
#include "item/itemf.h"

REGISTER_CLASS(XAnyCreature);

_CREATURE XCreatureStorage::creature_storage[CN_EOF];
CREATURE_SET_REC XCreatureStorage::creature_set[32];

XAnyCreature::XAnyCreature(_CREATURE * cr)
{
    view = cr->view;
    color = cr->color;
    strcpy(name, cr->name.c_str());

    creature_class = cr->cr_class;

    dice.Setup(cr->dice);

    s = cr->stats_gen.Generate();
    r = cr->r_gen.Generate();

    max_stats.Set(s);

    XBodyPart::Create(this, cr->body.c_str());

    ttmb = cr->move_energy.Throw();
    ttm	= ttmb;
    weight = cr->creature_weight.Throw();
    attack_energy = cr->attack_energy.Throw();
    move_energy = cr->move_energy.Throw();
    base_speed = cr->speed.Throw();

    creature_size	= cr->creature_size;

    _DV	= cr->dv.Throw();
    _PV	= cr->pv.Throw();
    _HIT	= cr->hit.Throw();
    MAX_HP	= cr->hp.Throw();
    _HP	= GetMaxHP();

    MAX_PP	= cr->pp.Throw();
    _PP	= MAX_PP;

    base_exp = GetCreatureStrength();
    creature_person_type = cr->person;
    creature_description = cr->creature_description.c_str();

    melee_attack = &cr->melee_attack;

    // Setup AI
    xai->SetAIFlag(static_cast<AI_FLAG>(cr->ai_flags));

    // EQUIP CREATURE
    for (auto [mask, count, probability, it] : cr->equipment) {
        for (int i = 0; i < count; i++)
            if (vRand(100) < probability) {
                XItem * item = nullptr;

                if (it == IT_UNKNOWN) {
                    item = ICREATEA((ITEM_MASK)(mask));
                } else {
                    item = ICREATEB((ITEM_MASK)(mask), it, 0, 10000000);
                }

                if (item->im & IM_BODY && item->it == IT_DRESS
                    && GetGender() == GEN_MALE) {
                    // This is a kludge to prevent a "Roderick in a dress" scenario.
                    item->it = IT_CLOTHES;
                    strcpy(item->name, "clothes");
                }

                if (CanWear(item)) {
                    // Create proper ammo for missile weapons
                    if (item->im & IM_MISSILEW) {
                        XItem * missile = ICREATEB(IM_MISSILE, IT_ARROW, 0, 10000000);
                        ContainItem(missile);
                    }

                    Wear(item);
                } else {
                    ContainItem(item);
                }
            }
    }

    // wear random items if it wasn't worn before.
    XBodyPart * hand_1 = nullptr;
    XBodyPart * hand_2 = nullptr;

    for (auto bp: components) {
        if (!bp->Item() && vRand(100) < cr->equip_probability) {
            if (bp->bp_uin == BP_HAND) {
                if (hand_1 == nullptr) {
                    hand_1 = bp;
                } else {
                    hand_2 = bp;
                }
            } else if (bp->bp_uin == BP_MISSILE) {
                continue;
            } else {
                bp->Wear(ICREATE(bp->GetProperIM(), 0, 1000000));
            }
        }
    }

    if (hand_1) {
        XItem * weapon = ICREATE(IM_WEAPON, 0, 10000000);
        wsk->SetLevel(weapon->wt, 2); // just basic weapon level
        hand_1->Wear(weapon);
    }

    if (hand_2) {
        hand_2->Wear(ICREATE(IM_SHIELD, 0, 10000000));
    }

    // supress invisibility, add see invisible
    XBodyPart * neck = GetBodyPart(BP_NECK);
    XBodyPart * ring1 = GetBodyPart(BP_RING, 0);
    XBodyPart * ring2 = GetBodyPart(BP_RING, 1);

    if (cr->generation_flags & GFS_SUPRESS_INVIS) {
        if (neck && neck->Item() && neck->Item()->r->GetResistance(R_INVISIBLE) > 0) {
            neck->UnWear()->Invalidate();
        }

        if (ring1 && ring1->Item() && ring1->Item()->r->GetResistance(R_INVISIBLE) > 0) {
            ring1->UnWear()->Invalidate();
        }

        if (ring2 && ring2->Item() && ring2->Item()->r->GetResistance(R_INVISIBLE) > 0) {
            ring2->UnWear()->Invalidate();
        }
    }

    if (cr->generation_flags & GFS_SEE_INVIS) {
        while (true) {
            if (neck && neck->Item() && neck->Item()->r->GetResistance(R_SEEINVISIBLE)) {
                break;
            }

            if (ring1 && ring1->Item() && ring1->Item()->r->GetResistance(R_SEEINVISIBLE)) {
                break;
            }

            if (ring2 && ring2->Item() && ring2->Item()->r->GetResistance(R_SEEINVISIBLE)) {
                break;
            }

            if (ring1 && ring1->Item()) {
                ring1->UnWear()->Invalidate();
                ring1->Wear(new XRing(ENH_SEEINVISIBLE));
            }

            break;
        }
    }

    // Create money if components more than 2
    if (components.size() > 2 && vRand(3) == 0) {
        for (int i = 0; i < vGetHighBitNum(cr->crl) + 1; i++) {
            XItem * it = ICREATEA(IM_MONEY);
            ContainItem(it);
        }
    }

    // Learn skills
    for (auto [skt, level] : cr->skills) {
        sk->Learn(skt, level);
    }

    // Learn spells
    for (auto spell : cr->spells) {
        m->Learn(spell);
    }
}

void XAnyCreature::Die(XCreature * killer)
{
    if (creature_class == CR_UNDEAD) {
        if (creature_name == CN_SKELETON && vRand() % 12 == 0) {
            XItem * it = new XBone();
            it->Drop(l.get(), x, y);
        }
    }

    if (creature_class == CR_ORC) {
        if (killer && killer->im & IM_HERO) {
            XQuest::quest.orcs_killed++;
        }

        XQuest::quest.total_orcs_killed++;
    }

    if (creature_class == CR_RAT && vRand(40) == 0) {
        if (creature_name == CN_RAT || creature_name == CN_LARGE_RAT) {
            XItem * it = new XRatTail();
            DropItem(it);
        } else if (creature_name == CN_BAT || creature_name == CN_HUGE_BAT) {
            XItem * it = new XBatWing();
            DropItem(it);
        }
    }

    if (vRand(5) == 0 && !(creature_class & CR_UNDEAD)) {
        DropItem(new XCorpse(this, &super_info->pCorpseData));
    }

    XCreature::Die(killer);
}

CREATURE_NAME XCreatureStorage::last_name = CN_NONE;

void XCreatureStorage::View(const CREATURE_NAME cn, const char* name, const char view, const int color, const CR_PERSON_TYPE person, const CREATURE_LEVEL crl, const CREATURE_CLASS crcl, const CREATURE_NAME cn_instance)
{
    last_name = cn;
    assert(creature_storage[last_name].name.empty());

    if (cn_instance != CN_NONE) {
        const _CREATURE * cr = GetCreatureData(cn_instance);
        creature_storage[last_name] = *cr;
    }

    creature_storage[last_name].name = name;
    creature_storage[last_name].view = view;
    creature_storage[last_name].color = color;
    creature_storage[last_name].person = person;
    creature_storage[last_name].crl = crl;
    creature_storage[last_name].cr_class = crcl;
}

void XCreatureStorage::Basic(const char* speed, const char* base_energy, const char* combat_energy, CREATURE_SIZE csize, const char* weight)
{
    creature_storage[last_name].speed = XDice(speed);
    creature_storage[last_name].move_energy = XDice(base_energy);
    creature_storage[last_name].attack_energy = XDice(combat_energy);
    creature_storage[last_name].creature_weight = XDice(weight);
}

void XCreatureStorage::Body(const char* body, const int prob, const unsigned int gen_flags)
{
    creature_storage[last_name].body = body;
    creature_storage[last_name].equip_probability = prob;
    creature_storage[last_name].generation_flags = gen_flags;
}

void XCreatureStorage::SetAI(const unsigned int aif)
{
    creature_storage[last_name].ai_flags = aif;
}

void XCreatureStorage::S(const char* stats)
{
    creature_storage[last_name].stats_gen.Init(stats);
}

void XCreatureStorage::R(const char* resists)
{
    creature_storage[last_name].r_gen.Init(resists);
}

void XCreatureStorage::Combat(const char* hit, const char* dice)
{
    creature_storage[last_name].hit = XDice(hit);
    creature_storage[last_name].dice = XDice(dice);
}

void XCreatureStorage::Melee(const BRAND_TYPE br, const int prob)
{
    MELEE_ATTACK ma{};
    ma.e_attack = EA_NONE;
    ma.br_attack = br;
    ma.prob = prob;
    creature_storage[last_name].melee_attack.push_back(ma);
}

void XCreatureStorage::Melee(const EXTENDED_ATTACK ea, const int prob)
{
    MELEE_ATTACK ma{};
    ma.e_attack = ea;
    ma.br_attack = BR_NONE;
    ma.prob = prob;
    creature_storage[last_name].melee_attack.push_back(ma);
}

void XCreatureStorage::Main(const char* dv, const char* pv, const char* hp, const char* pp)
{
    creature_storage[last_name].dv = XDice(dv);
    creature_storage[last_name].pv = XDice(pv);
    creature_storage[last_name].hp = XDice(hp);
    creature_storage[last_name].pp = XDice(pp);
}

void XCreatureStorage::D(const char* descr)
{
    creature_storage[last_name].creature_description = descr;
}

void XCreatureStorage::Learn(const XSkill::Skill skt, const int lvl)
{
    SKILL_REC scr{};
    scr.level = lvl;
    scr.skt = skt;
    creature_storage[last_name].skills.push_back(scr);
}

void XCreatureStorage::Learn(const SPELL_NAME spn)
{
    creature_storage[last_name].spells.push_back(spn);
}

void XCreatureStorage::Equip(const unsigned int mask, const int count, const int prob)
{
    EQUIP_REC er{};
    er.mask = mask;
    er.count = count;
    er.probability = prob;
    er.it = IT_UNKNOWN;
    creature_storage[last_name].equipment.push_back(er);
}

void XCreatureStorage::Equip(const unsigned int mask, const ITEM_TYPE it, const int prob)
{
    EQUIP_REC er{};
    er.mask = mask;
    er.count = 1;
    er.probability = prob;
    er.it = it;
    creature_storage[last_name].equipment.push_back(er);
}

void XCreatureStorage::Corpse(const int rotting_time, const FOOD_TYPE ft)
{
    creature_storage[last_name].pCorpseData.roating_time = rotting_time;
    creature_storage[last_name].pCorpseData.ft = ft;
}

void XCreatureStorage::CorpseEffects(const CORPSE_EFFECT_TYPE cet, const int val)
{
    CORPSE_EFFECT ce{};
    ce.type = cet;
    ce.value = val;
    creature_storage[last_name].pCorpseData.effect.push_back(ce);
}

_CREATURE* XCreatureStorage::GetCreatureData(const CREATURE_NAME cn)
{
    return &creature_storage[cn];
}

void XCreatureStorage::CreateQuickBase()
{
    for (int i = 0; i < CN_EOF; i++) {
        if (!creature_storage[i].name.empty()) {
            const CREATURE_CLASS crc = creature_storage[i].cr_class;
            creature_set[vGetBitNumber(crc)].cn[creature_set[vGetBitNumber(crc)].count] = (CREATURE_NAME)i;
            creature_set[vGetBitNumber(crc)].count++;
        }
    }
}

XCreature* XCreatureStorage::Create(const CREATURE_NAME cn)
{
    _CREATURE * cr = &creature_storage[cn];
    XCreature * tcr = nullptr;

    if (cn < CN_UNIQUE) {
        tcr = new XAnyCreature(cr);
    } else {
        switch (cn) {
            case CN_BANDIT:
                tcr = new XBandit(cr);
                break;

            case CN_SHOPKEEPER:
                tcr = new XShopkeeper(cr);
                break;

            case CN_GEFEON:
                tcr = new XGefeon(cr);
                break;

            case CN_RODERIK:
                tcr = new XRoderick(cr);
                break;

            case CN_BEELZEVILE:
                tcr = new XBeelzvile(cr);
                break;

            case CN_HIGHPRIEST:
                tcr = new XHighPriest(cr);
                break;

            case CN_ROTMOTH:
                tcr = new XRotmoth(cr);
                break;

            case CN_GIANA:
                tcr = new XGiana(cr);
                break;

            default:
                tcr = new XAnyCreature(cr);
                break;
        }
    }

    tcr->creature_name = cn;
    RestoreCreatureInfo(tcr);
    return tcr;
}

XCreature* XCreatureStorage::CreateRnd(const CREATURE_CLASS cc, const int lvl)
{
    const int set = vGetBitNumber(vGetRandomBit(cc));
    int count = 100;

    while (count > 0) {
        if (long r = vRand(creature_set[set].count); creature_storage[creature_set[set].cn[r]].crl <= lvl) {
            return Create(creature_set[set].cn[r]);
        }

        count--;
    }

    return Create(creature_set[set].cn[0]);
}

void XCreatureStorage::RestoreCreatureInfo(XCreature* cr)
{
    cr->melee_attack = &creature_storage[cr->creature_name].melee_attack;
    cr->creature_description = creature_storage[cr->creature_name].creature_description.c_str();
    cr->super_info = &creature_storage[cr->creature_name];
}
