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
 * Parachute
 *****************************************************************************/

#ifndef PARACHUTE_H
#define PARACHUTE_H

#include "weapon.h"

class ParachuteConfig : public WeaponConfig
{
  public:
    double wind_factor ;
    double air_resist_factor ;
  public:
    ParachuteConfig();
    void LoadXml(xmlpp::Element *elem);
};

//-----------------------------------------------------------------------------

class Parachute : public Weapon
{
  private:
    bool open ;
    bool closing ;
    Sprite* image;
  protected:
    void p_Select();
    void p_Deselect();
    void Refresh();
    bool p_Shoot();
  public:
    Parachute();
    void Draw() ;
    void SignalTurnEnd();

    ParachuteConfig& cfg();
    DECLARE_GETWEAPONSTRING();
};

#endif /* PARACHUTE_H */
