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
 * Define all Wormux actions.
 *****************************************************************************/

#include "action.h"
//-----------------------------------------------------------------------------
#include <SDL_net.h>
#include "action_handler.h"
#include "../tool/debug.h"

//-----------------------------------------------------------------------------
// Action without parameter
Action::Action (Action_t type)
{
  var.clear();
  m_type = type; 
}

// Action with various parameters
Action::Action (Action_t type, int value) : m_type(type)
{  var.clear();  Push(value);  }

Action::Action (Action_t type, double value) : m_type(type)
{  var.clear();  Push(value);  }

Action::Action (Action_t type, const std::string& value) : m_type(type)
{  var.clear();  Push(value);  }

Action::Action (Action_t type, double value1, int value2) : m_type(type)
{  var.clear();  Push(value1); Push(value2);  }

// Build an action from a network packet
Action::Action (const char *is)
{
  var.clear();
  m_type = (Action_t)SDLNet_Read32(is);
  is += 4;
  int m_lenght = SDLNet_Read32(is);
  is += 4;

  for(int i=0; i < m_lenght; i++)
  {
    Uint32 val = SDLNet_Read32(is);
    var.push_back(val);
    is += 4;
  }
}

Action::~Action ()
{
}

Action_t Action::GetType() const 
{ 
  return m_type; 
}

// Convert the action to a packet
void Action::WritePacket(char* &packet, int & size)
{
  size = 4 //Size of the type;
        + 4 //Size of the number of variable
        + int(var.size()) * 4;

  packet = (char*)malloc(size);
  char* os = packet;

  SDLNet_Write32(m_type, os);
  os += 4;
  Uint32 param_size = (Uint32)var.size();
  SDLNet_Write32(param_size, os);
  os += 4;

  for(std::list<Uint32>::iterator val = var.begin(); val!=var.end(); val++)
  {
    SDLNet_Write32(*val, os);
    os += 4;
  }
}

//-------------  Add datas to the action  ----------------
void Action::Push(int val)
{
  Uint32 tmp;
  memcpy(&tmp, &val, 4);
  var.push_back(tmp);
  MSG_DEBUG( "action", " (%s) Pushing int : %i",
        ActionHandler::GetInstance()->GetActionName(m_type).c_str(), val);

}

void Action::Push(double val)
{
  Uint32 tmp[2];
  memcpy(&tmp, &val, 8);
#if (SDL_BYTEORDER == SDL_LIL_ENDIAN)
  var.push_back(tmp[0]);
  var.push_back(tmp[1]);
#else
  var.push_back(tmp[1]);
  var.push_back(tmp[0]);
#endif

  MSG_DEBUG( "action", " (%s) Pushing double : %f",
        ActionHandler::GetInstance()->GetActionName(m_type).c_str(), val);
}

void Action::Push(std::string val)
{
  //Cut the string into 32bit values
  //But first, we write the size of the string:
  Push((int)val.size());
  char* ch = (char*)val.c_str();

  int count = val.size();
  while(count > 0)
  {
    Uint32 tmp = 0;
    // Fix-me : We are reading out of the c_str() buffer there :
#if (SDL_BYTEORDER == SDL_LIL_ENDIAN)
    strncpy((char*)&tmp, ch, 4);
    var.push_back(tmp);
    ch += 4;
    count -= 4;
#else
    char* c_tmp = (char*)&tmp;
    c_tmp +=3;
    for(int i=0; i < 4; i++)
      *(c_tmp--) = *(ch++);

    var.push_back(tmp);
    count -= 4;
#endif
  }
  MSG_DEBUG( "action", " (%s) Pushing string : %s",
  ActionHandler::GetInstance()->GetActionName(m_type).c_str(), val.c_str());
}

//-------------  Retrieve datas from the action  ----------------
int Action::PopInt()
{
  assert(var.size() > 0);
  if(var.size() <= 0)
	return 0;
  int val;
  Uint32 tmp = var.front();
  memcpy(&val, &tmp, 4);
  var.pop_front();
  MSG_DEBUG( "action", " (%s) Poping int : %i",
        ActionHandler::GetInstance()->GetActionName(m_type).c_str(), val);
  return val;
}

double Action::PopDouble()
{
  assert(var.size() > 0);
  if(var.size() <= 0)
    return 0.0;
  double val;
  Uint32 tmp[2];
#if (SDL_BYTEORDER == SDL_LIL_ENDIAN)
  tmp[0] = var.front();
  var.pop_front();
  tmp[1] = var.front();
  var.pop_front();
  memcpy(&val, &tmp, 8);
#else
  tmp[1] = var.front();
  var.pop_front();
  tmp[0] = var.front();
  var.pop_front();
  memcpy(&val, &tmp, 8);
#endif

  MSG_DEBUG( "action", " (%s) Poping double : %f",
        ActionHandler::GetInstance()->GetActionName(m_type).c_str(), val);
  return val;
}

std::string Action::PopString()
{
  assert(var.size() > 1);
  if(var.size() <= 1)
    return "";
  int lenght = PopInt();

  std::string str="";

  assert((int)var.size() >= lenght/4);
  if((int)var.size() < lenght/4)
    return "";

  while(lenght > 0)
  {
    Uint32 tmp = var.front();  
    var.pop_front();
    char tmp_str[5] = {0, 0, 0, 0, 0};
#if (SDL_BYTEORDER == SDL_LIL_ENDIAN)
    memcpy(tmp_str, &tmp, 4);
    str += tmp_str;
    lenght -= 4;
#else
    char* c_tmp_str = (char*)(&tmp_str) + 3;
    char* c_tmp = (char*)&tmp;
    for(int i=0; i < 4; i++)
      *(c_tmp_str--) = *(c_tmp++);

    str += tmp_str;
    lenght -= 4;    
#endif
    }
  MSG_DEBUG( "action", " (%s) Poping string : %s",
        ActionHandler::GetInstance()->GetActionName(m_type).c_str(), str.c_str());
  return str;
}

//-----------------------------------------------------------------------------
