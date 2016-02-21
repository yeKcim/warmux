/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2009 Wormux Team.
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

#include <sstream>

#include "game/config.h"
#include "game/time.h"
#include "graphic/font.h"
#include "graphic/polygon_generator.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "include/action_handler.h"
#include "include/app.h"
#include "interface/interface.h"
#include "interface/mouse.h"
#include "interface/weapon_menu.h"
#include "map/maps_list.h"
#include "team/team.h"
#include "team/teams_list.h"
#include "tool/resource_manager.h"
#include "sound/jukebox.h"
#include "weapon/weapon.h"
#include "weapon/weapons_list.h"

// Weapon menu
const uint ICONS_DRAW_TIME = 400;       // Time to display all icons (in ms)
const uint BLING_ICONS_DRAW_TIME = 600; // Bling bling version

const uint ICON_ZOOM_TIME = 150;        // Time to zoom one icon.

const uint JELLY_TIME = 0;              // Jelly time when appearing
const uint BLING_JELLY_TIME = 300;      // Bling bling version

const uint ROTATION_TIME = 0;           // Number of rotation
const uint BLING_ROTATION_TIME = 2;     // bling bling !

const double DEFAULT_ICON_SCALE = 0.7;
const double MAX_ICON_SCALE = 1.1;

const int WeaponsMenu::MAX_NUMBER_OF_WEAPON = 7;


WeaponMenuItem::WeaponMenuItem(Weapon * new_weapon, const Point2d & position) :
  PolygonItem(),
  zoom(false),
  weapon(new_weapon),
  zoom_start_time(0)
{
  SetSprite(new Sprite(weapon->GetIcon()));
  SetPosition(position);
  SetZoomTime(ICON_ZOOM_TIME);
}

WeaponMenuItem::~WeaponMenuItem()
{
  if(item)
    delete item;
}

bool WeaponMenuItem::IsMouseOver()
{
  int nb_bullets = ActiveTeam().ReadNbAmmos(weapon->GetType());
  if(nb_bullets == 0) {
    if(zoom)
      SetZoom(false);
    return false;
  }
  Point2i mouse_pos = Mouse::GetInstance()->GetPosition();
  if(Contains(Point2d((double)mouse_pos.x, (double)mouse_pos.y))) {
    if(!zoom)
      SetZoom(true);
    return true;
  }
  if(zoom)
    SetZoom(false);
  return false;
}

void WeaponMenuItem::SetZoom(bool value)
{
  zoom = value;
  zoom_start_time = Time::GetInstance()->Read();
}

void WeaponMenuItem::Draw(Surface * dest)
{
  double scale = DEFAULT_ICON_SCALE;
  if(zoom || zoom_start_time + GetZoomTime() > Time::GetInstance()->Read()) {
    scale = (Time::GetInstance()->Read() - zoom_start_time) / (double)GetZoomTime();
    if(zoom) {
      scale = DEFAULT_ICON_SCALE + (MAX_ICON_SCALE - DEFAULT_ICON_SCALE) * scale;
      scale = (scale > MAX_ICON_SCALE ? MAX_ICON_SCALE : scale);
    } else {
      scale = MAX_ICON_SCALE - (MAX_ICON_SCALE - DEFAULT_ICON_SCALE) * scale;
      scale = (scale > DEFAULT_ICON_SCALE ? scale : DEFAULT_ICON_SCALE);
    }
  }
  item->Scale((float)scale, (float)scale);
  PolygonItem::Draw(dest);
  int nb_bullets = ActiveTeam().ReadNbAmmos(weapon->GetType());
  Point2i tmp = GetOffsetAlignment() + Point2i(0, item->GetWidth() - 10);
  if(nb_bullets ==  INFINITE_AMMO) {
    (*Font::GetInstance(Font::FONT_MEDIUM, Font::FONT_BOLD)).WriteLeft(tmp, "∞", dark_gray_color);
  } else if(nb_bullets == 0) {
    tmp += Point2i(0, -(int)Interface::GetInstance()->GetWeaponsMenu().GetCrossSymbol()->GetHeight() / 2);
    Interface::GetInstance()->GetWeaponsMenu().GetCrossSymbol()->Blit(*dest, tmp);
  } else {
    std::ostringstream txt;
    txt << nb_bullets;
    (*Font::GetInstance(Font::FONT_MEDIUM, Font::FONT_BOLD)).WriteLeft(tmp, txt.str(), dark_gray_color);
  }
}

WeaponsMenu::WeaponsMenu():
  weapons_menu(NULL),
  tools_menu(NULL),
  current_overfly_item(NULL),
  position(),
  shear(),
  rotation(),
  zoom(),
  cross(NULL),
  show(false),
  motion_start_time(0),
  icons_draw_time(ICONS_DRAW_TIME),
  jelly_time(JELLY_TIME),
  rotation_time(ROTATION_TIME),
  nbr_weapon_type(0),
  nb_weapon_type(new int[MAX_NUMBER_OF_WEAPON]),
  old_pointer(Mouse::POINTER_SELECT)
{
  // Loading value from XML
  Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml", false);
  cross = new Sprite(GetResourceManager().LoadImage(res, "interface/cross"));
  // Polygon Size
  Point2i size = GetResourceManager().LoadPoint2i(res, "interface/weapons_interface_size");
  weapons_menu = PolygonGenerator::GenerateDecoratedBox(size.x, size.y);
  size = GetResourceManager().LoadPoint2i(res, "interface/tools_interface_size");
  tools_menu = PolygonGenerator::GenerateDecoratedBox(size.x, size.y);
  help = NULL;
  // Setting colors
  Color plane_color = GetResourceManager().LoadColor(res, "interface/background_color");
  Color border_color = GetResourceManager().LoadColor(res, "interface/border_color");
  weapons_menu->SetPlaneColor(plane_color);
  weapons_menu->SetBorderColor(border_color);
  tools_menu->SetPlaneColor(plane_color);
  tools_menu->SetBorderColor(border_color);


  GetResourceManager().UnLoadXMLProfile(res);
}

WeaponsMenu::~WeaponsMenu()
{
  if(weapons_menu)
    delete weapons_menu;
  if(tools_menu)
    delete tools_menu;
  if(help)
    delete help;
  if (cross)
    delete cross;
  if (nb_weapon_type)
    delete[] nb_weapon_type;
}

// Add a new weapon to the weapon menu.
void WeaponsMenu::AddWeapon(Weapon* new_item)
{
  if(!new_item->CanBeUsedOnClosedMap() && !ActiveMap()->IsOpened())
    return;

  Point2d position;
  Weapon::category_t num_sort = new_item->Category();
  if(num_sort < 6) {
    position = weapons_menu->GetMin() + Point2d(30 + nb_weapon_type[num_sort - 1] * 45, 25 + (num_sort - 1) * 45);
    WeaponMenuItem * item = new WeaponMenuItem(new_item, position);
    weapons_menu->AddItem(item);
  } else {
    position = tools_menu->GetMin() + Point2d(30 + nb_weapon_type[num_sort - 1] * 45, 25 + (num_sort - 6) * 45);
    WeaponMenuItem * item = new WeaponMenuItem(new_item, position);
    tools_menu->AddItem(item);
  }

  nb_weapon_type[num_sort - 1]++;
}

// Weapon menu display (init of the animation)
void WeaponsMenu::Show()
{
  ShowGameInterface();
  if(!show) {
    if(motion_start_time + GetIconsDrawTime() < Time::GetInstance()->Read())
      motion_start_time = Time::GetInstance()->Read();
    else
      motion_start_time = Time::GetInstance()->Read() - (GetIconsDrawTime() - (Time::GetInstance()->Read() - motion_start_time));
    show = true;

    JukeBox::GetInstance()->Play("default", "menu/weapon_menu_show");

    old_pointer = Mouse::GetInstance()->SetPointer(Mouse::POINTER_SELECT);
  }
}

void WeaponsMenu::Hide(bool play_sound)
{
  if(show) {
    Interface::GetInstance()->SetCurrentOverflyWeapon(NULL);
    if(motion_start_time + GetIconsDrawTime() < Time::GetInstance()->Read())
      motion_start_time = Time::GetInstance()->Read();
    else
      motion_start_time = Time::GetInstance()->Read() - (GetIconsDrawTime() - (Time::GetInstance()->Read() - motion_start_time));
    show = false;

    if (play_sound)
      JukeBox::GetInstance()->Play("default", "menu/weapon_menu_hide");

    Mouse::GetInstance()->SetPointer(old_pointer);
  }
}

void WeaponsMenu::Reset()
{
  Interface::GetInstance()->SetCurrentOverflyWeapon(NULL);
  RefreshWeaponList();
  motion_start_time = 0;
  show = false;
  if(Config::GetInstance()->IsBlingBlingInterface()) {
    SetJellyTime(BLING_JELLY_TIME);
    SetIconsDrawTime(BLING_ICONS_DRAW_TIME);
    SetRotationTime(BLING_ROTATION_TIME);
  } else {
    SetJellyTime(JELLY_TIME);
    SetIconsDrawTime(ICONS_DRAW_TIME);
    SetRotationTime(ROTATION_TIME);
  }
}

void WeaponsMenu::RefreshWeaponList()
{
  // reset number of weapon
  for(int i = 0; i < MAX_NUMBER_OF_WEAPON; i++)
    nb_weapon_type[i] = 0;
  weapons_menu->ResetTransformation();
  tools_menu->ResetTransformation();
  // Refreshing Weapons menu

  std::vector<PolygonItem *> items = weapons_menu->GetItem();
  std::vector<PolygonItem *>::iterator item = items.begin();
  for(; item != items.end(); item++) {
    delete (*item);
  }
  weapons_menu->ClearItem(false);

  // Tools menu
  items = tools_menu->GetItem();
  item = items.begin();
  for(; item != items.end(); item++) {
    delete (*item);
  }
  tools_menu->ClearItem(false);
  // Reinserting weapon
  WeaponsList *weapons_list = WeaponsList::GetInstance();
  for (WeaponsList::weapons_list_it it=weapons_list->GetList().begin();
       it != weapons_list->GetList().end();
       ++it)
    AddWeapon(*it);
}

AffineTransform2D WeaponsMenu::ComputeToolTransformation()
{
  uint scroll_border = 0;
  if (Config::GetInstance()->GetScrollOnBorder()) {
    scroll_border = Config::GetInstance()->GetScrollBorderSize();
  }

  // Init animation parameter
  Point2d start(GetMainWindow().GetWidth() - tools_menu->GetWidth() - 5 - scroll_border,
		GetMainWindow().GetHeight() + weapons_menu->GetHeight() + 50);
  Point2i pos(GetMainWindow().GetWidth() - tools_menu->GetWidth() - 5 - scroll_border,
	      GetMainWindow().GetHeight()- tools_menu->GetHeight() - 5 );

  if (Interface::GetRef().GetMenuPosition().GetX() + Interface::GetRef().GetWidth() > start.GetX()) {
    start.y -= Interface::GetRef().GetHeight();
    pos.y -= Interface::GetRef().GetHeight();
  }

  Point2d end(POINT2I_2_POINT2D(pos));

  // Define the animation
  position.SetTranslationAnimation(motion_start_time, GetIconsDrawTime(), Time::GetInstance()->Read(), !show, start, end);

  return position ;
}

AffineTransform2D WeaponsMenu::ComputeWeaponTransformation()
{
  uint scroll_border = 0;
  if (Config::GetInstance()->GetScrollOnBorder()) {
    scroll_border = Config::GetInstance()->GetScrollBorderSize();
  }

  // Init animation parameter
  Point2d start(GetMainWindow().GetWidth() - weapons_menu->GetWidth() - 5 - scroll_border,
		GetMainWindow().GetHeight());
  Point2i pos(GetMainWindow().GetWidth() - weapons_menu->GetWidth() - 5 - scroll_border,
	      GetMainWindow().GetHeight()- weapons_menu->GetHeight() - tools_menu->GetHeight() - 10 );

  if (Interface::GetRef().GetMenuPosition().GetX() + Interface::GetRef().GetWidth() > start.GetX()) {
    start.y -= Interface::GetRef().GetHeight();
    pos.y -= Interface::GetRef().GetHeight();
  }

  Point2d end(POINT2I_2_POINT2D(pos));

 // Define the animation
  position.SetTranslationAnimation(motion_start_time, GetIconsDrawTime(), Time::GetInstance()->Read(), !show, start, end);

  return position ;
}

void WeaponsMenu::Draw()
{
  if(!show && (motion_start_time == 0 || Time::GetInstance()->Read() >= motion_start_time + GetIconsDrawTime()))
    return;
  // Draw weapons menu
  weapons_menu->ApplyTransformation(ComputeWeaponTransformation());
  weapons_menu->DrawOnScreen();
  // Tools
  tools_menu->ApplyTransformation(ComputeToolTransformation());
  tools_menu->DrawOnScreen();
  // Update overfly weapon/tool
  if(UpdateCurrentOverflyItem(weapons_menu) == NULL)
    UpdateCurrentOverflyItem(tools_menu);
}

Weapon * WeaponsMenu::UpdateCurrentOverflyItem(const Polygon * poly)
{
  std::vector<PolygonItem *> items = poly->GetItem();
  WeaponMenuItem * tmp;
  Interface::GetInstance()->SetCurrentOverflyWeapon(NULL);
  std::vector<PolygonItem *>::iterator item = items.begin();
  for(; item != items.end(); item++) {
    tmp = (WeaponMenuItem *)(*item);
    if(tmp->IsMouseOver()) {
      Interface::GetInstance()->SetCurrentOverflyWeapon(tmp->GetWeapon());
      if(current_overfly_item != tmp) {
        if(current_overfly_item != NULL)
          current_overfly_item->SetZoom(false);
        current_overfly_item = tmp;
      }
      return tmp->GetWeapon();
    }
  }
  return NULL;
}

bool WeaponsMenu::ActionClic(const Point2i &/*mouse_pos*/)
{
  Weapon * tmp;
  if(!show)
    return false;
  if((tmp = UpdateCurrentOverflyItem(weapons_menu)) == NULL)
    tmp = UpdateCurrentOverflyItem(tools_menu);
  if(tmp != NULL) {
    // Check we have enough ammo
    int nb_bullets = ActiveTeam().ReadNbAmmos(tmp->GetType());
    if((nb_bullets == INFINITE_AMMO || nb_bullets > 0) && ActiveTeam().GetWeapon().CanChangeWeapon()) {
      ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_PLAYER_CHANGE_WEAPON, tmp->GetType()));
      Hide(false);
      return true;
    }
  }
  return false;
}
