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

#include "../game/game.h"
#include "../game/config.h"
#include "../game/game_mode.h"
#include "../graphic/video.h"
#include "../graphic/font.h"
#include "../map/maps_list.h"
#include "../include/app.h"
#include "../team/teams_list.h"
#include "../tool/i18n.h"
#include "../tool/string_tools.h"

const uint TEAMS_Y = 20;
const uint TEAMS_W = 160;
const uint TEAMS_H = 160;
const uint TEAM_LOGO_Y = 290;
const uint TEAM_LOGO_H = 48;

const uint MAPS_X = 20;
const uint MAPS_W = 160;

const uint MAP_PREVIEW_W = 300;

GameMenu::GameMenu() :
  Menu("menu/bg_play")
{
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml",false);
  Rectanglei rectZero(0, 0, 0, 0);

  normal_font = Font::GetInstance(Font::FONT_NORMAL);

  // Center the boxes!
  uint x = 30;

  /* Choose the teams !! */
  team_box = new VBox(Rectanglei( x, TEAMS_Y, 475, 1));
  team_box->AddWidget(new Label(_("Select the teams:"), rectZero, *normal_font));

  Box * tmp_box = new HBox( Rectanglei(0,0, 1, TEAMS_H), false);
  tmp_box->SetMargin(10);
  tmp_box->SetBorder( Point2i(0,0) );

  lbox_all_teams = new ListBox( Rectanglei( 0, 0, TEAMS_W, TEAMS_H - TEAM_LOGO_H - 5 ));
  lbox_all_teams->always_one_selected = false;
  tmp_box->AddWidget(lbox_all_teams);

  Box * buttons_tmp_box = new VBox(Rectanglei(0, 0, 68, 1), false);

  bt_add_team = new Button( Rectanglei(0, 0, 48, 48) ,res,"menu/arrow-right");
  buttons_tmp_box->AddWidget(bt_add_team);

  bt_remove_team = new Button( Rectanglei( 0, 0, 48, 48 ),res,"menu/arrow-left");
  buttons_tmp_box->AddWidget(bt_remove_team);

  space_for_logo = new NullWidget( Rectanglei(0,0,48,48) );
  buttons_tmp_box->AddWidget(space_for_logo);

  tmp_box->AddWidget(buttons_tmp_box);
  lbox_selected_teams = new ListBox( Rectanglei(0, 0, TEAMS_W, TEAMS_H - TEAM_LOGO_H - 5 ));
  lbox_selected_teams->always_one_selected = false;
  tmp_box->AddWidget(lbox_selected_teams);

  team_box->AddWidget(tmp_box);



  /* Choose the map !! */
  tmp_box = new HBox( Rectanglei(0, 0, 1, MAP_PREVIEW_W - 25 ), false);
  tmp_box->SetMargin(0);
  tmp_box->SetBorder( Point2i(0,0) );

  lbox_maps = new ListBox( Rectanglei(0, 0, MAPS_W, MAP_PREVIEW_W-25 ));
  tmp_box->AddWidget(lbox_maps);
  tmp_box->AddWidget(new NullWidget( Rectanglei(0, 0, MAP_PREVIEW_W+5, MAP_PREVIEW_W)));

  map_box = new VBox( Rectanglei(x, team_box->GetPositionY()+team_box->GetSizeY()+20, 475, 1) );
  map_box->AddWidget(new Label(_("Select the world:"), rectZero, *normal_font));
  map_box->AddWidget(tmp_box);
  // Values initialization

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

GameMenu::~GameMenu()
{
  delete map_preview;
  delete map_box;
  delete team_box;
}

void GameMenu::OnClic(const Point2i &mousePosition, int button)
{
  if (lbox_maps->Clic(mousePosition, button)) {
    ChangeMap();
  } else if (lbox_all_teams->Clic(mousePosition, button)) {

  } else if (lbox_selected_teams->Clic(mousePosition, button)) {

  } else if ( bt_add_team->Contains(mousePosition)) {
    if (lbox_selected_teams->GetItemsList()->size() < GameMode::GetInstance()->max_teams)
      MoveTeams(lbox_all_teams, lbox_selected_teams, false);
  } else if ( bt_remove_team->Contains(mousePosition)) {
    MoveTeams(lbox_selected_teams, lbox_all_teams, true);
  }
}

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
  Config::GetInstance()->Save();
}

void GameMenu::__sig_ok()
{
  SaveOptions();
  Game::GetInstance()->Start();
}

void GameMenu::__sig_cancel()
{
  // Nothing to do
}

void GameMenu::ChangeMap()
{
  std::string map_id = lbox_maps->ReadLabel();
  uint map = lst_terrain.FindMapById(map_id);
  if(terrain_init)
    delete map_preview;
  map_preview = new Sprite(lst_terrain.liste[map].preview);
  float scale = std::min( float(MAP_PREVIEW_W)/map_preview->GetHeight(),
                          float(MAP_PREVIEW_W)/map_preview->GetWidth() ) ;

  map_preview->Scale (scale, scale);
}

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

void GameMenu::Draw(const Point2i &mousePosition)
{
  Team* last_team = teams_list.FindByIndex(0);

  map_box->Draw(mousePosition);
  team_box->Draw(mousePosition);

  int t = lbox_all_teams->MouseIsOnWhichItem(mousePosition);
  if (t != -1) {
    int index = -1;
    Team * new_team = teams_list.FindById(lbox_all_teams->ReadValue(t), index);
    if (new_team!=NULL) last_team = new_team;
  } else {
    t = lbox_selected_teams->MouseIsOnWhichItem(mousePosition);
    if (t != -1) {
      int index = -1;
      Team * new_team = teams_list.FindById(lbox_selected_teams->ReadValue(t), index);
      if (new_team!=NULL) last_team = new_team;
    }
  }

  AppWormux * app = AppWormux::GetInstance();
  app->video.window.Blit( last_team->ecusson, space_for_logo->GetPosition() );

  if (!terrain_init){
      ChangeMap();
      terrain_init = true;
  }

  map_preview->Blit ( app->video.window,
		      map_box->GetPositionX()+MAPS_W+10,
		      map_box->GetPositionY()+map_box->GetSizeY()/2-map_preview->GetHeight()/2);
}

