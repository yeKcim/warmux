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
 * Caisse de bonus : tombe du ciel après un temps d'apparition aléatoire.
 * Lorsqu'un ver touche la caisse, il peut gagner différents bonus : munition,
 * énergie (gain ou perte!), etc.
 *****************************************************************************/

#ifndef CAISSES_H
#define CAISSES_H
//-----------------------------------------------------------------------------
#include <SDL.h>
#include "../include/base.h"
#include "../object/physical_obj.h"
#include "../team/team.h"
//-----------------------------------------------------------------------------

class Caisse : public PhysicalObj
{
private:
  bool parachute; 
  bool affiche;
  bool desactive;
  bool pos_valide;

  Sprite *anim;
  uint temps_caisse;
  int bonus;

  enum
  {
    // Si vous touchez à cet enum, modifiez aussi nbr_bonus_diff
    bonusDYNAMITE=1,
    bonusTELEPORTE,
    bonusENERGIE,
    bonusPIEGE,
    bonusAERIENNE,
    bonusBAZ_TETE_C
  } bonus_armes;
  static const uint nbr_bonus_diff = bonusBAZ_TETE_C;

public:
  // Initialise les données
  Caisse();
  void Init();
  void FreeMem();
  void Reset();

  // Active les caisses ?
  void Active (bool actif);

  // Applique le bonus à l'équipe qui l'a gagné
  void AppliqueBonus (Team &team, Character &character);

  // Signale la fin d'une chute
  virtual void SignalFallEnding();  

  // Signale un changement d'etat
  virtual void SignalGhostState (bool etait_mort);

  void Draw();
  void Refresh();
  bool FaitApparaitre();
};

extern Caisse caisse;
//-----------------------------------------------------------------------------
#endif
