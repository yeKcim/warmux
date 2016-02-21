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
 * Sky: background of the map
 *****************************************************************************/

#include "sky.h"
#include "camera.h"
#include "map.h"
#include "maps_list.h"
#include "graphic/surface.h"
#include "graphic/video.h"
#include "include/app.h"

// Vitesse (comprise entre 0 et 0.5)
const Point2d SKY_SPEED( 0.3, 1);

Sky::Sky(){
}

void Sky::Init(){
         // That is temporary -> image will be loaded directly without alpha chanel
        Surface tmp_image = ActiveMap().ReadImgSky();
        tmp_image.SetAlpha( 0, 0);
        image = tmp_image.DisplayFormat();

        tstVect = image.GetSize().inf( Camera::GetInstance()->GetSize() );
        margin = tstVect * (Camera::GetInstance()->GetSize() - image.GetSize())/2;
}

void Sky::Reset(){
        Init();
        lastPos.SetValues(INT_MAX, INT_MAX);
}

void Sky::Free(){
        image.Free();
}

void Sky::Draw(bool redraw_all)
{
  if( lastPos != Camera::GetInstance()->GetPosition() || redraw_all){
    lastPos = Camera::GetInstance()->GetPosition();
    RedrawParticle(*Camera::GetInstance());
    return;
  }

  RedrawParticleList(*world.to_redraw_now);
  RedrawParticleList(*world.to_redraw_particles_now);
}

void Sky::RedrawParticleList(std::list<Rectanglei> &list) const{
  std::list<Rectanglei>::iterator it;

  for( it = list.begin(); it != list.end(); ++it )
          RedrawParticle(*it);
}

void Sky::RedrawParticle(const Rectanglei &particle) const{
    Rectanglei ds(GetSkyPos() + particle.GetPosition() - Camera::GetInstance()->GetPosition() - margin,
                  particle.GetSize() );
    AppWormux::GetInstance()->video->window.Blit(image, ds, particle.GetPosition() - Camera::GetInstance()->GetPosition());
}

Point2i Sky::GetSkyPos() const{
        return (Point2i(1, 1) - tstVect) * Camera::GetInstance()->GetPosition() * SKY_SPEED;
}
