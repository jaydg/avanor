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

#ifndef __XHERO_H
#define __XHERO_H

#include <cmath>

#include "creature/anycr.h"
#include "creature/creature.h"
#include "engine/global.h"
#include "helpers/xgui.h"
#include "item/incl_i.h"

extern int _exit_flag;

enum INVENTORY_FLAG {
    IF_NONE,
    IF_FIXED_MASK,
    IF_VIEW_ONLY,
};

enum SKILL_FLAG {
    SKF_IMPROVE_SKILL,
    SKF_LIST_SKILL,
    SKF_USE_SKILL,
};

class XHero final : public XCreature
{
    protected:
        XHero()
        {
            last_char = '5';
            isDisturb = 0;
            run_way_count = 0;
            target = nullptr;
        }

        int last_char;
        int run_way_count;
        XPtr<XCreature> target; // for convenient user interface
        int PossibleWayCount(int px, int py) const;
    public:
        DECLARE_CREATOR(XHero, XCreature);
        explicit XHero(int flag);
        void PlayerSetup();
        void NewMove() override;
        void Move() override;
        XItem* Inventory(XItemList* item_list, ITEM_MASK mask = IM_ALL, INVENTORY_FLAG flag = IF_NONE, int ret_item_count = 0, ITEM_FILTR* ifiltr = nullptr, FILE* f = nullptr);
        void Equipment(FILE* f = nullptr);
        void PickItem();
        void DropItem();
        void LookAt();
        static void CreateScreenShot();
        static void DumpVBuffer(FILE* f);
        void ReadAll();
        int Compare(XObject* o) override
        {
            return -1;
        }

        void ExpList() const;
        void InfoList();
        void Eat();
        int XShoot();
        int Targeting(int range, XPoint* pt);
        int GetTarget(TARGET_REASON tr, XPoint* pt = nullptr, int max_range = 0, XObject** back = nullptr) override; //Get target for a spell
        XItem* SelectItem(ITEM_FILTR* filter, bool isGetAll = false) override;

        int SelectPosition(XPoint * pt, int flag = 0);
        unsigned int turn_count{};
        void OpenDoor();
        void OpenChest();
        void CloseDoor();
        void Die(XCreature* killer) override;
        int XCast(FILE* f = nullptr);
        XList<XSpell*>::iterator last_cast;
        int RepeatCast();

        void MagicLevelList() const;
        XSkill* SkillsList(SKILL_FLAG skill_flag, int marks_left = 0, FILE* f = nullptr) const;
        int UseSkill();
        void IncLevel() override;
        void WarSkillsList(FILE* f = nullptr) const;

        void DrinkPotion();
        void SetTactics();
        void SaveGame();
        int UseTool();
        int UseOuterObject();
        void QuickPay();
        void Pray();
        static int WhichDirection(XPoint* pt, int flag = 1); // flag == 1 - allow 0,0 coords (self)
        XItem* onIdentifyItem() override;
        void ShowResistance(FILE* f = nullptr);

        void ActivateTrap();
        void GiveItem();
        void ChatWithMonster();
        int Chat(XCreature* chatter, const char* msg) override;

        void FirstStep(int _x, int _y, XLocation* _l) override;
        void LastStep() override;

        int stopAction() override;

        void Store(XFile* f) override;
        void Restore(XFile* f) override;

        void doSacrifice();
        int OrderCompanion();
        static int ExecuteScript();

        int race{};
        int profession{};

        const char* GetRaceStr() const;
        const char* GetProfessionStr() const;

        static void EndGame(const char* end_msg);

        // ALCHEMY
        int LearnReception(POTION_NAME pn1, POTION_NAME pn2, POTION_NAME pn3);
        void ShowReception();
        void MixPotions();
        XList<XAlchemyRec*> reception_list;
};

class XGuiItem_Inventory final : public XGuiItem
{
        XItem* pItem;
        char buf[256]{};
    public:
        explicit XGuiItem_Inventory(XItem* item) : XGuiItem(), pItem(item)
        {
            char tbuf[256];
            strcpy(buf, MSG_LIGHTGRAY);
            item->toString(buf + strlen(buf));

            for (size_t i = strlen(buf); i < size_x; i++) {
                buf[i] = ' ';
            }

            sprintf(tbuf, MSG_BROWN "[" MSG_LIGHTGRAY "%d" MSG_BROWN "]", item->weight * item->quantity);
            strcpy(buf + size_x - 5 - x_strlen(tbuf), tbuf);
        }

        int isSelectable() override
        {
            return 1;
        }

        int isTitle() override
        {
            return 0;
        }

        bool SetWidth(int new_width) override
        {
            return true;
        }

        int GetHeight() override
        {
            return 1;
        }

        const char* operator[](int index) override
        {
            return buf;
        }
};

#endif
