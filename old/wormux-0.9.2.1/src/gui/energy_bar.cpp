/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Wormux Team.
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
#include <algorithm>
#include "gui/energy_bar.h"
#include "tool/resource_manager.h"
#include "gui/progress_bar.h"

static const int energy_step[EnergyBar::NB_OF_ENERGY_COLOR] = { 16, 33, 50, 67, 84, 100 };

EnergyBar::EnergyBar(uint _x,
                     uint _y,
                     uint _width,
                     uint _height,
                     long _value,
                     long minValue,
                     long maxValue,
                     enum orientation _orientation) : 
  ProgressBar(_x,
              _y,
              _width,
              _height,
              _value,
              minValue,
              maxValue,
              _orientation),
  profile(NULL),
  widgetNode(NULL),
  listThresholds()
{
  Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml", false);

  for(int i = 0; i < NB_OF_ENERGY_COLOR ;i++) {
    std::ostringstream color_name;
    color_name << "energy_bar/energy_color_" << energy_step[i] << "_percent";
    Color colors_value = GetResourceManager().LoadColor(res, color_name.str());

    ProcessThresholds(i, energy_step[i], colors_value);
  }
  GetResourceManager().UnLoadXMLProfile(res);
  SortThresholds();
}

EnergyBar::EnergyBar(Profile * _profile,
                     const xmlNode * _widgetNode) :
  profile(_profile),
  widgetNode(_widgetNode),
  listThresholds()
{
}

bool EnergyBar::LoadXMLConfiguration()
{
  if (NULL == profile || NULL == widgetNode) {
    return false;
  }
  XmlReader * xmlFile = profile->GetXMLDocument();

  unsigned int thresholdCount = xmlFile->GetNbChildren(widgetNode);
  const xmlNode * thresholdNode = xmlFile->GetFirstChild(widgetNode);
  uint i = 0;
  Double thresholdValue;

  for ( ; thresholdCount > 0; --thresholdCount) {

    if ("threshold" == xmlFile->GetNodeName(thresholdNode)) {
      if (xmlFile->ReadPercentageAttr(thresholdNode, "value", thresholdValue)) {
        Color thresholdColor(0, 0, 0, 255);
        xmlFile->ReadHexColorAttr(thresholdNode, "color", thresholdColor);
        ProcessThresholds(i++, thresholdValue, thresholdColor);
      } else {
        // Malformed threshold value !
      }
    }
    thresholdNode = xmlFile->GetNextSibling(thresholdNode);
  } 
  return true;
}

void EnergyBar::ProcessThresholds(int thresholdNumber,
                                  Double thresholdMax,
                                  Color & colorMax)
{
  if (1 > thresholdNumber || NB_OF_ENERGY_COLOR < thresholdNumber) {
    if (0 == thresholdNumber) {
      Threshold first;
      first.value = 0.0;
      first.color = colorMax;
      first.redCoef = 0.0;
      first.greenCoef = 0.0;
      first.blueCoef = 0.0;
      first.alphaCoef = 0.0;
      listThresholds.push_back(first);
    }
    return;
  }
  
  Color colorMin = listThresholds[thresholdNumber - 1].color;
  Double thresholdMin = listThresholds[thresholdNumber - 1].value;
  uint size = orientation == PROG_BAR_HORIZONTAL ? width : height;
  Double one_hunderd = 100;
  Double range = size * (thresholdMax - thresholdMin) / one_hunderd;

  Threshold newThreshold; 
  newThreshold.value = thresholdMax; 
  newThreshold.color = colorMax;
  newThreshold.redCoef   = (colorMax.GetRed()   - colorMin.GetRed())   / range;
  newThreshold.greenCoef = (colorMax.GetGreen() - colorMin.GetGreen()) / range;
  newThreshold.blueCoef  = (colorMax.GetBlue()  - colorMin.GetBlue())  / range;
  newThreshold.alphaCoef = (colorMax.GetAlpha() - colorMin.GetAlpha()) / range;
  listThresholds.push_back(newThreshold);
}

void EnergyBar::SortThresholds()
{
  if (2 > listThresholds.size()) {
    return;
  }
  sort(listThresholds.begin(), listThresholds.end());
}

void EnergyBar::Actu(long real_energy)
{
  val = ComputeValue(real_energy);
  val_barre = ComputeBarValue(val);
  Double one_hunderd = 100;
  Double currentPercentage = abs(val) / (Double)max * one_hunderd;
  Threshold thresholdMin;
  Threshold thresholdMax;
  int i = 0;
  int nbThresholds = listThresholds.size();

  while ((i < nbThresholds) &&
         (currentPercentage > listThresholds[i].value)) {
    ++i;
  }

  ASSERT(i < nbThresholds);

  if (i >= nbThresholds) {
    return;
  }

  if (i > 0) {
    thresholdMin = listThresholds[i - 1];
  } else {
    thresholdMin = listThresholds[0];
  }
  thresholdMax = listThresholds[i];

  Color colorMin = thresholdMin.color;
  uint coefVal = ComputeBarValue(abs(real_energy)) - 
                 ComputeBarValue((Double)max * (Double)thresholdMin.value / one_hunderd);

  value_color.SetColor((int) (colorMin.GetRed()   + (thresholdMax.redCoef   * coefVal)),
                       (int) (colorMin.GetGreen() + (thresholdMax.greenCoef * coefVal)),
                       (int) (colorMin.GetBlue()  + (thresholdMax.blueCoef  * coefVal)),
                       (int) (colorMin.GetAlpha() + (thresholdMax.alphaCoef * coefVal)));
}

