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
 * Parachute !
 *****************************************************************************/

#include "parachute.h"
#include "weapon_tools.h"
#include "../game/game.h"
#include "../game/game_mode.h"
#include "../game/game_loop.h"
#include "../interface/game_msg.h"
#include "../object/physical_obj.h"
#include "../sound/jukebox.h"
#include "../team/teams_list.h"
#include "../tool/i18n.h"

Parachute::Parachute() : Weapon(WEAPON_PARACHUTE, "parachute", new ParachuteConfig(), NEVER_VISIBLE)
{
  m_name = _("Parachute");
  m_initial_nb_ammo = 2 ;
  use_unit_on_first_shoot = false;    
  
  image = resource_manager.LoadSprite(weapons_res_profile,"parachute_sprite");
}

void Parachute::p_Select()
{
  m_is_active = true ;
  open = false ;
  closing = false ;
  image->animation.SetShowOnFinish(SpriteAnimation::show_last_frame);
}

void Parachute::p_Deselect()
{
  ActiveCharacter().SetAirResistFactor(GameMode::GetInstance()->character.air_resist_factor);
  ActiveCharacter().SetWindFactor(0);
  m_is_active = false;
}

bool Parachute::p_Shoot()
{
  GameMessages::GetInstance()->Add(_("Parachute is activated automatically."));
  return false;
}

void Parachute::Draw()
{
  if (open)
    {
      image->Update();
      image->Draw(ActiveCharacter().GetPosition() - 
			  Point2i(ActiveCharacter().GetWidth()/2,image->GetHeight()) );
    }
}

void Parachute::Refresh()
{
  Point2d speed;

  ActiveCharacter().GetSpeedXY(speed);

  if (ActiveCharacter().FootsInVacuum())
    {
      if (!open && (speed.y > cfg().open_speed_limit))
	{
	  if (EnoughAmmo())
	    {
	      UseAmmo();
	      ActiveCharacter().SetAirResistFactor(cfg().air_resist_factor);
	      ActiveCharacter().SetWindFactor(cfg().wind_factor);
	      open = true ;
	      image->animation.SetPlayBackward(false);
	      image->Start();

	    }
	}
    }
  else
    {
      /* We are on the ground */
      if (open)
	{
	  /* The parachute is opened */
	  if (!closing)
	    {
	      /* We have just hit the ground. Start closing animation */
	      image->animation.SetPlayBackward(true);
	      image->animation.SetShowOnFinish(SpriteAnimation::show_blank);
	      image->Start();
	      closing = true ;
	    }
	  else
	    {/* The parachute is closing */
	      if (image->IsFinished())
		{
		  /* The animation is finished...
		     We are done with the parachute */
		  open = false ;
		  closing = false ;
		  UseAmmoUnit();
		}
	    }
	}
    }
}

void Parachute::SignalTurnEnd()
{
  p_Deselect();
}

ParachuteConfig& Parachute::cfg() {
  return static_cast<ParachuteConfig&>(*extra_params);
}

ParachuteConfig::ParachuteConfig(){ 
  wind_factor = 10.0;
  air_resist_factor = 140.0 ;
  open_speed_limit = 2.0 ;
}

void ParachuteConfig::LoadXml(xmlpp::Element *elem){
  WeaponConfig::LoadXml(elem);
  LitDocXml::LitDouble (elem, "wind_factor", wind_factor);
  LitDocXml::LitDouble (elem, "air_resist_factor", air_resist_factor);
  LitDocXml::LitDouble (elem, "open_speed_limit", open_speed_limit);
}
