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

#include "../object/bonus_box.h"
//-----------------------------------------------------------------------------
#include "../include/constant.h" // NBR_BCL_MAX_EST_VIDE
#include "../map/map.h"
#include "../team/macro.h"
#include "../game/time.h"
#include "../interface/game_msg.h"
#include "../tool/i18n.h"
#include "../tool/random.h"
#ifdef CL
#include "../tool/geometry_tools.h"
#else
#include "../tool/resource_manager.h"
#include "../graphic/sprite.h"
#include "../include/app.h"
#endif
#include <sstream>
#include <iostream>
using namespace Wormux;
using namespace std;
//-----------------------------------------------------------------------------

#ifdef DEBUG

#define APPARAIT_VITE
//#define DEBUG_CADRE_TEST
//#define MSG_DBG_CAISSE
//#define DBG_PLACEMENT

#define COUT_DBG0 cout << "[caisse]"
#define COUT_DBG COUT_DBG0 " "
#define COUT_PLACEMENT COUT_DBG0 "[placement "<<bcl<<"] "
#endif

// Taille du sprite de la caisse
const double MASSE_CAISSE = 150;

// Combien de temps avant qu'on fabrique une caisse ?
#ifdef APPARAIT_VITE
  const uint TEMPS_MIN_CREATION = 1; // secondes
  const uint TEMPS_MAX_CREATION = 3; // secondes
#else
  const uint TEMPS_MIN_CREATION = 10; // secondes
  const uint TEMPS_MAX_CREATION = 5*60; // secondes
#endif

const uint VITESSE_CAISSE = 5; // meter / seconde
const uint VITESSE_REPLI_PARACHUTE = 170; // ms par image
const uint NBR_ESSAI_PLACEMENT_MAX = 20;

// Bonus offert par la caisse
const uint BONUS_ENERGIE=100;
const uint BONUS_PIEGE=75;
const uint BONUS_TELEPORTE=5;
const uint BONUS_DYNAMITE=10;
const uint BONUS_AERIENNE=1;
const uint BONUS_BAZ_TETE_C=5;

//-----------------------------------------------------------------------------
Caisse caisse;
//-----------------------------------------------------------------------------

Caisse::Caisse()
  : PhysicalObj("caisse", 0.0)
{
  SetTestRect (29, 29, 63, 6);
  m_allow_negative_y = true;
  desactive = true;
  pos_valide = false;
  m_wind_factor = 0.0;
}

//-----------------------------------------------------------------------------

void Caisse::Init()
{
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);
  anim = resource_manager.LoadSprite( res, "objet/caisse");
  SetSize (anim->GetWidth(), anim->GetHeight());
}

//-----------------------------------------------------------------------------

void Caisse::FreeMem()
{
  delete anim;
}

//-----------------------------------------------------------------------------

void Caisse::Reset()
{
  if (desactive) return;

  pos_valide = false;
  uint bcl=0;
  bool ok;
#ifdef DBG_PLACEMENT
  COUT_PLACEMENT << "Cherche une place ..." << std::endl;
#endif
  do
  {
    ok = true;
    Ready();
    if (NBR_ESSAI_PLACEMENT_MAX <= ++bcl) 
    {
#ifdef DBG_PLACEMENT
      COUT_PLACEMENT << "Impossible de trouver une position initiale." << std::endl;
#endif
      return;
    }

    // Placement au hasard en X
    int x = RandomLong(0, world.GetWidth() - GetWidth());
    int y = -GetHeight()+1;
    SetXY (x, y);
#ifdef DBG_PLACEMENT
    COUT_PLACEMENT << "Test en " << x <<"," << y << std::endl;
#endif

    // Vérifie que la caisse est dans le vide
    ok = !IsGhost() && IsInVacuum(0,0) && IsInVacuum(0,1);
    if (!ok) 
    {
#ifdef DBG_PLACEMENT
      COUT_PLACEMENT << "Placement dans un mur" << std::endl;
#endif
      continue;
    }

    // Vérifie que la caisse ne tombe pas dans le vide
    DirectFall();
    ok &= !IsGhost();
    if (!ok)
    {
      continue;
#ifdef DBG_PLACEMENT
      COUT_PLACEMENT << "Placement dans le vide" << std::endl;
#endif
    }

    // Vérifie que le caisse ne touche aucun ver au début
    POUR_TOUS_VERS_VIVANTS(equipe, ver)
    {
      if (ObjTouche(*this, *ver)) 
      {
#ifdef DBG_PLACEMENT
	COUT_PLACEMENT << "La caisse touche le ver " << (*ver).m_name << std::endl;
#endif
	ok = false;
      }
    }
    if (ok) SetXY (x,y);
  } while (!ok);
#ifdef DBG_PLACEMENT
  COUT_PLACEMENT << "Placée après " << bcl << " essai(s)" << std::endl;
#endif
  pos_valide = true;

  affiche = false;
  Ready();
  parachute = true;
#ifdef CL
  anim.set_play_loop(false);
  anim.set_frame(0);
#else
   anim->SetCurrentFrame(0);
   anim->Start();
#endif

  temps_caisse = RandomLong(TEMPS_MIN_CREATION, 
			    TEMPS_MAX_CREATION-TEMPS_MIN_CREATION);
  temps_caisse *= 1000;
  temps_caisse += global_time.Read();
  bonus = RandomLong (1, nbr_bonus_diff);

  SetMass (0);
  Ghost();
}

//-----------------------------------------------------------------------------

void Caisse::Draw()
{ 
  if (!affiche) return;
#ifdef CL
  anim.draw (GetX(), GetY());
#if defined(DEBUG_CADRE_TEST)
  CL_Rect rect=LitRectTest();
  CL_Display::draw_rect (rect, CL_Color::red);
#endif
#else
  anim->Draw(GetX(), GetY());
#endif
}

//-----------------------------------------------------------------------------

// Signale la fin d'une chute
void Caisse::SignalFallEnding()
{
#ifdef MSG_DBG_CAISSE
  COUT_DBG << "Fin de la chute : parachute=" << parachute << std::endl;
#endif
  if (!parachute) return;

#ifdef MSG_DBG_CAISSE
  COUT_DBG << "Début de l'animation 'repli du parachute'." << std::endl;
#endif
  parachute = false;
#ifdef CL
  anim.restart();
#else
  anim->SetCurrentFrame(0);
  anim->Start();
#endif
  SetMass (MASSE_CAISSE);
  UpdatePosition();
}

//-----------------------------------------------------------------------------

void Caisse::AppliqueBonus (Team &equipe, Character &ver)
{
  std::ostringstream txt;

  switch (bonus)
  {
  case bonusTELEPORTE: 
    txt << Format(ngettext(
                "%s team has won %u teleportation.", 
                "%s team has won %u teleportations.", 
                BONUS_TELEPORTE),
            ActiveTeam().GetName().c_str(), BONUS_TELEPORTE);
    equipe.m_nb_ammos[ _("Teleportation") ] += BONUS_TELEPORTE;
    break;

  case bonusENERGIE:
    txt << Format(ngettext(
                "%s has won %u point of energy!",
                "%s has won %u points of energy!",
                BONUS_ENERGIE),
            ver.m_name.c_str(), BONUS_ENERGIE);
    ver.SetEnergyDelta (BONUS_ENERGIE);
    break;

  case bonusPIEGE:
    txt << Format(ngettext(
                "%s has lost %u point of energy.",
                "%s has lost %u points of energy.",
                BONUS_PIEGE),
            ver.m_name.c_str(), BONUS_PIEGE);
    ver.SetEnergyDelta (-BONUS_PIEGE);
    break;

  case bonusAERIENNE:
    txt << Format(ngettext(
                "'%s has won %u air attack",
                "'%s has won %u air attacks",
                BONUS_AERIENNE),
            ActiveTeam().GetName().c_str(), BONUS_AERIENNE);
    equipe.m_nb_ammos[ _("Air attack") ] += BONUS_AERIENNE;
    break;

  case bonusBAZ_TETE_C:
    txt << Format(ngettext(
                "%s team has won %u automatic bazooka!",
                "%s team has won %u automatic bazookas!",
                BONUS_BAZ_TETE_C),
		  ActiveTeam().GetName().c_str(), BONUS_BAZ_TETE_C);
    equipe.m_nb_ammos[ _("Automatic bazooka") ] += BONUS_BAZ_TETE_C;
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

  game_messages.Add (txt.str());
}

//-----------------------------------------------------------------------------

bool Caisse::FaitApparaitre()
{
  if (desactive) return false;

  if (!pos_valide) Reset();

  if (affiche || (global_time.Read() < temps_caisse)) return false;
  affiche = true;
  m_ready = false;
  Ready();
  SetSpeed (VITESSE_CAISSE, M_PI_2);
  return true;
}

//-----------------------------------------------------------------------------

void Caisse::Refresh()
{
  if (!affiche) return;

  // Si un ver touche la caisse, on la réinitialise
  POUR_TOUS_VERS_VIVANTS(equipe, ver)
  {
    if (ObjTouche(*this, *ver))
    {
      // Offre des dynamites
      AppliqueBonus (**equipe, *ver);

      // Creation d'un nouvelle caisse
      Reset();
      return;
    }
  }

  // Refresh de l'animation
#ifdef CL
   if (!parachute) anim.update();
#else
   if (!parachute) anim->Update();
#endif
   
#ifdef CL
   m_ready = anim.is_finished();
#else
   m_ready = anim->IsFinished();
#endif
}

//-----------------------------------------------------------------------------

void Caisse::SignalGhostState (bool)
{
  if (!affiche) return;
#ifdef MSG_DBG_CAISSE
  COUT_DBG << "Une caisse sort de l'écran !" << std::endl;
#endif
  Reset();
}

//-----------------------------------------------------------------------------

// Active les caisses ?
void Caisse::Active (bool actif)
{
  desactive = !actif;
  
  if (desactive) affiche = false;
}

//-----------------------------------------------------------------------------
