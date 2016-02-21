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
 * Skin menu, to visualize them easily
 *****************************************************************************/

#include <assert.h>
#include "character/body_list.h"
#include "character/body.h"
#include "character/clothe.h"
#include "graphic/video.h"
#include "gui/box.h"
#include "gui/list_box_w_label.h"
#include "gui/spin_button.h"
#include "include/app.h"
#include "game/config.h"
#include "team/team.h"
#include "team/teams_list.h"
#include "tool/file_tools.h"
#include "tool/i18n.h"
#include "menu/skin_menu.h"

static const uint BORDER        = 20;
static const uint CHECKBOX_SIZE = 50;
static const uint SKIN_SIZE     = 100;

SkinMenu::SkinMenu(const char* name)
 : Menu("menu/bg_option")
 , frame(NULL)
 , body(NULL)
{
  uint size = GetMainWindow().GetHeight()
            - (2*BORDER+CHECKBOX_SIZE+SKIN_SIZE);
  Box  *box = new HBox(size);

  position = 2*BORDER + size;

  teams = new ListBoxWithLabel("Teams", Point2i(120, -1)); box->AddWidget(teams);
  clothe_list = new ListBoxWithLabel("Clothes", Point2i(130, -1)); box->AddWidget(clothe_list);
  movement_list = new ListBoxWithLabel("Movements", Point2i(240, -1)); box->AddWidget(movement_list);

  // Load Wormux teams - use TeamList::full_list if you don't mind the whole loading
  const Config * config = Config::GetConstInstance();
  bool  found = false;
  std::string dirname = config->GetDataDir() + "team" PATH_SEPARATOR;
  FolderSearch *f = OpenFolder(dirname);
  if (f) {
    const char *tname;
    while ((tname = FolderSearchNext(f)) != NULL)
    {
      if (tname[0] != '.')
      {
        bool match = name && !strcmp(name, tname);
        teams->AddItem(match, tname, tname);
        printf("Adding %s\n", tname);
        if (match)
          found = true;
      }
    }
    CloseFolder(f);
  } else {
    Error (Format(_("Cannot open teams directory (%s)!"), dirname.c_str()));
  }

  box->SetPosition(BORDER, BORDER);
  widgets.AddWidget(box);
  widgets.Pack();

  if (name && !found)
    Error("Team not found!");
}

SkinMenu::~SkinMenu()
{
}

void SkinMenu::LoadBody(const std::string& name)
{
  printf("Should load %s\n", name.c_str());
  body_list.FreeMem();
  body = body_list.GetBody(name);

  clothe_list->ClearItems();
  for (std::map<std::string, Clothe*>::const_iterator it = body->clothes_lst.begin();
       it != body->clothes_lst.end(); ++it)
  {
    clothe_list->AddItem(false, (*it).first, (*it).first);
  }

  movement_list->ClearItems();
  for (std::map<std::string, Movement*>::const_iterator it = body->mvt_lst.begin();
       it != body->mvt_lst.end(); ++it)
  {
    movement_list->AddItem(false, (*it).first, (*it).first);
  }
}

void SkinMenu::Display()
{
  if (body)
  {
    if (frame)
    {
      // Will also delete it
      widgets.RemoveWidget(frame);
      frame = NULL;
    }

    uint count = body->GetFrameCount()-1;
    if (count)
    {
      frame = new SpinButton("Frame", 90, 0, 1, 0, count);
      frame->SetPosition(Point2i(BORDER+50, position));
      widgets.AddWidget(frame);
      widgets.Pack();
    }
    RedrawBackground(Rectanglei(Point2i(BORDER-10, position-10), Point2i(160,60)));
    body->Draw(Point2i(BORDER, position));
  }
}

void SkinMenu::OnClick(const Point2i &mousePosition, int button)
{
  widgets.Click(mousePosition, button);
}

void SkinMenu::OnClickUp(const Point2i &mousePosition, int button)
{
  Widget* w = widgets.ClickUp(mousePosition, button);

  // Now that the click has been processed by the underlying widgets,
  // make use of their newer values in near-realtime!
  if (w == teams)
  {
    LoadBody(teams->ReadValue());
  }
  else if (w == clothe_list)
  {
    if (body)
    {
      body->SetClothe(clothe_list->ReadValue());
      if (movement_list->GetSelectedItem() != -1)
        Display();
    }
  }
  else if (w == movement_list)
  {
    if (body)
    {
      // Unfortunately, those movements make it crash:
      // black, breathe, drowned
      body->SetMovement(movement_list->ReadValue());
      if (clothe_list->GetSelectedItem() != -1)
        Display();
    }
  }
  else if (frame && w == frame)
  {
    if (body)
    {
      body->SetFrame(frame->GetValue());
      RedrawBackground(Rectanglei(Point2i(BORDER-10, position-10), Point2i(60,60)));
      body->Draw(Point2i(BORDER, position));
    }
  }
}
