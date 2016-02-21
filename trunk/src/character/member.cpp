/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
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

#include <iostream>
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

std::vector<std::string> MemberType::Map;

Member::Member(const std::string& name_)
  : parent(NULL)
  , angle_rad(0)
  , alpha(0)
  , go_through_ground(false)
  , pos(0,0)
  , scale(0,0)
  , spr(NULL)
  , name(name_)
  , type(name_)
  , anchor(0,0)
{
}

Member::Member(const xmlNode *     xml,
               const std::string & main_folder)
  : parent(NULL)
  , angle_rad(0)
  , alpha(0)
  , go_through_ground(false)
  , pos(0,0)
  , scale(0,0)
  , spr(NULL)
  , name("")
  , type("")
  , anchor(0,0)
{
  XmlReader::ReadStringAttr(xml, "name", name);
  ASSERT(name!="");

  // Load the sprite
  spr = GetResourceManager().LoadSprite(xml, name, main_folder);
  spr->EnableCaches(true, 0);

  // Get the various option
  std::string type_str;
  XmlReader::ReadStringAttr(xml, "type", type_str);
  ASSERT(type_str!="");
  type = MemberType(type_str);

  const xmlNode * el = XmlReader::GetMarker(xml, "anchor");

  if (el) {
    int dx = 0, dy = 0;
    XmlReader::ReadIntAttr(el, "dx", dx);
    XmlReader::ReadIntAttr(el, "dy", dy);
    MSG_DEBUG("body", "   Member %s has anchor (%i,%i)\n", name.c_str(), dx, dy);
    anchor = Point2i(dx, dy);
    spr->SetRotation_HotSpot(anchor);
  } else {
    MSG_DEBUG("body", "   Member %s has no anchor\n", name.c_str());
  }

  XmlReader::ReadBoolAttr(xml, "go_through_ground", go_through_ground);

  xmlNodeArray                 nodes = XmlReader::GetNamedChildren(xml, "attached");
  xmlNodeArray::const_iterator it    = nodes.begin();
  xmlNodeArray::const_iterator itEnd = nodes.end();

  std::string att_type;
  int         dx = 0;
  int         dy = 0;
  Point2i     d;     // TODO: Rename !!
  std::string frame_str;

  for (; it != itEnd; ++it) {
    //std::string att_type; // TODO lami : can be move ?!

    if (!XmlReader::ReadStringAttr(*it, "member_type", att_type)) {
      std::cerr << "Malformed attached member definition" << std::endl;
      continue;
    }
    MemberType type(att_type);

    XmlReader::ReadIntAttr(*it, "dx", dx);
    XmlReader::ReadIntAttr(*it, "dy", dy);
    MSG_DEBUG("body", "   Attached member %s has anchor (%i,%i)\n", att_type.c_str(), dx, dy);
    d.SetValues(dx, dy);
    XmlReader::ReadStringAttr(*it, "frame", frame_str);

    if ("*" == frame_str) {
      v_attached rot_spot;
      rot_spot.assign(spr->GetFrameCount(), d);
      attached_types[type] = rot_spot;
    } else {
      int frame;

      if (!str2int(frame_str, frame) || frame < 0 || frame >= (int)spr->GetFrameCount()) {
        std::cerr << "Malformed attached member definition (wrong frame number)" << std::endl;
        continue;
      }

      if (attached_types.find(type) == attached_types.end()) {
        v_attached rot_spot;
        rot_spot.resize(spr->GetFrameCount(), Point2i(0.0, 0.0));
        attached_types[type] = rot_spot;
      }
      (attached_types.find(type)->second)[frame] = d;
    }
  }

  AttachTypeMap::iterator attachment_it = attached_types.begin();
  for (; attachment_it != attached_types.end(); ++attachment_it)
    attachment_it->second.SetAnchor(anchor);

  ResetMovement();
}

Member::Member(const Member & m)
  : parent(NULL)
  , angle_rad(m.angle_rad)
  , alpha(m.alpha)
  , go_through_ground(m.go_through_ground)
  , pos(m.pos)
  , scale(m.scale)
  , spr(new Sprite(*m.spr))
  , name(m.name)
  , type(m.type)
  , anchor(m.anchor)
{
  spr->SetRotation_HotSpot(anchor);

  for (AttachTypeMap::const_iterator it = m.attached_types.begin();
       it != m.attached_types.end();
       ++it) {
    attached_types[it->first] = it->second;
  }

  // No need to copy attached_members, this will be rebuilt

  ResetMovement();
}

Member::~Member()
{
  delete spr;
  attached_members.clear();
  attached_types.clear();
}

bool Member::MustRefresh() const
{
  return spr->GetFrameCount() > 1;
}

void Member::RotateSprite()
{
  bool refreshSprite = false;

  if (spr->GetRotation_rad() != angle_rad) {
    spr->SetRotation_rad(angle_rad);
    refreshSprite = true;
  }

  if (spr->GetScaleX() != scale.x && spr->GetScaleY() != scale.y) {
    spr->Scale(scale.x, scale.y);
    refreshSprite = true;
  }

  if (refreshSprite) {
    spr->RefreshSurface();
  }
}

void Member::RefreshSprite(LRDirection direction)
{
  // The sprite pointer may be invalid at the weapon sprite.
  // Those are just asserts and not ASSERTs because they have never happened in fact
  assert(name != "weapon");
  assert(parent || type == "body");

  if (DIRECTION_RIGHT == direction) {
    spr->SetFlipped(false);
    spr->SetRotation_rad(angle_rad);
  } else {
    spr->SetFlipped(true);
    spr->SetRotation_rad(-angle_rad);
  }
  spr->Scale(scale.x, scale.y);
  spr->SetAlpha(alpha);
  spr->Update();
}

void Member::Draw(const Point2i & _pos,
                  int             flip_center,
                  LRDirection     direction)
{
  assert(name != "weapon");
  assert(parent || type == "body");

  Point2i posi(pos.x, pos.y);
  posi += _pos;

  if (DIRECTION_LEFT == direction) {
    posi.x = 2 * flip_center - posi.x - spr->GetWidth();
  }

  spr->Draw(posi);
}

void Member::ApplySqueleton(Member * parent_member)
{
  // Place the member to shape the skeleton
  ASSERT(parent_member);

  if (!parent_member) {
    std::cerr << "Member " << name << " have no parent member!" << std::endl;
    return;
  }
  parent = parent_member;

  ASSERT(parent->name != "weapon");

  // Set the position
  pos = parent->pos - anchor;

  AttachTypeMap::iterator itAttachedType = parent->attached_types.find(type);

  if (itAttachedType != parent->attached_types.end()) {
    pos += itAttachedType->second[parent->spr->GetCurrentFrame()].point;
  }
}

// We are building a cache in attached_members under the assumption that
// the member <-> attached_type is unique (ie unique squeleton)
void Member::ApplyMovement(const member_mvt &mvt)
{
  // Apply the movment to the member,
  // And apply the movement accordingly to the child members

  // spr == NULL when Member is the weapon
  uint frame = (spr) ? spr->GetCurrentFrame() : 0;

  // Do we have to propagate the movement at all to the child?
  bool check = mvt.GetAngle().IsNotZero();

  // We first apply to the child (makes computations simpler in this order):
  if (check) {
    for (uint i=0; i<attached_members.size(); i++) {
      // Calculate the movement to apply to the child
      member_mvt child_mvt;
      child_mvt.SetAngle(mvt.GetAngle());
      child_mvt.pos = mvt.pos;

      (*attached_members[i].second)[frame].Propagate(child_mvt.pos, mvt.GetAngle(), angle_rad);

      // Apply recursively to children:
      attached_members[i].first->ApplyMovement(child_mvt);
    }
    SetAngle(angle_rad + mvt.GetAngle());
  } else {
    // No check to perform !
    for (uint i=0; i<attached_members.size(); i++) {

      // Apply recursively to children:
      attached_members[i].first->ApplyMovement(mvt);
    }
  }

  // Apply the movement to the current member
  pos   += mvt.pos;
  alpha *= mvt.alpha;
  scale = scale * mvt.scale;
}

void Member::ResetMovement()
{
  pos.x     = 0;
  pos.y     = 0;
  angle_rad = 0;
  alpha     = 1.0;
  scale.x   = 1.0;
  scale.y   = 1.0;
  if (spr)
    spr->SetFlipped(false);
}

void Member::BuildAttachMemberMap(const std::vector<junction*> & skel_lst)
{
  if (attached_types.empty())
    return;
  attached_members.clear();

  for (AttachTypeMap::const_iterator child = attached_types.begin();
       child != attached_types.end();
       ++child) {

    // Find this member in the skeleton:
    for (std::vector<junction *>::const_iterator junction = skel_lst.begin();
         junction != skel_lst.end();
         ++junction) {

      Member *member = (*junction)->member;
      if (member->type == child->first) {
        // Build the member map, but only have a pointer to the v_attached,
        // in order to keep synch if needed, and save memory
        attached_members.push_back(std::make_pair(member, &child->second));
        break;
      }
    }
  }
  MSG_DEBUG("body", "Mapped "SIZET_FORMAT"u/"SIZET_FORMAT"u members to member %p of type %i!\n",
            attached_members.size(), attached_types.size(), this, (int)type);
}

void WeaponMember::Draw(const Point2i & /*_pos*/,
                        int /*flip_center*/,
                        LRDirection /*direction*/)
{
  if (!ActiveCharacter().IsDead() && Game::END_TURN != Game::GetInstance()->ReadState()) {
    ActiveTeam().crosshair.Draw();
    ActiveTeam().AccessWeapon().Draw();
  }
}
