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
 * Energy (progress) bar of a team
 *****************************************************************************/

#ifndef TEAM_ENERGY_H
#define TEAM_ENERGY_H

#include "../graphic/text.h"
#include "../gui/progress_bar.h"
#include "../object/physical_obj.h"

typedef enum {
  //Les jauges sont prêtes pour une nouvelle opération
  EnergyStatusOK,

  //Les jauges peuvent changer leur valeur
  EnergyStatusValueChange,

  //Les jauges peuvent changer leur classement
  EnergyStatusClassementChange,

  //LA jauge attend que toutes les jauges aient fini leur opération en cour
  EnergyStatusWait
} energy_t;

class TeamEnergy
{
  private :
    BarreProg barre_energie;
    Text* bar_text;
    uint valeur; //Valeur affichée
    uint nv_valeur; //Vrai valeur
    uint valeur_max; //Valeur initiale (en début de partie)

    int dx;
    int dy;

    uint classement; //0 = première position au classement
    uint nv_classement;

    std::string team_name;

    uint tps_debut_mvt;

  public :
    uint classement_tmp;
    energy_t status;

    TeamEnergy(const std::string& _team_name);
    ~TeamEnergy();
    void Config(uint _current_energy,
		uint _max_energy);

    void Refresh();
    void Draw();

    void SetValue(uint nv_energie); //(avec animation)

    void FixeClassement(uint classem); //(sans animation)
    void NouveauClassement(uint nv_classem); //(avec animation)

    void Move(); //Déplacement des jauges (changement dans le classement)
    bool IsMoving() const; //Déplacement des jauges (changement dans le classement)

    void SetTeamName(const std::string& _team_name);
};

#endif 
