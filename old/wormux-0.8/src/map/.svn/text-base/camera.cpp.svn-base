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
 * Camera : follow an object, center on it or follow mouse interaction.
 *****************************************************************************/

#include "map/camera.h"
#include "map/map.h"
#include "character/character.h"
#include "game/game.h"
#include "game/config.h"
#include "graphic/video.h"
#include "include/app.h"
#include "interface/cursor.h"
#include "interface/mouse.h"
#include "interface/interface.h"
#include "object/physical_obj.h"
#include "team/teams_list.h"
#include "tool/debug.h"
#include "tool/math_tools.h"
#include "game/time.h"
#include "tool/random.h"
#include <assert.h>

const Point2i CAMERA_SPEED(20, 20);

// minimum speed of an object that is followed in advance
#define MIN_SPEED_ADVANCE 5

// for this speed (and higher), the object can be in the corner of the screen
#define MAX_SPEED_ADVANCE 15

Camera::Camera():
  m_started_shaking( 0 ),
  m_shake_duration( 0 ),
  m_shake_amplitude( 0, 0 ),
  m_shake_centerpoint( 0, 0 ),
  m_shake( 0, 0 ),
  m_last_time_shake_calculated( 0 ),
  auto_crop(true),
  in_advance(false),
  followed_object(NULL)
{
  pointer_used_before_scroll = Mouse::POINTER_SELECT;
}

void Camera::Reset()
{
  auto_crop = true;
  in_advance = false;
  followed_object = NULL;
  SetXYabs(world.GetSize() / 2);
}

bool Camera::HasFixedX() const
{
  return (int)world.GetWidth() <= GetSizeX();
}

bool Camera::HasFixedY() const
{
  return (int)world.GetHeight() <= GetSizeY();
}

void Camera::SetXYabs(int x, int y)
{
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

void Camera::SetXY(Point2i pos)
{
  pos = pos * FreeDegrees();
  if( pos.IsNull() )
    return;

  SetXYabs(position + pos);
}

void Camera::AutoCrop()
{
  /* Stuff is put static in order to be able to reach the last position
   * of the object the camera was following, in case it desapears. This
   * typically happen when something explodes or a character dies. */
  static Point2i obj_pos(0, 0);
  static Point2i obj_size(1, 1);
  static Point2d obj_speed(0, 0);

  Point2i Camera_Speed = CAMERA_SPEED;

  if (followed_object && !followed_object->IsGhost() )
  {
    /* compute the ideal position!
     * it takes the physical object direction into account
     */
    obj_pos = followed_object->GetPosition();
    obj_size = followed_object->GetSize();

    if (followed_object->IsMoving() && in_advance)
    {
      Point2d prev_speed(obj_speed);
      obj_speed = followed_object->GetSpeed();

      if ((prev_speed.GetX() > 0 && obj_speed.GetX() < 0) ||
	  (prev_speed.GetX() < 0 && obj_speed.GetX() > 0))
      {
	if ((prev_speed.GetY() > 0 && obj_speed.GetY() < 0) ||
	    (prev_speed.GetY() < 0 && obj_speed.GetY() > 0))
	{
	  /* object has probably rebound, do not try anymore to follow it
	   * in advance
	   */
	  MSG_DEBUG("camera.follow", "No more in advance!");
	  in_advance = false;
	}
      }

      if (in_advance)
      {
	double speed_x = InRange_Double(fabs(obj_speed.GetX()), MIN_SPEED_ADVANCE,
					MAX_SPEED_ADVANCE) - MIN_SPEED_ADVANCE;
	uint diff_x = (uint)((speed_x * (GetSizeX()/3)) / (MAX_SPEED_ADVANCE - MIN_SPEED_ADVANCE));

	if (obj_speed.GetX() < -MIN_SPEED_ADVANCE) {
	  obj_pos.SetValues(obj_pos.GetX() - diff_x, obj_pos.GetY());
	} else if (obj_speed.GetX() > MIN_SPEED_ADVANCE) {
	  obj_pos.SetValues(obj_pos.GetX() + diff_x, obj_pos.GetY());
	}

	double speed_y = InRange_Double(fabs(obj_speed.GetY()), MIN_SPEED_ADVANCE,
					MAX_SPEED_ADVANCE) - MIN_SPEED_ADVANCE;
	uint diff_y = (uint)((speed_y * (GetSizeY()/3)) / (MAX_SPEED_ADVANCE - MIN_SPEED_ADVANCE));

	if (obj_speed.GetY() < -MIN_SPEED_ADVANCE) {
	  obj_pos.SetValues(obj_pos.GetX(), obj_pos.GetY() - diff_y);
	} else if (obj_speed.GetY() > MIN_SPEED_ADVANCE) {
	  obj_pos.SetValues(obj_pos.GetX(), obj_pos.GetY() + diff_y);
	}

	MSG_DEBUG("camera.follow", "-> Diff position: %u, %u",
		  diff_x, diff_y);

	Camera_Speed *= 4;
      }
    }
    else
    {
      obj_speed = Point2d(0, 0);
    }
  }

  if (obj_pos.y < 0)
    obj_pos.y = 0;

  Point2i dstMax = GetSize()/2;

  ASSERT(!dstMax.IsNull());

  // BR-> bottom right
  Point2i cameraBR = GetSize() + position;
  Point2i objectBRmargin = obj_pos + obj_size + GetSize()/2;
  Point2i dst(0, 0);

  dst += cameraBR.inf(objectBRmargin) * (objectBRmargin - cameraBR);
  dst += (obj_pos - GetSize()/2).inf(position) * (obj_pos - GetSize()/2 - position);

  SetXY(dst * Camera_Speed / dstMax);
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
  Point2i mousePos = Mouse::GetInstance()->GetPosition();

  uint zone_size = Config::GetInstance()->GetScrollBorderSize();
  Point2i sensitZone(zone_size, zone_size);

  /* tstVector represents the vector of how deep the cursor is in a sensit
   * zone; negative value means that the camera has to reduce its coordinates,
   * a positive value means that it should increase. Actually reduce means
   * LEFT/UP (for x/y) and increase RIGHT/DOWN directions.
   * The bigger tstVector is, the faster the camera will scroll. */
  Point2i tstVector;
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
  static Point2i first_mouse_pos(-1, -1);
  static Point2i last_mouse_pos(0, 0);
  Point2i curr_pos = Mouse::GetInstance()->GetPosition();

  int x,y;
  //Move camera with mouse holding Ctrl key down or with middle button of mouse
  if (SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_MIDDLE)
      || SDL_GetModState() & KMOD_CTRL)
    {
      // Begin to move the camera...
      if (Mouse::GetInstance()->GetPointer() != Mouse::POINTER_MOVE)
	{
	  first_mouse_pos = Point2i(x, y);
	  SaveMouseCursor();
	  Mouse::GetInstance()->SetPointer(Mouse::POINTER_MOVE);
	}

      SetAutoCrop(false);
      SetXY(last_mouse_pos - curr_pos);
      last_mouse_pos = curr_pos;
      return;
    }
  else if (Mouse::GetInstance()->GetPointer() == Mouse::POINTER_MOVE)
    {
      // if the mouse has not moved at all since the user pressed the middle button, we center the camera!
      if (first_mouse_pos == curr_pos)
	{
	  CenterOnActiveCharacter();
	}
      first_mouse_pos = Point2i(-1, -1);
      RestoreMouseCursor();
    }

  last_mouse_pos = curr_pos;

  if (!Interface::GetInstance()->weapons_menu.IsDisplayed() &&
      Config::GetInstance()->GetScrollOnBorder())
    ScrollCamera();
}

void Camera::Refresh(){
  // Check if player wants the camera to move
  TestCamera();

  if (auto_crop && followed_object != NULL)
    AutoCrop();
}

void Camera::FollowObject(const PhysicalObj *obj, bool follow,
			  bool _in_advance)
{
  MSG_DEBUG( "camera.tracking", "Following object %s",
                                 obj->GetName().c_str());

  if (followed_object != obj || !IsVisible(*obj) || auto_crop != follow)
    auto_crop = follow;

  in_advance = _in_advance;
  followed_object = obj;
}

void Camera::StopFollowingObj(const PhysicalObj* obj){

  if (followed_object == obj)
    followed_object = NULL;
}

bool Camera::IsVisible(const PhysicalObj &obj) const {
   return Intersect( obj.GetRect() );
}

void Camera::CenterOnActiveCharacter()
{
  CharacterCursor::GetInstance()->FollowActiveCharacter();
  FollowObject(&ActiveCharacter(), true);
}

Point2i Camera::ComputeShake() const
{
    uint time = Time::GetInstance()->Read();
    assert( time >= m_started_shaking );
    if ( time > m_started_shaking + m_shake_duration || m_shake_duration == 0 )
    {
        return Point2i( 0, 0 ); // not shaking now
    };

    if ( time == m_last_time_shake_calculated )
        return m_shake;

    // FIXME: we can underflow to 0 if time and m_started_shaking are large enough
    float t = ( float )( time - m_started_shaking ) / ( float )( m_shake_duration );

    float func_val = 1.0f;
    if ( t >= 0.0001f )
    {
        const float k_scale_angle = 10 * M_PI;
        float arg = k_scale_angle * t;
        // denormalized sinc
        func_val = ( 1 - t ) * sin( arg ) / arg;
    };

    float x_ampl = ( float )Random::GetDouble( -m_shake_amplitude.x, m_shake_amplitude.x );
    float y_ampl = ( float )Random::GetDouble( -m_shake_amplitude.y, m_shake_amplitude.y );
    m_shake.x = ( int )( x_ampl * func_val//( float )m_shake_amplitude.x * func_val
        + ( float )m_shake_centerpoint.x );
    m_shake.y = ( int )( y_ampl * func_val//( float )m_shake_amplitude.y * func_val
        + ( float )m_shake_centerpoint.y );

    static uint t_last_time_logged = 0;
    if ( time - t_last_time_logged > 10 )
    {
        MSG_DEBUG( "camera.shake", "Shaking: time = %d, t = %f, func_val = %f, shake: %d, %d",
            time,
            t, func_val, m_shake.x, m_shake.y );

        t_last_time_logged = time;
    };

    m_last_time_shake_calculated = time;
    return m_shake;
};

void Camera::Shake( uint how_long_msec, const Point2i & amplitude, const Point2i & centerpoint )
{
    MSG_DEBUG( "camera.shake", "Shake added!" );

    uint time = Time::GetInstance()->Read();

    assert( time >= m_started_shaking );
    if ( m_started_shaking + m_shake_duration > time )
    {
        // still shaking, so add amplitude/centerpoint to allow shakes to combine
        m_shake_amplitude = max( m_shake_amplitude, amplitude );
        m_shake_centerpoint = centerpoint;

        // increase shake duration so it lasts how_long_msec from this time
        m_shake_duration = how_long_msec + ( time - m_started_shaking );
    }
    else
    {
        // reinit the shake
        m_started_shaking = time;
        m_shake_duration = how_long_msec;
        m_shake_amplitude = amplitude;
        m_shake_centerpoint = centerpoint;
    };
};

void Camera::ResetShake()
{
    m_started_shaking = 0;
    m_shake_duration = 0;
    m_last_time_shake_calculated = 0;
    m_shake = Point2i( 0, 0 );
};
