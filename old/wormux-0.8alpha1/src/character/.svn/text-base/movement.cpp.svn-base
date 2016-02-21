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
#include "movement.h"
#include "../tool/xml_document.h"
#include "../tool/string_tools.h"

Movement::Movement(xmlpp::Element *xml)
{
  always_moving = false;
  LitDocXml::LitAttrString( xml, "name", type);
  assert(type!="");

  speed = 15;
  LitDocXml::LitAttrInt(xml, "speed", speed);

  // Load the test rectangle
  test_left = test_right = test_top = test_bottom = 0;
  xmlpp::Element *collision_rect = LitDocXml::AccesBalise (xml, "collision_rect");
  if (collision_rect == NULL) return;
  LitDocXml::LitAttrUint (collision_rect, "left", test_left);
  LitDocXml::LitAttrUint (collision_rect, "right", test_right);
  LitDocXml::LitAttrUint (collision_rect, "top", test_top);
  LitDocXml::LitAttrUint (collision_rect, "bottom", test_bottom);

  xmlpp::Node::NodeList nodes = xml -> get_children("frame");
  xmlpp::Node::NodeList::iterator
    it=nodes.begin(),
    end=nodes.end();

  for (; it != end; ++it)
  {
    xmlpp::Element *elem = dynamic_cast<xmlpp::Element*> (*it);
    assert (elem != NULL);
    int frame_number;
    LitDocXml::LitAttrInt(elem, "number", frame_number);

    xmlpp::Node::NodeList nodes2 = elem -> get_children("member");
    xmlpp::Node::NodeList::iterator
      it2=nodes2.begin(),
      end2=nodes2.end();

    for (; it2 != end2; ++it2)
    {
      xmlpp::Element *elem2 = dynamic_cast<xmlpp::Element*> (*it2);
      std::string member_type;
      LitDocXml::LitAttrString(elem2, "type", member_type);

      member_mvt mvt;
      int dx, dy;
      dx = dy = 0;
      double scale_x, scale_y, tmp_alpha;
      scale_x = scale_y = tmp_alpha = 1.0;
      LitDocXml::LitAttrInt(elem2, "dx", dx);
      LitDocXml::LitAttrInt(elem2, "dy", dy);
      LitDocXml::LitAttrDouble(elem2, "scale_x", scale_x);
      LitDocXml::LitAttrDouble(elem2, "scale_y", scale_y);
      LitDocXml::LitAttrDouble(elem2, "alpha", tmp_alpha);
      LitDocXml::LitAttrInt(elem2, "angle", mvt.angle);
      LitDocXml::LitAttrBool(elem2, "follow_crosshair", mvt.follow_crosshair);
      LitDocXml::LitAttrBool(elem2, "follow_half_crosshair", mvt.follow_half_crosshair);
      LitDocXml::LitAttrBool(elem2, "follow_speed", mvt.follow_speed);
      LitDocXml::LitAttrBool(elem2, "follow_direction", mvt.follow_direction);
      if(tmp_alpha < 0.0 || tmp_alpha > 1.0) tmp_alpha = 1.0;
      mvt.pos.x = dx;
      mvt.pos.y = dy;
      mvt.alpha = tmp_alpha;
      mvt.scale = Point2f(scale_x, scale_y);

      always_moving |= mvt.follow_crosshair;
      always_moving |= mvt.follow_half_crosshair;
      always_moving |= mvt.follow_speed;
      always_moving |= mvt.follow_direction;

      if((int)frames.size() <= frame_number)
        frames.resize(frame_number+1);

      frames[frame_number][member_type] = mvt;
    }
  }
}

Movement::~Movement()
{
}
