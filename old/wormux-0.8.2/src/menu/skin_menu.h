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
 * Skin menu, to visualize them easily
 *****************************************************************************/

#ifndef SKIN_MENU_H
#define SKIN_MENU_H

#include "menu.h"

class ListBoxWithLabel;
class Body;
class SpinButton;

class SkinMenu : public Menu
{
  ListBoxWithLabel *teams;
  ListBoxWithLabel *clothe_list;
  ListBoxWithLabel *movement_list;
  SpinButton       *frame;
  Body             *body;
  uint             position;

  void  LoadBody(const std::string& name);
  void  Display();

public:
  SkinMenu(const char* name = NULL);
  ~SkinMenu();

  void OnClick(const Point2i &mousePosition, int button);
  void OnClickUp(const Point2i &mousePosition, int button);
  void Draw(const Point2i&) { };

  bool signal_ok() { return true; };
  bool signal_cancel() { return true; };
};

#endif /* SKIN_MENU_H */
