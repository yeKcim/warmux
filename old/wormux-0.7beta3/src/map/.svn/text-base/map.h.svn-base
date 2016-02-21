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
 * Monde ou plateau de jeu.
 *****************************************************************************/

#ifndef MONDE_H
#define MONDE_H
//-----------------------------------------------------------------------------
#include "../include/base.h"
#include "../graphic/text.h"
#include "../object/physical_obj.h"
#include "sky.h"
#include "water.h"
#include "ground.h"
//-----------------------------------------------------------------------------

extern const uint MAX_WIND_OBJECTS;

class Map
{
 private:
  Text * author_info1;
  Text * author_info2;

public:
  Map();
  ~Map();
   
  Wormux::Ground ground;
  Wormux::Sky sky;
  double dst_min_entre_vers;
  Water water;

  std::list<Rectanglei> *to_redraw;
  std::list<Rectanglei> *to_redraw_now;
  std::list<Rectanglei> *to_redraw_particles;
  std::list<Rectanglei> *to_redraw_particles_now;

public:
  //void Init();
  void Reset();
  void Refresh();
  void FreeMem();
  void Draw();
  void DrawWater();
  void DrawSky();
  void DrawAuthorName();

  // To manage the cache mechanism
  void ToRedrawOnMap(Rectanglei r);
  void ToRedrawOnScreen(Rectanglei r);

  // Est-on dans le monde ou dans le vide ?
  bool EstDansVide (int x, int y);
  bool LigneH_EstDansVide (int left, int y,  int right);
  bool LigneV_EstDansVide (int x,  int top, int bottom);
  bool RectEstDansVide (const Rectanglei &rect);

  // La ligne du haut/bas d'un objet physique est dans le vide ?
  // Le test se fait sur le rectangle de test décalé de dx et dy.
  bool EstDansVide_haut (const PhysicalObj &obj, int dx, int dy);
  bool EstDansVide_bas (const PhysicalObj &obj, int dx, int dy);
  bool IsInVacuum_left (const PhysicalObj &obj, int dx, int dy);
  bool IsInVacuum_right (const PhysicalObj &obj, int dx, int dy);

  // Est en dehors du monde ?
  bool EstHorsMondeX (int x) const;
  bool EstHorsMondeY (int x) const;
  bool EstHorsMondeXlarg (int x, uint larg) const;
  bool EstHorsMondeYhaut (int x, uint haut) const;
  bool EstHorsMondeXY (int x, int y) const;
  bool EstHorsMonde (const Point2i &pos) const;

  // C'est un terrain ouvert ?
  bool EstOuvert() const { return ground.EstOuvert(); }

  // Creuse un pixel
  void Creuse(uint x, uint y, SDL_Surface *alpha_sur);
   
  // Lit la taille du monde
  uint GetWidth() const { return ground.GetWidth(); }
  uint GetHeight() const { return ground.GetHeight(); }
 private:
  void SwitchDrawingCache();
  void SwitchDrawingCacheParticles();
};

extern Map world;
//-----------------------------------------------------------------------------
#endif
