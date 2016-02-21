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
 *****************************************************************************/

#include <map>
#include <iostream>
#include <cstdio>
#include "character/movement.h"
#include "tool/xml_document.h"
#include <WORMUX_debug.h>

Movement::Movement(const xmlNode* xml) : ref_count(1), nb_loops(0), duration_per_frame(15), always_moving(false)
{
  frames.clear();

  XmlReader::ReadStringAttr(xml, "name", type);
  ASSERT(type != "");
  MSG_DEBUG("body.movement", "  Loading movement %s", type.c_str());

  XmlReader::ReadUintAttr(xml, "duration_per_frame", duration_per_frame);
  XmlReader::ReadUintAttr(xml, "nb_loops", nb_loops);

  // Load the test rectangle
  test_left = test_right = test_top = test_bottom = 0;
  const xmlNode* collision_rect = XmlReader::GetMarker(xml, "collision_rect");

  if (collision_rect == NULL) {
    fprintf(stderr, "No collision rect for %s\n", type.c_str());
    return;
  }

  XmlReader::ReadUintAttr(collision_rect, "left", test_left);
  XmlReader::ReadUintAttr(collision_rect, "right", test_right);
  XmlReader::ReadUintAttr(collision_rect, "top", test_top);
  XmlReader::ReadUintAttr(collision_rect, "bottom", test_bottom);
  MSG_DEBUG("body.movement", "Collision rect set to (%u,%u) -> (%u,%u)\n",
            test_left, test_top, test_left, test_bottom);

  xmlNodeArray nodes = XmlReader::GetNamedChildren(xml, "frame");
  xmlNodeArray::const_iterator it = nodes.begin(), end = nodes.end();
  MSG_DEBUG("body.movement", "  Found %i movement frames", nodes.size());
  MSG_DEBUG("body.movement", "  Nb loops: %i", nb_loops);

  /* We know the number of member frame that are being read so we can resize
   * thr array to be able to get all of them. */
  frames.resize(nodes.size());

  for (int frame_number=0; it != end; ++it, frame_number++) {

    xmlNodeArray members = XmlReader::GetNamedChildren(*it, "member");
    xmlNodeArray::const_iterator it2;
    MSG_DEBUG("body.movement", "    Found %i frame members", members.size());

    for (it2 = members.begin(); it2 != members.end(); ++it2) {

      const xmlNode *child = *it2;
      std::string member_type;
      XmlReader::ReadStringAttr(child, "type", member_type);

      member_mvt mvt;
      int dx = 0, dy = 0, angle_deg = 0;
      Double scale_x = 1.0, scale_y = 1.0, tmp_alpha = 1.0;

      XmlReader::ReadIntAttr(child, "dx", dx);
      XmlReader::ReadIntAttr(child, "dy", dy);
      XmlReader::ReadDoubleAttr(child, "scale_x", scale_x);
      XmlReader::ReadDoubleAttr(child, "scale_y", scale_y);
      XmlReader::ReadDoubleAttr(child, "alpha", tmp_alpha);
      XmlReader::ReadIntAttr(child, "angle", angle_deg);
      XmlReader::ReadBoolAttr(child, "follow_crosshair", mvt.follow_crosshair);
      XmlReader::ReadBoolAttr(child, "follow_half_crosshair", mvt.follow_half_crosshair);
      XmlReader::ReadBoolAttr(child, "follow_speed", mvt.follow_speed);
      XmlReader::ReadBoolAttr(child, "follow_direction", mvt.follow_direction);

      if (XmlReader::ReadBoolAttr(child, "follow_cursor", mvt.follow_cursor)
	  && !XmlReader::ReadIntAttr(child, "follow_cursor_limit", mvt.follow_cursor_limit))
	fprintf(stderr, "Warning ! \"follow_cursor\" flag used while \"follow_cursor_limit\" isn't defined, this won't do anything!\n");

      if (tmp_alpha < ZERO || tmp_alpha > ONE)
	tmp_alpha = 1.0;

      mvt.SetAngle(angle_deg * PI / 180);
      mvt.pos.x = dx;
      mvt.pos.y = dy;
      mvt.alpha = tmp_alpha;
      mvt.scale = Point2f(scale_x, scale_y);

      always_moving |= mvt.follow_cursor;
      always_moving |= mvt.follow_crosshair;
      always_moving |= mvt.follow_half_crosshair;
      always_moving |= mvt.follow_speed;
      always_moving |= mvt.follow_direction;

      frames[frame_number][member_type] = mvt;
    }
  }
}

Movement::~Movement()
{
}

void Movement::SetType(const std::string& _type)
{
  type = _type;
}

const std::string& Movement::GetType() const
{
  return type;
}

uint Movement::GetFrameDuration() const
{
  return duration_per_frame;
}

uint Movement::GetNbLoops() const
{
  return nb_loops;
}

bool Movement::IsAlwaysMoving() const
{
  return always_moving;
}

const std::vector<Movement::member_def> & Movement::GetFrames() const
{
  return frames;
}

uint Movement::GetTestLeft() const
{
  return test_left;
}

uint Movement::GetTestRight() const
{
  return test_right;
}

uint Movement::GetTestTop() const
{
  return test_top;
}

uint Movement::GetTestBottom() const
{
  return test_bottom;
}

// ===============================================================

void Movement::ShareMovement(Movement* mvt)
{
  ASSERT(mvt);
  mvt->ref_count++;
}

void Movement::UnshareMovement(Movement* mvt)
{
  mvt->ref_count--;

  if (mvt->ref_count == 0)
    delete mvt;
}
