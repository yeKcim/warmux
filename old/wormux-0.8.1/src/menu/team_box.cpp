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
 *  Teams selection box
 *****************************************************************************/

#include "gui/button.h"
#include "gui/label.h"
#include "gui/picture_widget.h"
#include "gui/spin_button.h"
#include "gui/text_box.h"
#include "menu/team_box.h"
#include "include/action_handler.h"
#include "network/network.h"
#include "team/team.h"
#include "team/custom_team.h"
#include "team/custom_teams_list.h"
#include "tool/i18n.h"
#include "tool/resource_manager.h"

TeamBox::TeamBox(const std::string& _player_name, const Point2i& _size) :
  HBox(W_UNDEF, false, false)
{
  associated_team=NULL;

  SetMargin(2);
  SetNoBorder();

  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);

  team_logo = new PictureWidget(Point2i(48, 48));
  AddWidget(team_logo);

  Box * tmp_box = new VBox(W_UNDEF, false, false);
  tmp_box->SetMargin(2);
  tmp_box->SetNoBorder();
  previous_player_name = "team";
  team_name = new Label(previous_player_name, _size.x - 50,
                        Font::FONT_MEDIUM, Font::FONT_BOLD,
                        dark_gray_color, false, false);

  Box * tmp_player_box = new HBox(W_UNDEF, false, false);
  tmp_player_box->SetMargin(0);
  tmp_player_box->SetNoBorder();

  custom_team_list = GetCustomTeamsList().GetList();
  custom_team_current_id = 0;

  player_name = new TextBox(_player_name, 100,
                            Font::FONT_SMALL, Font::FONT_NORMAL);

  if(custom_team_list.size()==0){
  tmp_player_box->AddWidget(new Label(_("Head commander"), _size.GetX()-50-100,
                                      Font::FONT_SMALL, Font::FONT_NORMAL, dark_gray_color, false, false));

    tmp_player_box->AddWidget(player_name);
  }
  else
  {
    tmp_player_box->AddWidget(new Label(_("Head commander"), _size.GetX()-60-100,
                                      Font::FONT_SMALL, Font::FONT_NORMAL, dark_gray_color, false, false));

    next_custom_team = new Button(res, "menu/plus");

    previous_custom_team = new Button(res, "menu/minus");

    tmp_player_box->AddWidget(previous_custom_team);
    tmp_player_box->AddWidget(player_name);
    tmp_player_box->AddWidget(next_custom_team);
  }

  nb_characters = new SpinButton(_("Number of characters"), _size.GetX()-50,
                                 6,1,1,10,
                                 dark_gray_color, false);

  tmp_box->AddWidget(team_name);
  tmp_box->AddWidget(tmp_player_box);
  tmp_box->AddWidget(nb_characters);


  AddWidget(tmp_box);
}

void TeamBox::ClearTeam()
{
  associated_team=NULL;

  NeedRedrawing();
}

Team* TeamBox::GetTeam() const
{
  return associated_team;
}

CustomTeam* TeamBox::GetCustomTeam()
{
  std::cout<<"TeamBox::GetCustomTeam"<<std::endl;
  if( custom_team_list.size() == 0)
  {
    return NULL;
  }
  else
  {
    return GetCustomTeamsList().GetByName(player_name->GetText());
  }
}

void TeamBox::Update(const Point2i &mousePosition,
                     const Point2i &lastMousePosition)
{
  Box::Update(mousePosition, lastMousePosition);
  if (need_redrawing) {
    Draw(mousePosition);
  }

  if (associated_team != NULL){
    WidgetList::Update(mousePosition);
  } else {
    RedrawBackground(*this);
  }

  if (associated_team != NULL && previous_player_name != player_name->GetText()) {
    previous_player_name = player_name->GetText();
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
    if(custom_team_list.size()>0)
    {
      if (w == next_custom_team)
      {
        player_name->SetText(custom_team_list[custom_team_current_id]->GetName());

        if(custom_team_current_id == custom_team_list.size()-1)
        {
          custom_team_current_id = 0;
        }
        else
        {
          custom_team_current_id++;
        }
      }
      if (w == previous_custom_team)
      {
        player_name->SetText(custom_team_list[custom_team_current_id]->GetName());

        if(custom_team_current_id == 0)
        {
          custom_team_current_id = custom_team_list.size()-1;
        }
        else
        {
          custom_team_current_id--;
        }
      }
    }
  }
  return NULL;
}

Widget* TeamBox::Click(const Point2i &/*mousePosition*/, uint /*button*/)
{
  return NULL;
}

void TeamBox::SetTeam(Team& _team, bool read_team_values)
{
  Team* old_team = associated_team;

  associated_team = &_team;

  if (_team.IsRemote()) {
    team_name->SetFont(dark_gray_color, Font::FONT_MEDIUM, Font::FONT_BOLD, false, false);

    // translators: this is the team listing and will expand in a context like "OOo team - Remote"
    team_name->SetText(Format(_("%s Team - Remote"), _team.GetName().c_str()));
  } else {
    team_name->SetFont(primary_red_color, Font::FONT_MEDIUM, Font::FONT_BOLD, true, false);
    team_name->SetText(Format(_("%s Team"), _team.GetName().c_str()));
  }
  team_logo->SetSurface(_team.GetFlag());

  if (read_team_values) {
    player_name->SetText(_team.GetPlayerName());
    nb_characters->SetValue(_team.GetNbCharacters());
  } else if (old_team) {
    UpdateTeam(old_team->GetId());
  }
  previous_player_name = player_name->GetText();

  NeedRedrawing();
}

void TeamBox::UpdateTeam(const std::string& old_team_id) const
{
  // set the number of characters
  associated_team->SetNbCharacters(uint(nb_characters->GetValue()));

  // set the player name
  associated_team->SetPlayerName(player_name->GetText());

  // change only for local teams...
  if (associated_team->IsLocal() || associated_team->IsLocalAI()) {

    // send team configuration to the remote clients
    if (Network::GetInstance()->IsConnected()) {
      Action* a = new Action(Action::ACTION_MENU_UPDATE_TEAM, old_team_id);
      a->Push(associated_team->GetId());
      a->Push(associated_team->GetPlayerName());
      a->Push(int(associated_team->GetNbCharacters()));
      ActionHandler::GetInstance()->NewAction (a);
    }
  }
}

void TeamBox::ValidOptions() const
{
  UpdateTeam(associated_team->GetId());
}

bool TeamBox::IsLocal() const
{
  if (associated_team != NULL && associated_team->IsLocal()) {
    return true;
  }

  return false;
}
