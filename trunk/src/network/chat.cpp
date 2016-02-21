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

#include "game/game.h"
#include "game/game_time.h"
#include "graphic/text.h"
#include "graphic/text_list.h"
#include "graphic/video.h"
#include "include/action_handler.h"
#include "include/app.h"
#include "network/chat.h"
#include "network/admin_commands.h"
#include "network/network.h"
#include "tool/text_handling.h"

#include <SDL_events.h>

#define HEIGHT       15
#define XPOS         25
#define YPOS        130
#define MAXLINES     10 //Fidel's advise
#define MAXSECONDS   40

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

void Chat::Show()
{
  uint now = GameTime::GetInstance()->ReadSec();

  if (now - last_time >= MAXSECONDS){
    chat.DeleteLine();
    last_time = now;
  }

  chat.Draw(XPOS, YPOS, HEIGHT);

  if(check_input)
    ShowInput();
}

void Chat::ShowInput()
{
  if (!check_input) {
    check_input = true;

    /* Enable key repeat when chatting :) */
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
  }

  if (!input) {
    input = new Text("", c_white);
    msg = new Text(_("Say: "), c_red);
  }

  /* FIXME where do those constants come from ?*/
  int ypos = GetMainWindow().GetHeight() - 100;
  msg->DrawLeftTop(Point2i(25, ypos));
  if (input->GetText() != "") {
    int x = 25 + msg->GetWidth() + 5;
    input->DrawCursor(Point2i(x, ypos), cursor_pos, input->GetWidth());
    input->DrawLeftTop(Point2i(x, ypos));
  }
}

void Chat::NewMessage(const std::string &msg, const Color& color)
{
  if (!chat.Size()){
    uint now = GameTime::GetInstance()->ReadSec();
    last_time = now;
  }

  chat.AddText(msg, color, MAXLINES);
}

void Chat::SendMessage(const std::string &msg, bool in_game)
{
  if (msg.size() == 0)
    return;

  Action* a = new Action((in_game) ? Action::ACTION_CHAT_INGAME_MESSAGE
                                   : Action::ACTION_CHAT_MENU_MESSAGE);
  a->Push(Network::GetInstance()->GetPlayer().GetId());
  a->Push(msg);
  ActionHandler::GetInstance()->NewAction(a);
}

void Chat::ProcessSendMessage(const std::string &msg)
{
  if (msg.size() == 0)
    return;

  bool in_game = GameIsRunning();
  if (msg[0] == '/')
    ProcessCommand(msg);
  else if (msg != "")
    SendMessage(msg, in_game);
}

void Chat::CloseInput()
{
  check_input = false; //Hide input widget

  input->SetText("");
  cursor_pos = 0;

  // Disable key repeat during the game!
  SDL_EnableKeyRepeat(0, 0);
}

void Chat::HandleKeyPressed(const SDL_Event& evnt)
{
  SDL_KeyboardEvent kbd_event = evnt.key;
  SDL_keysym key = kbd_event.keysym;
  std::string txt = input->GetText();

  if (TextHandle(txt, cursor_pos, key))
    input->SetText(txt);
}

void Chat::HandleKeyReleased(const SDL_Event& evnt)
{
  SDL_KeyboardEvent kbd_event = evnt.key;
  SDL_keysym key = kbd_event.keysym;
  std::string txt = input->GetText();

  switch (key.sym) {

  case SDLK_RETURN:
  case SDLK_KP_ENTER:
    ProcessSendMessage(txt);
    CloseInput();
    break;
  case SDLK_ESCAPE:
    CloseInput();
    break;
  default:
    break;
  }
}
