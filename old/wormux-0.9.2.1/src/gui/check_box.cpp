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
 * Checkbox in GUI.
 *****************************************************************************/

#include "gui/check_box.h"
#include "graphic/text.h"
#include "graphic/font.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "include/app.h"
#include "tool/resource_manager.h"

CheckBox::CheckBox(const std::string & label,
                   uint width,
                   bool value):
  Text(label,
       dark_gray_color,
       Font::FONT_SMALL,
       Font::FONT_BOLD,
       false,
       true),
  m_value(value),
  m_checked_image(NULL)
{
  Init(width);
}

CheckBox::CheckBox(Profile * profile,
                   const xmlNode * checkBoxNode) :
  Widget(profile, checkBoxNode),
  m_value(false),
  m_checked_image(NULL)
{
}

void CheckBox::Init(uint width)
{
  Profile * res = GetResourceManager().LoadXMLProfile( "graphism.xml", false);
  m_checked_image = GetResourceManager().LoadSprite(res, "menu/check");
  GetResourceManager().UnLoadXMLProfile(res);

  m_checked_image->cache.EnableLastFrameCache();

  position = Point2i(W_UNDEF, W_UNDEF);
  size.x = width;
  size.y = Text::GetHeight();
}

CheckBox::~CheckBox()
{
  if (NULL != m_checked_image) {
    delete m_checked_image;
  }
}

void CheckBox::Pack()
{
  Text::SetMaxWidth(size.x - m_checked_image->GetWidth() -2);
  size.y = std::max(uint(Text::GetHeight()),
		    m_checked_image->GetHeight());
}

bool CheckBox::LoadXMLConfiguration()
{
  if (NULL == profile || NULL == widgetNode) {
    return false;
  }

  XmlReader * xmlFile = profile->GetXMLDocument();

  ParseXMLPosition();
  ParseXMLSize();
  ParseXMLBorder();
  ParseXMLBackground();

  Text::LoadXMLConfiguration(xmlFile, widgetNode);

  std::string file;

  Surface picChecked;
  xmlFile->ReadStringAttr(widgetNode, "pictureChecked", file);
  file = profile->relative_path + file;
  if (!picChecked.ImgLoad(file)) {
    file = profile->relative_path + "menu/cbox_checked.png";
    if (!picChecked.ImgLoad(file)) {
      Error("XML Loading -> CheckBox: can't load " + file);
    }
  }

  Surface picUnchecked;
  xmlFile->ReadStringAttr(widgetNode, "pictureUnchecked", file);
  file = profile->relative_path + file;
  if (!picUnchecked.ImgLoad(file)) {
    file = profile->relative_path + "menu/cbox_unchecked.png";
    if (!picUnchecked.ImgLoad(file)) {
      Error("XML Loading -> CheckBox: can't load " + file);
    }
  }

  m_checked_image = new Sprite();
  m_checked_image->AddFrame(picChecked);
  m_checked_image->AddFrame(picUnchecked);

  //m_checked_image->cache.EnableLastFrameCache();

  return true;
}


void CheckBox::Draw(const Point2i &/*mousePosition*/) const
{
  Surface& surf = GetMainWindow();

  Text::DrawTopLeft(GetPosition());

  if (m_value) {
    m_checked_image->SetCurrentFrame(0);
  } else {
    m_checked_image->SetCurrentFrame(1);
  }

  m_checked_image->Blit(surf, GetPositionX() + GetSizeX() - 16, GetPositionY());
}

Widget * CheckBox::ClickUp(const Point2i &/*mousePosition*/,
                           uint /*button*/)
{
  NeedRedrawing();
  m_value = !m_value;
  return this;
}
