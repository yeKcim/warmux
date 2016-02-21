/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
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

#include <WARMUX_base.h>
#include <WARMUX_singleton.h>

#include "sample_cache.h"

typedef struct _Mix_Music Mix_Music;

//-----------------------------------------------------------------------------

/* Informations about music...
 *
 * Now, there is a playlist in JukeBox class.
 * A profil is opened in $DATA$/music/profile.xml, where there is a list of
 * "music" sections like this :
 *   <music type="menu" playlist="menu/menu.m3u" />
 *
 * You must create a m3u file, where all music files are listed (one per line)
 * with a relative or an absolute path.
 *
 * A comment is a line begining with a '#' char.
 *
 * When warmux is launched, there is a menu playlist and a ingame playlist.
 * At start of a playlist, a random music is selected, after that, at the end
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
  typedef struct
  {
    std::string filename;
    float       level;
  } sample_info;
  typedef std::multimap<std::string, sample_info>::value_type
    sound_sample;
  typedef std::multimap<std::string, sample_info>::iterator
    sample_iterator;
  std::multimap<std::string, sample_info> m_soundsamples;

  std::map<int, Mix_Chunk*> chunks;
  Mix_Music* music;

  typedef std::map<std::string, std::vector<std::string> > PlayListMap;
  PlayListMap playlist;

  PlayListMap::const_iterator playing_pl;
  std::vector<std::string>::const_iterator playing_music;

  int channels; // (1 channel = mono, 2 = stereo, etc)
  bool m_init;
  SampleCache m_cache;

  std::set<std::string> m_profiles_loaded;

  static void EndChunk(int channel);
  static void EndMusic();

  bool PlayMusicSample(const std::vector<std::string>::const_iterator& file);

protected:
  JukeBox();
  ~JukeBox() { End(); }
  friend class Singleton<JukeBox>;

public:
  static uint GetMaxVolume();
  static void SetMusicVolume(uint vol);

  void Init();
  void End();
  bool OpenDevice();
  void CloseDevice();

  void Pause(bool all=false) const;
  void Resume(bool all=false) const;

  void ActiveMusic(bool on);

  void SetFrequency(uint frequency);

  void LoadXML(const std::string& profile);

  void LoadMusicXML();

  bool PlayMusic(const std::string& type);
  void StopMusic();
  void NextMusic();

  bool IsPlayingMusic() const { return (!playlist.empty() && playing_pl != playlist.end()); }
  bool IsPlayingMusicSample() const { return (IsPlayingMusic() && playing_music != playing_pl->second.end()); }

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
  int PlaySample(Mix_Chunk * sample, float level=1.0, int loop=0);
};
//-----------------------------------------------------------------------------
#endif
