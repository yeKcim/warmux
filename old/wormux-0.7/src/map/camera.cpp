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
#include "map.h"
#include "maps_list.h"
#include "../include/app.h"
#include "../interface/mouse.h"
#include "../team/teams_list.h"
#include "../tool/debug.h"
#include "../tool/rectangle.h"
#include "../tool/math_tools.h"

// Marge de cadrage 
const int MARGE = 200;
const double VITESSE_CAMERA = 20;
const Point2i marge(200, 200);
const Point2i vitesseCamera(20, 20);

Camera camera;

Camera::Camera(){
  lance = false;
  autorecadre = true;
  obj_suivi = NULL;
}

bool Camera::HasFixedX() const{
  return (int)world.GetWidth() <= GetSizeX();
}

bool Camera::HasFixedY() const{
  return (int)world.GetHeight() <= GetSizeY();
}

Point2i Camera::FreeDegrees() const{
	return Point2i( HasFixedX()? 0:1,
			HasFixedY()? 0:1 );
}

Point2i Camera::NonFreeDegrees() const{
	return Point2i(1, 1) - FreeDegrees();
}

void Camera::SetXYabs(int x, int y){ 
  AppWormux * app = AppWormux::GetInstance();

  if( !TerrainActif().infinite_bg ){
    if( !HasFixedX() )
      position.x = BorneLong(x, 0, world.GetWidth() - GetSizeX());
    else
      position.x = - (app->video.window.GetWidth() - world.GetWidth())/2;
    
	if( !HasFixedY() )
      position.y = BorneLong(y, 0, world.GetHeight()-GetSizeY());
    else
      position.y = - (app->video.window.GetHeight() - world.GetHeight())/2;
  }else{
    position.x = x;

    if( y > (int)world.GetHeight() - GetSizeY() )
      position.y = world.GetHeight() - GetSizeY();
    else
      position.y = y;
  }
  lance = true;
}

void Camera::SetXYabs(const Point2i &pos){
	SetXYabs(pos.x, pos.y);
}

void Camera::SetXY(Point2i pos){
	pos = pos * FreeDegrees();
	if( pos.IsNull() )
		return;
	
	SetXYabs(position + pos);
}

void Camera::CenterOnFollowedObject(){
  CenterOn(*obj_suivi);
}

// Centrage immédiat sur un objet
void Camera::CenterOn(const PhysicalObj &obj){
  if (obj.IsGhost())
    return;

  MSG_DEBUG( "camera.scroll", "centering on %s", obj.m_name.c_str() );

  Point2i pos(0, 0);

  pos += FreeDegrees() * obj.GetPosition() - ( GetSize() - obj.GetSize() )/2;
  pos += NonFreeDegrees() * ( world.GetSize() - GetSize() )/2;

  SetXYabs( pos );
}

void Camera::AutoRecadre(){
  if( !obj_suivi || obj_suivi -> IsGhost() )
    return;
  
  if( !IsVisible(*obj_suivi) ){
	MSG_DEBUG("camera.scroll", "The object is not visible.");
	CenterOnFollowedObject();
    return;
  }

  Point2i pos = obj_suivi->GetPosition();
  Point2i size = obj_suivi->GetSize();
  
  if( pos.y < 0 && !TerrainActif().infinite_bg )
    pos.y = 0;

  Point2i dstMax = GetSize()/2 - marge;
  Point2i cameraBR = GetSize() + position;
  Point2i objectBRmargin = pos + size + marge;
  Point2i dst(0, 0);
 
  dst += cameraBR.inf(objectBRmargin) * (objectBRmargin - cameraBR);
  dst += (pos - marge).inf(position) * (pos - marge - position);
  
  SetXY( dst * vitesseCamera / dstMax );
}

void Camera::Refresh(){
  lance = false;

  // Camera à la souris
  Mouse::GetInstance()->TestCamera();
  if (lance) return;

#ifdef TODO_KEYBOARD // ??? 
  // Camera au clavier
  clavier.TestCamera();
  if (lance)
    return;
#endif
   
  if (autorecadre)
    AutoRecadre();
}

void Camera::ChangeObjSuivi (PhysicalObj *obj, bool suit, bool recentre,
			     bool force_recentrage){
  MSG_DEBUG( "camera.tracking", "Following object %s, recentre=%d, suit=%d", obj->m_name.c_str(), recentre, suit);
  if (recentre) 
    if ((obj_suivi != obj) || !IsVisible(*obj) || force_recentrage) 
    {
      CenterOn(*obj);
      autorecadre = suit;
    }
  obj_suivi = obj;
}

void Camera::StopFollowingObj (PhysicalObj* obj){
  if( obj_suivi == obj )
    ChangeObjSuivi((PhysicalObj*)&ActiveCharacter(), true, true, true);
}

bool Camera::IsVisible(const PhysicalObj &obj){
   return Intersect( obj.GetRect() );
}

