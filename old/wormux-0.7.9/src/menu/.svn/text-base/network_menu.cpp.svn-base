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

const uint MARGIN_TOP    = 5;
const uint MARGIN_SIDE   = 5;
const uint MARGIN_BOTTOM = 70;

const uint TEAMS_W = 160;
const uint TEAMS_BOX_H = 180;
const uint TEAM_LOGO_H = 48;
const uint OPTIONS_BOX_H = 150;

NetworkMenu::NetworkMenu() :
  Menu("menu/bg_network")
{
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml",false);
  Rectanglei rectZero(0, 0, 0, 0);
  Rectanglei stdRect (0, 0, 130, 30);

  Surface window = AppWormux::GetInstance()->video.window;

  // Calculate main box size
  uint mainBoxWidth = window.GetWidth() - 2*MARGIN_SIDE;
  uint mapBoxHeight = (window.GetHeight() - MARGIN_TOP - MARGIN_BOTTOM - 2*MARGIN_SIDE) 
    - TEAMS_BOX_H - OPTIONS_BOX_H;

  // ################################################
  // ##  TEAM SELECTION
  // ################################################
  team_box = new NetworkTeamsSelectionBox(Rectanglei(MARGIN_SIDE, MARGIN_TOP,
					      mainBoxWidth, TEAMS_BOX_H));
  widgets.AddWidget(team_box);

  // ################################################
  // ##  MAP SELECTION
  // ################################################
  if(network.IsServer()) {
    map_box = new MapSelectionBox( Rectanglei(MARGIN_SIDE, team_box->GetPositionY()+team_box->GetSizeY()+ MARGIN_SIDE,
					      mainBoxWidth, mapBoxHeight),
				   false);
  } else {
    map_box = new MapSelectionBox( Rectanglei(MARGIN_SIDE, team_box->GetPositionY()+team_box->GetSizeY()+ MARGIN_SIDE,
					      mainBoxWidth, mapBoxHeight),
				   true);
  }
  widgets.AddWidget(map_box);

  // ################################################
  // ##  GAME OPTIONS
  // ################################################

  options_box = new HBox( Rectanglei(MARGIN_SIDE, map_box->GetPositionY()+map_box->GetSizeY()+ MARGIN_SIDE,
				     mainBoxWidth, OPTIONS_BOX_H), true);
  options_box->AddWidget(new PictureWidget(Rectanglei(0,0,39,128), "menu/mode_label"));
  
  Box* tmp_box = new VBox( Rectanglei(0,0, 200,0), false);
  player_number = new SpinButton(_("Max number of players:"), rectZero, 
				 GameMode::GetInstance()->max_teams, 1, 2, 
				 GameMode::GetInstance()->max_teams);
  
  tmp_box->AddWidget(player_number);

  connected_players = new Label(Format(ngettext("%i player connected", "%i players connected", 0), 0), 
				rectZero, *Font::GetInstance(Font::FONT_SMALL));
  tmp_box->AddWidget(connected_players);

  inited_players = new Label(Format(ngettext("%i player ready", "%i players ready", 0), 0), 
			     rectZero, *Font::GetInstance(Font::FONT_SMALL));
  tmp_box->AddWidget(inited_players);

  options_box->AddWidget(tmp_box);
  widgets.AddWidget(options_box);

  // ################################################
  // ##  CHAT BOX
  // ################################################
  VBox* chat_box = new VBox(Rectanglei(options_box->GetPositionX() + options_box->GetSizeX() + MARGIN_SIDE,
				       options_box->GetPositionY(),
				       mainBoxWidth - options_box->GetSizeX() - MARGIN_SIDE,
				       OPTIONS_BOX_H), false);
  chat_box->SetBorder(Point2i(0,0));
  
  msg_box = new MsgBox(Rectanglei( 0, 0, 400, OPTIONS_BOX_H - 20), Font::GetInstance(Font::FONT_SMALL));  
  msg_box->NewMessage(_("Join #wormux on irc.freenode.net to find some opponents."));
  msg_box->NewMessage(_("WARNING! Disconnections are not yet handled. So you have to restart Wormux after each disconnection!"), c_red);

  chat_box->AddWidget(msg_box);

  HBox* tmp2_box = new HBox(Rectanglei(0,0,chat_box->GetSizeX(),16), false);
  tmp2_box->SetMargin(4);
  tmp2_box->SetBorder(Point2i(0,0));
  line_to_send_tbox = new TextBox(" ",
				  Rectanglei(0, 0, chat_box->GetSizeX()-20, 0), 
				  *Font::GetInstance(Font::FONT_SMALL));
  tmp2_box->AddWidget(line_to_send_tbox);
  
  send_txt_bt = new Button(Point2i(0,0), res, "menu/send_txt", true);
  tmp2_box->AddWidget(send_txt_bt);

  chat_box->AddWidget(tmp2_box);

  widgets.AddWidget(chat_box);

  resource_manager.UnLoadXMLProfile(res);
}

NetworkMenu::~NetworkMenu()
{
}

void NetworkMenu::OnClic(const Point2i &mousePosition, int button)
{
  Widget* w = widgets.Clic(mousePosition, button);

  if(w == player_number)
  {
    network.max_player_number = player_number->GetValue();
  }

  if(w == send_txt_bt)
  {
    std::string empty = "";
    network.SendChatMessage(line_to_send_tbox->GetText());
    line_to_send_tbox->SetText(empty);
  }
}

void NetworkMenu::SaveOptions()
{
  // map
  map_box->ValidMapSelection();

  // team
  team_box->ValidTeamsSelection();

  //Save options in XML
//  Config::GetInstance()->Save();
}

void NetworkMenu::__sig_ok()
{
  if(network.IsClient())
  {
    // Wait for the server, and stay in the menu map / team can still be changed
    Action a(Action::ACTION_CHANGE_STATE);
    network.SendAction(&a);
    while(network.state != Network::NETWORK_INIT_GAME)
    {
      Display(Point2i(-1,-1));
    }
  }

  SaveOptions();
  Game::GetInstance()->Start();
  network.network_menu = NULL;
}

void NetworkMenu::sig_ok()
{
  if(network.IsServer())
  {
    if(teams_list.playing_list.size() <= 1)
    {
      msg_box->NewMessage(Format(ngettext("There is only %i team.", "There are only %i teams.", teams_list.playing_list.size()), teams_list.playing_list.size()), c_red);
      return;
    }
    if(network.connected_player <= 1)
    {
      msg_box->NewMessage(_("You are alone..."), c_red);
      return;
    }
    if(network.connected_player != network.client_inited)
    {
      int nbr = network.connected_player - network.client_inited;
      std::string pl = Format(ngettext("Wait! %i player is not ready yet!", "Wait! %i players are not ready yet!", nbr), nbr);
      msg_box->NewMessage(pl, c_red);
      return;
    }
  }
  Menu::sig_ok();
}

void NetworkMenu::__sig_cancel()
{
  network.Disconnect();
}

void NetworkMenu::Draw(const Point2i &mousePosition)
{
  if(network.IsConnected())
  {
    //Refresh the number of connected players:
    int nbr = network.connected_player;
    std::string pl = Format(ngettext("%i player connected", "%i players connected", nbr), nbr);
    if(connected_players->GetText() != pl)
      connected_players->SetText(pl);
    //Refresh the number of players ready:
    nbr = network.client_inited;
    pl = Format(ngettext("%i player ready", "%i players ready", nbr), nbr);
    if(inited_players->GetText() != pl)
      inited_players->SetText(pl);
  }
  else {
    close_menu = true;
  }
  ActionHandler * action_handler = ActionHandler::GetInstance();
  action_handler->ExecActions();
}

void NetworkMenu::DelTeamCallback(std::string team_id)
{
  if( close_menu )
    return;

  // Called from the action handler
  team_box->DelTeamCallback(team_id);
}

void NetworkMenu::AddTeamCallback(std::string team_id)
{
  if ( close_menu )
    return;

  team_box->AddTeamCallback(team_id);
  msg_box->NewMessage(team_id + " selected");
}

void NetworkMenu::UpdateTeamCallback(std::string team_id)
{
  if ( close_menu )
    return;

  team_box->UpdateTeamCallback(team_id);
  msg_box->NewMessage(team_id + " updated");
}

void NetworkMenu::ChangeMapCallback()
{
  assert( !close_menu );
  // Called from the action handler

  map_box->ChangeMapCallback();
}

void NetworkMenu::ReceiveMsgCallback(std::string msg)
{
  msg_box->NewMessage(msg);
}
