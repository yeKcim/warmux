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
 * A beautiful checkbox with picture and text
 *****************************************************************************/

#ifndef PICTURE_TEXT_CBOX_H
#define PICTURE_TEXT_CBOX_H

#include "gui/check_box.h"
#include "graphic/surface.h"

class PictureTextCBox : public CheckBox
{
  Surface m_image;
  Surface m_enabled;
  Surface m_disabled_front;
  Surface m_disabled_back;

public:
  PictureTextCBox(const std::string & label,
                  const std::string & resource_id,
                  const Point2i & size,
                  bool value = true,
                  Font::font_size_t fsize = Font::FONT_SMALL);
  PictureTextCBox(Profile * profile,
                  const xmlNode * pictureNode);

  virtual bool LoadXMLConfiguration(void);
  virtual void Draw(const Point2i & mousePosition);
  virtual void Pack();
};

#endif

