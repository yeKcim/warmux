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
 * Jet Pack :-)
 *****************************************************************************/

#include "jetpack.h"
#include "weapon_tools.h"
#include "../game/game.h"
#include "../game/game_loop.h"
#include "../game/game_mode.h"
#include "../game/time.h"
#include "../interface/game_msg.h"
#include "../map/camera.h"
#include "../object/physical_obj.h"
#include "../sound/jukebox.h"
#include "../team/teams_list.h"
#include "../tool/i18n.h"


// Espace entre l'espace en l'image
const uint ESPACE = 5;
const double JETPACK_FORCE = 750.0;

const uint DELTA_FUEL_DOWN = 200 ;  // Delta time between 2 fuel unit consumption.

JetPack::JetPack() : Weapon(WEAPON_JETPACK, "jetpack",
			    new WeaponConfig(),
			    NEVER_VISIBLE)
{
  m_name = _("JetPack");
  m_unit_visibility = VISIBLE_ONLY_WHEN_ACTIVE;

  override_keys = true ;
  use_unit_on_first_shoot = false;  

  m_name = _("jetpack");
  m_x_force = 0.0;
  m_y_force = 0.0;
  channel = -1;
}

void JetPack::Refresh()
{
  Point2d F;

  if (m_is_active)
    {
      F.x = m_x_force ;
      F.y = m_y_force ;

      ActiveCharacter().SetExternForceXY(F);
      ActiveCharacter().UpdatePosition();

      if( !F.IsNull() )
	{
	  // We are using fuel !!!

	  uint current = Time::GetInstance()->Read() ;
	  double delta = (double)(current - m_last_fuel_down);

	  while (delta >= DELTA_FUEL_DOWN)
	    {
	      if (EnoughAmmoUnit())
		{
		  UseAmmoUnit();
		  m_last_fuel_down += DELTA_FUEL_DOWN ;
		  delta -= DELTA_FUEL_DOWN ;
		}
	      else
		{
		  p_Deselect();
		  break;
		}
	    }
	}
    }
}

void JetPack::p_Select()
{
}

void JetPack::p_Deselect()
{
  m_is_active = false;
  m_x_force = 0;
  m_y_force = 0;
  ActiveCharacter().SetExternForce(0,0);
  StopUse();
  ActiveCharacter().SetSkin("walking");
}

void JetPack::StartUse()
{
  if ( (m_x_force == 0) && (m_y_force == 0))
    {
      m_last_fuel_down = Time::GetInstance()->Read();
      channel = jukebox.Play(ActiveTeam().GetSoundProfile(),"weapon/jetpack", -1);

      camera.ChangeObjSuivi (&ActiveCharacter(),true, true, true); 
// 			     bool suit, bool recentre,
// 			     bool force_recentrage=false);
    }
}

void JetPack::StopUse()
{
  if (m_x_force == 0.0 && m_y_force == 0.0)
  {
    if(channel != -1)
      jukebox.Stop(channel);
    channel = -1;
    ActiveCharacter().SetSkin("jetpack");
  }
}

void JetPack::GoUp()
{
  StartUse();
  m_y_force = -(ActiveCharacter().GetMass() * GameMode::GetInstance()->gravity + JETPACK_FORCE) ;
  ActiveCharacter().SetSkin("jetpack-up");
}

void JetPack::GoLeft()
{
  StartUse();
  ActiveCharacter().SetSkin("jetpack-left-right");
  ActiveCharacter().image->Scale (-1, 1);

  m_x_force = - JETPACK_FORCE ;
}

void JetPack::GoRight()
{
  StartUse();
  ActiveCharacter().SetSkin("jetpack-left-right");
  ActiveCharacter().image->Scale (1, 1);

  m_x_force = JETPACK_FORCE ;
}

void JetPack::StopUp()
{
  m_y_force = 0.0 ;
  StopUse();
}

void JetPack::StopLeft()
{
  m_x_force = 0.0 ;
  StopUse();
}

void JetPack::StopRight()
{
  m_x_force = 0.0 ;
  StopUse();
}

void JetPack::HandleKeyEvent(int action, int event_type)
{
  switch (action) {
    case ACTION_UP:
      if (event_type == KEY_PRESSED)
	GoUp();
      else
	if (event_type == KEY_RELEASED)
	  StopUp();
      break ;

    case ACTION_MOVE_LEFT:
      if (event_type == KEY_PRESSED)
	GoLeft();
      else
	if (event_type == KEY_RELEASED)
	  StopLeft();
      break ;

    case ACTION_MOVE_RIGHT:
      if (event_type == KEY_PRESSED)
	GoRight();
      else
	if (event_type == KEY_RELEASED)
	  StopRight();
      break ;

    case ACTION_SHOOT:
      if (event_type == KEY_PRESSED)
	p_Deselect();
      break ;

    default:
      break ;
  } ;
}

bool JetPack::p_Shoot()
{
  m_is_active = true;
  ActiveCharacter().SetSkin("jetpack");

  return true;
}

void JetPack::SignalTurnEnd()
{
  p_Deselect();
}

