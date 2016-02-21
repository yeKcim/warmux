/******************************************************************************
 *  Wormux is a convivial mass murder game.
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
 *****************************************************************************/

#include <map>
#include <iostream>
#include "member.h"
#include "../game/game_loop.h"
#include "../team/teams_list.h"
#include "../tool/math_tools.h"
#include "../tool/xml_document.h"
#include "../tool/string_tools.h"

Member::Member(xmlpp::Element *xml, Profile* res)
{
  parent = NULL;
  if(xml == NULL) return;
  name="";
  LitDocXml::LitAttrString( xml, "name", name);
  assert(name!="");

  // Load the sprite
  spr = resource_manager.LoadSprite( res, name);
//  spr->EnableRotationCache(64);
//  spr->EnableFlippingCache();
//  spr->cache.EnableLastFrameCache();

  // Get the various option
  type="";
  LitDocXml::LitAttrString( xml, "type", type);
  assert(type!="");

  xmlpp::Element *el = LitDocXml::AccesBalise (xml, "anchor");
  if(el != 0)
  {
    int dx,dy;
    dx = dy = 0;
    LitDocXml::LitAttrInt( el, "dx", dx);
    LitDocXml::LitAttrInt( el, "dy", dy);
    anchor = Point2f((float)dx,(float)dy);
    spr->SetRotation_HotSpot(Point2i(dx,dy));
  }

  go_through_ground = false;
  LitDocXml::LitAttrBool(xml, "go_through_ground", go_through_ground);

  xmlpp::Node::NodeList nodes = xml -> get_children("attached");
  xmlpp::Node::NodeList::iterator
    it=nodes.begin(),
    end=nodes.end();

  for (; it != end; ++it)
  {
    xmlpp::Element *elem = dynamic_cast<xmlpp::Element*> (*it);
    assert (elem != NULL);
    std::string att_type;
    if (!LitDocXml::LitAttrString(elem, "member_type", att_type))
    {
      std::cerr << "Malformed attached member definition" << std::endl;
      continue;
    }

    int dx,dy;
    dx = dy = 0;
    LitDocXml::LitAttrInt(elem, "dx", dx);
    LitDocXml::LitAttrInt(elem, "dy", dy);
    Point2f d((float)dx, (float)dy);

    std::string frame_str;
    LitDocXml::LitAttrString(elem, "frame", frame_str);
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
}

Member::Member(Member* m)
{
  parent = NULL;
  name = m->name;
  spr = new Sprite(*m->spr);
  type = m->type;
  anchor = m->anchor;
  Point2i rot = Point2i((int)anchor.x, (int)anchor.y);
  spr->SetRotation_HotSpot(rot);
  go_through_ground = m->go_through_ground;

  for(std::map<std::string, v_attached>::iterator it = m->attached_members.begin();
      it != m->attached_members.end();
      ++it)
  {
    attached_members[it->first] = it->second;
  }
}

Member::~Member()
{
  delete spr;
  attached_members.clear();
}

void Member::RotateSprite()
{
  spr->SetRotation_deg(angle);
  spr->Scale(scale.x, scale.y);
  spr->RefreshSurface();
}

void Member::Draw(const Point2i & _pos, int flip_center, int direction)
{
  assert(name != "weapon" && type!="weapon");

  Point2i posi((int)pos.x, (int)pos.y);
  posi += _pos;

  if(direction == 1)
  {
    spr->SetRotation_deg(angle);
    spr->Scale(scale.x,scale.y);
  }
  else
  {
    spr->Scale(-scale.x,scale.y);
    spr->SetRotation_deg(-angle);
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

void Member::ResetMovement()
{
  pos.x = 0;
  pos.y = 0;
  angle = 0;
  alpha = 1.0;
  scale.x = 1.0;
  scale.y = 1.0;
}

void Member::ApplySqueleton(Member* parent_member)
{
  // Place the member to shape the squeleton
  if(parent_member == NULL)
  {
    std::cerr << "Member " << name << " have no parent member!" << std::endl;
    return;
  }
  parent = parent_member;

  assert(parent->name != "weapon" && parent->type != "weapon");

  // Set the position
  pos = parent->pos;
  pos = pos - anchor;

  if(parent->attached_members.find(type) != parent->attached_members.end())
    pos = pos + parent->attached_members.find(type)->second[parent->spr->GetCurrentFrame()];
}

void Member::ApplyMovement(member_mvt &mvt, std::vector<junction>& squel_lst)
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
    // Find this member in the squeleton:
    for(std::vector<junction>::iterator member = squel_lst.begin();
        member != squel_lst.end();
        member++)
    {
      if(member->member->type == child->first)
      {
        // Calculate the movement to apply to the child
        member_mvt child_mvt;
        child_mvt.angle = mvt.angle;
        child_mvt.pos = mvt.pos;
        float radius = anchor.Distance(child->second[frame]);

        if(radius != 0.0)
        {
          float angle_init;
          float angle_r = angle * M_PI / 180.0;
          float mvt_angle_r = mvt.angle * M_PI / 180.0;

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

          child_mvt.pos.x += radius * (cos(angle_init + angle_r + mvt_angle_r) - cos(angle_init + angle_r));
          child_mvt.pos.y += radius * (sin(angle_init + angle_r + mvt_angle_r) - sin(angle_init + angle_r));
        }
        // Apply recursively to childrens:
        member->member->ApplyMovement(child_mvt, squel_lst);
      }
    }
  }

  // Apply the movement to the current member
  angle += mvt.angle;
  pos += mvt.pos;
  alpha *= mvt.alpha;
  scale = scale * mvt.scale;
}

const Point2i Member::GetPos()
{
  Point2i posi((int)pos.x, (int)pos.y);
  return posi;
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

void WeaponMember::Draw(const Point2i & _pos, int flip_center, int direction)
{
  // Would be cool to display the weapon from here...
}
