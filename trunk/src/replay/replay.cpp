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
 * Replay engine for Wormux parading as network layer
 *****************************************************************************/

#include "replay.h"

#include <SDL_net.h>
#include <assert.h>
#ifdef _WIN32
#  include <wchar.h>
#endif

#include <WARMUX_debug.h>

#include "replay_info.h"
#include "game/game.h"
#include "game/game_mode.h"
#include "game/config.h"
#include "game/game_time.h"
#include "map/maps_list.h"
#include "include/action_handler.h"
#include "include/constant.h"
#include "game/game_time.h"
#include "sound/jukebox.h"
#include "team/teams_list.h"

Replay::Replay()
  : buf(NULL)
  , bufsize(0)
  , is_recorder(true)
{
  DeInit();
}

Replay::~Replay()
{
  DeInit();
  if (buf)
    free(buf);
}

void Replay::Init(bool rec)
{
  DeInit();

  is_recorder = rec;
  replay_state = (rec) ? PAUSED_RECORD : PAUSED_PLAY;
}

void Replay::DeInit()
{
  ptr = buf;
  config_loaded = false;
  old_time = 0;

  replay_state = NOTHING;
}

void Replay::ChangeBufsize(uint32_t n)
{
  if (n <= bufsize)
    return;

  // All data is supposed to be consumed
  uint32_t offset = (bufsize) ? MemUsed() : 0;
  buf = (uint8_t*)realloc(buf, n);
  bufsize = n;
  ptr = buf + offset;
}

bool Replay::StartRecording(const std::string& game_mode_name,
                            const std::string& game_mode,
                            const std::string& game_mode_objects)
{
  MSG_DEBUG("replay", "Asked to start recording\n");
  ASSERT(is_recorder && replay_state == PAUSED_RECORD);

  replay_state = RECORDING;
  start_time   = 0;
  old_time     = 0;

  // Write game mode rules at start of data
  uint total_size = game_mode_name.size() + game_mode.size()
                  + game_mode_objects.size() + 3*2;
  ChangeBufsize(total_size+30000); // twice the needed size

  Uint16 size = game_mode_name.size();
  SDLNet_Write16(size, ptr); ptr += 2;
  memcpy(ptr, game_mode_name.c_str(), size); ptr += size;

  size = game_mode.size();
  SDLNet_Write16(size, ptr); ptr += 2;
  memcpy(ptr, game_mode.c_str(), size); ptr += size;

  size = game_mode_objects.size();
  SDLNet_Write16(size, ptr); ptr += 2;
  memcpy(ptr, game_mode_objects.c_str(), size); ptr += size;

  MSG_DEBUG("replay", "Wrote game mode on %u bytes\n", total_size);
  count = 0;

  return true;
}

bool Replay::SaveReplay(const std::string& name, const char *comment)
{
  ASSERT(is_recorder);

  FILE *out;
#ifdef _WIN32 // Only case where is_wide is true
  out = _wfopen((wchar_t*)name.c_str(), L"wb");
#else
  out = fopen(name.c_str(), "wb");
#endif
  if (!out)
    return false;

  // Generate replay info and dump it to file
  uint32_t total_time = old_time - start_time;
  ReplayInfo *info = ReplayInfo::ReplayInfoFromCurrent(total_time, comment);
  if (!info->DumpToFile(out)) {
    delete info;
    return false;
  }
  delete info;

  // Save seed
  Write32(out, seed);

  // Flush actions recorded
#ifdef WMX_LOG
  uint32_t pos = ftell(out);
  MSG_DEBUG("replay", "Actions stored at %u on %u bytes in %s, seed %08X\n",
            pos, MemUsed(), name.c_str(), seed);
#endif

  fwrite(buf, 1, MemUsed(), out);
  if (count) {
    count--;
    MSG_DEBUG("replay", "Storing final calculate frames: %u\n", count);
    Write32(out, count);
  }

  bool good = !ferror(out);
  fclose(out);

  // should maybe return length actually written
  return good;
}

// The Replay packet header:
// u32: time (ms => 2^16=65s => u16 sufficient)
// Packet is directly an action:
//   . u32 => type
//   . u32 => length
//   . data
void Replay::StoreAction(const Action* a)
{
  uint          size;

  ASSERT(is_recorder && replay_state==RECORDING);

  Action::Action_t type = a->GetType();
  if ((type!=Action::ACTION_CHAT_INGAME_MESSAGE && a->IsFrameLess()) ||
      type == Action::ACTION_NETWORK_PING ||
      type == Action::ACTION_NETWORK_VERIFY_RANDOM_SYNC ||
      type == Action::ACTION_TIME_VERIFY_SYNC ||
      type == Action::ACTION_RULES_SET_GAME_MODE)
    return;

  // Special case to convert into local packet
  if (type == Action::ACTION_REQUEST_BONUS_BOX_DROP) {
    // The timestamp shouldn't have moved
    Action a(Action::ACTION_DROP_BONUS_BOX);
    StoreAction(&a);
    return;
  }

  size = a->GetSize();
  // Enlarge buffer if it can't contain max packet size
  if (MemUsed() > bufsize - size + 2)
    ChangeBufsize(bufsize+30000);

  if (type != Action::ACTION_GAME_CALCULATE_FRAME) {
    if (count) {
      count--;
      MSG_DEBUG("replay", "Calculate frame repeated %u\n", count);
      // 16 bits is sufficient, around 22 minutes without other actions
      SDLNet_Write16(count, ptr); ptr += 2;
    }
    MSG_DEBUG("replay", "Storing action %s: type=%i length=%i\n",
              ActionHandler::GetActionName(type).c_str(), type, size);
    a->Write((char*)ptr);
    ptr += size;
    count = 0;
  } else {
    if (!count) {
      // Packet body
      a->Write((char*)ptr);
      ptr += size;
    }
    count++;
  }

  // Check time
  if (start_time == 0)
    start_time = GameTime::GetInstance()->Read();
  else
    old_time = GameTime::GetInstance()->Read();
}


/////////////////////////////////////////////////////////////////////////////


bool Replay::LoadReplay(const std::string& name)
{
#define TEMP_SIZE 256
  char           temp[TEMP_SIZE];
  bool           status     = false;
  std::streampos pos;
  ReplayInfo     *info      = NULL;
  GameMode       *game_mode = GameMode::GetInstance();
  int            map_id, val;

  ASSERT(!is_recorder);
  old_time = 0;

  FILE *in = fopen(name.c_str(), "rb");
  if (!in) {
    Error(Format(_("Couldn't open %s\n"), name.c_str()));
    goto done;
  }

  info = ReplayInfo::ReplayInfoFromFile(in);
  if (!info->IsValid())
    goto done;

  if (info->GetVersion() != Constants::WARMUX_VERSION) {
    Error(Format(_("Bad version: %s != %s"),
                 info->GetVersion().c_str(),
                 Constants::WARMUX_VERSION.c_str()));
    goto done;
  }
  goto ok;

err:
  Error(Format(_("Warning, malformed replay with data of size %u"), bufsize));

done:
  fclose(in);
  if (info) delete info;
  return status;

ok:
  // duration
  old_time = info->GetMillisecondsDuration();

  // map ID
  map_id = MapsList::GetInstance()->FindMapById(info->GetMapId());
  if (map_id == -1) {
    Error(Format(_("Couldn't find map %s"), temp));
    return false;
  }
  MapsList::GetInstance()->SelectMapByIndex(map_id);

  // Backup playing list
  TeamsList& teams_list = GetTeamsList();
  backup_list = teams_list.playing_list;
  teams_list.playing_list.clear();

  // Teams
  for (uint i = 0; i<info->GetTeams().size(); i++) {
    ConfigTeam team_cfg;
    teams_list.AddTeam(info->GetTeams()[i], true);
  }

  // Game mode
  memcpy(&mode_info, info->GetGameModeInfo(), sizeof(GameModeInfo));

  // Set GameMode
  val = mode_info.allow_character_selection;
  mode_info.allow_character_selection = game_mode->allow_character_selection;
  game_mode->allow_character_selection = (GameMode::manual_change_character_t)val;
#define SWAP(a, b) val = a; a = b; b = val
  SWAP(mode_info.turn_duration, game_mode->duration_turn);
  SWAP(mode_info.duration_before_death_mode, game_mode->duration_before_death_mode);
  SWAP(mode_info.damage_per_turn_during_death_mode, game_mode->damage_per_turn_during_death_mode);
  SWAP(mode_info.init_energy, game_mode->character.init_energy);
  SWAP(mode_info.max_energy, game_mode->character.max_energy);
  SWAP(mode_info.gravity, game_mode->gravity);

  MSG_DEBUG("replay", "Game mode: turn=%us move_player=%u max_energy=%u init_energy=%u\n",
            game_mode->duration_turn, game_mode->duration_move_player,
            game_mode->character.max_energy, game_mode->character.init_energy);

  // All of the above could be avoided through a GameMode::Load
  config_loaded = true;

  seed = Read32(in);

  // Get remaining data
  pos = ftell(in);
  fseek(in, 0, SEEK_END);
  uint size = ftell(in)-pos;
  fseek(in, pos, SEEK_SET);
  MSG_DEBUG("replay", "Actions found at %u on %uB, seed=%08X\n", (uint)pos, size, seed);

  // Explicit buffer change to avoid garbage
  if (buf)
    free(buf);
  buf = (uint8_t*)malloc(size);
  ptr = buf;
  bufsize = size;

  if (fread(buf, size, 1, in)!=1 || ferror(in))
    goto err;

  size = SDLNet_Read16(ptr); ptr += 2;
  std::string mode_name((char*)ptr, size); ptr += size;

  size = SDLNet_Read16(ptr); ptr += 2;
  std::string mode((char*)ptr, size); ptr += size;

  size = SDLNet_Read16(ptr); ptr += 2;
  std::string mode_objects((char*)ptr, size); ptr += size;

  game_mode->LoadFromString(mode_name, mode, mode_objects);
  status = true;

  goto done;
}

// Only use is internal, but let those parameters be available
Action* Replay::GetAction()
{
  ASSERT(!is_recorder && replay_state == PLAYING);

  // Does it contain the 2 elements needed to decode at least
  // action header?
  if (MemUsed() > bufsize-sizeof(Action::Header)) {
    return NULL;
  }

  // Read action
  Action *a = new Action((char*)ptr, NULL);
  Action::Action_t type = a->GetType();
  if (type > Action::ACTION_TIME_VERIFY_SYNC) {
    Error(Format(_("Malformed replay: action with unknow type %08X"), type));
    StopPlaying();
    return NULL;
  }

  // Move pointer
  uint size = a->GetSize();
  if (MemUsed() > bufsize-size) {
    Error(Format(_("Malformed replay: action with datasize=%u"), size));
    StopPlaying();
    return NULL;
  }
  ptr += size;

  MSG_DEBUG("replay", "Read action %s: type=%u length=%i frameless=%i\n",
            ActionHandler::GetActionName(type).c_str(), type, size, a->IsFrameLess());

  return a;
}

bool Replay::RefillActions()
{
  ActionHandler *ah = ActionHandler::GetInstance();

  ah->Lock();
  while (1) {
    Action *a = GetAction();
    if (a) {
      ah->NewAction(a, false);
      if (a->GetType() == Action::ACTION_GAME_CALCULATE_FRAME) {
        // We write the number of calculate frame actions
        count = SDLNet_Read16(ptr); ptr += 2;
        MSG_DEBUG("replay", "Repeating %u\n", count);
        for (uint i=0; i<count; i++) {
          Action *a = new Action(Action::ACTION_GAME_CALCULATE_FRAME);
          ah->NewAction(a, false);
        }
      }
    } else
      break;
  }
  ah->UnLock();

  return true;
}

bool Replay::StartPlaying()
{
  ASSERT(!is_recorder && replay_state == PAUSED_PLAY);

  replay_state = PLAYING;

  return RefillActions();
}

void Replay::StopPlaying()
{
  ASSERT(!is_recorder);

  if (replay_state != PLAYING)
    return;

  // Only replay seems to use this, so we can quit it now
  replay_state = NOTHING;

  // Restore game mode
  GameMode * game_mode = GameMode::GetInstance();
  game_mode->allow_character_selection = (GameMode::manual_change_character_t)
    mode_info.allow_character_selection;
  game_mode->duration_turn = mode_info.turn_duration;
  game_mode->damage_per_turn_during_death_mode = mode_info.damage_per_turn_during_death_mode;
  game_mode->duration_before_death_mode = mode_info.duration_before_death_mode;
  game_mode->character.init_energy = mode_info.init_energy;
  game_mode->character.max_energy = mode_info.max_energy;
  game_mode->gravity = mode_info.gravity;

  // Restore also volume
  JukeBox::GetInstance()->SetMusicVolume(Config::GetInstance()->GetVolumeMusic());

  // Restore playing list
  GetTeamsList().SetPlayingList(backup_list);
}

void StartPlaying(const std::string& name)
{
  Replay *replay = Replay::GetInstance();

  replay->Init(false);
  if (replay->LoadReplay(name.c_str())) {
    Double speed = GameTime::GetInstance()->GetSpeed();
    if (replay->StartPlaying()) {
      Game::GetInstance()->Start();
      replay->StopPlaying();
      GameTime::GetInstance()->SetSpeed(speed);
    }
  }
  replay->DeInit();
}
