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
 * Medkit : fall from the sky at random time.
 * Contains health for a character.
 *****************************************************************************/

#include "object/medkit.h"
#include "character/character.h"
#include "game/game_mode.h"
#include "graphic/sprite.h"
#include "interface/game_msg.h"
#include "object/objects_list.h"
#include "sound/jukebox.h"
#include "team/team.h"
#include <WARMUX_debug.h>
#include "tool/resource_manager.h"
#include "tool/xml_document.h"

Sprite* Medkit::icon = NULL;
int Medkit::icon_ref = 0;

Medkit::Medkit()
  : ObjBox("medkit") {
  SetTestRect (29, 29, 63, 6);

  Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml", false);
  anim = GetResourceManager().LoadSprite(res, "object/medkit");

  SetSize(anim->GetSize());
  anim->animation.SetLoopMode(false);
  anim->SetCurrentFrame(0);

  if (!icon) {
    icon = CreateIcon();
  }
  icon_ref++;
}

Medkit::~Medkit()
{
  icon_ref--;
  if (!icon_ref) {
    delete icon;
    icon = NULL;
  }
}

void Medkit::ApplyBonus(Character * c)
{
  JukeBox::GetInstance()->Play("default","box/medkit_picking_up");
  ApplyMedkit(c->AccessTeam(), *c);
  Ghost();
}

void Medkit::ApplyMedkit(Team &team, Character &player) const
{
  std::string txt = Format(_("%s has won %u points of energy!"),
                           player.GetName().c_str(), nbr_health);
  player.SetEnergyDelta(nbr_health, &player);
  if (player.IsDiseased())
    player.Cure();
  GameMessages::GetInstance()->Add(txt, team.GetColor());
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Static methods
int Medkit::nbr_health = 24;

void Medkit::LoadXml(const xmlNode*  object)
{
  bool r;
  r = XmlReader::ReadInt(object, "life_points", start_life_points);
  if (!r)
    start_life_points = 41;

  r = XmlReader::ReadInt(object, "energy_boost", nbr_health);
  if (!r)
    nbr_health = 24;
}

const Surface* Medkit::GetIcon() const
{
  ASSERT(icon);
  icon->SetCurrentFrame(anim->GetCurrentFrame());
  icon->RefreshSurface();
  return &icon->GetSurface();
}
