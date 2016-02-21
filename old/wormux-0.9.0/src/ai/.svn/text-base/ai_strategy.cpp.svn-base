/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Wormux Team.
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
 * An AI player for a team.
 *****************************************************************************/

#include "ai/ai_strategy.h"

const uint RATING_EPSILON = 0.00001;
const uint WATCH_MISSILE_TIME_IN_MS = 2000;

AIStrategy::AIStrategy(float rating):
rating(rating)
{
  // do nothing
}

AIStrategy::CompareResult AIStrategy::CompareRatingWith(AIStrategy * other)
{
  if (this->GetRating() + RATING_EPSILON < other->GetRating())
    return AIStrategy::LOWER_RATING;

  if (this->GetRating() - LOWER_RATING > other->GetRating())
    return AIStrategy::HIGHER_RATING;

  return AIStrategy::SIMILAR_RATING;
}

DoNothingStrategy::DoNothingStrategy():
AIStrategy(-1.0f) // It's worse then skipping the turn (rating 0) as the human needs to wait.
{
  // do nothing
}

AICommand * DoNothingStrategy::CreateCommand()
{
  return new DoNothingForeverCommand;
}

SkipTurnStrategy::SkipTurnStrategy():
AIStrategy(0.0f)
{
  // do nothing
}

AICommand * SkipTurnStrategy::CreateCommand()
{
  CommandList * commands = new CommandList();
  commands->Add(new SelectWeaponCommand(Weapon::WEAPON_SKIP_TURN));
  commands->Add(new StartShootingCommand());
  commands->Add(new StopShootingCommand());
  return commands;
}

static CommandList * CreateSelectCommandList(Character & character, Weapon::Weapon_type weapon, LRDirection  direction, double angle, int timeout = -1)
{
  CommandList * commands = new CommandList();
  commands->Add(new SelectCharacterCommand(&character));
  commands->Add(new DoNothingCommand(1000));
  commands->Add(new SelectWeaponCommand(weapon));
  commands->Add(new DoNothingCommand(100));
  if (timeout > 0) {
    commands->Add(new SetTimeoutCommand(timeout));
  }
  commands->Add(new DoNothingCommand(100));
  commands->Add(new SetDirectionCommand(direction));
  commands->Add(new SetWeaponAngleCommand(angle));
  commands->Add(new DoNothingCommand(200));
  return commands;
}

ShootWithGunStrategy::ShootWithGunStrategy(double rating, Character & shooter, Weapon::Weapon_type weapon, LRDirection  direction, double angle, int bullets):
AIStrategy(rating),
shooter(shooter),
weapon(weapon),
direction(direction),
angle(angle),
bullets(bullets)
{
  // do nothing
}

AICommand * ShootWithGunStrategy::CreateCommand()
{
  CommandList * commands = CreateSelectCommandList(shooter, weapon, direction, angle);
  for (int i = 0; i < bullets; i++) {
    if (i != 0)
      commands->Add(new DoNothingCommand(1000));
    commands->Add(new StartShootingCommand());
    commands->Add(new StopShootingCommand());
  }
  commands->Add(new DoNothingCommand(WATCH_MISSILE_TIME_IN_MS));
  return commands;
}

LoadAndFireStrategy::LoadAndFireStrategy(double rating, Character & shooter, Weapon::Weapon_type weapon, LRDirection  direction, double angle, double strength, int timeout):
AIStrategy(rating),
shooter(shooter),
weapon(weapon),
direction(direction),
angle(angle),
strength(strength),
timeout(timeout)
{
  // do nothing
}

AICommand * LoadAndFireStrategy::CreateCommand()
{
  CommandList * commands = CreateSelectCommandList(shooter, weapon, direction, angle, timeout);
  commands->Add(new StartShootingCommand());
  commands->Add(new WaitForStrengthCommand(strength));
  commands->Add(new StopShootingCommand());
  commands->Add(new DoNothingForeverCommand());
  return commands;
}
