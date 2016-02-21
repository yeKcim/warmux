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
 * Caisse de bonus : tombe du ciel après un temps d'apparition aléatoire.
 * Lorsqu'un ver touche la caisse, il peut gagner différents bonus : munition,
 * énergie (gain ou perte!), etc.
 *****************************************************************************/

#include "bonus_box.h"
#include <sstream>
#include <iostream>
#include "../game/game_loop.h"
#include "../game/time.h"
#include "../graphic/sprite.h"
#include "../include/app.h"
#include "../include/constant.h" // NBR_BCL_MAX_EST_VIDE
#include "../interface/game_msg.h"
#include "../map/camera.h"
#include "../map/map.h"
#include "../network/randomsync.h"
#include "../object/objects_list.h"
#include "../team/macro.h"
#include "../tool/debug.h"
#include "../tool/i18n.h"
#include "../tool/resource_manager.h"

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
const uint BONUS_TELEPORTATION=5;
const uint BONUS_DYNAMITE=10;
const uint BONUS_AIR_ATTACK=1;
const uint BONUS_AUTO_BAZOOKA=5;

BonusBox::BonusBox()
  : PhysicalObj("bonus_box"){
  SetTestRect (29, 29, 63, 6);
  m_allow_negative_y = true;
  enable = false;

  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);
  anim = resource_manager.LoadSprite( res, "objet/caisse");
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
  // Si un ver touche la caisse, on la réinitialise
  FOR_ALL_LIVING_CHARACTERS(equipe, ver)
  {
    if( ObjTouche(*ver) )
    {
      // Offre des dynamites
      ApplyBonus (**equipe, *ver);

      lst_objects.RemoveObject(this);

      return;
    }
  }

  // Refresh animation
  if (!m_ready && !parachute) anim->Update();
  
  m_ready = anim->IsFinished();
}

// Signale la fin d'une chute
void BonusBox::SignalFallEnding()
{
  SetAirResistFactor(1.0);

  MSG_DEBUG("bonus", "Fin de la chute: parachute=%d", parachute);
  if (!parachute) return;

  MSG_DEBUG("bonus", "Début de l'animation 'repli du parachute'.");
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
    equipe.m_nb_ammos[ _("Teleportation") ] += BONUS_TELEPORTATION;
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
                "'%s has won %u air attack",
                "'%s has won %u air attacks",
                BONUS_AIR_ATTACK),
            ActiveTeam().GetName().c_str(), BONUS_AIR_ATTACK);
    equipe.m_nb_ammos[ _("Air attack") ] += BONUS_AIR_ATTACK;
    break;

  case bonusAUTO_BAZOOKA:
    txt << Format(ngettext(
                "%s team has won %u automatic bazooka!",
                "%s team has won %u automatic bazookas!",
                BONUS_AUTO_BAZOOKA),
		  ActiveTeam().GetName().c_str(), BONUS_AUTO_BAZOOKA);
    equipe.m_nb_ammos[ _("Automatic bazooka") ] += BONUS_AUTO_BAZOOKA;
    break;

  default: std::cout << bonus << std::endl; assert (false);
  case bonusDYNAMITE:  
    txt << Format(ngettext(
                "%s team has won %u stick of dynamite!",
                "%s team has won %u sticks of dynamite!",
                BONUS_DYNAMITE),
            ActiveTeam().GetName().c_str(), BONUS_DYNAMITE);
    equipe.m_nb_ammos[ _("Dynamite") ] += BONUS_DYNAMITE; 
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
    GameMessages::GetInstance()->Add (_("Is it a gift ?"));
    return true;
  }
 
  return false;
}
