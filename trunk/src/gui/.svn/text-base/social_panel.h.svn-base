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
 * Panel of widgets to enter credentials for several websites
 *****************************************************************************/

#ifndef SOCIAL_PANEL_H
#define SOCIAL_PANEL_H

#if defined(HAVE_FACEBOOK) || defined(HAVE_TWITTER)

#include "gui/vertical_box.h"

class TextBox;
class CheckBox;
class PasswordBox;
class SocialWidget;

class SocialPanel : public VBox
{
#ifdef HAVE_FACEBOOK
  SocialWidget *facebook;
#endif
#ifdef HAVE_TWITTER
  SocialWidget *twitter;
#endif
  bool save;

public:
  SocialPanel(int width, float factor, bool save);
  void Close();

#ifdef HAVE_FACEBOOK
  bool FacebookButtonPushed(const Widget *w, std::string& user, std::string& pwd) const;
#endif
#ifdef HAVE_TWITTER
  bool TwitterButtonPushed(const Widget *w, std::string& user, std::string& pwd) const;
#endif
};

#endif

#endif //  SOCIAL_PANEL_H
