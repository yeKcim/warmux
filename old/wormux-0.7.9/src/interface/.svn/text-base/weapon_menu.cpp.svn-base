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
 * Interface showing various informations about the game.
 *****************************************************************************/

#include "weapon_menu.h"

#include <sstream>
#include <math.h>
#include "interface.h"
#include "../graphic/video.h"
#include "../graphic/font.h"
#include "../game/time.h"
#include "../include/action_handler.h"
#include "../include/app.h"
#include "../interface/mouse.h"
#include "../map/camera.h"
#include "../map/map.h"
#include "../team/team.h"
#include "../team/teams_list.h"
#include "../tool/point.h"
#include "../tool/rectangle.h"
#include "../tool/string_tools.h"
#include "../tool/resource_manager.h"
#include "../graphic/sprite.h"
#include "../weapon/weapon.h"
#include "../weapon/weapons_list.h"


// Weapon menu
const uint BUTTON_ICO_WIDTH = 58;  // Width of the button icon
const uint BUTTON_ICO_HEIGHT = 58; // Height of the button icon

const uint WEAPON_ICO_WIDTH = 48;   // Width of the weapon icon
const uint WEAPON_ICO_HEIGHT = 48;  // Height of the button icon

const uint BUTTON_ICO_GAP = 8; // Gap between buttons when a button is zoomed


const uint ICONS_DRAW_TIME = 600; // Time to display all icons (in ms)
const uint ICON_ZOOM_TIME = 150; // Time to zomm one icon.

const double DEFAULT_ICON_SCALE = 0.7;
const double MAX_ICON_SCALE = 1.1;

const uint BUTTON_WIDTH = (int)(BUTTON_ICO_GAP + BUTTON_ICO_WIDTH  *
                               (DEFAULT_ICON_SCALE+MAX_ICON_SCALE)/2);

const uint BUTTON_HEIGHT = (int)(BUTTON_ICO_GAP + BUTTON_ICO_HEIGHT  *
                                (DEFAULT_ICON_SCALE+MAX_ICON_SCALE)/2);

WeaponMenuItem::WeaponMenuItem(uint num_sort)
{
  zoom_start_time = 0;
  weapon_type = num_sort;
  Reset();
}

void WeaponMenuItem::Reset()
{
  scale = DEFAULT_ICON_SCALE;
  zoom = false;
  dezoom = false;
}

void WeaponMenuItem::ChangeZoom()
{
  zoom_start_time = Time::GetInstance()->Read();

  if(!zoom && scale < 1)
    {
      zoom = true;
      dezoom = false;
    }
  else
    {
      zoom = false;
      dezoom = true;
    }
}

void WeaponMenuItem::ComputeScale()
{
  double scale_range, time_range;

  time_range = ((double)Time::GetInstance()->Read() - zoom_start_time) / ICON_ZOOM_TIME;
  if (time_range > 1)
    time_range = 1;

  scale_range = sin (time_range * M_PI / 2) * (MAX_ICON_SCALE - DEFAULT_ICON_SCALE);

  if(zoom)
  {
    scale = DEFAULT_ICON_SCALE + scale_range ;

    if(time_range == 1)
      zoom = false;
  }
  else
    if(dezoom)
      {
        scale = MAX_ICON_SCALE - scale_range ;

        if(time_range == 1)
          dezoom = false;
      }
}

bool WeaponMenuItem::MouseOn(const Point2i &mousePos)
{
  ComputeScale();

  Point2i scaled( (int)(BUTTON_ICO_WIDTH * scale), (int)(BUTTON_ICO_HEIGHT * scale) );
  Rectanglei rect(Interface::GetInstance()->weapons_menu.GetPosition() - scaled/2 + position, scaled );

  if( rect.Contains(mousePos) )
     return true;
  else
    {
      if(scale > DEFAULT_ICON_SCALE && !dezoom)
        dezoom = true;
      return false;
    }
}

// Draw a button
void WeaponMenuItem::Draw()
{
  Interface * interface = Interface::GetInstance();

  ComputeScale();
  Point2i buttonCenter(interface->weapons_menu.GetPosition() + position);
  Point2i buttonSize( (int)(BUTTON_ICO_WIDTH * scale), (int)(BUTTON_ICO_HEIGHT * scale) );
  Point2i iconSize( (int)(WEAPON_ICO_WIDTH * scale), (int)(WEAPON_ICO_HEIGHT * scale) );
  std::ostringstream txt;
  int nb_bullets;

  Sprite *button;

  switch(weapon_type){
    case 1:
      button = interface->weapons_menu.my_button1;
      break ;

    case 2:
      button = interface->weapons_menu.my_button2;
      break ;

    case 3:
      button = interface->weapons_menu.my_button3;
      break ;

    case 4:
      button = interface->weapons_menu.my_button4;
      break ;

    case 5:
      button = interface->weapons_menu.my_button5;
      break ;

  default:
      button = interface->weapons_menu.my_button1;
      break ;
  }

  // Button display
  button->Scale(scale, scale);
  button->Blit(AppWormux::GetInstance()->video.window, buttonCenter - buttonSize/2);

  // Weapon display
  weapon_icon->Scale(scale, scale);
  weapon_icon->Blit(AppWormux::GetInstance()->video.window, buttonCenter - iconSize/2);

  // Amunitions display
  nb_bullets = ActiveTeam().ReadNbAmmos(weapon->GetName());
  txt.str ("");
  if (nb_bullets ==  INFINITE_AMMO)
    txt << ("§");
  else
    txt << nb_bullets;

  (*Font::GetInstance(Font::FONT_TINY)).WriteLeftBottom(buttonCenter + Point2i(-1, 1) * iconSize / 2,
			    txt.str(), white_color);
}

WeaponsMenu::WeaponsMenu()
{
  display = false;
  show = false;
  hide = false;
  nbr_weapon_type = 0;
  motion_start_time = Time::GetInstance()->Read();

  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);
  my_button1 = new Sprite( resource_manager.LoadImage(res,"interface/button1_icon"));
  my_button1->cache.EnableLastFrameCache();
  my_button2 = new Sprite( resource_manager.LoadImage(res,"interface/button2_icon"));
  my_button2->cache.EnableLastFrameCache();
  my_button3 = new Sprite( resource_manager.LoadImage(res,"interface/button3_icon"));
  my_button3->cache.EnableLastFrameCache();
  my_button4 = new Sprite( resource_manager.LoadImage(res,"interface/button4_icon"));
  my_button4->cache.EnableLastFrameCache();
  my_button5 = new Sprite( resource_manager.LoadImage(res,"interface/button5_icon"));
  my_button5->cache.EnableLastFrameCache();
  resource_manager.UnLoadXMLProfile( res);
}

// Add a new weapon to the weapon menu.
void WeaponsMenu::NewItem(Weapon* new_item, uint num_sort)
{
  WeaponMenuItem item(num_sort);
  item.position.Clear();
  item.weapon = new_item;

  item.weapon_icon = new Sprite(new_item->GetIcon());
  item.weapon_icon->cache.EnableLastFrameCache();

  boutons.push_back (item);

  if(num_sort>nbr_weapon_type)
    nbr_weapon_type = num_sort;
}

// Weapon menu display (init of the animation)
void WeaponsMenu::Show()
{
  ShowGameInterface();
  Time * global_time = Time::GetInstance();
  if(display && hide)
    motion_start_time = global_time->Read() - (ICONS_DRAW_TIME - (global_time->Read()-motion_start_time));
  else
    motion_start_time = global_time->Read();

  display = true;
  show = true;
  hide = false;
}

// Compute maximum number of icons in weapon menu columns.
void WeaponsMenu::ComputeSize()
{
  max_weapon = 0;
  uint nbr_current_type = 0;

  iterator it=boutons.begin(), fin=boutons.end();
  for (; it != fin; ++it)
  {
    if(it != boutons.begin())
    if(((it-1)->weapon_type) != (it->weapon_type))
    {
      if(nbr_current_type > max_weapon)
        max_weapon = nbr_current_type;
      nbr_current_type = 0;
    }
    if(ActiveTeam().ReadNbAmmos(it->weapon->GetName())>0
       || ActiveTeam().ReadNbAmmos(it->weapon->GetName())==INFINITE_AMMO)
      nbr_current_type++;
  }

  if(nbr_current_type > max_weapon)
    max_weapon = nbr_current_type;
}

void WeaponsMenu::Hide()
{
  if(display && show)
    motion_start_time = Time::GetInstance()->Read() - (ICONS_DRAW_TIME - (Time::GetInstance()->Read()-motion_start_time));
  else
    motion_start_time = Time::GetInstance()->Read();

  hide = true;
  show = false;
}

void WeaponsMenu::SwitchDisplay()
{
  if(display && !hide)
    Hide();
  else
    Show();
}

int WeaponsMenu::GetX() const
{
  return AppWormux::GetInstance()->video.window.GetWidth()-GetWidth();
}

int WeaponsMenu::GetY() const
{
  return AppWormux::GetInstance()->video.window.GetHeight() - GetHeight() - Interface::GetInstance()->GetHeight();
}

Point2i WeaponsMenu::GetPosition() const{
  return AppWormux::GetInstance()->video.window.GetSize() - GetSize() - Point2i(0, Interface::GetInstance()->GetHeight());
}

int WeaponsMenu::GetWidth() const
{
  return BUTTON_ICO_GAP + ((nbr_weapon_type +1) * BUTTON_WIDTH) ;
}

int WeaponsMenu::GetHeight() const
{
  return BUTTON_ICO_GAP + BUTTON_HEIGHT * max_weapon;
}

Point2i WeaponsMenu::GetSize() const
{
  return Point2i( BUTTON_ICO_GAP + ((nbr_weapon_type +1) * BUTTON_WIDTH), BUTTON_ICO_GAP + BUTTON_HEIGHT * max_weapon);
}

bool WeaponsMenu::IsDisplayed() const
{
  return display;
}

void WeaponsMenu::Reset()
{
  display = false;
  show = false;
  hide = false;
  motion_start_time = Time::GetInstance()->Read();
}

void WeaponsMenu::ShowMotion(int nr_buttons,int button_no,iterator it,int column)
{
  int delta_t=ICONS_DRAW_TIME/(2*nr_buttons);
  Time * global_time = Time::GetInstance();

  if((global_time->Read() > motion_start_time + (delta_t*button_no))
     && (global_time->Read() < motion_start_time + (ICONS_DRAW_TIME/2)+(delta_t*(button_no))))
    {
      double delta_sin = -(asin((column+1.0)/(column+2.0)) - (M_PI/2));

      uint tps = global_time->Read() - (motion_start_time + delta_t*button_no);

      double tps_sin = ((double)tps * ((M_PI/2) + delta_sin)/(ICONS_DRAW_TIME/2));

      it->position.x -= (int)(sin(tps_sin) * BUTTON_WIDTH * (column+2.0));
      it->position.x += (BUTTON_WIDTH * (column+1));
    }
  else
    if(global_time->Read() < motion_start_time + (delta_t*button_no))
      {
        it->position.x += (BUTTON_WIDTH * (column+1));
      }

  if(global_time->Read() > motion_start_time + ICONS_DRAW_TIME)
    {
      show = false;
    }
}

bool WeaponsMenu::HideMotion(int nr_buttons,int button_no,iterator it,int column)
{
  int delta_t=ICONS_DRAW_TIME/(2*nr_buttons);

  Time * global_time = Time::GetInstance();
  if((global_time->Read() > motion_start_time + (delta_t*(nr_buttons-button_no)))
     && (global_time->Read() < motion_start_time + (ICONS_DRAW_TIME/2)+(delta_t*(nr_buttons-button_no))))
    {
      double delta_sin = -(asin((column+1.0)/(column+2.0)) - (M_PI/2));

      uint tps = global_time->Read() - (motion_start_time + delta_t*(nr_buttons-button_no));
      double tps_sin = ((double)tps * ((M_PI/2) + delta_sin)/(ICONS_DRAW_TIME/2));
      tps_sin = ((M_PI/2) + delta_sin) - tps_sin;

      it->position.x -= (int)(sin(tps_sin) * BUTTON_WIDTH * (column+2.0));
      it->position.x += BUTTON_WIDTH * (column+1);
    }
  else
    if(global_time->Read() > motion_start_time + (delta_t*(nr_buttons-button_no)))
      {
        it->position.x += BUTTON_WIDTH * (column+1);
        it->Reset();
      }

  if(global_time->Read() > motion_start_time + ICONS_DRAW_TIME)
    {
      hide = false;
      display = false;
      return true;
    }

  return false;
}

void WeaponsMenu::Draw()
{
  if (!display)
    return;

  MouseOver(Mouse::GetInstance()->GetWorldPosition() - camera.GetPosition());
  ComputeSize();

  uint nr_buttons = max_weapon * nbr_weapon_type;
  uint button_no = 0;
  uint current_type = 0;

  iterator it=boutons.begin(), fin=boutons.end();
  for (it=boutons.begin(); it != fin; ++it)
  {
    if(!it->weapon->CanBeUsedOnClosedMap()
       && !world.IsOpen())
      continue;

    if(ActiveTeam().ReadNbAmmos(it->weapon->GetName())<=0
       && ActiveTeam().ReadNbAmmos(it->weapon->GetName())!=INFINITE_AMMO)
      continue;

    if(it->weapon_type!=current_type)
    {
      button_no = 0;
      current_type = it->weapon_type;
    }

    int column = nbr_weapon_type - current_type;
    int row = button_no;

    it->position.x = GetWidth() - (int)(BUTTON_WIDTH * (column+0.5));
    it->position.y = BUTTON_ICO_GAP + (row * BUTTON_HEIGHT);

    if(show)
      ShowMotion(nr_buttons,(column * max_weapon) + row,it,column);
    else
    if(hide)
      if(HideMotion(nr_buttons,(column * max_weapon) + row,it,column))
        return;

    it->Draw();
    button_no++;
  }
}

void WeaponsMenu::MouseOver(const Point2i &mousePos)
{
  static int bouton_sous_souris = -1; // button under mouse cursor

  // analized button
  int button_no=0;
  int nv_bouton_sous_souris=-1;
  iterator it=boutons.begin(), fin=boutons.end();
  Interface::GetInstance()->weapon_under_cursor = NULL;
  for (; it != fin; ++it)
    {
      if(it->MouseOn(mousePos))
      {
        Interface::GetInstance()->weapon_under_cursor = it->weapon;
        nv_bouton_sous_souris = button_no;
        if(button_no != bouton_sous_souris)
        { // mouse cursor is on a new button
          it->ChangeZoom();
        }
      }
      else
        if(button_no == bouton_sous_souris)
      {
        it->ChangeZoom();
      }
      button_no++;
    }
  bouton_sous_souris = nv_bouton_sous_souris;
}

bool WeaponsMenu::ActionClic(const Point2i &mousePos)
{
  if (!display) return false;

  iterator it=boutons.begin(), fin=boutons.end();
  for (; it != fin; ++it)
  {
    if( it->MouseOn(mousePos) )
    {
      ActionHandler::GetInstance()->NewAction (new Action(Action::ACTION_CHANGE_WEAPON,
      it -> weapon -> GetType()));
      SwitchDisplay();
      return true;
    }
  }
  return false;
}
