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
 * Stores most of the object playing in the game
 *****************************************************************************/

#ifndef OBJECTS_LIST_H
#define OBJECTS_LIST_H
//-----------------------------------------------------------------------------
#include "../include/base.h"
#include "../object/physical_obj.h"
#include <list>
//-----------------------------------------------------------------------------

// Loop for all objects
#define FOR_ALL_OBJECTS(object) \
  for (ObjectsList::iterator object=lst_objects.Begin(), \
       end=lst_objects.End(); \
       object != end; \
       ++object) \
	if(!object->to_remove)

//-----------------------------------------------------------------------------

// Loop for all objects that aren't out of the screen
#define FOR_EACH_OBJECT(object) \
  FOR_ALL_OBJECTS(object) \
  if (!object->ptr->IsGhost())

//-----------------------------------------------------------------------------

class ObjectsList
{
public:
  typedef struct object_t {
    PhysicalObj* ptr;
    bool to_remove;
    object_t(PhysicalObj* o, bool e) { ptr = o; to_remove = e; }
  } object_t;

  typedef std::list<object_t>::iterator iterator;

private:
  std::list<object_t> lst;

public:
  ~ObjectsList();

  void AddObject (PhysicalObj* obj);
  void RemoveObject (PhysicalObj* obj);
  // Call the Refresh method of all the objects
  void Refresh();
  // Call the Draw method of all the objects
  void Draw();

  bool AllReady();

  iterator Begin() { return lst.begin(); }
  iterator End() { return lst.end(); }

  // Place mines randomly on the map
  void PlaceMines();
  // Place barrels randomly on the map
  void PlaceBarrels();

  void FreeMem();
};

extern ObjectsList lst_objects;
//-----------------------------------------------------------------------------
#endif
