/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
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
 * Weapon's crosshair
 *****************************************************************************/

#include "character/character.h"
#include "weapon/crosshair.h"
#include "weapon/weapon.h"
#include "game/game.h"
#include "graphic/surface.h"
#include "graphic/video.h"
#include "include/app.h"
#include "map/camera.h"
#include "map/map.h"
#include "team/teams_list.h"
#include "team/team.h"
#include "tool/math_tools.h"
#include "tool/resource_manager.h"

// Distance between crosshair and character
#define RAY 40 // pixels

CrossHair::CrossHair()
{
  enable = false;
  display = false;
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);
  image = resource_manager.LoadImage(res, "gfx/pointeur1");
  resource_manager.UnLoadXMLProfile(res);
}

void CrossHair::Reset() const
{
  ActiveCharacter().SetFiringAngle(0.0);
}

// Compute crosshair position
void CrossHair::Refresh(double angle)
{
  crosshair_position = Point2i(RAY, RAY) * Point2d(cos(angle), sin(angle)) - image.GetSize() / 2;
}

void CrossHair::Draw() const
{
  if(!display || !enable)
    return;
  if(ActiveCharacter().IsDead())
    return;
  if(Game::GetInstance()->ReadState() != Game::PLAYING)
    return;
  Point2i tmp = ActiveTeam().GetWeapon().GetGunHolePosition() + crosshair_position;
  AppWormux::GetInstance()->video->window.Blit(image, tmp - Camera::GetInstance()->GetPosition());
  world.ToRedrawOnMap(Rectanglei(tmp, image.GetSize()));
}
