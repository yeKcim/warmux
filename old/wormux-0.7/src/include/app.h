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
 * Application Wormux.
 *****************************************************************************/

#ifndef APP_WORMUX_H
#define APP_WORMUX_H

#include <SDL.h>
#include <string>
#include "base.h"
#include "../game/config.h"
#include "../graphic/video.h"

class AppWormux
{
public:
  Video video;

private:
  int x, y;
  std::string titre_fenetre;

  static AppWormux * singleton;

private:
  void Init(int, char**);
  void InitNetwork(int argc, char **argv);
  void InitScreen();
  void InitWindow();
  void DisplayLoadingPicture();
  void InitFonts();
  void End();
  void DisplayWelcomeMessage();
  AppWormux();

public:
  static AppWormux * GetInstance();

  int main( int argc, char ** argv );
};

#endif
