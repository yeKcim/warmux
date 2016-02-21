/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
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
 * EnergyBar.
 *****************************************************************************/

#ifndef ENERGY_BAR_H
#define ENERGY_BAR_H

#include "gui/progress_bar.h"
#include "tool/resource_manager.h"
#include <vector>

class EnergyBar : public ProgressBar
{
public:
  class Threshold
  {
  public:
    float value;
    Color color;
    float redCoef;
    float greenCoef;
    float blueCoef;

    bool operator < (const Threshold & threshold) const {
      return value < threshold.value;
    }
  };
  static const uint NB_OF_ENERGY_COLOR = 6;

private:
  Profile * profile;
  const xmlNode * widgetNode;
  std::vector<Threshold> listThresholds;

public:
  EnergyBar(uint _x,
            uint _y,
            uint _width,
            uint _height,
            int _value = 0,
            int minValue = 0,
            int maxValue = 100,
            enum orientation _orientation = PROG_BAR_HORIZONTAL);
  EnergyBar(Profile * _profile,
            const xmlNode * _widgetNode);
  virtual ~EnergyBar(void) {}

  virtual bool LoadXMLConfiguration(void);
  void ProcessThresholds(uint thresholdNumber,
                         float thresholdMax,
                         Color & colorMax);
  void SortThresholds();
  void Actu(int val);
};

#endif /* ENERGY_BAR_H */
