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
 * Game menu
 *****************************************************************************/

#include "game_menu.h"
//-----------------------------------------------------------------------------

#include "../include/app.h"
#include "../graphic/video.h"
#include "../team/teams_list.h"
#include "../game/game.h"
#include "../game/game_mode.h"
#include "../map/maps_list.h"
#include "../game/config.h"
#include "../tool/i18n.h"
#include "../tool/string_tools.h"
#include "../include/global.h"

using namespace Wormux;
using namespace std;

//-----------------------------------------------------------------------------

const uint TEAMS_X = 20;
const uint TEAMS_Y = 20;
const uint TEAMS_W = 160;
const uint TEAMS_H = 160;
const uint TEAM_LOGO_Y = 290;
const uint TEAM_LOGO_H = 48;

const uint MAPS_X = 20;
const uint MAPS_Y = TEAMS_Y+TEAMS_H+40;
const uint MAPS_W = 160;
 
const uint MAP_PREVIEW_W = 300;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

GameMenu::GameMenu() : Menu("menu/bg_option")
{  
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml",false);

  //-----------------------------------------------------------------------------
  // Widget creation
  //-----------------------------------------------------------------------------

  /* Choose the teams !! */
  team_box = new VBox(TEAMS_X, TEAMS_Y, 475);
  team_box->AddWidget(new Label(_("Select the teams:"), 0, 0, 0, global().normal_font()));

  Box * tmp_box = new HBox(0,0, TEAMS_H, false);
  tmp_box->SetMargin(10);
  tmp_box->SetBorder(0,0);

  lbox_all_teams = new ListBox(0, 0, TEAMS_W, TEAMS_H - TEAM_LOGO_H - 5);
  lbox_all_teams->always_one_selected = false;
  tmp_box->AddWidget(lbox_all_teams);

  Box * buttons_tmp_box = new VBox(0,0, 68, false);

  bt_add_team = new Button(0,0, 48,48,res,"menu/arrow-right");
  buttons_tmp_box->AddWidget(bt_add_team);
  
  bt_remove_team = new Button(0,0,48,48,res,"menu/arrow-left");
  buttons_tmp_box->AddWidget(bt_remove_team);

  space_for_logo = new NullWidget(0,0,48,48);
  buttons_tmp_box->AddWidget(space_for_logo);

  tmp_box->AddWidget(buttons_tmp_box);
  
  lbox_selected_teams = new ListBox(0, 0, TEAMS_W, TEAMS_H - TEAM_LOGO_H - 5); 
  lbox_selected_teams->always_one_selected = false;
  tmp_box->AddWidget(lbox_selected_teams);

  team_box->AddWidget(tmp_box);

  /* Choose the map !! */
  tmp_box = new HBox(0, 0, MAP_PREVIEW_W-25, false);
  tmp_box->SetMargin(0);
  tmp_box->SetBorder(0,0);

  lbox_maps = new ListBox(0, 0, MAPS_W, MAP_PREVIEW_W-25);
  tmp_box->AddWidget(lbox_maps);
  tmp_box->AddWidget(new NullWidget(0, 0, MAP_PREVIEW_W+5, MAP_PREVIEW_W));
  
  map_box = new VBox(MAPS_X, MAPS_Y, 475); //tmp_box->GetW()+10);
  map_box->AddWidget(new Label(_("Select the world:"), 0, 0, 0, global().normal_font()));
  map_box->AddWidget(tmp_box);


  //-----------------------------------------------------------------------------
  // Values initialization
  //-----------------------------------------------------------------------------

  // Load Maps' list
  std::sort(lst_terrain.liste.begin(), lst_terrain.liste.end(), compareMaps);

  ListeTerrain::iterator
    terrain=lst_terrain.liste.begin(),
    fin_terrain=lst_terrain.liste.end();
  for (; terrain != fin_terrain; ++terrain)
  {
    bool choisi = terrain -> name == lst_terrain.TerrainActif().name;
    lbox_maps->AddItem (choisi, terrain -> name, terrain -> name);
  }

  // Load Teams' list
  teams_list.full_list.sort(compareTeams);

  TeamsList::full_iterator
    it=teams_list.full_list.begin(), 
    end=teams_list.full_list.end();

  uint i=0;
  for (; it != end; ++it)
  {
    bool choix = teams_list.IsSelected (i);
    if (choix)
      lbox_selected_teams->AddItem (false, (*it).GetName(), (*it).GetId());
    else
      lbox_all_teams->AddItem (false, (*it).GetName(), (*it).GetId());
    ++i;
  }

  terrain_init = false;
}

//-----------------------------------------------------------------------------

GameMenu::~GameMenu()
{
  delete map_box;
  delete team_box;
}

//-----------------------------------------------------------------------------

void GameMenu::OnClic ( int x, int y, int button)
{     
  if (lbox_maps->Clic(x, y, button)) {
    ChangeMap();
  } else if (lbox_all_teams->Clic(x, y, button)) {

  } else if (lbox_selected_teams->Clic(x, y, button)) {

  } else if ( bt_add_team->MouseIsOver(x, y)) {
    if (lbox_selected_teams->GetItemsList()->size() < game_mode.max_teams)
      MoveTeams(lbox_all_teams, lbox_selected_teams, false); 
  } else if ( bt_remove_team->MouseIsOver(x, y)) {
    MoveTeams(lbox_selected_teams, lbox_all_teams, true);
  }
}

//-----------------------------------------------------------------------------

void GameMenu::SaveOptions()
{
  // Save values
  std::string map_id = lbox_maps->ReadLabel();
  lst_terrain.ChangeTerrainNom (map_id);

  // teams
  std::vector<list_box_item_t> * 
    selected_teams = lbox_selected_teams->GetItemsList();

  if (selected_teams->size() > 1) {
    std::list<uint> selection;

    std::vector<list_box_item_t>::iterator 
      it = selected_teams->begin(), 
      end = selected_teams->end();

    int index = -1;
    for (; it != end; ++it) {
      teams_list.FindById(it->value, index);
      if (index > -1)
	selection.push_back(uint(index));
    }
    teams_list.ChangeSelection (selection);

  }
   
  //Save options in XML
  config.Sauve();
}

//-----------------------------------------------------------------------------

void GameMenu::__sig_ok()
{
  SaveOptions();
  game.Start();
}

//-----------------------------------------------------------------------------

void GameMenu::__sig_cancel()
{
  // Nothing to do
}

//-----------------------------------------------------------------------------

void GameMenu::ChangeMap()
{
  std::string map_id = lbox_maps->ReadLabel();
  uint map = lst_terrain.FindMapById(map_id);
  map_preview = new Sprite(lst_terrain.liste[map].preview);
  float scale = std::min( float(MAP_PREVIEW_W)/map_preview->GetHeight(), 
                          float(MAP_PREVIEW_W)/map_preview->GetWidth() ) ;

  map_preview->Scale (scale, scale);
}

//-----------------------------------------------------------------------------

void GameMenu::MoveTeams(ListBox * from, ListBox * to, bool sort)
{
  if (from->GetSelectedItem() != -1) {
    to->AddItem (false, 
		 from->ReadLabel(),
		 from->ReadValue());
    to->Deselect();
    if (sort) to->Sort();
    
    from->RemoveSelected();
  }
}

//-----------------------------------------------------------------------------

void GameMenu::Draw(int mouse_x, int mouse_y)
{   
  Team* last_team = teams_list.FindByIndex(0);
   
  map_box->Draw(mouse_x,mouse_y);
  team_box->Draw(mouse_x,mouse_y);
     
  int t = lbox_all_teams->MouseIsOnWhichItem (mouse_x,mouse_y);    
  if (t != -1) {
    int index = -1;
    Team * new_team = teams_list.FindById(lbox_all_teams->ReadValue(t), index);
    if (new_team!=NULL) last_team = new_team;
  } else {
    t = lbox_selected_teams->MouseIsOnWhichItem (mouse_x,mouse_y);  
    if (t != -1) {
      int index = -1;
      Team * new_team = teams_list.FindById(lbox_selected_teams->ReadValue(t), index);
      if (new_team!=NULL) last_team = new_team;
    }
  }
   
  SDL_Rect team_icon_rect = { space_for_logo->GetX(), 
			      space_for_logo->GetY(),
			      TEAM_LOGO_H,
			      TEAM_LOGO_H};

  SDL_BlitSurface (last_team->ecusson, NULL, app.sdlwindow, &team_icon_rect); 
  
  if (!terrain_init)
    {
      terrain_init = true;
      ChangeMap();
    }
  
  map_preview->Blit ( app.sdlwindow, MAPS_X+MAPS_W+10, MAPS_Y+5);  
}

//-----------------------------------------------------------------------------
