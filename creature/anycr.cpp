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

#include <iostream>

#include "creature/anycr.h"
#include "creature/shopkeeper.h"
#include "engine/xapi.h"
#include "item/xring.h"
#include "item/item_misc.h"
#include "item/itemf.h"
#include "helpers/keyword_dice.h"
#include "magic/modifier.h"
#include "magic/resist.h"
#include "magic/brand.h"

void CreatureTemplate::RegisterLua(sol::state_view& lua)
{
    lua.new_enum("CreatureTemplate",
        "SUPPRESS_INVIS", CreatureTemplate::SUPPRESS_INVIS,
        "SEE_INVIS", CreatureTemplate::SEE_INVIS,
        "VERY_LOW", CreatureTemplate::Level::VERY_LOW,
        "LOW", CreatureTemplate::Level::LOW,
        "ABOVE_LOW", CreatureTemplate::Level::ABOVE_LOW,
        "AVG", CreatureTemplate::Level::AVG,
        "ABOVE_AVG", CreatureTemplate::Level::ABOVE_AVG,
        "HI", CreatureTemplate::Level::HI,
        "ABOVE_HI", CreatureTemplate::Level::ABOVE_HI,
        "VERY_HI", CreatureTemplate::Level::VERY_HI,
        "EXTREM_HI", CreatureTemplate::Level::EXTREM_HI,
        "UNIQUE", CreatureTemplate::Level::UNIQUE,
        "ANY", CreatureTemplate::Level::ANY,
        "VL", CreatureTemplate::Level::VL,
        "LA", CreatureTemplate::Level::LA,
        "AH", CreatureTemplate::Level::AH,
        "HVH", CreatureTemplate::Level::HVH
    );
}

REGISTER_CLASS(XAnyCreature);
CEREAL_REGISTER_TYPE(XAnyCreature);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XCreature, XAnyCreature);

std::unordered_map<CREATURE_NAME, CreatureTemplate> XCreatureStorage::creature_storage;
std::unordered_map<CREATURE_CLASS, CREATURE_SET_REC> XCreatureStorage::creature_set;

const std::unordered_map<CREATURE_NAME, XCreature*(*)(CreatureTemplate*)> XCreatureStorage::unique_creators = {
    {CN_SHOPKEEPER, [](CreatureTemplate* cr) -> XCreature* { return new XShopkeeper(cr); }},
};

XAnyCreature::XAnyCreature(CreatureTemplate * cr)
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

    dv	= cr->dv.Throw();
    pv	= cr->pv.Throw();
    to_hit	= cr->hit.Throw();
    MAX_HP	= cr->hp.Throw();
    HP	= GetMaxHP();

    MAX_PP	= cr->pp.Throw();
    PP	= MAX_PP;

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

                if (it == IT_NONE) {
                    item = ICREATEA((ItemKind)(mask));
                } else {
                    item = ICREATEB((ItemKind)(mask), it, 0, 10000000);
                }

                if (CanWear(item)) {
                    // Anything that needs a second thing to be any use -
                    // a bow without arrows - asks for it here.
                    item->OnOutfit(this);
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
                bp->Wear(ICREATE(bp->GetProperKind(), 0, 1000000));
            }
        }
    }

    if (hand_1) {
        XItem * weapon = ICREATE(ItemKind::WEAPON, 0, 10000000);
        wsk->SetLevel(weapon->wt, 2); // just basic weapon level
        hand_1->Wear(weapon);
    }

    if (hand_2) {
        hand_2->Wear(ICREATE(ItemKind::SHIELD, 0, 10000000));
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

    if (cr->generation_flags & CreatureTemplate::SUPPRESS_INVIS) {
        if (neck && neck->Item() && neck->Item()->resistances->GetResistance("invisible") > 0) {
            unwear_and_invalidate(neck);
        }

        if (ring1 && ring1->Item() && ring1->Item()->resistances->GetResistance("invisible") > 0) {
            unwear_and_invalidate(ring1);
        }

        if (ring2 && ring2->Item() && ring2->Item()->resistances->GetResistance("invisible") > 0) {
            unwear_and_invalidate(ring2);
        }
    }

    if (cr->generation_flags & CreatureTemplate::SEE_INVIS) {
        while (true) {
            if (neck && neck->Item() && neck->Item()->resistances->GetResistance("see_invisible")) {
                break;
            }

            if (ring1 && ring1->Item() && ring1->Item()->resistances->GetResistance("see_invisible")) {
                break;
            }

            if (ring2 && ring2->Item() && ring2->Item()->resistances->GetResistance("see_invisible")) {
                break;
            }

            if (ring1 && ring1->Item()) {
                unwear_and_invalidate(ring1);
                ring1->Wear(new XRing("see_invisible"));
            }

            break;
        }
    }

    // Create money if components more than 2
    if (components.size() > 2 && vRand(3) == 0) {
        for (int i = 0; i < vGetHighBitNum(static_cast<unsigned int>(cr->crl)) + 1; i++) {
            XItem * it = ICREATEA(ItemKind::MONEY);
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
    // Whether there is anything left to leave is the class's own
    // business - world/creature_classes.lua says so with :NoCorpse().
    const CreatureClassStats* row = FindCreatureClass(creature_class);

    if (vRand(5) == 0 && (!row || row->leaves_corpse)) {
        DropItem(new XCorpse(this));
    }

    XCreature::Die(killer);
}

CreatureTemplate* XCreatureStorage::GetCreatureData(const CREATURE_NAME cn)
{
    return &creature_storage.at(cn);
}

void XCreatureStorage::CreateQuickBase()
{
    for (auto& [cn, cr] : creature_storage) {
        creature_set[cr.cr_class].cn.push_back(cn);
    }
}

XCreature* XCreatureStorage::Create(const CREATURE_NAME cn)
{
    CreatureTemplate * cr = &creature_storage.at(cn);
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

XCreature* XCreatureStorage::CreateRnd(const CreatureClassSet& cc, const CreatureTemplate::Level lvl)
{
    // One of the classes asked for, drawn at random, then a monster of
    // that class - the same two steps the old bit-picking did.
    std::vector<const CREATURE_SET_REC*> sets;

    for (const CREATURE_CLASS& id : cc) {
        if (auto it = creature_set.find(id); it != creature_set.end() && !it->second.cn.empty()) {
            sets.push_back(&it->second);
        }
    }

    if (sets.empty()) {
        std::cerr << "world: asked for a creature of '" << cc.toString()
                  << "', and nothing of that sort is defined" << std::endl;

        return nullptr;
    }

    const CREATURE_SET_REC* set = sets[vRand(static_cast<int>(sets.size()))];
    int count = 100;

    while (count > 0) {
        if (long r = vRand(static_cast<int>(set->cn.size()));
            creature_storage.at(set->cn[r]).crl <= lvl) {
            return Create(set->cn[r]);
        }

        count--;
    }

    return Create(set->cn[0]);
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
    // CreatureTemplate::unique) - either can be true without the other.
    cr->unique = unique_creators.find(cr->creature_name) != unique_creators.end()
        || creature_storage[cr->creature_name].unique;
}

MonsterBuilder::MonsterBuilder(CREATURE_NAME id, CREATURE_NAME base) : id(id)
{
    if (!base.empty()) {
        cr = *XCreatureStorage::GetCreatureData(base);
    }
}

MonsterBuilder& MonsterBuilder::View(const std::string& name, char view, int color, XCreature::PersonType person, CreatureTemplate::Level crl, const CREATURE_CLASS& cr_class)
{
    cr.name = name;
    cr.view = view;
    cr.color = color;
    cr.person = person;
    cr.crl = crl;
    cr.cr_class = cr_class;
    return *this;
}

MonsterBuilder& MonsterBuilder::Basic(const std::string& speed, const std::string& move_energy, const std::string& attack_energy, XCreature::Size size, const std::string& weight)
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

MonsterBuilder& MonsterBuilder::Melee(const std::string& br, int prob)
{
    MELEE_ATTACK ma{};
    ma.e_attack = EA_NONE;
    ma.br_attack = BrandSet();

    if (CheckBrandExists(br, "a creature's melee attack")) {
        ma.br_attack.Add(br);
    }

    ma.prob = prob;
    cr.melee_attack.push_back(ma);
    return *this;
}

MonsterBuilder& MonsterBuilder::MeleeExtra(EXTENDED_ATTACK ea, int prob)
{
    MELEE_ATTACK ma{};
    ma.e_attack = ea;
    ma.br_attack = BrandSet();
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

MonsterBuilder& MonsterBuilder::Equip(unsigned int mask, ItemType it, int prob)
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
    er.it = IT_NONE;
    cr.equipment.push_back(er);
    return *this;
}

// How long a corpse of this species keeps, and how it sits when eaten.
// The taste half used to be written into the row and read by nothing at
// all, so a creature declaring one tasted like any other.
MonsterBuilder& MonsterBuilder::Corpse(const int rotting_time, const std::string& taste)
{
    cr.pCorpseData.roating_time = rotting_time;

    if (CheckTasteExists(taste, ("creature '" + id + "'").c_str())) {
        cr.pCorpseData.taste = taste;
    }

    return *this;
}

// The mechanisms that name nothing: VOMIT, STOMACH, SATIATION.
MonsterBuilder& MonsterBuilder::CorpseEffect(XCorpse::EffectType cet, int val)
{
    XCorpse::Effect ce{};
    ce.type = cet;
    ce.value = val;
    cr.pCorpseData.effect.push_back(ce);
    return *this;
}

// Eating this raises (or lowers) a stat: :CorpseStat("To", 1).
MonsterBuilder& MonsterBuilder::CorpseStat(const std::string& stat, int val)
{
    if (StatKeyword(stat) < 0) {
        std::cerr << "world: creature '" << id << "' has a corpse effect on '"
                  << stat << "', which is not a stat - ignored" << std::endl;
        return *this;
    }

    XCorpse::Effect ce{};
    ce.type = XCorpse::EffectType::STAT;
    ce.target = stat;
    ce.value = val;
    cr.pCorpseData.effect.push_back(ce);
    return *this;
}

// Eating this confers resistance: :CorpseResist("fire", 1). Any row
// world/resistances.lua declares, not a fixed few.
MonsterBuilder& MonsterBuilder::CorpseResist(const std::string& resist, int val)
{
    if (!FindResistance(resist)) {
        std::cerr << "world: creature '" << id << "' has a corpse effect on a resistance '"
                  << resist << "' that world/resistances.lua does not declare - ignored"
                  << std::endl;
        return *this;
    }

    XCorpse::Effect ce{};
    ce.type = XCorpse::EffectType::RESIST;
    ce.target = resist;
    ce.value = val;
    cr.pCorpseData.effect.push_back(ce);
    return *this;
}

// Eating this lays something on the eater after a delay:
// :CorpseModifier(Modifier.POISON, 10).
MonsterBuilder& MonsterBuilder::CorpseModifier(const std::string& modifier, int val)
{
    // A name nothing knows: say so while the file that wrote it is
    // loading, and lay nothing on.
    if (!IsKnownModifier(modifier)) {
        std::cerr << "world: :CorpseModifier() names a modifier '" << modifier
                  << "', which nothing defines" << std::endl;

        return *this;
    }

    XCorpse::Effect ce{};
    ce.type = XCorpse::EffectType::MODIFIER;
    ce.modifier = modifier;
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
    // Last chance to catch a definition that says nothing. A misspelt
    // class is a class no world file declares, and a CreatureTemplate
    // member the engine never registered arrives here as 0 rather than
    // as an error - either way the creature looks fine until something
    // downstream asks it what it is, possibly hours into a game.
    if (XCreatureStorage::creature_storage.count(id)) {
        std::cerr << "world: creature '" << id << "' is defined twice" << std::endl;
    }

    if (cr.name.empty()) {
        std::cerr << "world: creature '" << id << "' has no name" << std::endl;
    }

    if (cr.cr_class.empty()) {
        std::cerr << "world: creature '" << id << "' has no class" << std::endl;
    } else {
        CheckCreatureClassExists(cr.cr_class, ("creature '" + id + "'").c_str());
    }

    if (static_cast<unsigned int>(cr.crl) == 0) {
        std::cerr << "world: creature '" << id << "' has no level - the CreatureTemplate"
                     " member named in its View() is misspelt, or the engine never"
                     " registered it" << std::endl;
    }

    XCreatureStorage::creature_storage[id] = cr;
}
