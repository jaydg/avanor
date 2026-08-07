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

#ifndef BODYPART_H
#define BODYPART_H

#include <cereal/access.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>

#include "engine/xmapobj.h"
#include "engine/xobject.h"

class XItem;
class XCreature;

enum BODY_PART {
    BP_UNKNOWN,
    BP_HEAD,
    BP_NECK,
    BP_BODY,
    BP_CLOAK,
    BP_HAND,
    BP_RING,
    BP_GLOVES,
    BP_BOOTS,
    BP_LIGHT_SOURCE,
    BP_TOOL,
    BP_MISSILE_WEAPON,
    BP_MISSILE,
    BP_EOF,
    BP_OTHER,
    BP_ONE_HANDED,
    BP_TWO_HANDED
};

class XBodyPart
{
        // Weak: worn items now stay resident in XCreature::contain the
        // whole time they're worn (see Wear()), which is the item's real
        // owner. This is just an observing reference into that.
        std::weak_ptr<XItem> item;	//main item for this body part;
        std::weak_ptr<XCreature> owner;
        // Raw, not weak: XBodyPart is a strict 1:1 subobject of its owning
        // creature (held in XCreature::components, unique_ptr-owned,
        // destroyed exactly when the creature is) - so, unlike owner above,
        // this is always safe to dereference, including during the
        // creature's own constructor, before it's shared_from_this()-safe
        // (when owner.lock() still fails). Wear() needs this to put an
        // item in contain even from a starting-gear equip loop.
        XCreature* owner_raw = nullptr;
        XBodyPart() : bp_uin() {}
        friend class cereal::access;

    public:
        XBodyPart(XCreature* o, BODY_PART bp);
        ~XBodyPart() {
            item.reset();
            owner.reset();
        }

        const char* GetName() const;
        static const char* XGetName(BODY_PART bp);
        static void Create(XCreature *cr, std::string &str);

        int Fit(const BODY_PART bp) const
        {
            return bp == bp_uin;
        }

        BODY_PART bp_uin;

        XItem* Item() const;
        int Wear(XItem* new_item);
        std::shared_ptr<XItem> UnWear();
        void SetOwner(XCreature* o);

        int GetPartSize() const;
        ITEM_MASK GetProperIM() const;

        // `item` is a weak observing reference into XCreature::contain,
        // the item's real owner (see the comment above) - it resolves
        // correctly here because the owning XCreature's contain is part
        // of the very same archive graph. `owner` is self-referential
        // (it points back at the enclosing XCreature this XBodyPart is a
        // component of) - safe for the same reason proved out in the
        // Phase 2 pilot: Cereal registers a shared_ptr's id before
        // recursing into the pointee's own serialize(), so by the time
        // this runs (nested inside that XCreature's own serialize())
        // the id is already there to resolve against.
        //
        // `owner_raw` is re-derived from `owner` on load only - NOT
        // "harmless to redo on save" as originally assumed: a bodypart
        // whose starting gear was equipped from within its own
        // creature's constructor (before shared_from_this() is safe,
        // see the comment on Wear() below) legitimately has an `owner`
        // that never got bound, even though `owner_raw` was correctly
        // set at the time via SetOwner()'s direct pointer assignment.
        // Redoing this unconditionally on save clobbers that
        // still-correct owner_raw with nullptr the moment a real, live
        // creature's bodyparts are ever serialized for save - a subtle
        // but real corruption bug, not merely a redundant no-op, found
        // via a live *-test-cereal round trip of a whole populated
        // location (a single creature's round trip never happened to
        // include one of these still-unbound bodyparts).
        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(bp_uin, item, owner);

            if constexpr (Archive::is_loading::value) {
                owner_raw = owner.lock().get();
            }
        }
};

#endif
