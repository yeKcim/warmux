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
 *****************************************************************************/

#include <map>
#include <iostream>
#include "character/body.h"
#include "character/character.h"
#include "character/member.h"
#include "character/movement.h"
#include "game/game.h"
#include "graphic/sprite.h"
#include "team/teams_list.h"
#include "team/team.h"
#include "tool/resource_manager.h"
#include "tool/string_tools.h"
#include "tool/xml_document.h"

Member::Member(const xmlNode* xml, const Profile* res):
  parent(NULL),
  angle_rad(0),
  anchor(0,0),
  spr(NULL),
  name(""),
  attached_members(),
  pos(0,0),
  scale(0,0),
  alpha(0),
  type(""),
  go_through_ground(false)
{
  if (xml == NULL)
    return;
  XmlReader::ReadStringAttr(xml, "name", name);
  ASSERT(name!="");

  // Load the sprite
  spr = resource_manager.LoadSprite( res, name);
  //spr->EnableRotationCache(32);
  //spr->EnableFlippingCache();
  spr->cache.EnableLastFrameCache();

  // Get the various option
  XmlReader::ReadStringAttr(xml, "type", type);
  ASSERT(type!="");

  const xmlNode* el = XmlReader::GetMarker(xml, "anchor");
  if (el != NULL)
  {
    int dx = 0, dy = 0;
    XmlReader::ReadIntAttr(el, "dx", dx);
    XmlReader::ReadIntAttr(el, "dy", dy);
    MSG_DEBUG("body", "   Member %s has anchor (%i,%i)\n", name.c_str(), dx, dy);
    anchor = Point2f((float)dx,(float)dy);
    spr->SetRotation_HotSpot(Point2i(dx,dy));
  }
  else
    MSG_DEBUG("body", "   Member %s has no anchor\n", name.c_str());

  XmlReader::ReadBoolAttr(xml, "go_through_ground", go_through_ground);

  xmlNodeArray nodes = XmlReader::GetNamedChildren(xml, "attached");
  xmlNodeArray::const_iterator it;

  for (it = nodes.begin(); it != nodes.end(); ++it)
  {
    std::string att_type;
    if (!XmlReader::ReadStringAttr(*it, "member_type", att_type))
    {
      std::cerr << "Malformed attached member definition" << std::endl;
      continue;
    }

    int dx = 0, dy = 0;
    XmlReader::ReadIntAttr(*it, "dx", dx);
    XmlReader::ReadIntAttr(*it, "dy", dy);
    MSG_DEBUG("body", "   Attached member %s has anchor (%i,%i)\n", att_type.c_str(), dx, dy);
    Point2f d((float)dx, (float)dy);

    std::string frame_str;
    XmlReader::ReadStringAttr(*it, "frame", frame_str);
    if (frame_str == "*")
    {
      v_attached rot_spot;
      rot_spot.assign (spr->GetFrameCount(), d);
      attached_members[att_type] = rot_spot;
    }
    else
    {
      int frame;
      if (!str2int (frame_str, frame) || frame < 0 || frame >= (int)spr->GetFrameCount())
      {
        std::cerr << "Malformed attached member definition (wrong frame number)" << std::endl;
        continue;
      }
      if(attached_members.find(att_type) == attached_members.end())
      {
        v_attached rot_spot;
        rot_spot.resize(spr->GetFrameCount(), Point2f(0.0,0.0));
        attached_members[att_type] = rot_spot;
      }
      (attached_members.find(att_type)->second)[frame] = d;
    }
  }

  ResetMovement();
}

Member::Member(const Member& m):
  parent(NULL),
  angle_rad(m.angle_rad),
  anchor(m.anchor),
  spr(new Sprite(*m.spr)),
  name(m.name),
  attached_members(),
  pos(m.pos),
  scale(m.scale),
  alpha(m.alpha),
  type(m.type),
  go_through_ground(m.go_through_ground)
{
  Point2i rot = Point2i((int)anchor.x, (int)anchor.y);
  spr->SetRotation_HotSpot(rot);

  for (std::map<std::string, v_attached>::const_iterator it = m.attached_members.begin();
      it != m.attached_members.end();
      ++it)
  {
    attached_members[it->first] = it->second;
  }
  ResetMovement();
}

Member::~Member()
{
  delete spr;
  attached_members.clear();
}

void Member::RotateSprite()
{
  spr->SetRotation_rad(angle_rad);
  spr->Scale(scale.x, scale.y);
  spr->RefreshSurface();
}

void Member::Draw(const Point2i & _pos, int flip_center, int direction)
{
  ASSERT(name != "weapon" && type!="weapon");

  Point2i posi((int)pos.x, (int)pos.y);
  posi += _pos;

  if(direction == 1)
  {
    spr->SetRotation_rad(angle_rad);
    spr->Scale(scale.x,scale.y);
  }
  else
  {
    spr->Scale(-scale.x,scale.y);
    spr->SetRotation_rad(-angle_rad);
    posi.x = 2 * flip_center - posi.x - spr->GetWidth();
  }

  if(parent == NULL && type != "body")
  {
    std::cerr << "Error : Member " << name << " have no parent member!" << std::endl;
    return;
  }

  spr->SetAlpha(alpha);
  spr->Update();
  spr->Draw(posi);
}

void Member::ApplySqueleton(Member* parent_member)
{
  // Place the member to shape the skeleton
  if(parent_member == NULL)
  {
    std::cerr << "Member " << name << " have no parent member!" << std::endl;
    return;
  }
  parent = parent_member;

  ASSERT(parent->name != "weapon" && parent->type != "weapon");

  // Set the position
  pos = parent->pos;
  pos = pos - anchor;

  if(parent->attached_members.find(type) != parent->attached_members.end())
    pos = pos + parent->attached_members.find(type)->second[parent->spr->GetCurrentFrame()];
}

void Member::ApplyMovement(const member_mvt &mvt, std::vector<junction>& skel_lst)
{
  // Apply the movment to the member,
  // And apply the movement accordingly to the child members

  uint frame;
  if(spr!=NULL) // spr == NULL when Member is the weapon
    frame = spr->GetCurrentFrame();
  else
    frame = 0;

  // We first apply to the child (makes calcules simpler in this order):
  for(std::map<std::string, v_attached>::iterator child = attached_members.begin();
      child != attached_members.end();
      child++ )
  {
    // Find this member in the skeleton:
    for(std::vector<junction>::iterator member = skel_lst.begin();
        member != skel_lst.end();
        member++)
    {
      if(member->member->type == child->first)
      {
        // Calculate the movement to apply to the child
        member_mvt child_mvt;
        child_mvt.SetAngle(mvt.GetAngle());
        child_mvt.pos = mvt.pos;
        float radius = anchor.Distance(child->second[frame]);

        if(radius != 0.0)
        {
          float angle_init;

          if(child->second[frame].x > anchor.x)
          {
            if(child->second[frame].y > anchor.y)
              angle_init = acos( (child->second[frame].x - anchor.x) / radius );
            else
              angle_init = -acos( (child->second[frame].x - anchor.x) / radius );
          }
          else
          {
            if(child->second[frame].y > anchor.y)
              angle_init = acos( (child->second[frame].x - anchor.x) / radius );
            else
              angle_init = M_PI + acos( -(child->second[frame].x - anchor.x) / radius );
          }

          child_mvt.pos.x += radius * (cos(angle_init + angle_rad + mvt.GetAngle()) - cos(angle_init + angle_rad));
          child_mvt.pos.y += radius * (sin(angle_init + angle_rad + mvt.GetAngle()) - sin(angle_init + angle_rad));
        }
        // Apply recursively to children:
        member->member->ApplyMovement(child_mvt, skel_lst);
      }
    }
  }

  // Apply the movement to the current member
  SetAngle(angle_rad + mvt.GetAngle());
  pos += mvt.pos;
  alpha *= mvt.alpha;
  scale = scale * mvt.scale;
}

WeaponMember::WeaponMember() : Member(NULL, NULL)
{
  name = "weapon";
  type = "weapon";
  spr = NULL;
  anchor = Point2f(0.0,0.0);
}

WeaponMember::~WeaponMember()
{
}

void WeaponMember::Draw(const Point2i & /*_pos*/, int /*flip_center*/, int /*direction*/)
{
  if (!ActiveCharacter().IsDead() && Game::GetInstance()->ReadState() != Game::END_TURN)
    {
      ActiveTeam().crosshair.Draw();
      ActiveTeam().AccessWeapon().Draw();
    }
}
