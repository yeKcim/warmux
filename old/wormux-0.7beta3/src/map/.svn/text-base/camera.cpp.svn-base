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

#include "camera.h"
//-----------------------------------------------------------------------------
#include "map.h"
#include "maps_list.h"
#include "../team/teams_list.h"
#include "../graphic/video.h"
#include "../interface/mouse.h"
#include "../tool/Rectangle.h"
#include <iostream>

using namespace Wormux;
//-----------------------------------------------------------------------------

// Marge de cadrage 
const int MARGE = 200;

#if 0
// Pause entre deux scroll
const int PAUSE_SCROLLING = 1; // ms
#endif

#ifdef DEBUG

//#define DEBUG_MSG_SCROLL
//#define DEBUG_OBJ_SUIVI

#endif
#define COUT_DBG std::cout << "[Camera] "

const double VITESSE_CAMERA = 20;
//-----------------------------------------------------------------------------
Camera camera;
//-----------------------------------------------------------------------------

Camera::Camera()
{
  pos.x=0;
  pos.y=0;
  selec_rectangle=false;
  pause = SDL_GetTicks();
  lance = false;
  autorecadre = true;
  obj_suivi = NULL;
}

//-----------------------------------------------------------------------------

// Decalage du fond
int Camera::GetX() const { return pos.x; }
int Camera::GetY() const { return pos.y; }

//-----------------------------------------------------------------------------

bool Camera::HasFixedX() const { return (world.GetWidth() <= GetWidth()); }
bool Camera::HasFixedY() const { return (world.GetHeight() <= GetHeight()); }

//-----------------------------------------------------------------------------

void Camera::SetXYabs (int x, int y)
{ 
#ifdef DEBUG_MSG_SCROLL
  int sauve_fond_x = pos.x, sauve_fond_y = pos.y;
#endif
  if(!TerrainActif().infinite_bg)
  {
    if (!HasFixedX()) {
      pos.x = BorneLong(x, 0, world.GetWidth()-GetWidth());
    } else {
      //pos.x = BorneLong(x, 0, GetWidth()-world.GetWidth());
        pos.x = BorneLong(x, world.GetWidth()-GetWidth(), 0);
    }
  }
  else
  {
    pos.x = x;
  }

  if(!TerrainActif().infinite_bg)
  {
    if (!HasFixedY()) {
      pos.y = BorneLong(y, 0, world.GetHeight()-GetHeight());
    } else {
      //pos.y = BorneLong(y, 0, GetHeight()-world.GetHeight());
          pos.y = BorneLong(y, world.GetHeight()-GetHeight(), 0);
    }
  }
  else
  {
    if( y > (int)world.GetHeight()-(int)GetHeight() )
      pos.y = (int)world.GetHeight()-(int)GetHeight();
    else
      pos.y = y;
  }    
  lance = true;
#ifdef DEBUG_MSG_SCROLL
  if ((pos.x != sauve_fond_x) || (pos.y != sauve_fond_y )) 
  {
    COUT_DBG << "SetXY : " << x << "," << y 
	     << ", fond:" << pos.x << "," << pos.y << std::endl;

  }
#endif
}

//-----------------------------------------------------------------------------

void Camera::SetXY(int dx, int dy)
{ 
  if (camera.HasFixedX()) dx = 0;
  if (camera.HasFixedY()) dy = 0;
  if ((dx == 0) && (dy == 0)) return;
  SetXYabs (pos.x+dx,pos.y+dy);
}

//-----------------------------------------------------------------------------

void Camera::CentreObjSuivi ()
{ Centre (*obj_suivi); }

//-----------------------------------------------------------------------------

// Centrage immédiat sur un objet
void Camera::Centre (const PhysicalObj &obj)
{
  if (obj.IsGhost()) return;

#ifdef DEBUG_MSG_SCROLL
  COUT_DBG << "Se centre sur " << obj.m_name << std::endl;
#endif

  int x,y;

  if (!HasFixedX()) {
    x  = (int)obj.GetX();
    x -= ((int)GetWidth() - (int)obj.GetWidth())/(int)2;
  } else {
    x = ((int)world.GetWidth() - (int)GetWidth()) / 2;
  }
  if (!HasFixedY()) {
    y  = (int)obj.GetY();
    y -= ((int)GetHeight() - (int)obj.GetHeight())/(int)2;
  } else {
    y = ((int)world.GetHeight() - (int)GetHeight()) / 2;
  }
  SetXYabs (x,y);
}

//-----------------------------------------------------------------------------

void Camera::AutoRecadre ()
{
  int dx, dy;
  int x,y;
  int larg,haut;

  if (!obj_suivi || obj_suivi -> IsGhost()) return;

  x = (int)obj_suivi -> GetX();
  y = (int)obj_suivi -> GetY(); 
  if (y < 0 && !TerrainActif().infinite_bg) y = 0;
  larg = obj_suivi -> GetWidth();
  haut = obj_suivi -> GetHeight();

  if (!EstVisible(*obj_suivi)) 
  {
#ifdef DEBUG_MSG_SCROLL
    COUT_DBG << "L'objet n'est pas visible." << std::endl;
#endif
    Centre (*obj_suivi);
    return;
  }

  double dst_max = (GetWidth() - 2*MARGE)/2;
  double dst;
  if ((int)GetWidth() + pos.x < x+larg+MARGE)
    dst = x+larg+MARGE - ((int)GetWidth() + pos.x);  // positif
  else if (x-MARGE < pos.x)
    dst = (int)(x-MARGE) - pos.x; // négatif
  else
    dst = 0;
  dx = (int)(dst*VITESSE_CAMERA / dst_max);

  dst_max = ((int)GetHeight() - 2*MARGE)/2;
  if ((int)GetHeight() + pos.y < y+haut+MARGE)
    dst = y+haut+MARGE - (GetHeight() + pos.y);
  else if (y-MARGE < pos.y)
    dst = (int)(y-MARGE) - pos.y;
  else
    dst = 0;
  dy = (int)(dst*VITESSE_CAMERA / dst_max);

  SetXY (dx, dy);
}

//-----------------------------------------------------------------------------

void Camera::Refresh()
{
  lance = false;

  // Camera à la souris
  mouse.TestCamera();
  if (lance) return;

#ifdef TODO_KEYBOARD // ??? 
  // Camera au clavier
  clavier.TestCamera();
  if (lance) return;
#endif
   
  if (autorecadre) AutoRecadre();
}

//-----------------------------------------------------------------------------

void Camera::ChangeObjSuivi (PhysicalObj *obj, bool suit, bool recentre,
			     bool force_recentrage)
{
#ifdef DEBUG_OBJ_SUIVI
  COUT_DBG << "Suit l'objet " << obj -> m_name
	   << ", recentre=" << recentre
	   << ", suit=" << suit << std::endl;
#endif
  if (recentre) 
  {
    if ((obj_suivi != obj) || !EstVisible(*obj) || force_recentrage) 
    {
      Centre (*obj);
      autorecadre = suit;
#ifdef DEBUG_OBJ_SUIVI
    } else {
      COUT_DBG << "Finalement ne le suit pas ..." << std::endl;
#endif
    }
  }
  obj_suivi = obj;
}

//-----------------------------------------------------------------------------

void Camera::StopFollowingObj (PhysicalObj* obj)
{
  if( obj_suivi == obj )
  {
    ChangeObjSuivi((PhysicalObj*)&ActiveCharacter(),true,true,true);
  }
}

//-----------------------------------------------------------------------------

uint Camera::GetWidth() const { return video.GetWidth(); }
uint Camera::GetHeight() const { return video.GetHeight(); }

//-----------------------------------------------------------------------------

bool Camera::EstVisible (const PhysicalObj &obj)
{
#ifdef TODO_KEYBOARD // ??? 
  CL_Rect rect;
  rect.left = GetX();
  rect.top = GetY();
  rect.right = rect.left + GetWidth();
  rect.bottom = rect.top + GetHeight();
  return RectTouche (rect, obj.GetRect());
#else
   Rectanglei rect;
   rect.x = GetX();
   rect.y = GetY();
   rect.w = GetWidth();
   rect.h = GetHeight();
   return rect.Intersect (obj.GetRect());
#endif
}

//-----------------------------------------------------------------------------
