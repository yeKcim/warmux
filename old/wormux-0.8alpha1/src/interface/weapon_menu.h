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
 * Interface affichant différentes informations sur la jeu.
 *****************************************************************************/

#ifndef WEAPON_MENU_H
#define WEAPON_MENU_H

#include "../graphic/sprite.h"
#include "../include/base.h"
#include "../character/character.h"
#include "../team/team.h"
#include "../weapon/weapon.h"
#include <vector>

class WeaponMenuItem
{
public:
  Point2i position;
  double scale;
  Weapon* weapon;
  Sprite *weapon_icon;
  uint zoom_start_time;
  uint weapon_type;

private:
  bool zoom, dezoom;

public:
  WeaponMenuItem(uint num_sort);
  void Reset();

  void Draw();
  void ChangeZoom();

  bool MouseOn(const Point2i &mousePos);

private:
  void ComputeScale();
};

class WeaponsMenu
{
public:
  Sprite *my_button1;
  Sprite *my_button2;
  Sprite *my_button3;
  Sprite *my_button4;
  Sprite *my_button5;

private:
  std::vector<WeaponMenuItem> boutons;
  typedef std::vector<WeaponMenuItem>::iterator iterator;
  typedef std::vector<WeaponMenuItem>::const_iterator const_iterator;

  bool display;
  bool show; // True during the motion to show the weapon menu.
  bool hide; // True during the motion to hide the weapon menu.

  uint motion_start_time;

  uint nbr_weapon_type; //nombre de type d'arme = nbr de colonnes
  uint max_weapon;  //nombre max d'arme dans les differents type = nbr de lignes

public:
  WeaponsMenu();

  // Renvoie true si un bouton a été cliqué
  bool ActionClic(const Point2i &mousePos);

  void Draw();
  void Reset();

  int GetX() const;
  int GetY() const;
  Point2i GetPosition() const;
  int GetWidth() const;
  int GetHeight() const;
  Point2i GetSize() const;
  bool IsDisplayed() const;

  void NewItem(Weapon* new_item, uint num_sort);
  void SwitchDisplay();
  void Hide();

  void MouseOver(const Point2i &mousePos);

private:
  void ComputeSize();
  void Show();
  void DrawBouton(iterator bouton);
  void ShowMotion(int nr_bottons, int button_no, iterator it, int column);
  bool HideMotion(int nr_buttons, int button_no, iterator it, int column);
};

#endif
