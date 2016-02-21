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

#include "camera.h"
#include "map.h"
#include "maps_list.h"
#include "wind.h"
#include "include/app.h"
#include "interface/mouse.h"
#include "team/teams_list.h"
#include "tool/debug.h"
#include "tool/rectangle.h"
#include "tool/math_tools.h"
#include "game/game.h"

const Point2i CAMERA_MARGIN(200, 200);
const Point2i CAMERA_SPEED(20, 20);

Camera camera;

Camera::Camera():
  auto_crop(true),
  followed_object(NULL),
  throw_camera(false),
  follow_closely(false)
{}

void Camera::Reset()
{
  auto_crop = true;
  followed_object = NULL;
  throw_camera = false;
  follow_closely = false;
}

bool Camera::HasFixedX() const{
  return (int)world.GetWidth() <= GetSizeX();
}

bool Camera::HasFixedY() const{
  return (int)world.GetHeight() <= GetSizeY();
}

Point2i Camera::FreeDegrees() const{
  return Point2i(HasFixedX()? 0 : 1,
                 HasFixedY()? 0 : 1);
}

Point2i Camera::NonFreeDegrees() const{
  return Point2i(1, 1) - FreeDegrees();
}

void Camera::SetXYabs(int x, int y){
  AppWormux * app = AppWormux::GetInstance();

  if( !HasFixedX() )
    position.x = BorneLong(x, 0, world.GetWidth() - GetSizeX());
  else
    position.x = - (app->video.window.GetWidth() - world.GetWidth())/2;

  if( !HasFixedY() )
    position.y = BorneLong(y, 0, world.GetHeight()-GetSizeY());
  else
    position.y = - (app->video.window.GetHeight() - world.GetHeight())/2;

  throw_camera = true;
}

void Camera::SetXYabs(const Point2i &pos){
  SetXYabs(pos.x, pos.y);
}

void Camera::SetXY(Point2i pos){
  pos = pos * FreeDegrees();
  if( pos.IsNull() )
    return;

  SetXYabs(position + pos);
}

void Camera::CenterOnFollowedObject(){
  CenterOn(*followed_object);
}

// Center on a object
void Camera::CenterOn(const PhysicalObj &obj){
  if (obj.IsGhost())
    return;

  MSG_DEBUG( "camera.scroll", "centering on %s", obj.GetName().c_str() );

  Point2i pos(0, 0);

  pos += FreeDegrees() * obj.GetPosition() - ( GetSize() - obj.GetSize() )/2;
  pos += NonFreeDegrees() * ( world.GetSize() - GetSize() )/2;

  SetXYabs( pos );
}

void Camera::AutoCrop(){
  if( !followed_object || followed_object->IsGhost() )
    return;

  if( !IsVisible(*followed_object) )
  {
    MSG_DEBUG("camera.scroll", "The object is not visible.");
    CenterOnFollowedObject();
    return;
  }

  if(follow_closely)
  {
    CenterOn(*followed_object);
    return;
  }
  Point2i pos = followed_object->GetPosition();
  Point2i size = followed_object->GetSize();

  if( pos.y < 0 )
    pos.y = 0;

  Point2i dstMax = GetSize()/2 - CAMERA_MARGIN;
  Point2i cameraBR = GetSize() + position;
  Point2i objectBRmargin = pos + size + CAMERA_MARGIN;
  Point2i dst(0, 0);

  dst += cameraBR.inf(objectBRmargin) * (objectBRmargin - cameraBR);
  dst += (pos - CAMERA_MARGIN).inf(position) * (pos - CAMERA_MARGIN - position);

  SetXY( dst * CAMERA_SPEED / dstMax );
}

void Camera::SetAutoCrop(bool crop)
{
  auto_crop = crop;
}

bool Camera::IsAutoCrop() const
{
  return auto_crop;
}

void Camera::SetCloseFollowing(bool close)
{
  follow_closely = close;
}

void Camera::Refresh(){
  throw_camera = false;

  // mouse mouvement
  Mouse::GetInstance()->TestCamera();
  if (throw_camera) return;

  if (auto_crop)
    AutoCrop();
}

void Camera::FollowObject(PhysicalObj *obj, bool follow, bool center_on, bool force_center_on_object){
  MSG_DEBUG( "camera.tracking", "Following object %s, center_on=%d, follow=%d", obj->GetName().c_str(), center_on, follow);
  if ((center_on) && (followed_object != obj ||
                      !IsVisible(*obj) || force_center_on_object))
  {
    bool visible = IsVisible(*obj);
    CenterOn(*obj);
    auto_crop = follow;
    if(!visible)
      wind.RandomizeParticlesPos();
  }
  followed_object = obj;
}

void Camera::StopFollowingObj(PhysicalObj* obj){
  if(Game::GetInstance()->IsGameFinished())
    return;

  if (followed_object == obj)
    FollowObject((PhysicalObj*)&ActiveCharacter(), true, true, true);
}

bool Camera::IsVisible(const PhysicalObj &obj){
   return Intersect( obj.GetRect() );
}

