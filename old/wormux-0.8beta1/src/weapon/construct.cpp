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
 * Add a structure to the ground
 *****************************************************************************/

#include "construct.h"
#include "explosion.h"
#include "game/game_loop.h"
#include "game/game_mode.h"
#include "include/action_handler.h"
#include "interface/mouse.h"
#include "map/camera.h"
#include "map/map.h"
#include "team/teams_list.h"
#include "tool/i18n.h"

const double DELTA_ANGLE = M_PI / 6.0; // should be a multiple


Construct::Construct() : Weapon(WEAPON_CONSTRUCT, "construct",
				new WeaponConfig(),
				NEVER_VISIBLE)
{
  construct_spr = resource_manager.LoadSprite( weapons_res_profile, "construct_spr");
  construct_spr->EnableRotationCache(static_cast<int>(2 * M_PI / DELTA_ANGLE));
  m_name = _("Construct");
  m_category = TOOL;
  angle = 0;
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

void Construct::HandleKeyPressed_Up()
{
  Up();
}

void Construct::HandleKeyPressed_Down()
{
  Down();
}

void Construct::HandleMouseWheelUp()
{
  Up();
}

void Construct::HandleMouseWheelDown()
{
  Down();
}

void Construct::Up()
{
  double new_angle = angle + DELTA_ANGLE;

  Action* a = new Action(Action::ACTION_WEAPON_CONSTRUCTION, new_angle);
  ActionHandler::GetInstance()->NewAction(a);
}

void Construct::Down()
{
  double new_angle = angle - DELTA_ANGLE;

  Action* a = new Action(Action::ACTION_WEAPON_CONSTRUCTION, new_angle);
  ActionHandler::GetInstance()->NewAction(a);
}

void Construct::SetAngle(double _angle)
{
  angle = _angle;
}

WeaponConfig& Construct::cfg()
{ return static_cast<WeaponConfig&>(*extra_params); }

std::string Construct::GetWeaponWinString(const char *TeamName, uint items_count )
{
  return Format(ngettext(
            "%s team has won %u construct!",
            "%s team has won %u constructs!",
            items_count), TeamName, items_count);
}

