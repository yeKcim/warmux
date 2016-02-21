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
 * Configuration de Wormux : toutes les variables qui sont intéressantes à
 * modifier se retrouvent ici. Les variables ont une valeur par défaut qui
 * peut être modifiée avec le fichier de configuration.
 *****************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H
//-----------------------------------------------------------------------------
#include "../include/base.h"
#include "../tool/xml_document.h"
#include <list>
#include <string>
//-----------------------------------------------------------------------------
#if defined(WIN32) || defined(__MINGW32__)
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

#ifdef __MINGW32__
#undef LoadImage
#endif
//-----------------------------------------------------------------------------
namespace Wormux {
//-----------------------------------------------------------------------------

class Config
{
public:
  // Divers
  bool exterieur_monde_vide;
  bool display_energy_character;
  bool display_name_character;
  bool display_wind_particles;
  std::string ttf_filename;

  // Tempory values (loaded from XML, but may change during running)
  struct tmp_xml_config{
    struct tmp_xml_screen{
      int width,height;
      bool fullscreen;
    } video;
    struct tmp_xml_sound{
      bool music;
      bool effects;
      uint frequency;
    } sound;
    std::list<std::string> teams;
    std::string map_name;
  } tmp;
   enum Transparency 
     {
	ALPHA,
	COLORKEY,
     };
   Transparency transparency;
  
  std::string data_dir, locale_dir, personal_dir;

public:
  Config();
  bool Charge();
  void Applique();
  bool Sauve();

  // Return ~/.wormux/
  std::string GetWormuxPersonalDir() const;

protected:
  std::string m_game_mode;
  bool ChargeVraiment();
  bool ChargeXml (xmlpp::Element *xml);
  void SetKeyboardConfig();
  bool SauveXml();
  bool m_xml_charge;
  std::string m_nomfich;
};

//-----------------------------------------------------------------------------
extern Config config;
//-----------------------------------------------------------------------------

}
#endif
