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
 *****************************************************************************/

#include <sstream>

#include "gui/spin_button.h"
#include "gui/button.h"
#include "graphic/text.h"
#include "tool/math_tools.h"
#include "tool/resource_manager.h"

SpinButton::SpinButton (const std::string & _label,
                        int width,
                        int value,
                        int step,
                        int min_value,
                        int max_value,
                        const Color & color,
                        bool _shadowed) :
  AbstractSpinButton(value,
                     step,
                     min_value,
                     max_value),
  shadowed(false),
  txtLabel(NULL),
  txtValue(NULL),
  m_plus(NULL),
  m_minus(NULL)
{
  position = Point2i(-1, -1);
  size.x = width;
  size.y = (*Font::GetInstance(Font::FONT_SMALL)).GetHeight();
  shadowed = _shadowed;

  Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml", false);

  txtLabel = new Label(_label, 100, Font::FONT_SMALL, Font::FONT_NORMAL,
                       color, Text::ALIGN_LEFT_TOP, shadowed);
  txtLabel->SetMaxWidth(size.x - 30);

  txtValue = new Label(" ", 100, Font::FONT_SMALL, Font::FONT_NORMAL,
                       color, Text::ALIGN_LEFT_TOP, shadowed);
  std::ostringstream max_value_s;
  max_value_s << GetMaxValue();
  uint max_value_w = (*Font::GetInstance(Font::FONT_SMALL)).GetWidth(max_value_s.str());

  uint margin = 5;

  m_plus = new Button(res, "menu/plus");
  m_plus->SetPosition(position.x + size.x - 5, position.y);
  m_minus = new Button(res, "menu/minus");
  m_minus->SetPosition(position.x + size.x - max_value_w - 5 - 2 * margin, position.y);

  ValueHasChanged();
}

SpinButton::SpinButton(Profile * profile,
                       const xmlNode * spinButtonNode) :
  AbstractSpinButton(profile, spinButtonNode),
  shadowed(false),
  txtLabel(NULL),
  txtValue(NULL),
  m_plus(NULL),
  m_minus(NULL)
{
}

SpinButton::~SpinButton()
{
  if (NULL != txtLabel) {
    delete txtLabel;
  }
  if (NULL != txtValue) {
    delete txtValue;
  }
  if (NULL != m_plus) {
    delete m_plus;
  }
  if (NULL != m_minus) {
    delete m_minus;
  }
}

bool SpinButton::LoadXMLConfiguration(void)
{
  if (NULL == profile || NULL == widgetNode) {
    return false;
  }

  ParseXMLGeometry();
  ParseXMLBorder();
  ParseXMLBackground();

  XmlReader * xmlFile = profile->GetXMLDocument();

  const xmlNode * labelNode = xmlFile->GetFirstNamedChild(widgetNode, "Label");
  if (NULL != labelNode) {
    txtLabel = new Label(profile, labelNode);
    txtLabel->LoadXMLConfiguration();
  } else {
    txtLabel = new Label("n/a", 100);
  }
  txtLabel->Pack();
  txtLabel->SetPosition(position.x,
                        position.y + (size.y / 2) - (txtLabel->GetSizeY() / 2));

  const xmlNode * valueNode = xmlFile->GetFirstNamedChild(widgetNode, "Value");
  if (NULL != valueNode) {
    txtValue = new Label(profile, valueNode);
    txtValue->LoadXMLConfiguration();
  } else {
    txtValue = new Label("0", 100);
  }
  txtValue->Pack();
  txtValue->SetPosition(position.x + size.x - txtValue->GetWidth(),
                        position.y + (size.y / 2) - (txtValue->GetSizeY() / 2));

  AbstractSpinButton::LoadXMLConfiguration();
  ValueHasChanged();

  const xmlNode * buttonMinusNode = xmlFile->GetFirstNamedChild(widgetNode, "ButtonMinus");
  if (NULL != buttonMinusNode) {
    m_minus = new Button(profile, buttonMinusNode);
    m_minus->LoadXMLConfiguration();
    m_minus->SetPosition(position.x + size.x - m_minus->GetSizeX(),
                         position.y);
  }

  const xmlNode * buttonPlusNode = xmlFile->GetFirstNamedChild(widgetNode, "ButtonPlus");
  if (NULL != buttonPlusNode) {
    m_plus = new Button(profile, buttonPlusNode);
    m_plus->LoadXMLConfiguration();
    m_plus->SetPosition(position.x + size.x - m_plus->GetSizeX(),
                        position.y + size.y - m_plus->GetSizeY());
  }

  return true;
}

void SpinButton::Pack()
{
  std::ostringstream max_value_s;
  max_value_s << GetMaxValue();
  uint max_value_w = (*Font::GetInstance(txtValue->GetFontSize())).GetWidth(max_value_s.str());

  m_plus->SetPosition(position.x + size.x - m_plus->GetSizeX(),
                      position.y + (size.y / 2) - (m_plus->GetSizeY() / 2));
  m_minus->SetPosition(position.x + size.x - (m_plus->GetSizeX() + max_value_w + m_plus->GetSizeX()),
                       position.y + (size.y / 2) - (m_minus->GetSizeY() / 2));

  txtLabel->SetMaxWidth(size.x - 30);
  size.y = txtLabel->GetHeight();
}

void SpinButton::Draw(const Point2i & mousePosition)
{
  txtLabel->DrawCenterTop(position + Point2i(0, 6));

  uint center = m_plus->GetPositionX() - (m_plus->GetPositionX() - (m_minus->GetPositionX() + m_minus->GetSizeX())) / 2;
  txtValue->DrawCenterTop(Point2i(center, position.y)); //CenterTop

  if (GetValue() != GetMinValue()) {
    m_minus->Draw(mousePosition);
  }
  if (GetValue() != GetMaxValue()) {
    m_plus->Draw(mousePosition);
  }
}

Widget * SpinButton::ClickUp(const Point2i & mousePosition,
                             uint button)
{
  NeedRedrawing();

  bool is_click = Mouse::IS_CLICK_BUTTON(button);
  if ((button == SDL_BUTTON_WHEELDOWN && Contains(mousePosition)) ||
      (is_click && m_minus->Contains(mousePosition))){
    DecValue();
    return this;
  } else if ((button == SDL_BUTTON_WHEELUP && Contains(mousePosition)) ||
             (is_click && m_plus->Contains(mousePosition))){
    IncValue();
    return this;
  }
  return NULL;
}

void SpinButton::ValueHasChanged()
{
  std::ostringstream value_s;
  value_s << GetValue() ;

  std::string s(value_s.str());
  txtValue->SetText(s);
}
