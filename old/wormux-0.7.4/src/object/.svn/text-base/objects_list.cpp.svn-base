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
 * Liste des objets qui sont sur le plateau de jeu.
 *****************************************************************************/

#include "../object/objects_list.h"
//-----------------------------------------------------------------------------
#include "../include/app.h"
#include "../game/game_loop.h"
#include "../map/map.h"
#include "../map/maps_list.h"
#include "../map/camera.h"
#include "../tool/random.h"
#include "../tool/rectangle.h"
#include "../weapon/mine.h"
#include <vector>

//-----------------------------------------------------------------------------
ObjectsList lst_objects;
//-----------------------------------------------------------------------------

// Initialise la liste des objets standards
void ObjectsList::Init()
{
  lst.clear();

  for (uint i=0; i<lst_terrain.TerrainActif().nb_mine; ++i)
  {
    ObjMine *obj = new ObjMine(*MineConfig::GetInstance());

    if (obj->PutRandomly(false, MineConfig::GetInstance()->detection_range * 40 *1.5 ))
      // 40 is current PIXEL_PER_METER and detection range is in meter
      AddObject (obj);
    else
      delete obj;
  }
}

//-----------------------------------------------------------------------------

void ObjectsList::AddObject (PhysicalObj* obj)
{
  lst.push_back (object_t(obj,false));
}

ObjectsList::~ObjectsList()
{

  std::list<object_t>::iterator object;
  for (object = lst.begin();
       object != lst.end();
       ++object) {
    if((*object).ptr)
      delete (*object).ptr;
  }
}

//-----------------------------------------------------------------------------

void ObjectsList::RemoveObject (PhysicalObj* obj)
{
  FOR_EACH_OBJECT(it)
  {
    if ( it->ptr == obj)
    {
      it->to_remove = true;
      // please, do not call camera.StopFollowingObj(obj) here,
      // because we want to see the end of an explosion when removing projectiles
      return;
    }
  }
}

//-----------------------------------------------------------------------------

void ObjectsList::Refresh()
{
  ObjectsList::iterator object=lst_objects.Begin();

  while(object != lst_objects.End())
  {
    if (!object->to_remove && !object->ptr->IsGhost()) {
      object->ptr->UpdatePosition();
      object->ptr->Refresh();
      object++;
    } else {
      object = lst.erase(object);
    }
  }
}

//-----------------------------------------------------------------------------

void ObjectsList::Draw()
{
  FOR_EACH_OBJECT(object) object->ptr->Draw ();
}


//-----------------------------------------------------------------------------

// Tous les objets sont prêts ? (ou alors un objet est en cours
// d'animation ?)
bool ObjectsList::AllReady()
{
  FOR_EACH_OBJECT(object)
  {
    if (!object->ptr->IsReady()) return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
