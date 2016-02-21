/******************************************************************************
 *  Wormux is a convivial mass murder game.
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
 * Generic menu
 *****************************************************************************/

#ifndef MENU_H
#define MENU_H

#include "../graphic/sprite.h"
#include "../gui/button.h"
#include "../gui/list_box.h"
#include "../gui/list_box_w_label.h"
#include "../gui/check_box.h"
#include "../gui/spin_button.h"
#include "../gui/spin_button_big.h"
#include "../gui/spin_button_picture.h"
#include "../gui/box.h"
#include "../gui/question.h"
#include "../gui/label.h"
#include "../gui/null_widget.h"
#include "../gui/picture_widget.h"
#include "../gui/picture_text_cbox.h"
#include "../gui/container.h"
#include "../gui/text_box.h"

typedef enum {
  vOkCancel,
  vOk,
  vCancel,
  vNo
} t_action;

class Menu : public Container
{
public:
   WidgetList widgets;
   const t_action actions;

   Menu(char* bg, t_action actions = vOkCancel); 
   virtual ~Menu();

   void Run ();
   virtual void Redraw(const Rectanglei& rect, Surface& surf);

private:
   Sprite *background;

   /* Actions buttons  */
   HBox *actions_buttons;

   bool BasicOnClic(const Point2i &mousePosition);

protected:
   Button *b_cancel;
   Button *b_ok;
   bool close_menu;

   virtual void sig_ok();
   virtual void sig_cancel();
   virtual void DrawBackground();

   virtual void key_ok() {};
   virtual void key_cancel() {};
   virtual void __sig_ok() = 0;
   virtual void __sig_cancel() = 0;
   virtual void Draw(const Point2i &mousePosition) = 0;   
   virtual void OnClic(const Point2i &mousePosition, int button) = 0;
   void SetActionButtonsXY(int x, int y);
   void Display(const Point2i& mousePosition);
};

#endif
