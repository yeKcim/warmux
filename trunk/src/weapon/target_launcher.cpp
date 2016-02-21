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
 * Target launcher: base class for weapons using same targetting features
 *****************************************************************************/

#include "graphic/video.h"
#include "include/app.h"
#include "interface/mouse.h"
#include "map/camera.h"
#include "map/map.h"
#include "network/network.h"
#include "team/team.h"
#include "team/teams_list.h"
#include "tool/resource_manager.h"
#include "weapon/explosion.h"
#include "weapon/target_launcher.h"

TargetLauncher::TargetLauncher(Weapon_type type,
                               const std::string &id,
                               EmptyWeaponConfig * params) :
  WeaponLauncher(type, id, params)
{
  mouse_character_selection = false;
  m_allow_change_timeout = true;
  m_target.selected = false;
  m_target.image = GetResourceManager().LoadImage(weapons_res_profile, "baz_cible");
}

void TargetLauncher::Draw()
{
  WeaponLauncher::Draw();
  DrawTarget();
}

void TargetLauncher::Refresh()
{
  DrawTarget();
  WeaponLauncher::Refresh();
}

void TargetLauncher::p_Select()
{
  WeaponLauncher::p_Select();
  m_target.selected = false;

  if (Network::GetInstance()->IsTurnMaster())
    Mouse::GetInstance()->SetPointer(Mouse::POINTER_AIM);
}

void TargetLauncher::p_Deselect()
{
  WeaponLauncher::p_Deselect();
  if (m_target.selected) {
    // need to clear the old target
    GetWorld().ToRedrawOnMap(Rectanglei(m_target.pos - (m_target.image.GetSize()>>1),
                                        m_target.image.GetSize()));
  }
}

void TargetLauncher::ChooseTarget(Point2i mouse_pos)
{
  if (m_target.selected) {
    // need to clear the old target
    GetWorld().ToRedrawOnMap(Rectanglei(m_target.pos - (m_target.image.GetSize()>>1),
                                        m_target.image.GetSize()));
  }

  m_target.pos = mouse_pos;
  m_target.selected = true;

  if (!ActiveTeam().IsLocalHuman())
    Camera::GetInstance()->SetXYabs(mouse_pos - Camera::GetInstance()->GetSize()/2);
  DrawTarget();
}

void TargetLauncher::DrawTarget() const
{
  if (!m_target.selected)
    return;

  GetMainWindow().Blit(m_target.image,
                       m_target.pos - (m_target.image.GetSize()>>1)
                       - Camera::GetInstance()->GetPosition());

  GetWorld().ToRedrawOnMap(Rectanglei(m_target.pos - (m_target.image.GetSize()>>1),
                                      m_target.image.GetSize()));
}

bool TargetLauncher::IsReady() const
{
  return (EnoughAmmo() && m_target.selected);
}
