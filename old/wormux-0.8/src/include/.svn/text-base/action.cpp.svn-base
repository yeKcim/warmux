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
 * Define all Wormux actions.
 *****************************************************************************/

#include "action.h"
//-----------------------------------------------------------------------------
#include <SDL_net.h>
#include "action_handler.h"
#include "character/body.h"
#include "character/character.h"
#include "game/game.h"
#include "game/time.h"
#include "network/distant_cpu.h"
#include "team/team.h"
#include "team/teams_list.h"
#include "tool/debug.h"
#include "tool/euler_vector.h"
//-----------------------------------------------------------------------------


static inline uint TimeStamp()
{
  if (Game::GetInstance()->IsGameLaunched())
    return Time::GetInstance()->Read();

  return 0;
}

// Action with various parameters
Action::Action (Action_t type, double value1, double value2)
{
  Init(type);
  Push(value1);
  Push(value2);
}

Action::Action (Action_t type, double value1, int value2)
{
  Init(type);
  Push(value1);
  Push(value2);
}

// Build an action from a network packet
Action::Action (const char *is, DistantComputer* _creator)
{
  creator = _creator;

  var.clear();
  m_type = (Action_t)SDLNet_Read32(is);
  is += 4;
  m_timestamp = (uint)SDLNet_Read32(is);
  is += 4;
  int m_length = SDLNet_Read32(is);
  is += 4;

  for(int i=0; i < m_length; i++)
  {
    uint32_t val = SDLNet_Read32(is);
    var.push_back(val);
    is += 4;
  }
  crc = SDLNet_Read32(is);
}

uint Action::ComputeCRC() const
{
  uint crc = 0;
  for(std::list<uint32_t>::const_iterator it = var.begin(); it != var.end(); it++)
    crc += *it;
  return crc;
}

bool Action::CheckCRC() const
{
  uint32_t crc;
  crc = 0;
  for(std::list<uint32_t>::const_iterator it = var.begin(); it != var.end(); it++)
    crc += *it;
  return this->crc == crc;
}

void Action::Init(Action_t type)
{
  m_type = type;
  var.clear();
  m_timestamp = TimeStamp();
  creator = NULL;
  crc = 0;
}

void Action::Write(char *os) const
{
  SDLNet_Write32(m_type, os);
  os += 4;
  SDLNet_Write32(m_timestamp, os);
  os += 4;
  uint32_t param_size = (uint32_t)var.size();
  SDLNet_Write32(param_size, os);
  os += 4;

  for(std::list<uint32_t>::const_iterator val = var.begin(); val!=var.end(); val++)
  {
    SDLNet_Write32(*val, os);
    os += 4;
  }
  SDLNet_Write32(ComputeCRC(), os);
}

// Convert the action to a packet
void Action::WritePacket(char* &packet, int & size) const
{
  size = GetSize();
  packet = (char*)malloc(size);
  Write(packet);
}

//-------------  Add datas to the action  ----------------
void Action::Push(int val)
{
  uint32_t tmp;
  memcpy(&tmp, &val, 4);
  var.push_back(tmp);
  MSG_DEBUG( "action", " (%s) Pushing int : %i",
        ActionHandler::GetInstance()->GetActionName(m_type).c_str(), val);
}

void Action::Push(double val)
{
  uint32_t tmp[2];
  memcpy(tmp, &val, 8);
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

void Action::Push(const Point2i& val)
{
  Push(val.x);
  Push(val.y);
}

void Action::Push(const Point2d& val)
{
  Push(val.x);
  Push(val.y);
}

void Action::Push(const EulerVector &val)
{
  Push(val.x0);
  Push(val.x1);
  Push(val.x2);
}

void Action::Push(const std::string& val)
{
  //Cut the string into 32bit values
  //But first, we write the size of the string:
  Push((int)val.size());
  char* ch = (char*)val.c_str();

  int count = val.size();
  while(count > 0)
  {
    uint32_t tmp = 0;
    // Fix-me : We are reading out of the c_str() buffer there :
#if (SDL_BYTEORDER == SDL_LIL_ENDIAN)
    strncpy((char*)&tmp, ch, 4);
    ch += 4;
#else
    char* c_tmp = (char*)&tmp;
    c_tmp +=3;
    for(int i=0; i < 4; i++)
      *(c_tmp--) = *(ch++);
#endif
    var.push_back(tmp);
    count -= 4;
  }
  MSG_DEBUG( "action", " (%s) Pushing string : %s",
  ActionHandler::GetInstance()->GetActionName(m_type).c_str(), val.c_str());
}

//-------------  Retrieve datas from the action  ----------------
int Action::PopInt()
{
  NET_ASSERT(var.size() > 0)
  {
    if(creator) creator->force_disconnect = true;
    return 0;
  }

  int val;
  uint32_t tmp = var.front();
  memcpy(&val, &tmp, 4);
  var.pop_front();
  MSG_DEBUG( "action", " (%s) Poping int : %i",
        ActionHandler::GetInstance()->GetActionName(m_type).c_str(), val);
  return val;
}

double Action::PopDouble()
{
  NET_ASSERT(var.size() > 0)
  {
    if(creator) creator->force_disconnect = true;
    return 0.0;
  }

  double val;
  uint32_t tmp[2];
#if (SDL_BYTEORDER == SDL_LIL_ENDIAN)
  tmp[0] = var.front();
  var.pop_front();
  tmp[1] = var.front();
#else
  tmp[1] = var.front();
  var.pop_front();
  tmp[0] = var.front();
#endif
  var.pop_front();
  memcpy(&val, tmp, 8);

  MSG_DEBUG( "action", " (%s) Poping double : %f",
        ActionHandler::GetInstance()->GetActionName(m_type).c_str(), val);
  return val;
}

std::string Action::PopString()
{
  NET_ASSERT(var.size() > 1)
  {
    if(creator) creator->force_disconnect = true;
    return "";
  }

  int length = PopInt();

  std::string str="";

  NET_ASSERT((int)var.size() >= length/4)
  {
    if(creator) creator->force_disconnect = true;
    return "";
  }

  while(length > 0)
  {
    NET_ASSERT(var.size() > 0)
    {
      if(creator) creator->force_disconnect = true;
      return "";
    }

    uint32_t tmp = var.front();
    var.pop_front();
    char tmp_str[5] = {0, 0, 0, 0, 0};
#if (SDL_BYTEORDER == SDL_LIL_ENDIAN)
    memcpy(tmp_str, &tmp, 4);
#else
    char* c_tmp_str = (char*)(&tmp_str) + 3;
    char* c_tmp = (char*)&tmp;
    for(int i=0; i < 4; i++)
      *(c_tmp_str--) = *(c_tmp++);
#endif
    str += tmp_str;
    length -= 4;
  }
  MSG_DEBUG( "action", " (%s) Poping string : %s",
        ActionHandler::GetInstance()->GetActionName(m_type).c_str(), str.c_str());
  return str;
}

Point2i Action::PopPoint2i()
{
  int x, y;
  x = PopInt();
  y = PopInt();
  return Point2i(x, y);
}

Point2d Action::PopPoint2d()
{
  double x, y;
  x = PopDouble();
  y = PopDouble();
  return Point2d(x, y);
}

EulerVector Action::PopEulerVector()
{
  double x0, x1, x2;
  x0 = PopDouble();
  x1 = PopDouble();
  x2 = PopDouble();
  return EulerVector(x0, x1, x2);
}

//-------------  Send/Retrieve datas about Character

void Action::StoreActiveCharacter()
{
  StoreCharacter(ActiveCharacter().GetTeamIndex() ,ActiveCharacter().GetCharacterIndex());
}

void Action::StoreCharacter(uint team_no, uint char_no)
{
  Push((int)team_no);
  Push((int)char_no);
  Character * c = GetTeamsList().FindPlayingByIndex(team_no)->FindByIndex(char_no);
  c->StoreValue(this);
}

void Action::RetrieveCharacter()
{
  int team_no = PopInt();
  int char_no = PopInt();
  Character * c = GetTeamsList().FindPlayingByIndex(team_no)->FindByIndex(char_no);
  c->GetValueFromAction(this);
}


//-----------------------------------------------------------------------------
