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

#ifndef OBJETS_H
#define OBJETS_H
//-----------------------------------------------------------------------------
#include "../include/base.h"
#include "../object/physical_obj.h"
#include <list>
//-----------------------------------------------------------------------------

// Boucle pour chaque objet de la liste des objets (non fantome)
#define FOR_ALL_OBJECTS(object) \
  for (ObjectsList::iterator object=lst_objects.Begin(), \
       end=lst_objects.End(); \
       object != end; \
       ++object)

//-----------------------------------------------------------------------------

// Boucle pour chaque objet de la liste des objets (non fantome)
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
  void Init();

  // Ajoute un objet à la liste
  void AddObject (PhysicalObj* obj);

  // Retire un objet de la liste
  void RemoveObject (PhysicalObj* obj);

  // Refresh des tous les objets
  void Refresh();

  // Draw tous les objets
  void Draw();

  // Tous les objets sont prêts ? (ou alors un objet est en cours
  // d'animation ?)
  bool AllReady();

  // Début/fin de la liste
  iterator Begin() { return lst.begin(); }
  iterator End() { return lst.end(); }
};

extern ObjectsList lst_objects;
//-----------------------------------------------------------------------------
#endif
