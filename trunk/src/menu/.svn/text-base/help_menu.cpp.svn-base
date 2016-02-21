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
 * Help Menu
 *****************************************************************************/

#include "include/app.h"
#include "graphic/text.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "gui/control_config.h"
#include "gui/figure_widget.h"
#include "gui/tabs.h"
#include "menu/help_menu.h"
#include "game/config.h"
#include "tool/resource_manager.h"
#include "tool/xml_document.h"

HelpMenu::HelpMenu()
  : Menu("help/background", vCancel)
{
  int window_w = GetMainWindow().GetWidth();
  int window_h = GetMainWindow().GetHeight();

  float factor = (window_w < 640) ? 0.02f : 0.05f;
  int border   = window_w * factor;
  int max_w    = window_w - 2*border;
  int max_h    = window_h - actions_buttons->GetSizeY() - border;

  MultiTabs    *tabs = new MultiTabs(Point2i(max_w, max_h));
  FigureWidget *w;
  tabs->SetPosition(border, border);

  /********                     Start game menu                    ********/
  w = new FigureWidget(Point2i(max_w,
                               tabs->GetSizeY() - tabs->GetHeaderHeight()),
                       "help/startgame_menu", Font::FONT_BIG);
  w->AddCaption(_("Select number of playing teams"), 216, 28, 432); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Change number of players per team"), 619, 35, 362); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Click/wheelmouse to change the team.\nYou can't select the same team several times."), 167, 176, 325);
  w->AddCaption(_("Click to let a player handle the team instead of the AI, and vice versa"), 574, 187, 445); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Map author information: name, email, nickname, country"), 511, 263, 557); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Click on either side of the central (selected) map or use wheelmouse to change the selected map."), 388, 324, 753);
  tabs->AddNewTab("unused", _("Game menu"), w);

  /********                     Game mode menu                    ********/
  w = new FigureWidget(Point2i(max_w,
                               tabs->GetSizeY() - tabs->GetHeaderHeight()),
                       "help/game_mode", Font::FONT_BIG);
  w->AddCaption(_("Experiment new game rules"), 192, 70, 381);// Select between "classic", "blitz" and "unlimited" rules
  w->AddCaption(_("Increase this if you are a slow player"), 193, 24, 381);// TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("When a character can be switched"), 607, 24, 381);// TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("More energy, more shots needed"), 607, 70, 379);// TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("If you like medkits,\nincrease this"), 191, 361, 379);// TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Increase this if you like long games"), 191, 407, 381);// TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Increase this for faster death mode end"), 606, 407, 381);// TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("The bigger the faster you'll fall!"), 607, 361, 381);// TRANSLATORS: please be imaginative and keep it short!
  tabs->AddNewTab("unused", _("Game mode"), w);

  w = new FigureWidget(Point2i(max_w,
                               tabs->GetSizeY() - tabs->GetHeaderHeight()),
                       "help/ingame", Font::FONT_LARGE);
  w->AddCaption(_("Minimap, more details in another tab"), 313, 56, 595); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Interface and touch controls, more details in other tabs"), 399, 334, 737); // TRANSLATORS: please be imaginative and keep it short!
  tabs->AddNewTab("unused", _("Ingame display"), w);

  w = new FigureWidget(Point2i(max_w,
                               tabs->GetSizeY() - tabs->GetHeaderHeight()),
                       "help/interface", Font::FONT_LARGE);
  w->AddCaption(_("Wind strength and direction.\nClick here to switch to touch controls."), 389, 48, 775); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Character name.\nCharacter energy.\nTeam name."), 228, 152, 445); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Teams in decreasing order of total energy"), 625, 150, 331); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Current weapon and ammo left, long click to display its help.\nClick to open selection menu."), 238, 392, 474); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Time left in this turn and time elapsed since game start.\nClick to pause"), 639, 392, 315); // TRANSLATORS: please be imaginative and keep it short!
  tabs->AddNewTab("unused", _("Interface"), w);

  w = new FigureWidget(Point2i(max_w,
                               tabs->GetSizeY() - tabs->GetHeaderHeight()),
                       "help/touch interface", Font::FONT_LARGE);
  w->AddCaption(_("Touch/click to fire / start filling weapon speed gauge"), 415, 42, 707); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Touch/click the left or right arrows to move respectively left or right"), 204, 150, 400); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Touch/click the left or right arrows to respectively raise or lower the aim"), 597, 155, 359); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Touch/click to jump. Maintaining the touch/click to high jump."), 354, 345, 705); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Touch/click to change the weapon timer"), 382, 428, 758); // TRANSLATORS: please be imaginative and keep it short!
  tabs->AddNewTab("unused", _("Touch controls"), w);

  w = new FigureWidget(Point2i(max_w,
                               tabs->GetSizeY() - tabs->GetHeaderHeight()),
                       "help/replay", Font::FONT_LARGE);
  w->AddCaption(_("Switch between classical and replay interface"), 415, 37, 707); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Right button to resume/pause replay.\nSquare right button to immediately exit it."), 230, 138, 445); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Change replay speed.\nRight part displays current speed."), 624, 138, 327); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Replay progress bar, not used for precise seeking.\nShort click for a +30s jump, long for +60s jump."), 238, 375, 475); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Current turn time info.\nClick to launch the pause menu."), 640, 375, 315); // TRANSLATORS: please be imaginative and keep it short!
  tabs->AddNewTab("unused", _("Replay controls"), w);

  w = new FigureWidget(Point2i(max_w,
                               tabs->GetSizeY() - tabs->GetHeaderHeight()),
                       "help/minimap", Font::FONT_HUGE);
  w->AddCaption(_("Area currently displayed"), 205, 270, 405);// TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Position of the characters.\nA circle surrounds the active character."), 610, 335, 375);// TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Position of bonus boxes or medkits"), 207, 400, 405);// TRANSLATORS: please be imaginative and keep it short!
  tabs->AddNewTab("unused", _("Minimap"), w);

#if !defined(ANDROID) && !defined(__SYMBIAN32__)
  w = new FigureWidget(Point2i(max_w,
                               tabs->GetSizeY() - tabs->GetHeaderHeight()),
                       "help/shortkeys", (Font::font_size_t)14);
  w->AddCaption(_("Quit game"), 81, 13, 132); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("High jump"), 439, 326, 132); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Jump backwards"), 439, 297, 132); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Jump backwards"), 439, 356, 132); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Drag&drop: Move camera"), 520, 395, 132); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Center camera on character"), 520, 422, 132); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Show/hide the interface"), 271, 43, 132); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Toggle fullscreen"), 491, 42, 132); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Configuration menu"), 491, 72, 132); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Talk in network battles"), 92, 296, 132); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Change weapon category"), 81, 72, 132); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Change weapon countdown"), 618, 164, 132); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Change aim angle"), 618, 194, 132); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Move character"), 618, 286, 132); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("On map: select a target"), 618, 225, 132); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("On a character: select it"), 618, 256, 132); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Show weapons menu"), 618, 134, 132); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Smaller aim angle and walk step"), 92, 326, 132); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Move camera with mouse or arrows"), 386, 394, 132); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Weapon: Fire / Bonus box: drop"), 260, 326, 132); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Toggle minimap"), 271, 72, 132); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Change active character"), 92, 356, 132); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Center camera on character"), 386, 422, 132); // TRANSLATORS: please be imaginative and keep it short!
  w->AddCaption(_("Quickly quit game with Ctrl"), 81, 43, 132); // TRANSLATORS: please be imaginative and keep it short!
  tabs->AddNewTab("unused", _("Keyboard"), w);
#endif

  tabs->AddNewTab("unused", _("Current controls"),
                  new ControlConfig(tabs->GetSize(), true));

  widgets.AddWidget(tabs);
  widgets.Pack();
}
