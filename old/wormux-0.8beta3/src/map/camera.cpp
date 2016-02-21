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
 * Camera : follow an object, center on it or follow mouse interaction.
 *****************************************************************************/

#include "map/camera.h"
#include "map/map.h"
#include "game/game.h"
#include "game/config.h"
#include "graphic/video.h"
#include "include/app.h"
#include "interface/mouse.h"
#include "interface/interface.h"
#include "object/physical_obj.h"
#include "team/teams_list.h"
#include "tool/debug.h"
#include "tool/math_tools.h"

const Point2i CAMERA_SPEED(20, 20);

Camera* Camera::singleton = NULL;

Camera * Camera::GetInstance()
{
  if (singleton == NULL) {
    singleton = new Camera();
  }
  return singleton;
}

Camera::Camera():
  auto_crop(true),
  followed_object(NULL)
{
  pointer_used_before_scroll = Mouse::POINTER_SELECT;
}

void Camera::Reset()
{
  auto_crop = true;
  followed_object = NULL;
  SetXY(world.GetSize() / 2);
}

bool Camera::HasFixedX() const{
  return (int)world.GetWidth() <= GetSizeX();
}

bool Camera::HasFixedY() const{
  return (int)world.GetHeight() <= GetSizeY();
}

void Camera::SetXYabs(int x, int y){
  AppWormux * app = AppWormux::GetInstance();

  if(!HasFixedX())
    position.x = InRange_Long(x, 0, world.GetWidth() - GetSizeX());
  else
    position.x = - (app->video->window.GetWidth() - world.GetWidth())/2;

  if(!HasFixedY())
    position.y = InRange_Long(y, 0, world.GetHeight() - GetSizeY());
  else
    position.y = - (app->video->window.GetHeight() - world.GetHeight())/2;

}

void Camera::SetXY(Point2i pos){
  pos = pos * FreeDegrees();
  if( pos.IsNull() )
    return;

  SetXYabs(position + pos);
}

void Camera::AutoCrop(){
  /* Stuff is put static in order to be able to reach the last position
   * of the object the camera was following, in case it desapears. This
   * typically happen when something explodes or a character dies. */
  static Point2i pos(0, 0);
  static Point2i size(1, 1);

  if (followed_object && !followed_object->IsGhost() )
  {
    pos = followed_object->GetPosition();
    size = followed_object->GetSize();
  }

  if( pos.y < 0 )
    pos.y = 0;

  Point2i dstMax = GetSize()/2;

  ASSERT(!dstMax.IsNull());

  Point2i cameraBR = GetSize() + position;
  Point2i objectBRmargin = pos + size + GetSize()/2;
  Point2i dst(0, 0);

  dst += cameraBR.inf(objectBRmargin) * (objectBRmargin - cameraBR);
  dst += (pos - GetSize()/2).inf(position) * (pos - GetSize()/2 - position);

  SetXY(dst * CAMERA_SPEED / dstMax );
}

void Camera::SaveMouseCursor()
{
  Mouse::pointer_t current_pointer = Mouse::GetInstance()->GetPointer();
  if (current_pointer != Mouse::POINTER_MOVE &&
      current_pointer != Mouse::POINTER_ARROW_UP &&
      current_pointer != Mouse::POINTER_ARROW_DOWN &&
      current_pointer != Mouse::POINTER_ARROW_LEFT &&
      current_pointer != Mouse::POINTER_ARROW_RIGHT &&
      current_pointer != Mouse::POINTER_ARROW_DOWN_RIGHT &&
      current_pointer != Mouse::POINTER_ARROW_UP_RIGHT &&
      current_pointer != Mouse::POINTER_ARROW_UP_LEFT &&
      current_pointer != Mouse::POINTER_ARROW_DOWN_LEFT) {
    pointer_used_before_scroll = current_pointer;
  }
}

void Camera::RestoreMouseCursor()
{
  Mouse::GetInstance()->SetPointer(pointer_used_before_scroll);
}

void Camera::ScrollCamera()
{
  static const unsigned int SENSIT_SCROLL_MOUSE = 50;
  Point2i mousePos = Mouse::GetInstance()->GetPosition();

  Point2i tstVector;
  // If application is fullscreen, mouse is only sensitive when touching the
  // border screen
  int coef = (AppWormux::GetInstance()->video->IsFullScreen() ? 10 : 1);
  Point2i sensitZone(SENSIT_SCROLL_MOUSE / coef, SENSIT_SCROLL_MOUSE / coef);

  /* tstVector represents the vector of how deep the cursor is in a sensit
   * zone; negative value means that the camera has to reduce its coordinates,
   * a positive value means that it should increase. Actually reduce means
   * LEFT/UP (for x/y) and increase RIGHT/DOWN directions.
   * The bigger tstVector is, the faster the camera will scroll. */
  tstVector = GetSize().inf(mousePos + sensitZone) * (mousePos + sensitZone - GetSize()) ;
  tstVector -= mousePos.inf(sensitZone) * (sensitZone - mousePos);

  if (!tstVector.IsNull())
    {
      SetXY(tstVector);
      SetAutoCrop(false);
    }

  /* mouse pointer ***********************************************************/
  SaveMouseCursor();

  if (tstVector.IsNull())
    RestoreMouseCursor();
  else if (tstVector.IsXNull() && tstVector.y < 0)
    Mouse::GetInstance()->SetPointer(Mouse::POINTER_ARROW_UP);
  else if (tstVector.IsXNull() && tstVector.y > 0)
    Mouse::GetInstance()->SetPointer(Mouse::POINTER_ARROW_DOWN);
  else if (tstVector.IsYNull() && tstVector.x < 0)
    Mouse::GetInstance()->SetPointer(Mouse::POINTER_ARROW_LEFT);
  else if (tstVector.IsYNull() && tstVector.x > 0)
    Mouse::GetInstance()->SetPointer(Mouse::POINTER_ARROW_RIGHT);
  else if (tstVector.y > 0 && tstVector.x > 0)
    Mouse::GetInstance()->SetPointer(Mouse::POINTER_ARROW_DOWN_RIGHT);
  else if (tstVector.y < 0 && tstVector.x > 0)
    Mouse::GetInstance()->SetPointer(Mouse::POINTER_ARROW_UP_RIGHT);
  else if (tstVector.y < 0 && tstVector.x < 0)
    Mouse::GetInstance()->SetPointer(Mouse::POINTER_ARROW_UP_LEFT);
  else if (tstVector.y > 0 && tstVector.x < 0)
    Mouse::GetInstance()->SetPointer(Mouse::POINTER_ARROW_DOWN_LEFT);
  /***************************************************************************/
}

void Camera::TestCamera()
{
  static Point2i last_mouse_pos(0, 0);
  Point2i curr_pos = Mouse::GetInstance()->GetPosition();

  int x,y;
  //Move camera with mouse holding Ctrl key down or with middle button of mouse
  if (SDL_GetModState() & KMOD_CTRL ||
      SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_MIDDLE))
    {
      SetAutoCrop(false);
      SetXY(last_mouse_pos - curr_pos);
      SaveMouseCursor();
      Mouse::GetInstance()->SetPointer(Mouse::POINTER_MOVE);
      last_mouse_pos = curr_pos;
      return;
    }
  else if (Mouse::GetInstance()->GetPointer() == Mouse::POINTER_MOVE)
    RestoreMouseCursor();

  last_mouse_pos = curr_pos;

  if(!Interface::GetInstance()->weapons_menu.IsDisplayed() &&
     Config::GetInstance()->GetScrollOnBorder())
    ScrollCamera();
}

void Camera::Refresh(){
  // Check if player wants the camera to move
  TestCamera();

  if (auto_crop && followed_object != NULL)
    AutoCrop();
}

void Camera::FollowObject(const PhysicalObj *obj, bool follow){
  MSG_DEBUG( "camera.tracking", "Following object %s",
                                 obj->GetName().c_str());

  if (followed_object != obj || !IsVisible(*obj))
    auto_crop = follow;
  followed_object = obj;
}

void Camera::StopFollowingObj(const PhysicalObj* obj){

  if (followed_object == obj)
    followed_object = NULL;
}

bool Camera::IsVisible(const PhysicalObj &obj) const {
   return Intersect( obj.GetRect() );
}

