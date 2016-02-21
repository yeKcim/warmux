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
 * Liste des objets qui sont sur le plateau de jeu.
 *****************************************************************************/

#include "../object/objects_list.h"
//-----------------------------------------------------------------------------
#include "barrel.h"
#include "../include/app.h"
#include "../game/game_loop.h"
#include "../map/map.h"
#include "../map/maps_list.h"
#include "../map/camera.h"
#include "../tool/debug.h"
#include "../tool/random.h"
#include "../tool/rectangle.h"
#include "../game/time.h"
#include "../weapon/mine.h"
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

    if (obj->PutRandomly(false, MineConfig::GetInstance()->detection_range * 40 *1.5 ))
      // 40 is current PIXEL_PER_METER and detection range is in meter
      AddObject (obj);
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
      AddObject (obj);
    else
      delete obj;
  }
}

//-----------------------------------------------------------------------------

void ObjectsList::AddObject (PhysicalObj* obj)
{
  MSG_DEBUG("lst_objects","Adding \"%s\"(%p) to the object list", obj->GetName().c_str(), obj);
  FOR_ALL_OBJECTS(it)
    if ( it->ptr == obj) {
      MSG_DEBUG("lst_objects","Warning ! Adding \"%s\"(%p) 2 times to the object list", obj->GetName().c_str(), obj);
      assert(false);
      return;
    }
  lst.push_back (object_t(obj,false));
}

ObjectsList::~ObjectsList()
{
  FreeMem();
}

//-----------------------------------------------------------------------------

void ObjectsList::RemoveObject (PhysicalObj* obj)
{
  MSG_DEBUG("lst_objects","Object \"%s\"(%p) wants to be removed from the object list", obj->GetName().c_str(),obj);
  FOR_ALL_OBJECTS(it)
  {
    if ( it->ptr == obj)
    {
      it->to_remove = true;
      // please, do not call camera.StopFollowingObj(obj) here,
      // because we want to see the end of an explosion when removing projectiles
      return;
    }
  }
  // assert(false);
}

//-----------------------------------------------------------------------------
void ObjectsList::Refresh()
{
  ObjectsList::iterator object=lst_objects.Begin();

  while(object != lst_objects.End())
  {
    if (!object->to_remove) {
      if(object->ptr->IsGhost())
      {
#ifdef DEBUG
        std::cerr << "Warning, \"" << object->ptr->GetName() << "\" is ghost, and still in the the object list" << std::endl;
#endif
	camera.StopFollowingObj(object->ptr);
	delete object->ptr;
	object->ptr = NULL;
	object = lst.erase(object);
      }
      else
      {
        object->ptr->UpdatePosition();
        object->ptr->Refresh();
      }
    } else {
      MSG_DEBUG("lst_objects","Erasing object \"%s\" from the object list", object->ptr->GetName().c_str());
      // the following commented code causes other segfaults
      //camera.StopFollowingObj(object->ptr);
      //delete object->ptr;
      //object->ptr = NULL;
      object = lst.erase(object);
    }
    object++;
  }
}

//-----------------------------------------------------------------------------
void ObjectsList::Draw()
{
  for (ObjectsList::iterator it = lst.begin();
       it != lst.end();
       ++it)
  if(!it->to_remove)
  {
    assert(it->ptr != NULL);
    if(Time::GetInstance()->IsGamePaused())
    {
      MSG_DEBUG("lst_objects","Displaying %s",it->ptr->GetName().c_str());
    }
    if (!it->ptr->IsGhost())
      it->ptr->Draw();
  }
}

//-----------------------------------------------------------------------------
bool ObjectsList::AllReady()
{
  FOR_EACH_OBJECT(object)
  {
    if (!object->ptr->IsImmobile())
    {
      MSG_DEBUG("lst_objects", "\"%s\" is not ready ( IsImmobile()==fasle )", object->ptr->GetName().c_str());
      return false;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------

void ObjectsList::FreeMem()
{
  MSG_DEBUG("lst_objects", "Erasing object list");
  std::list<object_t>::iterator object;
  for (object = lst.begin();
       object != lst.end();
       ++object) {
    if((*object).ptr)
      delete (*object).ptr;
  }
  lst.clear();
}

