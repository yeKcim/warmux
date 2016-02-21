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
 * Define all Wormux actions.
 *****************************************************************************/

#include "action.h"
//-----------------------------------------------------------------------------
#include <SDL_net.h>
#include "action_handler.h"
#include "tool/debug.h"
#include "game/time.h"
#include "character/character.h"
#include "network/distant_cpu.h"
#include "team/teams_list.h"
#include "network/network.h"
//-----------------------------------------------------------------------------
// Action without parameter
Action::Action (Action_t type)
{
  var.clear();
  m_type = type;
  m_timestamp = Time::GetInstance()->Read();
  creator = NULL;
}

// Action with various parameters
Action::Action (Action_t type, int value) : m_type(type)
{
  var.clear();
  Push(value);
  m_timestamp = Time::GetInstance()->Read();
}

Action::Action (Action_t type, double value) : m_type(type)
{
  var.clear();
  Push(value);
  m_timestamp = Time::GetInstance()->Read();
  creator = NULL;
}

Action::Action (Action_t type, const std::string& value) : m_type(type)
{
  var.clear();
  Push(value);
  m_timestamp = Time::GetInstance()->Read();
  creator = NULL;
}

Action::Action (Action_t type, double value1, double value2) : m_type(type)
{
  var.clear();
  Push(value1);
  Push(value2);
  m_timestamp = Time::GetInstance()->Read();
  creator = NULL;
}

Action::Action (Action_t type, double value1, int value2) : m_type(type)
{
  var.clear();
  Push(value1);
  Push(value2);
  m_timestamp = Time::GetInstance()->Read();
  creator = NULL;
}

// Build an action from a network packet
Action::Action (const char *is, DistantComputer* _creator)
{
  creator = _creator;

  var.clear();
  m_type = (Action_t)SDLNet_Read32(is);
  is += 4;
  m_timestamp = (Action_t)SDLNet_Read32(is);
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

Action::Action_t Action::GetType() const
{
  return m_type;
}

bool Action::IsEmpty() const
{
  return var.empty();
}

void Action::SetTimestamp(uint timestamp)
{
  m_timestamp = timestamp;
}

uint Action::GetTimestamp()
{
  return m_timestamp;
}

// Convert the action to a packet
void Action::WritePacket(char* &packet, int & size)
{
  size = 4  //Size of the type;
        + 4 //Size of the timestamp
        + 4 //Size of the number of variable
        + int(var.size()) * 4;

  packet = (char*)malloc(size);
  char* os = packet;

  SDLNet_Write32(m_type, os);
  os += 4;
  SDLNet_Write32(m_timestamp, os);
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
  net_assert(var.size() > 0)
  {
    if(creator) creator->force_disconnect = true;
    return 0;
  }

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
  net_assert(var.size() > 0)
  {
    if(creator) creator->force_disconnect = true;
    return 0.0;
  }

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
  net_assert(var.size() > 1)
  {
    if(creator) creator->force_disconnect = true;
    return "";
  }

  int lenght = PopInt();

  std::string str="";

  net_assert((int)var.size() >= lenght/4)
  {
    if(creator) creator->force_disconnect = true;
    return "";
  }

  while(lenght > 0)
  {
    net_assert(var.size() > 0)
    {
      if(creator) creator->force_disconnect = true;
      return "";
    }

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

//-------------  Send/Retrieve datas about Character

void Action::StoreActiveCharacter()
{
  StoreCharacter(ActiveCharacter().GetTeamIndex() ,ActiveCharacter().GetCharacterIndex());
}

void Action::StoreCharacter(uint team_no, uint char_no)
{
  Push((int)team_no);
  Push((int)char_no);
  Character * c = teams_list.FindPlayingByIndex(team_no)->FindByIndex(char_no);
  Push(c->GetPosition());
  Push((int)c->GetDirection());
  Push(c->GetAbsFiringAngle());
  Push(c->GetEnergy());
  Push((int)c->GetLifeState());
  Push((int)c->GetDiseaseDamage());
  Push((int)c->GetDiseaseDuration());
  Push(c->GetSpeed());
  Push(c->GetExternForce());
  Push(c->GetRopeAngle());
  Push(c->GetRopeLength());
  if(c->IsActiveCharacter()) { // If active character, store step animation
    Push((int)true);
    Push(ActiveTeam().ActiveCharacter().GetBody()->GetClothe());
    Push(ActiveTeam().ActiveCharacter().GetBody()->GetMovement());
    Push((int)ActiveTeam().ActiveCharacter().GetBody()->GetFrame());
  } else {
    Push((int)false);
  }
}

void Action::RetrieveCharacter()
{
  int team_no = PopInt();
  int char_no = PopInt();
  Character * c = teams_list.FindPlayingByIndex(team_no)->FindByIndex(char_no);
  c->SetXY(PopPoint2i());
  c->SetDirection((Body::Direction_t)PopInt());
  c->SetFiringAngle(PopDouble());
  c->SetEnergy(PopInt());
  c->SetLifeState((alive_t)PopInt());
  int disease_damage_per_turn = PopInt();
  int disease_duration = PopInt();
  c->SetDiseaseDamage(disease_damage_per_turn, disease_duration);
  c->SetSpeedXY(PopPoint2d());
  c->SetExternForceXY(PopPoint2d());
  c->SetRopeAngle(PopDouble());
  c->SetRopeLength(PopDouble());
  if((bool)PopInt()) { // If active characters, retrieve stored animation
    if(c->GetTeam().IsActiveTeam())
      ActiveTeam().SelectCharacter(char_no);
    c->SetClothe(PopString());
    c->SetMovement(PopString());
    c->GetBody()->SetFrame((uint)PopInt());

    c->GetBody()->UpdateWeaponPosition(c->GetPosition());
  }
}


//-----------------------------------------------------------------------------
