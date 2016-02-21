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
 * Medkit
 *****************************************************************************/

#ifndef MEDKIT_H
#define MEDKIT_H
//-----------------------------------------------------------------------------
#include <SDL.h>
#include "include/base.h"
#include "objbox.h"
#include "object/physical_obj.h"
#include "team/team.h"
//-----------------------------------------------------------------------------

class Medkit : public ObjBox//public PhysicalObj
{
  private:
    //static bool enable;
    static int nbr_health;
    //static int start_life_points;

    //bool parachute; 
    //Sprite *anim;

  private:
    void ApplyMedkit (Team &team, Character &character);
  public:
    Medkit();
    //~Medkit();

    // Activate medkit ?
    //static void Enable (bool _enable);
    //static bool NewMedkit();
    //void DropMedkit();
    static void LoadXml(xmlpp::Element * object);

    void Draw();
    void Refresh();

  protected:
    // Signal Fall ending
    //void SignalCollision();
    //void SignalDrowning();
    //void SignalGhostState(bool was_already_dead);
};

//-----------------------------------------------------------------------------
#endif /* MEDKIT_H */
