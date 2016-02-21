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
 * Replay menu
 *****************************************************************************/

#ifndef REPLAY_MENU_H
#define REPLAY_MENU_H

#include "menu.h"

class VBox;
class FileListBox;
class Label;
class ScrollBox;

class ReplayMenu : public Menu
{
  VBox              *file_box;
  FileListBox       *replay_lbox;
  const std::string *selected; // Base filename of the replay

  // Display info on selected replay
  VBox              *info_box;
  Label             *version_lbl;
  Label             *date_lbl;
  Label             *duration_lbl;
  Label             *comment_lbl;
  ScrollBox         *teams_lbox;

  void ClearReplayInfo();
  void ChangeReplay();
  void OnClickUp(const Point2i &mousePosition, int button);

  bool signal_ok();

public:
  ReplayMenu();
};

#endif //REPLAY_MENU_H
