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
#include "gui/null_widget.h"
#include "gui/picture_widget.h"
#include "gui/spin_button_picture.h"
#include "gui/text_box.h"
#include "gui/vertical_box.h"
#include "menu/team_box.h"
#include "include/action_handler.h"
#include "network/network.h"
#include "team/team.h"
#include "team/team_group.h"
#include "team/custom_team.h"
#include "team/custom_teams_list.h"
#include "tool/resource_manager.h"

#define CHAR_COUNT_WIDGET_SIZE  120

static const std::string ai_names[] = { NO_AI_NAME, DEFAULT_AI_NAME, DUMB_AI_NAME, STRONG_AI_NAME };


TeamBox::TeamBox(const std::string& _player_name, const Point2i& _size, uint g)
  : HBox(_size.y, false, false, false)
  , ai_level(0)
  , group(g)
{
  associated_team = NULL;

  SetMargin(2);
  SetBorder(transparent_color, 4);
  Widget::SetBackgroundColor(transparent_color);

  Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml", false);

  player_local[0] = LOAD_RES_IMAGE("menu/player_local_human");
  player_local[1] = LOAD_RES_IMAGE("menu/player_local_ai");
  player_local[2] = LOAD_RES_IMAGE("menu/player_local_ai_dumb");
  player_local[3] = LOAD_RES_IMAGE("menu/player_local_ai_strong");

  player_remote[0] = LOAD_RES_IMAGE("menu/player_remote_human");
  player_remote[1] = LOAD_RES_IMAGE("menu/player_remote_ai");
  player_remote[2] = LOAD_RES_IMAGE("menu/player_remote_ai_dumb");
  player_remote[3] = LOAD_RES_IMAGE("menu/player_remote_ai_strong");

  int width = _size.x - (CHAR_COUNT_WIDGET_SIZE+2*margin+2*border_size);
  Box * tmp_player_box = new VBox(_size.y-2*border_size, false, false, false);
  tmp_player_box->SetMargin(4);
  tmp_player_box->SetNoBorder();
  AddWidget(tmp_player_box);

  tmp_player_box->AddWidget(new NullWidget(Point2i(width, 20)));

  /********   Box for team logo, commander, custom team, ai & group  *******/
  HBox *hbox = new HBox(W_UNDEF, false, false, false);
  hbox->SetMargin(0);
  hbox->SetNoBorder();
  tmp_player_box->AddWidget(hbox);

  /****    AI    ****/
  player_type =  new PictureWidget(Point2i(40, 40));
  player_type->SetSurface(player_local[ai_level]);
  hbox->AddWidget(player_type);

  /****    Team Logo    ****/
  team_logo = new PictureWidget(Point2i(40, 40));
  hbox->AddWidget(team_logo);

  /****     Team name/commander    ****/
  VBox *vbox = new VBox(W_UNDEF, false, false, false);
  vbox->SetMargin(0);
  vbox->SetNoBorder();
  hbox->AddWidget(vbox);

  previous_player_name = "team";
  team_name = new Label(previous_player_name, width - (40*2 + 60),
                        Font::FONT_MEDIUM, Font::FONT_BOLD);
  vbox->AddWidget(team_name);

  /****    Names: "Head commander" + text/custom team    ****/
  vbox->AddWidget(new Label(_("Head commander"), width - (40*2 + 60),
                            Font::FONT_SMALL, Font::FONT_BOLD));

  /****  Group selection box ****/
  nullw = new NullWidget(Point2i(60, 40));
  nullw->SetBackgroundColor(TeamGroup::Colors[g]);
  hbox->AddWidget(nullw);

  custom_team_list = GetCustomTeamsList().GetList();
  custom_team_current_id = 0;

  if (custom_team_list.empty()) {
    player_name = new TextBox(_player_name, width,
                              Font::FONT_SMALL, Font::FONT_BOLD);
    tmp_player_box->AddWidget(player_name);

    next_custom_team = NULL;
    previous_custom_team = NULL;

  } else {
    next_custom_team = new Button(res, "menu/big_plus");
    previous_custom_team = new Button(res, "menu/big_minus");

    player_name = new TextBox(_player_name, width - 2 * (next_custom_team->GetSizeX() + 2),
                              Font::FONT_SMALL, Font::FONT_BOLD);

    Box * tmp_name_box = new HBox(player_name->GetSizeY(), false, false, false);
    tmp_name_box->SetNoBorder();
    tmp_name_box->SetMargin(2);

    tmp_name_box->AddWidget(previous_custom_team);
    tmp_name_box->AddWidget(player_name);
    tmp_name_box->AddWidget(next_custom_team);
    tmp_player_box->AddWidget(tmp_name_box);
  }

  /**********     Number of characters        **********/
  nb_characters = new SpinButtonWithPicture(_("Number of characters"), "menu/ico_play",
                                            Point2i(CHAR_COUNT_WIDGET_SIZE, 110), 6, 1, 1, 10);
  AddWidget(nb_characters);
  Pack();
}

void TeamBox::ClearTeam()
{
  associated_team = NULL;
  ai_level = 0;

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
    if (w == nullw) {
      if (button == Mouse::BUTTON_LEFT()) {
        if (group == MAX_TEAM_GROUPS-1) group = 0;
        else                            group++;
      } else {
        if (group == 0) group = MAX_TEAM_GROUPS-1;
        else            group--;
      }
      SetGroup(group);
      UpdateTeam(associated_team->GetId());
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

  // Update group
  SetGroup(_team.GetGroup());

  if (read_team_values) {
    player_name->SetText(_team.GetPlayerName());
    nb_characters->SetValue(_team.GetNbCharacters());
    for (uint i=0; i<4; i++) {
      if (ai_names[i] == _team.GetAIName()) {
        SetAILevel(i);
        break;
      }
    }
  } else if (old_team) {
    UpdateTeam(old_team->GetId());
  }
  previous_player_name = player_name->GetText();

  NeedRedrawing();
}

void TeamBox::UpdatePlayerType()
{
  Surface *icons = (associated_team->IsLocal()) ? player_local : player_remote;
  player_type->SetSurface(icons[ai_level]);
}

void TeamBox::UpdateTeam(const std::string& old_team_id) const
{
  // set the number of characters
  associated_team->SetNbCharacters(uint(nb_characters->GetValue()));

  // set the player name
  associated_team->SetPlayerName(player_name->GetText());

  associated_team->SetAIName(ai_names[ai_level]);
  associated_team->SetGroup(group);

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
      a->Push(group);
      associated_team->PushCustomCharactersNamesIntoAction(a);
      ActionHandler::GetInstance()->NewAction(a);
    }
  }
}

void TeamBox::ValidOptions() const
{
  UpdateTeam(associated_team->GetId());
}

bool TeamBox::IsLocal() const
{
  if (associated_team && associated_team->IsLocal()) {
    return true;
  }

  return false;
}

void TeamBox::SwitchPlayerType()
{
  if (!associated_team)
    return;

  ai_level++;
  if (ai_level > 3)
    ai_level = 0;
  UpdatePlayerType();
  if (Network::GetInstance()->IsConnected()) {
    ValidOptions();
  }
}

void TeamBox::SetGroup(uint g)
{
  assert(g < MAX_TEAM_GROUPS);

  group = g;
  nullw->SetBackgroundColor(TeamGroup::Colors[g]);
}

bool TeamBox::IsTeamSwitcherAt(const Point2i& mousePosition) const
{
  return team_logo->Contains(mousePosition);
}

bool TeamBox::IsAISwitcherAt(const Point2i& mousePosition) const
{
  return player_type->Contains(mousePosition);
}
