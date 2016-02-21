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
public:
  typedef enum
  {
    NO_SCALING,
    X_SCALING,
    Y_SCALING,
    STRETCH_SCALING,
    FIT_SCALING
  } ScalingType;

private:
  bool disabled;
  bool loaded;
  std::string name;
  ScalingType type;
  Point2i picture_size;
  Sprite * spr;

  void ApplyScaling(ScalingType t);

public:
  PictureWidget(const Point2i & size);
  PictureWidget(const Surface& surface,
                ScalingType type = NO_SCALING);
  PictureWidget(const Point2i & size,
                const std::string & resource_id,
                ScalingType type = NO_SCALING);
  PictureWidget(Profile * profile,
                const xmlNode * pictureNode);
  virtual ~PictureWidget();

  // Load all attributs from a "Picture" node.
  virtual bool LoadXMLConfiguration(void);

  void SetSurface(const Surface & s,
                  ScalingType type = NO_SCALING);
  void SetNoSurface();

  virtual void Draw(const Point2i & mousePosition);
  virtual void Pack() { ApplyScaling(type); }

  // Apply a transparency color mask
  void Disable() { disabled = true; };
  void Enable() { disabled = false; };
  bool IsEnabled() { return !disabled; };
  Point2i GetPicturePosition() const;
  Point2f GetScale() const;
};

#endif

