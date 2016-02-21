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
 * Display a message on the game screen, and asks a question in the game or wait
 * at least for the player to press a key.
 *****************************************************************************/

#ifndef QUESTION_H
#define QUESTION_H
//-----------------------------------------------------------------------------
#include "include/base.h"
#include <string>
#include <list>

class Text;
class Sprite;
#ifndef _SDL_events_h
union SDL_Event;
#endif

//-----------------------------------------------------------------------------

class Question
{
  /* If you need this, implement it (correctly)*/
  Question(const Question&);
  Question operator=(const Question&);
  /*********************************************/

  Sprite* background;

  // A choice = a key return a value
  class choice_t
  {
    private:
      int m_key;
      int m_val;
    public:
      choice_t (int key, int value):
        m_key(key), m_val(value) { };
      inline const int & key() const { return m_key; };
      inline const int & val() const { return m_val; };
  };

  // Choices list
  std::list<choice_t> choices;
  typedef std::list<choice_t>::iterator choice_iterator;
  // Default choice used when another key is pressed
  struct s_default_choice
  {
    bool active;
    int value;
  } default_choice;

  int TreatsKey (const SDL_Event &event);

  // Message to display
  Text * text;

public:
  Question();
  ~Question();

  void Set(const std::string &message,
           bool default_active,
           int default_value,
      const std::string &bg_sprite="");
  int Ask();
  void Draw() const;

  inline void add_choice(int key, int value)
  {
    return this->choices.push_back(choice_t(key,value));
  }

};

//-----------------------------------------------------------------------------
#endif
