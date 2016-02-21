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
 * Application Wormux.
 *****************************************************************************/

#ifndef APP_WORMUX_H
#define APP_WORMUX_H

#include <string>
#include "base.h"

class Video;
class Menu;

class AppWormux
{
public:
  Video *video;

private:
  Menu *menu;
  static AppWormux * singleton;

private:
  void InitWindow();
  void DisplayLoadingPicture();
  void End() const;
  void DisplayWelcomeMessage() const;
  AppWormux();

public:
  void SetCurrentMenu(Menu *menu);
  void RefreshDisplay();
  static void DisplayError(const std::string &msg);
  void ReceiveMsgCallback(const std::string& msg);
  static AppWormux * GetInstance();
  ~AppWormux();

  int Main(void);
};

#endif
