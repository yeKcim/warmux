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
 * Application Warmux.
 *****************************************************************************/

#ifndef APP_WARMUX_H
#define APP_WARMUX_H

#include <string>
#include <WARMUX_base.h>

class Video;
class Menu;
class Color;
union SDL_Event;

class AppWarmux
{
public:
  Video *video;

private:
  Menu *menu;
  static AppWarmux * singleton;

  AppWarmux();
  void InitWindow();
  void DisplayLoadingPicture();
  void DisplayWelcomeMessage() const;
  void End() const;

public:
  Menu* GetCurrentMenu() const { return menu; }
  void SetCurrentMenu(Menu *_menu) { menu = _menu; }
  void RefreshDisplay();
  static void DisplayError(const std::string &msg);
  void ReceiveMsgCallback(const std::string& msg, const Color& color, bool in_game = false);
  static AppWarmux * GetInstance();
  ~AppWarmux();
  static void EmergencyExit();

  int Main(void);

  // Calls this to check if Warmux must be moved to background
  bool CheckInactive(SDL_Event& evnt);
};

#endif
