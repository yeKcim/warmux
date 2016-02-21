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
 * Sound Engine
 *****************************************************************************/

#ifndef JUKEBOX_H
#define JUKEBOX_H
//-----------------------------------------------------------------------------
#include <vector>
#include <map>
#include <set>
#include <utility>

#include <SDL.h>
#include <SDL_mixer.h>

#include "include/base.h"
#include "include/singleton.h"
#include "sample_cache.h"

//-----------------------------------------------------------------------------

/* Informations about music...
 *
 * Now, there is a playlist in JukeBox class.
 * A profil is openned in $DATA$/music/profile.xml, where there is a list of
 * "music" sections like this :
 *   <music type="menu" playlist="menu/menu.m3u" />
 *
 * Your must create a m3u file, where there are listed (one per line) all music
 * files with a relative or an absolute path.
 *
 * A comment is a line begined with a '#' char.
 *
 * When wormux is launched, there is a menu playlist and a ingame playlist.
 * At startup of a playlist, a random music is selected, after that, at the end
 * of the current music, we play the next music in list. If this is the last
 * music, we go back to the first music...
 *
 * Music is a separate section of JukeBox class, with his own members functions
 * and variables.
 *
 * Call LoadMusicXML() will load profile and playlists.
 * Next, a call to PlayMusic(profile_name) will stop any playing playlist, and
 * will play request playlist.
 * You can use NextMusic() and StopMusic().
 *
 * There isn't any PauseMusic() and ResumeMusic(), but if it is necessary it can
 * easily be done.
 * Note that Pause() and Resume() are used for CHUNK SOUNDS ONLY.
 *
 * -Progs
 */

class JukeBox : public Singleton<JukeBox>
{
private:
  /* If you need this, implement it (correctly) */
  JukeBox(const JukeBox&);
  JukeBox operator=(const JukeBox&);
  /**********************************************/

  typedef std::multimap<std::string, std::string>::value_type
    sound_sample;
  typedef std::multimap<std::string, std::string>::iterator
    sample_iterator;

  std::multimap<std::string, std::string> m_soundsamples;

  std::map<int, Mix_Chunk*> chunks;
  Mix_Music* music;

  typedef std::map<std::string, std::vector<std::string> > PlayListMap;
  PlayListMap playlist;

  PlayListMap::const_iterator playing_pl;
  std::vector<std::string>::const_iterator playing_music;

  struct s_m_config{
    bool music;
    bool effects;
    int frequency;
    int channels; // (1 channel = mono, 2 = stereo, etc)
  } m_config;

  bool m_init;
  SampleCache m_cache;

  std::set<std::string> m_profiles_loaded;

  static void EndChunk(int channel);
  static void EndMusic();

  bool PlayMusicSample(const std::vector<std::string>::const_iterator& file);

protected:
  JukeBox();
  friend class Singleton<JukeBox>;

public:
  static uint GetMaxVolume() { return MIX_MAX_VOLUME; }
  static void SetMusicVolume(uint vol) { Mix_VolumeMusic(vol); }

  void Init();
  void End();

  bool UseMusic() const {return m_config.music;};
  bool UseEffects() const {return m_config.effects;};
  int GetFrequency() const {return m_config.frequency;};
  int HowManyChannels() const {return m_config.channels;};
  void Pause() const;
  void Resume() const;

  void ActiveMusic (bool on);
  void ActiveEffects (bool on) {m_config.effects = on;};

  void SetFrequency (int frequency);
  void SetNumbersOfChannel(int channels); // Not used

  void LoadXML(const std::string& profile);

  void LoadMusicXML();

  bool PlayMusic(const std::string& type);
  void StopMusic();
  void NextMusic();

  inline bool IsPlayingMusic() const
    {
      return (!playlist.empty() && playing_pl != playlist.end());
    }
  inline bool IsPlayingMusicSample() const
    {
      return (IsPlayingMusic() && playing_music != playing_pl->second.end());
    }

  /**
   * Playing a sound effect
   * @return the channel used to play the sample
   * <i>loop</i>: -1 for loop forever, else number of times to play
   */
  int Play(const std::string& category,
           const std::string& sample,
           const int loop = 1);

  int Stop(int channel) const;

  int StopAll() const;

private:
  /**
   * Playing a sound effect
   * @return the channel used to play the sample
   * <i>loop</i>: -1 for loop forever, else number of times -1 to play
   */
  int PlaySample (Mix_Chunk * sample, int loop=0);
};
//-----------------------------------------------------------------------------
#endif
