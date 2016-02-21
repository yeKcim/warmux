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
 * Talk box: box handling chat in network menus
 *****************************************************************************/

#include "graphic/text.h"
#include "graphic/video.h"
#include "gui/button.h"
#include "gui/msg_box.h"
#include "gui/text_box.h"
#include "include/app.h"
#include "network/admin_commands.h"
#include "network/chat.h"
#include "tool/resource_manager.h"

#include "talk_box.h"


TalkBox::TalkBox(const Point2i& size, Font::font_size_t fsize, Font::font_style_t fstyle) :
  VBox(size.x, false, false)
{
  SetNoBorder();

  msg_box = new MsgBox(Point2i(size.x, size.y - 20), fsize, fstyle);
  AddWidget(msg_box);

  HBox* tmp2_box = new HBox(16, false);
  tmp2_box->SetMargin(4);
  tmp2_box->SetBorder(Point2i(0,0));
  line_to_send_tbox = new TextBox("", size.x-20, fsize, fstyle);
  tmp2_box->AddWidget(line_to_send_tbox);

  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml",false);
  send_txt_bt = new Button(res, "menu/send_txt", true);
  tmp2_box->AddWidget(send_txt_bt);
  resource_manager.UnLoadXMLProfile(res);

  AddWidget(tmp2_box);
}

void TalkBox::NewMessage(const std::string &msg, const Color& color)
{
  msg_box->NewMessage(msg, color);
}

void TalkBox::SendChatMsg()
{
  std::string empty = "";
  std::string txt = line_to_send_tbox->GetText();
  if (txt[0] == '/')
    ProcessCommand(txt);
  else if (txt != "" )
    Chat::SendMessage(txt);

  line_to_send_tbox->SetText(empty);
}

Widget* TalkBox::ClickUp(const Point2i &mousePosition, uint button)
{
  Widget* w = VBox::ClickUp(mousePosition, button);
  if (w == send_txt_bt) {
    SendChatMsg();
    return this;
  }
  return w;
}

bool TalkBox::TextHasFocus() const
{
  return line_to_send_tbox->HasFocus();
}

bool TalkBox::SendKey(SDL_keysym key)
{
  bool r = false;

  switch (key.sym) {
  case SDLK_RETURN:
  case SDLK_KP_ENTER:
    SendChatMsg();
    r = true;
    break;
  default:
    if (send_txt_bt->HasFocus()) {
      r = send_txt_bt->SendKey(key);
    }
    break;
  }

  return r;
}

void TalkBox::Clear()
{
  msg_box->Clear();
}
