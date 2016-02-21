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

#include <SDL_events.h>
#include "include/app.h"
#include "network/admin_commands.h"
#include "network/chat.h"
#include "graphic/text_list.h"
#include "network/network.h"
#include "graphic/text.h"
#include "game/time.h"
#include "tool/i18n.h"
#include "tool/text_handling.h"

const uint HEIGHT=15;
const uint XPOS=25;
const uint YPOS=130;
const uint MAXLINES=10; //Fidel's advise
const uint MAXSECONDS=10;

Chat::~Chat()
{
  delete msg;
  delete input;
}

Chat::Chat():
  input(NULL),
  msg(NULL),
  cursor_pos(0),
  check_input(false),
  last_time(0)
{
}

void Chat::Clear()
{
  chat.Clear();
}

void Chat::Show()
{
  uint now = Time::GetInstance()->ReadSec();

  if((now - last_time) >= MAXSECONDS){
    chat.DeleteLine();
    last_time = now;
  }

  chat.Draw(XPOS, YPOS, HEIGHT);

  if(check_input)
    ShowInput();
}

void Chat::ShowInput()
{
  check_input = true;
  if (input == NULL){
    input = new Text("", c_white);
    msg = new Text(_("Say: "), c_red);
  }

  /* FIXME where do those constants come from ?*/
  msg->DrawTopLeft(Point2i(25, 500));
  if (input->GetText() != "") {
    input->DrawTopLeft(Point2i(25 + msg->GetWidth() + 5, 500));
    input->DrawCursor(Point2i(25 + msg->GetWidth() + 5, 500), cursor_pos);
  }
}

bool Chat::CheckInput() const {
  return check_input;
}

void Chat::NewMessage(const std::string &msg)
{
  if (!chat.Size()){
    uint now = Time::GetInstance()->ReadSec();
    last_time = now;
  }

  chat.AddText(msg, MAXLINES);
}

void Chat::HandleKey(const SDL_Event& event)
{
  SDL_KeyboardEvent kbd_event = event.key;
  SDL_keysym key = kbd_event.keysym;
  std::string txt = input->GetText();

  if (TextHandle(txt, cursor_pos, key)) {
    input->Set(txt);
  } else {

    switch (key.sym){
      
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
      check_input = false; //Hide input widget
      if ( txt[0] == '/' )
	ProcessCommand(txt);
      else
	if (txt != "" )
	  Network::GetInstance()->SendChatMessage(txt); //Send 'txt' to other players
      input->Set("");
      break;

    default:
      break;
    }
  }
}
