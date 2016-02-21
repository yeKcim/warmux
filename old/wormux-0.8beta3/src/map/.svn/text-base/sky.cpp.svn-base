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

#include "map/sky.h"
#include "map/camera.h"
#include "map/map.h"
#include "map/maps_list.h"
#include "graphic/surface.h"
#include "graphic/video.h"
#include "include/app.h"

void Sky::Init()
{
  // That is temporary -> image will be loaded directly without alpha chanel
  Surface tmp_image = ActiveMap().ReadImgSky();
  tmp_image.SetAlpha( 0, 0);
  image = tmp_image.DisplayFormat();
}

void Sky::Reset()
{
  Init();
  last_pos.SetValues(INT_MAX, INT_MAX);
}

void Sky::Free()
{
  image.Free();
}

void Sky::Draw(bool redraw_all)
{
  if(last_pos != Camera::GetInstance()->GetPosition() || redraw_all) {
    last_pos = Camera::GetInstance()->GetPosition();
    RedrawParticle(Rectanglei(Camera::GetInstance()->GetPosition(), AppWormux::GetInstance()->video->window.GetSize()));
    return;
  }

  RedrawParticleList(*world.to_redraw_now);
  RedrawParticleList(*world.to_redraw_particles_now);
}

void Sky::RedrawParticleList(std::list<Rectanglei> &list) const
{
  std::list<Rectanglei>::iterator it;

  for( it = list.begin(); it != list.end(); ++it )
          RedrawParticle(*it);
}

void Sky::RedrawParticle(const Rectanglei &particle) const
{
  Rectanglei ds(GetSkyPos() + particle.GetPosition() - Camera::GetInstance()->GetPosition(),
                particle.GetSize());
  AppWormux::GetInstance()->video->window.Blit(image, ds, particle.GetPosition() - Camera::GetInstance()->GetPosition());
}

Point2i Sky::GetSkyPos() const
{
  Point2i min(0, 0);
  Point2i max = image.GetSize() - AppWormux::GetInstance()->video->window.GetSize();
  Point2i tmp = Camera::GetInstance()->GetPosition();
  int w_w = world.GetWidth();
  int w_h = world.GetHeight();
  int v_w = AppWormux::GetInstance()->video->window.GetWidth();
  int v_h = AppWormux::GetInstance()->video->window.GetHeight();
  double x_sky = (double)(tmp.x) / (double)(w_w - v_w);
  double y_sky = (double)(tmp.y) / (double)(w_h - v_h);
  return max * Point2d(x_sky, y_sky);
}
