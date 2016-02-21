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
 * Game menu
 *****************************************************************************/

#ifndef NETWORK_MENU_H
#define NETWORK_MENU_H

#include "menu.h"

class Team;
class SpinButton;
class NetworkTeamsSelectionBox;
class TalkBox;
class MapSelectionBox;
class CheckBox;
class ComboBox;
class MultiTabs;
class GameModeEditor;

class NetworkMenu : public Menu
{
  bool waiting_for_server;

  /* Options controllers */
  Label* mode_label;
  Label* connected_players;
  Label* initialized_players;
  CheckBox* play_in_loop;
  GameModeEditor *opt_game_mode;

  /* Chat controller */
  TalkBox* msg_box;

  /* Team controllers */
  NetworkTeamsSelectionBox *team_box;

  /* Map controllers */
  MapSelectionBox *map_box;

  MultiTabs *tabs;

  void PrepareForNewGame();
  void SaveOptions();
  void WaitingForGameMaster();
  void AddGameModeTab();

  virtual void Draw(const Point2i &mousePosition);
  virtual void HandleEvent(const SDL_Event& evnt);

  virtual void key_ok();
  virtual bool signal_ok();
  virtual bool signal_cancel();
  virtual void signal_begin_run();

  void RequestSavedTeams();

public:
  NetworkMenu();

  void AddTeamCallback(const std::string& team_id);
  void UpdateTeamCallback(const std::string& old_team_id, const std::string& team_id);
  void DelTeamCallback(const std::string& team_id);
  void ChangeMapCallback();
  void SetGameMasterCallback();
  Team * FindUnusedTeam(const std::string& default_team_id);
  bool HasOpenTeamSlot();

  void ReceiveMsgCallback(const std::string& msg, const Color& color);
  void SetMapsCallback(const std::vector<uint>& list);
  void SetTeamsCallback(const std::vector<uint>& list);
};

#endif
