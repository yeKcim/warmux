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
 * Weapon's crosshair
 *****************************************************************************/

#include "crosshair.h"
#include "weapon.h"
#include "game/game_loop.h"
#include "graphic/surface.h"
#include "include/app.h"
#include "map/camera.h"
#include "map/map.h"
#include "team/teams_list.h"
#include "tool/math_tools.h"

// Distance between crosshair and character
#define RAY 40 // pixels

CrossHair::CrossHair()
{
  enable = false;
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);
  image = resource_manager.LoadImage(res, "gfx/pointeur1");
  resource_manager.UnLoadXMLProfile(res);
}

void CrossHair::Reset()
{
  ActiveCharacter().SetFiringAngle(0.0);
}

// Compute crosshair position
void CrossHair::Refresh(double angle)
{
  crosshair_position = Point2i(RAY, RAY) * Point2d(cos(angle), sin(angle)) - image.GetSize() / 2;
}

void CrossHair::Draw()
{
  if( !enable )
    return;
  if( ActiveCharacter().IsDead() )
    return;
  if( GameLoop::GetInstance()->ReadState() != GameLoop::PLAYING )
    return;
  Point2i tmp = ActiveCharacter().GetHandPosition() + crosshair_position;
  AppWormux::GetInstance()->video.window.Blit(image, tmp - camera.GetPosition());
  world.ToRedrawOnMap(Rectanglei(tmp, image.GetSize()));
}
