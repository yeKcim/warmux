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

#ifndef SOUND_SAMPLE_H
#define SOUND_SAMPLE_H
//-----------------------------------------------------------------------------
#include <string>
#include <map>
//-----------------------------------------------------------------------------

class SoundSample
{
 private:
  static std::map<int, SoundSample*> sound_samples_channel;
  int channel;

 public:
  SoundSample();
  ~SoundSample();

  // return false if sound can't be played because it is already playing
  bool Play(const std::string& category,
            const std::string& sample,
            const int loop = 1);
  void Stop();

  bool IsPlaying();


  static void ChannelFinished(int channel);
};
//-----------------------------------------------------------------------------
#endif
