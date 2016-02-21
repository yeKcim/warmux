/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2007 Wormux Team.
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
 *  Handle the game time. The game can be paused.
 *****************************************************************************/

#include "time.h"
#include <SDL.h>
#include <sstream>
#include <iomanip>
#include <iostream>
#include "graphic/video.h"
#include "interface/game_msg.h"
#include "tool/math_tools.h"
#include "include/app.h"
#include "network/network.h"
#include "team/teams_list.h"
#include "game/game_loop.h"

Time * Time::singleton = NULL;

Time * Time::GetInstance() {
  if (singleton == NULL) {
    singleton = new Time();
  }
  return singleton;
}

bool Time::IsGamePaused() const {
  return is_game_paused;
}

Time::Time(){
  is_game_paused = false;
  delta_t = 20;
  //max_time = 0;
  real_time_game_start = 0;
  real_time_pause_dt = 0;
}

void Time::Reset(){
  current_time = 0;
  is_game_paused = false;
  real_time_game_start = SDL_GetTicks();
  real_time_pause_dt = 0;
  real_time_pause_begin = 0;
}

uint Time::ReadRealTime() {
  return SDL_GetTicks() - real_time_game_start - real_time_pause_dt;
}

uint Time::Read() const{
  return current_time;
}

void Time::Refresh(){
  /*
  TODO : Activate this condition later.
  Refresh time condition :
  - active team is Local 
  - current node is server and game loop is not in Playing state
  - game don't use network
  if((ActiveTeam().IsLocal() || ActiveTeam().IsLocalAI()) ||
     (Network::GetInstance()->IsServer() && GameLoop::GetInstance()->ReadState() != GameLoop::PLAYING) ||
     (!Network::GetInstance()->IsServer() && !Network::GetInstance()->IsClient()) ||
     current_time < max_time)
  */
  current_time += delta_t;
  //RefreshMaxTime(current_time);
}

/*
void Time::RefreshMaxTime(uint updated_max_time){
  if(updated_max_time > max_time)
    max_time = updated_max_time;
}
*/

uint Time::ReadSec() const{
  return Read() / 1000;
}

uint Time::ReadMin() const{
  return ReadSec() / 60;
}

uint Time::GetDelta() const{
  return delta_t;
}

void Time::Pause(){
  if (is_game_paused)
    return;
  is_game_paused = true;
  real_time_pause_begin = SDL_GetTicks();
}

void Time::Continue(){
  assert (is_game_paused);
  is_game_paused = false;
  real_time_pause_dt += SDL_GetTicks() - real_time_pause_begin;
}

uint Time::ClockSec(){
  return ReadSec() % 60;
}

uint Time::ClockMin(){
  return ReadMin() % 60;
}

std::string Time::GetString(){
  std::ostringstream ss;

  ss << ClockMin() << ":" << std::setfill('0') << std::setw(2) << ClockSec();
  return ss.str();
}
