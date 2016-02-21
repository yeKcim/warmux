/******************************************************************************
 *  Wormux is a convivial mass murder game.
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
 * Sound Engine
 *****************************************************************************/

#ifndef JUKEBOX_H
#define JUKEBOX_H
//-----------------------------------------------------------------------------
#include <SDL.h>
#include <SDL_mixer.h>

#include <map>
#include <set>
#include <utility>
#include "../include/base.h"
//-----------------------------------------------------------------------------

class JukeBox 
{
private:

  typedef std::multimap<std::string, std::string>::value_type 
    sound_sample;
  typedef std::multimap<std::string, std::string>::iterator 
    sample_iterator;

  std::multimap<std::string, std::string> m_soundsamples;

  std::map<int, Mix_Chunk*> chunks;

  struct s_m_config{
    bool music;
    bool effects;
    int frequency;
    int channels; // (1 channel = mono, 2 = stereo, etc)
  } m_config;

  bool m_init;

  std::set<std::string> m_profiles_loaded;

  static void EndChunk(int channel);

public:
  JukeBox();
  void Init();
  void End(); 
  
  bool UseMusic() const {return m_config.music;};
  bool UseEffects() const {return m_config.effects;};
  int GetFrequency() const {return m_config.frequency;};
  int HowManyChannels() const {return m_config.channels;};
  void Pause();
  void Resume();
  
  void ActiveMusic (bool on) {m_config.music = on ;};
  void ActiveEffects (bool on) {m_config.effects = on;};

  void SetFrequency (int frequency);
  void SetNumbersOfChannel(int channels);

  void LoadXML(const std::string& profile);

  /** 
   * Playing a sound effect
   * @return the channel used to play the sample
   * <i>loop</i>: -1 for loop forever, else number of times to play
   */
  int Play(const std::string& category, 
	   const std::string& sample, 
	   const int loop = 1);
  
  int Stop(int channel);

  int StopAll();

private:
  /** 
   * Playing a sound effect
   * @return the channel used to play the sample
   * <i>loop</i>: -1 for loop forever, else number of times -1 to play
   */
  int PlaySample (Mix_Chunk * sample, int loop=0); 
};

extern JukeBox jukebox;
//-----------------------------------------------------------------------------
#endif
