/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
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

#ifndef ABSTRACT_SPIN_BUTTON_H
#define ABSTRACT_SPIN_BUTTON_H

#include "gui/widget.h"
#include "tool/math_tools.h"

class AbstractSpinButton : public Widget
{
private:
  int m_value, m_step, m_min_value, m_max_value;

protected:
  int GetStep() const { return m_step; }
  int GetMinValue() const { return m_min_value; }
  int GetMaxValue() const { return m_max_value; }

public:
  AbstractSpinButton(int value, int step, int min_value, int max_value) :
    m_value(value), m_step(step), m_min_value(min_value), m_max_value(max_value) 
    {
      ASSERT(m_value >= m_min_value);
      ASSERT(m_max_value >= m_value);
      ASSERT(m_step <= (m_max_value - m_min_value));
    }

  int GetValue() const { return m_value; }

  int IncValue() 
  {
    SetValue(m_value + m_step);
    return m_value;
  }
  
  int DecValue() 
  {
    SetValue(m_value - m_step);
    return m_value;
  }

  void SetValue(int value) 
  {
    int old_value = m_value;
    m_value = InRange_Long(value, m_min_value, m_max_value);
    
    if (old_value != m_value) {
      ValueHasChanged();
      NeedRedrawing();
    }
  }

  virtual void ValueHasChanged() = 0;

  void SetMaxValue(int max_value) 
  {
    if (m_max_value != max_value) {
      m_max_value = max_value;
      SetValue(m_value);
      NeedRedrawing();
    }
    ASSERT(m_min_value < m_max_value);
    ASSERT(m_step <= (m_max_value - m_min_value));
  }

  void SetMinValue(int min_value) 
  {
    if (m_min_value != min_value) {
      m_min_value = min_value;
      SetValue(m_value);
      NeedRedrawing();
    }
    ASSERT(m_min_value < m_max_value);
    ASSERT(m_step <= (m_max_value - m_min_value));
  }

  void SetStep(int step) 
  {
    m_step = step;
  }
};

#endif

