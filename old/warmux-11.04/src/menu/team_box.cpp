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
 *  Teams selection box
 *****************************************************************************/

#include "ai/ai_stupid_player.h"
#include "graphic/video.h"
#include "gui/button.h"
#include "gui/label.h"
#include "gui/picture_widget.h"
#include "gui/spin_button_picture.h"
#include "gui/text_box.h"
#include "gui/vertical_box.h"
#include "menu/team_box.h"
#include "include/action_handler.h"
#include "network/network.h"
#include "team/team.h"
#include "team/custom_team.h"
#include "team/custom_teams_list.h"
#include "tool/resource_manager.h"

TeamBox::TeamBox(const std::string& _player_name, const Point2i& _size) :
  HBox(_size.y, false, false, false),
  ai_name(NO_AI_NAME)
{
  associated_team = NULL;

  SetMargin(2);
  SetNoBorder();
  Widget::SetBackgroundColor(transparent_color);

  Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml", false);

  /********        Logos: team mascott, player type icon      ********/
  Box * tmp_logo_box = new VBox(W_UNDEF, false, false, false);
  tmp_logo_box->SetMargin(1);
  tmp_logo_box->SetNoBorder();

  team_logo = new PictureWidget(Point2i(38, 38));
  tmp_logo_box->AddWidget(team_logo);

  player_local_ai_surf = GetResourceManager().LoadImage(res, "menu/player_local_ai");
  player_local_human_surf = GetResourceManager().LoadImage(res, "menu/player_local_human");
  player_remote_ai_surf = GetResourceManager().LoadImage(res, "menu/player_remote_ai");
  player_remote_human_surf = GetResourceManager().LoadImage(res, "menu/player_remote_human");

  player_type =  new PictureWidget(Point2i(38, 30));
  player_type->SetSurface(player_local_ai_surf);
  tmp_logo_box->AddWidget(player_type);

  AddWidget(tmp_logo_box);

  /********    Center box: team name, commander   *********/
  int width = _size.x - (2*2+(38+2*2)+(110+2*2));
  Box * tmp_player_box = new VBox(_size.y, false, false, false);
  tmp_player_box->SetMargin(0);
  tmp_player_box->SetNoBorder();

  previous_player_name = "team";
  team_name = new Label(previous_player_name, width,
                        Font::FONT_MEDIUM, Font::FONT_BOLD);
  tmp_player_box->AddWidget(team_name);

  /********    Names: "Head commander" + text/custom team    *******/
  tmp_player_box->AddWidget(new Label(_("Head commander"), width,
                                      Font::FONT_SMALL, Font::FONT_BOLD));

  custom_team_list = GetCustomTeamsList().GetList();
  custom_team_current_id = 0;

  if (custom_team_list.empty()) {
    player_name = new TextBox(_player_name, width,
                              Font::FONT_SMALL, Font::FONT_BOLD);
    tmp_player_box->AddWidget(player_name);

    next_custom_team = NULL;
    previous_custom_team = NULL;

  } else {
    next_custom_team = new Button(res, "menu/plus");
    previous_custom_team = new Button(res, "menu/minus");

    player_name = new TextBox(_player_name, width - 2 * (next_custom_team->GetSizeY() + 2),
                              Font::FONT_SMALL, Font::FONT_BOLD);

    Box * tmp_name_box = new HBox(player_name->GetSizeY(), false, false, false);
    tmp_name_box->SetNoBorder();
    tmp_name_box->SetMargin(2);

    tmp_name_box->AddWidget(previous_custom_team);
    tmp_name_box->AddWidget(player_name);
    tmp_name_box->AddWidget(next_custom_team);
    tmp_player_box->AddWidget(tmp_name_box);
  }
  AddWidget(tmp_player_box);


  /**********     Number of characters        **********/
  nb_characters = new SpinButtonWithPicture(_("Number of characters"), "menu/ico_play",
                                            Point2i(110, 120), 6, 1, 1, 10);
  AddWidget(nb_characters);
}

void TeamBox::ClearTeam()
{
  associated_team = NULL;
  ai_name = NO_AI_NAME;

  NeedRedrawing();
}

CustomTeam* TeamBox::GetCustomTeam() const
{
  if (custom_team_list.empty()) {
    return NULL;
  }

  return GetCustomTeamsList().GetByName(player_name->GetText());
}

void TeamBox::UpdatePlayerNameColor()
{
  if (associated_team) {
    if (associated_team->IsLocal()) {
      if (GetCustomTeam()) {
        // player name is head commander of custom team
        player_name->SetColor(c_yellow);
      } else {
        player_name->SetColor(c_white);
      }
    } else {
      // remote team
      player_name->SetColor(light_gray_color);
    }
  }
}

bool TeamBox::Update(const Point2i &mousePosition,
                     const Point2i &lastMousePosition)
{
  UpdatePlayerNameColor();

  Box::Update(mousePosition, lastMousePosition);
  if (need_redrawing) {
    Draw(mousePosition);
  }

  if (associated_team) {
    WidgetList::Update(mousePosition, lastMousePosition);
    if (previous_player_name != player_name->GetText()) {
      previous_player_name = player_name->GetText();
      if (Network::GetInstance()->IsConnected()) {
        ValidOptions();
      }
    }
  } else {
    RedrawBackground(*this);
  }

  //SwapWindowClip(r);
  need_redrawing = false;
  return true;
}

Widget* TeamBox::ClickUp(const Point2i &mousePosition, uint button)
{
  if (associated_team) {

    if (!associated_team->IsLocal())
      return NULL; // it's not a local team, we can't configure it !!

    Widget* w = WidgetList::ClickUp(mousePosition, button);

    if (w == nb_characters) {
      if (Network::GetInstance()->IsConnected()) {
        ValidOptions();
      }
      return w;
    }
    if (w == player_name) {
      return w;
    }

    if (!w) {
      return w;
    }

    if (!custom_team_list.empty()) {

      if (w == next_custom_team) {
        player_name->SetText(custom_team_list[custom_team_current_id]->GetName());

        if (custom_team_current_id == custom_team_list.size()-1) {
          custom_team_current_id = 0;
        } else {
          custom_team_current_id++;
        }
      }

      if (w == previous_custom_team) {

        player_name->SetText(custom_team_list[custom_team_current_id]->GetName());

        if (custom_team_current_id == 0) {
          custom_team_current_id = custom_team_list.size()-1;
        } else {
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
    team_name->SetFont(dark_gray_color, Font::FONT_MEDIUM, Font::FONT_BOLD, false);

    // translators: this is the team listing and will expand in a context like "OOo team - Remote"
    team_name->SetText(Format(_("%s Team - Remote"), _team.GetName().c_str()));

    if (previous_custom_team) {
      previous_custom_team->SetVisible(false);
      next_custom_team->SetVisible(false);
    }
  } else {
    team_name->SetFont(primary_red_color, Font::FONT_MEDIUM, Font::FONT_BOLD, true);
    team_name->SetText(Format(_("%s Team"), _team.GetName().c_str()));

    if (previous_custom_team) {
      previous_custom_team->SetVisible(true);
      next_custom_team->SetVisible(true);
    }
  }
  UpdatePlayerType();
  team_logo->SetSurface(_team.GetFlag());

  if (read_team_values) {
    player_name->SetText(_team.GetPlayerName());
    nb_characters->SetValue(_team.GetNbCharacters());
    SetAIName(_team.GetAIName());
  } else if (old_team) {
    UpdateTeam(old_team->GetId());
  }
  previous_player_name = player_name->GetText();

  NeedRedrawing();
}

void TeamBox::SetAIName(const std::string name)
{
  ai_name = name;
  UpdatePlayerType();
}

void TeamBox::UpdatePlayerType()
{
  bool is_human = ai_name == NO_AI_NAME;
  if (associated_team->IsLocal()) {
    if (is_human) {
      player_type->SetSurface(player_local_human_surf);
    } else {
      player_type->SetSurface(player_local_ai_surf);
    }
  } else {
    if (is_human) {
      player_type->SetSurface(player_remote_human_surf);
    } else {
      player_type->SetSurface(player_remote_ai_surf);
    }
  }
}

void TeamBox::UpdateTeam(const std::string& old_team_id) const
{
  // set the number of characters
  associated_team->SetNbCharacters(uint(nb_characters->GetValue()));

  // set the player name
  associated_team->SetPlayerName(player_name->GetText());

  associated_team->SetAIName(ai_name);

  // change only for local teams...
  if (associated_team->IsLocal()) {

    CustomTeam* custom = GetCustomTeam();
    if (custom) {
      associated_team->SetCustomCharactersNames(custom->GetCharactersNameList());
    } else {
      associated_team->ClearCustomCharactersNames();
    }

    // send team configuration to the remote clients
    if (Network::GetInstance()->IsConnected()) {
      Action* a = new Action(Action::ACTION_GAME_UPDATE_TEAM);
      a->Push(int(Network::GetInstance()->GetPlayer().GetId()));
      a->Push(old_team_id);
      a->Push(associated_team->GetId());
      a->Push(associated_team->GetPlayerName());
      a->Push(int(associated_team->GetNbCharacters()));
      a->Push(associated_team->GetAIName());
      associated_team->PushCustomCharactersNamesIntoAction(a);
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

void TeamBox::SwitchPlayerType()
{
  if (!associated_team)
    return;

  SetAIName((ai_name == NO_AI_NAME) ? DEFAULT_AI_NAME : NO_AI_NAME);
  if (Network::GetInstance()->IsConnected()) {
    ValidOptions();
  }
}
