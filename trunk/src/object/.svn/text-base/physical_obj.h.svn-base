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
 * Abstract class used for physical object (object with a size, mass,
 * etc.). This object can have differents state : ready, is moving, or ghost
 * (is outside of the world).
 *
 * You can : make the object move (with collision test), change state, etc.
 * If the object go outside of the world, it become a ghost.
 *****************************************************************************/

#ifndef PHYSICAL_OBJECT_H
#define PHYSICAL_OBJECT_H

#include "physics.h"
#include <WARMUX_point.h>
#include <WARMUX_rectangle.h>

// Alive state
typedef enum
{
  ALIVE,
  DEAD,
  GHOST,
  DROWNED
} alive_t;

// Forward declaration
class Action;
class Character;

#define PIXEL_PER_METER 40
static const Double METER_PER_PIXEL(1.0/PIXEL_PER_METER);

class PhysicalObj : public Physics
{
  // collision management
  bool m_collides_with_ground;
  bool m_collides_with_characters;
  bool m_collides_with_objects;
  // Special meaning, SignalObjectCollision is called but trajectory continues
  // until out of map
  bool m_go_through_objects;
  PhysicalObj *m_last_collided_object;

  Point2i m_rebound_position;

  // Rectangle used for collision tests
  int m_test_left, m_test_right, m_test_top, m_test_bottom;

  // Object size and position.
  int m_width, m_height;

  bool can_be_ghost;

protected:
  PhysicalObj* m_overlapping_object;
  uint m_minimum_overlapse_time;
  bool m_ignore_movements;
  bool m_is_character;
  bool m_is_fire;

  virtual void CheckOverlapping();

  std::string m_name;
  std::string m_unique_id;

  std::string m_rebound_sound;

  alive_t m_alive;
  int m_energy;

  bool m_allow_negative_y;

  void StartMoving()
  {
    m_last_collided_object = NULL;
    Physics::StartMoving();
  }

public:
  PhysicalObj(const std::string &name, const std::string &xml_config="");
  /* Note : The copy constructor is not implemented (and this is not a bug)
   * because we can copy directly the pointer m_overlapping_object whereas this
   * object does not own it.
   * FIXME what happen if the object is deleted meanwhile ???*/
  virtual ~PhysicalObj();

  //-------- Set position and size -------

  void CanBeGhost(bool state) { can_be_ghost = state; }

  // Set/Get position - notice how we don't introduce rounding of the other coordinate
  void SetX(Double x) { SetXY( Point2d(x, GetPhysY() * PIXEL_PER_METER) ); };
  void SetY(Double y) { SetXY( Point2d(GetPhysX() * PIXEL_PER_METER, y) ); };
  void SetXY(const Point2i &position) { SetXY(Point2d(position.x, position.y)); };
  void SetXY(const Point2d &position);
  // GetX/GetY use a hack assuming the coordinates are always positive!
  int GetX() const { return uround(GetPhysX() * PIXEL_PER_METER); };
  int GetY() const { return uround(GetPhysY() * PIXEL_PER_METER); };
  // Get{X,Y} used to depend on Get{X,Y}Double, but this seems unnecessary Double roundtrips
  Double GetXDouble() const { return GetX(); };
  Double GetYDouble() const { return GetY(); };
  Point2d GetPosition() const { return Point2d(GetXDouble(), GetYDouble()) ;};

  // Set/Get size
  void SetSize(const Point2i &newSize);
  int GetWidth() const { return m_width; };
  int GetHeight() const { return m_height; };
  Point2i GetSize() const { return Point2i(m_width, m_height); };

  // Set/Get test rectangles
  void SetTestRect (uint left, uint right, uint top, uint bottom);
  // Optimized intersection test
  inline bool Intersect(const Rectanglei & position) const;
  const Rectanglei GetTestRect() const
  {
    int width = m_width - m_test_right - m_test_left;
    int height = m_height - m_test_bottom - m_test_top;
    width  =  width ?  width : 1;
    height = height ? height : 1;
    return Rectanglei(GetX() + m_test_left, GetY() + m_test_top, width, height);
  }
  int GetTestWidth() const { return m_width -m_test_left -m_test_right; };
  int GetTestHeight() const { return m_height -m_test_top -m_test_bottom; };

  //----------- Access to datas (read only) ----------
  virtual const std::string &GetName() const { return m_name; }

  const std::string &GetUniqueId() const { return m_unique_id; }
  void SetUniqueId(const std::string& s) { m_unique_id = s; }

  int GetCenterX() const { return GetX() +m_test_left +GetTestWidth()/2; };
  int GetCenterY() const { return GetY() +m_test_top +GetTestHeight()/2; };
  Point2i GetCenter() const { return Point2i(GetCenterX(), GetCenterY()); };
  Rectanglei GetRect() const { return Rectanglei( GetX(), GetY(), m_width, m_height); };
  bool CollidesWithGround() const { return m_collides_with_ground; }
  // This is a hack
  bool CanBeBlasted() const { return m_collides_with_ground && !m_go_through_objects; }
  bool IsCharacter() const { return m_is_character; }

  //----------- Physics related function ----------

  // Update position (and state) with current time
  void UpdatePosition();

  // Move the character until he gets out of the ground
  bool PutOutOfGround();

  // Where direction is the angle of the direction where the object is moved
  // and max_distance is max distance allowed when putting out
  bool PutOutOfGround(Double direction, Double max_distance=30);

  // Collision management
  void SetCollisionModel(bool collides_with_ground,
                         bool collides_with_characters,
                         bool collides_with_objects,
                         bool go_through_objects = false);
  void SetOverlappingObject(PhysicalObj* obj, int timeout = 0);
  const PhysicalObj* GetOverlappingObject() const { return m_overlapping_object; };
  virtual bool IsOverlapping(const PhysicalObj* obj) const { return m_overlapping_object == obj; };

  bool IsInVacuumXY(const Point2i &position, bool check_objects = true) const;
  // Relative to current position
  bool IsInVacuum(const Point2i &offset, bool check_objects = true) const { return IsInVacuumXY(GetPosition() + offset, check_objects); };
  PhysicalObj* CollidedObjectXY(const Point2i & position) const;
  // Relative to current position
  PhysicalObj* CollidedObject(const Point2i & offset = Point2i(0,0)) const { return CollidedObjectXY(GetPosition() + offset); };
  bool FootsInVacuumXY(const Point2i & position) const;
  bool FootsInVacuum() const { return FootsInVacuumXY(GetPosition()); };

  bool IsInWater() const;

  // The object is outside of the world
  bool IsOutsideWorldXY(const Point2i& position) const;
  // Relative to current position
  bool IsOutsideWorld(const Point2i &offset = Point2i(0,0)) const { return IsOutsideWorldXY( GetPosition() + offset ); };

  // Refresh datas
  virtual void Refresh() = 0;

  // Draw the object
  virtual void Draw() = 0;

  // Damage handling
  virtual void SetEnergyDelta(int delta, Character* dealer);

  //-------- state ----
  void Init();
  void Ghost();
  void Drown();
  void GoOutOfWater(); // usefull for supertux.

  virtual bool IsImmobile() const { return IsSleeping() || m_ignore_movements ||(!IsMoving() && !FootsInVacuum())||(m_alive == GHOST); };

  bool IsGhost() const { return m_alive == GHOST; };
  bool IsDrowned() const { return m_alive == DROWNED; };
  bool IsDead() const { return IsGhost() || IsDrowned() || m_alive==DEAD; };
  bool IsFire() const { return m_is_fire; }
  virtual bool IsBullet() const { return false; }

  // Are the two object in contact ? (uses test rectangles)
  bool Overlapse(const PhysicalObj &b) const { return GetTestRect().Intersect( b.GetTestRect() ); };

  // Do the point p touch the object ?
  bool Contain(const Point2i &p) const { return  GetTestRect().Contains( p ); };

  bool PutRandomly(bool on_top_of_world, Double min_dst_with_characters, bool net_sync = true);

  collision_t NotifyMove(Point2d oldPos, Point2d newPos);

protected:
  virtual void SignalRebound();
  virtual void SignalGroundCollision(const Point2d& /* my_speed_before */, const Double& /*contactAngle*/) { };
  virtual void SignalObjectCollision(const Point2d& /* my_speed_before */,
                                     PhysicalObj * /* collided/ing object */,
                                     const Point2d& /* object speed_before */) { };
  virtual void SignalOutOfMap() { };

private:
  //Retrun the position of the point of contact of the obj on the ground
  bool ContactPoint(int &x, int &y) const;


  // The object fall directly to the ground (or become a ghost)
  void DirectFall();

  // Directly after a rebound, if we are stuck in a wall, we stop moving
  void CheckRebound();

  void Collide(collision_t collision, PhysicalObj* collided_obj, const Point2d& position, Point2d& contactPos, Double& contactAngle);

  void ContactPointAngleOnGround(const Point2d& oldPos,
                                 Point2d& contactPos,
                                 Double& contactAngle) const;
};

#endif
