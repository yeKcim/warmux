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
 * Generic Box that falls from they sky.
 *****************************************************************************/

#include "objbox.h"
#include <sstream>
#include <iostream>
#include "game/game_mode.h"
#include "game/game_loop.h"
#include "game/time.h"
#include "graphic/sprite.h"
#include "include/app.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "map/map.h"
#include "network/randomsync.h"
#include "object/objects_list.h"
#include "team/macro.h"
#include "tool/debug.h"
#include "tool/i18n.h"
#include "tool/resource_manager.h"
#include "weapon/explosion.h"

const uint SPEED = 5; // meter / seconde
const uint SPEED_PARACHUTE = 170; // ms par image
const uint NB_MAX_TRY = 20;

ObjBox::ObjBox(const std::string &name)
  : PhysicalObj(name) {
  m_allow_negative_y = true;

  parachute = true;

  life_points = start_life_points;

  SetSpeed (SPEED, M_PI_2);
  std::cout<<"super called"<<std::endl;
}

ObjBox::~ObjBox(){
  delete anim;
  GameLoop::GetInstance()->SetCurrentBox(NULL);
}

// Say hello to the ground
void ObjBox::SignalCollision()
{
  SetAirResistFactor(1.0);
  GameLoop::GetInstance()->SetCurrentBox(NULL);
  MSG_DEBUG("box", "End of the fall: parachute=%d", parachute);
  if (!parachute) return;

  MSG_DEBUG("box", "Start of the animation 'fold of the parachute'.");
  parachute = false;
  anim->SetCurrentFrame(0);
  anim->Start();
}

void ObjBox::SignalDrowning()
{
  SignalCollision();
}

void ObjBox::DropBox()
{
  if(parachute) {
    SetAirResistFactor(1.0);
    parachute = false;
    anim->SetCurrentFrame(anim->GetFrameCount() - 1);
  } else {
    m_ignore_movements = true;
  }
}

//Boxes can explode...
void ObjBox::SignalGhostState(bool was_already_dead)
{
  if(life_points > 0) return;
  ParticleEngine::AddNow(GetCenter() , 10, particle_FIRE, true);
  ApplyExplosion(GetCenter(), GameMode::GetInstance()->bonus_box_explosion_cfg); //reuse the bonus_box explosion
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Static methods
bool ObjBox::enable = true;
int ObjBox::start_life_points = 41;

// Make the box active?
void ObjBox::Enable (bool _enable)
{
  MSG_DEBUG("box", "Enable ? %d", _enable);
  enable = _enable;
}

bool ObjBox::NewBox()
{
  if (!enable) { // Boxes are disabled on closed map
    return false;
  }

  uint nbr_teams=teams_list.playing_list.size();
  if(nbr_teams<=1) {
    MSG_DEBUG("box", "There is less than 2 teams in the game");
    return false;
  }
  // .7 is a magic number to get the probability of boxes falling once every round close to .333
  double randValue = randomSync.GetDouble();
  if(randValue > (1-pow(.5,1.0/nbr_teams))) {
       return false;
  }

  ObjBox * box;
  if(randomSync.GetBool())
    box = new Medkit();
  else
    box = new BonusBox();
  if(!box->PutRandomly(true,0)) {
    MSG_DEBUG("box", "Missed to put a box");
    delete box;
  } else {
    lst_objects.AddObject(box);
    camera.FollowObject(box, true, true);
    GameMessages::GetInstance()->Add (_("It's a present!"));
    GameLoop::GetInstance()->SetCurrentBox(box);
    return true;
  }

  return false;
}
