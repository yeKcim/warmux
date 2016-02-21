/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2007 Wormux Team.
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
#include "teams_list.h"
#include "character/character.h"
#include "character/body_list.h"
#include "game/config.h"
#include "game/game.h"
#include "game/game_mode.h"
#include "game/game_loop.h"
#include "graphic/sprite.h"
#include "interface/cursor.h"
#include "include/base.h"
#include "include/constant.h"
#include "map/camera.h"
#include "map/map.h"
#include "network/network.h"
#include "tool/debug.h"
#include "tool/i18n.h"
#include "tool/file_tools.h"
#include "tool/point.h"
#include "tool/resource_manager.h"
#include "tool/xml_document.h"
#include "weapon/weapons_list.h"
#include <sstream>
#include <iostream>


Team::Team (const std::string& teams_dir, const std::string& id)
  : energy(this), m_teams_dir(teams_dir), m_id(id)
{
  std::string nomfich;
  XmlReader   doc;

  // Load XML
  nomfich = teams_dir+id+PATH_SEPARATOR+ "team.xml";

  if (!doc.Load(nomfich))
    throw "unable to load file of team data";

  if (!XmlReader::ReadString(doc.GetRoot(), "name", m_name))
    throw "Invalid file structure: cannot find a name for team ";

  // Load flag
  Profile *res = resource_manager.LoadXMLProfile( nomfich, true);
  flag = resource_manager.LoadImage(res, "flag");
  death_flag = resource_manager.LoadImage(res, "death_flag");
  big_flag = resource_manager.LoadImage(res, "big_flag");
  resource_manager.UnLoadXMLProfile(res);

  // Get sound profile
  if (!XmlReader::ReadString(doc.GetRoot(), "sound_profile", m_sound_profile))
    m_sound_profile = "default";

  active_character = characters.end();

  is_camera_saved = false;
  active_weapon = NULL;

  m_player_name = "";

  nb_characters = GameMode::GetInstance()->nb_characters;

  type_of_player = TEAM_human_local;
}

bool Team::LoadCharacters()
{
  ASSERT (nb_characters <= 10);

  std::string nomfich;
  try
  {
    XmlReader doc;

    // Load XML
    nomfich = m_teams_dir+m_id+PATH_SEPARATOR+ "team.xml";
    if (!IsFileExist(nomfich)) return false;
    if (!doc.Load(nomfich)) return false;

    // Create the characters
    xmlpp::Element *xml = XmlReader::GetMarker(doc.GetRoot(), "team");

    xmlpp::Node::NodeList nodes = xml->get_children("character");
    xmlpp::Node::NodeList::iterator it=nodes.begin();

    characters.clear();
    active_character = characters.end();
    do
    {
      xmlpp::Element *elem = dynamic_cast<xmlpp::Element*> (*it);
      Body* body;
      std::string character_name = "Unknown Soldier (it's all over)";
      std::string body_name = "";
      XmlReader::ReadStringAttr(elem, "name", character_name);
      XmlReader::ReadStringAttr(elem, "body", body_name);

      if (!(body = body_list.GetBody(body_name)) )
      {
        std::cerr
            << Format(_("Error: can't find the body \"%s\" for the team \"%s\"."),
                      body_name.c_str(),
                      m_name.c_str())
            << std::endl;
        return false;
      }

      // Create a new character and add him to the team
      Character new_character(*this, character_name, body);
      characters.push_back(new_character);
      active_character = characters.begin(); // we need active_character to be initialized here !!
      if (!characters.back().PutRandomly(false, world.GetDistanceBetweenCharacters()))
      {
        // We haven't found any place to put the characters!!
        if (!characters.back().PutRandomly(false, world.GetDistanceBetweenCharacters() / 2)) {
          std::cerr << std::endl;
          std::cerr << "Error: player " << character_name.c_str() << " will be probably misplaced!" << std::endl;
          std::cerr << std::endl;

            // Put it with no space...
          characters.back().PutRandomly(false, 0);
        }
      }
      characters.back().Init();

      MSG_DEBUG("team", "Add %s in team %s", character_name.c_str(), m_name.c_str());

        // C'est la fin ?
      ++it;
    } while (it!=nodes.end() && characters.size() < nb_characters );

    active_character = characters.begin();

  }
  catch (const xmlpp::exception &e)
  {
    std::cerr << std::endl
        << Format(_("Error loading team's data %s:"), m_id.c_str())
        << std::endl << e.what() << std::endl;
    return false;
  }

  return (characters.size() == nb_characters);
}

void Team::InitEnergy (uint max)
{
  energy.Config(ReadEnergy(), max);
}

uint Team::ReadEnergy () const
{
  uint total_energy = 0;

  const_iterator it = characters.begin(), end = characters.end();

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

TeamEnergy & Team::GetEnergyBar()
{
  return energy;
}

void Team::SelectCharacter(uint index)
{
  ASSERT(index <= characters.size());
  ActiveCharacter().StopPlaying();
  active_character = characters.begin();
  for(uint i = 0; i < index; ++i)
    ++active_character;
}

void Team::NextCharacter()
{
  ASSERT (0 < NbAliveCharacter());
  ActiveCharacter().StopPlaying();
  do
  {
    ++active_character;
    if (active_character == characters.end())
      active_character = characters.begin();
  } while (ActiveCharacter().IsDead());
  ActiveCharacter().StartPlaying();

  if (is_camera_saved) Camera::GetInstance()->GetInstance()->SetXYabs (sauve_camera.x, sauve_camera.y);
  Camera::GetInstance()->GetInstance()->FollowObject (&ActiveCharacter(),
                          !is_camera_saved, !is_camera_saved,
                          true);
  MSG_DEBUG("team", "%s (%d, %d)is now the active character",
            ActiveCharacter().GetName().c_str(),
            ActiveCharacter().GetX(),
            ActiveCharacter().GetY());
}

void Team::PreviousCharacter()
{
  ASSERT (0 < NbAliveCharacter());
  ActiveCharacter().StopPlaying();
  do
  {
    if (active_character == characters.begin())
      active_character = characters.end();
    --active_character;
  } while (ActiveCharacter().IsDead());
  ActiveCharacter().StartPlaying();

  if (is_camera_saved) Camera::GetInstance()->GetInstance()->SetXYabs (sauve_camera.x, sauve_camera.y);
  Camera::GetInstance()->GetInstance()->FollowObject (&ActiveCharacter(),
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

  if (is_camera_saved) Camera::GetInstance()->GetInstance()->SetXYabs (sauve_camera.x, sauve_camera.y);
  Camera::GetInstance()->GetInstance()->FollowObject (&ActiveCharacter(),
                          !is_camera_saved, !is_camera_saved,
                          true);
  CharacterCursor::GetInstance()->FollowActiveCharacter();

  // Active last weapon use if EnoughAmmo
  if (AccessWeapon().EnoughAmmo())
    AccessWeapon().Select();
  else { // try to find another weapon !!
    active_weapon = WeaponsList::GetInstance()->GetWeapon(Weapon::WEAPON_BAZOOKA);
    AccessWeapon().Select();
  }
}

Character& Team::ActiveCharacter() const
{
  return (*active_character);
}

void Team::SetWeapon (Weapon::Weapon_type type)
{

  ASSERT (type >= Weapon::WEAPON_FIRST && type <= Weapon::WEAPON_LAST);
  AccessWeapon().Deselect();
  active_weapon = WeaponsList::GetInstance()->GetWeapon(type);
  AccessWeapon().Select();
}

int Team::ReadNbAmmos() const
{
  return ReadNbAmmos(active_weapon->GetType());
}

int Team::ReadNbUnits() const
{
  return ReadNbUnits( active_weapon->GetType());
}

int Team::ReadNbAmmos(const Weapon::Weapon_type &weapon_type) const
{
  ASSERT((unsigned int)weapon_type < m_nb_ammos.size());
  return m_nb_ammos[weapon_type];
}

int Team::ReadNbUnits(const Weapon::Weapon_type &weapon_type) const
{
  ASSERT((unsigned int)weapon_type < m_nb_units.size());
  return m_nb_units[weapon_type];
}

int& Team::AccessNbAmmos()
{
  // if value not initialized, it initialize to 0 and then return 0
  return m_nb_ammos[ active_weapon->GetType() ] ;
}

int& Team::AccessNbUnits()
{
  // if value not initialized, it initialize to 0 and then return 0
  return m_nb_units[ active_weapon->GetType() ] ;
}

void Team::ResetNbUnits()
{
  m_nb_units[ active_weapon->GetType() ] = active_weapon->ReadInitialNbUnit();
}

Team::iterator Team::begin() { return characters.begin(); }
Team::iterator Team::end() { return characters.end(); }

Character* Team::FindByIndex(uint index)
{
  ASSERT(index < characters.size());
  iterator it= characters.begin(), end=characters.end();

  while(index != 0 && it != end)
  {
    index--;
    it++;
  }
  return &(*it);
}

void Team::LoadGamingData()
{
  // Reset ammos
  m_nb_ammos.clear();
  m_nb_units.clear();
  std::list<Weapon *> l_weapons_list = WeaponsList::GetInstance()->GetList() ;
  std::list<Weapon *>::iterator itw = l_weapons_list.begin(),
  end = l_weapons_list.end();

  m_nb_ammos.assign(l_weapons_list.size(), 0);
  m_nb_units.assign(l_weapons_list.size(), 0);

  for (; itw != end ; ++itw) {
    m_nb_ammos[ (*itw)->GetType() ] = (*itw)->ReadInitialNbAmmo();
    m_nb_units[ (*itw)->GetType() ] = (*itw)->ReadInitialNbUnit();
  }

  // Disable non-working weapons in network games
  if(Network::GetInstance()->IsConnected())
  {
    //m_nb_ammos[ Weapon::WEAPON_GRAPPLE ] = 0;
  }

  active_weapon = WeaponsList::GetInstance()->GetWeapon(Weapon::WEAPON_DYNAMITE);
  is_camera_saved = false;

  LoadCharacters();
}

void Team::UnloadGamingData()
{
  // Clear list of characters
  characters.clear();
}

void Team::SetNbCharacters(uint howmany)
{
  ASSERT(howmany >= 1 && howmany <= 10);
  nb_characters = howmany;
}

void Team::SetPlayerName(const std::string& _player_name)
{
  m_player_name = _player_name;
  // energy.SetTeamName(m_player_name+" - "+m_name);
}

void Team::DrawEnergy(const Point2i& pos)
{
  energy.Draw(pos);
}

void Team::Refresh()
{
  energy.Refresh();
}

Weapon& Team::AccessWeapon() const { return *active_weapon; }
Weapon& Team::GetWeapon() const { return *active_weapon; }
Weapon::Weapon_type Team::GetWeaponType() const { return GetWeapon().GetType(); }

bool Team::IsSameAs(const Team& other) const
{
  return (strcmp(m_id.c_str(), other.GetId().c_str()) == 0);
}

bool Team::IsActiveTeam() const
{
  return this == &ActiveTeam();
}

bool Team::IsLocal() const
{
  if (type_of_player == TEAM_human_local)
    return true;

  return false;
}

bool Team::IsLocalAI() const
{
  if (type_of_player == TEAM_ai_local)
    return true;

  return false;
}

void Team::SetLocal()
{
  type_of_player = TEAM_human_local;
}

void Team::SetLocalAI()
{
  type_of_player = TEAM_ai_local;
}

void Team::SetRemote()
{
  type_of_player = TEAM_remote;
}
