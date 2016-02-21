/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 ******************************************************************************
 * A factory for AICommands. It contains turn specfic data unlike AIIdea.
 * Unlike an AICommand an AIStrategy has a rating and has a much simpler structure.
 *****************************************************************************/

#ifndef AI_STRATEGY_H
#define AI_STRATEGY_H

#include "ai/ai_command.h"

class AIStrategy
{
  const float rating;
public:
  typedef enum {LOWER_RATING, SIMILAR_RATING, HIGHER_RATING} CompareResult;
  AIStrategy(float rating);
  virtual ~AIStrategy() {}
  /** Creates a command which implements the strategy */
  virtual AICommand * CreateCommand() const = 0;
  float GetRating() const { return rating; }
  CompareResult CompareRatingWith(AIStrategy * other) const;
};

class DoNothingStrategy : public AIStrategy
{
public:
  DoNothingStrategy();
  virtual AICommand * CreateCommand() const;
};

class SkipTurnStrategy : public AIStrategy
{
public:
  SkipTurnStrategy();
  virtual AICommand * CreateCommand() const;
};

class ShootWithGunStrategy : public AIStrategy
{
  const Character & shooter;
  Weapon::Weapon_type weapon;
  LRDirection  direction;
  float angle;
  int bullets;
public:
  virtual AICommand * CreateCommand() const;
  ShootWithGunStrategy(float rating, const Character & shooter,
                       Weapon::Weapon_type weapon, LRDirection  direction,
                       float angle, int bullets);
};

class LoadAndFireStrategy : public AIStrategy
{
  const Character & shooter;
  Weapon::Weapon_type weapon;
  LRDirection  direction;
  float angle;
  float strength;
  int timeout;
public:
  virtual AICommand * CreateCommand() const;
  LoadAndFireStrategy(float rating, const Character & shooter,
                      Weapon::Weapon_type weapon, LRDirection direction,
                      float angle, float strength, int timeout);
};


#endif
