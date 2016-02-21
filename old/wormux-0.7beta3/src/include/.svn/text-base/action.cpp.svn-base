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
#include "action_handler.h"
//-----------------------------------------------------------------------------

Action::Action (Action_t type) 
{ 
   m_type = type; 
}

Action::~Action () {}

Action_t Action::GetType() const 
{ 
   return m_type; 
}

void Action::Write(CL_OutputSource &os) const 
{ 
#ifdef CL
   os.write_int32 (m_type); 
#else
   os << m_type;
#endif
}

Action* Action::clone() const 
{ 
   return new Action(*this); 
}

std::ostream& Action::out(std::ostream&os) const
{
	os << action_handler.GetActionName(m_type);
	return os;
}

//-----------------------------------------------------------------------------

ActionInt2::ActionInt2 (Action_t type, int v1, int v2) : Action(type) 
{ 
   m_value1 = v1; m_value2 = v2; 
}

ActionInt2::ActionInt2(Action_t type, CL_InputSource &is) : Action(type)
{ 
#ifdef CL
        m_value1 = is.read_int32 (); 
	m_value2 = is.read_int32 (); 
#else
   is >> m_value1;
   is >> m_value2;
#endif
}

int ActionInt2::GetValue1() const 
{ 
   return m_value1; 
}

int ActionInt2::GetValue2() const 
{ 
   return m_value2; 
}

void ActionInt2::Write(CL_OutputSource &os) const 
{ 
	Action::Write(os);
#ifdef CL
        os.write_int32 (m_value1); 
	os.write_int32 (m_value2); 
#else
   os << m_value1; 
   os << m_value2; 
#endif
}

Action* ActionInt2::clone() const { return new ActionInt2(*this); }
std::ostream& ActionInt2::out(std::ostream&os) const
{
	Action::out (os);
	os << " (2x int) = " << m_value1 << ", " << m_value2;
	return os;
}

//-----------------------------------------------------------------------------

ActionInt::ActionInt (Action_t type, int value) : Action(type) 
{ m_value = value; }
ActionInt::ActionInt (Action_t type, CL_InputSource &is) : Action(type)
{ 
#ifdef CL
   m_value = is.read_int32 (); 
#else
   is >> m_value;
#endif
}
int ActionInt::GetValue() const { return m_value; }
void ActionInt::Write(CL_OutputSource &os) const 
{ 
	Action::Write(os);
#ifdef CL
   os.write_int32 (m_value); 
#else
   os << m_value;
#endif
}
Action* ActionInt::clone() const { return new ActionInt(*this); }
std::ostream& ActionInt::out(std::ostream&os) const
{
	Action::out (os);
	os << " (int) = " << m_value;
	return os;
}

//-----------------------------------------------------------------------------

double read_double(CL_InputSource &is)
{
  double answer;
  std::cout << "read_double"<< std::endl;
   
#ifdef CL
   if ( is.read(&answer, sizeof(double)) != sizeof(double)) 
    throw CErreur("action.cpp", 91, "read_double() failed");
#else
   is >> answer;
#endif
   
  //if (little_endian_mode)
  //  {
#ifdef CL
   SWAP_IF_BIG(answer);
#endif
   //  }
      //else
      //{
      //SWAP_IF_LITTLE(answer);
      //}
  return answer;
}

void write_double(CL_OutputSource &os, double data)
{
  double final = data;
  std::cout << "write_double"<< std::endl;
//   if (little_endian_mode)
//     {
#ifdef CL
   SWAP_IF_BIG(final);
#endif
//     }
//   else
//     {
//       SWAP_IF_LITTLE(final);
//     }
#ifdef CL
   os.write(&final, sizeof(double));
#else
   os << final;
#endif
}

ActionDoubleInt::ActionDoubleInt (Action_t type, double v1, int v2) : Action(type) 
{ 
   m_value1 = v1; 
   m_value2 = v2; 
}

ActionDoubleInt::ActionDoubleInt(Action_t type, CL_InputSource &is) : Action(type)
{ 
#ifdef CL
        m_value1 = read_double (is); // CL_InputSource::read_double32 does not exist 
	m_value2 = is.read_int32 (); 
#else
   is >> m_value1;
   is >> m_value2;
#endif
}

double ActionDoubleInt::GetValue1() const 
{ 
   return m_value1; 
}

int ActionDoubleInt::GetValue2() const 
{ 
   return m_value2; 
}

void ActionDoubleInt::Write(CL_OutputSource &os) const 
{ 
	Action::Write(os);
	write_double (os, m_value1);  // CL_InputSource::write_double32 does not exist 
#ifdef CL
   os.write_int32 (m_value2);  
#else
   os << m_value2;
#endif
}

Action* ActionDoubleInt::clone() const 
{ 
   return new ActionDoubleInt(*this); 
}

std::ostream& ActionDoubleInt::out(std::ostream&os) const
{
	Action::out (os);
	os << " (double, int) = " << m_value1 << ", " << m_value2;
	return os;
}

//-----------------------------------------------------------------------------

ActionString::ActionString (Action_t type, const std::string &value) : Action(type) { m_value = value; }
ActionString::ActionString (Action_t type, CL_InputSource &is) : Action(type)
{ 
#ifdef CL
   m_value = is.read_string(); 
#else
   is >> m_value;
#endif
}
std::string ActionString::GetValue() const { return m_value; }
void ActionString::Write(CL_OutputSource &os) const 
{ 
	Action::Write(os);
#ifdef CL
   os.write_string (m_value); 
#else
   os << m_value;
#endif
}
Action* ActionString::clone() const { return new ActionString(*this); }
std::ostream& ActionString::out(std::ostream&os) const
{
	Action::out (os);
	os << " (string) = " << m_value;
	return os;
}

//-----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const Action &a)
{
	a.out(os);
	return os;
}

//-----------------------------------------------------------------------------
