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
 * Picture widget: A widget containing a picture
 *****************************************************************************/

#ifndef PICTURE_WIDGET_H
#define PICTURE_WIDGET_H

#include <string>

#include "widget.h"
#include "tool/resource_manager.h"

class Sprite;
class Surface;

class PictureWidget : public Widget
{
 private:
  /* If you need this, implement it (correctly)*/
  PictureWidget(const PictureWidget&);
  PictureWidget operator=(const PictureWidget&);
  /*********************************************/

  bool disabled;
  Sprite * spr;

 public:
  PictureWidget(const Point2i & size);
  PictureWidget(const Point2i & size, 
                const std::string & resource_id, 
                bool scale = false);
  PictureWidget(Profile * profile,
                const xmlNode * pictureNode);
  virtual ~PictureWidget();

  // Load all attributs from a "Picture" node.
  virtual bool LoadXMLConfiguration(void);

  void SetSurface(const Surface & s, 
                  bool enable_scaling = false, 
                  bool antialiasing = false);
  void SetNoSurface();
  virtual void Draw(const Point2i & mousePosition) const;
  virtual void Pack() {};

  // Apply a transparency color mask
  void Disable() { disabled = true; };
  void Enable() { disabled = false; };
};

#endif

