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
 * Liste des terrains.
 *****************************************************************************/

#ifndef LST_TERRAINS_H
#define LST_TERRAINS_H
#include <SDL.h>
#include <string>
#include <map>
#include "../graphic/surface.h"
#include "../include/base.h"
#include "../tool/resource_manager.h"
#include "../tool/xml_document.h"

class InfoTerrain{
public:
  std::string name;
  uint nb_mine;
  bool use_water;
  bool is_opened;
  bool infinite_bg;
  Surface preview;
  Surface img_terrain, img_ciel;
  Profile *res_profile;
  std::string author_info;
  std::string m_directory;
  struct s_wind
  {
    uint nb_sprite;
    uint default_nb_sprite;
    double particle_wind_factor;
    double particle_mass; //Mean of the mass(read in the XML file)
    double particle_air_resist_factor;
    bool need_flip; //do we need to flip the sprite when it changes direction?
  } wind;

private:
  bool m_donnees_chargees;
  void LoadData();
  bool TraiteXml (xmlpp::Element *xml);

public:
  InfoTerrain ();
  Surface LitImgTerrain();
  Surface LitImgCiel();
  bool Init(const std::string &nom, const std::string &repertoire);
  bool DonneesChargees() const;
  void FreeData();
};


class ListeTerrain
{
public:
  std::vector<InfoTerrain> liste;
  typedef std::vector<InfoTerrain>::iterator iterator;

private:
  int terrain_actif;
  bool m_init;
  void LoadOneMap (const std::string &dir, const std::string &file);

public:
  ListeTerrain();
  void Init();

  // Return -1 if fails
  int FindMapById (const std::string &id);
  void ChangeTerrainNom (const std::string &nom);
  void ChangeTerrain (uint index);
  InfoTerrain& TerrainActif();
};

InfoTerrain& TerrainActif();
extern ListeTerrain lst_terrain;

bool compareMaps(const InfoTerrain& a, const InfoTerrain& b) ;

#endif
