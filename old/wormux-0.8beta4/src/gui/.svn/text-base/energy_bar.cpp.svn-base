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
 *Energy bar.
 *****************************************************************************/

#include <iostream>
#include <sstream>
#include "gui/energy_bar.h"
#include "tool/resource_manager.h"
#include "gui/progress_bar.h"

static const int energy_step[EnergyBar::NB_OF_ENERGY_COLOR] = { 16, 33, 50, 67, 84, 100 };

EnergyBar::EnergyBar() : ProgressBar()
{
  Profile *res = resource_manager.LoadXMLProfile("graphism.xml", false);
  for(int i = 0; i < NB_OF_ENERGY_COLOR ;i++) {
    std::ostringstream color_name;
    color_name << "energy_bar/energy_color_" << energy_step[i] << "_percent";
    colors_value[i] = resource_manager.LoadColor(res, color_name.str());
  }
  resource_manager.UnLoadXMLProfile(res);
}

Color EnergyBar::GetColorValue(long app_energy) const
{
  for(int i = 0; i < NB_OF_ENERGY_COLOR - 1; ++i) {
    if(energy_step[i] > ((float)app_energy / GetMaxVal()) * 100)
      return colors_value[i];
  }
  return colors_value[NB_OF_ENERGY_COLOR - 1];
}

void EnergyBar::Actu (long real_energy){
  long app_energy;

  /* update progress bar position*/
  ProgressBar::UpdateValue(real_energy);

  /* get the real applied enargie value. It may be different from the
   * real_energy in case of under/over flow*/
  app_energy = ProgressBar::GetVal();

  SetValueColor(GetColorValue(app_energy));
}
