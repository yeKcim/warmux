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
 *****************************************************************************/

#ifndef VIDEO_H
#define VIDEO_H

#include <SDL.h>
#include <string>
#include "surface.h"
#include "../include/base.h"

class Video{
  uint m_max_fps;     // If equals to zero, it means no limit
  uint m_sleep_max_fps;
  bool SDLReady;
  bool fullscreen;
  void SetWindowCaption(std::string caption);
  void SetWindowIcon(std::string icon);

public:
  Surface window;
  void SetMaxFps(uint max_fps);
  uint GetMaxFps();
  uint GetSleepMaxFps();

public:
  Video();
  ~Video();

  bool IsFullScreen(void) const;

  bool SetConfig(int width, int height, bool fullscreen);

  void InitWindow(void);
  void InitSDL(void);

  void Flip(void);
}; 

#endif
