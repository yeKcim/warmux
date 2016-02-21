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
 * Spinbutton in GUI.
 *****************************************************************************/

#ifndef SPIN_BUTTON_H
#define SPIN_BUTTON_H
//-----------------------------------------------------------------------------
#include "../include/base.h"
#include "../graphic/text.h"
#include "widget.h"
#include "button.h"
#include <string>
#include <sstream>
//-----------------------------------------------------------------------------

class SpinButton : public Widget
{
 private:
  Text *txt_label, *txt_value;

  int m_value;
  int m_min_value, m_max_value, m_step;
  Button *m_plus, *m_minus;
  
 public:
  SpinButton(const std::string &label, uint x, uint y, uint w,
	     int value=0, int step=1, int min_value=-1, int max_value=-1);
  virtual ~SpinButton();

  void SetSizePosition(uint _x, uint _y, uint _w, uint _h);

  void Draw (uint mouse_x, uint mouse_y) ;
  bool Clic (uint mouse_x, uint mouse_y, uint button) ;
  int GetValue() const;
  void SetValue(int value);
};

//-----------------------------------------------------------------------------
#endif

