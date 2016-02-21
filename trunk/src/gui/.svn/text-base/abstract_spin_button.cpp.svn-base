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
 * Abstract Spinbutton in GUI.
 *****************************************************************************/

#include "gui/abstract_spin_button.h"

AbstractSpinButton::AbstractSpinButton(int value,
                                       int step,
                                       int min_value,
                                       int max_value) :
  m_value(value), 
  m_step(step), 
  m_min_value(min_value), 
  m_max_value(max_value)
{
  ASSERT(m_value >= m_min_value);
  ASSERT(m_max_value >= m_value);
  ASSERT(m_step <= (m_max_value - m_min_value));
}

AbstractSpinButton::AbstractSpinButton(Profile * profile,
                                       const xmlNode * spinButtonNode) :
  Widget(profile, spinButtonNode),
  m_value(0),
  m_step(1),
  m_min_value(0),
  m_max_value(100)
{
}

bool AbstractSpinButton::LoadXMLConfiguration(void)
{
  if (NULL == profile || NULL == widgetNode) {
    return false;
  }

  XmlReader * xmlFile = profile->GetXMLDocument();

  const xmlNode * valueNode = xmlFile->GetFirstNamedChild(widgetNode, "Value");
  xmlFile->ReadIntAttr(valueNode, "initValue", m_value);
  xmlFile->ReadIntAttr(valueNode, "minValue", m_min_value);
  xmlFile->ReadIntAttr(valueNode, "maxValue", m_max_value);
  xmlFile->ReadIntAttr(valueNode, "stepValue", m_step);

  return true;
}

int AbstractSpinButton::IncValue() 
{
  SetValue(m_value + m_step);
  return m_value;
}

int AbstractSpinButton::DecValue()
{
  SetValue(m_value - m_step);
  return m_value;
}

void AbstractSpinButton::SetValue(int value, 
                                  bool force)
{
  int old_value = m_value;

  if (!force) {
    m_value = InRange_Long(value, m_min_value, m_max_value);
  } else {
    m_value = value;
    if (value > m_max_value) {
      m_max_value = value;
    } else if (value < m_min_value) {
      m_min_value = value;
    }
  }

  if (old_value != m_value) {
    ValueHasChanged();
    NeedRedrawing();
  }
}

void AbstractSpinButton::SetMaxValue(int max_value)
{
  if (m_max_value != max_value) {
    m_max_value = max_value;
    SetValue(m_value);
    NeedRedrawing();
  }
  ASSERT(m_min_value < m_max_value);
  ASSERT(m_step <= (m_max_value - m_min_value));
}

void AbstractSpinButton::SetMinValue(int min_value)
{
  if (m_min_value != min_value) {
    m_min_value = min_value;
    SetValue(m_value);
    NeedRedrawing();
  }
  ASSERT(m_min_value < m_max_value);
  ASSERT(m_step <= (m_max_value - m_min_value));
}


