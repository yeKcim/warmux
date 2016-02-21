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
 * Cache rendering of a partial torus
 *****************************************************************************/

#include "graphic/polygon.h"
#include "graphic/polygon_generator.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "gui/torus_cache.h"
#include "tool/affine_transform.h"
#include "tool/resource_manager.h"

static int      ref_count = 0;
static Surface *annulus_background = 0;
static Surface *annulus_foreground = 0;
static Sprite  *img_plus = 0;
static Sprite  *img_minus = 0;
static Color    progress_color;

TorusCache::TorusCache(Profile *res, const std::string& resource_id, int bigr, int smallr)
  : m_last_angle(-1.0)
  , m_torus(NULL)
  , m_big_r(bigr)
  , m_small_r(smallr)
{
  // Load the shared resources
  if (!annulus_background)
    annulus_background = new Surface(LOAD_RES_IMAGE("menu/annulus_background"));
  if (!annulus_foreground)
    annulus_foreground = new Surface(LOAD_RES_IMAGE("menu/annulus_foreground"));
  if (!img_plus)
    img_plus = LOAD_RES_SPRITE("menu/big_plus");
  if (!img_minus)
    img_minus = LOAD_RES_SPRITE("menu/big_minus");
  if (!ref_count)
    progress_color = LOAD_RES_COLOR("menu/annulus_progress_color");
  ref_count++;

  // Now load own values
  m_image = LOAD_RES_IMAGE(resource_id);
  m_plus = new Sprite(*img_plus);
  m_minus = new Sprite(*img_minus);
}

TorusCache::~TorusCache()
{
  delete m_plus;
  delete m_minus;
  if (m_torus)
    delete m_torus;

  // Check whether to free static resources
  ref_count--;
  if (!ref_count) {
    delete annulus_background; annulus_background = NULL;
    delete annulus_foreground; annulus_foreground = NULL;
    delete img_plus; img_plus = NULL;
    delete img_minus; img_minus = NULL;
  }
}

Point2i TorusCache::GetSize() const
{
  return annulus_background->GetSize();
}

void TorusCache::Draw(const Rectanglei& box)
{
  Surface& surf = GetMainWindow();
  //  the computed centers are to center on the image part of the widget
  Point2i center(box.GetPositionX() + box.GetSizeX()/2,
                 box.GetPositionY() + annulus_background->GetHeight()/2);

  // 1. first draw the annulus background
  surf.Blit(*annulus_background, center - annulus_background->GetSize()/2);

  // 2. then draw the progress annulus
  surf.Blit(*m_torus, center - m_torus->GetSize()/2);

  // 3. then draw the annulus foreground
  surf.Blit(*annulus_foreground, center - annulus_foreground->GetSize()/2);

  // 4. then draw the image
  surf.Blit(m_image, center - m_image.GetSize()/2);
}

void TorusCache::Refresh(float angle, float open)
{
  if (m_last_angle != angle) {
    delete m_torus;
    m_torus = NULL;
  }
  if (!m_torus) {
    m_torus = new Surface(Point2i(m_big_r*2+2, m_big_r*2+2), 0, true);
    m_torus->SetAlpha(0, 0); // Overwrite all

    Polygon *poly = PolygonGenerator::GeneratePartialTorus(m_big_r * 2, m_small_r * 2, 30,
                                                           angle, open / 2);
    poly->SetPlaneColor(progress_color);
    poly->ApplyTransformation(AffineTransform2D::Translate(m_big_r+1, m_big_r+1));
    poly->Draw(m_torus);
    delete poly;

    m_torus->SetAlpha(SDL_SRCALPHA, 0);
  }
  m_last_angle = angle;
}
