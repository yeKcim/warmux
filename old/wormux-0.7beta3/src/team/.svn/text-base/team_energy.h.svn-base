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

#ifndef ENERGIE_EQUIPE_H
#define ENERGIE_EQUIPE_H
 
#include "../graphic/text.h"
#include "../gui/progress_bar.h"
#include "../object/physical_obj.h"

//Les jauges sont prêtes pour une nouvelle opération
const uint EnergieStatusOK = 0;

//Les jauges peuvent changer leur valeur
const uint EnergieStatusValeurChange = 1;

//Les jauges peuvent changer leur classement
const uint EnergieStatusClassementChange = 2;

//LA jauge attend que toutes les jauges aient fini leur opération en cour
const uint EnergieStatusAttend = 3;


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
    
    std::string nom;

    uint tps_debut_mvt;
  
  public :
    uint classement_tmp;
    uint status;
    
    TeamEnergy();
    void Init();
    void ChoisitNom(const std::string &nom_equipe);
    void Refresh();
    void Draw();
    void Reset();

    void FixeValeur(uint energie); //(sans animation)
    void FixeMax(uint energie); //Choisit la valeur maximale de la jauge
    void NouvelleValeur(uint nv_energie); //(avec animation)
    
    void FixeClassement(uint classem); //(sans animation)
    void NouveauClassement(uint nv_classem); //(avec animation)

    void Mouvement(); //Déplacement des jauges (changement dans le classement)
    bool EstEnMouvement(); //Déplacement des jauges (changement dans le classement)
};

#endif //ENERGIE_EQUIPE_H
