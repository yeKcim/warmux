/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
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
#include "game/game_time.h"
#include "game/game.h"
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

const Double DEFAULT_ICON_SCALE = 0.7;
const Double MAX_ICON_SCALE = 1.1;

const int WeaponsMenu::MAX_NUMBER_OF_WEAPON = 7;

#define MAX_ICON_SIZE          45

WeaponMenuItem::WeaponMenuItem(Weapon * new_weapon, const Point2d & position)
  : PolygonItem()
  , zoom(false)
  , weapon(new_weapon)
  , zoom_start_time(0)
{
  SetSprite(new Sprite(weapon->GetIcon()));
  SetPosition(position);
  SetZoomTime(ICON_ZOOM_TIME);
}

bool WeaponMenuItem::IsMouseOver()
{
  if (!ActiveTeam().ReadNbAmmos(weapon->GetType())) {
    if (zoom)
      SetZoom(false);
    return false;
  }

  // Compute the size of the icon bounding box
  Point2i size(MAX_ICON_SIZE, MAX_ICON_SIZE);

  // The icon bounding box for this is centered around the transformed position
  Rectanglei r(transformed_position + 1 - size/2, size);
  if (r.Contains(Mouse::GetInstance()->GetPosition())) {
    if (!zoom)
      SetZoom(true);
    return true;
  }
  if (zoom)
    SetZoom(false);
  return false;
}

void WeaponMenuItem::SetZoom(bool value)
{
  zoom = value;
  zoom_start_time = GameTime::GetInstance()->Read();
}

void WeaponMenuItem::Draw(Surface * dest)
{
  Double scale = DEFAULT_ICON_SCALE;
  if (zoom || zoom_start_time + GetZoomTime() > GameTime::GetInstance()->Read()) {
    scale = (GameTime::GetInstance()->Read() - zoom_start_time) / (Double)GetZoomTime();
    if (zoom) {
      scale = DEFAULT_ICON_SCALE + (MAX_ICON_SCALE - DEFAULT_ICON_SCALE) * scale;
      scale = (scale > MAX_ICON_SCALE ? MAX_ICON_SCALE : scale);
    } else {
      scale = MAX_ICON_SCALE - (MAX_ICON_SCALE - DEFAULT_ICON_SCALE) * scale;
      scale = (scale > DEFAULT_ICON_SCALE ? scale : DEFAULT_ICON_SCALE);
    }
  }
  item->SetAlpha(1);
  item->Scale(scale, scale);

  int nb_bullets = ActiveTeam().ReadNbAmmos(weapon->GetType());
  Point2i tmp = GetOffsetAlignment() + Point2i(0, item->GetWidth() - 10);

  char buffer[5] = { 0, };
  if (nb_bullets ==  INFINITE_AMMO) {
#define UTF8_INFINITE "\xE2\x88\x9E"
    PolygonItem::Draw(dest);
    Font::GetInstance(Font::FONT_MEDIUM, Font::FONT_BOLD)->WriteLeft(tmp, UTF8_INFINITE, dark_gray_color);
  } else if (nb_bullets == 0) {
      int num = weapon->AvailableAfterTurn() - (int)Game::GetInstance()->GetCurrentTurn();
      if (num > -1){
        PolygonItem::Draw(dest);
        tmp.y -= 4;
        m_parent->m_not_yet_available->Blit(*dest, tmp);

        tmp.x += m_parent->m_not_yet_available->GetWidth()-5;
        tmp.y += 10;
        snprintf(buffer, 4, "%i ", num);
        Font::GetInstance(Font::FONT_SMALL, Font::FONT_BOLD)->WriteLeft(tmp, buffer, dark_red_color);
      } else {
        item->SetAlpha(0.3);
        PolygonItem::Draw(dest);
      }
  } else {
    PolygonItem::Draw(dest);
    std::ostringstream txt;
    snprintf(buffer, 4, "%i ", nb_bullets);
    Font::GetInstance(Font::FONT_MEDIUM, Font::FONT_BOLD)->WriteLeft(tmp, buffer, dark_gray_color);
  }
}


WeaponsMenu::WeaponsMenu()
  : m_not_yet_available(NULL)
  , weapons_menu(NULL)
  , tools_menu(NULL)
  , current_overfly_item(NULL)
  , position()
  , shear()
  , rotation()
  , zoom()
  , show(false)
  , motion_start_time(0)
  , icons_draw_time(ICONS_DRAW_TIME)
  , jelly_time(JELLY_TIME)
  , rotation_time(ROTATION_TIME)
  , nbr_weapon_type(0)
  , nb_weapon_type(new int[MAX_NUMBER_OF_WEAPON])
  , old_pointer(Mouse::POINTER_SELECT)
{
  // Loading value from XML
  Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml", false);
  m_not_yet_available = LOAD_RES_SPRITE("interface/hourglass");

  // Polygon Size
  Point2i size = LOAD_RES_POINT("interface/weapons_interface_size");
  weapons_menu = PolygonGenerator::GenerateDecoratedBox(size.x, size.y);
  size = LOAD_RES_POINT("interface/tools_interface_size");
  tools_menu = PolygonGenerator::GenerateDecoratedBox(size.x, size.y);
  // Setting colors
  Color plane_color = LOAD_RES_COLOR("interface/background_color");
  Color border_color = LOAD_RES_COLOR("interface/border_color");
  weapons_menu->SetPlaneColor(plane_color);
  weapons_menu->SetBorderColor(border_color);
  tools_menu->SetPlaneColor(plane_color);
  tools_menu->SetBorderColor(border_color);
}

WeaponsMenu::~WeaponsMenu()
{
  if (weapons_menu)
    delete weapons_menu;
  if (tools_menu)
    delete tools_menu;
  if (m_not_yet_available)
    delete m_not_yet_available;
  if (nb_weapon_type)
    delete[] nb_weapon_type;
}

// Add a new weapon to the weapon menu.
void WeaponsMenu::AddWeapon(Weapon* new_item)
{
  if (!new_item->CanBeUsedOnClosedMap() && !ActiveMap()->LoadedInfo()->IsOpened())
    return;

  Point2f pos;
  Weapon::category_t num_sort = new_item->Category();
  Polygon *menu;

  if (num_sort < 6) {
    menu = weapons_menu;
    pos  = Point2f(nb_weapon_type[num_sort-1], num_sort-1)*MAX_ICON_SIZE;
  } else {
    menu = tools_menu;
    pos  = Point2f(num_sort-6, nb_weapon_type[num_sort-1])*MAX_ICON_SIZE;
  }
  pos += P2D_TO_P2F(menu->GetMin()) + Point2f(30,25);

  WeaponMenuItem * item = new WeaponMenuItem(new_item, pos);
  item->SetParent(this);
  menu->AddItem(item);

  nb_weapon_type[num_sort - 1]++;
}

// Weapon menu display (init of the animation)
void WeaponsMenu::Show(const Point2i& pos)
{
  if (!ActiveTeam().GetWeapon().CanChangeWeapon())
    return;

  Surface& window = GetMainWindow();
  uint scroll_border = 0;
  
  if (Config::GetInstance()->GetScrollOnBorder()) {
    scroll_border = Config::GetInstance()->GetScrollBorderSize();
  }

  click_pos = pos;
  int p = 5 + scroll_border;
  if (click_pos.x < p)
    click_pos.x = p;
  if (click_pos.y < p)
    click_pos.y = window.GetHeight(); // Force moving to bottom
  p = window.GetWidth()-10
    - (weapons_menu->GetWidth() + 10 + tools_menu->GetWidth());
  if (click_pos.x > p)
    click_pos.x = p;
  p = window.GetHeight() - Interface::GetRef().GetMenuHeight()
    - weapons_menu->GetHeight() - 10;
  if (click_pos.y > p)
    click_pos.y = p;

  ShowGameInterface();
  if (!show) {
    if (motion_start_time + GetIconsDrawTime() < GameTime::GetInstance()->Read())
      motion_start_time = GameTime::GetInstance()->Read();
    else
      motion_start_time = GameTime::GetInstance()->Read() - (GetIconsDrawTime() - (GameTime::GetInstance()->Read() - motion_start_time));
    show = true;

    JukeBox::GetInstance()->Play("default", "menu/weapon_menu_show");

    old_pointer = Mouse::GetInstance()->SetPointer(Mouse::POINTER_SELECT);
  }
}

void WeaponsMenu::Hide(bool play_sound)
{
  if (show) {
    Interface::GetInstance()->SetCurrentOverflyWeapon(NULL);
    if (motion_start_time + GetIconsDrawTime() < GameTime::GetInstance()->Read())
      motion_start_time = GameTime::GetInstance()->Read();
    else
      motion_start_time = GameTime::GetInstance()->Read() - (GetIconsDrawTime() - (GameTime::GetInstance()->Read() - motion_start_time));
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
  if (Config::GetInstance()->IsBlingBlingInterface()) {
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
  for (int i = 0; i < MAX_NUMBER_OF_WEAPON; i++)
    nb_weapon_type[i] = 0;
  weapons_menu->ResetTransformation();
  tools_menu->ResetTransformation();
  // Refreshing Weapons menu

  const std::vector<PolygonItem *>& weapons = weapons_menu->GetItem();
  std::vector<PolygonItem *>::const_iterator item = weapons.begin();
  for (; item != weapons.end(); item++) {
    delete (*item);
  }
  weapons_menu->ClearItem(false);

  // Tools menu
  const std::vector<PolygonItem *>& tools = tools_menu->GetItem();
  item = tools.begin();
  for (; item != tools.end(); item++) {
    delete (*item);
  }
  tools_menu->ClearItem(false);
  // Reinserting weapon
  WeaponsList * weapons_list = Game::GetInstance()->GetWeaponsList();
  for (WeaponsList::iterator it=weapons_list->GetList().begin();
       it != weapons_list->GetList().end();
       ++it)
    AddWeapon(*it);
}

AffineTransform2D WeaponsMenu::ComputeToolTransformation()
{
#if 0 // dead code
  uint scroll_border = 0;
  if (Config::GetInstance()->GetScrollOnBorder()) {
    scroll_border = Config::GetInstance()->GetScrollBorderSize();
  }
#endif

  Point2i start = click_pos - Point2i(tools_menu->GetWidth(), 0);
  Point2i end = click_pos + Point2i(weapons_menu->GetWidth()+10, 0);

  // Define the animation
  position.SetTranslationAnimation(motion_start_time, GetIconsDrawTime(),
                                   GameTime::GetInstance()->Read(),
                                   !show, start, end);

  return position ;
}

AffineTransform2D WeaponsMenu::ComputeWeaponTransformation()
{
#if 0 // dead code
  uint scroll_border = 0;
  if (Config::GetInstance()->GetScrollOnBorder()) {
    scroll_border = Config::GetInstance()->GetScrollBorderSize();
  }
#endif

  // Init animation parameter
  Point2i start = click_pos
                - Point2i(weapons_menu->GetWidth()+10+tools_menu->GetWidth(), 0);

  // Define the animation
  position.SetTranslationAnimation(motion_start_time, GetIconsDrawTime(),
                                   GameTime::GetInstance()->Read(),
                                   !show, start, click_pos);

  return position;
}

void WeaponsMenu::Draw()
{
  if (!show && (motion_start_time == 0 || GameTime::GetInstance()->Read() >= motion_start_time + GetIconsDrawTime()))
    return;

  // Update animation
  m_not_yet_available->Update();

  Rectanglei clip(click_pos, weapons_menu->GetSize() + Point2i(10, 0) + tools_menu->GetSize());
  SwapWindowClip(clip);
  // Draw weapons menu
  weapons_menu->ApplyTransformation(ComputeWeaponTransformation());
  weapons_menu->DrawOnScreen();
  // Tools
  tools_menu->ApplyTransformation(ComputeToolTransformation());
  tools_menu->DrawOnScreen();
  // Update overfly weapon/tool
  if (!UpdateCurrentOverflyItem(weapons_menu))
    UpdateCurrentOverflyItem(tools_menu);
  SwapWindowClip(clip);
}

Weapon * WeaponsMenu::UpdateCurrentOverflyItem(const Polygon * poly)
{
  if (!show)
    return false;
  const std::vector<PolygonItem *>& items = poly->GetItem();
  WeaponMenuItem * tmp;
  Interface::GetInstance()->SetCurrentOverflyWeapon(NULL);
  std::vector<PolygonItem *>::const_iterator item = items.begin();
  for (; item != items.end(); item++) {
    tmp = (WeaponMenuItem *)(*item);
    if (tmp->IsMouseOver()) {
      Interface::GetInstance()->SetCurrentOverflyWeapon(tmp->GetWeapon());
      if (current_overfly_item != tmp) {
        if (current_overfly_item)
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
  if (!show)
    return false;
  Weapon * tmp = UpdateCurrentOverflyItem(weapons_menu);
  if (!tmp)
    tmp = UpdateCurrentOverflyItem(tools_menu);
  if (tmp) {
    // Check we have enough ammo
    int nb_bullets = ActiveTeam().ReadNbAmmos(tmp->GetType());
    if ((nb_bullets == INFINITE_AMMO || nb_bullets > 0) && ActiveTeam().GetWeapon().CanChangeWeapon()) {
      ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_PLAYER_CHANGE_WEAPON, tmp->GetType()));
      Hide(false);
      return true;
    }
  }
  return false;
}
