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
 *****************************************************************************/

#ifndef VIDEO_H
#define VIDEO_H

#include <string>
#include <list>
#include "surface.h"
#include "include/base.h"

class Video{
 private:
  uint m_max_fps;     // If equals to zero, it means no limit
  uint m_max_delay;
  bool SDLReady;
  bool fullscreen;
  SDL_Surface *icon;

  std::list<Point2i> available_configs;
  void ComputeAvailableConfigs();

  void SetWindowIcon(const std::string& icon);
  void InitSDL(void);

public:
  Surface window;
  void SetWindowCaption(const std::string& caption) const;
  void SetMaxFps(uint max_fps);
  uint GetMaxFps() const { return m_max_fps; };
  uint GetMaxDelay() const { return m_max_delay; };
  void AddConfigIfAbsent(int w, int h);

public:
  Video();
  ~Video();

  bool IsFullScreen() const { return fullscreen; };

  const std::list<Point2i>& GetAvailableConfigs() const { return available_configs; };
  bool SetConfig(int width, int height, bool fullscreen);
  void ToggleFullscreen();

  void Flip(void);
};

Surface& GetMainWindow();

#endif
