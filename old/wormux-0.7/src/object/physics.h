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

#ifndef PHYSICS_H
#define PHYSICS_H

#include "../include/base.h"
#include "../tool/euler_vector.h"
#include "../tool/point.h"

typedef enum
{
  NoMotion,
  FreeFall,
  Pendulum,
} MotionType_t;

class GameLoop;

class Physics
{
public:
  bool m_rebounding;

private:
  double m_mass;
  MotionType_t m_motion_type ;

protected:
  EulerVector m_pos_x;          // x0 = pos, x1 = speed, x2 = acc on the X axys
  EulerVector m_pos_y;          // x0 = pos, x1 = speed, x2 = acc on the Y axys
  Point2d m_extern_force;  // External strength applyed to the object
  uint m_last_move;             // Time since last move
  double m_phys_width, m_phys_height;

  Point2d m_fix_point_gnd;   // Rope fixation point to the ground.
  Point2d m_fix_point_dxy;   // Rope delta to fixation point to the object
  EulerVector m_rope_angle;       // Rope angle.
  EulerVector m_rope_length;      // Rope length.
  double m_rope_elasticity;       // The smallest, the more elastic.
  double m_elasticity_damping;    // 0 means perpetual motion.
  double m_balancing_damping;     // 0 means perpetual balancing.

  // Wind effect factor on the object. 0 means not affected.
  double m_wind_factor;

  // Air resistance factor. 1 = normal air resistance.
  double m_air_resist_factor ;

  // Define how the object is affected by gravity.
  double m_gravity_factor ;
  
  // Object rebound factor when the object collide with the ground.
  double m_rebound_factor;

  // Define if the rope is elastic or not.
  bool m_elasticity_off ;

public:
  Physics (double mass=0.0);
  virtual ~Physics ();

  // Set/Get position
  void SetPhysXY (double x, double y);
  void SetPhysXY(const Point2d &position);

  double GetPhysX() const;
  double GetPhysY() const;
  Point2d GetPos() const;

  // Set size
  void SetPhysSize (double width, double height);

  // Set / Get mass
  void SetMass (double mass);
  double GetMass() const;

  // Set Wind Factor
  void SetWindFactor (double wind_factor);

  // Set Air Resist Factor
  void SetAirResistFactor (double factor);

  // Set Gravity Factor
  void SetGravityFactor (double factor);

  // Set initial speed.
  void SetSpeed (double norme, double angle);
  void SetSpeedXY (Point2d vector);

  // Add a initial speed to the current speed.
  void AddSpeed (double norme, double angle);
  void AddSpeedXY (Point2d vector);
 
  // Get current object speed
  void GetSpeed (double &norm, double &angle);
  void GetSpeedXY (Point2d &vector);
  double GetAngularSpeed();
  double GetSpeedAngle();

  // Add new strength
  void SetExternForce (double length, double angle);
  void SetExternForceXY (Point2d vector);

  // Add / Remove a fixation point.
  void SetPhysFixationPointXY(double g_x, double g_y,
			      double dx, double dy) ;
  void UnsetPhysFixationPoint() ;
  void ChangePhysRopeSize(double dl) ;
  double GetRopeAngle() ;
  double GetRopeLength();

  // Physical engine : update position (and state) with current time
  void RunPhysicalEngine();

  // Notify the son class that the object has moved.
  virtual bool NotifyMove(Point2d oldPos, Point2d newPos, Point2d &contactPos,
			  double &contact_angle) = 0 ;

  // Start moving
  void StartMoving();

  // Stop moving : signal it if the object was falling
  void StopMoving ();

  // The object is moving ?
  bool IsMoving() const;

  // The object is falling ?
  bool IsFalling() const;

protected:
  // Get Object / Air contact surface.
  double GetContactSurface(double angle) ;

  // Compute current (x,y) position
  Point2d ComputeNextXY(double delta_t);

  virtual void SignalDeath();
  virtual void SignalGhostState (bool was_already_dead);
  virtual void SignalFallEnding();
  virtual void SignalDrowning();
  virtual void SignalRebound();

private:

  void ComputeFallNextXY (double delta_t);

  void ComputePendulumNextXY (double delta_t);

  // Make the object rebound
  void Rebound(Point2d contactPos, double contact_angle);
};

#endif
