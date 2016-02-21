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
 * Camera : follow an object, center on it or follow mouse interaction.
 *****************************************************************************/

#ifndef SCROLLING_H
#define SCROLLING_H

#include "../include/base.h"
#include "../object/physical_obj.h"
#include "../tool/point.h"
#include "../tool/rectangle.h"

class Camera : public Rectanglei{
public:
  bool auto_crop;

private:
  PhysicalObj* followed_object;
  bool throw_camera;
  bool follow_closely;

public:
  Camera();

  // set camera to position
  void SetXY(Point2i pos);
  void SetXYabs(int x, int y);
  void SetXYabs(const Point2i &pos);

  // Auto crop on an object
  void FollowObject (PhysicalObj *obj,
                     bool follow, bool center_on,
                     bool force_center_on_object=false);
  void StopFollowingObj (PhysicalObj* obj);

  bool IsVisible(const PhysicalObj &obj);

  void Refresh();

  bool HasFixedX() const;
  bool HasFixedY() const;
  Point2i FreeDegrees() const;
  Point2i NonFreeDegrees() const;

  void CenterOn(const PhysicalObj &obj);
  void CenterOnFollowedObject();
  void AutoCrop();
  void SetAutoCrop(bool crop);
  bool IsAutoCrop() const;
  void SetCloseFollowing(bool close);
};

extern Camera camera;

#endif
