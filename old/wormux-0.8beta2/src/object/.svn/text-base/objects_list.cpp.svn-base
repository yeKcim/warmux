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
 * List of objects that are displayed.
 *****************************************************************************/

#include "object/objects_list.h"
//-----------------------------------------------------------------------------
#include "barrel.h"
#include "include/app.h"
#include "game/game_loop.h"
#include "map/map.h"
#include "map/maps_list.h"
#include "map/camera.h"
#include "tool/debug.h"
//#include "tool/random.h"
#include "tool/rectangle.h"
#include "game/time.h"
#include "weapon/mine.h"
#include <vector>
#include <iostream>

//-----------------------------------------------------------------------------
ObjectsList lst_objects;
//-----------------------------------------------------------------------------

// Initialise la liste des objets standards
void ObjectsList::PlaceMines()
{
  MSG_DEBUG("lst_objects","Placing mines");
  for (uint i=0; i<ActiveMap().GetNbMine(); ++i)
  {
    ObjMine *obj = new ObjMine(*MineConfig::GetInstance());

    if (obj->PutRandomly(false, MineConfig::GetInstance()->detection_range * PIXEL_PER_METER *1.5 ))
      // detection range is in meter
      push_back(obj);
    else
      delete obj;
  }
}

void ObjectsList::PlaceBarrels()
{
  MSG_DEBUG("lst_objects","Placing barrels");
  for (uint i= 0; i<ActiveMap().GetNbBarrel(); ++i)
  {
    PetrolBarrel *obj = new PetrolBarrel();

    if (obj->PutRandomly(false, 20.0))
      push_back(obj);
    else
      delete obj;
  }
}


//-----------------------------------------------------------------------------
void ObjectsList::Refresh()
{
  ObjectsList::iterator object=begin();

  while(object != end())
  {
    (*object)->UpdatePosition();
    (*object)->Refresh();
    if((*object)->IsGhost()) {
      // Stop following this object, remove from overlapse reference then delete it.
      Camera::GetInstance()->GetInstance()->StopFollowingObj(*object);
      RemoveOverlappedObjectReference(*object);
      delete (*object);
      object = erase(object);
    } else {
      object++;
    }
  }
}

//-----------------------------------------------------------------------------
void ObjectsList::Draw()
{
  for (ObjectsList::iterator it = begin();
       it != end();
       ++it)
  {
    ASSERT((*it) != NULL);

    if (!(*it)->IsGhost())
      (*it)->Draw();
  }
}

//-----------------------------------------------------------------------------
bool ObjectsList::AllReady() const
{
  FOR_EACH_OBJECT(object)
  {
    if (!(*object)->IsImmobile())
    {
      MSG_DEBUG("lst_objects", "\"%s\" is not ready ( IsImmobile()==fasle )", (*object)->GetName().c_str());
      return false;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------

void ObjectsList::FreeMem()
{
  ObjectsList::iterator object;
  for (object = begin();
       object != end();
       ++object) {
    if((*object))
      delete (*object);
  }
  clear();
}

//-----------------------------------------------------------------------------

void ObjectsList::RemoveOverlappedObjectReference(const PhysicalObj * obj)
{
  for(iterator it = overlapped_objects.begin(); it != overlapped_objects.end(); it ++) {
    if((*it)->GetOverlappingObject() == obj) {
      MSG_DEBUG("lst_objects", "removing overlapse reference of \"%s\" in \"%s\"",
                obj->GetName().c_str(), (*it)->GetName().c_str());
      (*it)->SetOverlappingObject(NULL);
      it = overlapped_objects.erase(it);
    }
  }
}
