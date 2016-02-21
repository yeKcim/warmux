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
 * Interface showing various informations about the current weapon.
 *****************************************************************************/

#include "game/game_time.h"
#include "graphic/polygon.h"
#include "graphic/polygon_generator.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "interface/interface.h"
#include "interface/weapon_help.h"
#include "map/camera.h"
#include "map/map.h"
#include "tool/resource_manager.h"
#include "sound/jukebox.h"
#include "weapon/weapon.h"

#define ICONS_DRAW_TIME  400       // Time to display all icons (in ms)
#define HIDE_TIMEOUT    4000
#define ICON_SIZE         48

WeaponHelp::WeaponHelp()
  : weapon_icon(NULL)
  , help("", black_color, Font::FONT_SMALL, Font::FONT_NORMAL, false)
  , show(false)
  , motion_start_time(0)
{
  // Loading value from XML
  Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml", false);

  // Polygon Size
  background = LOAD_RES_IMAGE("interface/background_help");

  help.SetMaxWidth(background.GetWidth() - 10 - ICON_SIZE - 2*5);
}

WeaponHelp::~WeaponHelp()
{
}


void WeaponHelp::SetWeapon(Weapon& item)
{
  weapon_icon = &item.GetIcon();
  help.SetText(item.GetHelp());
}

// Weapon menu display (init of the animation)
void WeaponHelp::Show()
{
  if (!show) {
    if (motion_start_time + ICONS_DRAW_TIME < GameTime::GetInstance()->Read())
      motion_start_time = GameTime::GetInstance()->Read();
    else
      motion_start_time = GameTime::GetInstance()->Read() - (ICONS_DRAW_TIME - (GameTime::GetInstance()->Read() - motion_start_time));
    show = true;

    JukeBox::GetInstance()->Play("default", "menu/weapon_menu_show");
  }
}

void WeaponHelp::Hide(bool play_sound)
{
  if (show) {
    if (motion_start_time + ICONS_DRAW_TIME < GameTime::GetInstance()->Read())
      motion_start_time = GameTime::GetInstance()->Read();
    else
      motion_start_time = GameTime::GetInstance()->Read() - (ICONS_DRAW_TIME - (GameTime::GetInstance()->Read() - motion_start_time));
    show = false;

    if (play_sound)
      JukeBox::GetInstance()->Play("default", "menu/weapon_menu_hide");
  }
}

void WeaponHelp::Reset()
{
  motion_start_time = 0;
  show = false;
}

void WeaponHelp::Draw()
{
  if (!show && (motion_start_time == 0 || GameTime::GetInstance()->Read() >= motion_start_time + ICONS_DRAW_TIME))
    return;

  // Init animation parameter
  Surface& window = GetMainWindow();
  uint x = (window.GetWidth() - background.GetWidth())>>1;
  Point2i pos(x, -background.GetHeight());
  Point2i end(x, 10);
  int   diff = GameTime::GetInstance()->Read() - motion_start_time;
  float coef = diff / (float)ICONS_DRAW_TIME;
  coef = coef > 0.0f ? coef : 0.0f;
  coef = coef < 1.0f ? coef : 1.0f;
  if (!show)
    coef = 1.0f - coef;
  pos = end*coef + pos*(1.0f-coef);

  window.Blit(background, pos);
  weapon_icon->Blit(window, pos.GetX()+10, pos.GetY() + (background.GetHeight() - ICON_SIZE)/2);
  help.DrawCenter(Point2i(pos.GetX() + (background.GetWidth() + ICON_SIZE)/2,
                          pos.GetY() + background.GetHeight()/2));

  Rectanglei bottom(pos + Camera::GetConstRef().GetPosition(), background.GetSize());
  GetWorld().ToRedrawOnMap(bottom);

  if (diff > HIDE_TIMEOUT + ICONS_DRAW_TIME && show)
    Hide();
}
