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

#include "object/objbox.h"
#include "object/medkit.h"
#include "object/bonus_box.h"
#include <sstream>
#include <iostream>
#include "game/game_mode.h"
#include "game/game.h"
#include "game/time.h"
#include "graphic/sprite.h"
#include "include/app.h"
#include "include/action.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "map/map.h"
#include "network/randomsync.h"
#include "object/objects_list.h"
#include "team/macro.h"
#include "team/team.h"
#include "tool/debug.h"
#include "tool/i18n.h"
#include "tool/resource_manager.h"
#include "weapon/explosion.h"
#include "character/character.h"

const uint SPEED = 5; // meter / seconde
// XXX Unused !?
// const uint NB_MAX_TRY = 20;
// const uint SPEED_PARACHUTE = 170; // ms par image

ObjBox::ObjBox(const std::string &name)
  : PhysicalObj(name) {
  m_allow_negative_y = true;

  parachute = true;

  energy = start_life_points;

  SetSpeed (SPEED, M_PI_2);
  SetCollisionModel(false, false, true);
  std::cout<<"super called"<<std::endl;
}

ObjBox::~ObjBox(){
  delete anim;
  Game::GetInstance()->SetCurrentBox(NULL);
}

// Say hello to the ground
void ObjBox::SignalCollision()
{
  SetAirResistFactor(1.0);
  Game::GetInstance()->SetCurrentBox(NULL);
  MSG_DEBUG("box", "End of the fall: parachute=%d", parachute);
  if (!parachute) return;

  MSG_DEBUG("box", "Start of the animation 'fold of the parachute'.");
  parachute = false;
  anim->SetCurrentFrame(0);
  anim->Start();
}

void ObjBox::SignalObjectCollision(PhysicalObj * obj)
{
  SignalCollision();
  if(typeid(*obj) == typeid(Character))
    ApplyBonus((Character *)obj);
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

void ObjBox::Refresh()
{
  // If we touch a character, we remove the medkit
  FOR_ALL_LIVING_CHARACTERS(team, character)
  {
    if(Overlapse(*character)) {
      ApplyBonus(&(*character));
      Ghost();
      return;
    }
  }
  // Refresh animation
  if (!anim->IsFinished() && !parachute) anim->Update();
}

//Boxes can explode...
void ObjBox::SignalGhostState(bool /*was_already_dead*/)
{
  if(energy > 0) return;
  ParticleEngine::AddNow(GetCenter() , 10, particle_FIRE, true);
  ApplyExplosion(GetCenter(), GameMode::GetInstance()->bonus_box_explosion_cfg); //reuse the bonus_box explosion
}

void ObjBox::GetValueFromAction(Action * a)
{
  start_life_points = a->PopInt();
  SetXY(a->PopPoint2i());
  SetSpeedXY(a->PopPoint2d());
}

void ObjBox::StoreValue(Action *a)
{
  a->Push(start_life_points);
  a->Push(GetPosition());
  a->Push(GetSpeed());
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Static methods
int ObjBox::start_life_points = 41;
