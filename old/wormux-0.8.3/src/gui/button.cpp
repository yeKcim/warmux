/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
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

#include "gui/button.h"
#include "include/app.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "tool/resource_manager.h"


Button::Button (const Profile *res_profile,
                const std::string& resource_id, bool _img_scale):
  img_scale(_img_scale),
  image(GetResourceManager().LoadSprite(res_profile,resource_id))
{
  position = Point2i(-1, -1);
  size = image->GetSize();
}

Button::~Button()
{
  delete image;
}

void Button::Draw(const Point2i &mousePosition) const
{
  Surface& surf = GetMainWindow();

  uint frame = (IsHighlighted() || Contains(mousePosition));

  // Check that there are enough frames in the image...
  if (image->GetFrameCount() <= frame) {
    frame = 0;
  }

  image->SetCurrentFrame(frame);

  if (img_scale) {
    // image scalling : easy to place image
    image->Blit(surf, position);
  } else {
    // centering image
    Point2i pos = position;

    pos.x += (GetSizeX()/2) - (image->GetWidth()/2);
    pos.y += (GetSizeY()/2) - (image->GetHeight()/2);

    image->Blit(surf, pos);
  }
}

void Button::Pack()
{
  if (img_scale)
    image->ScaleSize(size);
}

