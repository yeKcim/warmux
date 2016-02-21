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
 * Sound engine
 *****************************************************************************/

#include "jukebox.h"
#include <iostream>

#include "../game/config.h"
#include "../tool/debug.h"
#include "../tool/i18n.h"
#include "../tool/random.h"
#include "../tool/file_tools.h"

JukeBox jukebox;

JukeBox::JukeBox()
{
  m_init = false;
  
  m_config.music = true;
  m_config.effects = true;
  m_config.frequency = 44100; //MIX_DEFAULT_FREQUENCY;
  m_config.channels = 2; // stereo
}

void JukeBox::Pause() 
{
    Mix_Pause(-1);
    Mix_PauseMusic();
}

void JukeBox::Resume() 
{    
    Mix_Resume(-1);
    Mix_ResumeMusic();
}

void JukeBox::Init() 
{
  if (!m_config.music && !m_config.effects) {
    End();
    return;
  }

  if (m_init) return;

  Uint16 audio_format = MIX_DEFAULT_FORMAT;

  /* Initialize the SDL library */
  if ( SDL_Init(SDL_INIT_AUDIO) < 0 ) {
    std::cerr << "Couldn't initialize SDL: "<< SDL_GetError() << std::endl;
    return;
  } 
  
  int audio_buffer = 4096;

  /* Open the audio device */
  if (Mix_OpenAudio(m_config.frequency, audio_format, m_config.channels, audio_buffer) < 0) {
    std::cerr << "Couldn't open audio: " <<  SDL_GetError() << std::endl;
    return;
  } else {
    Mix_QuerySpec(&m_config.frequency, &audio_format, &m_config.channels);
    std::cout << "Opened audio at " << m_config.frequency <<" Hz "<< (audio_format&0xFF) 
	      <<" bit " << std::endl;
  }
  m_init = true;  
  
  LoadXML("share");
}

void JukeBox::End() 
{
  if (!m_init) return;
  m_init = false;

  StopAll();

  m_soundsamples.clear();
  m_profiles_loaded.clear();

  Mix_CloseAudio();
}

void JukeBox::SetFrequency (int frequency)
{
  if ((frequency != 11025) 
      && (frequency != 22050) 
      && (frequency != 44100)) frequency = 44100;

  if (m_config.frequency == frequency) return;

  m_config.frequency = frequency;

  // Close and reopen audio device
  End();
  Init();
}

void JukeBox::SetNumbersOfChannel(int channels)
{
  if (m_config.channels == channels) return;

  m_config.channels = channels;

  // Close and reopen audio device
  End();
  Init();
}

void JukeBox::LoadXML(const std::string& profile)
{

  if (!UseEffects()) return;

  // is xml_file already loaded ?
  std::set<std::string>::iterator it_profile = m_profiles_loaded.find(profile) ;
  if (it_profile !=  m_profiles_loaded.end()) {
	  MSG_DEBUG("jukebox", "Profile %s is already loaded !", profile.c_str());
      return;
  } 
  LitDocXml doc;

  // Load the XML
  std::string folder = Config::GetInstance()->GetDataDir() + PATH_SEPARATOR + "sound"+ PATH_SEPARATOR + profile + PATH_SEPARATOR;
  std::string xml_filename = folder + "profile.xml";
  if( !IsFileExist(xml_filename) ){
    std::cerr << "[Sound] Error : file " << xml_filename << " not found" << std::endl;
    return;
  }
  if( !doc.Charge (xml_filename) )
    return;

  xmlpp::Node::NodeList nodes = doc.racine() -> get_children("sound");
  xmlpp::Node::NodeList::iterator 
    it=nodes.begin(),
    fin=nodes.end();

  for (; it != fin; ++it)
    {
      // lit le XML
      xmlpp::Element *elem = dynamic_cast<xmlpp::Element*> (*it);
      std::string sample="no_sample";
      std::string file="no_file";
      LitDocXml::LitAttrString(elem, "sample", sample);
      LitDocXml::LitAttrString(elem, "file", file);

	  MSG_DEBUG("jukebox", "Load sound sample %s/%s: %s", profile.c_str(), sample.c_str(), file.c_str());

      // Charge le son
      std::string sample_filename = folder + file;
      if( !IsFileExist(sample_filename) ){
	std::cerr << "Sound error: File " << sample_filename.c_str() 
		  << " does not exist !" << std::endl; 
	continue;
      }
	
      // Inserting sound sample in list
      m_soundsamples.insert(sound_sample(profile+"/"+sample, sample_filename));  
    }

  // The profile is loaded
  m_profiles_loaded.insert(profile);
}

int JukeBox::Play (const std::string& category, const std::string& sample, 
		   const int loop)
{
  if (!UseEffects()) return -1;

  uint nb_sons= m_soundsamples.count(category+"/"+sample);
  if (nb_sons) 
  {
    std::pair<sample_iterator, sample_iterator> p = m_soundsamples.equal_range(category+"/"+sample);
    sample_iterator it = p.first;

    // Choose a random sound sample
    if (nb_sons > 1)
    {
      uint selection = uint(randomObj.GetLong(0, nb_sons));
      if (selection == nb_sons) --selection ;

      it = p.first ;
      
      for ( uint i=0 ; i<selection && it!=p.second ; ++i ) it++ ;
    }

    // Play the sound
    Mix_Chunk * sampleChunk = Mix_LoadWAV(it->second.c_str());

    return PlaySample(sampleChunk, loop);
  }
  else if (category != "default") { // try with default profile
    return Play("default", sample, loop) ; // try with default profile
  } 

  MSG_DEBUG("jukebox", "Error: No sound found for sample %s/%s", category.c_str(), sample.c_str());
  return -1;
}

int JukeBox::Stop (int channel)
{
  if(!m_config.music && !m_config.effects) return 0;
  if (channel == -1) return 0;
  return Mix_HaltChannel(channel);
}

int JukeBox::StopAll()
{
  if (!m_config.music && !m_config.effects) return 0;

  // halt playback on all channels
  return Mix_HaltChannel(-1);
}

int JukeBox::PlaySample (Mix_Chunk * sample, int loop)
{
  if (loop != -1) loop--;

  int channel = Mix_PlayChannel(-1, sample, loop);

  if (channel == -1) {
	MSG_DEBUG("jukebox", "Error: Jukebox::PlaySample: %s", Mix_GetError());
  }
  return channel;
}

