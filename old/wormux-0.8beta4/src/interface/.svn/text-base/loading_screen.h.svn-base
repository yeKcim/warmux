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
 * Loading screen
 *****************************************************************************/

#ifndef LOADING_SCREEN_H
#define LOADING_SCREEN_H

#include "include/base.h"
#include <string>

// Forwar declarations
class Profile;
class Sprite;

class LoadingScreen
{
 private:
  Profile * res;
  Sprite * loading_bg;

 public:
  LoadingScreen();
  ~LoadingScreen();

  void DrawBackground();
  void StartLoading(uint nb, const std::string& resource,
                    const std::string& label) const;
};

#endif
