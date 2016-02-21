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
 * Label in GUI.
 *****************************************************************************/

#include "label.h"
//-----------------------------------------------------------------------------
#include "../include/app.h"

using namespace Wormux;
//-----------------------------------------------------------------------------


Label::Label (const std::string &label, uint x, uint y, uint w, Font& _font) :
  Widget(x, y, w, _font.GetHeight())
{
  txt_label = new Text(label, white_color, &_font);
}

//-----------------------------------------------------------------------------

Label::~Label()
{
  delete txt_label;
}

//-----------------------------------------------------------------------------

void Label::Draw (uint mouse_x, uint mouse_y)
{
  txt_label->DrawTopLeft(x, y);
}

//-----------------------------------------------------------------------------
void Label::SetSizePosition(uint _x, uint _y, uint _w, uint _h)
{
  StdSetSizePosition(_x, _y, _w, _h);
}
//-----------------------------------------------------------------------------
