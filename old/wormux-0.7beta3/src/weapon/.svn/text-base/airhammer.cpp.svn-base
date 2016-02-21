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
 * AirHammer - Use it to dig
 *****************************************************************************/

#include "../weapon/airhammer.h"
//-----------------------------------------------------------------------------
#include <sstream>
#include "../game/game_loop.h"
#include "../game/game.h"
#include "../map/map.h"
#include "../game/time.h"
#include "../object/objects_list.h"
#include "../team/teams_list.h"
#include "../tool/i18n.h"
#include "../interface/game_msg.h"
#include "../weapon/weapon_tools.h"

using namespace std;
//-----------------------------------------------------------------------------
namespace Wormux {

Airhammer airhammer; 

const uint MIN_TIME_BETWEEN_JOLT = 100; // in milliseconds

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

Airhammer::Airhammer() : Weapon(WEAPON_AIR_HAMMER,"airhammer")
{
  m_name = _("Airhammer");
  override_keys = true ;

  extra_params = new WeaponConfig();
}

//-----------------------------------------------------------------------------

void Airhammer::p_Init()
{
  impact = resource_manager.LoadImage( weapons_res_profile, "airhammer_impact");
  m_last_jolt = 0;
}

void Airhammer::p_Deselect()
{
  m_is_active = false;
}

//-----------------------------------------------------------------------------

bool Airhammer::p_Shoot()
{
  //jukebox.Play("weapon/airhammer");

  // initiate movement ;-)
  ActiveCharacter().SetXY(ActiveCharacter().GetX(),ActiveCharacter().GetY());

  world.Creuse (ActiveCharacter().GetX() + ActiveCharacter().GetWidth()/2 
		- impact->w/2,
		ActiveCharacter().GetY() + ActiveCharacter().GetHeight() -15,
		impact);

  return true;
}

//-----------------------------------------------------------------------------

void Airhammer::RepeatShoot()
{  
  uint time = Wormux::global_time.Read() - m_last_jolt; 
  uint tmp = Wormux::global_time.Read();

  if (time >= MIN_TIME_BETWEEN_JOLT) 
  {
    m_is_active = false;
    NewActionShoot();
    m_last_jolt = tmp;
  }    

}

//-----------------------------------------------------------------------------

void Airhammer::Refresh()
{
}

//-----------------------------------------------------------------------------

void Airhammer::HandleKeyEvent(int action, int event_type)
{
  switch (action) {    

  case ACTION_SHOOT:
    
    if (event_type == KEY_REFRESH)
      RepeatShoot();
    
    if (event_type == KEY_RELEASED) {
      // stop when key is released
      ActiveTeam().AccessNbUnits() = 0;
      m_is_active = false;
      game_loop.SetState(gameHAS_PLAYED);
    }

    break ;
    
  default:
    break ;
  } ;
      
}
//-----------------------------------------------------------------------------

} // namespace Wormux
