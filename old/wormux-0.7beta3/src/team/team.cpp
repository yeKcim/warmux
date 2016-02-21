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
 * Une équipe de vers.
 *****************************************************************************/

#include "team.h"
//-----------------------------------------------------------------------------
#include "../game/game.h"
#include "../game/game_mode.h"
#include "../interface/cursor.h"
#include "../include/constant.h"
#include "../game/config.h"
#include "../map/camera.h"
#include "../weapon/weapons_list.h"
#include "../tool/i18n.h"
#include "../tool/file_tools.h"
#ifndef CL
#include "../tool/resource_manager.h"
#include "../graphic/sprite.h"
#endif
#include <sstream>
#include <iostream>
using namespace Wormux;
using namespace std;
//-----------------------------------------------------------------------------

const char* NOM_DEFAUT_EQUIPE = "Team X";

//-----------------------------------------------------------------------------

Team::Team()
{
  vers_fin = 0;
  vers_fin_it = vers.end();
  is_local = true;
  ver_actif = -1;
}

//-----------------------------------------------------------------------------

bool Team::Init (const std::string &teams_dir, const std::string &id)
{
  std::string nomfich;
  ver_actif = -1;
  try
  {
    LitDocXml doc;
    m_id = id;

    // Charge le XML
    nomfich = teams_dir+id+PATH_SEPARATOR+ "team.xml";
    if (!FichierExiste(nomfich)) return false;
    if (!doc.Charge (nomfich)) return false;

    Profile *res = resource_manager.LoadXMLProfile( nomfich, true);
     
    // Charge les données
    if (!ChargeDonnee (doc.racine(), res)) return false;
  }
  catch (const xmlpp::exception &e)
  {
    std::cout << std::endl
              << Format(_("Error loading team %s:"),	id.c_str())
              << std::endl << e.what() << std::endl;
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------

void Team::InitEnergy (uint max)
{
  energie.Init();
  energie.ChoisitNom(m_name);
  energie.FixeMax(max);
  energie.FixeValeur(LitEnergie());
}

//-----------------------------------------------------------------------------

uint Team::LitEnergie ()
{
  uint total_energie = 0;
  for (int index=0; index < vers_fin; ++index) {
    if( !vers[index].IsDead() )
      total_energie += vers[index].GetEnergy();
  }
  return total_energie;
}
//-----------------------------------------------------------------------------

void Team::ActualiseBarreEnergie ()
{
  energie.NouvelleValeur(LitEnergie());
}

//-----------------------------------------------------------------------------
#ifdef CL
bool Team::ChargeDonnee (xmlpp::Element *xml, 
			   CL_ResourceManager *res)
{
  xml = LitDocXml::AccesBalise (xml, "equipe");
  // Valeurs par défaut
  camera_est_sauve = false;
  active_weapon = weapons_list.GetWeapon(WEAPON_DYNAMITE);

  m_name = "Team unamed";
  m_sound_profile="default";
  crosshair.Init();

  // Lit le nom
  if (!LitDocXml::LitString(xml, "nom", m_name)) return false;

  // Ecusson
  CL_Surface ecusson_tmp("flag", res);
  ecusson = ecusson_tmp;

  // Recupère le nom du profile sonore
  LitDocXml::LitString(xml, "sound_profile", m_sound_profile);

  // Créer les vers
  xmlpp::Node::NodeList nodes = xml -> get_children("ver");
  xmlpp::Node::NodeList::iterator 
    it=nodes.begin(),
    fin=nodes.end();

  vers.clear();
  bool fin_bcl;
  do
  {
    xmlpp::Element *elem = dynamic_cast<xmlpp::Element*> (*it);
    Skin *skin;
    std::string character_name="Soldat Inconnu";
    std::string skin_name="ver_jaune";
    LitDocXml::LitAttrString(elem, "nom", character_name);
    LitDocXml::LitAttrString(elem, "avatar", skin_name);

    if (skins_list.find(skin_name) != skins_list.end()) {
      skin = &skins_list[skin_name];
    } else {
      cout << Format(_("Error: can't find the skin \"%s\" for the team \"%s\"."),
		     skin_name.c_str(),
		     m_name.c_str()) << endl;
      return false;
    }

    // Initialise les variables du ver, puis l'ajoute à la liste
    Character new_character;
    vers.push_back(new_character);
    vers.back().InitTeam (this, character_name, skin);

    // C'est la fin ?
    ++it;
    fin_bcl = (it == fin);
    fin_bcl |= (game_mode.max_characters <= vers.size());
  } while (!fin_bcl);

  ver_actif = 0;
  vers_fin = vers.size();
  vers_fin_it = vers.end();
  return (1 <= vers.size());
}

#else // CL defined

bool Team::ChargeDonnee( xmlpp::Element *xml, Profile *res_profile)
{
  xml = LitDocXml::AccesBalise (xml, "equipe");
  // Valeurs par défaut
  camera_est_sauve = false;
  active_weapon = weapons_list.GetWeapon(WEAPON_DYNAMITE);

  m_name = "Team unamed";
  m_sound_profile="default";
  crosshair.Init();

  // Lit le nom
  if (!LitDocXml::LitString(xml, "nom", m_name)) return false;

  // Ecusson
  ecusson = resource_manager.LoadImage( res_profile, "flag");
   
  // Recupère le nom du profile sonore
  LitDocXml::LitString(xml, "sound_profile", m_sound_profile);

  // Créer les vers
  xmlpp::Node::NodeList nodes = xml -> get_children("ver");
  xmlpp::Node::NodeList::iterator 
    it=nodes.begin(),
    fin=nodes.end();

  vers.clear();
  bool fin_bcl;
  do
  {
    xmlpp::Element *elem = dynamic_cast<xmlpp::Element*> (*it);
    Skin *skin;
    std::string character_name="Soldat Inconnu";
    std::string skin_name="ver_jaune";
    LitDocXml::LitAttrString(elem, "nom", character_name);
    LitDocXml::LitAttrString(elem, "avatar", skin_name);

    if (skins_list.find(skin_name) != skins_list.end()) {
      skin = &skins_list[skin_name];
    } else {
      cout << Format(_("Error: can't find the skin \"%s\" for the team \"%s\"."),
		     skin_name.c_str(),
		     m_name.c_str()) << endl;
      return false;
    }

    // Initialise les variables du ver, puis l'ajoute à la liste
    Character new_character;
    vers.push_back(new_character);
    vers.back().InitTeam (this, character_name, skin);

    // C'est la fin ?
    ++it;
    fin_bcl = (it == fin);
    fin_bcl |= (game_mode.max_characters <= vers.size());
  } while (!fin_bcl);

  ver_actif = 0;
  vers_fin = vers.size();
  vers_fin_it = vers.end();
  return (1 <= vers.size());
}


#endif // CL not defined


//-----------------------------------------------------------------------------

int Team::NextCharacterIndex()
{
  // Passe au ver suivant
  assert (0 < NbAliveCharacter());
  int copy = ver_actif;
  do
  { 
    ++copy;
    if (copy == vers_fin) copy = 0;
  } while (vers[copy].IsDead());
  return copy;
}

//-----------------------------------------------------------------------------

void Team::internal_NextCharacter()
{
  // Passe au ver suivant
  assert (0 < NbAliveCharacter());
  do
  { 
    ++ver_actif;
    if (ver_actif == vers_fin) ver_actif = 0;
  } while (ActiveCharacter().IsDead());
}

//-----------------------------------------------------------------------------
/*   not used anymore
void Team::NextCharacter()
{
  internal_NextCharacter();

  // Prépare le tour pour le nouveau ver
  camera.ChangeObjSuivi (&ActiveCharacter(), true, true);
  curseur_ver.SuitVerActif();
}
*/
//-----------------------------------------------------------------------------

int Team::NbAliveCharacter() const
{
  uint nbr = 0;
  for (int index=0; index < vers_fin; ++index)
    if (!vers[index].IsDead()) ++nbr;
  return nbr;
}

//-----------------------------------------------------------------------------

void Team::SelectCharacterIndex (uint index)
{
  // Ver mort ?
  assert (index < vers.size());
  if (vers.at(index).IsDead()) {
    int i = (++index)%vers.size();
    SelectCharacterIndex(i);
    return;
  }

  // Change de ver
  if(ver_actif != -1)
    vers.at(ver_actif).StopPlaying();
  ver_actif = index;
  vers.at(ver_actif).StartPlaying();
  camera.ChangeObjSuivi (&ActiveCharacter(), true, true);
  curseur_ver.SuitVerActif();
}

//-----------------------------------------------------------------------------

// Prepare le tour d'une equipe
void Team::PrepareTour()
{
  // Choisi un ver vivant si possible
  if (ActiveCharacter().IsDead())
  {
    camera_est_sauve = false;
    internal_NextCharacter();
  }

  if (camera_est_sauve) camera.SetXYabs (sauve_camera.x, sauve_camera.y);
  camera.ChangeObjSuivi (&ActiveCharacter(), 
			 !camera_est_sauve, !camera_est_sauve, 
			 true);
  curseur_ver.SuitVerActif();

  // Active notre arme
  AccessWeapon().Select();
}

//-----------------------------------------------------------------------------

// Fin d'un tour : nettoyage avant de partir :-)
void Team::FinTour()
{
  // Désactive notre arme
  ActiveCharacter().EndTurn();
  AccessWeapon().Deselect();
  camera_est_sauve = true;
#ifdef CL
  sauve_camera = CL_Point(camera.GetX(), camera.GetY());
#else
  sauve_camera = Point2i(camera.GetX(), camera.GetY());
#endif
}

//-----------------------------------------------------------------------------

int Team::ActiveCharacterIndex() const
{ 
  return ver_actif;
}

//-----------------------------------------------------------------------------

Character& Team::ActiveCharacter()
{ 
  assert ((uint)ver_actif < vers.size());
  return vers.at(ver_actif);
}

//-----------------------------------------------------------------------------

// Change d'arme
void Team::SetWeapon (Weapon_type type)
{
  AccessWeapon().Deselect();
  active_weapon = weapons_list.GetWeapon(type);
  AccessWeapon().Select();
}

//-----------------------------------------------------------------------------

int Team::ReadNbAmmos() const
{
  // Read in the Map
  // The same method as in AccesNbAmmos can't be use on const team !
  std::map<std::string, int>::const_iterator it 
    = m_nb_ammos.find( active_weapon->GetName() ) ;
  
  if (it != m_nb_ammos.end())  return ( it->second ) ;  
  return 0 ;
}

//-----------------------------------------------------------------------------

int Team::ReadNbUnits() const
{
  std::map<std::string, int>::const_iterator it 
    = m_nb_units.find( active_weapon->GetName() ) ;
  
  if (it !=  m_nb_units.end())  return ( it->second ) ;  
  return 0 ;
}

//-----------------------------------------------------------------------------

int Team::ReadNbAmmos(const std::string &weapon_name) const
{
  // Read in the Map
  // The same method as in AccesNbAmmos can't be use on const team !
  std::map<std::string, int>::const_iterator it = 
    m_nb_ammos.find( weapon_name );
  
  if (it !=  m_nb_ammos.end()) return ( it->second ) ;  
  return 0 ;
  
}

//-----------------------------------------------------------------------------

int Team::ReadNbUnits(const std::string &weapon_name) const
{
  std::map<std::string, int>::const_iterator it = 
    m_nb_units.find( weapon_name );
  
  if (it !=  m_nb_units.end()) return ( it->second ) ;  
  return 0 ;
  
}

//-----------------------------------------------------------------------------

int& Team::AccessNbAmmos()
{
  // if value not initialized, it initialize to 0 and then return 0
  return m_nb_ammos[ active_weapon->GetName() ] ;
}

//-----------------------------------------------------------------------------

int& Team::AccessNbUnits()
{
  // if value not initialized, it initialize to 0 and then return 0
  return m_nb_units[ active_weapon->GetName() ] ;
}

//-----------------------------------------------------------------------------

void Team::ResetNbUnits()
{
  m_nb_units[ active_weapon->GetName() ] = active_weapon->ReadInitialNbUnit();
}

//-----------------------------------------------------------------------------

Team::iterator Team::begin() { return vers.begin(); }
Team::iterator Team::end() { return vers_fin_it; }

//-----------------------------------------------------------------------------

void Team::Reset()
{
  // Reset des munitions
  m_nb_ammos.clear();
  m_nb_units.clear();
  std::list<Weapon *> l_weapons_list = weapons_list.GetList() ;
  std::list<Weapon *>::iterator itw = l_weapons_list.begin(), 
    end = l_weapons_list.end();

  for (; itw != end ; ++itw) {
    m_nb_ammos[ (*itw)->GetName() ] = (*itw)->ReadInitialNbAmmo();
    m_nb_units[ (*itw)->GetName() ] = (*itw)->ReadInitialNbUnit();
  }
  
  active_weapon = weapons_list.GetWeapon(WEAPON_DYNAMITE);
  camera_est_sauve = false;

  // Reset des vers
  ver_actif = 0;
  vers_fin_it = vers.end();
  iterator it=vers.begin(), fin=vers.end();
  uint n = 1;
  for (; it != fin; ++it, ++n) 
  {
    vers_fin_it = it;
    ++vers_fin_it;
    it -> Reset();
    if (n == game_mode.max_characters) break;
  }
  vers_fin = n;
  crosshair.Reset();
}

//-----------------------------------------------------------------------------

void Team::Draw()
{
  energie.Draw ();
}

//-----------------------------------------------------------------------------

void Team::Refresh()
{
  energie.Refresh();
}

//-----------------------------------------------------------------------------

Weapon& Team::AccessWeapon() const { return *active_weapon; }
const Weapon& Team::GetWeapon() const { return *active_weapon; }
Weapon_type Team::GetWeaponType() const { return GetWeapon().GetType(); }

//-----------------------------------------------------------------------------
