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
 *  Constants.
 *****************************************************************************/

#include "constant.h"

const std::string Constants::VERSION = "0.7.3";

const std::string Constants::DEFAULT_DATADIR = INSTALL_DATADIR;
const std::string Constants::DEFAULT_LOCALEDIR = INSTALL_LOCALEDIR;
const std::string Constants::DEFAULT_FONT_PATH = FONT_FILE;
const std::string Constants::ENV_DATADIR = "WORMUX_DATADIR";
const std::string Constants::ENV_LOCALEDIR = "WORMUX_LOCALEDIR";
const std::string Constants::ENV_FONT_PATH = "WORMUX_FONT_PATH";

// Nombre de boucles maximum pour le test ...
const uint Constants::NBR_BCL_MAX_EST_VIDE = 200; // Un objet est dans le vide ?

std::vector<std::string> Constants::AUTHORS;

const std::string Constants::WEB_SITE = "http://www.wormux.org";
const std::string Constants::EMAIL = "wormux-dev@gna.org";

// Dimensions min/max du terrain (en pixel)
const Point2i Constants::MAP_MIN_SIZE = Point2i(100, 200);
const int Constants::MAP_MAX_SIZE = 4000*4000;

// Hauteur minimale libre pour que le terrain soit qualifié " d'ouvert "
const uint Constants::HAUT_MIN_TERRAIN_OUVERT = 40;

Constants * Constants::singleton = NULL;

Constants::Constants()
{
  AUTHORS.push_back ("Anthony CARRE");
  AUTHORS.push_back ("Jean-Christophe DUBERGA");
  AUTHORS.push_back ("Laurent DEFERT SIMONNEAU");
  AUTHORS.push_back ("Lawrence AZZOUG");
  AUTHORS.push_back ("Matthieu FERTRE");
  AUTHORS.push_back ("Olivier Boyer");
  AUTHORS.push_back ("Renaud LOTTIAUX");
  AUTHORS.push_back ("Victor STINNER");
}

Constants * Constants::GetInstance() {
  if (singleton == NULL) {
    singleton = new Constants();
  }
  return singleton;
}
