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

#ifndef ABSTRACT_SPIN_BUTTON_H
#define ABSTRACT_SPIN_BUTTON_H

#include "gui/widget.h"
#include "tool/math_tools.h"

class AbstractSpinButton : public Widget
{
  private:
    int m_value;
    int m_step;
    int m_min_value;
    int m_max_value;

  protected:
    int GetStep() const { return m_step; }
    int GetMinValue() const { return m_min_value; }
    int GetMaxValue() const { return m_max_value; }

  public:
    AbstractSpinButton(int value, 
                       int step, 
                       int min_value, 
                       int max_value);
    AbstractSpinButton(Profile * profile,
                       const xmlNode * spinButtonNode);
    virtual ~AbstractSpinButton(void) {}

    virtual bool LoadXMLConfiguration(void);
    int GetValue() const { return m_value; }
    int IncValue();
    int DecValue();
    void SetValue(int value, 
                  bool force = false);
    virtual void ValueHasChanged() = 0;
    void SetMaxValue(int max_value);
    void SetMinValue(int min_value);
    void SetStep(int step) { m_step = step; }
};

#endif

