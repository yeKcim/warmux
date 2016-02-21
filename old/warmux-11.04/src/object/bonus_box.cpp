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
 * Bonus box : fall from the sky at random time.
 * The box can contain any weapon in the game.
 *****************************************************************************/

#include <sstream>
#include <iostream>

#include <WARMUX_random.h>

#include "object/bonus_box.h"
#include "character/character.h"
#include "graphic/sprite.h"
#include "interface/game_msg.h"
#include "network/randomsync.h"
#include "sound/jukebox.h"
#include "team/macro.h"
#include "team/team.h"
#include "tool/resource_manager.h"
#include "tool/xml_document.h"
#include "tool/string_tools.h"

Sprite* BonusBox::icon = NULL;
int BonusBox::icon_ref = 0;

BonusBox::BonusBox(Weapon * weapon):
  ObjBox("bonus_box"),
  weapon(weapon)
{
  SetTestRect(29, 29, 63, 6);

  Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml", false);
  anim = GetResourceManager().LoadSprite( res, "object/bonus_box");

  SetSize(anim->GetSize());
  anim->animation.SetLoopMode(false);
  anim->SetCurrentFrame(0);

  if (!icon) {
    icon = CreateIcon();
  }
  icon_ref++;
}

BonusBox::~BonusBox()
{
  icon_ref--;
  if (!icon_ref) {
    delete icon;
    icon = NULL;
  }
}

void BonusBox::ApplyBonus(Character * c)
{
  std::ostringstream txt;
  if (ExplodesInsteadOfBonus(c)) {
    GameMessages::GetInstance()->Add(_("Someone put a booby trap into the crate!"),
                                     c->GetTeam().GetColor());
    Explode();
    return;
  };
  Weapon::Weapon_type w_type = weapon->GetType();

  if (c->AccessTeam().ReadNbAmmos(w_type) != INFINITE_AMMO) {
    int won_ammo = weapon->GetAmmoPerDrop();
    c->AccessTeam().m_nb_ammos[w_type] += won_ammo;
    txt << weapon->GetWeaponWinString(c->AccessTeam().GetName().c_str(), won_ammo);
  } else {
    // Can happen if the configuration is wrong...
    txt << Format(_("%s team already has infinite ammo for the %s!"),
           c->AccessTeam().GetName().c_str(), weapon->GetName().c_str());
  }
  GameMessages::GetInstance()->Add(txt.str(), c->GetTeam().GetColor());
  JukeBox::GetInstance()->Play("default","box/picking_up");
}

bool BonusBox::ExplodesInsteadOfBonus(Character * c)
{
  ASSERT(c);

  // Empyric formula:
  // 1% chance of explosion for each 5 points of energy
  // (with max 20% for 100 energy)
  uint explosion_probability = c->GetEnergy() / 5;

  #define MIN_EXPLOSION_PROBABILITY   5
  #define MAX_EXPLOSION_PROPABILITY  40
  // clamp to some reasonable values
  if (explosion_probability < MIN_EXPLOSION_PROBABILITY)
    explosion_probability = MIN_EXPLOSION_PROBABILITY;
  else if (explosion_probability > MAX_EXPLOSION_PROPABILITY)
    explosion_probability = MAX_EXPLOSION_PROPABILITY;

  uint randval = RandomSync().GetUint(1, 100);
  bool exploding = randval < explosion_probability;
  MSG_DEBUG("bonus","explosion chance: %u%%, actual value: %u, %sexploding",
            explosion_probability, randval, exploding ? "" : "not ");

  return exploding;
}

const Surface* BonusBox::GetIcon() const
{
  ASSERT(icon);
  icon->SetCurrentFrame(anim->GetCurrentFrame());
  icon->RefreshSurface();
  return &icon->GetSurface();
}
