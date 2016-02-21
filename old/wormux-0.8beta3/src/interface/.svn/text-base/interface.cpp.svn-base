/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2007 Wormux Team.
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
 * Graphical interface showing various information about the game.
 *****************************************************************************/

#include "interface/interface.h"
#include "interface/mouse.h"
#include "character/character.h"
#include "game/game.h"
#include "game/game_mode.h"
#include "game/time.h"
#include "graphic/text.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "include/app.h"
#include "map/camera.h"
#include "map/map.h"
#include "team/macro.h"
#include "team/team.h"
#include "tool/resource_manager.h"
#include "tool/string_tools.h"
#include "tool/i18n.h"
#include "weapon/weapon.h"
#include "weapon/weapon_strength_bar.h"

WeaponStrengthBar weapon_strength_bar;

const Point2i BORDER_POSITION(5, 5);

const uint MARGIN = 10;

Interface * Interface::singleton = NULL;

Interface * Interface::GetInstance()
{
  if (singleton == NULL) {
    singleton = new Interface();
  }
  return singleton;
}

Interface::Interface()
{
  display = true;
  start_hide_display = 0;
  start_show_display = 0;

  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);
  game_menu = resource_manager.LoadImage( res, "interface/background_interface");
  small_background_interface = resource_manager.LoadImage( res, "interface/small_background_interface");
  clock_background = resource_manager.LoadImage( res, "interface/clock_background");
  clock = new Sprite(resource_manager.LoadImage( res, "interface/clock"));
  wind_icon = resource_manager.LoadImage( res, "interface/wind");
  wind_indicator = resource_manager.LoadImage( res, "interface/wind_indicator");

  // energy bar
  energy_bar.InitVal(0, 0, GameMode::GetInstance()->character.init_energy);
  energy_bar.InitPos(0, 0, 120, 15);

  // wind bar
  wind_bar.InitPos(0, 0, wind_indicator.GetWidth() - 4, wind_indicator.GetHeight() - 4);
  wind_bar.InitVal(0, -100, 100);
  wind_bar.border_color.SetColor(0, 0, 0, 0);
  wind_bar.background_color.SetColor(0, 0, 0, 0);
  wind_bar.value_color = c_red;
  wind_bar.SetReferenceValue (true, 0);

  // strength bar initialisation
  weapon_strength_bar.InitPos (0, 0, 400, 20);
  weapon_strength_bar.InitVal (0, 0, 100);

  weapon_strength_bar.SetValueColor(resource_manager.LoadColor(res, "interface/weapon_strength_bar_value"));
  weapon_strength_bar.SetBorderColor(resource_manager.LoadColor(res, "interface/weapon_strength_bar_border"));
  weapon_strength_bar.SetBackgroundColor(resource_manager.LoadColor(res, "interface/weapon_strength_bar_background"));

  Color text_color = resource_manager.LoadColor(res, "interface/text_color");
  Color energy_text_color = resource_manager.LoadColor(res, "interface/energy_text_color");
  // XXX Unused !?
  // Color turn_timer_text_color = resource_manager.LoadColor(res, "interface/turn_timer_text_color");
  // Color global_clock_text_color = resource_manager.LoadColor(res, "interface/global_clock_text_color");

  global_timer = new Text(ulong2str(0), gray_color, Font::FONT_BIG, Font::FONT_NORMAL, false);
  timer = new Text(ulong2str(0), black_color, Font::FONT_MEDIUM, Font::FONT_NORMAL, false);

  t_character_name = new Text("None", text_color, Font::FONT_SMALL, Font::FONT_BOLD, false);
  t_team_name = new Text("None", text_color, Font::FONT_SMALL, Font::FONT_BOLD, false);
  t_player_name = new Text("None", text_color, Font::FONT_SMALL, Font::FONT_BOLD, false);
  t_weapon_name = new Text("None", text_color, Font::FONT_SMALL, Font::FONT_BOLD, false);
  t_weapon_stock = new Text("0", text_color, Font::FONT_SMALL, Font::FONT_BOLD, false);
  t_character_energy = new Text("Dead", energy_text_color, Font::FONT_SMALL, Font::FONT_BOLD);

  resource_manager.UnLoadXMLProfile( res);
}

Interface::~Interface()
{
  if (global_timer) delete global_timer;
  if (timer) delete timer;
  if (t_character_name) delete t_character_name;
  if (t_team_name) delete t_team_name;
  if (t_player_name) delete t_player_name;
  if (t_character_energy) delete t_character_energy;
  if (t_weapon_name) delete t_weapon_name;
  if (t_weapon_stock) delete t_weapon_stock;
}

void Interface::Reset()
{
  start_hide_display = 0;
  start_show_display = 0;
  character_under_cursor = NULL;
  weapon_under_cursor = NULL;
  weapons_menu.Reset();
  energy_bar.InitVal(0, 0, GameMode::GetInstance()->character.init_energy);
}

void Interface::DrawCharacterInfo()
{
  AppWormux * app = AppWormux::GetInstance();
  // XXX Not used !?
  // Point2i pos = (app->video->window.GetSize() - GetSize()) * Point2d(0.5, 1);

  // Get the character
  if (character_under_cursor == NULL) character_under_cursor = &ActiveCharacter();

  // Display energy bar
  Point2i energy_bar_offset = BORDER_POSITION + Point2i(MARGIN + character_under_cursor->GetTeam().GetFlag().GetWidth(),
                                                        character_under_cursor->GetTeam().GetFlag().GetHeight() / 2);
  energy_bar.DrawXY(bottom_bar_pos + energy_bar_offset);

  // Display team logo
  if(energy_bar.GetCurrentValue() == energy_bar.GetMinValue())
    app->video->window.Blit(character_under_cursor->GetTeam().GetDeathFlag(), bottom_bar_pos + BORDER_POSITION);
  else
    app->video->window.Blit(character_under_cursor->GetTeam().GetFlag(), bottom_bar_pos + BORDER_POSITION);

  // Display team name
  t_team_name->Set(character_under_cursor->GetTeam().GetName());
  Point2i team_name_offset = energy_bar_offset + Point2i(energy_bar.GetWidth() / 2, energy_bar.GetHeight() + t_team_name->GetHeight() / 2);
  t_team_name->DrawCenter(bottom_bar_pos + team_name_offset);

  // Display character's name
  t_character_name->Set(character_under_cursor->GetName());
  Point2i character_name_offset = energy_bar_offset + Point2i((energy_bar.GetWidth() > t_character_name->GetWidth() ? energy_bar.GetWidth() : t_character_name->GetWidth()) / 2, -t_character_name->GetHeight() / 2);
  t_character_name->DrawCenter(bottom_bar_pos + character_name_offset);

  // Display player's name
  t_player_name->Set(_("Head commander: ") + character_under_cursor->GetTeam().GetPlayerName());
  Point2i player_name_offset = energy_bar_offset + Point2i(energy_bar.GetWidth() / 2, t_team_name->GetHeight() + t_player_name->GetHeight() + MARGIN);
  t_player_name->DrawCenter(bottom_bar_pos + player_name_offset);

  // Display energy
  if (!character_under_cursor->IsDead()) {
    t_character_energy->Set(ulong2str(character_under_cursor->GetEnergy())+"%");
    energy_bar.Actu(character_under_cursor->GetEnergy());
  } else {
    t_character_energy->Set(_("(dead)"));
    energy_bar.Actu(0);
  }

  t_character_energy->DrawCenter(bottom_bar_pos + energy_bar_offset + energy_bar.GetSize()/2);
}

void Interface::DrawWeaponInfo() const
{
  Weapon* weapon;
  int nbr_munition;
  float icon_scale_factor = 0.75;

  // Get the weapon
  if(weapon_under_cursor==NULL) {
    weapon = &ActiveTeam().AccessWeapon();
    nbr_munition = ActiveTeam().ReadNbAmmos();
  } else {
    weapon = weapon_under_cursor;
    nbr_munition = ActiveTeam().ReadNbAmmos(weapon_under_cursor->GetType());
    icon_scale_factor = cos((float)Time::GetInstance()->Read() / 1000 * M_PI) * 0.9;
  }

  std::string tmp;

  // Draw weapon name
  t_weapon_name->Set(weapon->GetName());
  Point2i weapon_name_offset = Point2i(game_menu.GetWidth() / 2 - clock_background.GetWidth() / 2 - t_weapon_name->GetWidth() - MARGIN, 0);
  t_weapon_name->DrawTopLeft(bottom_bar_pos + weapon_name_offset);

  // Display number of ammo
  t_weapon_stock->Set((nbr_munition ==  INFINITE_AMMO ? _("(unlimited)") : _("Stock:") + Format("%i", nbr_munition)));
  Point2i weapon_stock_offset = Point2i(game_menu.GetWidth() / 2 - clock_background.GetWidth() / 2 - t_weapon_stock->GetWidth() - MARGIN, t_weapon_name->GetHeight());
  t_weapon_stock->DrawTopLeft(bottom_bar_pos + weapon_stock_offset);

  // Draw weapon icon
  weapon->GetIcon().Scale(icon_scale_factor, 0.75);
  Point2i weapon_icon_offset = game_menu.GetSize() / 2 - weapon->GetIcon().GetSize() / 2 + Point2i(- clock_background.GetWidth(), MARGIN);
  weapon->GetIcon().DrawXY(bottom_bar_pos + weapon_icon_offset);
}

void Interface::DrawTimeInfo() const
{
  AppWormux * app = AppWormux::GetInstance();
  Point2i turn_time_pos = (app->video->window.GetSize() - clock_background.GetSize()) * Point2d(0.5, 1) +
      Point2i(0, - GetHeight() + clock_background.GetHeight());
  Rectanglei dr(turn_time_pos, clock_background.GetSize());

  // Draw background interface
  app->video->window.Blit(clock_background, turn_time_pos);
  world.ToRedrawOnScreen(dr);
  DrawClock(turn_time_pos + clock_background.GetSize() / 2);
}

// display time left in a turn
void Interface::DrawClock(const Point2i &time_pos) const
{
  // Draw turn time
  if (display_timer)
    timer->DrawCenter(time_pos - Point2i(0, clock_background.GetHeight()/3));

  // Draw clock
  float scale;
  if(remaining_turn_time < 10)  // Hurry up !
    scale = 0.9 + cos((float)Time::GetInstance()->Read() / 250 * M_PI) * 0.1;
  else
    scale = 1.0;
  clock->Scale(1.0, scale);
  Point2i tmp_point = time_pos - clock->GetSize() / 2;
  clock->DrawXY(tmp_point);

  // Draw global timer
  std::string tmp(Time::GetInstance()->GetString());
  global_timer->Set(tmp);
  global_timer->DrawCenter(time_pos + Point2i(0, clock_background.GetHeight()/3));
}

// draw wind indicator
void Interface::DrawWindIndicator(const Point2i &wind_bar_pos, const bool draw_icon) const
{
  AppWormux * app = AppWormux::GetInstance();
  int height;

  // draw wind icon
  if(draw_icon) {
    app->video->window.Blit(wind_icon, wind_bar_pos);
    world.ToRedrawOnScreen(Rectanglei(wind_bar_pos, wind_icon.GetSize()));
    height = wind_icon.GetHeight() - wind_indicator.GetHeight();
  } else {
    height = MARGIN;
  }

  // draw wind indicator
  Point2i wind_bar_offset = Point2i(0, height);
  Point2i tmp = wind_bar_pos + wind_bar_offset + Point2i(2, 2);
  app->video->window.Blit(wind_indicator, wind_bar_pos + wind_bar_offset);
  wind_bar.DrawXY(tmp);
  world.ToRedrawOnScreen(Rectanglei(wind_bar_pos + wind_bar_offset, wind_indicator.GetSize()));
}

// display wind info
void Interface::DrawWindInfo() const
{
  Point2i wind_pos_offset = Point2i(game_menu.GetWidth() / 2 + clock_background.GetWidth() / 2 + MARGIN, game_menu.GetHeight() / 2 - wind_icon.GetHeight() / 2);
  DrawWindIndicator(bottom_bar_pos + wind_pos_offset, true);
}

// draw mini info when hidding interface
void Interface::DrawSmallInterface() const
{
  if(display) return;
  AppWormux * app = AppWormux::GetInstance();
  int height;
  height = ((int)Time::GetInstance()->Read() - start_hide_display - 1000) / 3 - 30;
  height = (height > 0 ? height : 0);
  height = (height < small_background_interface.GetHeight() ? height : small_background_interface.GetHeight());
  Point2i small_interface_position = Point2i(app->video->window.GetWidth() / 2 - small_background_interface.GetWidth() / 2, app->video->window.GetHeight() - height);
  app->video->window.Blit(small_background_interface,small_interface_position);
  world.ToRedrawOnScreen(Rectanglei(small_interface_position,small_background_interface.GetSize()));
  DrawWindIndicator(small_interface_position + Point2i(MARGIN, 0), false);
  if (display_timer)
    timer->DrawTopLeft(small_interface_position + Point2i(MARGIN * 2 + wind_bar.GetWidth(), MARGIN));
}

// draw team energy
void Interface::DrawTeamEnergy() const
{
  Point2i team_bar_offset = Point2i(game_menu.GetWidth() / 2 + clock_background.GetWidth() / 2 + wind_icon.GetWidth() + MARGIN, MARGIN);
  FOR_EACH_TEAM(tmp_team) {
    if(!display) // Fix bug #7753 (Team energy bar visible when the interface is hidden)
      (**tmp_team).GetEnergyBar().FinalizeMove();
    (**tmp_team).DrawEnergy(bottom_bar_pos + team_bar_offset);
  }
}

void Interface::Draw()
{
  AppWormux * app = AppWormux::GetInstance();
  bottom_bar_pos = (app->video->window.GetSize() - GetSize()) * Point2d(0.5, 1);

  if ( Game::GetInstance()->ReadState() == Game::PLAYING && weapon_strength_bar.visible)
  {
    // Position on the screen
    Point2i barPos = (app->video->window.GetSize() - weapon_strength_bar.GetSize()) * Point2d(0.5, 1)
        - Point2i(0, game_menu.GetHeight() + MARGIN);

    // Drawing on the screen
     weapon_strength_bar.DrawXY(barPos);
  }

  weapons_menu.Draw();

  // Display the background of both Character info and weapon info
  Rectanglei dr(bottom_bar_pos, game_menu.GetSize());
  app->video->window.Blit(game_menu, bottom_bar_pos);

  world.ToRedrawOnScreen(dr);

  // display wind, character and weapon info
  DrawWindInfo();
  DrawTimeInfo();
  DrawCharacterInfo();
  DrawTeamEnergy();
  DrawWeaponInfo();
  DrawSmallInterface();
}

int Interface::GetHeight() const
{
  if(!display) {
    int height = GetMenuHeight() - ((int)Time::GetInstance()->Read() - start_hide_display)/3;
    height = (height > 0 ? height : 0);
    return (height < GetMenuHeight() ? height : GetMenuHeight());
  } else if(start_show_display != 0) {
    int height = ((int)Time::GetInstance()->Read() - start_show_display)/3;
    height = (height < GetMenuHeight() ? height : GetMenuHeight());
    return (height < GetMenuHeight() ? height : GetMenuHeight());
  }
  return GetMenuHeight();
}

int Interface::GetMenuHeight() const
{
  return game_menu.GetHeight() + MARGIN;
}

Point2i Interface::GetSize() const
{
  return Point2i(GetWidth(), GetHeight());
}

void Interface::EnableDisplay(bool _display)
{
  display = _display;
}

void Interface::Show()
{
  if(display) return;
  display = true;
  if(start_show_display + 1000 < (int)Time::GetInstance()->Read())
    start_show_display = Time::GetInstance()->Read();
  else
    start_show_display = Time::GetInstance()->Read() - (1000 - ((int)Time::GetInstance()->Read() - start_show_display));
}

void Interface::Hide()
{
  if(!display) return;
  display = false;
  if(start_hide_display + 1000 < (int)Time::GetInstance()->Read())
    start_hide_display = Time::GetInstance()->Read();
  else
    start_hide_display = Time::GetInstance()->Read() - (1000 - ((int)Time::GetInstance()->Read() - start_hide_display));
}

void Interface::UpdateTimer(uint utimer)
{
  timer->Set(ulong2str(utimer));
  remaining_turn_time = utimer;
}

void AbsoluteDraw(const Surface &s, const Point2i& pos)
{
  Rectanglei rectSurface(pos, s.GetSize());

  if( !rectSurface.Intersect(*Camera::GetInstance()))
    return;

  world.ToRedrawOnMap(rectSurface);

  rectSurface.Clip(*Camera::GetInstance());

  Rectanglei rectSource(rectSurface.GetPosition() - pos, rectSurface.GetSize());
  Point2i ptDest = rectSurface.GetPosition() - Camera::GetInstance()->GetPosition();

  AppWormux::GetInstance()->video->window.Blit(s, rectSource, ptDest);
}

void HideGameInterface()
{
  if(Interface::GetInstance()->GetWeaponsMenu().IsDisplayed()) return;
  Mouse::GetInstance()->Hide();
  Interface::GetInstance()->Hide();
}

void ShowGameInterface()
{
  Interface::GetInstance()->Show();
}
