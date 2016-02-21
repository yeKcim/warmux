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
//-----------------------------------------------------------------------------
#include "../include/base.h"
#include "../object/physical_obj.h"
#include "../team/character.h"
//-----------------------------------------------------------------------------

class Camera
{
public:
  uint pause;
  bool autorecadre;

private:
  struct s_pos
  {
    int x;
    int y;
  } pos;
  bool selec_rectangle;
  PhysicalObj* obj_suivi;
  bool lance;

public:
  Camera();

  // Scrolle le fond en X ou Y
  void SetXY (int dx, int dy);
  void SetXYabs (int x, int y);

  // Recadrage automatique sur l'objet suivi
  void ChangeObjSuivi (PhysicalObj *obj, 
		       bool suit, bool recentre,
		       bool force_recentrage=false);
  void StopFollowingObj (PhysicalObj* obj);

  // Est-ce que l'objet obj est visible dans le monde ?
  bool EstVisible (const PhysicalObj &obj);

  // Decalage du fond
  int GetX() const;
  int GetY() const;
  uint GetWidth() const;
  uint GetHeight() const;

  void Refresh();

  bool HasFixedX() const;
  bool HasFixedY() const;

  void Centre (const PhysicalObj &obj);
  void CentreObjSuivi ();
  void AutoRecadre ();
};

extern Camera camera;
//-----------------------------------------------------------------------------
#endif
