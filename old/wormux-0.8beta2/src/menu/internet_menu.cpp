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
 * Network connection menu: this menu allows the user to choose between
 * hosting a game or connecting to a server.
 *****************************************************************************/


#include "internet_menu.h"

#include "include/app.h"
#include "gui/button.h"
#include "gui/label.h"
#include "gui/box.h"
#include "gui/button_text.h"
#include "gui/list_box.h"
#include "gui/question.h"
#include "graphic/video.h"
#include "network/net_error_msg.h"
#include "network/index_server.h"
#include "tool/i18n.h"
#include "tool/resource_manager.h"

InternetMenu::InternetMenu() :
  Menu("menu/bg_network", vCancel)
{
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml",false);
  Rectanglei rectZero(0, 0, 0, 0);
  
  Rectanglei stdRect(0, 0, 405, 64);

  uint x_button = AppWormux::GetInstance()->video->window.GetWidth()/2 - stdRect.GetSizeX()/2;
  uint y_box = AppWormux::GetInstance()->video->window.GetHeight()/2 - 200;

  connection_box = new VBox(Rectanglei( x_button, y_box, stdRect.GetSizeX(), 1), false);
  connection_box->SetBorder(Point2i(0,0));

  connect_lst = new ListBox( Rectanglei(0, 0, stdRect.GetSizeX(), 300), false);
  connection_box->AddWidget(connect_lst);

  refresh = new ButtonText( Point2i(0,0),
                            res, "main_menu/button",
                            _("Refresh"), // Refresh the list of available hosts
                            Font::FONT_BIG, 
                            Font::FONT_NORMAL);

  refresh->SetSizePosition( stdRect );
  connection_box->AddWidget(refresh);

  connect = new ButtonText( Point2i(0,0),
                            res, "main_menu/button",
                            _("Connect !"),
                            Font::FONT_BIG, 
                            Font::FONT_NORMAL);

  connect->SetSizePosition( stdRect );
  connection_box->AddWidget(connect);

  widgets.AddWidget(connection_box);

  resource_manager.UnLoadXMLProfile(res);
  RefreshList(false);
}

InternetMenu::~InternetMenu()
{
}

void InternetMenu::OnClickUp(const Point2i &mousePosition, int button)
{     
  Widget* w = widgets.ClickUp(mousePosition, button);  

  if (w == refresh)
    RefreshList(true);
  else
  if (w == connect && connect_lst->GetSelectedItem() != -1)
  {
    Network::connection_state_t conn = Network::ClientStart(connect_lst->ReadLabel(), connect_lst->ReadValue());
    if ( Network::IsConnected() && conn == Network::CONNECTED )
    {
      close_menu = true;
      Menu::mouse_ok();
    }
    else
    {
      DispNetworkError(conn);
      Menu::RedrawMenu();
    }
  }
}

void InternetMenu::OnClick(const Point2i &mousePosition, int button)
{     
  widgets.Click(mousePosition, button); 
}

void InternetMenu::DisplayNoGameRunning()
{
  Question question;
  question.Set(_("Sorry, currently, no game is waiting for players"), 1, 0);
  question.Ask();
  Menu::RedrawMenu();
}

void InternetMenu::RefreshList(bool warning_if_empty)
{
  // Save the currently selected address
  int current = connect_lst->GetSelectedItem();

  // Empty the list:
  while (connect_lst->Size() != 0)
  {
    connect_lst->Select(0);
    connect_lst->RemoveSelected();
  }

  std::list<address_pair> lst = index_server.GetHostList();

  if (warning_if_empty && lst.size() == 0) {
    DisplayNoGameRunning();
    return;
  }

  for (std::list<address_pair>::iterator pair_it = lst.begin();
       pair_it != lst.end();
       ++pair_it)
    connect_lst->AddItem( false, pair_it->first, pair_it->second );

  if (current != -1 && connect_lst->Size() != 0)
    connect_lst->Select( current );
}

void InternetMenu::Draw(const Point2i &/*mousePosition*/)
{
  index_server.Refresh();
}

bool InternetMenu::signal_ok()
{
  return true;
}

bool InternetMenu::signal_cancel()
{
  return true;
}
