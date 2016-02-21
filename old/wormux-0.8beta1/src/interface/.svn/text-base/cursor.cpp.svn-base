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
 * Arrow on top of the active character
 *****************************************************************************/

#include "cursor.h"
#include <SDL.h>
#include "game/game_loop.h"
#include "game/time.h"
#include "graphic/effects.h"
#include "include/app.h"
#include "map/camera.h"
#include "object/physical_obj.h"
#include "team/teams_list.h" // ActiveCharacter()
#include "tool/point.h"
#include "tool/resource_manager.h"

const uint show_hide_time = 200; //time to show and hide the arrow
const uint y_min = 20; //number of pixels between the bottom of the arrow and the top of the sprite
const uint y_max = 90; //number of pixels between the bottom of the arrow and the top of the sprite
                        //when the arrow is at the top of its movement
const uint rebound_time = 1000; //Duration of a full rebound

CharacterCursor * CharacterCursor::singleton = NULL;

CharacterCursor * CharacterCursor::GetInstance() {
  if (singleton == NULL) {
    singleton = new CharacterCursor();
  }
  return singleton;
}

CharacterCursor::CharacterCursor()
{
  actif = false;
  want_hide = false;
  time_begin_anim = 0;
  last_update = 0;
  image = NULL;
  dy = 0;

  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);
  image = resource_manager.LoadSprite( res, "gfx/curseur");
  resource_manager.UnLoadXMLProfile( res);
}

CharacterCursor::~CharacterCursor()
{
  if(image) delete image;
}

// Draw cursor
void CharacterCursor::Draw()
{
  if (!IsDisplayed()) return;
  if (obj_designe == NULL) return;
  if (obj_designe -> IsGhost()) return;

  // Draw cursor arround character
  Point2i centre = obj_designe->GetCenter();
  uint x = centre.x - image->GetWidth()/2;
  uint y = obj_designe->GetY() - image->GetHeight() - y_min;

  image->Draw( Point2i(x, y+dy) );
}

void CharacterCursor::Refresh()
{
  if (!IsDisplayed()) return;

  image->Scale(1.0, 1.0);

  Time * global_time = Time::GetInstance();

  //The arrow is appearing:
  if( actif && global_time->Read() < time_begin_anim + show_hide_time )
  {
    dy = (int)((camera.GetPosition().y - ActiveCharacter().GetY()) * (1.0 - (global_time->Read() - time_begin_anim) / (float)show_hide_time));
    return;
  }

  //If we want to hide the cursor, we have to wait the end of the current rebound to make the cursor disappear
  if(want_hide)
  {
    if( ((global_time->Read() - (time_begin_anim + show_hide_time)) % rebound_time < rebound_time / 2)
    &&  ((last_update                - (time_begin_anim + show_hide_time)) % rebound_time > rebound_time / 2))
    {
      //We are at the end of the rebound
      actif = false;
      time_begin_anim = global_time->Read();
    }
  }

  //The arrow is disappearing:
  if( !actif && global_time->Read() < time_begin_anim + show_hide_time )
  {
    dy = (int)((camera.GetPosition().y - ActiveCharacter().GetY()) * ((global_time->Read() - time_begin_anim) / (float)show_hide_time));
    return;
  }

  //The arrow is rebounding:
  Rebound(image, dy, time_begin_anim + show_hide_time, rebound_time, -y_max - (-y_min));

  last_update = global_time->Read();
}

// Hide the cursor
void CharacterCursor::Hide()
{
  if(!actif) return;
  want_hide = true;
}

void CharacterCursor::Reset()
{
  actif = false;
  want_hide = false;
  obj_designe = NULL;
}

void CharacterCursor::FollowActiveCharacter()
{
  PointeObj(&ActiveCharacter());
}

void CharacterCursor::PointeObj (PhysicalObj *obj)
{
  if(actif && obj==obj_designe)
    return;
  obj_designe = obj;
  actif = true;
  want_hide = false;
  time_begin_anim = Time::GetInstance()->Read();
}

// Are we displaying the arrow on the screen ?
bool CharacterCursor::IsDisplayed() const {
  return actif || (Time::GetInstance()->Read() < time_begin_anim + show_hide_time);
}
