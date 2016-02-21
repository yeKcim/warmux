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
 * Barre d'énergie de chaque équipe
 *****************************************************************************/

#include "team_energy.h"
#include <sstream>
#include <math.h>
#include "../map/camera.h"
#include "../game/time.h"
#include "../graphic/text.h"

const uint BARRE_LARG = 200;
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

TeamEnergy :: TeamEnergy(const std::string &_team_name)
{
  dx = 0;
  dy = 0;
  tps_debut_mvt = 0;
  valeur_max = 0;
  status = EnergyStatusOK;
  barre_energie.InitPos (0,0, BARRE_LARG, BARRE_HAUT);

  barre_energie.SetValueColor( Color(R_INIT, V_INIT, B_INIT, ALPHA) );
  barre_energie.SetBorderColor( Color(255, 255, 255, ALPHA) );
  barre_energie.SetBackgroundColor( Color(255*6/10, 255*6/10, 255*6/10, ALPHA_FOND) );

  bar_text = new Text("");
  team_name = _team_name;
}

TeamEnergy :: ~TeamEnergy ()
{
  delete bar_text;
}

void TeamEnergy::Config(uint _current_energy,
			uint _max_energy)
{
  valeur_max = _max_energy;

  valeur = _current_energy;
  nv_valeur = _current_energy;
  assert(valeur_max != 0)
  barre_energie.InitVal (valeur, 0, valeur_max);
}

void TeamEnergy :: Refresh ()
{
  switch(status)
  {
  //La valeur de l'énergie d'une des équipe change
  case EnergyStatusValueChange:
    if(nv_valeur > valeur)
      valeur = nv_valeur;
    if(valeur > nv_valeur)
      --valeur;
    if(valeur == nv_valeur)
      status = EnergyStatusWait;
    break;

  //Le classement se modifie
  case EnergyStatusClassementChange:
    Move();
    break;

  //Aucun changement ne s'effectue en ce moment
  case EnergyStatusOK:
    if( valeur != nv_valeur && !IsMoving())
      status = EnergyStatusValueChange;
    else
    if( classement != nv_classement )
      status = EnergyStatusClassementChange;
    break;

  //Cette barre d'énergie n'a plus rien à faire
  //Elle attend une synchronisation avec les autres barres
  case EnergyStatusWait:
    break;
  }
}

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

  Color color( (unsigned char)r, (unsigned char)v, (unsigned char)b, ALPHA);

  barre_energie.SetValueColor( color );

  int x,y;
  x = camera.GetSizeX() - (BARRE_LARG + 10) + dx;
  y = BARRE_HAUT +(classement * (BARRE_HAUT + ESPACEMENT)) +dy;
  barre_energie.DrawXY( Point2i(x, y) );

  std::ostringstream ss;
  ss << team_name << "/" << valeur;
  x = camera.GetSizeX() - ((BARRE_LARG/2) + 10) + dx;
  y = BARRE_HAUT + (classement * (BARRE_HAUT + ESPACEMENT)) + dy;
  std::string txt = ss.str();
  bar_text->Set(txt);
  bar_text->DrawCenterTop(x,y);
}

void TeamEnergy::SetValue (uint nv_energie)
{ nv_valeur = nv_energie; }

void TeamEnergy::FixeClassement (uint classem)
{
  classement = classem;
  nv_classement = classem;
}

void TeamEnergy::NouveauClassement (uint nv_classem)
{ nv_classement = nv_classem; }

void TeamEnergy::Move ()
{
  //Déplacement des jauges (changement dans le classement)
  if( valeur != nv_valeur && !IsMoving())
  {
    //D'autres jauges sont en train de changer de classement
    //Celle-là ne doit pas changer de classement tant que sa
    //valeur d'énergie n'a pas été actualisée à l'écran
    status = EnergyStatusWait;
    return;
  }

  if( classement == nv_classement && !IsMoving())
  {
    //D'autres jauges sont en train de changer de classement
    status = EnergyStatusWait;
    return;
  }

  //Le classement de cette jauge a changé
  Time * global_time = Time::GetInstance();
  if( classement != nv_classement )
  {
    if(tps_debut_mvt == 0)
      tps_debut_mvt = global_time->Read();

    dy = (int)(( (BARRE_HAUT+ESPACEMENT) * ((float)nv_classement - classement))
             * ((global_time->Read() - tps_debut_mvt) / DUREE_MVT));

    // Déplacement en arc de cercle seulement quand la jauge descend
    // dans le classement
    if( nv_classement > classement )
      dx = (int)(( 3.0 * (BARRE_HAUT+ESPACEMENT) * ((float)classement - nv_classement))
             * sin( M_PI * ((global_time->Read() - tps_debut_mvt) /DUREE_MVT)));

    //Mouvement terminé?
    if( (global_time->Read() - tps_debut_mvt) > DUREE_MVT )
    {
      dy = 0;
      dx = 0;
      classement = nv_classement;
      tps_debut_mvt = 0;
      status = EnergyStatusWait;
      return;
    }
  }
  else
  {//Pendant que la jauge bougeait, elle est revenue
  //à sa place d'origine dans le classement
    dy = (int)((float)dy - ((global_time->Read() - tps_debut_mvt) /DUREE_MVT) * dy);
    dx = (int)((float)dx - ((global_time->Read() - tps_debut_mvt) /DUREE_MVT) * dx);
  }
}

bool TeamEnergy::IsMoving () const
{
  if( dx != 0 || dy != 0 )
    return true;
  return false;
}

void TeamEnergy::SetTeamName(const std::string& _team_name)
{
  team_name = _team_name;
}
