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
 *  Constants...
 *****************************************************************************/

#ifndef CONSTANT_H
#define CONSTANT_H
#include "base.h"
#include <string>
#include <vector>
#include "tool/point.h"

class Constants
{
public:
  // Version number of Wormux
  static const std::string VERSION;
  
  /*
   * Data path: 
   * INSTALL_DATADIR set by configure script to 
   * "/usr/share/wormux" by default
   * Override with --with-datadir-name
   */
  static const std::string DEFAULT_DATADIR;
  
  /*
   * Locales path: 
   * INSTALL_LOCALEDIR set by configure script to 
   * "/usr/share/locale" by default
   * Override with --with-localedir-name
   */
  static const std::string DEFAULT_LOCALEDIR;
  
  static const std::string DEFAULT_FONT_PATH;

  // Env variables name to override previous values
  static const std::string ENV_DATADIR;
  static const std::string ENV_LOCALEDIR;
  static const std::string ENV_FONT_PATH;
  
  // Nombre de boucles maximum pour le test ...
  static const uint NBR_BCL_MAX_EST_VIDE; // Un objet est dans le vide ?
  
  // Authors list
  static std::vector<std::string> AUTHORS;
  
  // Web site address and email
  static const std::string WEB_SITE;
  static const std::string EMAIL;
  
  // Dimensions min/max du terrain (en pixel)
  static const Point2i MAP_MIN_SIZE;
  static const int MAP_MAX_SIZE;
  
  // Hauteur (en pixel) minimale libre pour que le terrain
  // soit qualifié " d'ouvert "
  static const uint HAUT_MIN_TERRAIN_OUVERT;

  static Constants * GetInstance();
  
private:
  Constants();

  static Constants * singleton;
};

#endif
