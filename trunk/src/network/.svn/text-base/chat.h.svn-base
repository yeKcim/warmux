/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2007 Jon de Andres
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
 * Chat in game session.
 * nefertum - Jon de Andres
 *****************************************************************************/

#ifndef CHAT_H
#define CHAT_H

#include <string>
#include <WARMUX_base.h>
#include "graphic/color.h"
#include "graphic/text_list.h"

// Forward declarations
class Text;
#ifndef _SDL_events_h
union SDL_Event;
#endif

class Chat
{
  TextList chat;
  Text* input;
  Text* msg;
  std::string::size_type cursor_pos;

  bool check_input;
  uint last_time;

  void CloseInput();

public:
  Chat();
  ~Chat();
  void Show();
  void ShowInput();
  bool CheckInput() const { return check_input; }
  void Clear() { chat.Clear(); }
  void NewMessage(const std::string& msg, const Color& color);
  void HandleKeyPressed(const SDL_Event& evnt);
  void HandleKeyReleased(const SDL_Event& evnt);

  static void SendMessage(const std::string& msg, bool in_game = true);
  static void ProcessSendMessage(const std::string &msg);
};

#endif
