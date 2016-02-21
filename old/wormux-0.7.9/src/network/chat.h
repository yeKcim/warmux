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
 * Chat in game session.
 *****************************************************************************/

#ifndef CHAT_H
#define CHAT_H

#define SAY "Say: " //We should use Multilanguage functions
#define HEIGHT 15
#define XPOS 25
#define YPOS 130
#define MAXLINES 10 //Fidel's advise
#define MAXSECONDS 10

#include <SDL.h>
#include "../graphic/text_list.h"
#include "../network/network.h"

class Chat
{
 private:
  TextList* chat;
  Text* input;
  Text* msg;

  int check_input;
  uint last_time;

 public:
  Chat();
  ~Chat();
  void Show();
  void ShowInput();
  int CheckInput();
  void Reset();
  void NewMessage(const std::string &msg);
  void HandleKey(const SDL_Event *event);
};

#endif
