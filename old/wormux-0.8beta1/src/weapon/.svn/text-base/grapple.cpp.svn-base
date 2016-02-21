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
 * Grapple
 *****************************************************************************/

#include "grapple.h"
#include <math.h>
#include "explosion.h"
#include "game/config.h"
#include "game/game.h"
#include "game/game_loop.h"
#include "game/time.h"
#include "include/app.h"
#include "include/action_handler.h"
#include "map/camera.h"
#include "map/map.h"
#include "sound/jukebox.h"
#include "team/teams_list.h"
#include "tool/math_tools.h"
#include "tool/i18n.h"

const int DT_MVT = 15 ; //delta_t between 2 up/down/left/right mvt
const int DST_MIN = 80 ;  //dst_minimal between 2 nodes

bool find_first_contact_point (Point2i from, double angle, int length,
			       int skip, Point2i &contact_point)
{
  Point2d posd;
  double x_step, y_step ;
  Point2i pos2;

  x_step = cos(angle) ;
  y_step = sin(angle) ;

  from.x += (int)(skip * x_step) ;
  from.y += (int)(skip * y_step) ;

  posd.x = (double)from.x ;
  posd.y = (double)from.y ;
  contact_point.x = from.x ;
  contact_point.y = from.y ;

  length -= skip;

  pos2.x = from.x + (int)(length * cos(angle));
  pos2.y = from.y + (int)(length * sin(angle));

  while(!world.IsOutsideWorld(contact_point) &&
	(length > 0))
    {
      if (!world.IsInVacuum(contact_point))
	return true ;

      posd.x += x_step ;
      posd.y += y_step ;
      contact_point.x = (int)round(posd.x) ;
      contact_point.y = (int)round(posd.y) ;
      length-- ;
    }

  return false ;
}

Grapple::Grapple() : Weapon(WEAPON_GRAPPLE, "grapple", new GrappleConfig())
{
  m_name = _("Grapple");
  m_category = MOVE;
  use_unit_on_first_shoot = false;

  m_hook_sprite = resource_manager.LoadSprite(weapons_res_profile,"grapple_hook");
  m_hook_sprite->EnableRotationCache(32);
  m_node_sprite = resource_manager.LoadSprite(weapons_res_profile,"grapple_node");

  m_is_active = false;
  m_attaching = false;
  go_left = false ;
  go_right = false ;
  delta_len = 0 ;
}

bool Grapple::p_Shoot()
{
  last_broken_node_angle = 100;

  m_attaching = true;
  m_launch_time = Time::GetInstance()->Read() ;
  m_initial_angle = ActiveCharacter().GetFiringAngle();
  last_mvt=Time::GetInstance()->Read();

  TryAttachRope();
  return true;
}

bool Grapple::TryAttachRope()
{
  Point2i pos;
  uint length;
  uint delta_time = Time::GetInstance()->Read() - m_launch_time;
  double angle ;

  // Remove the root node
  rope_nodes.clear();

  // The rope is being launching. Increase the rope length and check
  // collisions.

  Point2i handPos = ActiveCharacter().GetHandPosition();
  pos = handPos;

  length = cfg().automatic_growing_speed * delta_time / 10;
  if (length > cfg().max_rope_length)
    {
      // Hum the roe is too short !
      m_attaching = false;
      m_is_active = false;

      // Give back one ammo...
      int *ammo = &ActiveTeam().AccessNbAmmos();
      if (*ammo != INFINITE_AMMO) (*ammo)++;
      assert (*ammo > 0 || *ammo == INFINITE_AMMO);

      return false;
    }

  angle = m_initial_angle;

  Point2i contact_point;
  if (find_first_contact_point(pos, angle, length, 4, contact_point))
    {
      if (!ActiveTeam().IsLocal() && !ActiveTeam().IsLocalAI())
	return false;

      // The rope reaches the fixation point. Let's fix it !
      Action* a = new Action(Action::ACTION_WEAPON_GRAPPLE);
      a->Push(ATTACH_ROPE);
      a->Push(contact_point);
      ActionHandler::GetInstance()->NewAction(a);

      MSG_DEBUG("grapple.hook", "Creating ATTACH_GRAPPLE Action");
      return true;
    }

  rope_node_t root_node;
  root_node.pos.x = pos.x + int(length * cos(angle));
  root_node.pos.y = pos.y + int(length * sin(angle));
  root_node.angle = 0;
  root_node.sense = 0;
  rope_nodes.push_back(root_node);

  return false;
}

bool Grapple::TryAddNode(int CurrentSense)
{
  int lg;
  Point2d V;
  Point2i contact_point;
  double angle, rope_angle;

  Point2i handPos = ActiveCharacter().GetHandPosition();

  // Compute distance between hands and rope fixation point.

  V.x = handPos.x - m_fixation_point.x;
  V.y = handPos.y - m_fixation_point.y;
  angle = V.ComputeAngle();
  lg = (int)V.Norm();

  if (lg < DST_MIN)
    return false;

  // Check if the rope collide something

  if (find_first_contact_point(m_fixation_point, angle, lg, 2, contact_point))
    {
      rope_angle = ActiveCharacter().GetRopeAngle() ;

      if ( (last_broken_node_sense * CurrentSense > 0) &&
	   (fabs(last_broken_node_angle - rope_angle) < 0.1))
	return false ;

      // The rope has collided something...
      // Add a node on the rope and change the fixation point
      AttachNode(contact_point, rope_angle, CurrentSense);

      // Send node addition over the network
      Action a(Action::ACTION_WEAPON_GRAPPLE);
      a.Push(ATTACH_NODE);
      a.Push(contact_point);
      a.Push(rope_angle);
      a.Push(CurrentSense);
      Network::GetInstance()->SendAction(&a);

      return true;
    }

  return false;
}

bool Grapple::TryBreakNode(int currentSense)
{
  bool breakNode = false;

  // Check if we can break a node.
  int nodeSense = rope_nodes.back().sense;
  double nodeAngle = rope_nodes.back().angle;
  double angularSpeed = ActiveCharacter().GetAngularSpeed();
  double currentAngle = ActiveCharacter().GetRopeAngle();

  if ( (rope_nodes.size() != 1) &&              // We cannot break the initial node.
       (nodeSense * currentSense < 0) ) // Cannot break a node if we are in the
                                        // same sense of the node.
    {
      if ( (currentAngle > 0) &&
	   (angularSpeed > 0) &&
	   (currentAngle > nodeAngle))
	breakNode = true ;

      if ( (currentAngle > 0) &&
	   (angularSpeed < 0) &&
	   (currentAngle < nodeAngle))
	breakNode = true ;

      if ( (currentAngle < 0) &&
	   (angularSpeed > 0) &&
	   (currentAngle > nodeAngle))
	breakNode = true ;

      if ( (currentAngle < 0) &&
	   (angularSpeed < 0) &&
	   (currentAngle < nodeAngle))
	breakNode = true ;

//       if ( (currentAngle < 0) &&
// 	   (angularSpeed > 0) &&
// 	   (currentAngle < nodeAngle))
// 	breakNode = true;


    }

  // We can break the current node... Let's do it !

  if (breakNode)
    {
      last_broken_node_angle = currentAngle ;
      last_broken_node_sense = currentSense ;

      // remove last node
      DetachNode();

      // Send node suppression over the network
      Action a(Action::ACTION_WEAPON_GRAPPLE);
      a.Push(DETACH_NODE);
      Network::GetInstance()->SendAction(&a);
    }

  return breakNode ;
}

void Grapple::NotifyMove(bool collision)
{
  bool addNode = false;
  double angularSpeed;
  int currentSense;

  if (!m_is_active || m_attaching)
    return;

  if (!ActiveTeam().IsLocal() && !ActiveTeam().IsLocalAI())
    return;

  // Check if the character collide something.
  if (collision)
    {
      // Yes there has been a collision.
      if (delta_len != 0)
	{
	  // The character tryed to change the rope size.
	  // There has been a collision, so we cancel the rope length change.
	  ActiveCharacter().ChangePhysRopeSize (-delta_len);
	  delta_len = 0;
	}
      return;
    }

  angularSpeed = ActiveCharacter().GetAngularSpeed() ;
  currentSense = (int)(angularSpeed / fabs(angularSpeed)) ;

  // While there is nodes to add, we add !
  while (TryAddNode(currentSense))
    addNode = true;

  // If we have created nodes, we exit to avoid breaking what we
  // have just done !
  if (addNode)
    return;

  // While there is nodes to break, we break !
  while (TryBreakNode(currentSense));
}

void Grapple::Refresh()
{
  if (!m_is_active)
    return ;

  if (m_attaching)
    TryAttachRope();

  if (!ActiveTeam().IsLocal() && !ActiveTeam().IsLocalAI())
    return;

  ActiveCharacter().UpdatePosition();
  SendActiveCharacterInfo(true);
}

void Grapple::Draw()
{
  int x, y;
  double angle, prev_angle;

  struct CL_Quad {Sint16 x1,x2,x3,x4,y1,y2,y3,y4;} quad;

  Weapon::Draw();

  if (!m_is_active)
  {
    return ;
  }

  if (m_attaching) {
    angle = m_initial_angle + M_PI/2;
  }
  else {
    angle = ActiveCharacter().GetRopeAngle();
  }

  prev_angle = angle;

  // Draw the rope.
  Point2i handPos = ActiveCharacter().GetHandPosition();
  x = handPos.x;
  y = handPos.y;

  quad.x1 = (int)round((double)x - 2 * cos(angle));
  quad.y1 = (int)round((double)y + 2 * sin(angle));
  quad.x2 = (int)round((double)x + 2 * cos(angle));
  quad.y2 = (int)round((double)y - 2 * sin(angle));

  for (std::list<rope_node_t>::reverse_iterator it = rope_nodes.rbegin();
       it != rope_nodes.rend(); it++)
    {
      quad.x3 = (int)round((double)it->pos.x + 2 * cos(angle));
      quad.y3 = (int)round((double)it->pos.y - 2 * sin(angle));
      quad.x4 = (int)round((double)it->pos.x - 2 * cos(angle));
      quad.y4 = (int)round((double)it->pos.y + 2 * sin(angle));

      float dx = sin(angle) * (float)m_node_sprite->GetHeight();
      float dy = cos(angle) * (float)m_node_sprite->GetHeight();
      int step = 0;
      int size = (quad.x1-quad.x4) * (quad.x1-quad.x4)
                +(quad.y1-quad.y4) * (quad.y1-quad.y4);
      size -= m_node_sprite->GetHeight();
      while( (step*dx*step*dx)+(step*dy*step*dy) < size )
      {
        if(m_attaching)
          m_node_sprite->Draw(Point2i(quad.x1 + (int)((float) step * dx),
				      quad.y1 - (int)((float) step * dy)));
        else
          m_node_sprite->Draw(Point2i(quad.x4 + (int)((float) step * dx),
				      quad.y4 + (int)((float) step * dy)));
        step++;
      }
      quad.x1 = quad.x4 ;
      quad.y1 = quad.y4 ;
      quad.x2 = quad.x3 ;
      quad.y2 = quad.y3 ;
      prev_angle = angle;
      angle = it->angle ;
    }

  m_hook_sprite->SetRotation_rad(-prev_angle);
  m_hook_sprite->Draw( rope_nodes.front().pos - m_hook_sprite->GetSize()/2);
}

void Grapple::p_Deselect()
{
  DetachRope();
}

// force detaching rope if time is out
void Grapple::SignalTurnEnd()
{
  p_Deselect();
}

void Grapple::ActionStopUse()
{
  DetachRope();
}


void Grapple::AttachRope(Point2i contact_point)
{
  MSG_DEBUG("grapple.hook", "** AttachRope %d,%d", contact_point.x, contact_point.y);

  m_attaching = false;
  m_is_active = true;

  rope_nodes.clear();

  // The rope reaches the fixation point. Let's fix it !
  Point2i handPos = ActiveCharacter().GetHandPosition();
  Point2i pos(handPos.x - ActiveCharacter().GetX(),
	      handPos.y - ActiveCharacter().GetY());

  ActiveCharacter().SetPhysFixationPointXY(
					   contact_point.x / PIXEL_PER_METER,
					   contact_point.y / PIXEL_PER_METER,
					   (double)pos.x / PIXEL_PER_METER,
					   (double)pos.y / PIXEL_PER_METER);

  m_fixation_point = contact_point;

  rope_node_t root_node;
  root_node.pos = m_fixation_point;
  root_node.angle = 0;
  root_node.sense = 0;
  rope_nodes.push_back(root_node);

  ActiveCharacter().ChangePhysRopeSize (-10.0 / PIXEL_PER_METER);
  m_hooked_time = Time::GetInstance()->Read();
  ActiveCharacter().SetMovement("ninja-rope");

  ActiveCharacter().SetFiringAngle(-M_PI / 3);

  // Camera should focus on it!
  camera.FollowObject (&ActiveCharacter(),true, true, true);
  camera.SetCloseFollowing(true);
}

void Grapple::DetachRope()
{
  ActiveCharacter().UnsetPhysFixationPoint() ;
  rope_nodes.clear();
  m_is_active = false;
  camera.SetCloseFollowing(false);
}

void Grapple::AttachNode(Point2i contact_point,
			   double angle,
			   int sense)
{
  Point2i handPos = ActiveCharacter().GetHandPosition();

  // The rope has collided something...
  // Add a node on the rope and change the fixation point.
  Point2i pos(handPos.x - ActiveCharacter().GetX(),
	      handPos.y - ActiveCharacter().GetY());

  ActiveCharacter().SetPhysFixationPointXY(contact_point.x / PIXEL_PER_METER,
					   contact_point.y / PIXEL_PER_METER,
					   (double)pos.x / PIXEL_PER_METER,
					   (double)pos.y / PIXEL_PER_METER);

  m_fixation_point = contact_point;
  rope_node_t node;
  node.pos = m_fixation_point;
  node.angle = angle;
  node.sense = sense;
  rope_nodes.push_back(node);

  MSG_DEBUG("grapple.node", "+ %d,%d %f %d", node.pos.x, node.pos.y, node.angle, node.sense);
}

void Grapple::DetachNode()
{
  assert(rope_nodes.size() >= 1);

  { // for debugging only
    rope_node_t node;
    node = rope_nodes.back();
    MSG_DEBUG("grapple.node", "- %d,%d %f %d", node.pos.x, node.pos.y, node.angle, node.sense);
  }

  // remove last node
  rope_nodes.pop_back();

  m_fixation_point = rope_nodes.back().pos ;

  Point2i handPos = ActiveCharacter().GetHandPosition();
  int dx = handPos.x - ActiveCharacter().GetX();
  int dy = handPos.y - ActiveCharacter().GetY();

  ActiveCharacter().SetPhysFixationPointXY(m_fixation_point.x / PIXEL_PER_METER,
					   m_fixation_point.y / PIXEL_PER_METER,
					   (double)dx / PIXEL_PER_METER,
					   (double)dy / PIXEL_PER_METER);
}

// =========================== Moves management

void Grapple::SetRopeSize(double length)
{
  double delta = length - ActiveCharacter().GetRopeLength();
  ActiveCharacter().ChangePhysRopeSize (delta);
}

void Grapple::GoUp()
{
  if(Time::GetInstance()->Read()<last_mvt+DT_MVT)
    return;
  last_mvt = Time::GetInstance()->Read();

  delta_len = -0.1 ;
  ActiveCharacter().ChangePhysRopeSize (delta_len);
  ActiveCharacter().UpdatePosition();
  delta_len = 0 ;
}

void Grapple::GoDown()
{
  if(Time::GetInstance()->Read()<last_mvt+DT_MVT)
    return;
  last_mvt = Time::GetInstance()->Read();

  if (ActiveCharacter().GetRopeLength() >= cfg().max_rope_length / PIXEL_PER_METER)
    return;

  delta_len = 0.1 ;
  ActiveCharacter().ChangePhysRopeSize (delta_len) ;
  ActiveCharacter().UpdatePosition() ;
  delta_len = 0 ;
}

void Grapple::GoRight()
{
  go_right = true ;
  ActiveCharacter().SetExternForce(cfg().push_force,0);
  ActiveCharacter().SetDirection(Body::DIRECTION_RIGHT);
}

void Grapple::StopRight()
{
  go_right = false ;

  if (go_left || go_right)
    return ;

  ActiveCharacter().SetExternForce(0,0);
}

void Grapple::GoLeft()
{
  go_left = true ;
  ActiveCharacter().SetExternForce(-cfg().push_force,0);
  ActiveCharacter().SetDirection(Body::DIRECTION_LEFT);
}

void Grapple::StopLeft()
{
  go_left = false ;

  if (go_left || go_right)
    return ;

  ActiveCharacter().SetExternForce(0,0);
}

// =========================== Keys management

void Grapple::HandleKeyPressed_Up()
{
  if (m_is_active)
    GoUp();
  else
    ActiveCharacter().HandleKeyPressed_Up();
}

void Grapple::HandleKeyRefreshed_Up()
{
  if (m_is_active)
    GoUp();
  else
    ActiveCharacter().HandleKeyRefreshed_Up();
}

void Grapple::HandleKeyReleased_Up()
{
  if (!m_is_active)
    ActiveCharacter().HandleKeyReleased_Up();
}

void Grapple::HandleKeyPressed_Down()
{
  if (m_is_active)
    GoDown();
  else
    ActiveCharacter().HandleKeyPressed_Down();
}

void Grapple::HandleKeyRefreshed_Down()
{
  if (m_is_active)
    GoDown();
  else
    ActiveCharacter().HandleKeyRefreshed_Down();
}

void Grapple::HandleKeyReleased_Down()
{
  if (!m_is_active)
    ActiveCharacter().HandleKeyReleased_Down();
}

void Grapple::HandleKeyPressed_MoveLeft()
{
  if (m_is_active)
    GoLeft();
  else
    ActiveCharacter().HandleKeyPressed_MoveLeft();
}

void Grapple::HandleKeyRefreshed_MoveLeft()
{
  if (!m_is_active)
    ActiveCharacter().HandleKeyRefreshed_MoveLeft();
}

void Grapple::HandleKeyReleased_MoveLeft()
{
  if (m_is_active)
    StopLeft();
  else
    ActiveCharacter().HandleKeyReleased_MoveLeft();
}

void Grapple::HandleKeyPressed_MoveRight()
{
  if (m_is_active)
    GoRight();
  else
    ActiveCharacter().HandleKeyPressed_MoveRight();
}

void Grapple::HandleKeyRefreshed_MoveRight()
{
  if (!m_is_active)
    ActiveCharacter().HandleKeyRefreshed_MoveRight();
}

void Grapple::HandleKeyReleased_MoveRight()
{
  if (m_is_active)
    StopRight();
  else
    ActiveCharacter().HandleKeyReleased_MoveRight();
}

void Grapple::HandleKeyPressed_Shoot()
{
  if (m_is_active) {
    NewActionWeaponStopUse();
  } else
    NewActionWeaponShoot();
}

void Grapple::HandleKeyRefreshed_Shoot(){}

void Grapple::HandleKeyReleased_Shoot(){}

void Grapple::PrintDebugRope()
{
  printf("%05d %05d %03.3f\n",
	 ActiveCharacter().GetPosition().GetX(),
	 ActiveCharacter().GetPosition().GetY(),
	 ActiveCharacter().GetRopeAngle());

  for (std::list<rope_node_t>::iterator it = rope_nodes.begin();
       it != rope_nodes.end();
       it++) {

    printf("%05d %05d %03.3f %d\n", it->pos.x, it->pos.y,
	   it->angle, it->sense);
  }
}

std::string Grapple::GetWeaponWinString(const char *TeamName, uint items_count )
{
  return Format(ngettext(
            "%s team has won %u grapple!",
            "%s team has won %u grapples!",
            items_count), TeamName, items_count);
}

//-----------------------------------------------------------------------------

GrappleConfig& Grapple::cfg()
{
  return static_cast<GrappleConfig&>(*extra_params);
}
//-----------------------------------------------------------------------------

GrappleConfig::GrappleConfig()
{
  max_rope_length = 450;
  automatic_growing_speed = 12;
  push_force = 10;
}

void GrappleConfig::LoadXml(xmlpp::Element *elem)
{
  EmptyWeaponConfig::LoadXml(elem);
  XmlReader::ReadUint(elem, "max_rope_length", max_rope_length);
  XmlReader::ReadUint(elem, "automatic_growing_speed", automatic_growing_speed);
  XmlReader::ReadInt(elem, "push_force", push_force);
}
