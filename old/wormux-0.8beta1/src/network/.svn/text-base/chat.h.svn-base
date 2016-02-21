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

#include <SDL.h>
#include "graphic/text_list.h"
#include "network.h"

class Chat
{
 private:
  /* If you need this, implement it (correctly)*/
  Chat(const Chat&);
  const Chat& operator=(const Chat&);
  /*********************************************/

  TextList* chat;
  Text* input;
  Text* msg;

  bool check_input;
  uint last_time;

 public:
  Chat();
  ~Chat();
  void Show();
  void ShowInput();
  bool CheckInput();
  void Reset();
  void NewMessage(const std::string& msg);
  void HandleKey(const SDL_Event& event);
};

#endif
