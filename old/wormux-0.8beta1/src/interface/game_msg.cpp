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

#include "game_msg.h"
#include <iostream>
#include "game/time.h"
#include "graphic/video.h"
#include "graphic/font.h"
#include "include/app.h"
#include "game_msg.h"

// Hauteur de la police de caractere "mini"
#define HAUT_POLICE_MINI 12 // pixels

// Interligne police "mini" (pour les messages)
#define INTERLIGNE_MINI 3 // pixels

// Lifespan of messages
#define MSG_LIFESPAN 7000 // ms

const uint NBR_MSG_MAX = 14;

GameMessages * GameMessages::singleton = NULL;

GameMessages * GameMessages::GetInstance() {
  if (singleton == NULL) {
    singleton = new GameMessages();
  }
  return singleton;
}

GameMessages::GameMessages() {
}

// Clean up the message list
void GameMessages::Reset(){
  iterator i;
  for( i=liste.begin(); i != liste.end(); i++){
    Message * msg = *i;
    assert(msg); /* the message must be valid if nothing went wrong */
    delete (msg);
    msg = NULL;
  }
  liste.clear();
}

void GameMessages::Draw(){
  // Display messages
  uint msgy = 50;

  for( iterator i=liste.begin(); i != liste.end(); ++i ){
    (*i)->DrawCenterTop(AppWormux::GetInstance()->video.window.GetWidth()/2, msgy);
    msgy += HAUT_POLICE_MINI + INTERLIGNE_MINI;
  }
}

// Erase messages older than MSG_LIFESPAN
void GameMessages::Refresh(){
  iterator i;
  for( i=liste.begin(); i != liste.end(); ){
    Message * msg = *i;
    if( MSG_LIFESPAN < Time::GetInstance()->Read() - msg->get_time() ){
      delete (msg);
      /* erase method return the next element */
      i = liste.erase (i);
    }
    else /* nothing was removed, take next */
      i++;
  }
}

// Add a message to the end of a the list of messages
void GameMessages::Add(const std::string &message){
  // Debug message
  std::cout << "o MSG: " << message << std::endl;
  // Add message at the end of the list
  Message * newMessage = new Message(message, white_color, Font::FONT_SMALL, Font::FONT_NORMAL, Time::GetInstance()->Read());
  liste.push_back (newMessage);

  /* if there are too many messages, remove some of them */
  while( NBR_MSG_MAX < liste.size()) {
    Message * msg = liste.front();
    assert(msg); /* the message must be valid if nothing went wrong */
    liste.pop_front();
    delete msg;
  }
}

