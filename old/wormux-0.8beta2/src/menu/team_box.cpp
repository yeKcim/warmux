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
 *  Teams selection box
 *****************************************************************************/

#include "gui/label.h"
#include "gui/picture_widget.h"
#include "gui/spin_button.h"
#include "gui/text_box.h"
#include "team_box.h"
#include "include/action_handler.h"
#include "network/network.h"
#include "team/team.h"
#include "tool/i18n.h"

TeamBox::TeamBox(const std::string& _player_name, const Rectanglei& rect) :
  HBox(rect, false)
{
  associated_team=NULL;

  SetMargin(2);

  team_logo = new PictureWidget( Rectanglei(0,0,48,48) );
  AddWidget(team_logo);

  Box * tmp_box = new VBox(Rectanglei(0, 0, rect.GetSizeX()-80, 80), false);
  tmp_box->SetMargin(2);
  tmp_box->SetBorder(Point2i(0,0));
  team_name = new Label(" ", Rectanglei(0,0,rect.GetSizeX()-80,0),
                        Font::FONT_MEDIUM, Font::FONT_BOLD,
                        dark_gray_color, false, false);

  Box * tmp_player_box = new HBox(Rectanglei(0,0,0,Font::GetInstance(Font::FONT_SMALL)->GetHeight()), false);
  tmp_player_box->SetMargin(0);
  tmp_player_box->SetBorder(Point2i(0,0));
  tmp_player_box->AddWidget(new Label(_("Head commander"), Rectanglei(0,0,(rect.GetSizeX()-80)-100,0),
                                      Font::FONT_SMALL, Font::FONT_NORMAL, dark_gray_color, false, false));
  player_name = new TextBox(_player_name, Rectanglei(0,0,100,0),
                            Font::FONT_SMALL, Font::FONT_NORMAL);
  tmp_player_box->AddWidget(player_name);
  previous_name = " ";

  nb_characters = new SpinButton(_("Number of characters"), Rectanglei(0,0,0,0),
                                 6,1,1,10,
                                 dark_gray_color, false);

  tmp_box->AddWidget(team_name);
  tmp_box->AddWidget(tmp_player_box);
  tmp_box->AddWidget(nb_characters);

  AddWidget(tmp_box);
}

void TeamBox::SetTeam(Team& _team, bool read_team_values)
{
  associated_team=&_team;

  if (!_team.IsLocal() && !_team.IsLocalAI()) {
    // translators: this is the team listing and will expand in a context like "OOo team - Remote"
    team_name->SetText(Format(_("%s Team - Remote"), _team.GetName().c_str()));
  } else {
    team_name->SetText(Format(_("%s Team"), _team.GetName().c_str()));
  }
  team_logo->SetSurface(_team.GetFlag());

  if (read_team_values) {
    player_name->SetText(_team.GetPlayerName());
    nb_characters->SetValue(_team.GetNbCharacters());
  }

  ForceRedraw();
}

void TeamBox::ClearTeam()
{
  associated_team=NULL;

  ForceRedraw();
}

Team* TeamBox::GetTeam() const
{
  return associated_team;
}

void TeamBox::Update(const Point2i &mousePosition,
                     const Point2i &lastMousePosition,
                     Surface& surf)
{
  Box::Update(mousePosition, lastMousePosition, surf);
  if (need_redrawing) {
    Draw(mousePosition, surf);
  }

  if (associated_team != NULL){
    WidgetList::Update(mousePosition, surf);
  } else {
    Redraw(*this, surf);
  }

  if (associated_team != NULL && previous_name != player_name->GetText()) {
    previous_name = player_name->GetText();
    if (Network::GetInstance()->IsConnected()) {
      ValidOptions();
    }
  }

  need_redrawing = false;
}

Widget* TeamBox::ClickUp(const Point2i &mousePosition, uint button)
{
  if (associated_team != NULL) {

    Widget* w = WidgetList::ClickUp(mousePosition, button);

    if ( !associated_team->IsLocal() && !associated_team->IsLocalAI() )
      return NULL; // it's not a local team, we can't configure it !!

    if (w == nb_characters) {
      if (Network::GetInstance()->IsConnected()) {
              ValidOptions();
      }
      return w;
    }
    if (w == player_name) {
      return w;
    }
  }
  return NULL;
}

Widget* TeamBox::Click(const Point2i &/*mousePosition*/, uint /*button*/)
{
  return NULL;
}

void TeamBox::ValidOptions() const
{
  // set the number of characters
  associated_team->SetNbCharacters(uint(nb_characters->GetValue()));

  // set the player name
  associated_team->SetPlayerName(player_name->GetText());

  // change only for local teams...
  if (associated_team->IsLocal() || associated_team->IsLocalAI()) {

    // player or AI ?
    if (player_name->GetText() == "AI-stupid")
      associated_team->SetLocalAI();
    else
      associated_team->SetLocal();

    // send team configuration to the remote clients
    if (Network::GetInstance()->IsConnected()) {
      Action* a = new Action(Action::ACTION_MENU_UPDATE_TEAM, associated_team->GetId());
      a->Push(associated_team->GetPlayerName());
      a->Push(int(associated_team->GetNbCharacters()));
      ActionHandler::GetInstance()->NewAction (a);
    }
  }
}

bool TeamBox::IsLocal() const
{
  if (associated_team != NULL && associated_team->IsLocal()) {
    return true;
  }

  return false;
}
