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
 * Skin : gestion des différents aspects pour les vers. Spécifications :
 * - image (animation),
 * - position du viseur pour les différentes armes,
 * - taille,
 * - ...
 *
 * Chaque avatar différent a une seule instance. Un ver possède seulement un
 * pointeur vers une de ces instances.
 *****************************************************************************/

#ifndef SKIN_H
#define SKIN_H
//-----------------------------------------------------------------------------
#include <SDL.h>
#include "../graphic/sprite.h"
#include "../include/base.h"
#include "../tool/xml_document.h"
#include "../tool/resource_manager.h"
#include <map>
#include <string>
//-----------------------------------------------------------------------------

typedef struct s_skin_translate_t{ int dx,dy; } skin_translate_t;

class CfgSkin
{
public:
  Sprite *image;
  uint test_dx, test_top, test_bottom;
public:
  CfgSkin();
  ~CfgSkin();
  CfgSkin(const CfgSkin &a_CfgSkin);
  void Reset();
};

typedef std::map<std::string,CfgSkin>::value_type paire_skin;

//-----------------------------------------------------------------------------

class CfgSkin_Walking : public CfgSkin
{
public:
  Sprite *image_helmet;
  uint repetition_frame;
  bool full_walk;
  std::vector<skin_translate_t> hand_position;
public:
  CfgSkin_Walking();
  void Reset();
};

typedef std::map<std::string,CfgSkin_Walking>::value_type paire_walking_skin;

//-----------------------------------------------------------------------------

class CfgSkin_Anim
{
public:
  bool utilise;
  bool not_while_playing;
  Sprite *image;
  skin_translate_t pos;
  uint vitesse;

public:
  CfgSkin_Anim();
  void Reset();
};

//-----------------------------------------------------------------------------

class Skin
{
public:
  // Représentation du ver
//  CfgSkin_Walking walking;
//  CfgSkin_Dead dead;
//  CfgSkin_Dead drowned;

  // Animation (yeux qui clignotent ?)
  CfgSkin_Anim anim;

  std::map<std::string, CfgSkin> many_skins;
  std::map<std::string, CfgSkin_Walking> many_walking_skins;

public:
  Skin();
  void Reset();
  bool Charge (const std::string &nom, const std::string &repertoire);

protected:
  bool GetXmlConfig (xmlpp::Element *xml, CfgSkin_Walking &config);
  void Xml_LitRectTest (xmlpp::Element *elem, CfgSkin &config);
  void Xml_ReadHandPosition(xmlpp::Element *elem, CfgSkin_Walking &config);
  void LoadManySkins(xmlpp::Element *elem, Profile *res);
};

//-----------------------------------------------------------------------------

// Liste des avatars
extern std::map<std::string, Skin> skins_list;

// Initialisation des avatars
void InitSkins();

//-----------------------------------------------------------------------------
#endif
