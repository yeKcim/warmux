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
//-----------------------------------------------------------------------------
#include <sstream>
#include "../map/camera.h"
#include "../map/map.h"
#include "../game/game_loop.h"
#include "../game/game_mode.h"
#include "../team/teams_list.h"
#include "../tool/string_tools.h"
#include "../weapon/weapons_list.h"
#include "../graphic/colors.h"
#include "../tool/i18n.h"
#include "../graphic/video.h"
#include <SDL.h>
#include "../tool/resource_manager.h"
#include "../include/app.h"
#include "../game/time.h"
#include <iostream>
#include "../include/global.h"

//-----------------------------------------------------------------------------
WeaponStrengthBar weapon_strength_bar;
//-----------------------------------------------------------------------------

using namespace Wormux;
//-----------------------------------------------------------------------------

// Nom du ver
const uint NOM_VER_X = 32;
const uint NOM_VER_Y = 28;

// Energie du ver
const uint ENERGIE_VER_X = NOM_VER_X;
const uint ENERGIE_VER_Y = NOM_VER_Y+20;

// Nom de l'arme
const uint NOM_ARME_X = 508;
const uint NOM_ARME_Y = NOM_VER_Y;

// Munitions
const uint MUNITION_X = NOM_ARME_X;
const uint MUNITION_Y = ENERGIE_VER_Y;

// Ecusson de l'équipe
const uint ECUSSON_EQUIPE_X = 303;
const uint ECUSSON_EQUIPE_Y = 20;

// Icône de l'arme
const uint ICONE_ARME_X = 450;
const uint ICONE_ARME_Y = 20;

const uint INFO_VER_X2 = 296;

// Barre d'énergie
const uint BARENERGIE_X = 170;
const uint BARENERGIE_Y = ENERGIE_VER_Y+2;
const uint BARENERGIE_LARG = INFO_VER_X2-BARENERGIE_X;
const uint BARENERGIE_HAUT = 15;

const uint MARGIN = 10;

//-----------------------------------------------------------------------------
Interface interface;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

Interface::Interface()
{
}

//-----------------------------------------------------------------------------

void Interface::Init()
{
  display = true;

  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);
  game_menu = resource_manager.LoadImage( res, "interface/menu_jeu");
  bg_time = resource_manager.LoadImage( res, "interface/fond_compteur");
  weapons_menu.Init();
  weapon_box_button = resource_manager.LoadImage( res, "interface/weapon_box_button");
   
  barre_energie.InitVal (0, 0, game_mode.character.init_energy);
  barre_energie.InitPos (BARENERGIE_X, BARENERGIE_Y, 
			 BARENERGIE_LARG, BARENERGIE_HAUT);
  barre_energie.border_color = white_color;
  barre_energie.value_color = lightgray_color;
  barre_energie.background_color = gray_color;  
   
  // strength bar initialisation  
  weapon_strength_bar.InitPos (0, 0, 400, 10);
  weapon_strength_bar.InitVal (0, 0, 100);

  weapon_strength_bar.SetValueColor (255, 255, 255, 127);
  weapon_strength_bar.SetBorderColor (0, 0, 0, 127);
  weapon_strength_bar.SetBackgroundColor (255*6/10, 255*6/10, 255*6/10, 96); 

  // constant text initialisation
  t_NAME = new Text( _("Name:"), white_color, &global().normal_font());
  t_ENERGY = new Text( _("Energy:"),white_color, &global().normal_font());
  t_WEAPON = new Text( _("Weapon:"), white_color, &global().normal_font());
  t_STOCK = new Text( _("Stock:"), white_color, &global().normal_font());

  global_timer = new Text(ulong2str(0), white_color, &global().normal_font());
  timer = NULL;

  t_character_name = new Text("None", white_color, &global().normal_font());
  t_character_energy = new Text("Dead", white_color, &global().normal_font());
  t_weapon_name = new Text("None", white_color, &global().normal_font());
  t_weapon_stock = new Text("0", white_color, &global().normal_font());;
  
  
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

void Interface::Reset()
{
  character_under_cursor = NULL;
  weapon_under_cursor = NULL;
  weapons_menu.Reset();
  barre_energie.InitVal (0, 0, game_mode.character.init_energy);
}

//-----------------------------------------------------------------------------

void Interface::DisplayCharacterInfo ()
{

  int x = (video.GetWidth() - GetWidth())/2;
  int y = video.GetHeight() - GetHeight();

  // Get the character
  if (character_under_cursor == NULL) character_under_cursor = &ActiveCharacter();

  // Display name
  t_NAME->DrawTopLeft(bottom_bar_ox+NOM_VER_X, 
		      bottom_bar_oy+NOM_VER_Y);

  std::string s(character_under_cursor->m_name+" ("+character_under_cursor->GetTeam().GetName()+" )");
  t_character_name->Set(s);

  t_character_name->DrawTopLeft(bottom_bar_ox+NOM_VER_X+t_NAME->GetWidth()+MARGIN,
				bottom_bar_oy+NOM_VER_Y);
  
  // Display energy
  t_ENERGY->DrawTopLeft(bottom_bar_ox+ENERGIE_VER_X,
			bottom_bar_oy+ENERGIE_VER_Y);

  if (!character_under_cursor->IsDead()) {
    s = ulong2str(character_under_cursor->GetEnergy())+"%";
    t_character_energy->Set(s);

    barre_energie.Actu (character_under_cursor->GetEnergy());
  } else {
    std::string txt( _("(dead)") );
    t_character_energy->Set( txt );
    barre_energie.Actu (0);
  }

  t_character_energy->DrawTopLeft(bottom_bar_ox+ENERGIE_VER_X+t_ENERGY->GetWidth()+MARGIN,
				  bottom_bar_oy+ENERGIE_VER_Y);
   
  barre_energie.DrawXY (bottom_bar_ox+BARENERGIE_X,bottom_bar_oy+BARENERGIE_Y);
   
  // Display team logo
  SDL_Rect dest = { x+ECUSSON_EQUIPE_X, y+ECUSSON_EQUIPE_Y, character_under_cursor->TeamAccess().ecusson->w, character_under_cursor->TeamAccess().ecusson->h};	
  SDL_BlitSurface( character_under_cursor->TeamAccess().ecusson, NULL, app.sdlwindow, &dest);


}

//-----------------------------------------------------------------------------

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
  t_WEAPON->DrawTopLeft(bottom_bar_ox+NOM_ARME_X, 
		     bottom_bar_oy+NOM_ARME_Y);

  std::string tmp( _(weapon->GetName().c_str()) );
  t_weapon_name->Set( tmp );

  t_weapon_name->DrawTopLeft(bottom_bar_ox+NOM_ARME_X+t_WEAPON->GetWidth()+MARGIN, 
			  bottom_bar_oy+NOM_ARME_Y);
  
  // Display number of ammo
  if (nbr_munition ==  INFINITE_AMMO)
    tmp = _("(unlimited)");
  else
    tmp = Format("%i", nbr_munition);
 
  t_STOCK->DrawTopLeft(bottom_bar_ox+MUNITION_X, 
		       bottom_bar_oy+MUNITION_Y);

  t_weapon_stock->Set(tmp);
  t_weapon_stock->DrawTopLeft(bottom_bar_ox+MUNITION_X+t_STOCK->GetWidth()+MARGIN, 
			      bottom_bar_oy+MUNITION_Y);

  // Display weapon icon
  if( weapon->icone )
    {
      SDL_Rect dest_rect = { bottom_bar_ox+ICONE_ARME_X, bottom_bar_oy+ICONE_ARME_Y, weapon->icone->w, weapon->icone->h};	
      SDL_BlitSurface( weapon->icone, NULL, app.sdlwindow, &dest_rect);   
    }
  else
    {
      std::cout << "Can't blit weapon->icone => NULL " << std::endl;
    }

  // Display CURRENT weapon icon on top
  weapon = &ActiveTeam().AccessWeapon();
  if (weapon != NULL) weapon->DrawWeaponBox();
}

//-----------------------------------------------------------------------------

void Interface::Draw ()
{    
  // display global timer
  SDL_Rect dest = { (video.GetWidth()/2)-40, 0, bg_time->w, bg_time->h};	
  SDL_BlitSurface( bg_time, NULL, app.sdlwindow, &dest);   
  std::string tmp(global_time.GetString());
  global_timer->Set(tmp);
  global_timer->DrawCenterTop(video.GetWidth()/2, 10); 
  
  Rectanglei tmpr(dest.x,dest.y, dest.w, dest.h);
  world.ToRedrawOnScreen(tmpr);

  if ( game_loop.ReadState() == gamePLAYING && weapon_strength_bar.visible)
  {
    // Position on the screen 
    uint barre_x = (video.GetWidth()-weapon_strength_bar.GetWidth())/2;
    uint barre_y = video.GetHeight()-weapon_strength_bar.GetHeight() 
                   - interface.GetHeight()-10;
     
    // Drawing on the screen
     weapon_strength_bar.DrawXY (barre_x, barre_y);
  }
       
  weapons_menu.Draw();
  
  if (!display) return;

  int x = (video.GetWidth() - GetWidth())/2;
  int y = video.GetHeight() - GetHeight();

  bottom_bar_ox = x;
  bottom_bar_oy = y;
   
  SDL_Rect dr = { x, y, game_menu->w, game_menu->h};	
  SDL_BlitSurface( game_menu, NULL, app.sdlwindow, &dr);   

  Rectanglei tmpr2(dr.x,dr.y, dr.w, dr.h);
  world.ToRedrawOnScreen(tmpr2);
  
  // display time left in a turn ?
  if (timer != NULL && display_timer)
    timer->DrawCenter(x+GetWidth()/2, y+GetHeight()/2+3);
  
  // display character info
  DisplayCharacterInfo();

  // display weapon info
  DisplayWeaponInfo();
}

//-----------------------------------------------------------------------------

uint Interface::GetWidth() const { return 800; }
uint Interface::GetHeight() const { return 70; }

//-----------------------------------------------------------------------------

void Interface::EnableDisplay (bool _display)
{
  display = _display;
  camera.CentreObjSuivi ();
}

//-----------------------------------------------------------------------------

void Interface::UpdateTimer(uint utimer)
{
  if (utimer >= 0){
    if (timer!= NULL) {
      std::string s(ulong2str(utimer));
      timer->Set(s);
    }
    else timer = new Text(ulong2str(utimer), white_color, &global().big_font());
  } else {
    timer = NULL;
  }
}


//-----------------------------------------------------------------------------

void AbsoluteDraw(SDL_Surface* s, int x, int y)
{
  world.ToRedrawOnMap(Rectanglei(x, y, s->w, s->h));

  assert(s!=NULL);
/*  if(x + s->w < 0 || y + s->h < 0)
  {
    std::cout << "WARNING: Trying to display a SDL_Surface out of the screen!" << std::endl;
    return;
  }
*/  
  if(x + s->w < (int)camera.GetX() || x > (int)camera.GetX()+(int)camera.GetWidth()
  || y + s->h < (int)camera.GetY() || y > (int)camera.GetY()+(int)camera.GetHeight())
  {
    //Drawing out of camera area
    return;
  }

  SDL_Rect src={0,0,s->w,s->h};
  SDL_Rect dst={x - (int)camera.GetX(), y - (int)camera.GetY(), s->w , s->h};

  if(dst.x<0)
  {
    src.w+=src.x;
    src.x=0;
  }

  if(dst.x+src.w>camera.GetX())
  {
    src.w=camera.GetWidth()-src.x;
  }

  if(dst.y<0)
  {
    src.h+=src.y;
    src.y=0;
  }

  if(dst.y+src.h>camera.GetY())
  {
    src.h=camera.GetHeight()-src.y;
  }

  

  //TODO:blit only the displayed part of the SDL_Surface
  SDL_BlitSurface(s,&src,app.sdlwindow,&dst);
}
