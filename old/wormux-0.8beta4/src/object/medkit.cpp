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
 * Medkit : fall from the sky at random time.
 * Contains health for a character.
 *****************************************************************************/

#include "object/medkit.h"
#include <sstream>
#include <iostream>
#include "character/character.h"
#include "game/game_mode.h"
#include "game/time.h"
#include "graphic/sprite.h"
#include "include/app.h"
#include "include/action.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "map/map.h"
#include "network/randomsync.h"
#include "object/objects_list.h"
#include "team/macro.h"
#include "team/team.h"
#include "tool/debug.h"
#include "tool/i18n.h"
#include "tool/resource_manager.h"
#include "tool/xml_document.h"
#include "weapon/explosion.h"

// XXX Unused !?
//const uint SPEED = 5; // meter / seconde
//const uint NB_MAX_TRY = 20;
//const uint SPEED_PARACHUTE = 170; // ms par image

Medkit::Medkit()
  : ObjBox("medkit") {
  SetTestRect (29, 29, 63, 6);

  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);
  anim = resource_manager.LoadSprite( res, "object/medkit");
  resource_manager.UnLoadXMLProfile(res);

  SetSize(anim->GetSize());
  anim->animation.SetLoopMode(false);
  anim->SetCurrentFrame(0);
  std::cout<<"anim set"<<std::endl;
}

void Medkit::Draw()
{
  anim->Draw(GetPosition());
}

void Medkit::ApplyBonus(Character * c)
{
  ApplyMedkit(c->AccessTeam(), *c);
  Ghost();
}

void Medkit::ApplyMedkit(Team &/*equipe*/, Character &ver) const {
  std::ostringstream txt;
 txt << Format(ngettext(
                "%s has won %u point of energy!",
                "%s has won %u points of energy!",
                nbr_health),
            ver.GetName().c_str(), nbr_health);
  ver.SetEnergyDelta (nbr_health);
  GameMessages::GetInstance()->Add (txt.str());
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Static methods
int Medkit::nbr_health = 24;

void Medkit::LoadXml(const xmlpp::Element * object)
{
  XmlReader::ReadInt(object,"life_points",start_life_points);
  XmlReader::ReadInt(object,"energy_boost",nbr_health);
}

void Medkit::GetValueFromAction(Action * a)
{
  ObjBox::GetValueFromAction(a);
  nbr_health = a->PopInt();
}

void Medkit::StoreValue(Action * a)
{
  ObjBox::StoreValue(a);
  a->Push(nbr_health);
}
