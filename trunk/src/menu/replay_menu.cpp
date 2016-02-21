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
 * Replay menu
 *****************************************************************************/

#include <fstream>
#include <time.h>

#include <WARMUX_i18n.h>

#include "game/game.h"
#include "game/config.h"
#include "graphic/font.h"
#include "graphic/video.h"
#include "gui/file_list_box.h"
#include "gui/question.h"
#include "gui/select_box.h"
#include "gui/vertical_box.h"
#include "menu/replay_menu.h"
#include "replay/replay.h"
#include "replay/replay_info.h"

ReplayMenu::ReplayMenu()
  : Menu("menu/bg_play")
  , selected(NULL)
{
  // Center the boxes!
  uint h      = GetMainWindow().GetHeight();
  uint size_y = h-50;
  float z     = size_y/430.0;
  uint border = 5*z;
  uint size_x = GetMainWindow().GetWidth()/19;
  uint w      = 8*size_x - 2*border;
  Font::font_size_t fsize1 = Font::GetFixedSize(z * 18);
  Font::font_size_t fsize2 = Font::GetFixedSize(z * 16);

#define ARG_LIST  fsize1, Font::FONT_BOLD, primary_red_color, Text::ALIGN_LEFT_TOP, true
#define ARG_LIST2 fsize2, Font::FONT_BOLD, dark_gray_color, Text::ALIGN_LEFT_TOP, false

  //Replay files
  VBox *vbox = new VBox(8*size_x, true);
  vbox->SetPosition(size_x, size_y/20);
  widgets.AddWidget(vbox);

  file_box = new VBox(w, false);
  file_box->SetBorder(border);
  file_box->SetSelfBackgroundColor(transparent_color);
  vbox->AddWidget(file_box);
  file_box->AddWidget(new Label(_("Select a replay:"), w, ARG_LIST));

  replay_lbox = new FileListBox(Point2i(w-20, (9*size_y)/10 - fsize1 - 3*5));
  replay_lbox->SetBorder(defaultOptionColorRect, 2);
  // Windows may SFN => uppercase extensions
  replay_lbox->AddExtensionFilter("WRF");
  replay_lbox->AddExtensionFilter("wrf");
  replay_lbox->StartListing();
  file_box->AddWidget(replay_lbox);

  //Info
  vbox = new VBox(8*size_x);
  vbox->SetPosition(10*size_x, size_y/20);
  widgets.AddWidget(vbox);

  info_box = new VBox(w, false);
  info_box->SetBorder(border);
  info_box->SetSelfBackgroundColor(transparent_color);
  vbox->AddWidget(info_box);

  info_box->AddWidget(new Label(_("Replay info"), w, ARG_LIST));

  vbox = new VBox(w-10, true);
  info_box->AddWidget(vbox);

  Box *panel = new VBox(w-10-2*border, false);
  panel->SetBorder(border);
  panel->SetSelfBackgroundColor(transparent_color);
  panel->SetMargin(0);
  vbox->AddWidget(panel);

  // Version
  panel->AddWidget(new Label(_("Version"), w-10, ARG_LIST));
  version_lbl = new Label(" ", w-10, ARG_LIST2);
  panel->AddWidget(version_lbl);

  // Date
  panel->AddWidget(new Label(_("Date"), w-10, ARG_LIST));
  date_lbl = new Label(" ", w-10, ARG_LIST2);
  panel->AddWidget(date_lbl);

  // Duration
  panel->AddWidget(new Label(_("Duration"), w-10, ARG_LIST));
  duration_lbl = new Label(" ", w-20, ARG_LIST2);
  panel->AddWidget(duration_lbl);

  // Comment
  panel->AddWidget(new Label(_("Comment"), w-10, ARG_LIST));
  comment_lbl = new Label(" ", w-10, ARG_LIST2);
  panel->AddWidget(comment_lbl);

  // Teams
  panel->AddWidget(new Label(_("Teams"), w-10, ARG_LIST));
  teams_lbox = new ScrollBox(Point2i(w-10, (9*size_y)/10 - 6*(fsize1+fsize2)- 4*border -2*5));
  panel->AddWidget(teams_lbox);

  widgets.Pack();
}

void ReplayMenu::ClearReplayInfo()
{
  version_lbl->SetText("");
  date_lbl->SetText("");
  duration_lbl->SetText("");
  comment_lbl->SetText("");
  teams_lbox->Clear();
}

void ReplayMenu::ChangeReplay()
{
  const std::string *name = replay_lbox->GetSelectedFile();
  selected = NULL;
  if (!name) {
    ClearReplayInfo();
    return;
  }

  // Get info from Replay:: and fill in here
  FILE *in = fopen(name->c_str(), "rb");
  if (!in) {
    Question question;
    std::string err = "Error: file ";
    err += *name;
    err += " not found";
    question.Set(err, true, 0);
    question.Ask();
    return; /* File deleted meanwhile ? */
  }

  ReplayInfo *info = ReplayInfo::ReplayInfoFromFile(in);
  fclose(in);

  if (!info)
    return; /* Bad problem */

  // Below gets risky to analyze so error out
  if (!info->IsValid()) {
    const std::string& err = info->GetLastError();

    // Clean current state
    ClearReplayInfo();

    Question question;
    fprintf(stderr, "Error: %s\n", err.c_str());
    question.Set(err, true, 0);
    question.Ask();

    delete info;
    return;
  }

  // Version
  std::string text = info->GetVersion();
  version_lbl->SetText(text);

  // Date
  time_t      t        = info->GetDate();
  struct tm * timeinfo = localtime(&t);
  char buffer[20];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %HH%Mm%S", timeinfo);
  date_lbl->SetText(buffer);

  // Duration
  Uint32 time = (info->GetMillisecondsDuration()+500)/1000;
  char   temp[32];
  snprintf(temp, 32, "%um%us", time/60, time%60);
  text = temp;
  duration_lbl->SetText(text);

  // Comment
  text = info->GetComment();
  comment_lbl->SetText(text);

  // Teams
  teams_lbox->Clear();
  for (uint i=0; i<info->GetTeams().size(); i++) {
    printf("Adding %s\n", info->GetTeams()[i].id.c_str());
    teams_lbox->AddWidget(new Label(info->GetTeams()[i].id, 0, Font::FONT_MEDIUM));
  }
  teams_lbox->Pack();
  teams_lbox->NeedRedrawing();

  delete info;
  selected = name;
}

void ReplayMenu::OnClickUp(const Point2i &mousePosition, int button)
{
  if (widgets.ClickUp(mousePosition, button) == replay_lbox)
    ChangeReplay();
}

bool ReplayMenu::signal_ok()
{
  // Restore path, because paths are expected to be relative afterwards
  if (selected) {
    StartPlaying(*selected);
    return true;
  }
  return false;
}
