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

#include <iostream>
#include "object/physical_obj.h"
#include "object/physics.h"
#include "object/objects_list.h"
#include "character/character.h"
#include "game/config.h"
#include "game/time.h"
#include "map/map.h"
#include "network/randomsync.h"
#include "sound/jukebox.h"
#include "team/macro.h"
#include "team/team.h"
#include "team/teams_list.h"
#include "tool/debug.h"
#include "tool/isnan.h"
#include "tool/math_tools.h"
#include "tool/point.h"
#include "tool/random.h"
#include "tool/rectangle.h"
#include "weapon/weapon_launcher.h"

const int Y_OBJET_MIN = -10000;
const int WATER_RESIST_FACTOR = 40;

const double PIXEL_PER_METER = 40;

double MeterDistance (const Point2i &p1, const Point2i &p2)
{
  return p1.Distance(p2) / PIXEL_PER_METER;
}

PhysicalObj::PhysicalObj (const std::string &name, const std::string &xml_config) :
  m_goes_through_wall(false),
  m_collides_with_characters(false),
  m_collides_with_objects(false),
  m_rebound_position(-1,-1),
  last_collision_type(NO_COLLISION),
  // No collision with this object until we have gone out of his collision rectangle
  m_overlapping_object(NULL),
  m_minimum_overlapse_time(0),
  m_ignore_movements(false),
  m_is_character(false),
  m_name(name),
  m_test_left(0),
  m_test_right(0),
  m_test_top(0),
  m_test_bottom(0),
  m_width(0),
  m_height(0),
  m_rebound_sound(""),
  m_alive(ALIVE),
  energy(-1),
  m_allow_negative_y(false)
{
  m_cfg = Config::GetInstance()->GetOjectConfig(m_name,xml_config);
  ResetConstants();       // Set physics constants from the xml file

  MSG_DEBUG("physical.mem", "Construction of %s", m_name.c_str());
}

PhysicalObj::~PhysicalObj ()
{
  MSG_DEBUG("physical.mem", "Destruction of %s", m_name.c_str());
}

//---------------------------------------------------------------------------//
//--                         Class Parameters SET/GET                      --//
//---------------------------------------------------------------------------//

void PhysicalObj::SetXY(const Point2i &position)
{
  CheckOverlapping();

  if( IsOutsideWorldXY( position ) )
  {
    Point2d physPos(position.x, position.y);
    SetPhysXY( physPos / PIXEL_PER_METER );
    Ghost();
    SignalOutOfMap();
  }
  else
  {
    Point2d physPos(position.x, position.y);
    SetPhysXY( physPos / PIXEL_PER_METER );
    if( FootsInVacuum() ) StartMoving();
  }
}

void PhysicalObj::SetXY(const Point2d &position)
{
  CheckOverlapping();

  if( IsOutsideWorldXY( Point2i(int(position.x), int(position.y)) ) )
  {
    SetPhysXY( position / PIXEL_PER_METER );
    Ghost();
    SignalOutOfMap();
  }
  else
  {
    SetPhysXY( position / PIXEL_PER_METER );
    if( FootsInVacuum() ) StartMoving();
  }
}

int PhysicalObj::GetX() const { return (int)round(GetPhysX() * PIXEL_PER_METER); };
int PhysicalObj::GetY() const { return (int)round(GetPhysY() * PIXEL_PER_METER); };

void PhysicalObj::SetSize(const Point2i &newSize){
  if( newSize == Point2i(0, 0) )
          Error( "New size of (0, 0) !");
  m_width = newSize.x;
  m_height = newSize.y;
  SetPhysSize( (double)newSize.x / PIXEL_PER_METER, (double)newSize.y/PIXEL_PER_METER );
}

void PhysicalObj::SetOverlappingObject(PhysicalObj* obj, int timeout)
{
  m_minimum_overlapse_time = 0;
  if(obj != NULL) {
    m_overlapping_object = obj;
    lst_objects.AddOverlappedObject(this);
    MSG_DEBUG("physic.overlapping", "\"%s\" doesn't check any collision with \"%s\" anymore during %d ms",
              GetName().c_str(), obj->GetName().c_str(), timeout);
  } else {
    if(m_overlapping_object != NULL) {
      m_overlapping_object = NULL;
      lst_objects.RemoveOverlappedObject(this);
      MSG_DEBUG( "physic.overlapping", "clearing overlapping object in \"%s\"", GetName().c_str());
    }
    return;
  }
  if (timeout > 0)
    m_minimum_overlapse_time = Time::GetInstance()->Read() + timeout;

  CheckOverlapping();
}

void PhysicalObj::CheckOverlapping()
{
  if (m_overlapping_object == NULL)
    return;

  // Check if we are still overlapping with this object
  if (!m_overlapping_object->GetTestRect().Intersect( GetTestRect() ) &&
      m_minimum_overlapse_time <= Time::GetInstance()->Read())
  {
    MSG_DEBUG("physic.overlapping", "\"%s\" just stopped overlapping with \"%s\" (%d ms left)", 
	      GetName().c_str(), m_overlapping_object->GetName().c_str(),
	      (m_minimum_overlapse_time - Time::GetInstance()->Read()));
    SetOverlappingObject(NULL);
  }
  else
  {
    MSG_DEBUG("physic.overlapping", "\"%s\" is overlapping with \"%s\"", 
	       GetName().c_str(), m_overlapping_object->GetName().c_str());
  }
}

void PhysicalObj::SetTestRect (uint left, uint right, uint top, uint bottom)
{
  m_test_left =  left;
  m_test_right = right;
  m_test_top = top;
  m_test_bottom = bottom;
}

void PhysicalObj::SetEnergyDelta(int delta, bool /*do_report*/)
{
  if(energy == -1)
    return;
  energy += delta;
  if(energy <= 0 && !IsGhost())
  {
    Ghost();
    energy = -1;
  }
}

// Move to a point with collision test
// Return true if collision occured
void PhysicalObj::NotifyMove(Point2d oldPos, Point2d newPos)
{
  if (IsGhost())
    return;

  Point2d pos, offset;
  PhysicalObj* collided_obj = NULL;

  last_collision_type = NO_COLLISION;

  if (IsGhost())
    return;

  // Convert meters to pixels.
  oldPos *= PIXEL_PER_METER;
  newPos *= PIXEL_PER_METER;

  // Compute distance between old and new position.
  double lg = oldPos.Distance( newPos);

  MSG_DEBUG("physic.move", "%s moves (%f, %f) -> (%f, %f), distance: %f",
            typeid(*this).name(), oldPos.x, oldPos.y, newPos.x, newPos.y, lg);

  if (lg == 0)
    return;

  // Compute increments to move the object step by step from the old
  // to the new position.
  offset = (newPos - oldPos) / lg;

  // First iteration position.
  pos = oldPos + offset;

  if (m_goes_through_wall || IsInWater())
  {
    MSG_DEBUG("physic.move", "%s moves (%f, %f) -> (%f, %f), thr_wall:%d, water:%d",
              typeid(*this).name(), oldPos.x, oldPos.y, newPos.x, newPos.y,
              m_goes_through_wall, IsInWater());

    SetXY(newPos);
    return;
  }

  do
  {
    Point2i tmpPos( lround(pos.x), lround(pos.y) );

    // Check if we exit the world. If so, we stop moving and return.
    if( IsOutsideWorldXY(tmpPos) ){

      if( !world.IsOpen() ){
        tmpPos.x = InRange_Long(tmpPos.x, 0, world.GetWidth() - GetWidth() - 1);
        tmpPos.y = InRange_Long(tmpPos.y, 0, world.GetHeight() - GetHeight() - 1);
        MSG_DEBUG( "physic.state", "%s - DeplaceTestCollision touche un bord : %d, %d",  m_name.c_str(), tmpPos.x, tmpPos.y );
        last_collision_type = COLLISION_ON_GROUND;
        break;
      }

      SetXY( pos );

      MSG_DEBUG("physic.move", "%s moves (%f, %f) -> (%f, %f) : OUTSIDE WORLD",
                typeid(*this).name(), oldPos.x, oldPos.y, newPos.x, newPos.y);
      return;
    }

    // Test if we collide...
    collided_obj = CollidedObjectXY(tmpPos);
    if( collided_obj != NULL)
      MSG_DEBUG( "physic.state", "%s collide on %s", m_name.c_str(), collided_obj->GetName().c_str() );

    if(collided_obj != NULL)
      last_collision_type = COLLISION_ON_OBJECT;
    else
    if(!IsInVacuumXY(tmpPos, false))
      last_collision_type = COLLISION_ON_GROUND;

    if(last_collision_type != NO_COLLISION) // Nothing more to do!
    {
      MSG_DEBUG( "physic.state", "%s - Collision at %d,%d : %s", m_name.c_str(), tmpPos.x, tmpPos.y,
          last_collision_type == COLLISION_ON_GROUND ? "on ground" : "on an object");

      // Set the object position to the current position.
      SetXY(Point2d(pos.x - offset.x, pos.y - offset.y));
      break;
    }

    // Next motion step
    pos += offset;
    lg -= 1.0 ;
  } while (0 < lg);

  // Notify the weapon that there is a movement
  // Useful for grapple for example
  ActiveTeam().AccessWeapon().NotifyMove(!!last_collision_type);

  if (last_collision_type == NO_COLLISION ) // Nothing more to do!
    return;
  if (last_collision_type == COLLISION_ON_GROUND ) {
      // Find the contact point and collision angle.
//       // !!! ContactPoint(...) _can_ return false when CollisionTest(...) is true !!!
//       // !!! WeaponProjectiles collide on objects, so computing the tangeante to the ground leads
//       // !!! uninitialised values of cx and cy!!
//       if( ContactPoint(cx, cy) ){
    int cx, cy;
    Point2d contactPos;
    double ground_angle;

    if (ContactPoint(cx, cy)) {
      ground_angle = world.ground.Tangent(cx, cy);
      if(!isNaN(ground_angle)) {
        contactPos.x = (double)cx / PIXEL_PER_METER;
        contactPos.y = (double)cy / PIXEL_PER_METER;
      } else {
        ground_angle = - GetSpeedAngle();
        contactPos = pos;
      }
    } else {
      ground_angle = - GetSpeedAngle();
      contactPos = pos;
    }

    SignalGroundCollision();
    SignalCollision();
    // Make it rebound on the ground !!
    MSG_DEBUG("physic.state", "Rebound on %s at %d,%d", m_name.c_str(), contactPos.x, contactPos.y);
    Rebound(contactPos, ground_angle);
    CheckRebound();
  } else if (last_collision_type == COLLISION_ON_OBJECT ) {
    SignalObjectCollision(collided_obj);
    collided_obj->SignalObjectCollision(this);

    // Get the current speed
    double v1, v2, mass1, angle1, angle2, mass2;
    collided_obj->GetSpeed(v1, angle1);
    GetSpeed(v2, angle2);
    mass1 = GetMass();
    mass2 = collided_obj->GetMass();

    // Give speed to the other object
    // thanks to physic and calculations about chocs, we know that :
    //
    // v'1 =  ((m1 - m2) * v1 + 2m1 *v2) / (m1 + m2)
    // v'2 =  ((m2 - m1) * v2 + 2m1 *v1) / (m1 + m2)
    SignalCollision();

    collided_obj->SetSpeed(((mass1 - mass2) * v1 + 2 * mass1 *v2 * m_cfg.m_rebound_factor) / (mass1 + mass2),
                           angle1);
    SetSpeed(((mass2 - mass1) * v2 + 2 * mass1 *v1 * m_cfg.m_rebound_factor) / (mass1 + mass2), angle2);

    // Rebound on the object
    double contact_angle = - GetSpeedAngle();
    Point2d contactPos = pos;
    MSG_DEBUG("physic.state", "Rebound on %s at %d,%d", m_name.c_str(), contactPos.x, contactPos.y);
    Rebound(contactPos, contact_angle);
    CheckRebound();
  }
  return;
}

void PhysicalObj::UpdatePosition ()
{
  // No ghost allowed here !
  if (IsGhost()) return;

  if ( !m_goes_through_wall )
    {
      // object is not moving and has no reason to move
      if ( !IsMoving() && !FootsInVacuum() && !IsInWater() ) return;

      // object is not moving BUT it should fall !
      if ( !IsMoving() && FootsInVacuum() ) StartMoving();
    }

  // Compute new position.
  RunPhysicalEngine();

  if (IsGhost()) return;

  // Classical object sometimes sinks in water and sometimes goes out of water!
  if ( !m_goes_through_wall )
    {
      if ( IsInWater() && m_alive != DROWNED && m_alive != DEAD) Drown();
      else if ( !IsInWater() && m_alive == DROWNED ) GoOutOfWater();
    }

}

bool PhysicalObj::PutOutOfGround(double direction)
{
  if(IsOutsideWorld(Point2i(0, 0)))
    return false;

  const int max_step = 30;

  if( IsInVacuum(Point2i(0, 0), false) )
    return true;

  double dx = cos(direction);
  double dy = sin(direction);

  int step=1;
  while(step<max_step && !IsInVacuum(
                          Point2i((int)(dx * (double)step),(int)(dy * (double)step)), false ))
    step++;

  if(step<max_step)
    SetXY( Point2i((int)(dx * (double)step)+GetX(),(int)(dy * (double)step)+GetY()) );
  else
    return false; //Can't put the object out of the ground

  return true;
}

bool PhysicalObj::PutOutOfGround()
{
  if(IsOutsideWorld(Point2i(0, 0)))
    return false;

  if( IsInVacuum(Point2i(0, 0)) )
    return true;

  bool left,right,top,bottom;
  left   = world.IsInVacuum_left(*this, 0, 0);
  right  = world.IsInVacuum_right(*this, 0, 0);
  top    = world.IsInVacuum_top(*this, 0, 0);
  bottom = world.IsInVacuum_bottom(*this, 0, 0);

  int dx = (int)GetTestRect().GetSizeX() * (right-left);
  int dy = (int)GetTestRect().GetSizeY() * (top-bottom);

  if( dx == 0 && dy == 0 )
    return false; //->Don't know in which direction we should go...

  Point2i b(dx, dy);

  double dir = b.ComputeAngle();
  return PutOutOfGround(dir);
}

void PhysicalObj::Init()
{
  if (m_alive != ALIVE)
    MSG_DEBUG( "physic.state", "%s - Init.", m_name.c_str());
  m_alive = ALIVE;
  SetOverlappingObject(NULL);
  StopMoving();
}

void PhysicalObj::Ghost ()
{
  if (m_alive == GHOST)
    return;

  bool was_dead = IsDead();
  m_alive = GHOST;
  MSG_DEBUG("physic.state", "%s - Ghost, was_dead = %d", m_name.c_str(), was_dead);

  // The object became a gost
  StopMoving();

  SignalGhostState(was_dead);
}

void PhysicalObj::Drown()
{
  ASSERT (m_alive != DROWNED);
  MSG_DEBUG("physic.state", "%s - Drowned...", m_name.c_str());
  m_alive = DROWNED;

  // Set the air grab to water resist factor.
  SetAirResistFactor(WATER_RESIST_FACTOR * GetAirResistFactor());

  // Ensure the gravity factor is upper than 0.0
  if (EqualsZero(GetGravityFactor()))
    SetGravityFactor(0.1);

  StopMoving();
  StartMoving();
  SignalDrowning();
}

void PhysicalObj::GoOutOfWater()
{
  ASSERT (m_alive == DROWNED);
  MSG_DEBUG("physic.state", "%s - Go out of water!...", m_name.c_str());
  m_alive = ALIVE;

  // Set the air grab to normal air resist factor.
  SetAirResistFactor(m_cfg.m_air_resist_factor);
  SetGravityFactor(m_cfg.m_gravity_factor);
  StartMoving();
  SignalGoingOutOfWater();
}

void PhysicalObj::SignalRebound()
{
  // TO CLEAN...
   if (!m_rebound_sound.empty())
     jukebox.Play("share", m_rebound_sound) ;
}

void PhysicalObj::SetCollisionModel(bool goes_through_wall,
                                    bool collides_with_characters,
                                    bool collides_with_objects)
{
  m_goes_through_wall = goes_through_wall;
  m_collides_with_characters = collides_with_characters;
  m_collides_with_objects = collides_with_objects;

  // Check boolean values
  {
    if (m_collides_with_characters || m_collides_with_objects)
      ASSERT(m_goes_through_wall == false);

    if (m_goes_through_wall) {
      ASSERT(m_collides_with_characters == false);
      ASSERT(m_collides_with_objects == false);
    }
  }
}

void PhysicalObj::CheckRebound()
{
  // If we bounce twice in a row at the same place, stop bouncing
  // cause it's almost sure this object is stuck bouncing indefinitely
  if( m_rebound_position != Point2i( -1, -1) )
  {
    if ( m_rebound_position == GetPosition() )
    {
      MSG_DEBUG("physic.state", "%s seems to be stuck in ground. Stop moving!", m_name.c_str());
      StopMoving();
    }
  }
  m_rebound_position = GetPosition();
}

bool PhysicalObj::IsOutsideWorldXY(const Point2i& position) const{
  int x = position.x + m_test_left;
  int y = position.y + m_test_top;

  if( world.IsOutsideWorldXwidth(x, GetTestWidth()) )
          return true;
  if( world.IsOutsideWorldYheight(y, GetTestHeight()) ){
    if( m_allow_negative_y )
      if( (Y_OBJET_MIN <= y) && (y + GetTestHeight() - 1 < 0) )
                  return false;
    return true;
  }
  return false;
}

bool PhysicalObj::FootsOnFloor(int y) const
{
  // If outside is empty, the object can't hit the ground !
  if ( world.IsOpen() ) return false;

  const int y_max = world.GetHeight()-m_height +m_test_bottom;
  return (y_max <= y);
}

bool PhysicalObj::IsInVacuumXY(const Point2i &position, bool check_object) const
{
  if( IsOutsideWorldXY(position) )
    return world.IsOpen();

  if( FootsOnFloor(position.y - 1) )
    return false;

  if( check_object && CollidedObjectXY(position) )
    return false;

  Rectanglei rect(position.x + m_test_left, position.y + m_test_top,
                  m_width - m_test_right - m_test_left, m_height -m_test_bottom - m_test_top);

  return world.RectIsInVacuum (rect);
}

PhysicalObj* PhysicalObj::CollidedObjectXY(const Point2i & position) const
{
  if( IsOutsideWorldXY(position) )
    return NULL;

  Rectanglei rect(position.x + m_test_left, position.y + m_test_top,
                 m_width - m_test_right - m_test_left, m_height - m_test_bottom - m_test_top);

  if (m_collides_with_characters)
    {
      FOR_ALL_LIVING_CHARACTERS(team,character)
      {
        // We check both objet if one overlapse the other
        if (&(*character) != this && !IsOverlapping(&(*character)) && !character->IsOverlapping(this)
        && character->GetTestRect().Intersect( rect ))
          return (PhysicalObj*) &(*character);
      }
    }

  if (m_collides_with_objects)
    {
      FOR_EACH_OBJECT(it)
      {
        PhysicalObj * object=*it;
        // We check both objet if one overlapse the other
        if (object != this && !IsOverlapping(object) && !object->IsOverlapping(this)
            && object->m_collides_with_objects
            && object->GetTestRect().Intersect(rect) )
        {
          if (!m_is_character || object->m_collides_with_characters)
            return object;
        }
      }
    }
  return NULL;
}

bool PhysicalObj::FootsInVacuumXY(const Point2i &position) const
{
  if( IsOutsideWorldXY(position) ){
    MSG_DEBUG("physical", "%s - physobj is outside the world", m_name.c_str());
    return world.IsOpen();
  }

  if( FootsOnFloor(position.y) ){
    MSG_DEBUG("physical", "%s - physobj is on floor", m_name.c_str());
    return false;
  }

  int y_test = position.y + m_height - m_test_bottom;

  Rectanglei rect( position.x + m_test_left, y_test,
                   m_width - m_test_right - m_test_left, 1);

  if( m_allow_negative_y && rect.GetPositionY() < 0){
    int b = rect.GetPositionY() + rect.GetSizeY();

    rect.SetPositionY( 0 );
    rect.SetSizeY( ( b > 0 ) ? b - rect.GetPositionY() : 0 );
  }

  if(CollidedObjectXY( position + Point2i(0, 1)) != NULL )
    return false;

  return world.RectIsInVacuum (rect);
}

bool PhysicalObj::IsInWater () const
{
  ASSERT (!IsGhost());
  if (!world.water.IsActive()) return false;
  int x = InRange_Long(GetCenterX(), 0, world.GetWidth()-1);
  return (int)world.water.GetHeight(x) < GetCenterY();
}

void PhysicalObj::DirectFall()
{
  while (!IsGhost() && !IsInWater() && FootsInVacuum())
    SetY((int)(GetY()+1.0));
}

bool PhysicalObj::ContactPoint (int & contact_x, int & contact_y) const
{
  int x1, x2, y1, y2;

  // We are looking for a point in contact with the bottom of the object:
  y1 = (GetY()+m_height-m_test_bottom);
  y2 = y1-1;
  for (uint x=GetX()+ m_test_left; x<=(GetX()+m_width)-m_test_right; x++)
  {
    if(!world.IsOutsideWorld(Point2i(x,y1)) && !world.IsOutsideWorld(Point2i(x,y2))
    && world.ground.IsEmpty(Point2i(x,y2)) && !world.ground.IsEmpty(Point2i(x,y1)))
    {
      contact_x = x;
      contact_y = GetY() +m_height-m_test_bottom;
      return true;
    }
  }

  // We are looking for a point in contact on the left hand of object:
  x1 = GetX()+m_test_left;
  x2 = x1+1;
  for(uint y=GetY()+m_test_top;y<=GetY()+m_height-m_test_bottom;y++)
  {
    if(!world.IsOutsideWorld(Point2i(x1,y)) && !world.IsOutsideWorld(Point2i(x2,y))
    && !world.ground.IsEmpty(Point2i(x1,y)) &&  world.ground.IsEmpty(Point2i(x2,y)))
    {
      contact_x = GetX() +m_test_left;
      contact_y = y;
      return true;
    }
  }

  // We are looking for a point in contact on the rigth hand of object:
  x1 = (GetX()+m_width-m_test_right);
  x2 = x1-1;
  for(uint y=GetY()+m_test_top;y<=GetY()+m_height-m_test_bottom;y++)
  {
    if(!world.IsOutsideWorld(Point2i(x1, y)) && !world.IsOutsideWorld(Point2i(x2, y))
       && !world.ground.IsEmpty(Point2i(x1, y)) && world.ground.IsEmpty(Point2i(x2, y)))
    {
      contact_x = GetX() + m_width - m_test_right;
      contact_y = y;
      return true;
    }
  }

  // We are looking for a point in contact on top of object:
  y1 = GetY()+m_test_top;
  y2 = y1 - 1;
  for(uint x=GetX()+m_test_left;x<=GetX()+m_width-m_test_right;x++)
  {
    if(!world.IsOutsideWorld(Point2i(x,y1)) && !world.IsOutsideWorld(Point2i(x,y2))
    && !world.ground.IsEmpty(Point2i(x, y1)) && world.ground.IsEmpty(Point2i(x, y2)))
    {
      contact_x =x;
      contact_y = GetY() +m_test_top;
      return true;
    }
  }
  return false;
}

bool PhysicalObj::PutRandomly(bool on_top_of_world, double min_dst_with_characters, bool net_sync)
{
  uint bcl=0;
  uint NB_MAX_TRY = 60;
  bool ok;
  Point2i position;

  MSG_DEBUG("physic.position", "%s - Search a position...", m_name.c_str());

  do
  {
    bcl++;
    ok = true;
    Init();

    if (bcl >= NB_MAX_TRY) {
      MSG_DEBUG("physic.position", "%s - Impossible to find an initial position !!", m_name.c_str());
      return false;
    }

    if (on_top_of_world) {
      // Give a random position for x
      if(net_sync)
        position.x = randomSync.GetLong(0, world.GetWidth() - GetWidth());
      else
        position.x = Random::GetLong(0, world.GetWidth() - GetWidth());
      position.y = -GetHeight()+1;
    } else {
      if(net_sync)
        position = randomSync.GetPoint(world.GetSize() - GetSize() + 1);
      else
        position = Random::GetPoint(world.GetSize() - GetSize() + 1);
    }
    SetXY(position);
    MSG_DEBUG("physic.position", "%s (try %u/%u) - Test in %d, %d",
              m_name.c_str(), bcl, NB_MAX_TRY, position.x, position.y);

    // Check physical object is not in the ground
    ok &= !IsGhost() && world.ParanoiacRectIsInVacuum(GetTestRect())  && IsInVacuum( Point2i(0, 1) );
    if (!ok) {
      MSG_DEBUG("physic.position", "%s - Put it in the ground -> try again !", m_name.c_str());
      continue;
    }

    /* check if the area rigth under the object has a bottom on the ground */
    ok &= !world.ParanoiacRectIsInVacuum(Rectanglei(GetCenter().x, position.y, 1, world.GetHeight() -
             (WATER_INITIAL_HEIGHT + 30) - position.y));
    if (!ok) {
      MSG_DEBUG("physic.position", "%s - Put in outside the map or in water -> try again", m_name.c_str());
      continue;
    }

    DirectFall();

    // Check distance with characters
    FOR_ALL_LIVING_CHARACTERS(team, character) if (&(*character) != this)
    {
      if (min_dst_with_characters == 0) {

        if(Overlapse(*character)) {
            MSG_DEBUG("physic.position", "%s - Object is too close from character %s", m_name.c_str(), (*character).m_name.c_str());
            ok = false;
        }
      } else {
        Point2i p1 = character->GetCenter();
        Point2i p2 = GetCenter();
        double dst = p1.Distance( p2 );

        // ok this test is not perfect but quite efficient ;-)
        // else we need to check each distance between each "corner"
        if (dst < min_dst_with_characters) ok = false;
      }
    }

    if (ok && on_top_of_world) SetXY(position);
  } while (!ok);

  MSG_DEBUG("physic.position", "Putted after %u try", m_name.c_str(), bcl);

  return true;
}
