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
 * Picture widget: A widget containing a picture
 *****************************************************************************/

#include "gui/picture_widget.h"
#include "graphic/colors.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "include/app.h"

PictureWidget::PictureWidget(const Point2i & _size)
  : Widget(_size, false)
  , disabled(false)
  , loaded(false)
  , type(NO_SCALING)
  , picture_size(0, 0)
  , spr(NULL)
{
}

PictureWidget::PictureWidget(const Point2i & _size,
                             const std::string & resource_id,
                             ScalingType _type)
  : Widget(_size, false)
  , disabled(false)
  , loaded(false)
  , name(resource_id)
  , type(_type)
  , picture_size(0, 0)
  , spr(NULL)
{
}

PictureWidget::PictureWidget(const Surface & s, ScalingType type)
  : Widget(s.GetSize(), false)
  , disabled(false)
  , type(type)
  , spr(NULL)
{
  SetSurface(s, type);
}

PictureWidget::PictureWidget(Profile * profile,
                             const xmlNode * pictureNode)
  : Widget(profile, pictureNode)
  , disabled(false)
  , type(NO_SCALING)
  , spr(NULL)
{
}

PictureWidget::~PictureWidget()
{
  if (spr != NULL) {
    delete spr;
  }
}

/*
  Picture node example :
  <Picture file="menu/image.png"
           alpha="false"
           x="0" y="0"
           width="100" height="100"
           scale="true"
           antialiasing="true" />
*/
bool PictureWidget::LoadXMLConfiguration()
{
  if (NULL == profile || NULL == widgetNode) {
    //TODO error ... xml attributs not initialized !
    return false;
  }
  XmlReader * xmlFile = profile->GetXMLDocument();

  std::string file("menu/pic_not_found.png");
  xmlFile->ReadStringAttr(widgetNode, "file", file);

  bool activeAlpha = false;
  xmlFile->ReadBoolAttr(widgetNode, "alpha", activeAlpha);

  file = profile->relative_path + file;
  Surface surface(file.c_str());

  if (!activeAlpha) {
    surface = surface.DisplayFormat();
  } else {
    surface = surface.DisplayFormatAlpha();
  }

  ParseXMLGeometry();
  ParseXMLMisc();

  bool activeScale = false;
  xmlFile->ReadBoolAttr(widgetNode, "scale", activeScale);

  SetSurface(surface, activeScale ? FIT_SCALING : NO_SCALING);
  return true;
}

void PictureWidget::ApplyScaling(ScalingType t)
{
  if (spr) {
    Point2d scale(Double(size.x) / picture_size.x,
                  Double(size.y) / picture_size.y);

    if (size.x==0 || size.y==0)
      return;

    switch (t) {
    case NO_SCALING: break;
    case X_SCALING: spr->Scale(scale.x, 1.0); break;
    case Y_SCALING: spr->Scale(1.0, scale.x); break;
    case STRETCH_SCALING: spr->Scale(scale.x, scale.y); break;
    case FIT_SCALING:
    default:
      {
        Double zoom = std::min(scale.x, scale.y);
        spr->Scale(zoom, zoom);
        break;
      }
    }
  }

  type = t;
}

void PictureWidget::SetSurface(const Surface & s, ScalingType type_)
{
  if (spr)
    delete spr;

  picture_size = s.GetSize();
  spr = new Sprite(s);
  type = type_;
  loaded = true;
  // Don't call immediately ApplyScaling(type) to save on rotozooms
  NeedRedrawing();
}

void PictureWidget::SetNoSurface()
{
  NeedRedrawing();

  if (spr)
    delete spr;
  spr = NULL;
}

void PictureWidget::Draw(const Point2i &/*mousePosition*/)
{
  if (!loaded) {
    if (name.empty())
      return;
    Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml", false);
    SetSurface(LOAD_RES_IMAGE(name), type);

    // Needed to set the resizing
    ApplyScaling(type);
  }

  if (!spr)
    return;

  Surface & surf = GetMainWindow();
  Point2i pos = GetPicturePosition();

  spr->Blit(surf, pos);

  // Draw a transparency mask
  if (disabled) {
    surf.BoxColor(Rectanglei(pos, spr->GetSize()), defaultOptionColorBox);
  }
}

Point2i PictureWidget::GetPicturePosition() const
{
  return position + (size - spr->GetSize()) / 2;
}

Point2f PictureWidget::GetScale() const
{
  return Point2f(spr->GetScaleX().tofloat(), spr->GetScaleY().tofloat());
}
