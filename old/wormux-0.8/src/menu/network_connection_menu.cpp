/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
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


#include "menu/network_connection_menu.h"
#include "menu/network_menu.h"

#include "game/config.h"
#include "graphic/video.h"
#include "gui/button.h"
#include "gui/box.h"
#include "gui/check_box.h"
#include "gui/msg_box.h"
#include "gui/list_box.h"
#include "gui/picture_widget.h"
#include "gui/tabs.h"
#include "gui/text_box.h"
#include "include/app.h"
#include "network/net_error_msg.h"
#include "network/index_server.h"
#include "team/teams_list.h"
#include "tool/i18n.h"
#include "tool/resource_manager.h"

class GameInfoBox : public HBox
{
public:
  bool password;
  std::string port, ip_address;
  GameInfoBox(uint width, bool pwd, const std::string& ip, const std::string& p,
              const std::string& dns, const std::string& name)
    : HBox(width, true, false)
    , password(pwd)
    , port(p)
    , ip_address(ip)
  {
    AddWidget(new PictureWidget(Point2i(16, 16),
                                (pwd) ? "menu/disabled_back" : "menu/enabled", true));
    AddWidget(new Label(ip, 100));
    AddWidget(new Label(p, 40));
    AddWidget(new Label(dns, 340));
    AddWidget(new Label(name, 200));
    Pack();
  }
  void Draw(const Point2i &mousePosition) const
  {
    HBox::Draw(mousePosition);
    for (std::list<Widget*>::const_iterator it = widget_list.begin(); it != widget_list.end(); ++it)
      (*it)->Draw(mousePosition);
  }
};

class GameListBox : public BaseListBox
{
public:
  GameListBox(const Point2i &size, bool b = true) : BaseListBox(size, b) { }
  void Select(uint index) { BaseListBox::Select(index); }
  void AddItem(bool selected, bool pwd, const std::string& ip_address,
               const std::string& port, const std::string& dns, const std::string& name)
  {
    AddWidgetItem(selected, new GameInfoBox(600, pwd, ip_address, port, dns, name));
  }
  const std::string& GetAddress() { return ((GameInfoBox*)m_items[selected_item])->ip_address; }
  const std::string& GetPort() { return ((GameInfoBox*)m_items[selected_item])->port; }
};

NetworkConnectionMenu::NetworkConnectionMenu() :
  Menu("menu/bg_network", vOkCancel)
{
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml",false);
  Point2i def_size(300, 20);

  uint max_width = AppWormux::GetInstance()->video->window.GetWidth()-50;

  /* Tabs */
  tabs = new MultiTabs(Point2i(max_width,
			       AppWormux::GetInstance()->video->window.GetHeight()-180));
  tabs->SetPosition(25, 25);

  /* server connection related widgets */
  Box * srv_connection_box = new VBox(W_UNDEF, false, false);
  srv_connection_box->SetBorder(Point2i(0,0));

  // Server port
  Box * srv_tmp_box = new HBox(W_UNDEF, false, false);
  srv_tmp_box->SetMargin(0);
  srv_tmp_box->SetBorder(Point2i(0,0));

  srv_tmp_box->AddWidget(new Label(_("Port:"), def_size.x/2));
  srv_port_number = new TextBox(Config::GetInstance()->GetNetworkPort(), def_size.x/2);
  srv_tmp_box->AddWidget(srv_port_number);

  srv_connection_box->AddWidget(srv_tmp_box);

  // Game name
  srv_tmp_box = new HBox(W_UNDEF, false, false);
  srv_tmp_box->SetMargin(0);
  srv_tmp_box->SetBorder(Point2i(0,0));

  srv_tmp_box->AddWidget(new Label(_("Game name:"), def_size.x/2));
  srv_game_name = new TextBox("Wormux party", def_size.x/2);
  srv_game_name->SetMaxNbChars(15);
  srv_tmp_box->AddWidget(srv_game_name);

  srv_connection_box->AddWidget(srv_tmp_box);

  // Server password
  srv_tmp_box = new HBox(W_UNDEF, false, false);
  srv_tmp_box->SetMargin(0);
  srv_tmp_box->SetBorder(Point2i(0,0));

  srv_tmp_box->AddWidget(new Label(_("Password:"), def_size.x/2));
  srv_game_pwd = new PasswordBox("", def_size.x/2);
  srv_game_pwd->SetMaxNbChars(15);
  srv_tmp_box->AddWidget(srv_game_pwd);

  srv_connection_box->AddWidget(srv_tmp_box);

  // Available on internet ?
  srv_internet_server = new CheckBox(_("Server available on Internet"), def_size.x, true);
  srv_connection_box->AddWidget(srv_internet_server);

  tabs->AddNewTab("TAB_server", _("Host a game"), srv_connection_box);

  /* client connection related widgets */
  Box * cl_connection_box = new VBox(W_UNDEF, false, false);
  cl_connection_box->SetBorder(Point2i(0,0));

  // Public battles
  Box * cl_tmp_box = new HBox(W_UNDEF, false, false);
  cl_tmp_box->SetMargin(0);
  cl_tmp_box->SetBorder(Point2i(0,0));

  cl_refresh_net_games = new Button(res, "menu/refresh_small", false);
  cl_tmp_box->AddWidget(cl_refresh_net_games);
  cl_tmp_box->AddWidget(new Label(_("Public battles"), def_size.x, Font::FONT_MEDIUM, Font::FONT_BOLD, c_red));
  cl_connection_box->AddWidget(cl_tmp_box);

  cl_net_games_lst = new GameListBox( Point2i(def_size.x, 30), false);
  cl_connection_box->AddWidget(cl_net_games_lst);

  // Server password
  cl_tmp_box = new HBox(W_UNDEF, false, false);
  cl_tmp_box->SetMargin(0);
  cl_tmp_box->SetBorder(Point2i(0,0));

  cl_tmp_box->AddWidget(new Label(_("Password:"), def_size.x/2));
  cl_net_server_pwd = new PasswordBox("", def_size.x/2);
  cl_tmp_box->AddWidget(cl_net_server_pwd);
  cl_connection_box->AddWidget(cl_tmp_box);

  // #####################################
  // Manual connection
  cl_connection_box->AddWidget(new Label(_("Manual connection"), def_size.x, Font::FONT_MEDIUM, Font::FONT_BOLD, c_red));

  // Server address
  cl_tmp_box = new HBox(W_UNDEF, false, false);
  cl_tmp_box->SetMargin(0);
  cl_tmp_box->SetBorder(Point2i(0,0));

  cl_tmp_box->AddWidget(new Label(_("Server address:"), def_size.x/2));
  cl_server_address = new TextBox(Config::GetInstance()->GetNetworkHost(), def_size.x/2);
  cl_tmp_box->AddWidget(cl_server_address);

  cl_connection_box->AddWidget(cl_tmp_box);

  // Server port
  cl_tmp_box = new HBox(W_UNDEF, false, false);
  cl_tmp_box->SetMargin(0);
  cl_tmp_box->SetBorder(Point2i(0,0));

  cl_tmp_box->AddWidget(new Label(_("Port:"), def_size.x/2));
  cl_port_number = new TextBox(Config::GetInstance()->GetNetworkPort(), def_size.x/2);
  cl_tmp_box->AddWidget(cl_port_number);

  cl_connection_box->AddWidget(cl_tmp_box);

  // Server password
  cl_tmp_box = new HBox(W_UNDEF, false, false);
  cl_tmp_box->SetMargin(0);
  cl_tmp_box->SetBorder(Point2i(0,0));

  cl_tmp_box->AddWidget(new Label(_("Password:"), def_size.x/2));
  cl_server_pwd = new PasswordBox("", def_size.x/2);
  cl_tmp_box->AddWidget(cl_server_pwd);

  cl_connection_box->AddWidget(cl_tmp_box);
  tabs->AddNewTab("TAB_client", _("Connect to game"), cl_connection_box);

  widgets.AddWidget(tabs);
  widgets.Pack();

  // Optimize height of public net games table
  uint net_games_lst_width = tabs->GetSizeX() - 2*(cl_connection_box->GetPositionX()-tabs->GetPositionX());
  uint net_games_lst_height = cl_net_games_lst->GetSizeY() +
    (tabs->GetPositionY() + tabs->GetSizeY()) - (cl_connection_box->GetPositionY() + cl_connection_box->GetSizeY()) - 10;
  cl_net_games_lst->SetSize(net_games_lst_width, net_games_lst_height);

  // Warning about experimental networking
  Point2i msg_box_pos(25, tabs->GetPositionY() + tabs->GetSizeY() + 10);
  Point2i msg_box_size(max_width,
		       AppWormux::GetInstance()->video->window.GetHeight() - 50 - msg_box_pos.y);

  msg_box = new MsgBox(msg_box_size, Font::FONT_SMALL, Font::FONT_NORMAL);
  msg_box->SetPosition(msg_box_pos);

  widgets.AddWidget(msg_box);
  widgets.Pack();

  msg_box->NewMessage(_("Join #wormux on irc.freenode.net to find some opponents."));
  msg_box->NewMessage(""); // Skip a line
  msg_box->NewMessage(_("Have a good game!"));
  msg_box->NewMessage(""); // Skip a line

  resource_manager.UnLoadXMLProfile(res);
}

NetworkConnectionMenu::~NetworkConnectionMenu()
{
}

void NetworkConnectionMenu::OnClickUp(const Point2i &mousePosition, int button)
{
  Widget* w = widgets.ClickUp(mousePosition, button);

  if (w == cl_refresh_net_games)
    RefreshList();
  else if (w == cl_net_games_lst)
    printf("%s\n", cl_net_games_lst->GetAddress().c_str());
}

void NetworkConnectionMenu::OnClick(const Point2i &mousePosition, int button)
{
  widgets.Click(mousePosition, button);
}

void NetworkConnectionMenu::RefreshList()
{

  // Save the currently selected address
  //int current = cl_net_games_lst->GetSelectedItem();

  // Empty the list:
  while (cl_net_games_lst->Size() != 0)
  {
    cl_net_games_lst->Select(0);
    cl_net_games_lst->RemoveSelected();
  }

  // Connect to the index server
  connection_state_t conn = IndexServer::GetInstance()->Connect();
  if (conn != CONNECTED) {
    DisplayNetError(conn);
    msg_box->NewMessage(_("Error: Unable to contact index server to search an internet game"), c_red);
    return;
  }

  std::list<GameServerInfo> lst = IndexServer::GetInstance()->GetHostList();

  if (lst.size() == 0) {
    Menu::DisplayError(_("Sorry, currently, no game is waiting for players"));
    IndexServer::GetInstance()->Disconnect();
    return;
  }

  for (std::list<GameServerInfo>::iterator it = lst.begin(); it != lst.end(); ++it) {
    cl_net_games_lst->AddItem(false, it->passworded, it->ip_address,
                              it->port, it->dns_address, it->game_name);
  }
  /* if (current != -1 && cl_net_games_lst->Size() != 0)
     cl_net_games_lst->Select( current ); */
  cl_net_games_lst->NeedRedrawing();

  IndexServer::GetInstance()->Disconnect();
}

void NetworkConnectionMenu::Draw(const Point2i &/*mousePosition*/){}

void NetworkConnectionMenu::DisplayNetError(connection_state_t conn)
{
  Menu::DisplayError(NetworkErrorToString(conn));
}

bool NetworkConnectionMenu::HostingServer(const std::string& port,
					  const std::string& game_name,
                                          const std::string& password,
					  bool internet)
{
  bool r = false;

  if (!internet)
    IndexServer::GetInstance()->SetHiddenServer();

  connection_state_t conn = IndexServer::GetInstance()->Connect();
  if (conn != CONNECTED)
    {
      DisplayNetError(conn);
      msg_box->NewMessage(_("Error: Unable to contact index server to host a game"), c_red);
      goto out;
    }

  conn = Network::GetInstance()->ServerStart(port, password);
  if( conn != CONNECTED)
    {
      DisplayNetError(conn);
      goto out;
    }

  r = IndexServer::GetInstance()->SendServerStatus(game_name, password != "");
  if (false == r) {
    DisplayNetError(CONN_BAD_PORT);
    msg_box->NewMessage(_("Error: Your server is not reachable from the internet. Check your firewall configuration")
			, c_red);
    goto out;
  }

  if (!Network::GetInstance()->IsConnected()) {
    msg_box->NewMessage(_("Error: Unable to start server"), c_red);
    goto out;
  }
  r = true;

 out:
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
			       srv_address.c_str(), port.c_str()),
			c_red);
    r = false;
  }
  return r;
}

bool NetworkConnectionMenu::signal_ok()
{
  bool r = false;

  // Hack: force loading of teams before creating threads.
  GetTeamsList();

  // Which tab is displayed ?
  std::string id = tabs->GetCurrentTabId();

  if (id == "TAB_server") {
    // Hosting your own server
    r = HostingServer(srv_port_number->GetText(),
		      srv_game_name->GetText(),
		      srv_game_pwd->GetPassword(),
		      srv_internet_server->GetValue());
    if (!r)
      goto out;
  } else if (id == "TAB_client") {

    if (cl_net_games_lst->GetSelectedItem() != -1)
      {
	// Connect to an internet game!
	r = ConnectToClient(cl_net_games_lst->GetAddress(),
			    cl_net_games_lst->GetPort(),
			    cl_net_server_pwd->GetPassword());
	if (!r)
	  goto out;
      }
    else if (!cl_server_address->GetText().empty())
      {
	// Direct connexion to a server
	r = ConnectToClient(cl_server_address->GetText(),
                              cl_port_number->GetText(),
                              cl_server_pwd->GetPassword());
	if (!r)
	  goto out;

	// Remember the parameters
	Config::GetInstance()->SetNetworkHost(cl_server_address->GetText());
	Config::GetInstance()->SetNetworkPort(cl_port_number->GetText());
      }
    else
      goto out;
  }

  if (Network::IsConnected()) {
    play_ok_sound();

    // run the network menu ! :-)
    NetworkMenu nm;
    Network::GetInstance()->network_menu = &nm;
    nm.Run();
    Network::GetInstance()->network_menu = NULL;
    IndexServer::GetInstance()->Disconnect();

    // back to main menu after playing
    Network::Disconnect();
    return true;
  }

  r = true;
 out:
  Network::Disconnect();
  return r;
}

bool NetworkConnectionMenu::signal_cancel()
{
  Network::Disconnect();
  return true;
}

