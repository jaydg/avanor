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

#ifndef DICE_H
#define DICE_H

#include <string_view>

class XFile;

// Represents an XdY+Z dice expression: throw a Y-sided die X times, add bonus Z.
class XDice
{
public:
    XDice() = default;

    XDice(const int count, const int sides, const int bonus = 0)
    {
        Setup(count, sides, bonus);
    }

    explicit XDice(const XDice* d)
    {
        Setup(*d);
    }

    // Accepts "XdY+Z" expressions, e.g. "2d6 - 5" or "4d12 + 30"
    explicit XDice(const std::string_view str)
    {
        Setup(str);
    }

    void Setup(int count, int sides, int bonus = 0)
    {
        count_ = count;
        sides_ = sides;
        bonus_ = bonus;
        Throw();
    }

    void Setup(std::string_view str);

    void Setup(const XDice& d)
    {
        Setup(d.count_, d.sides_, d.bonus_);
    }

    [[nodiscard]] int GetCount()  const { return count_; }  // number of dice
    [[nodiscard]] int GetSides()  const { return sides_; }  // sides per die
    [[nodiscard]] int GetBonus()  const { return bonus_; }  // flat bonus/penalty
    [[nodiscard]] int GetResult() const { return result_; }  // last throw result

    // Mutates the flat bonus. Used by callers that adjust damage bonuses.
    void ModifyBonus(int delta) { bonus_ += delta; }
    void SetBonus(int value)    { bonus_ = value;  }

    void Add(const XDice* d)
    {
        count_ += d->count_;
        sides_ = (sides_ + d->sides_) / 2;
        bonus_ += d->bonus_;
    }

    void Add(const int toCount, const int toSides, const int toBonus)
    {
        count_ += toCount;
        sides_ += toSides;
        bonus_ += toBonus;
    }

    int Throw();

    [[nodiscard]] bool isEqual(const XDice* d) const
    {
        return count_ == d->count_ && sides_ == d->sides_ && bonus_ == d->bonus_;
    }

    void Store(const XFile* f) const;
    void Restore(const XFile* f);

    // Generates a value 0-20 with heavily right-skewed distribution:
    //   0 ~ 75%, 1 ~ 5%, ..., 20 ~ 0.1%
    static int DFunc();

    // Normalises DFunc() result to the range (-maximum, +maximum).
    static int NDFunc(int maximum);

    // Throws normalized by DFunc.
    [[nodiscard]] int NThrow() const;

private:
    int result_{}; // last generated result
    int count_{};  // number of dice
    int sides_{};  // sides per die
    int bonus_{};  // flat bonus/penalty
};

#endif
