/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
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
 * Network connection menu: this menu allows the user to choose between
 * hosting a game or connecting to a server.
 *****************************************************************************/
#include "game/config.h"
#include "graphic/video.h"
#include "gui/button.h"
#include "gui/box.h"
#include "gui/vertical_box.h"
#include "gui/check_box.h"
#include "gui/msg_box.h"
#include "gui/null_widget.h"
#include "gui/picture_widget.h"
#include "gui/select_box.h"
#include "gui/tabs.h"
#include "gui/text_box.h"
#include "include/app.h"
#include "include/constant.h"
#include "network/net_error_msg.h"
#include "menu/network_connection_menu.h"
#include "menu/network_menu.h"
#include "team/teams_list.h"
#include "tool/resource_manager.h"
#include "tool/string_tools.h"
#ifdef HAVE_LIBCONIC
#include "maemo/conic.h"

static bool network_interface_up = false;
#endif

class GameInfoBox : public HBox
{
public:
  bool password;
  std::string port, ip_address;
  GameInfoBox(uint width, bool pwd, const std::string& ip,
              const std::string& port, const std::string& name)
    : HBox(width, false, false, false)
    , password(pwd)
    , port(port)
    , ip_address(ip)
  {
    if (pwd) {
      AddWidget(new PictureWidget(Point2i(16, 16), "menu/password_lock",
                                  PictureWidget::FIT_SCALING));
    } else {
      AddWidget(new NullWidget(Point2i(16, 16)));
    }
    width -= 20;
    AddWidget(new Label(ip, 0.3f*width));
    AddWidget(new Label(port, 0.2f*width));
    AddWidget(new Label(name, 0.5f*width));
    SetNoBorder();
    Pack();
  }
};

class GameListBox : public SelectBox
{
public:
  GameListBox(const Point2i &size) : SelectBox(size, false) { }
  void Select(uint index) { SelectBox::Select(index); }
  void AddItem(bool selected, bool pwd, const std::string& ip_address,
               const std::string& port, const std::string& name)
  {
    AddWidgetItem(selected, new GameInfoBox(size.x-10, pwd, ip_address, port, name));
  }
  const std::string& GetAddress() { return ((GameInfoBox*)GetSelectedWidget())->ip_address; }
  const std::string& GetPort() { return ((GameInfoBox*)GetSelectedWidget())->port; }
};

struct shared_net_info {
  SDL_Thread* thread_refresh;
  SDL_sem* lock;
  std::list<GameServerInfo> lst_games;
  connection_state_t index_conn_state;
  bool todisplay;
  bool finished;
} net_info;

static void InitNetInfo()
{
  net_info.thread_refresh = NULL;
  net_info.index_conn_state = CONNECTED;
  net_info.lock = SDL_CreateSemaphore(1);
  net_info.todisplay = false;
  net_info.finished = true;
}

static void CleanNetInfo()
{
  SDL_SemWait(net_info.lock);
  SDL_DestroySemaphore(net_info.lock);
  int status;
  if (net_info.thread_refresh)
    SDL_WaitThread(net_info.thread_refresh, &status);
  net_info.thread_refresh = NULL;
  net_info.finished = true;
  net_info.lst_games.clear();
}

static int RefreshNetInfo(void *)
{
  MSG_DEBUG("network.refresh_games_list", "Begin");

  // Connect to the index server
  connection_state_t conn = IndexServer::GetInstance()->Connect(Constants::WARMUX_VERSION);
  if (conn != CONNECTED) {
    SDL_SemWait(net_info.lock);
    net_info.index_conn_state = conn;
    net_info.finished = true;
    SDL_SemPost(net_info.lock);
    return -1;
  }

  std::list<GameServerInfo> lst = IndexServer::GetInstance()->GetHostList(false);
  IndexServer::GetInstance()->Disconnect();

  SDL_SemWait(net_info.lock);
  net_info.lst_games = lst;
  net_info.todisplay = true;
  net_info.finished = true;
  SDL_SemPost(net_info.lock);

  // make sure the list will be updated right now
  Menu::WakeUpOnCallback();

  MSG_DEBUG("network.refresh_games_list", "End");
  return 0;
}

#ifdef HAVE_LIBCONIC
static void NetworkInterfaceEvent(bool new_network_interface_up)
{
  printf("RestartNetwork\n");
  if (!network_interface_up && new_network_interface_up) {
    network_interface_up = new_network_interface_up;
    NetworkConnectionMenu::ThreadRefreshList();
  } else if (network_interface_up && !new_network_interface_up) {
    CleanNetInfo();
    if (IndexServer::GetInstance()->IsConnected())
      IndexServer::GetInstance()->Disconnect();
    if (Network::IsConnected())
      Network::Disconnect();
    InitNetInfo();
  }

  network_interface_up = new_network_interface_up;
}
#endif

#define TAB_MANUAL_ID   "TAB_manual"
#define TAB_CLIENT_ID   "TAB_client"
#define TAB_SERVER_ID   "TAB_server"

NetworkConnectionMenu::NetworkConnectionMenu(network_menu_action_t action) :
  Menu("menu/bg_network", vOkCancel)
{
  Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml",false);

  uint max_width = 0.95f*GetMainWindow().GetWidth();
  uint offset    = (GetMainWindow().GetWidth() - max_width)/2;
  float zoom     = GetMainWindow().GetWidth() / 640.0;
  uint border    = 5*zoom+0.5f;
  uint width     = max_width - 2*(border + 5); // 5 is the tab default margin
  Font::font_size_t fsmall  = Font::GetFixedSize(Font::FONT_SMALL*zoom);
  Font::font_size_t fmedium = Font::GetFixedSize(Font::FONT_MEDIUM*zoom);
  Font::font_size_t fadapt  = (fmedium > Font::FONT_BIG) ? Font::FONT_BIG : fmedium;

#ifdef HAVE_LIBCONIC
  Conic::ConnectionConnect(NetworkInterfaceEvent);
#endif
  /* Tabs */
  tabs = new MultiTabs(Point2i(max_width,
                               GetMainWindow().GetHeight()*0.6f), fadapt);
  tabs->SetPosition(offset, offset);

  // #############################
  /* client connection related widgets */
  Box * cl_connection_box = new VBox(width, false, false, true);
  cl_connection_box->SetBorder(border);

  // Public battles
  Box * cl_tmp_box = new HBox(W_UNDEF, false, false, false);
  cl_tmp_box->SetMargin(0);
  cl_tmp_box->SetNoBorder();

  cl_refresh_net_games = new Button(res, "menu/refresh_small", false);
  cl_tmp_box->AddWidget(cl_refresh_net_games);
  refresh_net_games_label = new Label(_("Public battles"), width - cl_refresh_net_games->GetSizeX(),
                                      fmedium, Font::FONT_BOLD, c_red,
                                      Text::ALIGN_LEFT_TOP, true);
  cl_tmp_box->AddWidget(refresh_net_games_label);
  cl_connection_box->AddWidget(cl_tmp_box);

  cl_net_games_lst = new GameListBox(Point2i(width-2*border, 30*zoom));
  cl_connection_box->AddWidget(cl_net_games_lst);

  // Server password
  cl_tmp_box = new HBox(W_UNDEF, false, false, false);
  cl_tmp_box->SetMargin(0);
  cl_tmp_box->SetNoBorder();

  cl_tmp_box->AddWidget(new Label(_("Password:"), width/3, fsmall));
  cl_net_server_pwd = new PasswordBox("", (2*width)/3, fsmall);
  cl_tmp_box->AddWidget(cl_net_server_pwd);

  cl_connection_box->AddWidget(cl_tmp_box);

  // #############################
  // Manual connection
  Box *manual_connection_box = new VBox(W_UNDEF, false, false, false);
  manual_connection_box->SetBorder(border);

  // Server address
  cl_tmp_box = new HBox(W_UNDEF, false, false, false);
  cl_tmp_box->SetMargin(0);
  cl_tmp_box->SetNoBorder();

  cl_tmp_box->AddWidget(new Label(_("Server address:"), width/3, fsmall));
  cl_server_address = new TextBox(Config::GetInstance()->GetNetworkClientHost(), (2*width)/3, fsmall);
  cl_tmp_box->AddWidget(cl_server_address);
  manual_connection_box->AddWidget(cl_tmp_box);

  // Server port
  cl_tmp_box = new HBox(W_UNDEF, false, false, false);
  cl_tmp_box->SetMargin(0);
  cl_tmp_box->SetNoBorder();

  cl_tmp_box->AddWidget(new Label(_("Port:"), width/3, fsmall));
  cl_port_number = new TextBox(Config::GetInstance()->GetNetworkClientPort(), (2*width)/3, fsmall);
  cl_tmp_box->AddWidget(cl_port_number);

  manual_connection_box->AddWidget(cl_tmp_box);

  // Server password
  cl_tmp_box = new HBox(W_UNDEF, false, false, false);
  cl_tmp_box->SetMargin(0);
  cl_tmp_box->SetNoBorder();

  cl_tmp_box->AddWidget(new Label(_("Password:"), width/3, fsmall));
  cl_server_pwd = new PasswordBox("", (2*width)/3, fsmall);
  cl_tmp_box->AddWidget(cl_server_pwd);

  manual_connection_box->AddWidget(cl_tmp_box);

  // #############################
  /* server connection related widgets */
  Box * srv_connection_box = new VBox(W_UNDEF, false, false, false);
  srv_connection_box->SetBorder(border);

  // Server port
  Box * srv_tmp_box = new HBox(W_UNDEF, false, false, false);
  srv_tmp_box->SetMargin(0);
  srv_tmp_box->SetNoBorder();

  srv_tmp_box->AddWidget(new Label(_("Port:"), width/3, fsmall));
  srv_port_number = new TextBox(Config::GetInstance()->GetNetworkServerPort(), (2*width)/3, fsmall);
  srv_tmp_box->AddWidget(srv_port_number);

  srv_connection_box->AddWidget(srv_tmp_box);

  // Game name
  srv_tmp_box = new HBox(W_UNDEF, false, false, false);
  srv_tmp_box->SetMargin(0);
  srv_tmp_box->SetNoBorder();

  srv_tmp_box->AddWidget(new Label(_("Game name:"), width/3, fsmall));
  srv_game_name = new TextBox(Config::GetInstance()->GetNetworkServerGameName(), (2*width)/3, fsmall);
  srv_game_name->SetMaxNbChars(15);
  srv_tmp_box->AddWidget(srv_game_name);

  srv_connection_box->AddWidget(srv_tmp_box);

  // Server password
  srv_tmp_box = new HBox(W_UNDEF, false, false, false);
  srv_tmp_box->SetMargin(0);
  srv_tmp_box->SetNoBorder();

  srv_tmp_box->AddWidget(new Label(_("Password:"), width/3, fsmall));
  srv_game_pwd = new PasswordBox("", (2*width)/3, fsmall);
  srv_game_pwd->SetMaxNbChars(15);
  srv_tmp_box->AddWidget(srv_game_pwd);

  srv_connection_box->AddWidget(srv_tmp_box);

  // Available on internet ?
  srv_internet_server = new CheckBox(_("Server available on Internet"), width,
                                     Config::GetInstance()->GetNetworkServerPublic(), fsmall);
  srv_connection_box->AddWidget(srv_internet_server);

  tabs->AddNewTab(TAB_CLIENT_ID, _("Connect to game"), cl_connection_box);
  tabs->AddNewTab(TAB_MANUAL_ID, _("Manual connection"), manual_connection_box);
  tabs->AddNewTab(TAB_SERVER_ID, _("Host a game"), srv_connection_box);

  // #############################
  widgets.AddWidget(tabs);
  widgets.Pack();

  // Optimize height of public net games table
  uint net_games_lst_width = tabs->GetSizeX() - 2*(cl_connection_box->GetPositionX()-tabs->GetPositionX());
  uint net_games_lst_height = cl_net_games_lst->GetSizeY() +
    (tabs->GetPositionY() + tabs->GetSizeY()) -
    (cl_connection_box->GetPositionY() + cl_connection_box->GetSizeY()) - 10;
  cl_net_games_lst->SetSize(net_games_lst_width, net_games_lst_height);

  // Warning about experimental networking
  Point2i msg_box_pos(offset, tabs->GetPositionY() + tabs->GetSizeY() + 10);
  Point2i msg_box_size(max_width,
                       GetMainWindow().GetHeight() - 50 - msg_box_pos.y);

  msg_box = new MsgBox(msg_box_size, fsmall, Font::FONT_BOLD);
  msg_box->SetPosition(msg_box_pos);

  widgets.AddWidget(msg_box);
  widgets.Pack();

  msg_box->NewMessage(_("Have a good game!"));
#ifndef HAVE_LIBCURL
  msg_box->NewMessage(_("This version was compiled without curl, you can't list public games!"));
#endif
  msg_box->NewMessage(""); // Skip a line

  //Double click
  m_last_click_on_games_lst = 0;
  m_Double_click_interval = 400;
  // ************************************************************************
  InitNetInfo();

  switch (action) {
  case NET_HOST:
    tabs->SelectTab(1);
    break;
  case NET_CONNECT:
    tabs->SelectTab(0);
    break;
  default:
    ThreadRefreshList();
    break;
  }
}

NetworkConnectionMenu::~NetworkConnectionMenu()
{
  CleanNetInfo();
}

void NetworkConnectionMenu::OnClickUp(const Point2i &mousePosition, int button)
{
  Widget* w = widgets.ClickUp(mousePosition, button);

  if (w == cl_refresh_net_games || w == refresh_net_games_label)
    ThreadRefreshList();

  if (w == cl_server_address || w == cl_port_number || w == cl_server_pwd)
    cl_net_games_lst->Deselect();

  //Hack to handle Double click
  if (w == cl_net_games_lst) {
    if (m_last_click_on_games_lst + m_Double_click_interval > SDL_GetTicks()) {
      if (cl_net_games_lst->GetSelectedItem() == -1) {
        cl_net_games_lst->Select(cl_net_games_lst->MouseIsOnWhichItem(mousePosition));
      }
      signal_ok();
    } else {
      m_last_click_on_games_lst = SDL_GetTicks();
    }
  }
}

void NetworkConnectionMenu::__RefreshList()
{
  SDL_SemWait(net_info.lock);
  if (!net_info.todisplay) {
    SDL_SemPost(net_info.lock);
    return;
  }

  net_info.todisplay = false;

  // Save the currently selected address
  int current = cl_net_games_lst->GetSelectedItem();
  if (current == -1)
    current = 0;

  // Empty the list:
  cl_net_games_lst->Clear();

  if (net_info.index_conn_state != CONNECTED) {
    DisplayNetError(net_info.index_conn_state);
    SDL_SemPost(net_info.lock);
    msg_box->NewMessage(_("Error: Unable to contact the index server to search for an internet game"), c_red);
    return;
  }

  if (net_info.lst_games.empty()) {
    Menu::DisplayError(_("Sorry, currently, no game is waiting for players"));
    SDL_SemPost(net_info.lock);
    return;
  }

  for (std::list<GameServerInfo>::iterator it = net_info.lst_games.begin();
       it != net_info.lst_games.end(); ++it) {
    cl_net_games_lst->AddItem(false, it->passworded, it->ip_address,
                              it->port, it->game_name);
  }
  SDL_SemPost(net_info.lock);

  if (cl_net_games_lst->Size()) {
    if (current+1 > cl_net_games_lst->Size()) {
      current = 0;
    }
    cl_net_games_lst->Select(current);
  }

  cl_net_games_lst->Pack();
}

void NetworkConnectionMenu::ThreadRefreshList()
{
#ifdef HAVE_LIBCONIC
  if (!network_interface_up)
    return;
#endif
  SDL_SemWait(net_info.lock);

  if (net_info.thread_refresh != NULL) {
    // Check if the thread finished
    if (net_info.finished) {
      int status;
      SDL_WaitThread(net_info.thread_refresh, &status);
    } else {
      MSG_DEBUG("network.refresh_games_list", "A thread is already running");
      SDL_SemPost(net_info.lock);
      return;
    }
  }
  SDL_SemPost(net_info.lock);

  net_info.finished = false;
  net_info.thread_refresh = SDL_CreateThread(RefreshNetInfo, NULL);
}

void NetworkConnectionMenu::HandleEvent(const SDL_Event& evnt)
{
  __RefreshList();
  Menu::HandleEvent(evnt);
}

void NetworkConnectionMenu::DisplayNetError(connection_state_t conn)
{
  std::string error_msg;

  if (conn == CONN_WRONG_VERSION) {
    error_msg = Format(_("Sorry, your version is not supported anymore. "
                         "Supported versions are %s. "
                         "You can download an updated version "
                         "from http://www.wormux.org/wiki/download.php"),
                       IndexServer::GetInstance()->GetSupportedVersions().c_str());
  } else {
    error_msg = NetworkErrorToString(conn);
  }

  Menu::DisplayError(error_msg);
}

bool NetworkConnectionMenu::HostingServer(const std::string& port,
                                          const std::string& game_name,
                                          const std::string& password,
                                          bool internet)
{
  bool r = false;
  int net_port;
  connection_state_t conn;

  if (!internet)
    IndexServer::GetInstance()->SetHiddenServer();

  if (internet) {
    SDL_SemWait(net_info.lock);

    conn = IndexServer::GetInstance()->Connect(Constants::WARMUX_VERSION);
    if (conn != CONNECTED) {
      DisplayNetError(conn);
      msg_box->NewMessage(_("Error: Unable to contact the index server to host a game"), c_red);
      goto out;
    }
  }

  conn = Network::ServerStart(port, game_name, password);
  if (conn != CONNECTED) {
    DisplayNetError(conn);
    goto out;
  }

  r = str2int(port, net_port);
  if (false == r) {
    DisplayNetError(CONN_BAD_PORT);
    goto out;
  }

  if (internet) {
    r = IndexServer::GetInstance()->SendServerStatus(game_name, password != "", net_port);
    if (false == r) {
      DisplayNetError(CONN_BAD_PORT);
      msg_box->NewMessage(Format(_("Error: Your server is not reachable from the internet. Check your firewall configuration: TCP Port %s must accept connections from the outside. If you are not directly connected to the internet, check your router configuration: TCP Port %s must be forwarded on your computer."), port.c_str(), port.c_str()),
                          c_red);
      goto out;
    }
  }

  if (!Network::GetInstance()->IsConnected()) {
    msg_box->NewMessage(_("Error: Unable to start the server"), c_red);
    goto out;
  }
  r = true;

 out:
  if (internet)
    SDL_SemPost(net_info.lock);
  return r;
}

bool NetworkConnectionMenu::ConnectToClient(const std::string& srv_address,
                                            const std::string& port,
                                            const std::string& passwd)
{
  bool r = true;

  connection_state_t conn = Network::ClientStart(srv_address, port, passwd);
  if (!Network::IsConnected() || conn != CONNECTED) {
    DisplayNetError(conn);

    // translators: %s:%s will expand to something like "example.org:9999"
    msg_box->NewMessage(Format(_("Error: Unable to connect to %s:%s"),
                               srv_address.c_str(), port.c_str()), c_red);
    r = false;
  }
  return r;
}

bool NetworkConnectionMenu::signal_ok()
{
  bool r = false;
  std::list<GameServerInfo> lst;

  // Hack: force loading of teams before creating threads.
  GetTeamsList();

  // Which tab is displayed ?
  std::string id = tabs->GetCurrentTabId();

  if (id == TAB_SERVER_ID) {
    // Hosting your own server
    r = HostingServer(srv_port_number->GetText(),
                      srv_game_name->GetText(),
                      srv_game_pwd->GetPassword(),
                      srv_internet_server->GetValue());
    if (!r)
      goto out;

    // Remember the parameters
    Config::GetInstance()->SetNetworkServerPort(srv_port_number->GetText());
    Config::GetInstance()->SetNetworkServerGameName(srv_game_name->GetText());
    Config::GetInstance()->SetNetworkServerPublic(srv_internet_server->GetValue());

  } else if (id == TAB_CLIENT_ID) { // Direct connexion to a server

    if (cl_net_games_lst->GetSelectedItem() != -1) {
      // Connect to an internet game!
      r = ConnectToClient(cl_net_games_lst->GetAddress(),
                          cl_net_games_lst->GetPort(),
                          cl_net_server_pwd->GetPassword());
      if (!r)
        goto out;

    } else if (!cl_server_address->GetText().empty()) {
      r = ConnectToClient(cl_server_address->GetText(),
                          cl_port_number->GetText(),
                          cl_server_pwd->GetPassword());
      if (!r)
        goto out;

      // Remember the parameters
      Config::GetInstance()->SetNetworkClientHost(cl_server_address->GetText());
      Config::GetInstance()->SetNetworkClientPort(cl_port_number->GetText());
    } else
      goto out;
  } else if (id == TAB_MANUAL_ID) {
    if (!cl_server_address->GetText().empty()) {
      r = ConnectToClient(cl_server_address->GetText(),
                          cl_port_number->GetText(),
                          cl_server_pwd->GetPassword());
      if (!r)
        goto out;

      // Remember the parameters
      Config::GetInstance()->SetNetworkClientHost(cl_server_address->GetText());
      Config::GetInstance()->SetNetworkClientPort(cl_port_number->GetText());
    }
  }

  if (Network::IsConnected()) {
    play_ok_sound();

    // run the network menu ! :-)
    NetworkMenu nm;
    Network::GetInstance()->network_menu = &nm;
    nm.Run();
    Network::GetInstance()->network_menu = NULL;
    IndexServer::GetInstance()->Disconnect();

    Network::Disconnect();

    // Don't go back to main menu after playing
    Menu::RedrawMenu();
    ThreadRefreshList();

    return false;
  }

 out:
  Network::Disconnect();
  return r;
}

bool NetworkConnectionMenu::signal_cancel()
{
  IndexServer::GetInstance()->Disconnect();
  Network::Disconnect();
  return true;
}
