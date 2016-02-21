/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
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
 * Caméra : gêre la position à l'intérieur du terrain. On peut "suivre" un
 * objet et se centrer sur un objet. Lors d'un déplacement manuel (au clavier
 * ou à la souris), le mode "suiveur" est désactivé.
 *****************************************************************************/

#ifndef SCROLLING_H
#define SCROLLING_H

#include "../include/base.h"
#include "../object/physical_obj.h"
#include "../tool/point.h"
#include "../tool/rectangle.h"

class Camera : public Rectanglei{
public:
  bool autorecadre;

private:
  PhysicalObj* obj_suivi;
  bool lance;

public:
  Camera();

  // Scrolle le fond en X ou Y
  void SetXY(Point2i pos);
  void SetXYabs(int x, int y);
  void SetXYabs(const Point2i &pos);

  // Recadrage automatique sur l'objet suivi
  void ChangeObjSuivi (PhysicalObj *obj, 
		       bool suit, bool recentre,
		       bool force_recentrage=false);
  void StopFollowingObj (PhysicalObj* obj);

  bool IsVisible(const PhysicalObj &obj);

  void Refresh();

  bool HasFixedX() const;
  bool HasFixedY() const;
  Point2i FreeDegrees() const;
  Point2i NonFreeDegrees() const;

  void CenterOn(const PhysicalObj &obj);
  void CenterOnFollowedObject();
  void AutoRecadre();
};

extern Camera camera;

#endif
