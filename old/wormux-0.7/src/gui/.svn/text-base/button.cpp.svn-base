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
 * Simple button
 *****************************************************************************/

#include "button.h"
#include "../tool/resource_manager.h"
#include "../graphic/sprite.h"
#include "../include/app.h"

Button::Button (const Rectanglei &rect, const Profile *res_profile, const std::string& resource_id) : Widget(rect){
  image = resource_manager.LoadSprite(res_profile,resource_id);
  image->cache.EnableLastFrameCache();
  image->ScaleSize(rect.GetSize());
}

Button::Button (const Point2i &m_position, const Profile *res_profile, const std::string& resource_id){
  image = resource_manager.LoadSprite(res_profile, resource_id);
  position = m_position;
  size = image->GetSize();
}

Button::~Button(){
	delete image;
}

void Button::Draw(const Point2i &mousePosition){
  uint frame = Contains(mousePosition)?1:0;

  image->SetCurrentFrame(frame);
  image->Blit(AppWormux::GetInstance()->video.window, position);
}

void Button::SetSizePosition(const Rectanglei &rect){
  StdSetSizePosition(rect);
  image->ScaleSize(size);
}
