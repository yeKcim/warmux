/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Wormux Team.
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

#include "weapon/grapple.h"
#include "weapon/weapon_cfg.h"

#include <WORMUX_types.h>
#include "weapon/explosion.h"
#include "character/character.h"
#include "game/config.h"
#include "game/game.h"
#include "game/time.h"
#include "graphic/sprite.h"
#include "include/action_handler.h"
#include "map/camera.h"
#include "map/map.h"
#include "sound/jukebox.h"
#include "team/team.h"
#include "team/teams_list.h"
#include "tool/math_tools.h"
#include "tool/resource_manager.h"
#include "tool/xml_document.h"
#include "tool/string_tools.h"

const uint DT_MVT = 15; //delta_t between 2 up/down/left/right mvt
const uint DST_MIN = 4;  //dst_minimal between 2 nodes
const int SKIP_DST = 8;

bool find_first_contact_point (Point2i from, Double angle, uint length,
                               int skip, Point2i &contact_point)
{
  Point2d posd;
  Double x_step, y_step;

  if ((int)length <= skip) /* to avoid integer overflow */
    return false;

  x_step = cos(angle);
  y_step = sin(angle);

  posd.x = (Double)from.x;
  posd.y = (Double)from.y;

  posd.x += ((Double)skip) * x_step;
  posd.y += ((Double)skip) * y_step;

  from.x = (int)round(posd.x);
  from.y = (int)round(posd.y);

  contact_point.x = from.x;
  contact_point.y = from.y;

  length -= skip;

  // make it return the last point still in vacuum
  Point2i new_contact_point = contact_point;
  bool contact_point_uncertain = true;
  while(!GetWorld().IsOutsideWorld(new_contact_point) &&
        (length > 0))
    {
      if ( !GetWorld().IsInVacuum( new_contact_point ) )
      {
        ASSERT( contact_point_uncertain || GetWorld().IsInVacuum( contact_point ) );

        // for uncertain contact points, see if it's in vacuum
        if ( contact_point_uncertain && !GetWorld().IsInVacuum( contact_point ) )
        {
           // it's not, so try our best to return a contact point in vacuum
           // try searching in area NxN around our original point and return
           // the closest pixel in vacuum

           // FIXME: can be optimized!
           const int search_radius = 5; // 121 pixels to search

           Point2i closest_point;
           Point2i cur;
           int closest_point_distance = 2 * search_radius * search_radius + 1;// max
           bool found = false;

           for ( int i = -search_radius; i <= search_radius; i ++ )
           {
             for ( int j = -search_radius; j <= search_radius; j ++ )
             {
               cur = contact_point + Point2i( i, j );
               if ( GetWorld().IsInVacuum( cur ) )
               {
                 // check for new closest
                 int distance = i * i + j * j;
                 if ( distance < closest_point_distance )
                 {
                   closest_point_distance = distance;
                   closest_point = cur;
                   found = true;
                 }
               }
             }
           }

           if ( found )
             contact_point = closest_point;
        }
        return true ;
      }

      contact_point = new_contact_point;
      contact_point_uncertain = false; //now we know that it's in vacuum
      posd.x += x_step ;
      posd.y += y_step ;
      new_contact_point.x = (int)round(posd.x) ;
      new_contact_point.y = (int)round(posd.y) ;
      length--;
    }

  return false ;
}

class GrappleConfig : public EmptyWeaponConfig
{
 public:
  uint max_rope_length; // Max rope length in pixels
  int push_force;

 public:
  GrappleConfig();
  void LoadXml(const xmlNode* elem);
};

//-----------------------------------------------------------------------------

Grapple::Grapple() :
  Weapon(WEAPON_GRAPPLE, "grapple", new GrappleConfig())
{
  UpdateTranslationStrings();

  m_category = MOVE;
  use_unit_on_first_shoot = false;

  m_hook_sprite = GetResourceManager().LoadSprite(weapons_res_profile,"grapple_hook");
  m_hook_sprite->EnableRotationCache(32);
  m_node_sprite = GetResourceManager().LoadSprite(weapons_res_profile,"grapple_node");

  attached = false;
  go_left = false;
  go_right = false;
  delta_len = 0;
  move_left_pressed = false;
  move_right_pressed = false;
  move_up_pressed = false;
  move_down_pressed = false;
}

void Grapple::UpdateTranslationStrings()
{
  m_name = _("Grapple");
  /* TODO: FILL IT */
  /* m_help = _(""); */
}

Grapple::~Grapple()
{
  if (m_hook_sprite) delete m_hook_sprite;
  if (m_node_sprite) delete m_node_sprite;
}

bool Grapple::p_Shoot()
{
  last_broken_node_angle = 100;

  last_mvt = Time::GetInstance()->Read();

  if (!TryAttachRope()) // We have failed to attach!
    return false;

  JukeBox::GetInstance()->Play("default", "weapon/grapple_attaching");
  return true;
}

bool Grapple::TryAttachRope()
{
  Point2i pos;
  uint length;
  Double angle;

  ASSERT(rope_nodes.empty());

  ActiveCharacter().GetHandPosition(pos);

  length = cfg().max_rope_length;
  angle = ActiveCharacter().GetFiringAngle();

  Point2i contact_point;
  if (find_first_contact_point(pos, angle, length, SKIP_DST, contact_point)) {
    AttachRope(contact_point);
    return true;
  }

  return false;
}

bool Grapple::TryAddNode()
{
  uint lg;
  Point2d V;
  Point2i contact_point;
  Double angle, rope_angle;
  Point2i handPos;

  ActiveCharacter().GetHandPosition(handPos);

  // Compute distance between hands and rope fixation point.

  V.x = handPos.x - m_fixation_point.x;
  V.y = handPos.y - m_fixation_point.y;
  angle = V.ComputeAngle();
  lg = static_cast<int>(V.Norm());

  if (lg < DST_MIN)
    return false;

  // Check if the rope collide something

  if (find_first_contact_point(m_fixation_point, angle, lg, SKIP_DST, contact_point))
    {
      rope_angle = ActiveCharacter().GetRopeAngle() ;

      // if contact point is the same as position of the last node
      // (can happen because of jitter applied in find_first_contact_point),
      // give up adding such node
      if ( rope_nodes.size() > 0 && rope_nodes.back().pos == contact_point )
        return false;

      // The rope has collided something...
      // Add a node on the rope and change the fixation point
      AttachNode(contact_point, rope_angle);

      return true;
    }

  return false;
}

void Grapple::TryRemoveNodes()
{
  std::list<rope_node_t>::reverse_iterator nodeit;

  int lg;
  Point2d V;
  Point2i handPos, contact_point;
  Double angle;

  ActiveCharacter().GetHandPosition(handPos);

  while (rope_nodes.size() > 1) {

    nodeit = rope_nodes.rbegin();
    ++nodeit;

    V.x = handPos.x - nodeit->pos.x;
    V.y = handPos.y - nodeit->pos.y;
    angle = V.ComputeAngle();
    lg = static_cast<int>(V.Norm());

    if (find_first_contact_point(nodeit->pos, angle, lg, SKIP_DST, contact_point))
      break;

    DetachNode();
  }
}

void Grapple::NotifyMove(bool collision)
{
  bool addNode = false;

  if (!attached)
    return;

  // Check if the character collide something.
  if (collision)
    {
      // Yes there has been a collision.
      if (delta_len != ZERO)
        {
          // The character tryed to change the rope size.
          // There has been a collision, so we cancel the rope length change.
          ActiveCharacter().ChangePhysRopeSize (-delta_len);
          delta_len = 0;
        }
      return;
    }


  // While there is nodes to add, we add !
  while (TryAddNode())
    addNode = true;

  // If we have created nodes, we exit to avoid breaking what we
  // have just done !
  if (addNode)
    return;

  TryRemoveNodes();
}

void Grapple::Refresh()
{
  if (!attached)
    return ;

  if (move_left_pressed && !move_right_pressed) {
    GoLeft();
  } else if (move_right_pressed && !move_left_pressed) {
    GoRight();
  }

  if (move_up_pressed && !move_down_pressed) {
    GoUp();
  } else if (move_down_pressed && !move_up_pressed) {
    GoDown();
  }

  ActiveCharacter().SetMovement("ninja-rope");
  ActiveCharacter().UpdatePosition();
}

void Grapple::Draw()
{
  int x, y;
  Double angle, prev_angle;
  Point2i handPos;

  struct CL_Quad {Sint16 x1,x2,x3,x4,y1,y2,y3,y4;} quad;

  Weapon::Draw();

  if (!attached) {
    return;
  }

  angle = ActiveCharacter().GetRopeAngle();
  prev_angle = angle;

  // Draw the rope.
  ActiveCharacter().GetHandPosition(handPos);
  x = handPos.x;
  y = handPos.y;

  quad.x1 = (int)round((Double)x - 2 * cos(angle));
  quad.y1 = (int)round((Double)y + 2 * sin(angle));
  quad.x2 = (int)round((Double)x + 2 * cos(angle));
  quad.y2 = (int)round((Double)y - 2 * sin(angle));

  for (std::list<rope_node_t>::reverse_iterator it = rope_nodes.rbegin();
       it != rope_nodes.rend(); it++)
    {
      quad.x3 = (int)round((Double)it->pos.x + 2 * cos(angle));
      quad.y3 = (int)round((Double)it->pos.y - 2 * sin(angle));
      quad.x4 = (int)round((Double)it->pos.x - 2 * cos(angle));
      quad.y4 = (int)round((Double)it->pos.y + 2 * sin(angle));

      Double dx = sin(angle) * (Double)m_node_sprite->GetHeight();
      Double dy = cos(angle) * (Double)m_node_sprite->GetHeight();
      int step = 0;
      int size = (quad.x1-quad.x4) * (quad.x1-quad.x4)
                +(quad.y1-quad.y4) * (quad.y1-quad.y4);
      size -= m_node_sprite->GetHeight();
      while( (step*dx*step*dx)+(step*dy*step*dy) < size ) {
	m_node_sprite->Draw(Point2i(quad.x4 + (int)((Double) step * dx),
				    quad.y4 + (int)((Double) step * dy)));
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

void Grapple::AttachRope(const Point2i& contact_point)
{
  MSG_DEBUG("grapple.hook", "** AttachRope %d,%d", contact_point.x, contact_point.y);

  attached = true;
  move_left_pressed = false;
  move_right_pressed = false;
  move_up_pressed = false;
  move_down_pressed = false;

  rope_nodes.clear();

  // The rope reaches the fixation point. Let's fix it !
  Point2i pos;
  ActiveCharacter().GetRelativeHandPosition(pos);

  ActiveCharacter().SetPhysFixationPointXY(
                                           contact_point.x / PIXEL_PER_METER,
                                           contact_point.y / PIXEL_PER_METER,
                                           (Double)pos.x / PIXEL_PER_METER,
                                           (Double)pos.y / PIXEL_PER_METER);

  m_fixation_point = contact_point;

  rope_node_t root_node;
  root_node.pos = m_fixation_point;
  root_node.angle = 0;
  rope_nodes.push_back(root_node);

  ActiveCharacter().ChangePhysRopeSize (((Double)(-10)) / PIXEL_PER_METER);
  ActiveCharacter().SetMovement("ninja-rope");

  ActiveCharacter().SetFiringAngle(-PI / 3);

  // Camera should focus on it!
  Camera::GetInstance()->FollowObject(&ActiveCharacter());
}

void Grapple::DetachRope()
{
  ActiveCharacter().UnsetPhysFixationPoint();
  rope_nodes.clear();
  attached = false;

  cable_sound.Stop();
}

void Grapple::AttachNode(const Point2i& contact_point, Double angle)
{
  // The rope has collided something...
  // Add a node on the rope and change the fixation point.
  Point2i pos;
  ActiveCharacter().GetRelativeHandPosition(pos);

  ActiveCharacter().SetPhysFixationPointXY(contact_point.x / PIXEL_PER_METER,
                                           contact_point.y / PIXEL_PER_METER,
                                           (Double)pos.x / PIXEL_PER_METER,
                                           (Double)pos.y / PIXEL_PER_METER);

  m_fixation_point = contact_point;
  rope_node_t node;
  node.pos = m_fixation_point;
  node.angle = angle;
  rope_nodes.push_back(node);

  MSG_DEBUG("grapple.node", "+ %d,%d %s", node.pos.x, node.pos.y, Double2str(node.angle).c_str());
}

void Grapple::DetachNode()
{
  ASSERT(rope_nodes.size() >= 1);

#ifdef DEBUG
  { // for debugging only
    rope_node_t node;
    node = rope_nodes.back();
    MSG_DEBUG("grapple.node", "- %d,%d %s", node.pos.x, node.pos.y, Double2str(node.angle).c_str());
  }
#endif

  // remove last node
  rope_nodes.pop_back();

  m_fixation_point = rope_nodes.back().pos ;

  Point2i pos;
  ActiveCharacter().GetRelativeHandPosition(pos);

  ActiveCharacter().SetPhysFixationPointXY(m_fixation_point.x / PIXEL_PER_METER,
                                           m_fixation_point.y / PIXEL_PER_METER,
                                           (Double)pos.x / PIXEL_PER_METER,
                                           (Double)pos.y / PIXEL_PER_METER);
}

// =========================== Moves management

void Grapple::SetRopeSize(Double length) const
{
  Double delta = length - ActiveCharacter().GetRopeLength();
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

void Grapple::StopUp()
{
  cable_sound.Stop();
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

void Grapple::StopDown()
{
  cable_sound.Stop();
}

void Grapple::GoRight()
{
  if (!go_right) {
    cable_sound.Play("default", "weapon/grapple_cable");
  }
  go_right = true ;
  ActiveCharacter().SetExternForce(cfg().push_force,0);
  ActiveCharacter().SetDirection(DIRECTION_RIGHT);
  ActiveCharacter().UpdatePosition() ;
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
  if (!go_left) {
    cable_sound.Play("default", "weapon/grapple_cable");
  }
  go_left = true ;
  ActiveCharacter().SetExternForce(-cfg().push_force,0);
  ActiveCharacter().SetDirection(DIRECTION_LEFT);
  ActiveCharacter().UpdatePosition() ;
}

void Grapple::StopLeft()
{
  go_left = false ;

  if (go_left || go_right)
    return ;

  ActiveCharacter().SetExternForce(0,0);
}

void Grapple::StartMovingLeft()
{
  move_left_pressed = true;
}

void Grapple::StopMovingLeft()
{
  move_left_pressed = false;
  StopLeft();
}

void Grapple::StartMovingRight()
{
  move_right_pressed = true;
}

void Grapple::StopMovingRight()
{
  move_right_pressed = false;
  StopRight();
}

void Grapple::StartMovingUp()
{
  move_up_pressed = true;
}

void Grapple::StopMovingUp()
{
  move_up_pressed = false;
  StopUp();
}

void Grapple::StartMovingDown()
{
  move_down_pressed = true;
  cable_sound.Play("default", "weapon/grapple_cable", -1);
}

void Grapple::StopMovingDown()
{
  move_down_pressed = false;
  StopDown();
}

bool Grapple::IsPreventingLRMovement()
{
  return attached;
}

bool Grapple::IsPreventingWeaponAngleChanges()
{
  return attached;
}

// =========================== Keys management

void Grapple::HandleKeyPressed_Up(bool /*slowly*/)
{
  if (attached)
    StartMovingUpForAllPlayers();
}

void Grapple::HandleKeyReleased_Up(bool /*slowly*/)
{
  if (attached)
    StopMovingUpForAllPlayers();
}

void Grapple::HandleKeyPressed_Down(bool /*slowly*/)
{
  if (attached)
    StartMovingDownForAllPlayers();
}

void Grapple::HandleKeyReleased_Down(bool /*slowly*/)
{
  if (attached)
    StopMovingDownForAllPlayers();
}

void Grapple::HandleKeyPressed_MoveLeft(bool /*slowly*/)
{
  if (attached)
    StartMovingLeftForAllPlayers();
}

void Grapple::HandleKeyReleased_MoveLeft(bool /*slowly*/)
{
  if (attached)
    StopMovingLeftForAllPlayers();
}

void Grapple::HandleKeyPressed_MoveRight(bool /*slowly*/)
{
  if (attached)
    StartMovingRightForAllPlayers();
}

void Grapple::HandleKeyReleased_MoveRight(bool /*slowly*/)
{
  if (attached)
    StopMovingRightForAllPlayers();
}

void Grapple::StartShooting()
{
  if (!attached)
    Weapon::StartShooting();
}

void Grapple::StopShooting()
{
  if (attached)
    DetachRope();
  else
    Weapon::StopShooting();
}

void Grapple::PrintDebugRope()
{
  std::cout << ActiveCharacter().GetX() << " " << ActiveCharacter().GetY() << " " << ActiveCharacter().GetRopeAngle() << std::endl;

  for (std::list<rope_node_t>::iterator it = rope_nodes.begin();
       it != rope_nodes.end();
       it++) {

    std::cout << it->pos.x << " " << it->pos.y << " " << it->angle << std::endl;
  }
}

std::string Grapple::GetWeaponWinString(const char *TeamName, uint items_count ) const
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
  push_force = 10;
}

void GrappleConfig::LoadXml(const xmlNode* elem)
{
  EmptyWeaponConfig::LoadXml(elem);
  XmlReader::ReadUint(elem, "max_rope_length", max_rope_length);
  XmlReader::ReadInt(elem, "push_force", push_force);
}
