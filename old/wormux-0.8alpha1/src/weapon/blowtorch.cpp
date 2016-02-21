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
 * Blowtorch - burns holes into walls
 *****************************************************************************/

#include "../weapon/blowtorch.h"
#include "../tool/i18n.h"
#include "../map/map.h"
#include "../team/teams_list.h"
#include "../game/game_loop.h"
#include "../game/time.h"
#include "../character/move.h"
#include "../character/body.h"
#include "explosion.h"

static const uint pause_time = 200;	// milliseconds

Blowtorch::Blowtorch() : Weapon(WEAPON_BLOWTORCH, "blowtorch", new BlowtorchConfig())
{
	m_name = _("Blowtorch");
	override_keys = true;

	new_timer = 0;
	old_timer = 0;

	m_weapon_fire = new Sprite(resource_manager.LoadImage(weapons_res_profile, "blowtorch_fire"));
}

void Blowtorch::Refresh()
{

}

void Blowtorch::EndTurn()
{
	ActiveCharacter().body->ResetWalk();
	ActiveCharacter().body->StopWalk();
	ActiveTeam().AccessNbUnits() = 0;
	m_is_active = false;

	// XXX This doesn't seem to be the correct to end a turn, does it?
	GameLoop::GetInstance()->SetState(GameLoop::HAS_PLAYED);
}

bool Blowtorch::p_Shoot()
{
	ActiveCharacter().SetRebounding(false);
	ActiveCharacter().body->StartWalk();

	Point2i hole = ActiveCharacter().GetCenter();

	double angle = ActiveTeam().crosshair.GetAngleRad();
	uint h = cfg().range;
	double dx = cos(angle) * h;
	double dy = sin(angle) * h;

	Point2i pos = Point2i(hole.x+(int)dx, hole.y+(int)dy);
	world.Dig(pos, ActiveCharacter().GetHeight()/2);

	MoveCharacter(ActiveCharacter());
	ActiveCharacter().SetXY(ActiveCharacter().GetPosition());

	return true;
}

void Blowtorch::HandleKeyEvent(int action, int event_type)
{
	switch(action)
	{
		case ACTION_SHOOT:
			if(event_type == KEY_RELEASED)
				EndTurn();
			else if(event_type == KEY_REFRESH)
			{
				if(!EnoughAmmoUnit() || ActiveCharacter().GotInjured())
					EndTurn();

				new_timer = Time::GetInstance()->Read();
				if(new_timer - old_timer >= pause_time)
				{
					NewActionShoot();
					old_timer = new_timer;
				}
			}

			break;
		default:
			break;
	}
}

//-------------------------------------------------------------------------------------

BlowtorchConfig::BlowtorchConfig()
{
	range = 2;
}

BlowtorchConfig& Blowtorch::cfg()
{
	return static_cast<BlowtorchConfig&>(*extra_params);
}

void BlowtorchConfig::LoadXml(xmlpp::Element* elem)
{
	WeaponConfig::LoadXml(elem);
	LitDocXml::LitUint(elem, "range", range);
}
