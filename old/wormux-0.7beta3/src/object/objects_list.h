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
#include <vector>
//-----------------------------------------------------------------------------

// Boucle pour chaque objet de la liste des objets (non fantome)
#define POUR_TOUS_OBJETS(objet) \
  for (ListeObjets::iterator objet=lst_objets.Debut(), \
       fin_pour_chaque_objet=lst_objets.Fin(); \
       objet != fin_pour_chaque_objet; \
       ++objet)

//-----------------------------------------------------------------------------

// Boucle pour chaque objet de la liste des objets (non fantome)
#define POUR_CHAQUE_OBJET(objet) \
  POUR_TOUS_OBJETS(objet) \
  if (!objet -> ptr -> IsGhost())

//-----------------------------------------------------------------------------

class ListeObjets
{
public:
  typedef struct objet_t {
    PhysicalObj* ptr;
    bool efface;
    objet_t(PhysicalObj* o, bool e) { ptr = o; efface = e; }
  } objet_t;
  typedef std::vector<objet_t>::iterator iterator;

private:
  std::vector<objet_t> liste;

public:
  // Remise à zéro des objets standards
  void CreeListe();
  void VideListe();
  void Reset();
  void Init();

  // Ajoute un objet à la liste
  void AjouteObjet (PhysicalObj *ptr_obj, bool efface_fin_partie);

  // Retire un objet de la liste
  void RetireObjet (PhysicalObj *ptr_obj);

  // Refresh des tous les objets
  void Refresh();

  // Draw tous les objets
  void Draw();

  // Tous les objets sont prêts ? (ou alors un objet est en cours
  // d'animation ?)
  bool TousReady();

  // Début/fin de la liste
  iterator Debut() { return liste.begin(); }
  iterator Fin() { return liste.end(); }
};

extern ListeObjets lst_objets;
//-----------------------------------------------------------------------------
#endif
