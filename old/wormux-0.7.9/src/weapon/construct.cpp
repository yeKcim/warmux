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
 * Add a structure to the ground
 *****************************************************************************/

#include "construct.h"
#include "explosion.h"
#include "../game/game_loop.h"
#include "../game/game_mode.h"
#include "../include/action_handler.h"
#include "../interface/mouse.h"
#include "../map/camera.h"
#include "../map/map.h"
#include "../network/network.h"
#include "../team/teams_list.h"
#include "../tool/i18n.h"

const double angle_step = M_PI / 6.0; // should be a multiple


Construct::Construct() : Weapon(WEAPON_CONSTRUCT, "construct",
					new WeaponConfig(),
					NEVER_VISIBLE)
{
  construct_spr = resource_manager.LoadSprite( weapons_res_profile, "construct_spr");
  construct_spr->EnableRotationCache(static_cast<int>(2 * M_PI / angle_step));
  m_name = _("Construct");
  angle = 0;
  force_override_keys = true;
  target_chosen = false;
}

Construct::~Construct()
{
  delete construct_spr;
}

bool Construct::p_Shoot ()
{
  if(!target_chosen)
	return false;
  jukebox.Play("share", "weapon/construct");
  world.MergeSprite(dst - construct_spr->GetSizeMax()/2, construct_spr);
  GameLoop::GetInstance()->interaction_enabled = false;
  return true;
}

void Construct::Refresh()
{
  m_is_active = false;
}

void Construct::Draw()
{
  if (!m_is_active) {
    Weapon::Draw();

    dst = Mouse::GetInstance()->GetWorldPosition();
    construct_spr->SetRotation_rad(angle);
    construct_spr->Draw(dst - construct_spr->GetSize()/2);
  }
}

void Construct::ChooseTarget(Point2i mouse_pos)
{
  dst = mouse_pos;
  target_chosen = true;
  Shoot();
}

void Construct::HandleKeyEvent(Action::Action_t action, Keyboard::Key_Event_t event_type)
{
  switch (action) {
    case Action::ACTION_UP:
      if (event_type == Keyboard::KEY_PRESSED)
        Up();
      break ;
    case Action::ACTION_DOWN:
      if (event_type == Keyboard::KEY_PRESSED)
        Down();
      break ;
    default:
      ActiveCharacter().HandleKeyEvent( action, event_type);
      break ;
  }
}

void Construct::ActionUp()
{
  Up();
}

void Construct::ActionDown()
{
  Down();
}

void Construct::Up()
{
  Action a(Action::ACTION_CONSTRUCTION_UP);
  if(ActiveTeam().IsLocal() || ActiveTeam().IsLocalAI())
    network.SendAction(&a);
  angle += angle_step;
}

void Construct::Down()
{
  Action a(Action::ACTION_CONSTRUCTION_DOWN);
  if(ActiveTeam().IsLocal() || ActiveTeam().IsLocalAI())
    network.SendAction(&a);
  angle -= angle_step;
}

WeaponConfig& Construct::cfg()
{ return static_cast<WeaponConfig&>(*extra_params); }

