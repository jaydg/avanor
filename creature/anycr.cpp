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

#include <cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>
#include <sol/sol.hpp>

#include "creature/anycr.h"
#include "creature/unique.h"
#include "engine/xapi.h"
#include "game/quest.h"
#include "item/item_misc.h"
#include "item/itemf.h"

void CREATURE_DEF::RegisterLua(sol::state_view& lua)
{
    lua.new_enum("CREATURE_DEF",
        "SUPPRESS_INVIS", CREATURE_DEF::SUPPRESS_INVIS,
        "SEE_INVIS", CREATURE_DEF::SEE_INVIS
    );
}

REGISTER_CLASS(XAnyCreature);
CEREAL_REGISTER_TYPE(XAnyCreature);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XCreature, XAnyCreature);

std::unordered_map<CREATURE_NAME, CREATURE_DEF> XCreatureStorage::creature_storage;
CREATURE_SET_REC XCreatureStorage::creature_set[32];

const std::unordered_map<CREATURE_NAME, XCreature*(*)(CREATURE_DEF*)> XCreatureStorage::unique_creators = {
    {CN_BANDIT,     [](CREATURE_DEF* cr) -> XCreature* { return new XBandit(cr); }},
    {CN_SHOPKEEPER, [](CREATURE_DEF* cr) -> XCreature* { return new XShopkeeper(cr); }},
    {CN_GEFEON,     [](CREATURE_DEF* cr) -> XCreature* { return new XGefeon(cr); }},
    {CN_RODERIK,    [](CREATURE_DEF* cr) -> XCreature* { return new XRoderick(cr); }},
    {CN_BEELZEVILE, [](CREATURE_DEF* cr) -> XCreature* { return new XBeelzvile(cr); }},
    {CN_HIGHPRIEST, [](CREATURE_DEF* cr) -> XCreature* { return new XHighPriest(cr); }},
    {CN_ROTMOTH,    [](CREATURE_DEF* cr) -> XCreature* { return new XRotmoth(cr); }},
    {CN_GIANA,      [](CREATURE_DEF* cr) -> XCreature* { return new XGiana(cr); }},
};

XAnyCreature::XAnyCreature(CREATURE_DEF * cr)
{
    view = cr->view;
    color = cr->color;
    name = cr->name.c_str();

    creature_class = cr->cr_class;

    dice.Setup(cr->dice);

    stats = cr->stats_gen.Generate();
    resistances = cr->r_gen.Generate();

    max_stats.Set(stats.get());

    XBodyPart::Create(this, cr->body);

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
    xai->SetAIFlag(static_cast<XStandardAI::Flag>(cr->ai_flags));

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
                    && GetGender() == XCreature::MALE) {
                    // This is a kludge to prevent a "Roderick in a dress" scenario.
                    item->it = IT_CLOTHES;
                    item->name = "clothes";
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

    for (auto& bp: components) {
        if (!bp->Item() && vRand(100) < cr->equip_probability) {
            if (bp->bp_uin == BP_HAND) {
                if (hand_1 == nullptr) {
                    hand_1 = bp.get();
                } else {
                    hand_2 = bp.get();
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

    // UnWear() doesn't remove the item from contain anymore (worn items
    // stay resident there - see XBodyPart::Wear()), so a bare
    // UnWear()->Invalidate() would leave a zombie entry behind: still in
    // contain, but invalid. Erase it first.
    auto unwear_and_invalidate = [this](XBodyPart* bp) {
        auto old_item = bp->UnWear();

        if (auto it = contain.find(old_item); it != contain.end()) {
            contain.erase(it);
        }

        old_item->Invalidate();
    };

    if (cr->generation_flags & CREATURE_DEF::SUPPRESS_INVIS) {
        if (neck && neck->Item() && neck->Item()->resistances->GetResistance(XResistance::INVISIBLE) > 0) {
            unwear_and_invalidate(neck);
        }

        if (ring1 && ring1->Item() && ring1->Item()->resistances->GetResistance(XResistance::INVISIBLE) > 0) {
            unwear_and_invalidate(ring1);
        }

        if (ring2 && ring2->Item() && ring2->Item()->resistances->GetResistance(XResistance::INVISIBLE) > 0) {
            unwear_and_invalidate(ring2);
        }
    }

    if (cr->generation_flags & CREATURE_DEF::SEE_INVIS) {
        while (true) {
            if (neck && neck->Item() && neck->Item()->resistances->GetResistance(XResistance::SEE_INVISIBLE)) {
                break;
            }

            if (ring1 && ring1->Item() && ring1->Item()->resistances->GetResistance(XResistance::SEE_INVISIBLE)) {
                break;
            }

            if (ring2 && ring2->Item() && ring2->Item()->resistances->GetResistance(XResistance::SEE_INVISIBLE)) {
                break;
            }

            if (ring1 && ring1->Item()) {
                unwear_and_invalidate(ring1);
                ring1->Wear(new XRing(XEnhance::SEEINVISIBLE));
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
            it->Drop(l, x, y);
        }
    }

    if (creature_class == CR_ORC) {
        if (killer && killer->isHero()) {
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

CREATURE_DEF* XCreatureStorage::GetCreatureData(const CREATURE_NAME cn)
{
    return &creature_storage.at(cn);
}

void XCreatureStorage::CreateQuickBase()
{
    for (auto& [cn, cr] : creature_storage) {
        creature_set[vGetBitNumber(cr.cr_class)].cn.push_back(cn);
    }
}

XCreature* XCreatureStorage::Create(const CREATURE_NAME cn)
{
    CREATURE_DEF * cr = &creature_storage.at(cn);
    XCreature * tcr = nullptr;

    if (auto it = unique_creators.find(cn); it != unique_creators.end()) {
        tcr = it->second(cr);
    } else {
        tcr = new XAnyCreature(cr);
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
        if (long r = vRand(static_cast<int>(creature_set[set].cn.size()));
            creature_storage.at(creature_set[set].cn[r]).crl <= lvl) {
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

    // Derived, not persisted - same reasoning as the three fields above:
    // fully determined by its (persisted) creature_name, so it's
    // re-derived here rather than stored, on both the fresh-creation
    // path (from Create()) and the Cereal-load path (from XCreature::
    // FixupCreatureInfo(), which calls this too). True for a hand-
    // written unique-NPC class (registry membership) or a generic
    // monster explicitly marked unique in its own definition (see
    // CREATURE_DEF::unique) - either can be true without the other.
    cr->unique = unique_creators.find(cr->creature_name) != unique_creators.end()
        || creature_storage[cr->creature_name].unique;
}

MonsterBuilder::MonsterBuilder(CREATURE_NAME id, CREATURE_NAME base) : id(id)
{
    if (!base.empty()) {
        cr = *XCreatureStorage::GetCreatureData(base);
    }
}

MonsterBuilder& MonsterBuilder::View(const std::string& name, char view, int color, XCreature::PersonType person, CREATURE_LEVEL crl, CREATURE_CLASS cr_class)
{
    cr.name = name;
    cr.view = view;
    cr.color = color;
    cr.person = person;
    cr.crl = crl;
    cr.cr_class = cr_class;
    return *this;
}

MonsterBuilder& MonsterBuilder::Basic(const std::string& speed, const std::string& move_energy, const std::string& attack_energy, CREATURE_SIZE size, const std::string& weight)
{
    cr.speed = XDice(speed);
    cr.move_energy = XDice(move_energy);
    cr.attack_energy = XDice(attack_energy);
    cr.creature_size = size;
    cr.creature_weight = XDice(weight);
    return *this;
}

MonsterBuilder& MonsterBuilder::Body(const std::string& body, int prob, unsigned int gen_flags)
{
    cr.body = body;
    cr.equip_probability = prob;
    cr.generation_flags = gen_flags;
    return *this;
}

MonsterBuilder& MonsterBuilder::AI(unsigned int flags)
{
    cr.ai_flags = flags;
    return *this;
}

MonsterBuilder& MonsterBuilder::Stats(const std::string& stats)
{
    cr.stats_gen.Init(stats.c_str());
    return *this;
}

MonsterBuilder& MonsterBuilder::Resist(const std::string& resists)
{
    cr.r_gen.Init(resists.c_str());
    return *this;
}

MonsterBuilder& MonsterBuilder::Combat(const std::string& hit, const std::string& dice)
{
    cr.hit = XDice(hit);
    cr.dice = XDice(dice);
    return *this;
}

MonsterBuilder& MonsterBuilder::Main(const std::string& dv, const std::string& pv, const std::string& hp, const std::string& pp)
{
    cr.dv = XDice(dv);
    cr.pv = XDice(pv);
    cr.hp = XDice(hp);
    cr.pp = XDice(pp);
    return *this;
}

MonsterBuilder& MonsterBuilder::Description(const std::string& descr)
{
    cr.creature_description = descr;
    return *this;
}

MonsterBuilder& MonsterBuilder::Melee(BRAND_TYPE br, int prob)
{
    MELEE_ATTACK ma{};
    ma.e_attack = EA_NONE;
    ma.br_attack = br;
    ma.prob = prob;
    cr.melee_attack.push_back(ma);
    return *this;
}

MonsterBuilder& MonsterBuilder::MeleeExtra(EXTENDED_ATTACK ea, int prob)
{
    MELEE_ATTACK ma{};
    ma.e_attack = ea;
    ma.br_attack = BR_NONE;
    ma.prob = prob;
    cr.melee_attack.push_back(ma);
    return *this;
}

MonsterBuilder& MonsterBuilder::LearnSkill(XSkill::Skill skt, int lvl)
{
    SKILL_REC scr{};
    scr.level = lvl;
    scr.skt = skt;
    cr.skills.push_back(scr);
    return *this;
}

MonsterBuilder& MonsterBuilder::LearnSpell(SPELL_NAME spn)
{
    cr.spells.push_back(spn);
    return *this;
}

MonsterBuilder& MonsterBuilder::Equip(unsigned int mask, ITEM_TYPE it, int prob)
{
    EQUIP_REC er{};
    er.mask = mask;
    er.count = 1;
    er.probability = prob;
    er.it = it;
    cr.equipment.push_back(er);
    return *this;
}

MonsterBuilder& MonsterBuilder::EquipCount(unsigned int mask, int count, int prob)
{
    EQUIP_REC er{};
    er.mask = mask;
    er.count = count;
    er.probability = prob;
    er.it = IT_UNKNOWN;
    cr.equipment.push_back(er);
    return *this;
}

MonsterBuilder& MonsterBuilder::Corpse(int rotting_time, FOOD_TYPE ft)
{
    cr.pCorpseData.roating_time = rotting_time;
    cr.pCorpseData.ft = ft;
    return *this;
}

MonsterBuilder& MonsterBuilder::CorpseEffect(CORPSE_EFFECT_TYPE cet, int val)
{
    CORPSE_EFFECT ce{};
    ce.type = cet;
    ce.value = val;
    cr.pCorpseData.effect.push_back(ce);
    return *this;
}

MonsterBuilder& MonsterBuilder::Unique()
{
    cr.unique = true;
    return *this;
}

void MonsterBuilder::Register()
{
    XCreatureStorage::creature_storage[id] = cr;
}
