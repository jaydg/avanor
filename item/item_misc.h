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

#ifndef ITEM_MISC_H
#define ITEM_MISC_H

#include <sol/sol.hpp>

#include <functional>
#include <string>
#include <unordered_map>

#include <cereal/types/set.hpp>

#include "item/itemkind.h"
#include "item/xanyfood.h"
#include "item/xcap.h"
#include "item/xcloak.h"
#include "item/xshield.h"
#include "item/xtool.h"
#include "item/xweapon.h"
#include "magic/attack_effect_type.h"

// A food defined by content rather than by a C++ class.
struct FoodTemplate {
    std::string name;
    char view;
    int color;

    // Distinct per food, and load-bearing: XAnyFood::Compare() stacks two
    // foods when their nutrition and their ItemType match, so two different
    // trophies sharing a type would merge into one "heap of (2) rat tails".
    ItemType it;

    int value;
    int weight;
    int food_nutrio;
    int consume_nutrio;

    // How well it sits: FT_BESTFOOD through FT_VOMIT shift how much
    // satiation eating it actually yields (XAnyFood::onEat).
    FOOD_TYPE food_type;

    // Weight in the draw when the game asks for a food and does not say
    // which (XItemFactory::CreateItem). Zero - the default - keeps a food
    // out of that draw entirely, which is what a quest trophy wants: a
    // shop should never stock a rat tail.
    int probability;
};

// An item with no behaviour of its own: a name, a look and some numbers.
// The carrier that content-defined plain items are instances of, the way
// XAnyFood is the carrier for content-defined foods. It exists because
// XItem::toString() asserts - every leaf is expected to say how it reads -
// and because cereal needs one concrete registered type to restore into.
class XPlainItem : public XItem
{
    public:
        DECLARE_CREATOR(XPlainItem, XItem);
        XPlainItem() = default;
        explicit XPlainItem(XPlainItem* copy)
            : XItem(copy), content_id(copy->content_id), unique(copy->unique),
              artifact(copy->artifact) {}

        int Compare(XObject* o) override
        {
            return unique ? -1 : XItem::Compare(o);
        }

        [[nodiscard]] std::string GetContentId() const override
        {
            return content_id;
        }

        [[nodiscard]] bool isArtifact() const override
        {
            return artifact;
        }

        std::string content_id;
        bool unique{false};
        bool artifact{false};

        XItem* MakeCopy() override
        {
            return new XPlainItem(this);
        }

        std::string toString() override
        {
            return GetFullName();
        }

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XItem>(this));
            ar(content_id, unique, artifact);
        }
};

struct ContentItemTemplate;

// Everything world/ has defined for itself, by the id it named it.
// Consulted by CreateObjectByName() before the C++ class factory, so
// CreateObject() takes a content id exactly as it takes a class name.
//
// Entries hold a maker rather than one struct of every field any kind of
// item might need: a food and a plain item have almost nothing in common
// beyond a name, and the builders each know how to make their own.
class XItemStorage
{
    public:
        struct Entry {
            std::function<XItem*()> make;

            // What kind of thing it is, and its weight in the draw when
            // the game asks for one of that kind without saying which.
            // Zero probability - the default - keeps it out of that draw,
            // which is what a quest item wants: no shop should stock a
            // part of an ancient machine.
            ItemKind kind;
            int probability;
        };

        static std::unordered_map<std::string, Entry> items;

        // nullptr when no content defined this id - the caller falls back
        // to XClassFactory.
        static XItem* Create(const std::string& id);

        // One item of that kind, drawn by probability. nullptr if content
        // defined nothing drawable of that kind.
        static XItem* CreateRandom(ItemKind kind);

        // Builds a configured item of whichever base its template names,
        // running that base's real constructor first.
        static XItem* MakeItem(const std::string& id, const ContentItemTemplate& t);

        // Builds a configured XAnyFood. A named function rather than
        // something inside the builder's lambda, so that XAnyFood can
        // befriend it - consume_nutrio is protected, and this is what used
        // to be a subclass constructor.
        static XItem* MakeFood(const FoodTemplate& t);
};

// Fluent builder, the item-side twin of MonsterBuilder:
//
//   Food.new("rat_tail")
//       :View("rat tail", '%', xColor.xBROWN)
//       :Basic(ItemType.RATTAIL, 100, 3)
//       :Nutrition(10, 10)
//       :Register()
//
// :Taste() and :Random() are optional - a food that says neither is
// ordinary fare that the game will never hand out on its own.
// Everything a content-defined item states about itself. One struct for
// every base, because they differ only in which of the fields matter: a
// crown says nothing about its dice, a machine part says nothing about
// either. Each field maps onto one line of the constructor such an item
// used to have, so a definition reads against the class it replaced.
struct ContentItemTemplate {
    // Which carrier it becomes, and so which base constructor runs first.
    // PLAIN is the one with no base of its own - an XItem with a name.
    enum Base { PLAIN, WEAPON, CAP, SHIELD, CLOAK, TOOL };

    Base base;

    // Which row of its kind's table the base constructor fills from. It
    // decides the war skill a weapon trains and the material it is made of,
    // the two things a definition does not restate. Unused by PLAIN.
    ItemType base_type;

    std::string name;
    ItemType it;
    ItemKind kind;
    BODY_PART bp;

    // Zero and -1 mean "keep what the base chose": an item that names no
    // look of its own wears its material's.
    char view;
    int color;

    int value;
    int weight;
    int dv;
    int pv;
    int to_hit;
    int dice_count;
    int dice_sides;
    int dice_bonus;

    std::string stats;
    std::string resists;
    AttackEffectType aet;

    // What it is called once known. Empty means it reads as an ordinary
    // item of its kind - see the carrier's toString().
    std::string display_name;

    // Never merges with anything, not even another of itself.
    bool unique;

    // Nothing picks it up but the hero. Something depends on it lying
    // where it was put (see XStandardAI::PickUpItems).
    bool artifact;

    // For a tool: the Lua function the use command hands over to.
    std::string use_handler;
};

// The one builder for items that are not food:
//
//   Item.new("ancient_machine_part")
//       :Plain(ItemType.ANCIENTMACHINEPART, ItemKind.TOOL)
//       :View("ancient machine part", ']', xColor.xDARKGRAY)
//       :Basic(1000, 15)
//       :Register()
//
//   Item.new("torin_axe")
//       :Weapon(ItemType.GREATAXE)
//       :View("ancient axe", '\\')
//       :Basic(12000, 1200)
//       :Combat(8, 2, 8, 5)
//       :Resist("stun:1d1+99 confuse:1d1+99 see_invisible:0d0+20")
//       :Stats("To:1d1+10")
//       :Brand(AttackEffectType.COLD)
//       :Called("Axe of Torin")
//       :Unique()
//       :Register()
//
// An artifact is not a kind of its own: it is an item of its base type that
// says what it is Called, declares itself Unique, and never asks to be in
// the random draw. An ordinary weapon is the same definition with :Random()
// instead of those last two.
class ItemBuilder
{
    public:
        explicit ItemBuilder(std::string id);

        // Exactly one of these, first: it says what the thing is.
        ItemBuilder& Plain(ItemType it, ItemKind kind);
        ItemBuilder& Weapon(ItemType base_type);
        ItemBuilder& Cap(ItemType base_type);
        ItemBuilder& Shield(ItemType base_type);
        ItemBuilder& Cloak(ItemType base_type);
        ItemBuilder& Tool(ItemType it);

        // view and color are optional; without them it keeps its base's.
        ItemBuilder& View(const std::string& name, sol::optional<std::string> view,
            sol::optional<int> color);

        // Only where it differs from the base row's type - a forest
        // brother's cloak is built as a cloak but is its own kind of thing.
        ItemBuilder& Type(ItemType it);

        ItemBuilder& Basic(int value, int weight);
        ItemBuilder& Armour(int dv, int pv);
        ItemBuilder& Combat(int to_hit, int count, int sides, int bonus);
        ItemBuilder& Stats(const std::string& stats);
        ItemBuilder& Resist(const std::string& resists);
        ItemBuilder& Brand(AttackEffectType aet);
        ItemBuilder& Called(const std::string& display_name);
        ItemBuilder& Unique();
        ItemBuilder& Artifact();
        ItemBuilder& Use(const std::string& handler);
        ItemBuilder& Random(int probability);

        void Register();

    private:
        std::string id;
        int probability;
        ContentItemTemplate t{};
};

class FoodBuilder
{
    public:
        explicit FoodBuilder(std::string id);

        FoodBuilder& View(const std::string& name, char view, int color);
        FoodBuilder& Basic(ItemType it, int value, int weight);
        FoodBuilder& Nutrition(int food_nutrio, int consume_nutrio);
        FoodBuilder& Taste(FOOD_TYPE food_type);
        FoodBuilder& Random(int probability);

        void Register();

    private:
        std::string id;
        FoodTemplate t{};
};

// The carriers content-defined items are instances of, one per base class
// that matters.
//
// They exist for two reasons. Construction has to run the real base
// constructor - XWeapon(it) fills in the war-skill binding and the material
// that a definition does not restate - and toString() has to be able to
// answer with a name of the item's own where it has one, rather than the
// material-and-template name its base composes ("steel broad sword" instead
// of 'broadsword named "Glamdring"').
//
// Nothing else needed a subclass: nothing in the game dynamic_casts to
// XWeapon, XCap, XShield or XCloak, so one carrier per base covers every
// content item of that base - an artifact and an ordinary sword alike.
#define DECLARE_CONTENT_CARRIER(CarrierName, BaseName)                        \
    class CarrierName : public BaseName                                       \
    {                                                                         \
        public:                                                               \
            DECLARE_CREATOR(CarrierName, BaseName);                           \
            explicit CarrierName(ItemType base_type = ItemType::RANDOM)       \
                : BaseName(base_type) {}                                      \
            explicit CarrierName(CarrierName* copy)                           \
                : BaseName(copy), content_id(copy->content_id),               \
                  display_name(copy->display_name), unique(copy->unique),     \
                  artifact(copy->artifact) {}                                 \
                                                                              \
            XItem* MakeCopy() override                                        \
            {                                                                 \
                return new CarrierName(this);                                 \
            }                                                                 \
                                                                              \
            /* One of a kind never merges with anything, not even another */  \
            /* of itself. Ordinary items fall through to the base rule. */     \
            int Compare(XObject* o) override                                  \
            {                                                                 \
                return unique ? -1 : BaseName::Compare(o);                    \
            }                                                                 \
                                                                              \
            /* An item with no name of its own reads as an ordinary one of */ \
            /* its kind - which is what the forest brother's cloak wants, */  \
            /* being a disguise, and what every plain weapon wants. */         \
            std::string toString() override                                   \
            {                                                                 \
                return display_name.empty()                                   \
                    ? BaseName::toString()                                    \
                    : GetArtifactName(display_name);                          \
            }                                                                 \
                                                                              \
            [[nodiscard]] std::string GetContentId() const override           \
            {                                                                 \
                return content_id;                                            \
            }                                                                 \
                                                                              \
            /* Its own name, under the same condition toString() shows */     \
            /* it: an item with no display_name of its own reads as an */     \
            /* ordinary one of its kind, which is what the forest */          \
            /* brother's cloak wants, being a disguise. */                    \
            [[nodiscard]] std::string GetProperName() override                \
            {                                                                 \
                return isIdentified() ? display_name : std::string{};         \
            }                                                                 \
                                                                              \
            [[nodiscard]] bool isArtifact() const override                    \
            {                                                                 \
                return artifact;                                              \
            }                                                                 \
                                                                              \
            std::string content_id;                                           \
            std::string display_name;                                         \
            bool unique{false};                                               \
            bool artifact{false};                                             \
                                                                              \
            template<class Archive>                                           \
            void serialize(Archive& ar)                                       \
            {                                                                 \
                ar(cereal::base_class<BaseName>(this));                       \
                ar(content_id, display_name, unique, artifact);               \
            }                                                                 \
    };

DECLARE_CONTENT_CARRIER(XContentWeapon, XWeapon)
DECLARE_CONTENT_CARRIER(XContentCap, XCap)
DECLARE_CONTENT_CARRIER(XContentShield, XShield)
DECLARE_CONTENT_CARRIER(XContentCloak, XCloak)

#undef DECLARE_CONTENT_CARRIER

// The carrier for a content-defined tool - the first content item with
// behaviour rather than only figures.
//
// A tool is the one item the use command can be pointed at, and what it
// then does is content, not engine: the Eye of Raa throws lightning, a
// pickaxe digs. XLuaTool holds the name of a Lua function and hands the
// use over to it, the way a creature hands its turn to its event handler.
//
// Written out rather than macro-made, because unlike the others it is not
// just its base with a name: XTool is where onUse lives, and this is the
// only carrier that overrides it.
class XLuaTool : public XTool
{
    public:
        DECLARE_CREATOR(XLuaTool, XTool);
        XLuaTool() = default;
        explicit XLuaTool(XLuaTool* copy)
            : XTool(copy), content_id(copy->content_id), unique(copy->unique),
              artifact(copy->artifact), use_handler(copy->use_handler) {}

        XItem* MakeCopy() override
        {
            return new XLuaTool(this);
        }

        // Every tool in the game reads as its bare name - no material, no
        // enhancement prefix, no "heap of".
        std::string toString() override
        {
            return name;
        }

        int Compare(XObject* o) override
        {
            return unique ? -1 : XTool::Compare(o);
        }

        [[nodiscard]] std::string GetContentId() const override
        {
            return content_id;
        }

        [[nodiscard]] bool isArtifact() const override
        {
            return artifact;
        }

        // Calls the Lua function named by use_handler as
        //   handler(state, item, user) -> Result
        // A tool that named no handler cannot be used, which is XTool's own
        // default answer.
        RESULT onUse(ItemUsageState uis, XCreature* cr) override;

        std::string content_id;
        bool unique{false};
        bool artifact{false};
        std::string use_handler;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XTool>(this));
            ar(content_id, unique, artifact, use_handler);
        }
};

class XChest : public XItem
{
    public:
        XItemList contain;
        DECLARE_CREATOR(XChest, XItem);
        XChest()
        {
            assert(0);
        }

        XChest(int item_count, ItemKind kind_mask, int low_v, int high_v);
        // Chests are not copyable
        XChest(XChest*) = delete;

        int Compare(XObject * /*o*/) override
        {
            return -1;
        }

        std::string toString() override;
    protected:
        // teardown hook, called by XObject::Invalidate()
        void OnInvalidate() override;
    public:

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XItem>(this));
            ar(contain);
        }
};

#endif
