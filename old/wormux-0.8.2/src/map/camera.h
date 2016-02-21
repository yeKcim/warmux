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

#ifndef SCROLLING_H
#define SCROLLING_H

#include "include/base.h"
#include "interface/mouse.h"
#include "tool/point.h"
#include "tool/rectangle.h"

class PhysicalObj;

class Camera : public Rectanglei, public Singleton<Camera>
{
  Camera(const Camera&);
  const Camera& operator=(const Camera&);

private:
  Mouse::pointer_t pointer_used_before_scroll;
  uint m_started_shaking;
  uint m_shake_duration;
  Point2i m_shake_amplitude;
  Point2i m_shake_centerpoint;
  mutable Point2i m_shake;
  mutable uint m_last_time_shake_calculated;

  void SaveMouseCursor();
  void RestoreMouseCursor();

  void TestCamera();
  void ScrollCamera();

  bool auto_crop;
  bool in_advance;
  const PhysicalObj* followed_object;
  void AutoCrop();

  Point2i FreeDegrees() const { return Point2i(HasFixedX()? 0 : 1, HasFixedY()? 0 : 1); };
  Point2i NonFreeDegrees() const { return Point2i(1, 1) - FreeDegrees(); };
  Point2i ComputeShake() const;

protected:
  friend class Singleton<Camera>;
  Camera();

public:
  // before beginning a game
  void Reset();

  bool HasFixedX() const;
  bool HasFixedY() const;

  // set camera to position
  void SetXY(Point2i pos);
  void SetXYabs(int x, int y);
  void SetXYabs(const Point2i &pos) { SetXYabs(pos.x, pos.y); };

  // Auto crop on an object
  // in_advance is used to center the camera on the direction where the object is going
  void FollowObject(const PhysicalObj *obj, bool follow, bool in_advance = false);
  void StopFollowingObj(const PhysicalObj* obj);

  void CenterOnActiveCharacter();

  bool IsVisible(const PhysicalObj &obj) const;

  void Refresh();

  inline Point2i GetPosition() const
  {
      return position + ComputeShake();
  }

  inline int GetPositionX() const
  {
      return position.x + ComputeShake().x;
  }

  inline int GetPositionY() const
  {
      return position.y + ComputeShake().y;
  }

  void Shake( uint how_long_msec, const Point2i & amplitude, const Point2i & centerpoint );
  void ResetShake();

  void SetAutoCrop(bool crop) { auto_crop = crop; };
  bool IsAutoCrop() const { return auto_crop; };
};

#endif
