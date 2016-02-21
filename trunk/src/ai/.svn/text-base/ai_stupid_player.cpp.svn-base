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
 * An AI player for a team.
 *****************************************************************************/

#include <WARMUX_random.h>
#include "ai/ai_command.h"
#include "ai/ai_idea.h"
#include "ai/ai_strategy.h"
#include "ai/ai_stupid_player.h"
#include "character/character.h"
#include "game/game.h"
#include "game/stopwatch.h"
#include "game/game_time.h"
#include "team/macro.h"

const uint MAX_GAME_TIME_USED_THINKING_IN_MS = 10000;
const uint REAL_THINK_TIME_PER_REFRESH_IN_MS = 1;
#define  MAX_GUN_DISTANCE               600.0f
#define  MAX_SHOTGUN_DISTANCE           250.0f
#define  MAX_SNIPER_RIFILE_DISTANCE   46000.0f // squared to int at some point => overflow!
#define  MAX_SUBMACHINE_GUN_DISTANCE    500.0f

//#define DBG_AI_TIME

class AIStats {
  const char *name;
  uint64_t time;
  uint64_t sq_time;
  uint     calls;
  uint     min;
  uint     max;

public:
  AIStats(const char *n) : name(n), time(0), sq_time(0), calls(0), min(0xFFFFFFFFU), max(0) { };
  ~AIStats()
  {
#ifdef DBG_AI_TIME
    if (calls) {
      float avg = time/(float)calls;
      printf("Strategy '%s': calls=%u  total=%llums  min=%ums  max=%ums  avg=%.2fms  stddev=%.3fms\n",
             name, calls, time, min, max, avg, sqrt( (sq_time/(float)calls) - avg*avg ));
    } else {
      printf("Strategy '%s': not called\n", name);
    }
#endif
  }

#ifdef DBG_AI_TIME
  void AddTiming(uint t)
  {
    if (t < min)
      min = t;
    if (t > max)
      max = t;
    time += t;
    sq_time += t*t;
    calls++;
  }
#else
  void AddTiming(uint) { };
#endif
};

class AllStats : public Singleton<AllStats>
{
public:
  AIStats SkipTurn, WasteAmmo, ShootDirectly, WeaponLauncher;


  friend class Singleton<AllStats>;
  AllStats()
    : SkipTurn("SkipTurn")
    , WasteAmmo("WasteAmmo")
    , ShootDirectly("ShootDirectly")
    , WeaponLauncher("WeaponLauncher")
  { }
};

bool AIStupidPlayer::CompareIdeaMaxRating(const AIItem& i1, const AIItem& i2)
{
  return i1.first->GetMaxRating(false) > i2.first->GetMaxRating(false);
}

AIStupidPlayer::AIStupidPlayer(Team * team, float accuracy)
  : team(team)
  , accuracy(accuracy)
  , item_iterator(items.begin())
  , command(NULL)
  , best_strategy(NULL)
{
  AllStats *stats = AllStats::GetInstance();
  items.push_back(std::make_pair(new SkipTurnIdea(), &stats->SkipTurn));
  items.push_back(std::make_pair(new WasteAmmoUnitsIdea(), &stats->WasteAmmo));
  FOR_EACH_LIVING_AND_DEAD_CHARACTER(team, character) {
    FOR_EACH_TEAM(other_team) {
      bool is_enemy = !team->IsSameAs(**other_team);
      if (is_enemy) {
        FOR_EACH_LIVING_AND_DEAD_CHARACTER(*other_team, other_character) {
          items.push_back(std::make_pair(new ShootDirectlyAtEnemyIdea(weapons_weighting, *character, *other_character,
                                                                      Weapon::WEAPON_GUN, MAX_GUN_DISTANCE),
                                         &stats->ShootDirectly));
          items.push_back(std::make_pair(new ShootDirectlyAtEnemyIdea(weapons_weighting, *character, *other_character,
                                                                      Weapon::WEAPON_SHOTGUN, MAX_SHOTGUN_DISTANCE),
                                         &stats->ShootDirectly));
          items.push_back(std::make_pair(new ShootDirectlyAtEnemyIdea(weapons_weighting, *character, *other_character,
                                                                      Weapon::WEAPON_SNIPE_RIFLE, MAX_SNIPER_RIFILE_DISTANCE),
                                         &stats->ShootDirectly));
          items.push_back(std::make_pair(new FireMissileWithFixedDurationIdea(weapons_weighting, *character, *other_character,
                                                                              Weapon::WEAPON_BAZOOKA, 0.5f),
                                         &stats->WeaponLauncher));
          items.push_back(std::make_pair(new FireMissileWithFixedDurationIdea(weapons_weighting, *character, *other_character,
                                                                              Weapon::WEAPON_BAZOOKA, 0.9f),
                                         &stats->WeaponLauncher));
          items.push_back(std::make_pair(new FireMissileWithFixedDurationIdea(weapons_weighting, *character, *other_character,
                                                                              Weapon::WEAPON_BAZOOKA, 2.0f),
                                         &stats->WeaponLauncher));
          items.push_back(std::make_pair(new FireMissileWithFixedDurationIdea(weapons_weighting, *character, *other_character,
                                                                              Weapon::WEAPON_GRENADE, 1.2f, 2),
                                         &stats->WeaponLauncher));
          items.push_back(std::make_pair(new FireMissileWithFixedDurationIdea(weapons_weighting, *character, *other_character,
                                                                              Weapon::WEAPON_DISCO_GRENADE, 1.2f, 2),
                                         &stats->WeaponLauncher));
        }
      }
    }
  }
  items.sort(CompareIdeaMaxRating);
}

AIStupidPlayer::~AIStupidPlayer()
{
  if (command)
    delete command;
  if (best_strategy)
    delete best_strategy;
  for (item_iterator = items.begin(); item_iterator != items.end(); item_iterator++)
    delete (*item_iterator).first;
}

void AIStupidPlayer::PrepareTurn()
{
  Reset();
  weapons_weighting.RandomizeFactors();

  std::list<AIItem>::iterator it = items.begin();
  while (it != items.end()) {
    if (it->first->NoLongerPossible()) {
      delete it->first;
      it = items.erase(it);
    } else
      ++it;
  }
  item_iterator = items.begin();
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
  item_iterator = items.begin();
  game_time_at_turn_start = GameTime::GetInstance()->Read();
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
  uint now = GameTime::GetInstance()->Read();
  bool is_thinking = (command == NULL);
  if (is_thinking) {
    bool think_time_over = now >= game_time_at_turn_start + MAX_GAME_TIME_USED_THINKING_IN_MS;
    if (!think_time_over) {
      Stopwatch stopwatch;
      while (stopwatch.GetValue() < REAL_THINK_TIME_PER_REFRESH_IN_MS && item_iterator != items.end()) {
        CheckNextIdea();
      }
    }
    bool no_more_items = (item_iterator == items.end());
    if (think_time_over || no_more_items) {
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
  AIIdea* idea = (*item_iterator).first;
  float rating = idea->GetMaxRating(true);
  if (rating < best_strategy->GetRating()) {
    // All following strategies are going to be less effective, abort search
    item_iterator = items.end();
    return;
  }

  Stopwatch stopwatch;
  AIStrategy * strategy = idea->CreateStrategy(accuracy);
  (*item_iterator).second->AddTiming(stopwatch.GetValue());
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
  item_iterator++;
}
