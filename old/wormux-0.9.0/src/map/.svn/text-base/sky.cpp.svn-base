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
 * Sky: background of the map
 *****************************************************************************/

#include <climits>
#include "include/app.h"
#include "graphic/surface.h"
#include "graphic/video.h"
#include "map/sky.h"
#include "map/camera.h"
#include "map/map.h"
#include "map/maps_list.h"

void Sky::Init()
{
  std::vector<Surface> sky_layer = ActiveMap()->ReadSkyLayer();

  if (0 < sky_layer.size()) {
    for (uint i = 0; i < sky_layer.size(); i++) {
      images.push_back(sky_layer[i]);
    }
  } else {
    Surface tmp_image = ActiveMap()->ReadImgSky();
    tmp_image.SetAlpha(0, 0);
    images.push_back(tmp_image.DisplayFormat());
  }
}

void Sky::Reset()
{
  Free();
  Init();
  last_pos.SetValues(INT_MAX, INT_MAX);
}

void Sky::Free()
{
  std::vector<Surface>::iterator it = images.begin();

  for ( ; it != images.end(); ++it) {
    (*it).Free();
  }
  images.clear();
}

void Sky::Draw(bool redraw_all)
{
  if (last_pos != Camera::GetInstance()->GetPosition() || redraw_all) {
    last_pos = Camera::GetInstance()->GetPosition();
    RedrawParticle(Rectanglei(Camera::GetInstance()->GetPosition(), GetMainWindow().GetSize()));
    return;
  }

  RedrawParticleList(*GetWorld().to_redraw_now);
  RedrawParticleList(*GetWorld().to_redraw_particles_now);
}

void Sky::RedrawParticleList(std::list<Rectanglei> & list) const
{
  std::list<Rectanglei>::iterator it = list.begin();

  for ( ; it != list.end(); ++it) {
    RedrawParticle(*it);
  }
}

void Sky::RedrawParticle(const Rectanglei & particle) const
{
  Point2i tmpPos =  particle.GetPosition() - Camera::GetInstance()->GetPosition();
  Rectanglei ds;
  ds.SetSize(particle.GetSize());

  for (uint layer = 0; layer < images.size(); ++layer) {
    ds.SetPosition(GetSkyPos(layer) + tmpPos);
    GetMainWindow().Blit(images[layer], ds, tmpPos);
  }
}

Point2i Sky::GetSkyPos(uint layer) const
{
  ASSERT(layer < images.size());

  Point2i tmp = Camera::GetInstance()->GetPosition();
  double x_sky = (double)(tmp.x) / (double)(GetWorld().GetWidth() - GetMainWindow().GetWidth());
  double y_sky = (double)(tmp.y) / (double)(GetWorld().GetHeight() - GetMainWindow().GetHeight());

  return (images[layer].GetSize() - GetMainWindow().GetSize()) * Point2d(x_sky, y_sky);
}
