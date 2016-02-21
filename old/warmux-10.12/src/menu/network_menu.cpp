/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Warmux Team.
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
#include <WARMUX_index_server.h>
#include <WARMUX_team_config.h>

#include "menu/network_menu.h"

#include "menu/network_teams_selection_box.h"
#include "menu/map_selection_box.h"
#include "game/config.h"
#include "game/game.h"
#include "game/game_mode.h"
#include "graphic/video.h"
#include "gui/button.h"
#include "gui/check_box.h"
#include "gui/combo_box.h"
#include "gui/grid_box.h"
#include "gui/label.h"
#include "gui/msg_box.h"
#include "gui/picture_widget.h"
#include "gui/spin_button.h"
#include "gui/tabs.h"
#include "gui/talk_box.h"
#include "gui/text_box.h"
#include "include/action_handler.h"
#include "include/app.h"
#include "include/constant.h"
#include "network/network.h"
#include "network/network_client.h"
#include "network/network_server.h"
#include "sound/jukebox.h"
#include "team/teams_list.h"
#include "team/team.h"
#include "tool/resource_manager.h"

static const uint MARGIN_TOP    = 5;
static const uint MARGIN_SIDE   = 5;
static const uint MARGIN_BOTTOM = 40;

NetworkMenu::NetworkMenu() :
  Menu("menu/bg_network")
{
  waiting_for_server = false;

  Profile *res = GetResourceManager().LoadXMLProfile( "graphism.xml",false);
  Point2i pointZero(W_UNDEF, W_UNDEF);

  Surface& window = GetMainWindow();

  // Calculate main box size
  int chat_box_height = window.GetHeight()/4;
  int team_box_height = 180;
  int mainBoxWidth = window.GetWidth() - 2*MARGIN_SIDE;
  int mainBoxHeight = window.GetHeight() - MARGIN_TOP - MARGIN_BOTTOM - 2*MARGIN_SIDE - chat_box_height;
  int mapsHeight = mainBoxHeight - team_box_height - 60;
  int multitabsWidth = mainBoxWidth;
  bool multitabs = false;

  if (window.GetWidth() > 640 && mapsHeight > 200) {
    multitabs = true;
    multitabsWidth = mainBoxWidth - 20;
    mapsHeight = 200;
    team_box_height = mainBoxHeight - 200 - 60;
  } else {
    mapsHeight = mainBoxHeight - 60;
    team_box_height = mainBoxHeight - 60;
  }

  MultiTabs * tabs = new MultiTabs(Point2i(mainBoxWidth, mainBoxHeight));

  // ################################################
  // ##  TEAM AND MAP SELECTION
  // ################################################
  team_box = new NetworkTeamsSelectionBox(Point2i(multitabsWidth-4, team_box_height), multitabs);

  map_box = new MapSelectionBox(Point2i(multitabsWidth, mapsHeight),
				 multitabs, !Network::GetInstance()->IsGameMaster());

  if (!multitabs) {
    tabs->AddNewTab("TAB_Team", _("Teams"), team_box);
    tabs->AddNewTab("TAB_Map", _("Map"), map_box);
  } else {
    VBox *box = new VBox(mainBoxWidth, false, false, true);
    std::string tabs_title = _("Teams") + std::string(" - ");
    tabs_title += _("Map");

    box->AddWidget(team_box);
    box->AddWidget(map_box);
    tabs->AddNewTab("TAB_Team_Map", tabs_title, box);
  }

  // ################################################
  // ##  GAME OPTIONS
  // ################################################

  if (Network::GetInstance()->IsGameMaster()) {
    // Using the game mode editor but currently we are not able to send
    // custom parameters to client

    Box *box = new GridBox(4, 4, 0, false);

    Point2i option_size(114, 114);
    std::string selected_gamemode = Config::GetInstance()->GetGameMode();

    opt_game_mode = new ComboBox(_("Game mode"), "menu/game_mode", option_size,
				 GameMode::ListGameModes(), selected_gamemode);
    box->AddWidget(opt_game_mode);

    tabs->AddNewTab("TAB_Game", _("Game"), box);
  }

  tabs->SetPosition(MARGIN_SIDE, MARGIN_TOP);

  widgets.AddWidget(tabs);
  widgets.Pack();


  Box* bottom_box = new HBox(chat_box_height, false, false, true);
  bottom_box->SetNoBorder();

  Box* options_box = new VBox(200, true, true);

  mode_label = new Label("", 0, Font::FONT_MEDIUM, Font::FONT_BOLD,
                         primary_red_color, Text::ALIGN_LEFT_TOP, true);
  options_box->AddWidget(mode_label);

  play_in_loop = new CheckBox(_("Play several times"), W_UNDEF, true);
  options_box->AddWidget(play_in_loop);

  connected_players = new Label(Format(ngettext("%i player connected", "%i players connected", 0), 0),
                                0, Font::FONT_SMALL, Font::FONT_BOLD);
  options_box->AddWidget(connected_players);

  initialized_players = new Label(Format(ngettext("%i player ready", "%i players ready", 0), 0),
                                  0, Font::FONT_SMALL, Font::FONT_BOLD);
  options_box->AddWidget(initialized_players);

  options_box->Pack();
  bottom_box->AddWidget(options_box);

  // ################################################
  // ##  CHAT BOX
  // ################################################

  msg_box = new TalkBox(Point2i(mainBoxWidth - options_box->GetSizeX() - MARGIN_SIDE, chat_box_height),
                        Font::FONT_SMALL, Font::FONT_BOLD);
  msg_box->SetPosition(options_box->GetPositionX() + options_box->GetSizeX() + MARGIN_SIDE,
                       options_box->GetPositionY());

  bottom_box->AddWidget(msg_box);
  bottom_box->SetPosition(MARGIN_SIDE, tabs->GetPositionY() + tabs->GetSizeY() + MARGIN_SIDE);

  widgets.AddWidget(bottom_box);
  widgets.Pack();

  GetResourceManager().UnLoadXMLProfile(res);

  if (!Network::GetInstance()->IsServer()) {

    // First %s will be replaced with the name of the network game,
    // second %s by the server hostname
    msg_box->NewMessage(Format(_("Welcome to %s on %s!"),
                               Network::GetInstance()->GetGameName().c_str(),
                               ((NetworkClient*)Network::GetInstance())->GetServerAddress().c_str())
                        , c_red);
  }

  if (!Network::GetInstance()->IsGameMaster()) {
    // Client Mode
    mode_label->SetText(_("Client mode"));
    initialized_players->SetVisible(false);
    msg_box->NewMessage(_("Don't forget to validate once you have selected your team(s)!"), c_red);

  } else if (Network::GetInstance()->IsServer()) {
    // Server Mode
    mode_label->SetText(_("Server mode"));
  } else {
    // The first player to connect to a headless server asumes the game master role
    SetGameMasterCallback();
  }
}

void NetworkMenu::signal_begin_run()
{
  ActionHandler::GetInstance()->ExecFrameLessActions();
  RequestSavedTeams();
}

void NetworkMenu::RequestSavedTeams()
{
  const std::list<ConfigTeam> & team_list = Config::GetInstance()->AccessNetworkTeamsList();
  std::list<ConfigTeam>::const_iterator it;

  if (team_list.size() > 0) {
    for (it = team_list.begin(); it != team_list.end(); it++) {
      ActionHandler::GetInstance()->NewRequestTeamAction(*it);
    }
  } else {
    team_box->RequestTeam();
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

  if (Network::GetInstance()->IsGameMaster()) {
    Config::GetInstance()->SetGameMode(opt_game_mode->GetValue());
  }
}

void NetworkMenu::PrepareForNewGame()
{
  msg_box->Clear();
  b_ok->SetVisible(true);

  Network::GetInstance()->SetState(WNet::NETWORK_NEXT_GAME);
  Network::GetInstance()->SendNetworkState();

  // to choose another random map
  if (Network::GetInstance()->IsGameMaster())
    map_box->ChangeMapDelta(0);

  RedrawMenu();
}

bool NetworkMenu::signal_ok()
{
  if (!Network::GetInstance()->IsGameMaster())
  {
    // Check the user have selected a team:
    bool found = false;
    for(std::vector<Team*>::iterator team = GetTeamsList().playing_list.begin();
                    team != GetTeamsList().playing_list.end();
                    team++)
    {
      if((*team)->IsLocalHuman())
      {
        found = true;
        break;
      }
    }
    if(!found)
    {
      msg_box->NewMessage(_("You won't be able to play before selecting a team!"));
      goto error;
    }

    // Wait for the game master, and stay in the menu map / team can still be changed
    WaitingForGameMaster();

    if (Network::GetInstance()->IsGameMaster()) {
      goto error;
    }

  }
  else
  {
    if (GetTeamsList().playing_list.size() <= 1)
    {
      msg_box->NewMessage(Format(ngettext("There is only %i team.",
                                          "There are only %i teams.",
                                          GetTeamsList().playing_list.size()),
                                 GetTeamsList().playing_list.size()), c_red);
      goto error;
    }
    if (Network::GetInstance()->GetNbPlayersConnected() == 0)
    {
      msg_box->NewMessage(_("You are alone. :-/"), c_red);
      goto error;
    }
    if (Network::GetInstance()->GetNbPlayersConnected() != Network::GetInstance()->GetNbPlayersWithState(Player::STATE_INITIALIZED))
    {
      int nbr = Network::GetInstance()->GetNbPlayersConnected() - Network::GetInstance()->GetNbPlayersWithState(Player::STATE_INITIALIZED);
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

    if (Network::GetInstance()->IsServer())
      IndexServer::GetInstance()->Disconnect();

    if (Network::GetInstance()->IsGameMaster())
      GameMode::GetInstance()->Load();

    Game::GetInstance()->Start();

    if (Network::GetInstance()->IsConnected()
        && Network::GetInstance()->GetNbPlayersConnected() != 0
        && play_in_loop->GetValue()) {
      PrepareForNewGame();
      return false;
    }

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
  if (msg_box->TextHasFocus())
  {
    msg_box->SendChatMsg();
    return;
  }

  IndexServer::GetInstance()->Disconnect();

  Menu::key_ok();
}

bool NetworkMenu::signal_cancel()
{
  Network::Disconnect();
  return true;
}

void NetworkMenu::Draw(const Point2i& /*mousePosition*/)
{
  if (Network::GetInstance()->IsConnected())
  {
    //Refresh the number of connected players:
    int nbr = Network::GetInstance()->GetNbPlayersConnected() + 1;
    std::string pl = Format(ngettext("%i player connected", "%i players connected", nbr), nbr);
    if (connected_players->GetText() != pl)
      connected_players->SetText(pl);

    if (Network::GetInstance()->IsGameMaster()) {
      //Refresh the number of players ready (does not work on client):
      nbr = Network::GetInstance()->GetNbPlayersWithState(Player::STATE_INITIALIZED);
      if (Network::GetInstance()->GetState() == WNet::NETWORK_MENU_OK)
        nbr++;

      pl = Format(ngettext("%i player ready", "%i players ready", nbr), nbr);
      if (initialized_players->GetText() != pl) {
        initialized_players->SetText(pl);
        msg_box->NewMessage(pl, c_red);
        if (Network::GetInstance()->GetNbPlayersConnected() ==
            Network::GetInstance()->GetNbPlayersWithState(Player::STATE_INITIALIZED)) {
          msg_box->NewMessage(_("The others are waiting for you! Wake up! :-)"), c_red);
          JukeBox::GetInstance()->Play("default", "menu/newcomer");
        }
        else if (Network::GetInstance()->GetNbPlayersConnected() == 0) {
          msg_box->NewMessage(_("You are alone. :-/"), c_red);
        }
      }
    }
  }
  else {
    close_menu = true;
  }
  ActionHandler * action_handler = ActionHandler::GetInstance();
  action_handler->ExecFrameLessActions();
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
}

void NetworkMenu::UpdateTeamCallback(const std::string& old_team_id, const std::string& team_id)
{
  if ( close_menu )
    return;

  team_box->UpdateTeamCallback(old_team_id, team_id);
}

void NetworkMenu::ChangeMapCallback()
{
  if (close_menu)
    return;

  map_box->ChangeMapCallback();
}

void NetworkMenu::SetGameMasterCallback()
{
  // We are becoming game master, updating the menu...
  AppWarmux::GetInstance()->video->SetWindowCaption( std::string("Warmux ") +
                                                     Constants::WARMUX_VERSION + " - " +
                                                     _("Master mode"));
  mode_label->SetText(_("Master mode"));
  connected_players->SetVisible(true);
  initialized_players->SetVisible(true);
  map_box->AllowSelection();
  b_ok->SetVisible(true); // make sure OK button is available if we had already clicked it
  waiting_for_server = false;
  msg_box->NewMessage(_("You are the new turn master!"), c_red);
  msg_box->NewMessage(_("Wait until some opponent(s) connect!"), c_red);

}

void NetworkMenu::ReceiveMsgCallback(const std::string& msg, const Color& color)
{
  msg_box->NewMessage(msg, color);
}

Team * NetworkMenu::FindUnusedTeam(const std::string default_team_id)
{
  return team_box->FindUnusedTeam(default_team_id);
}

bool NetworkMenu::HasOpenTeamSlot()
{
  return team_box->HasOpenTeamSlot();
}

// to be call from NetworkMenu::WaitingForGameMaster()
void NetworkMenu::HandleEvent(const SDL_Event& evnt)
{
  if (!waiting_for_server) {
    Menu::HandleEvent(evnt);
    return;
  }

  if (evnt.type == SDL_QUIT) {
    Menu::mouse_cancel();
  } else if (evnt.type == SDL_KEYDOWN) {
    switch (evnt.key.keysym.sym) {
    case SDLK_ESCAPE:
      Menu::mouse_cancel();
      break;
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
      msg_box->SendChatMsg();
      break;
    case SDLK_F10:
      AppWarmux::GetInstance()->video->ToggleFullscreen();
      break;
    default:
      widgets.SendKey(evnt.key.keysym);
      break;
    }
  } else if (evnt.type == SDL_MOUSEBUTTONUP) {

    int x=0, y=0;
    SDL_GetMouseState( &x, &y );
    Point2i mousePosition(x, y);

    if (b_cancel->Contains(mousePosition))
      Menu::mouse_cancel();
  }
}

void NetworkMenu::WaitingForGameMaster()
{
  Network::GetInstance()->SetState(WNet::NETWORK_MENU_OK);

  // warn the server that we have validated the menu
  Network::GetInstance()->SendNetworkState();

  waiting_for_server = true;
  b_ok->SetVisible(false);
  actions_buttons->NeedRedrawing();

  msg_box->NewMessage(_("Waiting for the server. All you can do is cancel or chat!"), c_red);

  widgets.SetFocusOn(msg_box->GetTextBox());

  do {
    HandleEvents();

    int x=0, y=0;
    SDL_GetMouseState( &x, &y );
    Point2i mousePosition(x, y);

    Display(mousePosition);
    Menu::Display(mousePosition);

  } while (Network::GetInstance()->IsConnected() &&
           Network::GetInstance()->GetState() == WNet::NETWORK_MENU_OK);

  waiting_for_server = false;
}
