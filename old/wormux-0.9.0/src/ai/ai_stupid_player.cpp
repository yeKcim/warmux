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

#include <WORMUX_random.h>
#include "ai/ai_stupid_player.h"
#include "character/character.h"
#include "game/game.h"
#include "game/stopwatch.h"
#include "game/time.h"
#include "team/macro.h"

const uint MAX_GAME_TIME_USED_THINKING_IN_MS = 10000;
const uint REAL_THINK_TIME_PER_REFRESH_IN_MS = 1;
const double MAX_GUN_DISTANCE = 600;
const double MAX_SHOTGUN_DISTANCE = 250;
const double MAX_SNIPER_RIFILE_DISTANCE = 10E10;
const double MAX_SUBMACHINE_GUN_DISTANCE = 500;

AIStupidPlayer::AIStupidPlayer(Team * team):
  team(team),
  idea_iterator(ideas.begin()),
  command(NULL),
  best_strategy(NULL)
{
  ideas.push_back(new SkipTurnIdea());
  ideas.push_back(new WasteAmmoUnitsIdea());
  FOR_EACH_LIVING_AND_DEAD_CHARACTER(team, character) {
    FOR_EACH_TEAM(other_team) {
      bool is_enemy = !team->IsSameAs(**other_team);
      if (is_enemy) {
        FOR_EACH_LIVING_AND_DEAD_CHARACTER(*other_team, other_character) {
          ideas.push_back(new ShootDirectlyAtEnemyIdea(weapons_weighting, *character, *other_character, Weapon::WEAPON_GUN, MAX_GUN_DISTANCE));
          ideas.push_back(new ShootDirectlyAtEnemyIdea(weapons_weighting, *character, *other_character, Weapon::WEAPON_SHOTGUN, MAX_SHOTGUN_DISTANCE));
          ideas.push_back(new ShootDirectlyAtEnemyIdea(weapons_weighting, *character, *other_character, Weapon::WEAPON_SNIPE_RIFLE, MAX_SNIPER_RIFILE_DISTANCE));
          ideas.push_back(new FireMissileWithFixedDurationIdea(weapons_weighting, *character, *other_character, Weapon::WEAPON_BAZOOKA, 0.9));
          ideas.push_back(new FireMissileWithFixedDurationIdea(weapons_weighting, *character, *other_character, Weapon::WEAPON_BAZOOKA, 1.8));
          ideas.push_back(new FireMissileWithFixedDurationIdea(weapons_weighting, *character, *other_character, Weapon::WEAPON_GRENADE, 2.01, 2));
          ideas.push_back(new FireMissileWithFixedDurationIdea(weapons_weighting, *character, *other_character, Weapon::WEAPON_DISCO_GRENADE, 2.01, 2));
          ideas.push_back(new FireMissileWithFixedDurationIdea(weapons_weighting, *character, *other_character, Weapon::WEAPON_BAZOOKA, 3.0));
        }
      }
    }
  }
}

AIStupidPlayer::~AIStupidPlayer()
{
  if (command)
    delete command;
  if (best_strategy)
    delete best_strategy;
  for (idea_iterator = ideas.begin(); idea_iterator != ideas.end(); idea_iterator++)
    delete (*idea_iterator);
}

void AIStupidPlayer::PrepareTurn()
{
  Reset();
  weapons_weighting.RandomizeFactors();
}

void AIStupidPlayer::Reset()
{
  if (command) {
    delete command;
    command = NULL;
  }
  command_executed = false;
  if (best_strategy)
    delete best_strategy;
  best_strategy = new DoNothingStrategy();
  best_strategy_counter = 1;
  idea_iterator = ideas.begin();
  game_time_at_turn_start = Time::GetInstance()->Read();
}

void AIStupidPlayer::Refresh()
{
  if (&ActiveTeam() != team)
    return;
  if (Game::GetInstance()->ReadState() == Game::END_TURN)
    return;
  if (ActiveCharacter().IsDead())
    return;
  if (command_executed)
    return;
  uint now = Time::GetInstance()->Read();
  bool is_thinking = (command == NULL);
  if (is_thinking) {
    bool think_time_over = now >= game_time_at_turn_start + MAX_GAME_TIME_USED_THINKING_IN_MS;
    if (!think_time_over) {
      Stopwatch stopwatch;
      while(stopwatch.GetValue() < REAL_THINK_TIME_PER_REFRESH_IN_MS && idea_iterator != ideas.end()) {
        CheckNextIdea();
      }
    }
    bool no_more_ideas = (idea_iterator == ideas.end());
    if (think_time_over || no_more_ideas) {
      command = best_strategy->CreateCommand();
    }
  }
  if (command) {
    if (command->Execute())
      Reset();
  }
}

void AIStupidPlayer::CheckNextIdea()
{
  AIStrategy * strategy = (*idea_iterator)->CreateStrategy();
  if (strategy) {
    AIStrategy::CompareResult compare_result = strategy->CompareRatingWith(best_strategy);
    bool replace_best;
    if (compare_result != AIStrategy::LOWER_RATING) {
      if (compare_result == AIStrategy::SIMILAR_RATING) {
        best_strategy_counter++;
        // Of all found best strategies one gets randomly choosen.
        // Example:
        // There are 4 strategies with rating 5: a, b, c and d.
        // First a gets choosen with 1/1 = 100% propablity.
        // Then there is a 50% chance that b replaces a.
        // Afterwards c replaces a or b with 1/3 properblity and a 2/3 properblity that a or b stays choosen.
        // Thus a, b and c have a 1/3 properblity to be now the current best strategy
        // when there is a 3/4 chance that they don't get replaced by d.
        // At the end all four strategies had a 1/4 (25%) chance to be picked.
        replace_best = RandomLocal().GetInt(1, best_strategy_counter) == 1;
      } else {
        ASSERT(compare_result == AIStrategy::HIGHER_RATING);
        best_strategy_counter = 1;
        replace_best = true;
      }
    } else {
      replace_best = false;
    }
    if (replace_best) {
      delete best_strategy;
      best_strategy = strategy;
    } else {
      delete strategy;
    }
  }
  idea_iterator++;
}
