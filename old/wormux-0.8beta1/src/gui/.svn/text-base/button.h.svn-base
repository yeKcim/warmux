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
 * Simple button
 *****************************************************************************/

#ifndef GUI_BUTTON_H
#define GUI_BUTTON_H

#include "include/base.h"
#include "graphic/sprite.h"
#include "tool/resource_manager.h"
#include "widget.h"

class Button : public Widget
{
  /* If you need this, implement it (correctly)*/
  Button(const Button&);
  Button operator=(const Button&);
  /*********************************************/
  bool hidden;

protected:
  bool img_scale;
  Sprite *image;

public:
  Button (const Rectanglei &rect,
	  const Profile *res_profile, const std::string& resource_id,
	  bool img_scale = true);
  Button (const Point2i &position,
	  const Profile *res_profile, const std::string& resource_id,
	  bool img_scale = false);
  virtual ~Button();

  virtual void Draw(const Point2i &mousePosition, Surface& surf) const;
  virtual void SetSizePosition(const Rectanglei &rect);

  Sprite* GetSprite() { return image; };

  void SetVisible(bool visible);
};

#endif
