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
 * Bonus box : fall from the sky at random time.
 * The box can contain bonus or mallus (dynamite, Guns, loss of energy etc).
 *****************************************************************************/

#include "bonus_box.h"
#include <sstream>
#include <iostream>
#include "../game/game_loop.h"
#include "../game/time.h"
#include "../graphic/sprite.h"
#include "../include/app.h"
#include "../interface/game_msg.h"
#include "../map/camera.h"
#include "../map/map.h"
#include "../network/randomsync.h"
#include "../object/objects_list.h"
#include "../team/macro.h"
#include "../tool/debug.h"
#include "../tool/i18n.h"
#include "../tool/resource_manager.h"
#include "../weapon/weapons_list.h"

//#define FAST

#ifdef FAST
  const uint MIN_TIME_BETWEEN_CREATION = 1; // seconds
  const uint MAX_TIME_BETWEEN_CREATION = 3; // seconds
#else
  const uint MIN_TIME_BETWEEN_CREATION = 10; // seconds
  const uint MAX_TIME_BETWEEN_CREATION = 5*60; // seconds
#endif

const uint SPEED = 5; // meter / seconde
const uint SPEED_PARACHUTE = 170; // ms par image
const uint NB_MAX_TRY = 20;

// Bonus offert par la caisse
const uint BONUS_ENERGY=100;
const uint BONUS_TRAP=75;
const uint BONUS_DYNAMITE=3;
const uint BONUS_ANVIL=1;
const uint BONUS_BASEBALL=3;
const uint BONUS_HOLLY_GRENADE=1;
const uint BONUS_LOWGRAV=2;
const uint BONUS_AIR_ATTACK=1;
const uint BONUS_TELEPORTATION=2;
const uint BONUS_AUTO_BAZOOKA=3;
const uint BONUS_RIOT_BOMB=2;



BonusBox::BonusBox()
  : PhysicalObj("bonus_box"){
  SetTestRect (29, 29, 63, 6);
  m_allow_negative_y = true;
  enable = false;

  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);
  anim = resource_manager.LoadSprite( res, "objet/caisse");
  resource_manager.UnLoadXMLProfile(res);

  SetSize(anim->GetSize());
  anim->animation.SetLoopMode(false);
  anim->SetCurrentFrame(0);

  parachute = true;

  SetSpeed (SPEED, M_PI_2);
}

BonusBox::~BonusBox(){
  delete anim;
}

void BonusBox::Draw()
{
  anim->Draw(GetPosition());
}

void BonusBox::Refresh()
{
  // If we touch a character, we remove the bonus box
  FOR_ALL_LIVING_CHARACTERS(equipe, ver)
  {
    if( ObjTouche(*ver) )
    {
      // here is the gift (truly a gift ?!? :)
      ApplyBonus (**equipe, *ver);
      Ghost();
      return;
    }
  }

  // Refresh animation
  if (!anim->IsFinished() && !parachute) anim->Update();
}

// Signale la fin d'une chute
void BonusBox::SignalCollision()
{
  SetAirResistFactor(1.0);

  MSG_DEBUG("bonus", "End of the fall: parachute=%d", parachute);
  if (!parachute) return;

  MSG_DEBUG("bonus", "Start of the animation 'fold of the parachute'.");
  parachute = false;

  anim->SetCurrentFrame(0);
  anim->Start();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void BonusBox::ApplyBonus (Team &equipe, Character &ver){
  std::ostringstream txt;
  uint bonus = randomSync.GetLong (1, nb_bonus);
  switch (bonus){
  case bonusTELEPORTATION:
    txt << Format(ngettext(
                "%s team has won %u teleportation.",
                "%s team has won %u teleportations.",
                BONUS_TELEPORTATION),
            ActiveTeam().GetName().c_str(), BONUS_TELEPORTATION);
    equipe.m_nb_ammos[ weapons_list.GetWeapon(WEAPON_TELEPORTATION)->GetName() ] += BONUS_TELEPORTATION;
    break;

  case bonusENERGY:
    txt << Format(ngettext(
                "%s has won %u point of energy!",
                "%s has won %u points of energy!",
                BONUS_ENERGY),
            ver.GetName().c_str(), BONUS_ENERGY);
    ver.SetEnergyDelta (BONUS_ENERGY);
    break;

  case bonusTRAP:
    txt << Format(ngettext(
                "%s has lost %u point of energy.",
                "%s has lost %u points of energy.",
                BONUS_TRAP),
            ver.GetName().c_str(), BONUS_TRAP);
    ver.SetEnergyDelta (-BONUS_TRAP);
    break;

  case bonusAIR_ATTACK:
    txt << Format(ngettext(
                "'%s has won %u Air Attack",
                "'%s has won %u Air Attacks",
                BONUS_AIR_ATTACK),
            ActiveTeam().GetName().c_str(), BONUS_AIR_ATTACK);
    equipe.m_nb_ammos[ weapons_list.GetWeapon(WEAPON_AIR_ATTACK)->GetName() ] += BONUS_AIR_ATTACK;
    break;

  case bonusBASEBALL:
    txt << Format(ngettext(
                "'%s has won %u Baseball bat",
                "'%s has won %u Baseball bats",
                BONUS_BASEBALL),
            ActiveTeam().GetName().c_str(), BONUS_BASEBALL);
    equipe.m_nb_ammos[ weapons_list.GetWeapon(WEAPON_BASEBALL)->GetName() ] += BONUS_BASEBALL;
    break;

  case bonusLOWGRAV:
    txt << Format(ngettext(
                "'%s has won %u Low gravity",
                "'%s has won %u Low gravity",
                BONUS_LOWGRAV),
            ActiveTeam().GetName().c_str(), BONUS_LOWGRAV);
    equipe.m_nb_ammos[ weapons_list.GetWeapon(WEAPON_LOWGRAV)->GetName() ] += BONUS_LOWGRAV;
    break;

  case bonusANVIL:
    txt << Format(ngettext(
                "'%s has won %u Anvil",
                "'%s has won %u Anvil",
                BONUS_ANVIL),
            ActiveTeam().GetName().c_str(), BONUS_ANVIL);
    equipe.m_nb_ammos[ weapons_list.GetWeapon(WEAPON_ANVIL)->GetName() ] += BONUS_ANVIL;
    break;

  case bonusHOLLY_GRENADE:
    txt << Format(ngettext(
                "'%s has won %u Holy grenade",
                "'%s has won %u Holy grenades",
                BONUS_HOLLY_GRENADE),
            ActiveTeam().GetName().c_str(), BONUS_HOLLY_GRENADE);
    equipe.m_nb_ammos[ weapons_list.GetWeapon(WEAPON_HOLLY_GRENADE)->GetName() ] += BONUS_HOLLY_GRENADE;
    break;

  case bonusAUTO_BAZOOKA:
    txt << Format(ngettext(
                "%s team has won %u Automatic Bazooka!",
                "%s team has won %u Automatic Bazookas!",
                BONUS_AUTO_BAZOOKA),
		  ActiveTeam().GetName().c_str(), BONUS_AUTO_BAZOOKA);
    equipe.m_nb_ammos[ weapons_list.GetWeapon(WEAPON_AUTOMATIC_BAZOOKA)->GetName() ] += BONUS_AUTO_BAZOOKA;
    break;

  case bonusRIOT_BOMB:
    txt << Format(ngettext(
                "%s team has won %u Riot Bomb!",
                "%s team has won %u Riot Bombs!",
                BONUS_RIOT_BOMB),
		  ActiveTeam().GetName().c_str(), BONUS_RIOT_BOMB);
    equipe.m_nb_ammos[ weapons_list.GetWeapon(WEAPON_RIOT_BOMB)->GetName() ] += BONUS_RIOT_BOMB;
    break;

  default: std::cout << bonus << std::endl; assert (false);
  case bonusDYNAMITE:
    txt << Format(ngettext(
                "%s team has won %u stick of Dynamite!",
                "%s team has won %u sticks of Dynamite!",
                BONUS_DYNAMITE),
            ActiveTeam().GetName().c_str(), BONUS_DYNAMITE);
    equipe.m_nb_ammos[ weapons_list.GetWeapon(WEAPON_DYNAMITE)->GetName() ] += BONUS_DYNAMITE;
    break;
  }

  GameMessages::GetInstance()->Add (txt.str());
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Static methods
bool BonusBox::enable = false;
uint BonusBox::time = 0;

// Active les caisses ?
void BonusBox::Enable (bool _enable)
{
  MSG_DEBUG("bonus", "Enable ? %d", _enable);
  enable = _enable;
}

bool BonusBox::PlaceBonusBox (BonusBox& bonus_box)
{
  if (!bonus_box.PutRandomly(true, 0)) return false;

  time = randomSync.GetLong(MIN_TIME_BETWEEN_CREATION,
			   MAX_TIME_BETWEEN_CREATION-MIN_TIME_BETWEEN_CREATION);
  time *= 1000;
  time += Time::GetInstance()->Read();

  return true;
}

bool BonusBox::NewBonusBox()
{

  if (!enable || (Time::GetInstance()->Read() < time)) {
    return false;
  }

  BonusBox * box = new BonusBox();
  if (!PlaceBonusBox(*box)) {
    MSG_DEBUG("bonus", "Missed to put the bonus box");
    delete box;
  } else {
    lst_objects.AddObject(box);
    camera.ChangeObjSuivi(box, true, true);
    GameMessages::GetInstance()->Add (_("Is it a gift?"));
    return true;
  }

  return false;
}
