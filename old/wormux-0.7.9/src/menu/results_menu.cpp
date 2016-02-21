/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
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
 * Results menu
 *****************************************************************************/

#include "results_menu.h"

#include "../team/results.h"
#include "../character/character.h"
#include "../include/app.h"
#include "../tool/i18n.h"
#include "../tool/string_tools.h"

#define DEF_MARGIN    16
#define DEF_BORDER    8
#define DEF_SIZE      32
#define USE_MOST      1

const Point2i BorderSize(DEF_BORDER, DEF_BORDER);
const Vector2<double> Zoom(1.7321, 1.7321);
const Point2i DefSize(DEF_SIZE, DEF_SIZE);

class ResultBox : public HBox
{
private:
  Box   *type_box;
  Box   *name_box;
  Label *name_lbl;
  Box   *score_box;
  Label *score_lbl;

public:
  ResultBox(const Rectanglei &rect, bool _visible,
            const char* type_name, Font& font,
            const Point2i& type_size,
            const Point2i& name_size,
            const Point2i& score_size);
  // Hopefully no need for ~ResultBox() as it automatically
  // destroy child widgets by inheriting from HBox
  void SetResult(const std::string& name, int score);
};

ResultBox::ResultBox(const Rectanglei &rect, bool _visible,
                     const char *type_name, Font& font,
                     const Point2i& type_size,
                     const Point2i& name_size,
                     const Point2i& score_size)
  : HBox(rect, _visible)
{
  Point2i pos(0, 0);
  Point2i posZero(0,0);

  margin = DEF_MARGIN;
  border.SetValues(DEF_BORDER, DEF_BORDER);

  type_box = new HBox( Rectanglei(pos, type_size), true);
  type_box->AddWidget(new Label(type_name, Rectanglei(pos, type_size), font));
  AddWidget(type_box);

  pos.SetValues(pos.GetX()+type_size.GetX(), pos.GetY());
  name_box = new HBox( Rectanglei(pos, name_size), true);
  name_lbl = new Label("", Rectanglei(pos, name_size), font);
  name_box->AddWidget(name_lbl);
  AddWidget(name_box);

  pos.SetValues(pos.GetX()+name_size.GetX(), pos.GetY());
  score_box = new HBox( Rectanglei(pos, score_size), true);
  score_lbl = new Label("", Rectanglei(pos, score_size), font);
  score_box->AddWidget(score_lbl);
  AddWidget(score_box);
}

void ResultBox::SetResult(const std::string& name, int score)
{
  char buffer[16];
  std::string copy_name(name);

  snprintf(buffer, 16, "%i", score);

  std::string score_str(buffer);

  name_lbl->SetText(copy_name);
  score_lbl->SetText(score_str);

  ForceRedraw();
}

ResultsMenu::ResultsMenu(const std::vector<TeamResults*>* v,
                         const char *winner_name)
  : Menu("menu/bg_play", vOk)
  , results(v)
  , index(0)
  , max_height(DEF_SIZE+3*DEF_BORDER)
  , team_size(360, 40)
  , type_size(260, 40)
  , name_size(250, 40)
  , score_size(60, 40)
{
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml",false);
  Point2i pos (0, 0);
  Font* big_font = Font::GetInstance(Font::FONT_BIG);

  // Center the boxes!
  uint x = 60;
  uint y = 60;

  //Team selection
  team_box = new HBox(Rectanglei(x, y, total_width, max_height), true);
  team_box->SetMargin(DEF_MARGIN);
  team_box->SetBorder(Point2i(DEF_BORDER, DEF_BORDER));

  bt_prev_team = new Button(Rectanglei(pos, Point2i(DEF_SIZE, DEF_SIZE)),
                            res, "menu/arrow-left");
  team_box->AddWidget(bt_prev_team);

  pos.SetValues(pos.GetX()+DEF_SIZE, pos.GetY());

  HBox* tmp_box = new HBox( Rectanglei(pos, team_size), true);
  team_logo = new PictureWidget( Rectanglei(0,0,48,48) );
  tmp_box->AddWidget(team_logo);

  pos.SetValues(pos.GetX()+team_logo->GetSizeX(),pos.GetY());
  team_name = new Label("", Rectanglei(pos, team_size-48), *big_font);
  tmp_box->AddWidget(team_name);

  team_box->AddWidget(tmp_box);
  pos.SetValues(pos.GetX()+team_size.GetX(), pos.GetY());
  bt_next_team = new Button(Rectanglei(pos, Point2i(DEF_SIZE, DEF_SIZE)),
                            res, "menu/arrow-right");
  team_box->AddWidget(bt_next_team);

  widgets.AddWidget(team_box);

  resource_manager.UnLoadXMLProfile(res);

  //Results
  most_violent = new ResultBox(Rectanglei(x, y+int(1.5*max_height), total_width, max_height),
                               true, _("Most violent"), *big_font,
                               type_size, name_size, score_size);
  widgets.AddWidget(most_violent);

  most_usefull = new ResultBox(Rectanglei(x, y+3*max_height, total_width, max_height),
                               true, _("Most useful"), *big_font,
                               type_size, name_size, score_size);
  widgets.AddWidget(most_usefull);

  most_useless = new ResultBox(Rectanglei(x, y+int(4.5*max_height), total_width, max_height),
                               true, _("Most useless"), *big_font,
                               type_size, name_size, score_size);
  widgets.AddWidget(most_useless);

  biggest_traitor = new ResultBox(Rectanglei(x, y+6*max_height, total_width, max_height),
                                  true, _("Most sold-out"), *big_font,
                                  type_size, name_size, score_size);
  widgets.AddWidget(biggest_traitor);

  SetResult(0);
}

ResultsMenu::~ResultsMenu()
{
}

void ResultsMenu::SetResult(int i)
{
  const Character* player = NULL;
  const TeamResults* res = NULL;
  std::string name;

  DrawBackground();
  b_ok->ForceRedraw();
  index = i;
  if (index < 0) index = results->size()-1;
  else if (index>(int)results->size()-1) index = 0;
  res = (*results)[index];
  assert(res);

  //Team header
  name = res->getTeamName();
  if (res->getTeamLogo() == NULL) {
    team_logo->SetNoSurface();
  }  else  {
    team_logo->SetSurface( *(res->getTeamLogo()) );
  }
  printf("Now result %i/%i: team '%s'\n",
         index, (int)results->size(), name.c_str());
  team_name->SetText(name);
  team_box->ForceRedraw();

  //Most violent
  player = res->getMostViolent();
  if(player)
    most_violent->SetResult(player->GetName(), player->GetMostDamage());
  else
    most_violent->SetResult(_("Nobody!"), 0);

  //Most usefull
  player = res->getMostUsefull();
  if(player)
    most_usefull->SetResult(player->GetName(), player->GetOtherDamage());
  else
    most_usefull->SetResult(_("Nobody!"), 0);

  //Most usefull
  player = res->getMostUseless();
  if(player)
    most_useless->SetResult(player->GetName(), player->GetOtherDamage());
  else
    most_useless->SetResult(_("Nobody!"), 0);

  // Biggest sold-out
  player = res->getBiggestTraitor();
  if(player)
    biggest_traitor->SetResult(player->GetName(), player->GetOwnDamage());
  else
    biggest_traitor->SetResult(_("Nobody!"), 0);
}

void ResultsMenu::OnClic(const Point2i &mousePosition, int button)
{
  if (bt_prev_team->Contains(mousePosition))
    SetResult(index-1);
  else if ( bt_next_team->Contains(mousePosition))
    SetResult(index+1);
}

void ResultsMenu::Draw(const Point2i &mousePosition)
{
}

