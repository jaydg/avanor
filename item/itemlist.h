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

#ifndef ITEMLIST_H
#define ITEMLIST_H

#include <memory>
#include <set>

/* Forward declaration */
class XItem;

struct compare {
    // Transparent so contain.erase(raw_ptr)/.find(raw_ptr) - pervasive
    // throughout the codebase - keep working by heterogeneous lookup once
    // XItemList holds shared_ptr<XItem> instead of raw XItem*, without
    // requiring every one of those call sites to be rewritten.
    using is_transparent = void;

    bool operator()(const XItem* lhs, const XItem* rhs) const;
    bool operator()(const std::shared_ptr<XItem>& lhs, const std::shared_ptr<XItem>& rhs) const;
    bool operator()(const std::shared_ptr<XItem>& lhs, const XItem* rhs) const;
    bool operator()(const XItem* lhs, const std::shared_ptr<XItem>& rhs) const;
};

class XItemList : public std::set<std::shared_ptr<XItem>, compare>
{
        using Base = std::set<std::shared_ptr<XItem>, compare>;

        // Returns an iterator to the stack item got merged into, or
        // end() if no matching stack exists.
        iterator TryMerge(const std::shared_ptr<XItem>& item);

    public:
        using Base::Base;

        // By-value, not the usual const-ref-plus-rvalue-ref pair: std::set
        // has both insert(const value_type&) and insert(value_type&&), and
        // every caller passes a temporary (XItem::Own(...)).
        // One by-value overload hides both base ones unambiguously and
        // handles lvalue/rvalue callers alike.
        std::pair<iterator, bool> insert(std::shared_ptr<XItem> item);
        iterator insert(iterator hint, std::shared_ptr<XItem> item);
};

#endif
