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

#ifndef PHYSICS_H
#define PHYSICS_H

#include "include/base.h"
#include "tool/euler_vector.h"
#include "tool/point.h"
#include "object_cfg.h"

typedef enum
{
  NoMotion,
  FreeFall,
  Pendulum,
} MotionType_t;

class Game;
class Action;

class Physics : private ObjectConfig
{
private:
  MotionType_t m_motion_type ;
  EulerVector m_pos_x;          // x0 = pos, x1 = speed, x2 = acc on the X axys
  EulerVector m_pos_y;          // x0 = pos, x1 = speed, x2 = acc on the Y axys
  Point2d m_extern_force;  // External strength applyed to the object

protected:
  uint m_last_move;             // Time since last move
  double m_phys_width, m_phys_height;

  Point2d m_fix_point_gnd;   // Rope fixation point to the ground.
  Point2d m_fix_point_dxy;   // Rope delta to fixation point to the object
  EulerVector m_rope_angle;       // Rope angle.
  EulerVector m_rope_length;      // Rope length.
  double m_rope_elasticity;       // The smallest, the more elastic.
  double m_elasticity_damping;    // 0 means perpetual motion.
  double m_balancing_damping;     // 0 means perpetual balancing.

  // Define if the rope is elastic or not.
  bool m_elasticity_off ;

  // Other physics constants stored there :
  ObjectConfig m_cfg;
public:
  Physics ();
  virtual ~Physics () {};

  // Set/Get position
  void SetPhysXY (double x, double y) { m_pos_x.x0 = x; m_pos_y.x0 = y; }
  void SetPhysXY(const Point2d &position) { SetPhysXY(position.x, position.y); };

  double GetPhysX() const { return m_pos_x.x0; };
  double GetPhysY() const { return m_pos_y.x0; };
  Point2d GetPos() const { return Point2d( m_pos_x.x0, m_pos_y.x0); };

  // Set size
  void SetPhysSize (double width, double height) { m_phys_width = width; m_phys_height = height ; };

  void SetMass (double mass) { m_mass = mass ; };
  const double GetMass() const { return m_mass; }

  void SetWindFactor (double wind_factor) { m_wind_factor = wind_factor; };
  const double GetWindFactor () const { return m_wind_factor; }

  void SetAirResistFactor (double factor) { m_air_resist_factor = factor; };
  const double GetAirResistFactor () const{ return m_air_resist_factor; }

  void SetGravityFactor (double factor) { m_gravity_factor = factor; };
  const double GetGravityFactor () const { return m_gravity_factor; }

  void SetRebounding (bool rebounding) { m_rebounding = rebounding; }
  const bool GetRebounding () const { return m_rebounding; }

  // Used to sync value across network
  virtual void GetValueFromAction(Action *);
  virtual void StoreValue(Action *);

  // Reset the physics constants (mass, air_resistance...) to the default values in the cfg
  void ResetConstants() { *((ObjectConfig*)this) = m_cfg; };

  // Set initial speed.
  void SetSpeedXY (Point2d vector);
  void SetSpeed (double norm, double angle) { SetSpeedXY(Point2d::FromPolarCoordinates(norm, angle)); };

  // Add a initial speed to the current speed.
  void AddSpeedXY (Point2d vector);
  void AddSpeed (double norm, double angle) { AddSpeedXY(Point2d::FromPolarCoordinates(norm, angle)); };

  // Get current object speed
  void GetSpeed (double &norm, double &angle) const;
  Point2d GetSpeedXY () const { return (!IsMoving()) ? Point2d(0.0, 0.0) : Point2d(m_pos_x.x1, m_pos_y.x1); };
  Point2d GetSpeed() const { return GetSpeedXY(); };
  double GetAngularSpeed() const { return m_rope_angle.x1; };
  double GetSpeedAngle() const { return GetSpeedXY().ComputeAngle(); };

  // Add new strength
  void SetExternForceXY (const Point2d& vector);
  void SetExternForce (double norm, double angle) { SetExternForceXY(Point2d::FromPolarCoordinates(norm, angle)); };
  Point2d GetExternForce() const { return m_extern_force; };

  // Add / Remove a fixation point.
  void SetPhysFixationPointXY(double g_x, double g_y,
                              double dx, double dy) ;
  void UnsetPhysFixationPoint() ;
  void ChangePhysRopeSize(double delta) ;

  double GetRopeAngle() const { return m_rope_angle.x0; };
  void SetRopeAngle(double angle) { m_rope_angle.x0 = angle; };

  double GetRopeLength() const { return m_rope_length.x0; };
  void SetRopeLength(double length) { m_rope_length.x0 = length; };

  // Physical engine : update position (and state) with current time
  void RunPhysicalEngine();

  // Notify the son class that the object has moved.
  virtual void NotifyMove(Point2d oldPos, Point2d newPos) = 0 ;

  // Start moving
  void StartMoving();

  // Stop moving
  void StopMoving();

  // The object is moving ?
  bool IsMoving() const;

  // The object is falling ?
  bool IsFalling() const { return (m_motion_type==FreeFall) && (m_pos_y.x1 > 0.1); };

protected:
  // Compute current (x,y) position
  Point2d ComputeNextXY(double delta_t);

  virtual void SignalDeath() { };
  virtual void SignalGhostState (bool) { };
  virtual void SignalDrowning() { };
  virtual void SignalGoingOutOfWater() { };
  virtual void SignalRebound() { };

  // Make the object rebound
  void Rebound(Point2d contactPos, double contact_angle);
private:

  void ComputeFallNextXY (double delta_t);

  void ComputePendulumNextXY (double delta_t);
};

#endif
