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
 * Network connection menu: this menu allows the user to choose between
 * hosting a game or connecting to a server.
 *****************************************************************************/


#include "network_connection_menu.h"
#include "network_menu.h"
#include "internet_menu.h"

#include "../game/game.h"
#include "../game/config.h"
#include "../game/game_mode.h"
#include "../graphic/video.h"
#include "../graphic/font.h"
#include "../map/maps_list.h"
#include "../network/network.h"
#include "../network/index_server.h"
#include "../include/app.h"
#include "../include/action_handler.h"
#include "../team/teams_list.h"
#include "../tool/i18n.h"
#include "../tool/string_tools.h"

NetworkConnectionMenu::NetworkConnectionMenu() :
  Menu("menu/bg_network", vOkCancel)
{
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml",false);
  Rectanglei rectZero(0, 0, 0, 0);
  
  Font * normal_font = Font::GetInstance(Font::FONT_NORMAL);
  Font * big_font = Font::GetInstance(Font::FONT_BIG);

  Rectanglei stdRect(0, 0, 300, 64);

  uint x_button = AppWormux::GetInstance()->video.window.GetWidth()/2 - stdRect.GetSizeX()/2;
  uint y_box = AppWormux::GetInstance()->video.window.GetHeight()/2 - 200;

  // Connection related widgets
  connection_box = new VBox(Rectanglei( x_button, y_box, stdRect.GetSizeX(), 1), false);
  connection_box->SetBorder(Point2i(0,0));

  // What do we want to do ?
  Box* action_box = new HBox(stdRect, false);

  previous_action_bt = new Button(Point2i(0, 0), res, "menu/big_minus", false);
  next_action_bt = new Button(Point2i(0, 0), res, "menu/big_plus", false);
  action_label = new Label(_("Connect to an internet game"), 
			   Rectanglei(0,0,250,0), 
			   *big_font, white_color, true);
  action_box->AddWidget(previous_action_bt);
  action_box->AddWidget(action_label);
  action_box->AddWidget(next_action_bt);

  connection_box->AddWidget(action_box);

  // Server address
  server_address_label = new Label(_("Server address:"), rectZero, *normal_font);
  connection_box->AddWidget(server_address_label);
  server_address = new TextBox("localhost", rectZero, *normal_font);
  connection_box->AddWidget(server_address);

  // Server port
  port_number_label = new Label(_("Port:"), rectZero, *normal_font);
  connection_box->AddWidget(port_number_label);
  port_number = new TextBox(WORMUX_NETWORK_PORT, rectZero, *normal_font);
  connection_box->AddWidget(port_number);

  // Available on internet ?
  internet_server = new CheckBox(_("Server available on Internet"),
				 Rectanglei(0,0,0,0),
				 true);
  connection_box->AddWidget(internet_server);

  widgets.AddWidget(connection_box);

  SetAction(NET_BROWSE_INTERNET);

  // Warning about experimental networking
  msg_box = new MsgBox(Rectanglei( AppWormux::GetInstance()->video.window.GetWidth()/2 - 300,
				   y_box+connection_box->GetSizeY() + 30, 
				   600, 200), 
		       Font::GetInstance(Font::FONT_SMALL));
  widgets.AddWidget(msg_box);

  msg_box->NewMessage(_("WARNING!! Network is still under developement and therefore a little experimental."), c_red);
  msg_box->NewMessage(""); // Skip a line
  msg_box->NewMessage(_("Some weapons are disabled, because of known bugs (ninjarope, airhammer, blowtorch, submachine gun) and surely many other things don't work either!"));
  msg_box->NewMessage(""); // Skip a line
  msg_box->NewMessage(_("Join #wormux on irc.freenode.net to find some opponents."));
  msg_box->NewMessage(""); // Skip a line
  msg_box->NewMessage(_("Have a good game!"));
  msg_box->NewMessage(""); // Skip a line

  resource_manager.UnLoadXMLProfile(res);
}

NetworkConnectionMenu::~NetworkConnectionMenu()
{
}

void NetworkConnectionMenu::OnClic(const Point2i &mousePosition, int button)
{
  Widget* w = widgets.Clic(mousePosition, button);

  if ((w == next_action_bt) ||
      (w == action_label && (button == SDL_BUTTON_LEFT || button == SDL_BUTTON_WHEELDOWN)))
    {
      switch (current_action) {
      case NET_HOST:
	SetAction(NET_CONNECT_LOCAL);
	break;
      case NET_CONNECT_LOCAL:
	SetAction(NET_BROWSE_INTERNET);
	break;
      case NET_BROWSE_INTERNET:
	SetAction(NET_HOST);
	break;
      }
      return;
    } 

  if ((w == previous_action_bt) ||
      (w == action_label && (button == SDL_BUTTON_RIGHT || button == SDL_BUTTON_WHEELUP)))
    {
      switch (current_action) {
      case NET_HOST:
	SetAction(NET_BROWSE_INTERNET);
	break;
      case NET_CONNECT_LOCAL:
	SetAction(NET_HOST);
	break;
      case NET_BROWSE_INTERNET:
	SetAction(NET_CONNECT_LOCAL);
	break;
      }
      return;
    }
}

void NetworkConnectionMenu::SetAction(network_menu_action_t action)
{
  current_action = action;

  switch (current_action) {
  case NET_HOST:
    action_label->SetText(_("Host a game"));
    server_address_label->SetVisible(false);
    server_address->SetVisible(false);
    port_number_label->SetVisible(true);
    port_number->SetVisible(true);
    internet_server->SetVisible(true);
    break;
  case NET_CONNECT_LOCAL:
    action_label->SetText(_("Connect to game"));
    server_address_label->SetVisible(true);
    server_address->SetVisible(true);
    port_number_label->SetVisible(true);
    port_number->SetVisible(true);
    internet_server->SetVisible(false);
    break;
  case NET_BROWSE_INTERNET:
    action_label->SetText(_("Connect to an internet game"));
    server_address_label->SetVisible(false);
    server_address->SetVisible(false);
    port_number_label->SetVisible(false);
    port_number->SetVisible(false);
    internet_server->SetVisible(false);
    break;
  } 
}

void NetworkConnectionMenu::Draw(const Point2i &mousePosition){}

void NetworkConnectionMenu::sig_ok()
{
  switch (current_action) {
  case NET_HOST: // Hosting your own server
    if( !internet_server->GetValue() )
      index_server.SetHiddenServer();

    if( !index_server.Connect() ) {
      msg_box->NewMessage(_("Error: Unable to contact index server to host a game"), c_red);
      return;
    }

    network.Init();
    network.ServerStart(port_number->GetText());

    index_server.SendServerStatus();

    if(network.IsConnected()) {
      network.client_inited = 1;
    } else {
      msg_box->NewMessage(_("Error: Unable to start server"), c_red);
      return;
    }
    break;


  case NET_CONNECT_LOCAL: // Direct connexion to a server
    network.Init();
    network.ClientConnect(server_address->GetText(), port_number->GetText());
    if (!network.IsConnected()) {
      msg_box->NewMessage(Format(_("Error: Unable to connect to %s:%s"),
				 (server_address->GetText()).c_str(), (port_number->GetText()).c_str()),
			  c_red);
      return;
    }
    break;

  case NET_BROWSE_INTERNET: // Search an internet game!
    if( !index_server.Connect() ) {
      msg_box->NewMessage(_("Error: Unable to contact index server to search an internet game"), c_red);
      return;
    }

    InternetMenu im;
    im.Run();

    index_server.Disconnect();

    // we don't go back into the main menu!
    // -> im.Run() may have connected to a host so the 
    // if(network.IsConnected()) just below will be catched and close the menu
    break;
  }

  if (network.IsConnected()) {
    // run the network menu ! :-)
    NetworkMenu nm;
    network.network_menu = &nm;
    nm.Run();
    network.network_menu = NULL;
    index_server.Disconnect();

    // back to main menu after playing
    Menu::sig_ok();
  } else {
    // TODO : add error sound and dialog
  }
}

void NetworkConnectionMenu::__sig_ok()
{
  network.Disconnect();
}

void NetworkConnectionMenu::__sig_cancel()
{
  network.Disconnect();
}

