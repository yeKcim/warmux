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
 * Refresh des fichiers.
 *****************************************************************************/

#include "../tool/file_tools.h"
//-----------------------------------------------------------------------------
#include <fstream>
#include <stdlib.h> // getenv
#include "i18n.h"
using namespace std;
//-----------------------------------------------------------------------------

bool FichierExiste(const std::string &nom)
{
  std::ifstream f(nom.c_str());
  bool existe = f;
  f.close();
  return existe;
}

//-----------------------------------------------------------------------------

// Extension d'un nom de fichier
std::string ExtensionFichier (const std::string &nom)
{
  int pos = nom.rfind('.');
  if (pos != -1)
    return nom.substr(pos+1);
  else
    return "";
}

//-----------------------------------------------------------------------------

// Lit le répertoire $HOME de l'utilisateur
#ifndef WIN32
std::string RepertoireHome ()
{
  char *txt = getenv("HOME");
  if (txt == NULL) 
  {
    Error (_("HOME directory (environment variable $HOME) could not be found!"));
  }
  return txt;
}
#else
std::string RepertoireHome () {return "";}
#endif
//-----------------------------------------------------------------------------

// Traduit un répertoire en nom de fichier classique
// Exemple : ~ est remplacé pa $HOME
std::string TraduitRepertoire (const std::string &repertoire)
{
  std::string home = RepertoireHome();
  std::string txt = repertoire;
  for (int pos = txt.length()-1;
       (pos = txt.rfind ('~', pos)) != -1;
       --pos)
  {
    txt.replace(pos,1,home);
  }
  return txt;
}

//-----------------------------------------------------------------------------
