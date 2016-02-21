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
 * Mine : Il s'agit de mines capables de detecter la presence d'un ver. Si 
 * c'est le cas, elle s'arme et là... faut pas y toucher ;) Apres un temps si
 * elle ne detecte personne elle se desarme.
 *****************************************************************************/
#ifndef MINE_H
#define MINE_H
//-----------------------------------------------------------------------------
#include <SDL.h>
#include "../graphic/sprite.h"
#include "../include/base.h"
#include "../object/physical_obj.h"
#include "../team/character.h"
#include "weapon.h"

//-----------------------------------------------------------------------------
namespace Wormux {
//-----------------------------------------------------------------------------

     
class ObjMine : public PhysicalObj
{
private:
  // channel used for sound
  int channel;

  // La mine est affichée ?
  bool affiche;

  // Activation des mines ?
  bool animation;//,repos;
  SDL_Surface *impact;
  Sprite *detection, *explosion;
  uint attente;
  uint armer;
  uint depart;

  bool non_defectueuse;

  Character *ver_declancheur;

public:
  ObjMine();
  void Init();
  void Reset();
  void Explosion ();
  void Draw();
  void ActiveDetection();
  void DesactiveDetection();
  void Refresh();
  void Detection();
  virtual void SignalFallEnding();
  virtual void SignalGhostState (bool etait_mort);
};

//-----------------------------------------------------------------------------

class MineConfig : public ExplosiveWeaponConfig
{ 
public: 
  double detection_range;
  uint temps_fuite;
public:
  MineConfig();
  virtual void LoadXml(xmlpp::Element *elem);
};

//-----------------------------------------------------------------------------

class Mine : public Weapon
{
private:
  std::vector<Mine> liste;
  bool already_put;
  uint fuite;

  bool p_Shoot();
  void Add (int x, int y);

public:
  Mine();
  void Refresh();

  MineConfig& cfg();
};


extern Mine mine;
//-----------------------------------------------------------------------------
} // namespace Wormux
#endif
