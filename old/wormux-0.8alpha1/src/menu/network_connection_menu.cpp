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

#define WORMUX_NETWORK_PORT "9999"

NetworkConnectionMenu::NetworkConnectionMenu() :
  Menu("menu/bg_network", vCancel)
{
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml",false);
  Rectanglei rectZero(0, 0, 0, 0);
  
  normal_font = Font::GetInstance(Font::FONT_NORMAL);
  big_font = Font::GetInstance(Font::FONT_BIG);

  uint button_height = 64;
  uint button_width = 300;

  uint x_button = AppWormux::GetInstance()->video.window.GetWidth()/2 - button_width/2;
  uint y_box = AppWormux::GetInstance()->video.window.GetHeight()/2 - 200;

  // Connection related widgets
  connection_box = new VBox(Rectanglei( x_button, y_box, button_width, 1), false);
  connection_box->SetBorder(Point2i(0,0));
  connection_box->AddWidget(new Label(_("Server adress:"), rectZero, *normal_font));

  server_adress = new TextBox("localhost", rectZero, *normal_font);
  connection_box->AddWidget(server_adress);

  start_client = new ButtonText( Point2i(0,0),
				 res, "main_menu/button",
				 _("Connect to game"),
				 big_font);
  start_client->SetSizePosition(Rectanglei(0,0, button_width, button_height));
  connection_box->AddWidget(start_client);

  start_server = new ButtonText( Point2i(0,0),
				 res, "main_menu/button",
				 _("Host a game"),
				 big_font);
  start_server->SetSizePosition(Rectanglei(0,0, button_width, button_height));
  connection_box->AddWidget(start_server);

  widgets.AddWidget(connection_box);

  // Warning about experimental networking
  msg_box = new MessageBox(7, 
			   Rectanglei( AppWormux::GetInstance()->video.window.GetWidth()/2 - 300,
				       y_box+connection_box->GetSizeY() + 30, 
				       600, 1), 
			   Font::GetInstance(Font::FONT_SMALL));
  widgets.AddWidget(msg_box);

  msg_box->NewMessage("WARNING!! Network is still under developement and therefore a little experimental.");
  msg_box->NewMessage(""); // Skip a line
  msg_box->NewMessage("Some weapons are disabled, because of known bugs");
  msg_box->NewMessage("(ninjarope, airhammer, blowtorch, submachine gun)");
  msg_box->NewMessage("and surely many other things don't work either!");
  msg_box->NewMessage(""); // Skip a line
  msg_box->NewMessage("Have a good game!");

  resource_manager.UnLoadXMLProfile(res);
}

NetworkConnectionMenu::~NetworkConnectionMenu()
{
}

void NetworkConnectionMenu::OnClic(const Point2i &mousePosition, int button)
{     
  Widget* w = widgets.Clic(mousePosition, button);  
  
  if (w == start_client)
  {
    network.Init();
    network.ClientConnect(server_adress->GetText(),WORMUX_NETWORK_PORT);
    if(network.IsConnected())
    {
      msg_box->NewMessage(_("Connected to ") + server_adress->GetText());
      msg_box->NewMessage("Click the green check when you are ready to");
      msg_box->NewMessage("play!");
    }
    else
      msg_box->NewMessage(_("Unable to connect"));
  }

  if (w == start_server)
  {
    network.Init();
    network.ServerStart(WORMUX_NETWORK_PORT);
    if(network.IsConnected())
    {
      network.client_inited = 1;
      msg_box->NewMessage(_("Server started"));
    }
    else
      msg_box->NewMessage(_("Unable to start server"));
  }


  if (network.IsConnected()) {
    // run the network menu ! :-)
    NetworkMenu nm;
    network.network_menu = &nm;
    nm.Run();

    // for the moment, it's just for test...
    close_menu = true;
    sig_ok();
  }
}

void NetworkConnectionMenu::Draw(const Point2i &mousePosition){}

void NetworkConnectionMenu::__sig_ok()
{
  network.Disconnect();
}

void NetworkConnectionMenu::__sig_cancel()
{
  network.Disconnect();
}

