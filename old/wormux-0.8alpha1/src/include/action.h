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

#ifndef ACTION_H
#define ACTION_H
//-----------------------------------------------------------------------------
#include <SDL.h>
#include <string>
#include <iostream>
#include <list>
#include "base.h"
#include "enum.h"
//-----------------------------------------------------------------------------

class Action
{
private:
  std::list<Uint32> var;
protected:
  Action_t m_type;
public:
  // Action without parameter
  Action (Action_t type);
  // Action with various parameter
  Action (Action_t type, int value);
  Action (Action_t type, double value);
  Action (Action_t type, double value1, int value2);
  Action (Action_t type, const std::string& value);

  // Build an action from a network packet
  Action (const char* is);

  ~Action();

  std::ostream& out(std::ostream &os) const;
  // Push / Back functions to add / retreive datas
  // Work as a FIFO container, inspiteof the name of methods !
  void Push(int val);
  void Push(double val);
  void Push(std::string val);
  int PopInt();
  double PopDouble();
  std::string PopString();

  void WritePacket(char* & packet, int & size);
  Action_t GetType() const;
};

//-----------------------------------------------------------------------------
// Output action in a ostream (for debug)
std::ostream& operator<<(std::ostream& os, const Action &a);

//-----------------------------------------------------------------------------
#endif
