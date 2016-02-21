/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
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
//-----------------------------------------------------------------------------
//#include "../include/base.h"
//#include "../tool/math_tools.h"
#include "../object/physics.h"
#include "../tool/Point.h"
#include "../tool/Rectangle.h"
//-----------------------------------------------------------------------------

// Alive state
typedef enum
{
  ALIVE,
  DEAD,
  GHOST,
  DROWNED
} alive_t;

//-----------------------------------------------------------------------------

// Object type
typedef enum
{
  // Unbreakable object : detected by collision test, but isn't touch by
  // explosion
  objUNBREAKABLE,

  // Regular object : detected by collision test, suffers from explosions blast.
  objCLASSIC
} type_objet_t;

//-----------------------------------------------------------------------------
extern const double PIXEL_PER_METER ;

double MeterDistance (const Point2i &p1, const Point2i &p2) ;
//-----------------------------------------------------------------------------

class PhysicalObj : public Physics
{
public:
  // Object name (useful for debug ;-))
  std::string m_name;

  type_objet_t m_type;

private:
  // Object size and position.
  uint m_width, m_height;
  int m_posx, m_posy ;

  // Rectangle used for collision tests
  uint m_test_left, m_test_right, m_test_top, m_test_bottom;

protected:

  // Used by the sons of this class to allow modification of READY/BUSY state
  // (Unused by PhysicalObj)
  bool m_ready;

  std::string m_rebound_sound;

  alive_t m_alive;
  bool m_allow_negative_y;

public:
  PhysicalObj (const std::string &name, double mass=0.0);
  virtual ~PhysicalObj ();

  //-------- Set position and size -------

  // Set/Get position
  void SetX (int x);
  void SetY (int y);
  void SetXY (int x, int y);
  int GetX() const;
  int GetY() const;
  const Point2i GetPos() const; 
     
  // Set/Get size
  void SetSize (uint width, uint height);
  int GetWidth() const;
  int GetHeight() const;

  // Set/Get test rectangles
  void SetTestRect (uint left, uint right, uint top, uint bottom);
  const Rectanglei GetTestRect() const;   
  int GetTestWidth() const;
  int GetTestHeight() const;

  //----------- Access to datas (read only) ----------

  // Get Center position.
  int GetCenterX() const;
  int GetCenterY() const;
  const Point2i GetCenter() const;
  const Rectanglei GetRect() const;
  type_objet_t GetObjectType() const { return m_type; }

  //----------- Physics related function ----------

  // Update position (and state) with current time
  void UpdatePosition();

  // Move the character until he gets out of the ground
  bool PutOutOfGround();
  bool PutOutOfGround(double direction); //Where direction is the angle of the direction
                                         // where the object is moved

  bool NotifyMove(double old_x, double old_y, double new_x, double new_y,
		  double &contact_x, double &contact_y, double &contact_angle);

  bool IsInVacuumXY (int x, int y) const;
  bool IsInVacuum (int dx, int dy) const; // relative to current position
  bool FootsInVacuum () const;
  bool FootsInVacuumXY (int x, int y) const;
  
  bool FootsOnFloor(int y) const;

  bool IsInWater () const;

  // The object is outside of the world
  bool IsOutsideWorldXY (int x, int y) const;
  bool IsOutsideWorld (int dx, int dy) const;

  // Refresh datas
  virtual void Refresh() = 0;

  // Draw the object
  virtual void Draw() = 0;

  // Initialisation of datas (only called once)
  virtual void Init() = 0;

  // Reset datas
  virtual void Reset() = 0;

  //-------- state ----

  void Ready();
  void Die();
  void Ghost();
  void Drown();
  
  bool IsReady() const;
  bool IsDead() const;
  bool IsGhost() const;
  bool IsDrowned() const;

protected:

  // The object fall directly to the ground (or become a ghost)
  void DirectFall () ;

private:
  //Renvoie la position du point de contact entre
  //l'obj et le terrain
  bool ContactPoint (int &x, int &y);

  // Collision test for point (x,y)
  bool CollisionTest (int x, int y);

  void SignalRebound() ;


};

// Est-ce que deux objets se touchent ? (utilise les rectangles de test)
bool ObjTouche (const PhysicalObj &a, const PhysicalObj &b);

// Est-ce que le point p touche l'objet a ?
bool ObjTouche (const PhysicalObj &a, const Point2i &p);

//-----------------------------------------------------------------------------
#endif
