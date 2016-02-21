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
#include "chat.h"
#include "graphic/text_list.h"
#include "network.h"
#include "graphic/text.h"
#include "game/time.h"
#include "tool/i18n.h"

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
  input->DrawTopLeft(Point2i(25 + msg->GetWidth() + 5, 500));
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

  switch (key.sym){

  case SDLK_RETURN:
    check_input = false; //Hide input widget
    if (txt != "" )
      Network::GetInstance()->SendChatMessage(txt); //Send 'txt' to other players
    input->Set("");
    break;

  case SDLK_BACKSPACE:
    if (kbd_event.state == 1 && txt != "")
    {
      int off = txt.size() -1;

      // UTF-8 character encoded on 1 octet
      if(! (txt[off] & 0x80))
        txt = txt.substr(0, txt.size()-1);
      else
      // Multi-byte char
      {
        while(! (txt[off] & 0x40))
          off--;
      }
      txt = txt.substr(0, off);
    }
    input->Set(txt);
    break;

  case SDLK_TAB:
  case SDLK_CLEAR:
  case SDLK_ESCAPE:
  case SDLK_DELETE:
  case SDLK_UP:
  case SDLK_DOWN:
  case SDLK_RIGHT:
  case SDLK_LEFT:
  case SDLK_INSERT:
  case SDLK_HOME:
  case SDLK_END:
  case SDLK_PAGEUP:
  case SDLK_PAGEDOWN:
    break;

  default:
    if (kbd_event.state == 1 && key.unicode > 0){
      if(key.unicode < 0x80) { // 1 byte char
        txt = txt + (char)key.unicode;
      }
      else if (key.unicode < 0x800) {// 2 byte char
        txt = txt + (char)(((key.unicode & 0x7c0) >> 6) | 0xc0);
        txt = txt + (char)((key.unicode & 0x3f) | 0x80);
      }
      else {// if (key.unicode < 0x10000) // 3 byte char
        txt = txt + (char)(((key.unicode & 0xf000) >> 12) | 0xe0);
        txt = txt + (char)(((key.unicode & 0xfc0) >> 6) | 0x80);
        txt = txt + (char)((key.unicode & 0x3f) | 0x80);
      }
      input->Set(txt);
    }
  }
}
