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
 * Picture widget: A widget containing a picture
 *****************************************************************************/

#include "gui/picture_widget.h"
#include "graphic/colors.h"
#include "graphic/sprite.h"
#include "tool/resource_manager.h"

PictureWidget::PictureWidget (const Point2i& _size)
{
  size = _size;
  spr = NULL;
  disabled = false;
}

PictureWidget::PictureWidget (const Point2i& _size, const std::string& resource_id)
{
  size = _size;
  spr = NULL;
  disabled = false;

  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);
  Surface tmp = resource_manager.LoadImage(res, resource_id);
  SetSurface(tmp, false);
  resource_manager.UnLoadXMLProfile( res);
}

PictureWidget::~PictureWidget()
{
  if (spr != NULL)
    delete spr;
}

void PictureWidget::SetSurface(const Surface& s, bool enable_scaling, bool antialiasing)
{
  NeedRedrawing();

  if (spr != NULL)
    delete spr;

  spr = new Sprite(s, antialiasing);
  if (enable_scaling) {
    float scale = std::min( float(GetSizeY())/spr->GetHeight(),
                            float(GetSizeX())/spr->GetWidth() ) ;

    spr->Scale (scale, scale);
  }
}

void PictureWidget::SetNoSurface()
{
  NeedRedrawing();

  if (spr != NULL)
    delete spr;

  spr = NULL;
}

void PictureWidget::Draw(const Point2i &/*mousePosition*/,
                         Surface& surf) const
{
  if (spr != NULL) {
    int x = GetPositionX() + ( GetSizeX()/2 ) - (spr->GetWidth()/2);
    int y = GetPositionY() + ( GetSizeY()/2 ) - (spr->GetHeight()/2);

    spr->Blit ( surf, x, y);

    // Draw a transparency mask
    if (disabled) {
      surf.BoxColor(Rectanglei(x,y,spr->GetWidth(),spr->GetHeight()),
                    defaultOptionColorBox);
    }
  }
}
