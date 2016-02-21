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
 * Stores most of the object playing in the game
 *****************************************************************************/

#ifndef OBJECTS_LIST_H
#define OBJECTS_LIST_H
//-----------------------------------------------------------------------------
#include "include/base.h"
#include "object/physical_obj.h"
#include <list>
//-----------------------------------------------------------------------------

// Loop for all objects
#define FOR_ALL_OBJECTS(object) \
  for (ObjectsList::iterator object=lst_objects.begin(), \
       end=lst_objects.end(); \
       object != end; \
       ++object)

//-----------------------------------------------------------------------------

// Loop for all objects that aren't out of the screen
#define FOR_EACH_OBJECT(object) \
  FOR_ALL_OBJECTS(object) \
  	if (!(*object)->IsGhost())

//-----------------------------------------------------------------------------

class ObjectsList : public std::list<PhysicalObj*>
{
  public:
    typedef std::list<PhysicalObj*>::iterator iterator;
    std::list<PhysicalObj*> overlapped_objects;
  public:
    ~ObjectsList() { FreeMem(); };
    inline void AddObject(PhysicalObj * obj) { push_back(obj);};

    // Call the Refresh method of all the objects
    void Refresh();
    // Call the Draw method of all the objects
    void Draw();

    bool AllReady() const;

    // Place mines randomly on the map
    void PlaceMines();
    // Place barrels randomly on the map
    void PlaceBarrels();

    void FreeMem();

    // Overlapse handling
    inline void RemoveObject(PhysicalObj * obj)
    {
      remove(obj);
      RemoveOverlappedObjectReference(obj);
    };
    void AddOverlappedObject(PhysicalObj * obj) { overlapped_objects.push_back(obj); };
    void RemoveOverlappedObjectReference(const PhysicalObj * obj);
    void RemoveOverlappedObject(PhysicalObj * obj) { overlapped_objects.remove(obj); };
};

extern ObjectsList lst_objects;
//-----------------------------------------------------------------------------
#endif
