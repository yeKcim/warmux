/******************************************************************************
 *  Wormux is a convivial mass murder game.
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
 * A team
 *****************************************************************************/

#include "team.h"
#include "../character/body_list.h"
#include "../game/game.h"
#include "../game/game_mode.h"
#include "../game/game_loop.h"
#include "../interface/cursor.h"
#include "../include/constant.h"
#include "../game/config.h"
#include "../map/camera.h"
#include "../map/map.h"
#include "../weapon/weapons_list.h"
#include "../tool/debug.h"
#include "../tool/i18n.h"
#include "../tool/file_tools.h"
#include "../tool/resource_manager.h"
#include "../graphic/sprite.h"
#include "../network/network.h"
#include <sstream>
#include <iostream>

Team::Team(const std::string& _teams_dir,
	   const std::string& _id,
	   const std::string& _name,
	   const Surface& _flag,
	   const std::string& _sound_profile) :
  energy(_name)
{
  is_local = true;
  active_character = characters.end();

  is_camera_saved = false;
  active_weapon = weapons_list.GetWeapon(WEAPON_DYNAMITE);

  m_teams_dir = _teams_dir;
  m_id = _id;
  m_name = _name;
  m_sound_profile = _sound_profile;
  m_player_name = "";

  nb_characters = GameMode::GetInstance()->max_characters;
  
  flag = _flag;
}

Team * Team::CreateTeam (const std::string& teams_dir,
			 const std::string& id)
{
  std::string nomfich;
  try
  {
    LitDocXml doc;

    // Load XML
    nomfich = teams_dir+id+PATH_SEPARATOR+ "team.xml";
    if (!IsFileExist(nomfich)) return false;
    if (!doc.Charge (nomfich)) return false;

    // Load name
    std::string name;
    if (!LitDocXml::LitString(doc.racine(), "name", name)) return NULL;

    // Load flag
    Profile *res = resource_manager.LoadXMLProfile( nomfich, true);
    Surface flag = resource_manager.LoadImage(res, "flag");
    resource_manager.UnLoadXMLProfile(res);

    // Get sound profile
    std::string sound_profile;
    if (!LitDocXml::LitString(doc.racine(), "sound_profile", sound_profile))
      sound_profile = "default";

    return new Team(teams_dir, id, name, flag, sound_profile) ;
  }
  catch (const xmlpp::exception &e)
  {
    std::cerr << std::endl
              << Format(_("Error loading team %s:"), id.c_str())
              << std::endl << e.what() << std::endl;
    return NULL;
  }

  return NULL;
}


bool Team::LoadCharacters(uint howmany)
{
  //assert(howmany <= GameMode::GetInstance()->max_characters);
  assert (howmany <= 10);

  std::string nomfich;
  try
  {
    LitDocXml doc;

    // Load XML
    nomfich = m_teams_dir+m_id+PATH_SEPARATOR+ "team.xml";
    if (!IsFileExist(nomfich)) return false;
    if (!doc.Charge (nomfich)) return false;

    // Create the characters
    xmlpp::Element *xml = LitDocXml::AccesBalise (doc.racine(), "team");

    xmlpp::Node::NodeList nodes = xml -> get_children("character");
    xmlpp::Node::NodeList::iterator
      it=nodes.begin(),
      fin=nodes.end();

    characters.clear();
    bool fin_bcl;
    active_character = characters.end();
    do
      {
	xmlpp::Element *elem = dynamic_cast<xmlpp::Element*> (*it);
	Body* body;
	std::string character_name="Soldat Inconnu";
	std::string body_name="";
	LitDocXml::LitAttrString(elem, "name", character_name);
	LitDocXml::LitAttrString(elem, "body", body_name);

	if (!(body = body_list.GetBody(body_name)) )
	{
	  std::cerr
	    << Format(_("Error: can't find the body \"%s\" for the team \"%s\"."),
		      body_name.c_str(),
		      m_name.c_str())
	    << std::endl;
	  return false;
	}

	// Initialise les variables du ver, puis l'ajoute �la liste
	Character new_character(*this, character_name);
	characters.push_back(new_character);
	active_character = characters.begin(); // we need active_character to be initialized here !!
	characters.back().SetBody(body);
	if (!characters.back().PutRandomly(false, world.dst_min_entre_vers))
	{
	  // We haven't found any place to put the characters!!
	  if (!characters.back().PutRandomly(false, world.dst_min_entre_vers/2)) {
	    std::cerr << std::endl;
	    std::cerr << "Error: " << character_name.c_str() << " will be probably misplaced!" << std::endl;
	    std::cerr << std::endl;
	    
	    // Put it with no space...
	    characters.back().PutRandomly(false, 0);
	  }
	}
        characters.back().Init();

	MSG_DEBUG("team", "Add %s in team %s", character_name.c_str(), m_name.c_str());

	// C'est la fin ?
	++it;
	fin_bcl = (it == fin);
	fin_bcl |= (howmany <= characters.size());
      } while (!fin_bcl);

    active_character = characters.begin();

  }
  catch (const xmlpp::exception &e)
    {
      std::cerr << std::endl
		<< Format(_("Error loading team's data %s:"), m_id.c_str())
		<< std::endl << e.what() << std::endl;
      return false;
    }

  return (characters.size() == howmany);
}

void Team::InitEnergy (uint max)
{
  energy.Config(ReadEnergy(), max);
}

uint Team::ReadEnergy ()
{
  uint total_energy = 0;

  iterator it = characters.begin(), end = characters.end();

  for (; it != end; ++it) {
    if ( !(*it).IsDead() )
      total_energy += (*it).GetEnergy();
  }

  return total_energy;
}

void Team::UpdateEnergyBar ()
{
  energy.SetValue(ReadEnergy());
}

void Team::NextCharacter()
{
  assert (0 < NbAliveCharacter());
  ActiveCharacter().StopPlaying();
  do
  {
    ++active_character;
    if (active_character == characters.end())
      active_character = characters.begin();
  } while (ActiveCharacter().IsDead());
  ActiveCharacter().StartPlaying();

  if (is_camera_saved) camera.SetXYabs (sauve_camera.x, sauve_camera.y);
  camera.ChangeObjSuivi (&ActiveCharacter(),
			 !is_camera_saved, !is_camera_saved,
			 true);
  MSG_DEBUG("team", "%s (%d, %d)is now the active character",
	    ActiveCharacter().GetName().c_str(),
	    ActiveCharacter().GetX(),
	    ActiveCharacter().GetY());
}

int Team::NbAliveCharacter() const
{
  uint nbr = 0;
  const_iterator it= characters.begin(), end=characters.end();

  for (; it!=end; ++it)
    if ( !(*it).IsDead() ) nbr++;

  return nbr;
}

// Prepare a new team turn
void Team::PrepareTurn()
{
  // Get a living character if possible
  if (ActiveCharacter().IsDead())
  {
    is_camera_saved = false;
    NextCharacter();
  }

  if (is_camera_saved) camera.SetXYabs (sauve_camera.x, sauve_camera.y);
  camera.ChangeObjSuivi (&ActiveCharacter(),
			 !is_camera_saved, !is_camera_saved,
			 true);
  CharacterCursor::GetInstance()->FollowActiveCharacter();

  // Active last weapon use if EnoughAmmo
  if (AccessWeapon().EnoughAmmo())
    AccessWeapon().Select();
  else { // try to find another weapon !!
    active_weapon = weapons_list.GetWeapon(WEAPON_BAZOOKA);
    AccessWeapon().Select();
  }
}

Character& Team::ActiveCharacter()
{
  return (*active_character);
}

void Team::SetWeapon (Weapon_type type)
{
  AccessWeapon().Deselect();
  active_weapon = weapons_list.GetWeapon(type);
  AccessWeapon().Select();
}

int Team::ReadNbAmmos() const
{
  return ReadNbAmmos(active_weapon->GetName());
}

int Team::ReadNbUnits() const
{
  return ReadNbUnits( active_weapon->GetName());
}

int Team::ReadNbAmmos(const std::string &weapon_name) const
{
  // Read in the Map
  // The same method as in AccesNbAmmos can't be use on const team !
  std::map<std::string, int>::const_iterator it =
      m_nb_ammos.find( weapon_name );

  if (it !=  m_nb_ammos.end()) return ( it->second ) ;
  // We should not be here !
  MSG_DEBUG("team", "%s : not found into the ammo map.", weapon_name.c_str());
  assert(false);
  return 0 ;
}

int Team::ReadNbUnits(const std::string &weapon_name) const
{
  std::map<std::string, int>::const_iterator it =
      m_nb_units.find( weapon_name );

  if (it !=  m_nb_units.end()) return ( it->second ) ;
  // We should not be here !
  MSG_DEBUG("team", "%s : not found into the ammo map.", weapon_name.c_str());
  assert(false);
  return 0 ;
}

int& Team::AccessNbAmmos()
{
  // if value not initialized, it initialize to 0 and then return 0
  return m_nb_ammos[ active_weapon->GetName() ] ;
}

int& Team::AccessNbUnits()
{
  // if value not initialized, it initialize to 0 and then return 0
  return m_nb_units[ active_weapon->GetName() ] ;
}

void Team::ResetNbUnits()
{
  m_nb_units[ active_weapon->GetName() ] = active_weapon->ReadInitialNbUnit();
}

Team::iterator Team::begin() { return characters.begin(); }
Team::iterator Team::end() { return characters.end(); }

Character* Team::FindByIndex(uint index)
{
  assert(index < characters.size());
  iterator it= characters.begin(), end=characters.end();

  while(index != 0 && it != characters.end())
  {
    index--;
    it++;
  }
  return &(*it);
}

void Team::LoadGamingData(uint howmany)
{
  // Reset ammos
  m_nb_ammos.clear();
  m_nb_units.clear();
  std::list<Weapon *> l_weapons_list = weapons_list.GetList() ;
  std::list<Weapon *>::iterator itw = l_weapons_list.begin(),
    end = l_weapons_list.end();

  for (; itw != end ; ++itw) {
    m_nb_ammos[ (*itw)->GetName() ] = (*itw)->ReadInitialNbAmmo();
    m_nb_units[ (*itw)->GetName() ] = (*itw)->ReadInitialNbUnit();
  }

  // Disable non-working weapons in network games
  if(network.IsConnected())
  {
    m_nb_ammos[ weapons_list.GetWeapon(WEAPON_NINJA_ROPE)->GetName() ] = 0;
    m_nb_ammos[ weapons_list.GetWeapon(WEAPON_AIR_HAMMER)->GetName() ] = 0;
    m_nb_ammos[ weapons_list.GetWeapon(WEAPON_BLOWTORCH)->GetName() ] = 0;
    m_nb_ammos[ weapons_list.GetWeapon(WEAPON_SUBMACHINE_GUN)->GetName() ] = 0;
  }    

  active_weapon = weapons_list.GetWeapon(WEAPON_DYNAMITE);
  is_camera_saved = false;

  if (howmany == 0)
    LoadCharacters(nb_characters);
  else
    LoadCharacters(howmany);
}

void Team::UnloadGamingData()
{
  // Clear list of characters
  characters.clear();
}

void Team::SetNbCharacters(uint howmany)
{
  assert(howmany >= 2 && howmany <= 10);
  nb_characters = howmany;
}

void Team::SetPlayerName(const std::string& _player_name)
{
  m_player_name = _player_name;
  energy.SetTeamName(m_player_name+" - "+m_name);
}

void Team::DrawEnergy()
{
  energy.Draw ();
}

void Team::Refresh()
{
  energy.Refresh();
}

Weapon& Team::AccessWeapon() const { return *active_weapon; }
const Weapon& Team::GetWeapon() const { return *active_weapon; }
Weapon_type Team::GetWeaponType() const { return GetWeapon().GetType(); }

bool Team::IsSameAs(const Team& other) const
{
  return (strcmp(m_id.c_str(), other.GetId().c_str()) == 0);
}
