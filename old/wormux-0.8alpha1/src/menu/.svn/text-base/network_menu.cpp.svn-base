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
 * Game menu
 *****************************************************************************/

#include "network_menu.h"

#include "../game/game.h"
#include "../game/config.h"
#include "../game/game_mode.h"
#include "../graphic/video.h"
#include "../graphic/font.h"
#include "../map/maps_list.h"
#include "../network/network.h"
#include "../include/app.h"
#include "../include/action_handler.h"
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

#define WORMUX_NETWORK_PORT "9999"

NetworkMenu::NetworkMenu() :
  Menu("menu/bg_network")
{
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml",false);
  Rectanglei rectZero(0, 0, 0, 0);

  normal_font = Font::GetInstance(Font::FONT_NORMAL);

  // Game options widgets
  player_number = new SpinButton(_("Max number of players:"), rectZero, GameMode::GetInstance()->max_teams, 1, 2, GameMode::GetInstance()->max_teams);
  options_box = new VBox(Rectanglei( 475 + 30 + 5, TEAMS_Y + 2 * TEAMS_Y, 800-475-40, 1));
  options_box->AddWidget(new Label(_("Game options:"),rectZero, *normal_font));
  options_box->AddWidget(player_number);
  connected_players = new Label((std::string)"0" + _(" players connected"), rectZero, *normal_font);
  inited_players = new Label((std::string)"0" + _(" players ready"), rectZero, *normal_font);
  options_box->AddWidget(connected_players);
  options_box->AddWidget(inited_players);
  //options_box->enabled = false;
  widgets.AddWidget(options_box);

  msg_box = new MessageBox(11, Rectanglei( 475 + 30 + 5, options_box->GetPositionY() + options_box->GetSizeY() + TEAMS_Y, 800-475-40, 1), Font::GetInstance(Font::FONT_SMALL));
  widgets.AddWidget(msg_box);
  msg_box->NewMessage(_("Join #wormux on irc.freenode.net to find"));
  msg_box->NewMessage(_("some opponents."));
  msg_box->NewMessage(_("WARNING! Disconnections are not yet handled."));
  msg_box->NewMessage(_("So you have to restart Wormux after each"));
  msg_box->NewMessage(_("disconnection!"));

  chat_box = new TextBox(std::string(""),Rectanglei(475 + 30 + 5, 600 - 100, 800 - 475 + 40, 25), *Font::GetInstance(Font::FONT_SMALL));
  widgets.AddWidget(chat_box);
  send_txt = new ButtonText(Point2i(475 + 30 + 5, 600 - 75), res, "main_menu/button", "Send text !",normal_font);
  widgets.AddWidget(send_txt);

  // Center the boxes!
  uint x = 30;

  /* Choose the teams !! */
  team_box = new VBox(Rectanglei( x, TEAMS_Y, 475, 1));
  team_box->AddWidget(new Label(_("Select the teams:"), rectZero, *normal_font));

  Box * tmp_box = new HBox( Rectanglei(0,0, 1, TEAMS_H), false);
  tmp_box->SetMargin(10);
  tmp_box->SetBorder( Point2i(0,0) );

  lbox_all_teams = new ListBox( Rectanglei( 0, 0, TEAMS_W, TEAMS_H - TEAM_LOGO_H - 5 ), false);
  tmp_box->AddWidget(lbox_all_teams);

  Box * buttons_tmp_box = new VBox(Rectanglei(0, 0, 68, 1), false);

  bt_add_team = new Button( Point2i(0, 0) ,res,"menu/arrow-right");
  buttons_tmp_box->AddWidget(bt_add_team);

  bt_remove_team = new Button( Point2i( 0, 0),res,"menu/arrow-left");
  buttons_tmp_box->AddWidget(bt_remove_team);

  team_logo = new PictureWidget( Rectanglei(0,0,48,48) );
  buttons_tmp_box->AddWidget(team_logo);

  tmp_box->AddWidget(buttons_tmp_box);

  lbox_selected_teams = new ListBox( Rectanglei(0, 0, TEAMS_W, TEAMS_H - TEAM_LOGO_H - 5 ), false);
  tmp_box->AddWidget(lbox_selected_teams);

  team_box->AddWidget(tmp_box);
  //team_box->enabled = false;
  widgets.AddWidget(team_box);

  last_team = NULL;

  /* Choose the map !! */
  tmp_box = new HBox( Rectanglei(0, 0, 1, MAP_PREVIEW_W - 25 ), false);
  tmp_box->SetMargin(2);
  tmp_box->SetBorder( Point2i(0,0) );

  lbox_maps = new ListBox( Rectanglei(0, 0, MAPS_W, MAP_PREVIEW_W-25 ));
  tmp_box->AddWidget(lbox_maps);

  map_preview = new PictureWidget( Rectanglei(0, 0, MAP_PREVIEW_W+5, MAP_PREVIEW_W));
  tmp_box->AddWidget(map_preview);

  map_box = new VBox( Rectanglei(x, team_box->GetPositionY()+team_box->GetSizeY()+20, 475, 1) );
  map_box->AddWidget(new Label(_("Select the world:"), rectZero, *normal_font));
  map_box->AddWidget(tmp_box);
  //map_box->enabled = false;
  widgets.AddWidget(map_box);

  resource_manager.UnLoadXMLProfile(res);

  // Values initialization

  // Load Maps' list
  std::sort(MapsList::GetInstance()->lst.begin(), MapsList::GetInstance()->lst.end(), compareMaps);

  MapsList::iterator
    terrain=MapsList::GetInstance()->lst.begin(),
    fin_terrain=MapsList::GetInstance()->lst.end();
  for (; terrain != fin_terrain; ++terrain)
    lbox_maps->AddItem (false, terrain -> ReadName(), terrain -> ReadName());
  lbox_maps->Select(0);

  // Load Teams' list
  teams_list.full_list.sort(compareTeams);

  TeamsList::full_iterator
    it=teams_list.full_list.begin(),
    end=teams_list.full_list.end();

  // No selected team by default
  for (; it != end; ++it)
    lbox_all_teams->AddItem (false, (*it).GetName(), (*it).GetId());

  //b_ok->enabled = false;

  teams_list.Clear();
  Reset();

  ChangeMap();
}

NetworkMenu::~NetworkMenu()
{
}

void NetworkMenu::OnClic(const Point2i &mousePosition, int button)
{
  ActionHandler * action_handler = ActionHandler::GetInstance();
  Widget* w = widgets.Clic(mousePosition, button);

  if (w == lbox_maps)
  {
    if(network.IsServer())
    {
      ChangeMap();
      action_handler->NewAction (new Action(ACTION_SET_MAP, ActiveMap().ReadName()));
    }
    else
      msg_box->NewMessage(_("Only the server can set the map!"));
  }

  if (w == bt_add_team) {
    if (lbox_all_teams->GetSelectedItem() != -1 && lbox_selected_teams->GetItemsList()->size() < GameMode::GetInstance()->max_teams)
    {
      int index = -1;
      teams_list.FindById(lbox_all_teams->ReadValue(),index)->is_local = true;
      std::string team_id = teams_list.FindById(lbox_all_teams->ReadValue(),index)->GetId();
      action_handler->NewAction (new Action(ACTION_NEW_TEAM, team_id));
      MoveTeams(lbox_all_teams, lbox_selected_teams, false);
    }
  }
  if (w == bt_remove_team) {
    int index = -1;
    if(lbox_selected_teams->GetSelectedItem() != -1 && teams_list.FindById(lbox_selected_teams->ReadValue(),index)->is_local)
    {
      std::string team_id = teams_list.FindById(lbox_selected_teams->ReadValue(),index)->GetId();
      action_handler->NewAction (new Action(ACTION_DEL_TEAM, team_id));
      MoveTeams(lbox_selected_teams, lbox_all_teams, true);
    }
  }
  if(w == player_number)
  {
    network.max_player_number = player_number->GetValue();
  }
  if(w == send_txt)
  {
    std::string empty = "";
    network.SendChatMessage(chat_box->GetText());
    chat_box->SetText(empty);
  }
}

void NetworkMenu::Reset()
{
  //If we are a client and the server disconnected:
//   map_box->enabled = false;
//   team_box->enabled = false;
//   connection_box->enabled = true;
//   lbox_maps->enabled = true;

  // Remove selected teams from the list
  while(lbox_selected_teams->GetItemsList()->size()!=0)
  {
    lbox_selected_teams->Select(0);
    MoveTeams(lbox_selected_teams, lbox_all_teams,true);
  }
  teams_list.Clear();
}

void NetworkMenu::SaveOptions()
{
  teams_list.Clear();
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
//  Config::GetInstance()->Save();
}

void NetworkMenu::__sig_ok()
{
  if(network.IsClient())
  {
    // Wait for the server, and stay in the menu map / team can still be changed
    Action a(ACTION_CHANGE_STATE);
    network.SendAction(&a);
//     b_ok->enabled = false;
//     b_cancel->enabled = false;
//     team_box->enabled = false;
    while(network.state != Network::NETWORK_INIT_GAME)
    {
      Display(Point2i(-1,-1));
    }
  }

  SaveOptions();
  Game::GetInstance()->Start();
}

void NetworkMenu::sig_ok()
{
  if(network.IsServer())
  {
    if(network.connected_player != network.client_inited)
    {
      char nbr[3];
      sprintf(nbr,"%i",network.connected_player - network.client_inited);
      std::string pl = _("Wait! ") + (std::string)nbr + _(" players are not yet ready!");
      msg_box->NewMessage(pl);
      return;
    }
  }
  Menu::sig_ok();
}

void NetworkMenu::__sig_cancel()
{
  network.Disconnect();
}

void NetworkMenu::ChangeMap()
{
  std::string map_id = lbox_maps->ReadLabel();
  uint map_index = MapsList::GetInstance()->FindMapById(map_id);
  MapsList::GetInstance()->SelectMapByIndex(map_index);

  map_preview->SetSurface(MapsList::GetInstance()->lst[map_index].ReadPreview(), false);
}

void NetworkMenu::SelectTeamLogo(Team * t)
{
  if (last_team != t) {
    last_team = t;
    team_logo->SetSurface(last_team->flag);
  }
}

void NetworkMenu::MoveTeams(ListBox * from, ListBox * to, bool sort)
{
  if (from->GetSelectedItem() != -1) {
    to->AddItem (false,
		 from->ReadLabel(),
		 from->ReadValue());
    to->Deselect();
    if (sort) to->Sort();

    from->RemoveSelected();

    from->ForceRedraw();
    to->ForceRedraw();
  }
}

void NetworkMenu::MoveDisableTeams(ListBox * from, ListBox * to, bool sort)
{
  if (from->GetSelectedItem() != -1) {
    to->AddItem (false,
		 from->ReadLabel(),
		 from->ReadValue(),
       false);
    to->Deselect();
    if (sort) to->Sort();

    from->RemoveSelected();

    from->ForceRedraw();
    to->ForceRedraw();
  }
}

void NetworkMenu::Draw(const Point2i &mousePosition)
{
  if(network.IsConnected())
  {
    int t = lbox_all_teams->MouseIsOnWhichItem(mousePosition);
    if (t != -1) {
      int index = -1;
      Team * new_team = teams_list.FindById(lbox_all_teams->ReadValue(t), index);
      SelectTeamLogo(new_team);
    } else {
      t = lbox_selected_teams->MouseIsOnWhichItem(mousePosition);
      if (t != -1) {
	int index = -1;
	Team * new_team = teams_list.FindById(lbox_selected_teams->ReadValue(t), index);
	SelectTeamLogo(new_team);
      }
    }

    //map_box->Draw(mousePosition);

    //Refresh the number of connected players:
    char nbr[3];
    sprintf(nbr,"%i",network.connected_player);
    std::string pl = (std::string)nbr + _(" players connected");
    if(connected_players->GetText() != pl)
      connected_players->SetText(pl);
    //Refresh the number of players ready:
    sprintf(nbr,"%i",network.client_inited);
    pl = (std::string)nbr + _(" players ready");
    if(inited_players->GetText() != pl)
      inited_players->SetText(pl);
    }
    ActionHandler * action_handler = ActionHandler::GetInstance();
    action_handler->ExecActions();
}

void NetworkMenu::DelTeamCallback(std::string team)
{
  // Called from the action handler
  for(std::vector<list_box_item_t>::iterator lst_it = lbox_selected_teams->GetItemsList()->begin();
      lst_it != lbox_selected_teams->GetItemsList()->end();
      lst_it++)
  {
    if(lst_it->value == team)
    {
      lbox_selected_teams->Select(lst_it->label);
      msg_box->NewMessage(lst_it->label + " unselected");
      MoveTeams(lbox_selected_teams, lbox_all_teams, true);
      return;
    }
  }
}

void NetworkMenu::AddTeamCallback(std::string team)
{
  // Called from the action handler
  for(std::vector<list_box_item_t>::iterator lst_it = lbox_all_teams->GetItemsList()->begin();
      lst_it != lbox_all_teams->GetItemsList()->end();
      lst_it++)
  {
    if(lst_it->value == team)
    {
      int index;
      teams_list.FindById(team, index)->is_local = false;

      lbox_all_teams->Select(lst_it->label);
      msg_box->NewMessage(lst_it->label + " selected");
      MoveDisableTeams(lbox_all_teams, lbox_selected_teams, false);
      return;
    }
  }
}

void NetworkMenu::ChangeMapCallback()
{
  // Called from the action handler
  map_preview->SetSurface(ActiveMap().ReadPreview(), false);
}
