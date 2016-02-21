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
 * Barre d'énergie de chaque équipe
 *****************************************************************************/

#include "team_energy.h"
//-----------------------------------------------------------------------------
#include <sstream>
#include <math.h>
#include "../map/camera.h"
#include "../game/time.h"
#include "../graphic/text.h"

using namespace std;
using namespace Wormux;

const uint BARRE_LARG = 140;
const uint BARRE_HAUT = 13;
const uint ESPACEMENT = 3;

const uchar ALPHA = 127;
const uchar ALPHA_FOND = 0;

const uchar R_INIT = 0; //Couleur R à 100%
const uchar V_INIT = 255; //Couleur V à 100%
const uchar B_INIT = 0; //Couleur B à 100%

const uchar R_INTER = 255; //Couleur R à 50%
const uchar V_INTER = 255; //Couleur V à 50%
const uchar B_INTER = 0; //Couleur B à 50%

const uchar R_FINAL = 255; //Couleur R à 0%
const uchar V_FINAL = 0; //Couleur V à 0%
const uchar B_FINAL = 0; //Couleur B à 0%


const float DUREE_MVT = 750.0;

//-----------------------------------------------------------------------------

TeamEnergy :: TeamEnergy()
{
  bar_text = NULL;
}

//-----------------------------------------------------------------------------

void TeamEnergy :: Init ()
{
  dx = 0;
  dy = 0;
  tps_debut_mvt = 0;
  valeur_max = 0;
  status = EnergieStatusOK;
  barre_energie.InitPos (0,0, BARRE_LARG, BARRE_HAUT);

  barre_energie.SetValueColor(R_INIT, V_INIT, B_INIT, ALPHA);
  barre_energie.SetBorderColor(255, 255, 255, ALPHA);
  barre_energie.SetBackgroundColor(255*6/10, 255*6/10, 255*6/10, ALPHA_FOND);

  if(bar_text == NULL)
    bar_text = new Text("");
}

//-----------------------------------------------------------------------------

void TeamEnergy :: ChoisitNom (const string &nom_equipe)
{ 
  nom = nom_equipe; 
}

//-----------------------------------------------------------------------------

void TeamEnergy :: Refresh ()
{
  switch(status)
  {
  //La valeur de l'énergie d'une des équipe change
  case EnergieStatusValeurChange:
    if(nv_valeur > valeur)
      valeur = nv_valeur;
    if(valeur > nv_valeur)
      --valeur;
    if(valeur == nv_valeur)
      status = EnergieStatusAttend;
    break;

  //Le classement se modifie
  case EnergieStatusClassementChange:
    Mouvement();
    break;

  //Aucun changement ne s'effectue en ce moment
  case EnergieStatusOK:
    if( valeur != nv_valeur && !EstEnMouvement())
      status = EnergieStatusValeurChange;
    else
    if( classement != nv_classement )
      status = EnergieStatusClassementChange;
    break;

  //Cette barre d'énergie n'a plus rien à faire
  //Elle attend une synchronisation avec les autres barres
  case EnergieStatusAttend:
    break;
  }
}

//-----------------------------------------------------------------------------

void TeamEnergy :: Draw ()
{
  barre_energie.Actu(valeur);

  float r,v,b;
  if( valeur < (valeur_max / 2) )
  {
    r = ( 2.0 * ((R_FINAL * ((valeur_max / 2) - valeur)) + (valeur * R_INTER))) / valeur_max;
    v = ( 2.0 * ((V_FINAL * ((valeur_max / 2) - valeur)) + (valeur * V_INTER))) / valeur_max;
    b = ( 2.0 * ((B_FINAL * ((valeur_max / 2) - valeur)) + (valeur * B_INTER))) / valeur_max;
  }
  else
  {
    r = ( 2.0 * ((R_INIT * (valeur - (valeur_max / 2))) + (R_INTER * (valeur_max - valeur)))) / valeur_max;
    v = ( 2.0 * ((V_INIT * (valeur - (valeur_max / 2))) + (V_INTER * (valeur_max - valeur)))) / valeur_max;
    b = ( 2.0 * ((B_INIT * (valeur - (valeur_max / 2))) + (B_INTER * (valeur_max - valeur)))) / valeur_max;
  }

  barre_energie.SetValueColor( (unsigned char)r, (unsigned char)v, (unsigned char)b, ALPHA);
   
  int x,y;
  x = camera.GetWidth() - (BARRE_LARG + 10) + dx;
  y = BARRE_HAUT +(classement * (BARRE_HAUT + ESPACEMENT)) +dy;
  barre_energie.DrawXY(x,y);
  
  std::ostringstream ss;
  ss << nom << "/" << valeur;
  x = camera.GetWidth() - ((BARRE_LARG/2) + 10) + dx;
  y = BARRE_HAUT + (classement * (BARRE_HAUT + ESPACEMENT)) + dy;
  std::string txt = ss.str();
  bar_text->Set(txt);
  bar_text->DrawCenterTop(x,y);
}

//-----------------------------------------------------------------------------

void TeamEnergy :: Reset ()
{
}

//-----------------------------------------------------------------------------

void TeamEnergy::FixeMax (uint energie)
{ valeur_max = energie; }

//-----------------------------------------------------------------------------

void TeamEnergy::FixeValeur (uint energie)
{
  valeur = energie;
  nv_valeur = energie;
  assert(valeur_max != 0)
  barre_energie.InitVal (energie, 0, valeur_max);
}

//-----------------------------------------------------------------------------

void TeamEnergy::NouvelleValeur (uint nv_energie)
{ nv_valeur = nv_energie; }

void TeamEnergy::FixeClassement (uint classem)
{
  classement = classem;
  nv_classement = classem;
}

void TeamEnergy::NouveauClassement (uint nv_classem)
{ nv_classement = nv_classem; }

//-----------------------------------------------------------------------------

void TeamEnergy::Mouvement ()
{
  //Déplacement des jauges (changement dans le classement)
  if( valeur != nv_valeur && !EstEnMouvement())
  {
    //D'autres jauges sont en train de changer de classement
    //Celle-là ne doit pas changer de classement tant que sa
    //valeur d'énergie n'a pas été actualisée à l'écran
    status = EnergieStatusAttend;
    return;
  }
  
  if( classement == nv_classement && !EstEnMouvement())
  {
    //D'autres jauges sont en train de changer de classement
    status = EnergieStatusAttend;
    return;
  }

  //Le classement de cette jauge a changé
  if( classement != nv_classement )
  {
    if(tps_debut_mvt == 0)
      tps_debut_mvt = global_time.Read();
    
    dy = (int)(( (BARRE_HAUT+ESPACEMENT) * ((float)nv_classement - classement))
             * ((global_time.Read() - tps_debut_mvt) / DUREE_MVT));
    
    // Déplacement en arc de cercle seulement quand la jauge descend 
    // dans le classement
    if( nv_classement > classement )
      dx = (int)(( 3.0 * (BARRE_HAUT+ESPACEMENT) * ((float)classement - nv_classement))
             * sin( M_PI * ((global_time.Read() - tps_debut_mvt) /DUREE_MVT)));
    
    //Mouvement terminé?
    if( (global_time.Read() - tps_debut_mvt) > DUREE_MVT )
    {
      dy = 0;
      dx = 0;
      classement = nv_classement;
      tps_debut_mvt = 0;
      status = EnergieStatusAttend;
      return;
    }
  }
  else
  {//Pendant que la jauge bougeait, elle est revenue
  //à sa place d'origine dans le classement
    dy = (int)((float)dy - ((global_time.Read() - tps_debut_mvt) /DUREE_MVT) * dy);
    dx = (int)((float)dx - ((global_time.Read() - tps_debut_mvt) /DUREE_MVT) * dx);
  }
}

//-----------------------------------------------------------------------------

bool TeamEnergy::EstEnMouvement ()
{
  if( dx != 0 || dy != 0 )
    return true;
  return false;
}

//-----------------------------------------------------------------------------
