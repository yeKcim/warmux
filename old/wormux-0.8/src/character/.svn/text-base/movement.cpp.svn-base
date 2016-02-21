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
#include "character/movement.h"
#include "tool/xml_document.h"
#include "tool/debug.h"

Movement::Movement(xmlNode* xml)
{
  frames.clear();
  play_mode = LOOP;
  always_moving = false;

  XmlReader::ReadStringAttr(xml, "name", type);
  ASSERT(type!="");
  MSG_DEBUG("body.movement", "  Loading movement %s\n", type.c_str());

  speed = 15;
  XmlReader::ReadIntAttr(xml, "speed", speed);

  std::string pm;
  if (XmlReader::ReadStringAttr(xml, "play_mode", pm))
  if (pm == "play_once")
    play_mode = PLAY_ONCE;

   // Load the test rectangle
  test_left = test_right = test_top = test_bottom = 0;
  xmlNode* collision_rect = XmlReader::GetMarker(xml, "collision_rect");
  if (collision_rect == NULL)
  {
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
  MSG_DEBUG("body.movement", "  Found %i movement frames\n", nodes.size());

  /* We know the number of member frame that are being read so we can resize
   * thr array to be able to get all of them. */
  frames.resize(nodes.size());

  for (int frame_number=0; it != end; ++it, frame_number++)
  {
    xmlNodeArray members = XmlReader::GetNamedChildren(*it, "member");
    xmlNodeArray::const_iterator it2;
    MSG_DEBUG("body.movement", "    Found %i frame members\n", members.size());

    for (it2 = members.begin(); it2 != members.end(); ++it2)
    {
      xmlNode *child = *it2;
      std::string member_type;
      XmlReader::ReadStringAttr(child, "type", member_type);

      member_mvt mvt;
      int dx = 0, dy = 0, angle_deg = 0;
      double scale_x = 1.0, scale_y = 1.0, tmp_alpha = 1.0;

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
      if( XmlReader::ReadBoolAttr(child, "follow_cursor", mvt.follow_cursor)
          && !XmlReader::ReadIntAttr(child, "follow_cursor_limit", mvt.follow_cursor_limit))
        printf("Warning ! \"follow_cursor\" flag used while \"follow_cursor_limit\" isn't defined, this won't do anything!\n");
      if(tmp_alpha < 0.0 || tmp_alpha > 1.0) tmp_alpha = 1.0;
      mvt.SetAngle(angle_deg * M_PI / 180);
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
