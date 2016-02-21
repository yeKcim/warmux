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
 * Camera : follow an object, center on it or follow mouse interaction.
 *****************************************************************************/

#include <WARMUX_debug.h>
#include <WARMUX_random.h>

#include "character/character.h"
#include "game/config.h"
#include "game/game.h"
#include "game/game_time.h"
#include "graphic/video.h"
#include "include/app.h"
#include "interface/cursor.h"
#include "interface/interface.h"
#include "interface/mouse.h"
#include "map/camera.h"
#include "map/map.h"
#include "object/physical_obj.h"
#include "team/teams_list.h"
#include "tool/math_tools.h"
#include "tool/string_tools.h"

static const Point2f MAX_CAMERA_SPEED(5000, 5000);
static const Point2f MAX_CAMERA_ACCELERATION(1.5,1.5);

#define REACTIVITY             0.6f
#define SPEED_REACTIVITY       0.05f
#define REALTIME_FOLLOW_FACTOR 0.15f

#define ANTICIPATION               18
#define ADVANCE_ANTICIPATION       20
#define SPEED_REACTIVITY_CEIL       4
#define SCROLL_KEYBOARD            20 // pixel
#define REALTIME_FOLLOW_LIMIT      25
#define MAX_REFRESHES_PER_SECOND  100

Camera::Camera()
  : m_started_shaking(0)
  , m_shake_duration(0)
  , m_shake_amplitude(0, 0)
  , m_shake_centerpoint(0, 0)
  , m_shake(0, 0)
  , m_last_time_shake_calculated(0)
  , m_speed(0, 0)
  , m_stop(false)
  , m_control_mode(NO_CAMERA_CONTROL)
  , m_begin_controlled_move_time(0)
  , m_mouse_counter(0)
  , m_scroll_start_pos(0, 0)
  , m_last_mouse_pos(0, 0)
  , m_scroll_vector(0.0f, 0.0f)
  , auto_crop(true)
  , followed_object(NULL)
{
  pointer_used_before_scroll = Mouse::POINTER_SELECT;
}

void Camera::Reset()
{
  m_stop = false;
  auto_crop = true;
  followed_object = NULL;
  m_begin_controlled_move_time = 0;
  m_control_mode = NO_CAMERA_CONTROL;
  SetXYabs(GetWorld().GetSize() / 2);
}

bool Camera::HasFixedX() const
{
  return GetWorld().GetWidth() <= GetSizeX();
}

bool Camera::HasFixedY() const
{
  return GetWorld().GetHeight() <= GetSizeY();
}

void Camera::SetXYabs(int x, int y)
{
  Surface &window = GetMainWindow();

  if (!HasFixedX())
    position.x = InRange_Long(x, 0, GetWorld().GetWidth() - GetSizeX());
  else
    position.x = (GetWorld().GetWidth() - window.GetWidth())/2;

  if (!HasFixedY())
    position.y = InRange_Long(y, 0, GetWorld().GetHeight() - GetSizeY());
  else
    position.y = (GetWorld().GetHeight() - window.GetHeight())/2;
}

void Camera::SetXY(Point2i pos)
{
  pos = pos*FreeDegrees();
  if (pos.IsNull())
    return;

  SetXYabs(position + pos);
}

void Camera::AutoCrop()
{
  /* Stuff is put static in order to be able to reach the last position
   * of the object the camera was following, in case it disappears. This
   * typically happen when something explodes or a character dies. */
  static Point2i obj_pos(0, 0);

  Point2i target(0,0);
  bool stop = false;

  //Stop kinetic scrolling from interfering camera movement
  m_scroll_vector = Point2f();

  if (followed_object && !followed_object->IsGhost()) {

    /* compute the ideal position!
     * it takes the physical object direction into account
     */
    obj_pos = followed_object->GetCenter();

    if (obj_pos > GetPosition() + (GetSize()>>3) &&
        obj_pos < GetPosition() + ((7 * GetSize())>>3)) {
      if (m_stop)
        stop = true;

    } else {
      m_stop = false;
    }

    target = obj_pos;

    if (followed_object->IsMoving()) {
      float fps = Game::GetInstance()->GetLastFrameRate();
      if (fps < 0.1f)
        fps = 0.1f;
      Double time_delta = 1000 / (Double)fps;
      Point2d anticipation = followed_object->GetSpeed() * time_delta;

      //limit anticipation to screen size/3
      Point2d anticipation_limit = GetSize()/3;
      target += anticipation.clamp(-anticipation_limit, anticipation_limit);
    }

    target -= GetSize()>>1;

  } else {
    target = GetPosition();
    m_stop = true;
  }

  //Compute new speed to reach target
  Point2f acceleration = (target - m_speed*ANTICIPATION - position)*REACTIVITY;
  // Limit acceleration
  acceleration = acceleration.clamp(-MAX_CAMERA_ACCELERATION, MAX_CAMERA_ACCELERATION);

  // std::cout<<"acceleration before : "<<acceleration.x<<" "<<acceleration.y<<std::endl;
  if ((int)abs(m_speed.x) > SPEED_REACTIVITY_CEIL) {
    acceleration.x *= (1 + SPEED_REACTIVITY * ((int)abs(m_speed.x) - SPEED_REACTIVITY_CEIL));
  }

  if ((int)abs(m_speed.y) > SPEED_REACTIVITY_CEIL) {
    acceleration.y *= (1 + SPEED_REACTIVITY * ((int)abs(m_speed.y) - SPEED_REACTIVITY_CEIL));
  }

  //Stop camera oscillation when near target
  if (position.x - target.x == 0) {
    acceleration.x = 0.0f;
    m_speed.x = 0.0f;
  }

  if (position.y - target.y == 0) {
    acceleration.y = 0.0f;
    m_speed.y = 0.0f;
  }

  //printf("speed=(%.2f,%.2f)  target=(%i,%i)\n", m_speed.x, m_speed.y, target.x, target.y);
  if (stop) {
    m_speed = m_speed/2;

  } else {

    //Apply acceleration
    m_speed += acceleration;

    // Realtime follow is enabled if object is too fast to be correctly followed
    // stop can only be true if followed_obj!=NULL
    // speed is set to 0 if not moving
    if (abs((int)followed_object->GetSpeed().x) > REALTIME_FOLLOW_LIMIT) {
      m_speed.x = (target.x - position.x) * REALTIME_FOLLOW_FACTOR;
    }

    if (abs((int)followed_object->GetSpeed().y) > REALTIME_FOLLOW_LIMIT) {
      m_speed.y = (target.y - position.y) * REALTIME_FOLLOW_FACTOR;
    }

    //Limit
    m_speed = m_speed.clamp(-MAX_CAMERA_SPEED, MAX_CAMERA_SPEED);
  }

  //Update position
  Point2i next_position((int)m_speed.x, (int)m_speed.y);
  //printf("pos=(%i,%i) speed=(%.2f,%.2f)\n", next_position.x, next_position.y, m_speed.x, m_speed.y);
  SetXY(next_position);

  if (!m_stop && next_position.IsNull() && followed_object->GetSpeed().IsNull()) {
    m_stop = true;
  }
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
  if (!Mouse::GetInstance()->HasFocus()) // The application has not the focus, don't move the camera!!
    return;

  Point2i mousePos = Mouse::GetInstance()->GetPosition();
  bool over_interface = Interface::GetInstance()->Intersect(mousePos);

  if (!Config::GetInstance()->GetScrollOnBorder()) {
    /* Kinetic scrolling */
    if (!SDL_GetMouseState(NULL, NULL) || over_interface) {
      m_scroll_start_pos = Point2i();
      m_last_mouse_pos   = Point2i();
      m_mouse_counter    = 0;

      if (!m_scroll_vector.IsNull()) {
        Point2f brk = 0.2f * m_scroll_vector.GetfloatNormal();

        MSG_DEBUG("camera",
                  "scroll_vector=(%.3f,%.3f)  scroll_vector_break=(%.3f,%.3f)\n",
                  m_scroll_vector.GetX(), m_scroll_vector.GetY(),
                  brk.GetX(), brk.GetY());

        m_scroll_vector -= brk;
        SetXY(-m_scroll_vector);
        if (m_scroll_vector.Norm() < 1)
          m_scroll_vector = Point2f();
      }
      return;
    }

    if (over_interface)
      return;

    m_mouse_counter++;

    if (m_scroll_start_pos.IsNull())
      m_scroll_start_pos = mousePos;
    if (m_last_mouse_pos.IsNull())
      m_last_mouse_pos = mousePos;

    m_scroll_vector = mousePos - m_scroll_start_pos;
    m_scroll_vector = m_scroll_vector / m_mouse_counter;
    MSG_DEBUG("camera",
              "scroll_vector=(%.3f,%.3f) mousePos=(%i,%i) lastMousePos=(%i,%i) scrollStartPos=(%i,%i)",
              m_scroll_vector.GetX(), m_scroll_vector.GetY(),
              mousePos.GetX(), mousePos.GetY(), 
              m_last_mouse_pos.GetX(), m_last_mouse_pos.GetY(),
              m_scroll_start_pos.GetX(), m_scroll_start_pos.GetY());

    if (mousePos != m_last_mouse_pos) {
      SetXY(-(mousePos-m_last_mouse_pos));
      m_last_mouse_pos = mousePos;
      SetAutoCrop(false);
    }
  } else {

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

    if (!tstVector.IsNull()) {
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
}

void Camera::HandleMouseMovement()
{
  static Point2i first_mouse_pos(-1, -1);
  static Point2i last_mouse_pos(0, 0);
  Point2i curr_pos = Mouse::GetInstance()->GetPosition();

  int x,y;
  //Move camera with mouse holding Ctrl key down or with middle button of mouse
  if (SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_MIDDLE)
      || SDL_GetModState() & KMOD_CTRL) {

    // Begin to move the camera...
    if (Mouse::GetInstance()->GetPointer() != Mouse::POINTER_MOVE) {
      first_mouse_pos = Point2i(x, y);
      SaveMouseCursor();
      Mouse::GetInstance()->SetPointer(Mouse::POINTER_MOVE);
    }

    SetAutoCrop(false);
    SetXY(last_mouse_pos - curr_pos);
    last_mouse_pos = curr_pos;

    if (m_begin_controlled_move_time == 0) {
      m_begin_controlled_move_time = GameTime::GetInstance()->Read();
    }

    if (SDL_GetModState() & KMOD_CTRL) {
      m_control_mode = KEYBOARD_CAMERA_CONTROL;
    } else {
      m_control_mode = MOUSE_CAMERA_CONTROL;
    }
    return;

  } else if (m_control_mode == MOUSE_CAMERA_CONTROL) {

    // if the mouse has not moved at all since the user pressed the middle button, we center the camera!
    if (abs((int)first_mouse_pos.x - curr_pos.x) < 5 &&
        abs((int)first_mouse_pos.y - curr_pos.y) < 5 &&
        GameTime::GetInstance()->Read() - m_begin_controlled_move_time < 500) {
      CenterOnActiveCharacter();
    }

    first_mouse_pos = Point2i(-1, -1);
    RestoreMouseCursor();
    m_control_mode = NO_CAMERA_CONTROL;
    m_begin_controlled_move_time = 0;

  } else if (m_control_mode == KEYBOARD_CAMERA_CONTROL) {
    first_mouse_pos = Point2i(-1, -1);
    RestoreMouseCursor();
    m_control_mode = NO_CAMERA_CONTROL;
    m_begin_controlled_move_time = 0;
  }

  ScrollCamera();
  last_mouse_pos = curr_pos;
}

void Camera::HandleMoveIntentions()
{
  const UDMoveIntention * ud_move_intention = GetLastUDMoveIntention();
  if (ud_move_intention) {
    if (ud_move_intention->GetDirection() == DIRECTION_UP)
      SetXY(Point2i(0, -SCROLL_KEYBOARD));
    else
      SetXY(Point2i(0, SCROLL_KEYBOARD));
  }
  const LRMoveIntention * lr_move_intention = GetLastLRMoveIntention();
  if (lr_move_intention) {
    if (lr_move_intention->GetDirection() == DIRECTION_RIGHT)
      SetXY(Point2i(SCROLL_KEYBOARD, 0));
    else
      SetXY(Point2i(-SCROLL_KEYBOARD, 0));
  }
  if (lr_move_intention || ud_move_intention)
    SetAutoCrop(false);
}

void Camera::Refresh(bool ignore_user)
{
  // Refresh gets called very often when the game is paused.
  // This "if" ensures that the camera doesn't move too fast.
  if (refresh_stopwatch.GetValue() >= 1000 / MAX_REFRESHES_PER_SECOND) {
    // Check if player wants the camera to move
    if (!ignore_user) {
      HandleMouseMovement();
      HandleMoveIntentions();
    }

    if (auto_crop && followed_object)
      AutoCrop();
    refresh_stopwatch.Reset();
  }
}

void Camera::FollowObject(const PhysicalObj *obj, bool follow_closely)
{
  MSG_DEBUG("camera.tracking", "Following object %s (%d)", obj->GetName().c_str(), follow_closely);

  Mouse::GetInstance()->Hide();

  auto_crop = true;

  m_stop = !follow_closely;
  followed_object = obj;
}

void Camera::StopFollowingObj(const PhysicalObj* obj)
{
  if (followed_object == obj) {
    followed_object = NULL;
    m_stop = true;
    m_speed = Point2f(0,0);
  }
}

bool Camera::IsVisible(const PhysicalObj &obj) const
{
  return Intersect(obj.GetRect());
}

void Camera::CenterOnActiveCharacter()
{
  CharacterCursor::GetInstance()->FollowActiveCharacter();
  FollowObject(&ActiveCharacter(),true);
}

Point2i Camera::ComputeShake() const
{
  uint time = GameTime::GetInstance()->Read();
  ASSERT(time >= m_started_shaking);

  if (time > m_started_shaking + m_shake_duration || m_shake_duration == 0) {
    return Point2i(0, 0); // not shaking now
  }

  if (time == m_last_time_shake_calculated)
    return m_shake;

  // FIXME: we can underflow to 0 if time and m_started_shaking are large enough
  float t = (float)(time - m_started_shaking) / (float)m_shake_duration;

  float func_val = 1.0f;
  if (t >= 0.001f) {
    float k_scale_angle = float(10 * M_PI);
    float arg = k_scale_angle * t;
    // denormalized sinc
    func_val = (1 - t) * sin(arg) / arg;
  }

  float x_ampl = RandomLocal().Getfloat(-m_shake_amplitude.x, m_shake_amplitude.x);
  float y_ampl = RandomLocal().Getfloat(-m_shake_amplitude.y, m_shake_amplitude.y);

  m_shake.x = x_ampl * func_val + m_shake_centerpoint.x;
  m_shake.y = y_ampl * func_val + m_shake_centerpoint.y;

  static uint t_last_time_logged = 0;
  if (time - t_last_time_logged > 10) {
    MSG_DEBUG("camera.shake", "Shaking: time = %d, t = %.3f, func_val = %.3f, shake: %d, %d",
              time, t, func_val, m_shake.x, m_shake.y);
    t_last_time_logged = time;
  }

  m_last_time_shake_calculated = time;
  return m_shake;
}

void Camera::Shake(uint how_long_msec, const Point2i & amplitude, const Point2i & centerpoint)
{
  MSG_DEBUG("camera.shake", "Shake added!");

  uint time = GameTime::GetInstance()->Read();

  ASSERT(time >= m_started_shaking);

  if (m_started_shaking + m_shake_duration > time) {
    // still shaking, so add amplitude/centerpoint to allow shakes to combine
    m_shake_amplitude = max(m_shake_amplitude, amplitude);
    m_shake_centerpoint = centerpoint;

    // increase shake duration so it lasts how_long_msec from this time
    m_shake_duration = how_long_msec + (time - m_started_shaking);
  } else {
    // reinit the shake
    m_started_shaking = time;
    m_shake_duration = how_long_msec;
    m_shake_amplitude = amplitude;
    m_shake_centerpoint = centerpoint;
  }
}

void Camera::ResetShake()
{
  m_started_shaking = 0;
  m_shake_duration = 0;
  m_last_time_shake_calculated = 0;
  m_shake = Point2i(0, 0);
}
