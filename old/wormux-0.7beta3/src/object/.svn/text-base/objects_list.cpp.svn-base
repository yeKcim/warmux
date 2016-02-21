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
#include "../map/maps_list.h"
#include "../map/camera.h"
#ifdef CL
# include "../weapon/mine.h"
#endif
#include "bonus_box.h"
#include <vector>
using namespace Wormux;
//-----------------------------------------------------------------------------
ListeObjets lst_objets;
//-----------------------------------------------------------------------------

#undef POUR_CHAQUE_OBJET
#define POUR_CHAQUE_OBJET(objet) \
  for (ListeObjets::iterator objet=Debut(), \
       fin_pour_chaque_objet=Fin(); \
       objet != fin_pour_chaque_objet; \
       ++objet)

//-----------------------------------------------------------------------------

void ListeObjets::CreeListe()
{
  liste.clear();
}

//-----------------------------------------------------------------------------

void ListeObjets::VideListe()
{
  iterator it,actuel;
  for (it=liste.begin(); it != liste.end(); )
  {
    actuel = it;
    ++it;
    if (actuel -> efface)
    {
      bool fin = (it == liste.end());
      liste.erase (actuel);
      if (fin) break;
    }
  }
}

//-----------------------------------------------------------------------------

// Initialise la liste des objets standards
void ListeObjets::Init()
{
  CreeListe();
  POUR_CHAQUE_OBJET (objet) (*objet).ptr -> Init();
}

//-----------------------------------------------------------------------------

// Initialise la liste des objets standards
void ListeObjets::Reset()
{
  VideListe();

  for (uint i=0; i<lst_terrain.TerrainActif().nb_mine; ++i)
  {
#ifdef CL
    ObjMine *obj = new ObjMine();
    AjouteObjet (obj, true);
#endif
  }

  POUR_CHAQUE_OBJET(objet) (*objet).ptr -> Reset();
}

//-----------------------------------------------------------------------------

void ListeObjets::AjouteObjet (PhysicalObj *ptr_obj, bool efface_fin_partie)
{
  liste.push_back (objet_t(ptr_obj,efface_fin_partie));
}

//-----------------------------------------------------------------------------

void ListeObjets::RetireObjet (PhysicalObj *ptr_obj)
{
  POUR_CHAQUE_OBJET(it)
  {
    if (it -> ptr == ptr_obj) 
    {
      liste.erase (it);
      camera.StopFollowingObj(it->ptr);
      return;
    }
  }
}

//-----------------------------------------------------------------------------

void ListeObjets::Refresh()
{
  POUR_CHAQUE_OBJET(objet)
  {
    objet -> ptr -> UpdatePosition();
    objet -> ptr -> Refresh();
  }
}

//-----------------------------------------------------------------------------

void ListeObjets::Draw()
{
  POUR_CHAQUE_OBJET(objet) (*objet).ptr -> Draw ();
}


//-----------------------------------------------------------------------------

// Tous les objets sont prêts ? (ou alors un objet est en cours
// d'animation ?)
bool ListeObjets::TousReady()
{
  POUR_CHAQUE_OBJET(objet)
  {
    if (!objet -> ptr -> IsReady()) return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
