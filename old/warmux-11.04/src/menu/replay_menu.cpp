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
  uint size_x = GetMainWindow().GetWidth()/20;
  uint size_y = h-50;
  Font::font_size_t fsize = (h<=480) ? Font::FONT_MEDIUM : Font::FONT_BIG;


  //Replay files
  uint  w = 8*size_x;
  file_box = new VBox(w);
  file_box->SetPosition(size_x, size_y/20);
#define ARG_LIST  fsize, Font::FONT_BOLD, primary_red_color, Text::ALIGN_LEFT_TOP, true
  file_box->AddWidget(new Label(_("Select a replay:"), w, ARG_LIST));

  replay_lbox = new FileListBox(Point2i(w-20, (9*size_y)/10 - fsize - 3*5));
  replay_lbox->SetBorder(defaultOptionColorRect, 2);
  // Windows may SFN => uppercase extensions
  replay_lbox->AddExtensionFilter("WRF");
  replay_lbox->AddExtensionFilter("wrf");
  replay_lbox->StartListing();
  file_box->AddWidget(replay_lbox);
  widgets.AddWidget(file_box);

  //Info
  info_box = new VBox(w);
  info_box->SetPosition(11*size_x, size_y/20);
  info_box->AddWidget(new Label(_("Replay info"), w, ARG_LIST));
  Box *panel = new VBox(w-10);
  panel->SetMargin(0);
  
  // Version
  panel->AddWidget(new Label(_("Version"), w-10, ARG_LIST));
  version_lbl = new Label(" ", w-10, Font::FONT_MEDIUM);
  panel->AddWidget(version_lbl);

  // Date
  panel->AddWidget(new Label(_("Date"), w-10, ARG_LIST));
  date_lbl = new Label(" ", w-10, Font::FONT_MEDIUM);
  panel->AddWidget(date_lbl);
  
  // Duration
  panel->AddWidget(new Label(_("Duration"), w-10, ARG_LIST));
  duration_lbl = new Label(" ", w-20, Font::FONT_MEDIUM);
  panel->AddWidget(duration_lbl);

  // Comment
  panel->AddWidget(new Label(_("Comment"), w-10, ARG_LIST));
  comment_lbl = new Label(" ", w-10, Font::FONT_MEDIUM);
  panel->AddWidget(comment_lbl);

  // Teams
  panel->AddWidget(new Label(_("Teams"), w-10, ARG_LIST));
  teams_lbox = new ScrollBox(Point2i(w-10, (9*size_y)/10 - 6*(fsize+Font::FONT_MEDIUM+5)));
  panel->AddWidget(teams_lbox);
  
  // Finish info box
  info_box->AddWidget(panel);
 
  widgets.AddWidget(info_box);
  widgets.Pack();
}

ReplayMenu::~ReplayMenu()
{
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
  time_t      t    = info->GetDate();
  text = ctime(&t);
  if (text.size() > 1)
    text[text.size()-1] = 0;
  date_lbl->SetText(text);

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
