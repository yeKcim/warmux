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
 * Game menu
 *****************************************************************************/

#ifndef NETWORK_MENU_H
#define NETWORK_MENU_H

#include "menu.h"

class Team;
class SpinButton;
class NetworkTeamsSelectionBox;
class MsgBox;
class MapSelectionBox;
class TextBox;

class NetworkMenu : public Menu
{
  NetworkMenu(const NetworkMenu&);
  const NetworkMenu& operator=(const NetworkMenu&);

  bool waiting_for_server;

  /* Options controllers */
  SpinButton* player_number;
  Box* options_box;
  Label* mode;
  Label* connected_players;
  Label* initialized_players;

  /* Chat controllers */
  MsgBox* msg_box;
  TextBox* line_to_send_tbox;
  Button* send_txt_bt;

  /* Team controllers */
  NetworkTeamsSelectionBox *team_box;

  /* Map controllers */
  MapSelectionBox *map_box;

  void SaveOptions();
  void OnClick(const Point2i &mousePosition, int button);
  void OnClickUp(const Point2i &mousePosition, int button);
  void Draw(const Point2i &mousePosition);
  void SendChatMsg();
  void WaitingForServer();

  void key_ok();
  bool signal_ok();
  bool signal_cancel();

public:
  NetworkMenu();
  ~NetworkMenu();

  void AddTeamCallback(const std::string& team_id);
  void UpdateTeamCallback(const std::string& team_id);
  void DelTeamCallback(const std::string& team_id);
  void ChangeMapCallback();

  void ReceiveMsgCallback(const std::string& msg);
};

#endif
