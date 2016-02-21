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
#include <cstdio>
#include "character/movement.h"
#include "tool/xml_document.h"
#include <WARMUX_debug.h>

Movement::Movement(const xmlNode* xml) : ref_count(1), nb_loops(0), duration_per_frame(15), always_moving(false)
{
  XmlReader::ReadStringAttr(xml, "name", type);
  ASSERT(type != "");
  MSG_DEBUG("body.movement", "  Loading movement %s", type.c_str());

  XmlReader::ReadUintAttr(xml, "duration_per_frame", duration_per_frame);
  XmlReader::ReadUintAttr(xml, "nb_loops", nb_loops);

  // Load the test rectangle
  test_left = test_right = test_top = test_bottom = 0;
  const xmlNode* collision_rect = XmlReader::GetMarker(xml, "collision_rect");

  if (!collision_rect) {
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
  MSG_DEBUG("body.movement", "  Found "SIZET_FORMAT"u movement frames", nodes.size());
  MSG_DEBUG("body.movement", "  Nb loops: %i", nb_loops);

  // We know the number of member frames that will be read,
  // so we can set set the array size now
  frames.reserve(nodes.size());

  for (; it != end; ++it) {
    xmlNodeArray members = XmlReader::GetNamedChildren(*it, "member");
    MSG_DEBUG("body.movement", "    Found "SIZET_FORMAT"u frame members", members.size());

    member_def def;
    def.reserve(members.size());

    xmlNodeArray::const_iterator it2;
    for (it2 = members.begin(); it2 != members.end(); ++it2) {
      const xmlNode *child = *it2;
      std::string member_type;
      XmlReader::ReadStringAttr(child, "type", member_type);

      member_mvt mvt(member_type);
      int dx = 0, dy = 0, angle_deg = 0;
      Double scale_x = ONE, scale_y = ONE, tmp_alpha = ONE;

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
      XmlReader::ReadIntAttr(child, "follow_cursor_square_limit", mvt.follow_cursor_square_limit);

      if (tmp_alpha < ZERO || tmp_alpha > ONE)
        tmp_alpha = 1.0;

      mvt.SetAngle(angle_deg * PI / 180);
      mvt.pos.x = dx;
      mvt.pos.y = dy;
      mvt.alpha = tmp_alpha;
      mvt.scale.x = scale_x;
      mvt.scale.y = scale_y;

      always_moving |= mvt.follow_cursor_square_limit!=0;
      always_moving |= mvt.follow_crosshair;
      always_moving |= mvt.follow_half_crosshair;
      always_moving |= mvt.follow_speed;
      always_moving |= mvt.follow_direction;

      def.push_back(mvt);
    }
    frames.push_back(def);
  }
}
