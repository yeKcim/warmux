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

std::map<int, SoundSample*> SoundSample::sound_samples_channel;

void SoundSample::ChannelFinished(int channel)
{
  std::map<int, SoundSample*>::iterator it=sound_samples_channel.find(channel);

  if (it != sound_samples_channel.end()) {
    SoundSample* s = it->second;
    s->channel = -1;
    sound_samples_channel.erase(it);
  }
}

SoundSample::SoundSample() : channel(-1)
{}

SoundSample::~SoundSample()
{
  if (channel == -1)
    return;

  // removing sample from the table
  std::map<int, SoundSample*>::iterator it=sound_samples_channel.find(channel);

  if (it != sound_samples_channel.end()) {
    SoundSample* s = it->second;
    ASSERT(s == this);
    sound_samples_channel.erase(it);
  }
}

bool SoundSample::Play(const std::string& category,
                       const std::string& sample,
                       const int loop)
{
  if (!IsPlaying()) {
    channel = JukeBox::GetInstance()->Play(category, sample, loop);
    sound_samples_channel.insert(std::make_pair(channel, this));
    return true;
  }
  return false;
}

void SoundSample::Stop()
{
  if (IsPlaying())
    JukeBox::GetInstance()->Stop(channel);
  channel = -1;
}

bool SoundSample::IsPlaying()
{
  if (channel == -1)
    return false;

  return Mix_Playing(channel)>0;
}

