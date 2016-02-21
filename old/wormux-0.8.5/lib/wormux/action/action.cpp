/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2009 Wormux Team.
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

//-----------------------------------------------------------------------------
#include <SDL_net.h>
#include <WORMUX_action.h>
#include <WORMUX_distant_cpu.h>
#include <WORMUX_error.h>
//-----------------------------------------------------------------------------

Action::Action(Action_t type)
{
  Init(type);
}

Action::Action(Action_t type, int value)
{
  Init(type);
  Push(value);
}

Action::Action(Action_t type, double value)
{
  Init(type);
  Push(value);
}

Action::Action(Action_t type, const std::string& value)
{
  Init(type);
  Push(value);
}

Action::Action(Action_t type, double value1, double value2)
{
  Init(type);
  Push(value1);
  Push(value2);
}

Action::Action(Action_t type, double value1, int value2)
{
  Init(type);
  Push(value1);
  Push(value2);
}

// Build an action from a network packet
Action::Action(const char *buffer, DistantComputer* _creator)
{
  creator = _creator;

  var.clear();
  m_type = (Action_t)SDLNet_Read32(buffer);
  buffer += 4;
  m_timestamp = (uint)SDLNet_Read32(buffer);
  buffer += 4;
  int m_length = SDLNet_Read32(buffer);
  buffer += 4;

  for(int i=0; i < m_length; i++)
  {
    uint32_t val = SDLNet_Read32(buffer);
    var.push_back(val);
    buffer += 4;
  }
}

void Action::Init(Action_t type)
{
  m_type = type;
  var.clear();
  m_timestamp = Action_TimeStamp();
  creator = NULL;
}

void Action::WriteTo(char *os) const
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
}

// Convert the action to a packet
void Action::WriteToPacket(char* &packet, int & size) const
{
  size = GetSize();
  packet = (char*)malloc(size);
  WriteTo(packet);
}

//-------------  Add datas to the action  ----------------
void Action::Push(int val)
{
  uint32_t tmp;
  memcpy(&tmp, &val, 4);
  var.push_back(tmp);
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
}

//-------------  Retrieve datas from the action  ----------------
int Action::PopInt()
{
  NET_ASSERT(var.size() > 0)
  {
    if(creator) creator->ForceDisconnection();
    return 0;
  }

  int val;
  uint32_t tmp = var.front();
  memcpy(&val, &tmp, 4);
  var.pop_front();

  return val;
}

double Action::PopDouble()
{
  NET_ASSERT(var.size() > 0)
  {
    if(creator) creator->ForceDisconnection();
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

  return val;
}

std::string Action::PopString()
{
  NET_ASSERT(var.size() > 1)
  {
    if(creator) creator->ForceDisconnection();
    return "";
  }

  int length = PopInt();

  std::string str="";

  NET_ASSERT((int)var.size() >= length/4)
  {
    if(creator) creator->ForceDisconnection();
    return "";
  }

  while(length > 0)
  {
    NET_ASSERT(var.size() > 0)
    {
      if(creator) creator->ForceDisconnection();
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

//-----------------------------------------------------------------------------

bool Action::IsEmpty() const
{
  return var.empty();
}

DistantComputer* Action::GetCreator() const
{
  return creator;
}

int Action::GetSize() const
{
  return 4  //Size of the type;
    + 4 //Size of the timestamp
    + 4 //Size of the number of variable
    + int(var.size()) * 4;
}

uint Action::GetTimestamp() const
{
  return m_timestamp;
}

Action::Action_t Action::GetType() const
{
  return m_type;
}


