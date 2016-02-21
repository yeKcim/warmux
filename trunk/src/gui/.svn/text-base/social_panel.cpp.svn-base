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

#include <string>

#include "game/config.h"
#include "gui/label.h"
#include "gui/check_box.h"
#include "gui/horizontal_box.h"
#include "gui/picture_widget.h"
#include "gui/social_panel.h"
#include "gui/text_box.h"
#include "gui/vertical_box.h"

#if defined(HAVE_FACEBOOK) || defined(HAVE_TWITTER)

class SocialWidget : public HBox
{
  Surface        icon;
  PictureWidget *pic;

  TextBox*     UserBox;
  CheckBox*    SaveBox;
  HBox*        HideBox;
  PasswordBox* PwdBox;
  bool         use_save;

public:
  SocialWidget(const std::string& name, int ssize, float factor, bool use_save,
               const std::string& user, const std::string& pwd, bool saved)
    : HBox(122*factor, false)
  {
    Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml", false);
    Font::font_size_t fbig = Font::GetFixedSize(Font::FONT_BIG*factor+0.5f);
    Font::font_size_t fmedium = Font::GetFixedSize(Font::FONT_MEDIUM*factor+0.5f);

    // Default look
    SetBorder(5*factor);

    // Set the icon
    icon = LOAD_RES_IMAGE("menu/" + name);
    pic  = new PictureWidget(icon);
    AddWidget(pic);

    ssize -= 10*factor + icon.GetWidth();
    VBox *vbox = new VBox(ssize, false, false, false);
    // Name
    vbox->AddWidget(new Label(name, ssize, fbig, Font::FONT_BOLD, c_red));

    // Email
    HBox *hbox = new HBox(30*factor, false); hbox->SetNoBorder();
    hbox->AddWidget(new Label(_("User"), ssize/3-5*factor, fmedium));
    UserBox = new TextBox(user, (2*ssize)/3-5*factor, fmedium);
    hbox->AddWidget(UserBox);
    vbox->AddWidget(hbox);

    // Password
    SaveBox = new CheckBox(_("Save password"), ssize - 10*factor, saved, fmedium);
    SaveBox->SetValue(saved);
    vbox->AddWidget(SaveBox);

    HideBox = new HBox(30*factor, false); HideBox->SetNoBorder();
    HideBox->AddWidget(new Label(_("Password"), ssize/3-5*factor, fmedium));
    PwdBox = new PasswordBox(pwd, (2*ssize)/3-5*factor, fmedium);
    HideBox->AddWidget(PwdBox);
    if (use_save) HideBox->SetVisible(saved);
    vbox->AddWidget(HideBox);

    AddWidget(vbox);
  }

  virtual Widget * ClickUp(const Point2i & mousePosition, uint button)
  {
    if (pic->Contains(mousePosition))
      return pic;
    Widget *w = HBox::ClickUp(mousePosition, button);
    if (use_save && w == SaveBox) {
      HideBox->SetVisible(SaveBox->GetValue());
    }
    return w;
  }

  bool IsSaved() const { return SaveBox->GetValue(); }
  bool ButtonPushed(const Widget* w) const { return w == pic; }
  const std::string& GetUser() const { return UserBox->GetText(); }
  const std::string& GetPassword() const { return PwdBox->GetPassword(); }
};

SocialPanel::SocialPanel(int width, float factor, bool s)
  : VBox(width, false)
  , save(s)
{
  std::string user, pwd;

#ifdef HAVE_FACEBOOK
  Config::GetInstance()->GetFaceBookCreds(user, pwd);
  facebook = new SocialWidget("Facebook", width, factor, s,
                              user, pwd, Config::GetInstance()->GetFaceBookSave());
  AddWidget(facebook);
#endif
#ifdef HAVE_TWITTER
  Config::GetInstance()->GetTwitterCreds(user, pwd);
  twitter = new SocialWidget("Twitter", width, factor, s,
                             user, pwd, Config::GetInstance()->GetTwitterSave());
  AddWidget(twitter);
#endif

  // To set every child background color
  SetBackgroundColor(transparent_color);
}

void SocialPanel::Close()
{
  if (save) {
    Config* config = Config::GetInstance();
#  ifdef HAVE_FACEBOOK
    config->SetFaceBookSave(facebook->IsSaved());
    config->SetFaceBookCreds(facebook->GetUser(), facebook->GetPassword());
#  endif
#  ifdef HAVE_TWITTER
    config->SetTwitterSave(twitter->IsSaved());
    config->SetTwitterCreds(twitter->GetUser(), twitter->GetPassword());
#  endif
  }
}

#ifdef HAVE_FACEBOOK
bool SocialPanel::FacebookButtonPushed(const Widget *w, std::string& user, std::string& pwd) const
{
  if (facebook->ButtonPushed(w)) {
    user = facebook->GetUser();
    pwd  = facebook->GetPassword();
    return true;
  }
  return false;
}
#endif
#ifdef HAVE_TWITTER
bool SocialPanel::TwitterButtonPushed(const Widget *w, std::string& user, std::string& pwd) const
{
  if (twitter->ButtonPushed(w)) {
    user = twitter->GetUser();
    pwd  = twitter->GetPassword();
    return true;
  }
  return false;
}
#endif

#endif
