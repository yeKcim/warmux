/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
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
 * Refresh du temps qui passe. Le temps du jeu peut être mise en pause.
 *****************************************************************************/

#include "time.h"
#include <SDL.h>
#include <sstream>
#include <iomanip>
#include <iostream>
#include "../graphic/video.h"
#include "../interface/game_msg.h"
#include "../tool/math_tools.h"

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
  pause_offset = 0;
  is_game_paused = false;
}

void Time::Reset(){
  pause_offset = SDL_GetTicks();
  is_game_paused = false;
}

uint Time::Read() const{
  return SDL_GetTicks() - pause_offset;
}

uint Time::ReadSec() const{
  return Read() / 1000;
}

uint Time::ReadMin() const{
  return ReadSec() / 60;
}

void Time::Pause(){
  if (is_game_paused)
    return;
  pause_start = SDL_GetTicks();
  is_game_paused = true;
}

void Time::Continue(){
  assert (is_game_paused);
  pause_offset += SDL_GetTicks() - pause_start;
  is_game_paused = false;
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
