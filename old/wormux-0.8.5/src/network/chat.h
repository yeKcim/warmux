/******************************************************************************
 *  Wormux is a convivial mass murder game.
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
#include "include/base.h"
#include "graphic/text_list.h"

// Forward declarations
class Text;
#ifndef _SDL_events_h
union SDL_Event;
#endif

class Chat
{
 private:
  /* If you need this, implement it (correctly)*/
  Chat(const Chat&);
  const Chat& operator=(const Chat&);
  /*********************************************/

  TextList chat;
  Text* input;
  Text* msg;
  std::string::size_type cursor_pos;

  bool check_input;
  uint last_time;

 public:
  Chat();
  ~Chat();
  void Show();
  void ShowInput();
  bool CheckInput() const;
  void Clear();
  void NewMessage(const std::string& msg);
  void HandleKey(const SDL_Event& event);

  static void SendMessage(const std::string& msg);
};

#endif
