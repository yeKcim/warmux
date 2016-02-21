/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2007 Wormux Team.
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
 * Messages s'affichant en haut de l'ecran (et écrit dans la console).
 *****************************************************************************/

#ifndef GAME_MESSAGES_H
#define GAME_MESSAGES_H
//-----------------------------------------------------------------------------
#include "include/base.h"
#include "graphic/text.h"
#include <string>
#include <list>
//-----------------------------------------------------------------------------

/*
 * Class Message
 * derivated from Text
 * Stores a Text and a date (typically the date of creation of the message)
 */
class Message : public Text
{
private:
  uint time;

public:
  Message(const std::string &new_txt,
          const Color &new_color,
          Font::font_size_t font_size,
          Font::font_style_t font_style,
          uint _time) :
    Text(new_txt, new_color, font_size, font_style),
    time(_time) {};

  inline uint get_time() { return time; }
};
/*
 * class GameMessages
 * stores and displays messages on the screen
 *
 * use
 * void Add(const std::string &message)
 * to add a message
 */
class GameMessages
{
 public:
  static GameMessages * GetInstance();

  // remove all messages
  void Reset();

  // display all messages in list
  void Draw();

  // Remove too old messages
  void Refresh();

  // Add a message
  void Add(const std::string &message);

 private:
  std::list<Message *> liste;
  typedef std::list<Message *>::iterator iterator;
  static GameMessages * singleton;
  GameMessages() { };
  ~GameMessages() { Reset(); };
};

//-----------------------------------------------------------------------------
#endif
