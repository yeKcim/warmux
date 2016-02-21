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
 * Sound Sample : stupid class that avoids to take care of channel assigned by
 * SDL
 *****************************************************************************/

#include <SDL_mixer.h>
#include "sound/sound_sample.h"
#include "sound/jukebox.h"

SoundSample::SoundSample() : channel(-1)
{}

void SoundSample::Play(const std::string& category,
                       const std::string& sample,
                       const int loop)
{
  channel = JukeBox::GetInstance()->Play(category, sample, loop);
}

void SoundSample::Stop()
{
  if (channel != -1)
    JukeBox::GetInstance()->Stop(channel);
  channel = -1;
}

bool SoundSample::IsPlaying()
{
  return Mix_Playing(channel)>0;
}

