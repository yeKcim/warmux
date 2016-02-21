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
 * Game menu
 *****************************************************************************/

#include "network_menu.h"

#include "network_teams_selection_box.h"
#include "map_selection_box.h"
#include "game/game.h"
#include "game/game_mode.h"
#include "graphic/video.h"
#include "gui/button.h"
#include "gui/label.h"
#include "gui/msg_box.h"
#include "gui/picture_widget.h"
#include "gui/spin_button.h"
#include "gui/text_box.h"
#include "include/action_handler.h"
#include "include/app.h"
#include "network/index_server.h"
#include "network/network.h"
#include "network/network_server.h"
#include "team/teams_list.h"
#include "team/team.h"
#include "tool/i18n.h"
#include "tool/resource_manager.h"

const uint MARGIN_TOP    = 5;
const uint MARGIN_SIDE   = 5;
const uint MARGIN_BOTTOM = 70;

// XXX Not used ?
// const uint TEAMS_W = 160;
// const uint TEAM_LOGO_H = 48;
const uint TEAMS_BOX_H = 180;
const uint OPTIONS_BOX_H = 150;

NetworkMenu::NetworkMenu() :
  Menu("menu/bg_network")
{
  waiting_for_server = false;

  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml",false);
  Rectanglei rectZero(0, 0, 0, 0);
  Rectanglei stdRect (0, 0, 130, 30);

  Surface window = AppWormux::GetInstance()->video->window;

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
  if(Network::GetInstance()->IsServer()) {
    map_box = new MapSelectionBox( Rectanglei(MARGIN_SIDE, team_box->GetPositionY()+team_box->GetSizeY()+ MARGIN_SIDE,
                                              mainBoxWidth, mapBoxHeight));
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

  mode = new Label("", rectZero, Font::FONT_MEDIUM, Font::FONT_NORMAL);

  if (Network::GetInstance()->IsClient()) {
    // Client Mode
    mode->SetText(_("Client mode"));
    tmp_box->AddWidget(mode);

    player_number = NULL;
    connected_players = NULL;
    initialized_players = NULL;
  } else {

    // Server Mode
    mode->SetText(_("Server mode"));
    tmp_box->AddWidget(mode);

    player_number = new SpinButton(_("Max number of players:"), rectZero,
                                   GameMode::GetInstance()->max_teams, 1, 2,
                                   GameMode::GetInstance()->max_teams);

    tmp_box->AddWidget(player_number);

    connected_players = new Label(Format(ngettext("%i player connected", "%i players connected", 0), 0),
                                rectZero, Font::FONT_SMALL, Font::FONT_NORMAL);
    tmp_box->AddWidget(connected_players);

    initialized_players = new Label(Format(ngettext("%i player ready", "%i players ready", 0), 0),
                                    rectZero, Font::FONT_SMALL, Font::FONT_NORMAL);
    tmp_box->AddWidget(initialized_players);
  }

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

  msg_box = new MsgBox(Rectanglei( 0, 0, 400, OPTIONS_BOX_H - 20), Font::FONT_SMALL, Font::FONT_NORMAL);
  msg_box->NewMessage(_("Join #wormux on irc.freenode.net to find some opponents."));

  chat_box->AddWidget(msg_box);

  HBox* tmp2_box = new HBox(Rectanglei(0,0,chat_box->GetSizeX(),16), false);
  tmp2_box->SetMargin(4);
  tmp2_box->SetBorder(Point2i(0,0));
  line_to_send_tbox = new TextBox(" ",
                                  Rectanglei(0, 0, chat_box->GetSizeX()-20, 0),
                                  Font::FONT_SMALL, Font::FONT_NORMAL);
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

void NetworkMenu::OnClickUp(const Point2i &mousePosition, int button)
{
  Widget* w = widgets.ClickUp(mousePosition, button);

  if (player_number != NULL && w == player_number)
  {
    Network::GetInstanceServer()->SetMaxNumberOfPlayers(player_number->GetValue());
  }
  else if (w == send_txt_bt)
  {
    SendChatMsg();
  }
}

void NetworkMenu::OnClick(const Point2i &mousePosition, int button)
{
  widgets.Click(mousePosition, button);
}

void NetworkMenu::SendChatMsg()
{
  std::string empty = "";
  Network::GetInstance()->SendChatMessage(line_to_send_tbox->GetText());
  line_to_send_tbox->SetText(empty);
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

bool NetworkMenu::signal_ok()
{
  if (Network::GetInstance()->IsClient())
  {
    // Check the user have selected a team:
    bool found = false;
    for(std::vector<Team*>::iterator team = teams_list.playing_list.begin();
                    team != teams_list.playing_list.end();
                    team++)
    {
      if((*team)->IsLocal())
      {
        found = true;
        break;
      }
    }
    if(!found)
    {
      msg_box->NewMessage(_("You won't be able to play before selecting a team !"));
      goto error;
    }

    // Wait for the server, and stay in the menu map / team can still be changed
    WaitingForServer();

  }
  else if (Network::GetInstance()->IsServer())
  {
    if (teams_list.playing_list.size() <= 1)
    {
      msg_box->NewMessage(Format(ngettext("There is only %i team.",
                                          "There are only %i teams.",
                                          teams_list.playing_list.size()),
                                 teams_list.playing_list.size()), c_red);
      goto error;
    }
    if (Network::GetInstanceServer()->GetNbConnectedPlayers() <= 1)
    {
      msg_box->NewMessage(_("You are alone..."), c_red);
      goto error;
    }
    if (Network::GetInstanceServer()->GetNbConnectedPlayers() != Network::GetInstanceServer()->GetNbInitializedPlayers()+1)
    {
      int nbr = Network::GetInstanceServer()->GetNbConnectedPlayers() - Network::GetInstanceServer()->GetNbInitializedPlayers();
      std::string pl = Format(ngettext("Wait! %i player is not ready yet!", "Wait! %i players are not ready yet!", nbr), nbr);
      msg_box->NewMessage(pl, c_red);
      goto error;
    }
  }

  // Disconnection :-/
  if (!Network::IsConnected()) {
    Network::GetInstance()->network_menu = NULL;
    return true;

  } else {

    // Starting the game :-)
    SaveOptions();
    play_ok_sound();
    Game::GetInstance()->Start();
    Network::GetInstance()->network_menu = NULL;
  }

  Network::Disconnect();
  return true;

 error:
  play_error_sound();
  return false;
}

void NetworkMenu::key_ok()
{
  // return was pressed while chat texbox still had focus (player wants to send his msg)
  if (line_to_send_tbox->have_focus)
  {
    SendChatMsg();
    return;
  }

  Menu::key_ok();
}

bool NetworkMenu::signal_cancel()
{
  Network::Disconnect();
  return true;
}

void NetworkMenu::Draw(const Point2i &/*mousePosition*/)
{
  if(Network::GetInstance()->IsConnected())
  {
    if (connected_players != NULL) {
      //Refresh the number of connected players:
      int nbr = Network::GetInstanceServer()->GetNbConnectedPlayers();
      std::string pl = Format(ngettext("%i player connected", "%i players connected", nbr), nbr);
      if(connected_players->GetText() != pl)
        connected_players->SetText(pl);
    }

    if (initialized_players != NULL) {
      //Refresh the number of players ready:
      int nbr = Network::GetInstanceServer()->GetNbInitializedPlayers();
      std::string pl = Format(ngettext("%i player ready", "%i players ready", nbr), nbr);
      if (initialized_players->GetText() != pl) {
        initialized_players->SetText(pl);
        msg_box->NewMessage(pl, c_red);
        if (Network::GetInstanceServer()->GetNbConnectedPlayers() -
            Network::GetInstanceServer()->GetNbInitializedPlayers() == 1
            && Network::GetInstanceServer()->GetNbConnectedPlayers() >= 1) {
          msg_box->NewMessage(_("The others are waiting for you! Wake up :-)"), c_red);
        }
        else if (Network::GetInstanceServer()->GetNbConnectedPlayers() == 1) {
          msg_box->NewMessage(_("You are alone :-/"), c_red);
        }
      }
    }

    index_server.Refresh();
  }
  else {
    close_menu = true;
  }
  ActionHandler * action_handler = ActionHandler::GetInstance();
  action_handler->ExecActions();
}

void NetworkMenu::DelTeamCallback(const std::string& team_id)
{
  if( close_menu )
    return;

  // Called from the action handler
  team_box->DelTeamCallback(team_id);
}

void NetworkMenu::AddTeamCallback(const std::string& team_id)
{
  if ( close_menu )
    return;

  team_box->AddTeamCallback(team_id);
//  msg_box->NewMessage(team_id + " selected");
}

void NetworkMenu::UpdateTeamCallback(const std::string& team_id)
{
  if ( close_menu )
    return;

  team_box->UpdateTeamCallback(team_id);
}

void NetworkMenu::ChangeMapCallback()
{
  if (close_menu)
    return;

  map_box->ChangeMapCallback();
}

void NetworkMenu::ReceiveMsgCallback(const std::string& msg)
{
  msg_box->NewMessage(msg);
}

void NetworkMenu::WaitingForServer()
{
  Network::GetInstance()->SetState(Network::NETWORK_MENU_OK);

  // warn the server that we have validated the menu
  Network::GetInstance()->SendNetworkState();

  waiting_for_server = true;
  b_ok->SetVisible(false);
  actions_buttons->ForceRedraw();

  msg_box->NewMessage(_("Waiting for server, all you can do is cancel or chat!"), c_red);

  int x=0, y=0;
  SDL_GetMouseState( &x, &y );
  Point2i mousePosition(x, y);

  do
  {
    // Poll and treat events
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
      Point2i mousePosition(event.button.x, event.button.y);

      if (event.type == SDL_QUIT) {
        Menu::mouse_cancel();
      } else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym)
          {
          case SDLK_ESCAPE:
            Menu::mouse_cancel();
            break;
          case SDLK_RETURN:
            SendChatMsg();
            break;
          case SDLK_F10:
            AppWormux::GetInstance()->video->ToggleFullscreen();
            break;
          default:
            widgets.SendKey(event.key.keysym);
            break;
          }
      } else if (event.type == SDL_MOUSEBUTTONUP) {
        if (b_cancel->Contains(mousePosition))
          Menu::mouse_cancel();

        if (send_txt_bt->Contains(mousePosition))
          SendChatMsg();
      }
    }

    Menu::Display(mousePosition);
    widgets.SetFocusOn(line_to_send_tbox);

  } while (Network::GetInstance()->GetState() == Network::NETWORK_MENU_OK &&
           Network::GetInstance()->IsConnected());
}
