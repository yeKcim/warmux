/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
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
#include <SDL.h>
#include "action_handler.h"
//-----------------------------------------------------------------------------

// Copy b(unknown type, 32bits) to a(Uint32), bit for bit
#define TO_UINT32(a, b) a=*((Uint32*)(&(b)))
// Copy b(Uint32) to a(unknown type,32bits), bit for bit
#define FROM_UINT32(a, b) *((Uint32*)(&(a)))=b

// Copy b(unknown type, 64bits) to a(Uint32), bit for bit
#define TO_2UINT32(a, b) a=*((Uint32*)(&(b))); \
                         *((&a)+1)=*(((Uint32*)(&(b)))+1)

// Copy b(Uint32) to a(unknown type,64bits), bit for bit
#define FROM_2UINT32(a, b) *((Uint32*)(&(a)))=b; \
                           *(((Uint32*)(&(a)))+1)=*((&b)+1); \

Action::Action (Action_t type) 
{ 
  m_type = type; 
}

Action::~Action () {}

Action_t Action::GetType() const 
{ 
  return m_type; 
}

void Action::Write(Uint32* os) const 
{ 
  TO_UINT32(os[0],m_type);
}

Action* Action::clone() const 
{ 
  return new Action(*this); 
}

std::ostream& Action::out(std::ostream &os) const
{
  os << ActionHandler::GetInstance()->GetActionName(m_type);
  return os;
}

//-----------------------------------------------------------------------------

ActionInt2::ActionInt2 (Action_t type, int v1, int v2) : Action(type) 
{ 
  m_value1 = v1; m_value2 = v2; 
}

ActionInt2::ActionInt2(Action_t type, Uint32* is) : Action(type)
{ 
  FROM_UINT32(m_value1, is[0]);
  FROM_UINT32(m_value2, is[1]);
}

int ActionInt2::GetValue1() const 
{ 
  return m_value1; 
}

int ActionInt2::GetValue2() const 
{ 
  return m_value2; 
}

void ActionInt2::Write(Uint32* os) const 
{ 
  Action::Write(os);
  TO_UINT32(os[1], m_value1);
  TO_UINT32(os[2], m_value2);
}

Action* ActionInt2::clone() const { return new ActionInt2(*this); }
std::ostream& ActionInt2::out(std::ostream &os) const
{
  Action::out (os);
  os <<  " (2x int) = " << m_value1 << ", " << m_value2;
  return os;
}

//-----------------------------------------------------------------------------

ActionInt::ActionInt (Action_t type, int value) : Action(type) 
{ m_value = value; }
ActionInt::ActionInt (Action_t type, Uint32* is) : Action(type)
{ 
  FROM_UINT32(m_value, is[0]);
}
int ActionInt::GetValue() const { return m_value; }
void ActionInt::Write(Uint32* os) const 
{ 
  Action::Write(os);
  TO_UINT32(os[1], m_value);
}
Action* ActionInt::clone() const { return new ActionInt(*this); }
std::ostream& ActionInt::out(std::ostream &os) const
{
  Action::out (os);
  os << " (int) = " << m_value;
  return os;
}

//-----------------------------------------------------------------------------

ActionDouble::ActionDouble (Action_t type, double value) : Action(type) 
{ m_value = value; }
ActionDouble::ActionDouble (Action_t type, Uint32* is) : Action(type)
{ 
  FROM_2UINT32(m_value, is[0]);
}
double ActionDouble::GetValue() const { return m_value; }
void ActionDouble::Write(Uint32* os) const 
{ 
  Action::Write(os);
  TO_2UINT32(os[1], m_value);
}
Action* ActionDouble::clone() const { return new ActionDouble(*this); }
std::ostream& ActionDouble::out(std::ostream &os) const
{
  Action::out (os);
  os << " (double) = " << m_value;
  return os;
}

//-----------------------------------------------------------------------------

ActionDoubleInt::ActionDoubleInt (Action_t type, double v1, int v2) : Action(type) 
{ 
  m_value1 = v1; 
  m_value2 = v2; 
}

ActionDoubleInt::ActionDoubleInt(Action_t type, Uint32* is) : Action(type)
{
  assert( sizeof(m_value1) == 8);
//  ((Uint32*)&m_value1)[0] = is[0];
//  ((Uint32*)&m_value1)[1] = is[1];
  FROM_2UINT32(m_value1, is[0]);
  FROM_UINT32(m_value2, is[2]);
}

double ActionDoubleInt::GetValue1() const 
{ 
  return m_value1; 
}

int ActionDoubleInt::GetValue2() const 
{ 
  return m_value2; 
}

void ActionDoubleInt::Write(Uint32* os) const
{ 
  Action::Write(os);
//  os[1] = ((Uint32*)&m_value1)[0];
//  os[2] = ((Uint32*)&m_value1)[1];
  TO_2UINT32(os[1], m_value1);
  TO_UINT32(os[3], m_value2);
}

Action* ActionDoubleInt::clone() const 
{ 
  return new ActionDoubleInt(*this); 
}

std::ostream& ActionDoubleInt::out(std::ostream &os) const
{
  Action::out (os);
  os << " (double, int) = " << m_value1 << ", " << m_value2;
  return os;
}

//-----------------------------------------------------------------------------

ActionString::ActionString (Action_t type, const std::string &value) : Action(type)
{
  m_length = strlen(value.c_str())+1;
  m_value = new char[m_length];
  strcpy(m_value, value.c_str());
}

ActionString::ActionString (Action_t type, Uint32 *is) : Action(type)
{
  m_length = is[0];
  m_value = new char[m_length];
  strcpy(m_value,(char*)(&is[1]));
}

ActionString::~ActionString ()
{
  delete []m_value;
}

char* ActionString::GetValue() const { return m_value; }
void ActionString::Write(Uint32 *os) const 
{ 
  Action::Write(os);
  os[1] = m_length;
  strcpy((char*)(&os[2]),m_value);
}

Action* ActionString::clone() const { return new ActionString(m_type, std::string(m_value)); }
std::ostream& ActionString::out(std::ostream &os) const
{
  Action::out (os);
  os << " (string) = " << m_value;
  return os;
}

//-----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream &os, const Action &a)
{
  a.out(os);
  return os;
}

//-----------------------------------------------------------------------------
