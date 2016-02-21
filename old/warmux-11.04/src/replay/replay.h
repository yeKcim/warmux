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
 *****************************************************************************
 * Replay engine for Warmux parading as network layer
 * @todo Evaluate rate generation and determine wheter gziping is interesting
 *****************************************************************************/

#ifndef REPLAY_H
#define REPLAY_H

#include <vector>

#include <WARMUX_base.h>
#include <WARMUX_singleton.h>

#include "game_mode_info.h"

class Action;
class Team;

class Replay : public Singleton<Replay>
{
public:
  typedef enum
  {
    NOTHING,
    PLAYING,
    PAUSED_PLAY,
    RECORDING,
    PAUSED_RECORD
  } replay_state_t;

 private:
  uint8_t           *buf;
  uint8_t           *ptr;
  uint               bufsize;
  uint               seed;
  // Number of calculate frame actions seen so far
  uint               count;

  // State
  replay_state_t     replay_state;
  bool               is_recorder;

  // For recording
  uint32_t           start_time;
  uint32_t           old_time;

  // For replaying: GameMode save
  bool               config_loaded;
  GameModeInfo       mode_info;
  std::vector<Team*> backup_list;

  void ChangeBufsize(uint32_t n);
  uint32_t MemUsed() { return ptr - buf; }

protected:
  friend class Singleton<Replay>;
  Replay();
  ~Replay();

public:
  void Init(bool is_rec);
  void DeInit();

  // Seed access
  void SetSeed(uint s) { seed = s; }
  uint GetSeed() const { return seed; }

  uint GetDuration() const { return old_time; }

  // Recorder
  bool StartRecording(const std::string& game_mode_name,
                      const std::string& game_mode,
                      const std::string& game_mode_objects);
  void StopRecording() { replay_state = NOTHING; }
  void StoreAction(const Action* a);
  bool IsRecording() const { return is_recorder && replay_state==RECORDING; }
  bool SaveReplay(const std::string& name, const char *comment = NULL);

  // Replayer
  bool  LoadReplay(const std::string& name);
  bool  RefillActions();
  Action* GetAction();
  bool StartPlaying();
  void StopPlaying();
  bool IsPlaying() const { return !is_recorder && replay_state==PLAYING; }
};

void StartPlaying(const std::string& name);

#endif //REPLAY_H
