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
 * Options menu
 *****************************************************************************/

#ifndef OPTIONS_MENU_H
#define OPTIONS_MENU_H

#include "menu.h"
#include "../include/base.h"
#include "../graphic/font.h"

class OptionMenu : public Menu
{
  Font * normal_font;

public:
   OptionMenu(); 
   ~OptionMenu();

private:

   /* Graphic options controllers */   
   ListBox *lbox_video_mode;
   CheckBox *opt_display_wind_particles;
   CheckBox *opt_display_energy;
   CheckBox *opt_display_name;
   CheckBox *full_screen; 
   SpinButtonBig *opt_max_fps;

   /* Sound options controllers */
   ListBox *lbox_sound_freq;
   CheckBox *opt_music;
   CheckBox *opt_sound_effects;

   void SaveOptions();
   void OnClic(const Point2i &mousePosition, int button);
   void Draw(const Point2i &mousePosition);

   void __sig_ok();
   void __sig_cancel();
};

#endif
