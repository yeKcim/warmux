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
 * Game menu from which one may start a new game, modify options, obtain some
 * infomations or leave the game.
 *****************************************************************************/

#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "include/base.h"
#include "graphic/fps.h"
#include "graphic/surface.h"
#include "gui/widget_list.h"
#include "gui/button_text.h"
#include "gui/picture_widget.h"
#include "menu/menu.h"
#include <SDL.h>
#include <vector>

typedef enum
{
  menuNULL=0,
  menuPLAY,
  menuNETWORK,
  menuOPTIONS,
  menuCREDITS,
  menuQUIT
} menu_item;

class Main_Menu : public Menu
{
  /* If you need this, implement it (correctly) */
  Main_Menu(const Main_Menu&);
  Main_Menu operator=(const Main_Menu&);
  /**********************************************/

  ButtonText *play, *network, *options, *infos, *quit;

  Text *version_text, *website_text;


public:
  menu_item choice;

  Main_Menu();
  ~Main_Menu();
  menu_item Run ();

  void Redraw(const Rectanglei& rect, Surface& surf);

protected:
   bool signal_ok();
   bool signal_cancel();

private:
   virtual void DrawBackground();
   void OnClick(const Point2i &mousePosition, int button);
   void OnClickUp(const Point2i &mousePosition, int button);

  // Main drawing function: refresh parts of screen
  void Draw(const Point2i &mousePosition) {};
  void button_click();
};

#endif
