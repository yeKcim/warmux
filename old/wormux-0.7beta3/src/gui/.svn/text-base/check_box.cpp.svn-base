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
 * Checkbox in GUI.
 *****************************************************************************/

#include "check_box.h"
//-----------------------------------------------------------------------------
#include <algorithm>
#include "../include/app.h"
#include "../graphic/sprite.h"
#include "../tool/resource_manager.h"
#include "../include/global.h"

using namespace Wormux;
//-----------------------------------------------------------------------------


CheckBox::CheckBox (const std::string &label, uint x, uint y, uint w, bool value) :
  Widget(x, y, w, global().small_font().GetHeight())
{
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);   
  m_image = resource_manager.LoadSprite( res, "menu/check");
  m_image->EnableLastFrameCache();

  // Copy arguments
  m_value = value;

  txt_label = new Text(label, white_color, &global().small_font());
}

//-----------------------------------------------------------------------------

CheckBox::~CheckBox()
{
  delete m_image;
  delete txt_label;
}

//-----------------------------------------------------------------------------

void CheckBox::Draw (uint mouse_x, uint mouse_y)
{
  txt_label->DrawTopLeft(x, y);
 
  if (m_value)
    m_image->SetCurrentFrame(0);
  else 
    m_image->SetCurrentFrame(1);

  m_image->Blit(app.sdlwindow, x+w-16, y);
}

//-----------------------------------------------------------------------------

bool CheckBox::Clic (uint mouse_x, uint mouse_y, uint button)
{
  if (!MouseIsOver(mouse_x, mouse_y)) return false;

  m_value = !m_value ;
  return true ;
}

//-----------------------------------------------------------------------------
void CheckBox::SetSizePosition(uint _x, uint _y, uint _w, uint _h)
{
  StdSetSizePosition(_x, _y, _w, _h);
}
//-----------------------------------------------------------------------------

bool CheckBox::GetValue()  const { return m_value; }
void CheckBox::SetValue(bool value)  { m_value = value; }

//-----------------------------------------------------------------------------
