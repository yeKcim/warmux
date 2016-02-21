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
 * Interface affichant différentes informations sur la jeu.
 *****************************************************************************/

#include "interface.h"
#include <iostream>
#include <SDL.h>
#include <sstream>
#include "../game/game_loop.h"
#include "../game/game_mode.h"
#include "../game/time.h"
#include "../graphic/colors.h"
#include "../graphic/video.h"
#include "../graphic/font.h"
#include "../include/app.h"
#include "../map/camera.h"
#include "../map/map.h"
#include "../team/teams_list.h"
#include "../tool/debug.h"
#include "../tool/string_tools.h"
#include "../tool/i18n.h"
#include "../tool/resource_manager.h"
#include "../weapon/weapons_list.h"

WeaponStrengthBar weapon_strength_bar;

const Point2i WORM_NAME_POS(32, 28);
const Point2i WORM_ENERGY_POS = WORM_NAME_POS + Point2i(0, 20);
const Point2i WEAPON_NAME_POS(508, WORM_NAME_POS.y);
const Point2i AMMOS_POS(WEAPON_NAME_POS.x, WORM_ENERGY_POS.y);
const Point2i TEAM_ICON_POS(303, 20);
const Point2i WEAPON_ICON_POS(450, 20);

const uint INFO_VER_X2 = 296;

// Barre d'énergie
const Point2i ENERGY_BAR_POS(170, WORM_ENERGY_POS.y + 2);
const uint BARENERGIE_LARG = INFO_VER_X2 - ENERGY_BAR_POS.x;
const uint BARENERGIE_HAUT = 15;

const uint MARGIN = 10;

Interface * Interface::singleton = NULL;

Interface * Interface::GetInstance() {
  if (singleton == NULL) {
    singleton = new Interface();
  }
  return singleton;
}

Interface::Interface()
{
}

void Interface::Init()
{
  display = true;

  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);
  game_menu = resource_manager.LoadImage( res, "interface/menu_jeu");
  bg_time = resource_manager.LoadImage( res, "interface/fond_compteur");
  weapons_menu.Init();
  weapon_box_button = resource_manager.LoadImage( res, "interface/weapon_box_button");
   
  barre_energie.InitVal (0, 0, GameMode::GetInstance()->character.init_energy);
  barre_energie.InitPos (ENERGY_BAR_POS.x, ENERGY_BAR_POS.y, 
			 BARENERGIE_LARG, BARENERGIE_HAUT);
  barre_energie.border_color = white_color;
  barre_energie.value_color = lightgray_color;
  barre_energie.background_color = gray_color;  
   
  // strength bar initialisation  
  weapon_strength_bar.InitPos (0, 0, 400, 10);
  weapon_strength_bar.InitVal (0, 0, 100);

  weapon_strength_bar.SetValueColor(WeaponStrengthBarValue);
  weapon_strength_bar.SetBorderColor(WeaponStrengthBarBorder);
  weapon_strength_bar.SetBackgroundColor(WeaponStrengthBarBackground);

  // constant text initialisation
  Font * normal_font = Font::GetInstance(Font::FONT_NORMAL);

  t_NAME = new Text( _("Name:"), white_color, normal_font);
  t_ENERGY = new Text( _("Energy:"),white_color, normal_font);
  t_WEAPON = new Text( _("Weapon:"), white_color, normal_font);
  t_STOCK = new Text( _("Stock:"), white_color, normal_font);

  global_timer = new Text(ulong2str(0), white_color, normal_font);
  timer = NULL;

  t_character_name = new Text("None", white_color, normal_font);
  t_character_energy = new Text("Dead", white_color, normal_font);
  t_weapon_name = new Text("None", white_color, normal_font);
  t_weapon_stock = new Text("0", white_color, normal_font);;
}

Interface::~Interface()
{
  delete t_NAME;
  delete t_ENERGY;
  delete t_WEAPON;
  delete t_STOCK;

  if (global_timer) delete global_timer;
  if (timer) delete timer;
  if (t_character_name) delete t_character_name;
  if (t_character_energy) delete t_character_energy;
  if (t_weapon_name) delete t_weapon_name;
  if (t_weapon_stock) delete t_weapon_stock;
}

void Interface::Reset()
{
  character_under_cursor = NULL;
  weapon_under_cursor = NULL;
  weapons_menu.Reset();
  barre_energie.InitVal (0, 0, GameMode::GetInstance()->character.init_energy);
}

void Interface::DisplayCharacterInfo ()
{
  AppWormux * app = AppWormux::GetInstance();
  Point2i pos = (app->video.window.GetSize() - GetSize()) * Point2d(0.5, 1);

  // Get the character
  if (character_under_cursor == NULL) character_under_cursor = &ActiveCharacter();

  // Display name
  t_NAME->DrawTopLeft(bottom_bar_pos + WORM_NAME_POS);

  std::string s(character_under_cursor->m_name+" ("+character_under_cursor->GetTeam().GetName()+" )");
  t_character_name->Set(s);

  t_character_name->DrawTopLeft(bottom_bar_pos + WORM_NAME_POS + 
		  Point2i(t_NAME->GetWidth()+MARGIN, 0));
  
  // Display energy
  t_ENERGY->DrawTopLeft(bottom_bar_pos + WORM_ENERGY_POS);

  if (!character_under_cursor->IsDead()) {
    s = ulong2str(character_under_cursor->GetEnergy())+"%";
    t_character_energy->Set(s);

    barre_energie.Actu (character_under_cursor->GetEnergy());
  } else {
    std::string txt( _("(dead)") );
    t_character_energy->Set( txt );
    barre_energie.Actu (0);
  }

  t_character_energy->DrawTopLeft(
		  bottom_bar_pos + WORM_ENERGY_POS + Point2i(t_ENERGY->GetWidth()+MARGIN, 0));
   
  barre_energie.DrawXY(bottom_bar_pos + ENERGY_BAR_POS);
   
  // Display team logo
  Point2i dst(pos + TEAM_ICON_POS);
  app->video.window.Blit( character_under_cursor->TeamAccess().ecusson, dst);
}

void Interface::DisplayWeaponInfo ()
{
  Weapon* weapon;
  int nbr_munition;

  // Get the weapon
  if(weapon_under_cursor==NULL)
  {
    weapon = &ActiveTeam().AccessWeapon();
    nbr_munition = ActiveTeam().ReadNbAmmos();
  }
  else
  {
    weapon = weapon_under_cursor;
    nbr_munition = ActiveTeam().ReadNbAmmos(weapon_under_cursor->GetName());
  }

  // Display the name of the weapon
  t_WEAPON->DrawTopLeft(bottom_bar_pos + WEAPON_NAME_POS);

  std::string tmp( _(weapon->GetName().c_str()) );
  t_weapon_name->Set( tmp );

  t_weapon_name->DrawTopLeft(bottom_bar_pos + WEAPON_NAME_POS + Point2i(t_WEAPON->GetWidth() + MARGIN, 0));
  
  // Display number of ammo
  if (nbr_munition ==  INFINITE_AMMO)
    tmp = _("(unlimited)");
  else
    tmp = Format("%i", nbr_munition);
 
  t_STOCK->DrawTopLeft(bottom_bar_pos + AMMOS_POS);

  t_weapon_stock->Set(tmp);
  t_weapon_stock->DrawTopLeft(
		  bottom_bar_pos + AMMOS_POS + Point2i(t_STOCK->GetWidth()+MARGIN, 0) );

  // Display weapon icon
  if( !weapon->icone.IsNull() )
      AppWormux::GetInstance()->video.window.Blit( weapon->icone, bottom_bar_pos + WEAPON_ICON_POS);
  else
      std::cout << "Can't blit weapon->icone => NULL " << std::endl;

  // Display CURRENT weapon icon on top
  weapon = &ActiveTeam().AccessWeapon();
  if (weapon != NULL) weapon->DrawWeaponBox();
}

void Interface::Draw ()
{    
  AppWormux * app = AppWormux::GetInstance();

  // display global timer
  Rectanglei dest ( (app->video.window.GetWidth()/2)-40, 0, bg_time.GetWidth(), bg_time.GetHeight() );	
  app->video.window.Blit( bg_time, dest.GetPosition() );
  std::string tmp(Time::GetInstance()->GetString());
  global_timer->Set(tmp);
  global_timer->DrawCenterTop(app->video.window.GetWidth()/2, 10); 
  
  world.ToRedrawOnScreen(dest);

  if ( GameLoop::GetInstance()->ReadState() == GameLoop::PLAYING && weapon_strength_bar.visible)
  {
    // Position on the screen
	Point2i barPos = (app->video.window.GetSize() - weapon_strength_bar.GetSize()) * Point2d(0.5, 1) 
		- Point2i(0, GetHeight() + 10);
     
    // Drawing on the screen
     weapon_strength_bar.DrawXY(barPos);
  }
       
  weapons_menu.Draw();
  
  if (!display) return;

  bottom_bar_pos = (app->video.window.GetSize() - GetSize()) * Point2d(0.5, 1);
   
  Rectanglei dr( bottom_bar_pos, game_menu.GetSize() );
  app->video.window.Blit( game_menu, bottom_bar_pos);

  world.ToRedrawOnScreen(dr);
  
  // display time left in a turn ?
  if (timer != NULL && display_timer)
    timer->DrawCenter(bottom_bar_pos + GetSize()/2 + Point2i(0, 3));
  
  // display character info
  DisplayCharacterInfo();

  // display weapon info
  DisplayWeaponInfo();
}

int Interface::GetWidth() const { return 800; }
int Interface::GetHeight() const { return 70; }
Point2i Interface::GetSize() const{
	return Point2i( GetWidth(), GetHeight() );
}

void Interface::EnableDisplay (bool _display)
{
  display = _display;
  camera.CenterOnFollowedObject();
}

void Interface::UpdateTimer(uint utimer)
{
  if (timer!= NULL)
    timer->Set( ulong2str(utimer) );
  else 
    timer = new Text(ulong2str(utimer), white_color, Font::GetInstance(Font::FONT_BIG));
}

void AbsoluteDraw(Surface &s, Point2i pos){
	Rectanglei rectSurface(pos, s.GetSize());

	if( !rectSurface.Intersect(camera) )
		return;
	
	world.ToRedrawOnMap(rectSurface);

	rectSurface.Clip( camera );

	Rectanglei rectSource(rectSurface.GetPosition() - pos, rectSurface.GetSize());
	Point2i ptDest = rectSurface.GetPosition() - camera.GetPosition();
	
	AppWormux::GetInstance()->video.window.Blit(s, rectSource, ptDest);
}

