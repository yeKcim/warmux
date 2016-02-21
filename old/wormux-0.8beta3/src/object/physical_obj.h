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
#include "tool/point.h"
#include "tool/rectangle.h"

// Alive state
typedef enum
{
  ALIVE,
  DEAD,
  GHOST,
  DROWNED
} alive_t;

typedef enum {
  NO_COLLISION = 0,
  COLLISION_ON_GROUND,
  COLLISION_ON_OBJECT
} collision_t;

extern const double PIXEL_PER_METER;

double MeterDistance (const Point2i &p1, const Point2i &p2);

class PhysicalObj : public Physics
{
  /* If you need this, implement it (correctly)*/
  const PhysicalObj& operator=(const PhysicalObj&);
  /*********************************************/

private:
  // collision management
  bool m_goes_through_wall;
  bool m_collides_with_characters;
  bool m_collides_with_objects;
  Point2i m_rebound_position;
protected:
  collision_t last_collision_type;
  PhysicalObj* m_overlapping_object;
  uint m_minimum_overlapse_time;
  bool m_ignore_movements;
  bool m_is_character;

  virtual void CheckOverlapping();

  std::string m_name;
  std::string m_unique_id;

  // Rectangle used for collision tests
  uint m_test_left, m_test_right, m_test_top, m_test_bottom;

  // Object size and position.
  uint m_width, m_height;

  std::string m_rebound_sound;

  alive_t m_alive;
  int energy;

  bool m_allow_negative_y;

public:
  PhysicalObj (const std::string &name, const std::string &xml_config="");
  /* Note : The copy constructor is not implemented (and this is not a bug)
   * because we can copy directly the pointer m_overlapping_object whereas this
   * object does not own it.
   * FIXME what happen if the object is deleted meanwhile ???*/
  virtual ~PhysicalObj ();

  //-------- Set position and size -------

  // Set/Get position
  void SetX (int x) { SetXY( Point2i(x, GetY()) ); };
  void SetY (int y) { SetXY( Point2i(GetX(), y) ); };
  void SetXY(const Point2i &position);
  void SetXY(const Point2d &position);
  int GetX() const;
  int GetY() const;
  const Point2i GetPosition() const { return Point2i(GetX(), GetY()); };

  // Set/Get size
  void SetSize(const Point2i &newSize);
  int GetWidth() const { return m_width; };
  int GetHeight() const { return m_height; };
  Point2i GetSize() const { return Point2i(m_width, m_height); };

  // Set/Get test rectangles
  void SetTestRect (uint left, uint right, uint top, uint bottom);
  const Rectanglei GetTestRect() const
  {
    return Rectanglei(GetX()+m_test_left,
                      GetY()+m_test_top,
                      m_width-m_test_right-m_test_left,
                      m_height-m_test_bottom-m_test_top);
  }
  int GetTestWidth() const { return m_width -m_test_left -m_test_right; };
  int GetTestHeight() const { return m_height -m_test_top -m_test_bottom; };

  //----------- Access to datas (read only) ----------
  virtual const std::string &GetName() const { return m_name; }
  const std::string &GetUniqueId() const { return m_unique_id; }
  int GetCenterX() const { return GetX() +m_test_left +GetTestWidth()/2; };
  int GetCenterY() const { return GetY() +m_test_top +GetTestHeight()/2; };
  const Point2i GetCenter() const { return Point2i(GetCenterX(), GetCenterY()); };
  const Rectanglei GetRect() const { return Rectanglei( GetX(), GetY(), m_width, m_height); };
  bool GoesThroughWall() const { return m_goes_through_wall; }

  //----------- Physics related function ----------

  // Update position (and state) with current time
  void UpdatePosition();

  // Move the character until he gets out of the ground
  bool PutOutOfGround();
  bool PutOutOfGround(double direction); //Where direction is the angle of the direction
                                         // where the object is moved

  // Collision management
  void SetCollisionModel(bool goes_through_wall,
                         bool collides_with_characters,
                         bool collides_with_objects);
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

  bool FootsOnFloor(int y) const;

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
  virtual void SetEnergyDelta(int delta, bool do_report = true);

  //-------- state ----
  void Init();
  void Ghost();
  void Drown();
  void GoOutOfWater(); // usefull for supertux.

  virtual bool IsImmobile() const { return m_ignore_movements ||(!IsMoving() && !FootsInVacuum())||(m_alive == GHOST); };
  bool IsGhost() const { return (m_alive == GHOST); };
  bool IsDrowned() const { return (m_alive == DROWNED); };
  bool IsDead() const { return (IsGhost() || IsDrowned() || (m_alive == DEAD)); };

  // Are the two object in contact ? (uses test rectangles)
  bool Overlapse(const PhysicalObj &b) const { return GetTestRect().Intersect( b.GetTestRect() ); };

  // Do the point p touch the object ?
  bool Contain(const Point2i &p) const { return  GetTestRect().Contains( p ); };

  bool PutRandomly(bool on_top_of_world, double min_dst_with_characters, bool net_sync = true);

  void NotifyMove(Point2d oldPos, Point2d newPos);

protected:
  virtual void SignalRebound();
  virtual void SignalObjectCollision(PhysicalObj *) { };
  virtual void SignalGroundCollision() { };
  virtual void SignalCollision() { };
  virtual void SignalOutOfMap() { };

private:
  //Retrun the position of the point of contact of the obj on the ground
  bool ContactPoint (int &x, int &y) const;


  // The object fall directly to the ground (or become a ghost)
  void DirectFall();

  // Directly after a rebound, if we are stuck in a wall, we stop moving
  void CheckRebound();
};

#endif
