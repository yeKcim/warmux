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

#include "../object/physics.h"
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include "../game/config.h"
#include "../tool/math_tools.h"
//#include "../tool/geometry_tools.h"
#include "../game/game_mode.h"
#include "../game/time.h"
#include "../map/wind.h"
using namespace Wormux;
//-----------------------------------------------------------------------------

#ifdef DEBUG
#  include <iostream>

//#define DEBUG_CHG_ETAT
//#define DEBUG_MSG_PHYSIQUE

#define COUT_DEBUG0 std::cout << "[Objet " << this << "]"
#define COUT_DEBUG COUT_DEBUG0 " "
#define COUT_PHYSIC COUT_DEBUG0 << "[Physique] "

#endif
//-----------------------------------------------------------------------------

// Physical constants

const double STOP_REBOUND_LIMIT = 0.5 ;
const double AIR_RESISTANCE_FACTOR = 14 ;
const double PHYS_DELTA_T = 0.01 ;         // Physical simulation time step
const double PENDULUM_REBOUND_FACTOR = 0.8 ;

//-----------------------------------------------------------------------------

Physics::Physics (double mass)
{
  SetVectorZero(m_extern_force);
  SetEulerVectorZero(m_pos_x);
  SetEulerVectorZero(m_pos_y);

  SetVectorZero(m_fix_point_gnd);
  SetVectorZero(m_fix_point_dxy);
  SetEulerVectorZero(m_rope_angle);
  SetEulerVectorZero(m_rope_length);
  
  m_rebounding = false;
  m_rebound_factor = 0.01;
  
  m_rope_elasticity = 10.0 ;
  m_elasticity_damping = 0.20 ;
  m_balancing_damping = 0.40 ;

  m_air_resist_factor = 1.0;
  m_wind_factor = 1.0;

  m_motion_type = NoMotion ;

  m_gravity_factor = 1.0;
  m_elasticity_off = true;

  SetMass(mass);
  m_last_move = Wormux::global_time.Read() ;
}

//-----------------------------------------------------------------------------

Physics::~Physics ()
{}

//---------------------------------------------------------------------------//
//--                                                                       --//
//--                         Class Parameters SET/GET                      --//
//--                                                                       --//
//---------------------------------------------------------------------------//

// Set / Get positions

void Physics::SetPhysXY(double x, double y)
{
  m_pos_x.x0 = x;
  m_pos_y.x0 = y;
}

double Physics::GetPhysX() const 
{
  return m_pos_x.x0; 
}

double Physics::GetPhysY() const 
{ 
  return m_pos_y.x0; 
}

//-----------------------------------------------------------------------------

void Physics::SetPhysSize (double width, double height)
{
  m_phys_width = width ;
  m_phys_height = height ;
}

//-----------------------------------------------------------------------------

// Set / Get positions
void Physics::SetMass(double mass)
{
  m_mass = mass ;
}

double Physics::GetMass() const
{
  return m_mass ;
}

//-----------------------------------------------------------------------------

// Set the wind factor
void Physics::SetWindFactor (double factor)
{ 
  m_wind_factor = factor;
}

//-----------------------------------------------------------------------------

// Set the wind factor
void Physics::SetAirResistFactor (double factor)
{ 
  m_air_resist_factor = factor;
}

//-----------------------------------------------------------------------------

// Set the wind factor
void Physics::SetGravityFactor (double factor)
{ 
  m_gravity_factor = factor;
}

//-----------------------------------------------------------------------------

void Physics::SetSpeed (double length, double angle)
{
  DoubleVector vector ;

  InitVector(vector, length*cos(angle), length*sin(angle)) ;
  SetSpeedXY(vector);
}

//-----------------------------------------------------------------------------

void Physics::SetSpeedXY (DoubleVector vector)
{
  if (EgalZero(vector.x)) vector.x = 0;
  if (EgalZero(vector.y)) vector.y = 0;
  bool was_moving = IsMoving();

  m_pos_x.x1 = vector.x ;
  m_pos_y.x1 = vector.y ;
  m_motion_type = FreeFall ;

  if (!was_moving && IsMoving()) StartMoving();
}

//-----------------------------------------------------------------------------

void Physics::AddSpeed (double length, double angle)
{
  DoubleVector vector ;

  InitVector(vector, length*cos(angle), length*sin(angle)) ;
  AddSpeedXY (vector);
}

//-----------------------------------------------------------------------------

void Physics::AddSpeedXY (DoubleVector vector)
{
  if (EgalZero(vector.x)) vector.x = 0;
  if (EgalZero(vector.y)) vector.y = 0;
  bool was_moving = IsMoving();

  m_pos_x.x1 += vector.x ;
  m_pos_y.x1 += vector.y ;
  m_motion_type = FreeFall ;

  if (!was_moving && IsMoving()) StartMoving();
}

//-----------------------------------------------------------------------------

void Physics::GetSpeed(double &norm, double &angle)
{
  DoubleVector speed ;

  switch (m_motion_type) {
    case FreeFall:
      GetSpeedXY(speed);
      norm = Norm (speed);
      angle = GetSpeedAngle();
      break ;

    case Pendulum:
      // Transform angular speed to linear speed.
      
      norm = fabs(m_rope_length.x0 * m_rope_angle.x1);
      
      if (m_rope_angle.x1 > 0)
	angle = fabs(m_rope_angle.x0) ;
      else
	angle = fabs(m_rope_angle.x0) - M_PI ;
      
      if (m_rope_angle.x0 < 0)
	angle = -angle ;
      break;

    default:
      break ;
  }
}

//-----------------------------------------------------------------------------

void Physics::GetSpeedXY(DoubleVector &vector)
{
  if(!IsMoving())
    {
      SetVectorZero(vector);
      return;
    }
  InitVector (vector, m_pos_x.x1, m_pos_y.x1);
}

//-----------------------------------------------------------------------------

double Physics::GetAngularSpeed()
{
  return m_rope_angle.x1 ;
}

//-----------------------------------------------------------------------------

double Physics::GetSpeedAngle()
{
  double angle ;
  DoubleVector speed ;

  GetSpeedXY(speed);

  angle = CalculeAnglef (speed);

  return angle ;
}

//-----------------------------------------------------------------------------

void Physics::SetExternForce (double length, double angle)
{
  DoubleVector vector ;

  InitVector(vector, length*cos(angle), length*sin(angle)) ;
  SetExternForceXY(vector) ;
}

//-----------------------------------------------------------------------------

void Physics::SetExternForceXY (DoubleVector vector)
{
  bool was_moving = IsMoving();

  InitVector (m_extern_force, vector.x, vector.y);

  if (!was_moving && IsMoving()) StartMoving();
}

//-----------------------------------------------------------------------------

// Set fixation point positions

void Physics::SetPhysFixationPointXY(double g_x, double g_y, double dx,
				     double dy)
{
  double fix_point_x, fix_point_y ;
  double old_length ;

  DoubleVector V ;
  m_fix_point_gnd.x = g_x ;
  m_fix_point_gnd.y = g_y ;
  m_fix_point_dxy.x = dx ;
  m_fix_point_dxy.y = dy ;

  //  printf ("Fixation (%f,%f) dxy(%f,%f)\n",  g_x, g_y, dx, dy);

  fix_point_x = m_pos_x.x0 + dx ;
  fix_point_y = m_pos_y.x0 + dy ;

  old_length = m_rope_length.x0 ;
  m_rope_length.x0 = Point2d(fix_point_x,fix_point_y).Distance( Point2d(g_x,g_y) );

  if (m_motion_type == Pendulum)
    {
      // We was already fixed. By changing the fixation point, we have
      // to recompute the angular speed depending of the new rope length.
      m_rope_angle.x1 = m_rope_angle.x1 * old_length / m_rope_length.x0 ;
    }
  else
    {
      // We switch from a regular move to a pendulum move.
      // Compute the initial angle
      V.x = fix_point_x - g_x ;
      V.y = fix_point_y - g_y ;
      m_rope_angle.x0 = M_PI/2 - CalculeAnglef (V) ;

      // Convert the linear speed to angular speed.
      m_rope_angle.x1 = (m_pos_x.x1 * cos(m_rope_angle.x0) +
			 m_pos_y.x1 * sin(m_rope_angle.x0) ) / m_rope_length.x0;

      // Reset the angular acceleration.
      m_rope_angle.x2 = 0 ;

      bool was_moving = IsMoving();
      m_motion_type = Pendulum ;
      if (!was_moving && IsMoving()) StartMoving();
    }
}

void Physics::UnsetPhysFixationPoint()
{
  double speed_norm, angle ;

  GetSpeed (speed_norm, angle);

  angle = -angle ;

  SetSpeed(speed_norm, angle);

  m_pos_x.x2 = 0 ;
  m_pos_y.x2 = 0 ;

  SetEulerVectorZero(m_rope_angle);
  SetEulerVectorZero(m_rope_length);

  m_motion_type = FreeFall ;
}

void Physics::ChangePhysRopeSize(double dl)
{
  if ((dl < 0) && (m_rope_length.x0 < 0.5))
    return ;

  bool was_moving = IsMoving();
  
  m_rope_length.x0 += dl ;

  // Recompute angular speed depending on the new rope length.
  m_rope_angle.x1 = m_rope_angle.x1 * (m_rope_length.x0 - dl) / m_rope_length.x0 ;

  if (!was_moving && IsMoving()) StartMoving();
}

double Physics::GetRopeAngle()
{
  return m_rope_angle.x0 ;
}

double Physics::GetRopeLength()
{
  return m_rope_length.x0;
}


//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------//
//--                                                                       --//
//--                            Physical Simulation                        --//
//--                                                                       --//
//---------------------------------------------------------------------------//

//-----------------------------------------------------------------------------

void Physics::StartMoving()
{
  m_last_move = Wormux::global_time.Read();

  if (m_motion_type == NoMotion)
    m_motion_type = FreeFall ;  

#ifdef DEBUG_MSG_PHYSIQUE
//  COUT_DEBUG0 << "Start moving." << std::endl;
#endif
}

//-----------------------------------------------------------------------------

void Physics::StopMoving()
{
#ifdef DEBUG_MSG_PHYSIQUE
//  if (IsMoving()) COUT_PHYSIC << "Fin d'un mouvement." << std::endl;
#endif

  m_pos_x.x1 = 0 ;
  m_pos_x.x2 = 0 ;
  m_pos_y.x1 = 0 ;
  m_pos_y.x2 = 0 ;
  if (m_motion_type != Pendulum)
    m_motion_type = NoMotion ;

  SetVectorZero(m_extern_force);
}

//-----------------------------------------------------------------------------

bool Physics::IsMoving() const
{
  return ( (!EgalZero(m_pos_x.x1)) ||
	   (!EgalZero(m_pos_y.x1)) ||
	   (! VectorNull (m_extern_force)) ||
	   (m_motion_type != NoMotion) ) ;
//	   (m_motion_type == Pendulum) ) ;
}

//-----------------------------------------------------------------------------

bool Physics::IsFalling() const
{
  return ( ( m_motion_type == FreeFall ) &&
	   ( m_pos_y.x1 > 0.1) );
}

//-----------------------------------------------------------------------------

double Physics::GetContactSurface(double angle)
{
  double x_surface, y_surface ;

  // Very approximative contact surface

  x_surface = m_phys_width * sin(angle) ;
  x_surface *= x_surface ;
		  
  y_surface = m_phys_height * cos(angle) ;
  y_surface *= y_surface ;

  return 0.5 * (x_surface + y_surface) ;
}

//-----------------------------------------------------------------------------

// Compute the next position of the object during a pendulum motion.
void Physics::ComputePendulumNextXY (double delta_t)
{
  //  double l0 = 5.0 ;

  //  printf ("Physics::ComputePendulumNextXY - Angle %f\n", m_rope_angle.x0);

  // Elactic rope length equation
  // l" + D.l' + (k/m - a'^2).l = g.cos a + k/m . l0

//   ComputeOneEulerStep(m_rope_length,
// 		      /* a */ 1,
// 		      /* b */ m_elasticity_damping,
// 		      /* c */ m_rope_elasticity / m_mass - m_rope_angle.x1 * m_rope_angle.x1,
// 		      /* d */ game_mode.gravity * cos (m_rope_angle.x0) + m_rope_elasticity/m_mass * l0,
//                       delta_t);

  // Pendulum motion equation (angle equation)
  // a'' + (D + 2.l'/l).a' = -g/l.sin a + F/l.cos a
  ComputeOneEulerStep(m_rope_angle,
		      /* a */ 1,
		      /* b */ m_balancing_damping + 2 * m_rope_length.x1 / m_rope_length.x0,
		      /* c */ 0,
		      /* d */ -game_mode.gravity / m_rope_length.x0 * sin (m_rope_angle.x0)
		              +m_extern_force.x / m_rope_length.x0 * cos (m_rope_angle.x0),
                      delta_t);

  double x = m_fix_point_gnd.x - m_fix_point_dxy.x 
             + m_rope_length.x0 * sin(m_rope_angle.x0);
  double y = m_fix_point_gnd.y - m_fix_point_dxy.y
             + m_rope_length.x0 * cos(m_rope_angle.x0);

  //  printf ("Physics::ComputePendulumNextXY - Angle(%f,%f,%f)\n",
  //  	  m_rope_angle.x0, m_rope_angle.x1, m_rope_angle.x2);

  SetPhysXY(x,y);
}

//-----------------------------------------------------------------------------

// Compute the next position of the object during a free fall.
void Physics::ComputeFallNextXY (double delta_t)
{
  double speed_norm, speed_angle ;
  double air_resistance_factor ;

  double weight_force ;
  double wind_force ;

  // Free fall motion equation
  // m.g + wind -k.v = m.a

  // Weight force = m * g

  //  printf ("av : (%5f,%5f) - (%5f,%5f) - (%5f,%5f)\n", m_pos_x.x0,
  //	  m_pos_y.x0, m_pos_x.x1, m_pos_y.x1, m_pos_x.x2, m_pos_y.x2);

  weight_force = game_mode.gravity * m_gravity_factor * m_mass ;

  // Wind force

  wind_force = wind.GetStrength() * m_wind_factor ;

  // Air resistanceance factor = K * contact_surface

  GetSpeed(speed_norm, speed_angle);

  air_resistance_factor = AIR_RESISTANCE_FACTOR * GetContactSurface(speed_angle) * m_air_resist_factor ;

  // Equation on X axys : m.x'' + k.x' = wind
  ComputeOneEulerStep(m_pos_x, m_mass, air_resistance_factor, 0,
		      wind_force + m_extern_force.x, delta_t);

  // Equation on Y axys : m.y'' + k.y' = m.g
  ComputeOneEulerStep(m_pos_y, m_mass, air_resistance_factor, 0,
		      weight_force + m_extern_force.y, delta_t);

  //  printf ("F : Pd(%5f) EF(%5f)\n", weight_force, m_extern_force.y);

  //  printf ("ap : (%5f,%5f) - (%5f,%5f) - (%5f,%5f)\n", m_pos_x.x0,
  //	  m_pos_y.x0, m_pos_x.x1, m_pos_y.x1, m_pos_x.x2, m_pos_y.x2);
}

//-----------------------------------------------------------------------------

// Compute the position of the object at current time.
void Physics::ComputeNextXY (double &x, double &y, double delta_t)
{
  if (m_motion_type == FreeFall)
    ComputeFallNextXY(delta_t);

  if (m_motion_type == Pendulum)
    ComputePendulumNextXY(delta_t);

  m_last_move = Wormux::global_time.Read() ;

  x = m_pos_x.x0 ;
  y = m_pos_y.x0 ;
}


//-----------------------------------------------------------------------------

void Physics::RunPhysicalEngine ()
{
  double step_t, delta_t = (Wormux::global_time.Read() - m_last_move) / 1000.0;
  double old_x,old_y;
  double new_x,new_y;
  double contact_x, contact_y, contact_angle;
  bool contact = false;

  step_t = PHYS_DELTA_T;

  //  printf ("Delta_t = %f (last %f - current %f)\n", delta_t, m_last_move/1000.0,
  //	  Wormux::global_time.Read()/1000.0);

  // Compute object move for each physical engine time step.

  while (delta_t > 0.0)
    {
      if (delta_t < PHYS_DELTA_T)
        step_t = delta_t ;

      old_x = GetPhysX();
      old_y = GetPhysY();
      
      ComputeNextXY(new_x,new_y, step_t);

      if ( (new_x != old_x) || (new_y != old_y))
	// The object has moved. Notify the son class.
	contact = NotifyMove(old_x, old_y, new_x, new_y,
			     contact_x, contact_y, contact_angle);

      if (contact)
	{
#ifdef DEBUG_MSG_PHYSIQUE
	  COUT_PHYSIC << "Collision durant le déplacement "
		      << '(' << old_x << ',' << old_y 
		      << " -> " << new_x << ',' << new_y << ")." << std::endl;
#endif
	  Rebound(contact_x, contact_y, contact_angle);
	}

      delta_t -= PHYS_DELTA_T ;
    }

  return;
}

//-----------------------------------------------------------------------------

void Physics::Rebound(double contact_x, double contact_y, double contact_angle)
{
  double norme, angle;

  // Get norm and angle of the object speed vector.
  
  GetSpeed(norme, angle);

  switch (m_motion_type) {
    case FreeFall :
      if (m_rebounding)
	{ 
	  // Compute rebound angle.
	  
	  if(contact_angle == -1.0)
	    {
	      angle = angle + M_PI ;
	    }
	  else
	    angle -= 2.0 * (angle - contact_angle);

	  // Apply rebound factor to the object speed.
	  
	  norme = norme * m_rebound_factor ;
	  	  
	  // Apply the new speed to the object.
	  
	  SetSpeed(norme, angle);
	  
	  // Check if we should stop rebounding.
	  
	  if (norme < STOP_REBOUND_LIMIT)
	    {
	      StopMoving();
	      return;
	    }
	  
	  SignalRebound();
	}
      else
	{
	  SignalFallEnding();
	  StopMoving();
	}

      break ;

    case Pendulum:
      DoubleVector V ;

      // Recompute new angle.
      
      V.x = m_pos_x.x0 + m_fix_point_dxy.x - m_fix_point_gnd.x ;
      V.y = m_pos_y.x0 + m_fix_point_dxy.y - m_fix_point_gnd.y ;
      m_rope_angle.x0 = M_PI/2 - CalculeAnglef (V) ;
      
      // Convert the linear speed of the rebound to angular speed.

      V.x = PENDULUM_REBOUND_FACTOR * norme * cos(angle);
      V.y = PENDULUM_REBOUND_FACTOR * norme * sin(angle);

      angle = angle + M_PI ;

      m_rope_angle.x1 = (norme * cos(angle) * cos(m_rope_angle.x0) +
			 norme * sin(angle) * sin(m_rope_angle.x0) ) /
	                 m_rope_length.x0;

      m_rope_angle.x2 = 0 ;
      SetVectorZero(m_extern_force) ;
      break ;

    default:
      break ;
  }

}

//-----------------------------------------------------------------------------

void Physics::SignalGhostState(bool)  {}
void Physics::SignalDeath() {}
void Physics::SignalDrowning() {}
void Physics::SignalFallEnding() {}
void Physics::SignalRebound() {}

//-----------------------------------------------------------------------------
