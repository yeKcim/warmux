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
 * Results menu
 *****************************************************************************/

#include "results_menu.h"
#ifdef _MSC_VER
#  include <algorithm>  //std::sort
#endif

#include "character/character.h"
#include "character/damage_stats.h"
#include "game/time.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "gui/button.h"
#include "gui/label.h"
#include "gui/box.h"
#include "gui/picture_widget.h"
#include "include/app.h"
#include "sound/jukebox.h"
#include "team/results.h"
#include "team/team.h"
#include "tool/debug.h"
#include "tool/i18n.h"
#include "tool/math_tools.h"
#include "tool/resource_manager.h"

#define DEF_MARGIN     16
#define DEF_BORDER      8
#define DEF_SIZE       32
#define LINE_THICKNESS  2

#define GRAPH_X        20
#define GRAPH_Y       500
#define GRAPH_W       400
#define GRAPH_H       200

const Point2i BorderSize(DEF_BORDER, DEF_BORDER);
const Vector2<double> Zoom(1.7321, 1.7321);
const Point2i DefSize(DEF_SIZE, DEF_SIZE);

class ResultBox : public HBox
{
private:
  Label *name_lbl;
  Label *score_lbl;
  PictureWidget *team_picture;
public:
  ResultBox(const Rectanglei &rect, bool _visible,
            const char* type_name,
            Font::font_size_t font_size,
            Font::font_style_t font_style,
            const Point2i& type_size,
            const Point2i& name_size,
            const Point2i& score_size);
  // Hopefully no need for ~ResultBox() as it automatically
  // destroy child widgets by inheriting from HBox
  void SetIntResult(const std::string& name, int score, const Surface& team_logo);
  void SetDoubleResult(const std::string& name, double score, const Surface& team_logo);
  void SetNoResult();
};

ResultBox::ResultBox(const Rectanglei &rect, bool _visible,
                     const char *type_name,
                     Font::font_size_t font_size,
                     Font::font_style_t font_style,
                     const Point2i& type_size,
                     const Point2i& name_size,
                     const Point2i& score_size)
  : HBox(rect, _visible)
{
  Point2i pos(0, 0);
  Point2i posZero(0,0);

  margin = DEF_MARGIN;
  border.SetValues(DEF_BORDER, DEF_BORDER);

  AddWidget(new Label(type_name, Rectanglei(pos, type_size), font_size, font_style));

  pos.SetValues(pos.GetX()+type_size.GetX(), pos.GetY());
  name_lbl = new Label("", Rectanglei(pos, name_size), font_size, font_style);
  AddWidget(name_lbl);

  pos.SetValues(pos.GetX()+name_size.GetX(), pos.GetY());
  score_lbl = new Label("", Rectanglei(pos, score_size), font_size, font_style);
  AddWidget(score_lbl);

  team_picture = new PictureWidget( Rectanglei(0,0,48,48) );
  AddWidget(team_picture);
}

void ResultBox::SetIntResult(const std::string& name, int score, const Surface& team_logo)
{
  char buffer[16];
  std::string copy_name(name);

  snprintf(buffer, 16, "%i", score);

  std::string score_str(buffer);

  name_lbl->SetText(copy_name);
  score_lbl->SetText(score_str);
  team_picture->SetSurface(team_logo);

  //ForceRedraw();
}

void ResultBox::SetDoubleResult(const std::string& name, double score, const Surface& team_logo)
{
  char buffer[16];
  std::string copy_name(name);

  snprintf(buffer, 16, "%.1f", score);

  std::string score_str(buffer);

  name_lbl->SetText(copy_name);
  score_lbl->SetText(score_str);
  team_picture->SetSurface(team_logo);

  //ForceRedraw();
}

void ResultBox::SetNoResult()
{
  name_lbl->SetText(_("Nobody!"));
  score_lbl->SetText("0");
  team_picture->SetNoSurface();

  //ForceRedraw();
}

//=========================================================

bool compareTeamResults(const TeamResults* a, const TeamResults* b)
{
  if (a->getTeam() == NULL)
    return false;
  if (b->getTeam() == NULL)
    return true;

  const Team* team_a = a->getTeam();
  const Team* team_b = b->getTeam();

  if (team_a->NbAliveCharacter() < team_b->NbAliveCharacter())
    return false;
  else if (team_a->NbAliveCharacter() > team_b->NbAliveCharacter())
    return true;

  // Same number of alive characters, compare left energy
  if (team_a->ReadEnergy() < team_b->ReadEnergy())
    return false;
  else if (team_a->ReadEnergy() > team_b->ReadEnergy())
    return true;

  // Same energy, propably 0, compare death_time
  return (a->GetDeathTime() > b->GetDeathTime());
}

//=========================================================

ResultsMenu::ResultsMenu(std::vector<TeamResults*>& v)
  : Menu("menu/bg_results", vOk)
  , results(v)
  , index(-1)
  , max_height(DEF_SIZE+3*DEF_BORDER)
  , team_size(360, 40)
  , type_size(200, 40)
  , name_size(150, 40)
  , score_size(40, 40)
{
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml",false);
  Point2i pos (0, 0);

  uint x = 20;
  uint y = 20;

  ComputeTeamsOrder();

  // And the winner is :
  if (first_team) {
    jukebox.Play("share","victory");

    winner_box = new VBox(Rectanglei(x, y, 240, 0), true);
    winner_box->AddWidget(new Label(_("Winner"), Rectanglei(0,0, 240,1), Font::FONT_BIG, Font::FONT_BOLD,
                                    white_color, true));
    PictureWidget* winner_logo = new PictureWidget( Rectanglei(0,0,64,64));
    winner_logo->SetSurface(first_team->GetBigFlag());
    winner_box->AddWidget(winner_logo);
    winner_box->AddWidget(new Label(first_team->GetName(), Rectanglei(0,0, 240,1), Font::FONT_BIG, Font::FONT_NORMAL,
                                    white_color, true));

    std::string tmp = _("Controlled by: ") + first_team->GetPlayerName();
    winner_box->AddWidget(new Label(tmp, Rectanglei(0,0, 240,1), Font::FONT_MEDIUM, Font::FONT_NORMAL,
                                    white_color, true));

    widgets.AddWidget(winner_box);
  }

  // Load the podium img
  podium_img = resource_manager.LoadImage(res, "menu/podium");

  x+=260;

  //Team selection
  team_box = new HBox(Rectanglei(x, y, 0, max_height), true);
  team_box->SetMargin(DEF_MARGIN);
  team_box->SetBorder(Point2i(DEF_BORDER, DEF_BORDER));

  bt_prev_team = new Button(Rectanglei(pos, Point2i(DEF_SIZE, DEF_SIZE)),
                            res, "menu/really_big_minus");
  team_box->AddWidget(bt_prev_team);

  pos.SetValues(pos.GetX()+DEF_SIZE, pos.GetY());

  HBox* tmp_box = new HBox( Rectanglei(pos, team_size), false);
  team_logo = new PictureWidget( Rectanglei(0,0,48,48) );
  tmp_box->AddWidget(team_logo);

  pos.SetValues(pos.GetX()+team_logo->GetSizeX(),pos.GetY());
  team_name = new Label("", Rectanglei(pos, team_size-48), Font::FONT_BIG, Font::FONT_NORMAL);
  tmp_box->AddWidget(team_name);

  team_box->AddWidget(tmp_box);
  pos.SetValues(pos.GetX()+team_size.GetX(), pos.GetY());
  bt_next_team = new Button(Rectanglei(pos, Point2i(DEF_SIZE, DEF_SIZE)),
                            res, "menu/really_big_plus");
  team_box->AddWidget(bt_next_team);

  widgets.AddWidget(team_box);

  resource_manager.UnLoadXMLProfile(res);

  //Results
  statistics_box = new VBox(Rectanglei(x, y+int(1.5*max_height), 510, 0), true);

  most_violent = new ResultBox(Rectanglei(0,0,0, max_height),
                               false, _("Most violent"), Font::FONT_BIG, Font::FONT_NORMAL,
                               type_size, name_size, score_size);
  statistics_box->AddWidget(most_violent);

  most_useful = new ResultBox(Rectanglei(0,0,0, max_height),
                               false, _("Most useful"), Font::FONT_BIG, Font::FONT_NORMAL,
                               type_size, name_size, score_size);
  statistics_box->AddWidget(most_useful);

  most_useless = new ResultBox(Rectanglei(0,0,0, max_height),
                               false, _("Most useless"), Font::FONT_BIG, Font::FONT_NORMAL,
                               type_size, name_size, score_size);
  statistics_box->AddWidget(most_useless);

  biggest_traitor = new ResultBox(Rectanglei(0,0,0, max_height),
                                  false, _("Most sold-out"), Font::FONT_BIG, Font::FONT_NORMAL,
                                  type_size, name_size, score_size);
  statistics_box->AddWidget(biggest_traitor);

  most_clumsy = new ResultBox(Rectanglei(0,0,0, max_height),
                              false, _("Most clumsy"), Font::FONT_BIG, Font::FONT_NORMAL,
                              type_size, name_size, score_size);
  statistics_box->AddWidget(most_clumsy);

  most_accurate = new ResultBox(Rectanglei(0,0,0, max_height),
                                false, _("Most accurate"), Font::FONT_BIG, Font::FONT_NORMAL,
                                type_size, name_size, score_size);
  statistics_box->AddWidget(most_accurate);

  widgets.AddWidget(statistics_box);

  // Label for graph axes
  //widgets.AddWidget(new Label(_("Time"), Rectanglei(GRAPH_X, GRAPH_Y+8, GRAPH_W, 32),
  //                            Font::FONT_SMALL, Font::FONT_BOLD, black_color, true, false));
}

ResultsMenu::~ResultsMenu()
{
}

void ResultsMenu::ComputeTeamsOrder()
{
  std::sort(results.begin(), results.end(), compareTeamResults);

  first_team = results.at(0)->getTeam();
  second_team = results.at(1)->getTeam();
  if (results.size() > 3)
    third_team = results.at(2)->getTeam();
  else
    third_team = NULL;
}

void ResultsMenu::DrawTeamOnPodium(const Team& team, const Point2i& podium_position,
                                   const Point2i& relative_position) const
{
  Point2i flag_pos(team.GetFlag().GetWidth()/2, team.GetFlag().GetHeight());
  Point2i position = podium_position + relative_position - flag_pos;

  Surface team_character(team.GetFlag());
  //team_character.Flip(); ==> Why does it not work ?

  AppWormux::GetInstance()->video->window.Blit(team_character, position);
}

void ResultsMenu::DrawPodium(const Point2i& position) const
{
  AppWormux::GetInstance()->video->window.Blit(podium_img, position);

  if (first_team)
    DrawTeamOnPodium(*first_team, position, Point2i(60,8));

  if (second_team)
    DrawTeamOnPodium(*second_team, position, Point2i(20,20));

  if (third_team)
    DrawTeamOnPodium(*third_team, position, Point2i(98,42));
}

void ResultsMenu::DrawTeamGraph(const Team *team,
                                int x, int y,
                                double duration_scale, double energy_scale,
                                const Color& color) const
{
  EnergyList::const_iterator it = team->energy.energy_list.begin(),
    end = team->energy.energy_list.end();

  MSG_DEBUG("menu", "Drawing graph for team %s\n", team->GetName().c_str());

  if (it == end)
    return;

  int sx = x+lround((*it)->GetDuration()*duration_scale)+LINE_THICKNESS,
    sy = y-lround((*it)->GetValue()*energy_scale);
  Surface &surface = AppWormux::GetInstance()->video->window;
  MSG_DEBUG("menu", "   First point: (%u,%u) -> (%i,%i)\n",
            (*it)->GetDuration(), (*it)->GetValue(), sx, sy);

  ++it;
  if (it == end)
    return;

  do 
  {
    int ex = x+lround((*it)->GetDuration()*duration_scale),
      ey = y-lround((*it)->GetValue()*energy_scale);

    MSG_DEBUG("menu", "   Next point: (%u,%u) -> (%i,%i)\n",
              (*it)->GetDuration(), (*it)->GetValue(), ex, ey);
    surface.BoxColor(Rectanglei(sx, sy, ex-sx, LINE_THICKNESS), color);
    surface.BoxColor(Rectanglei(ex, std::min(sy,ey), LINE_THICKNESS, abs(ey-sy)), color);

    sx = ex;
    sy = ey;
    ++it;
  } while (it != end);
}

void ResultsMenu::DrawGraph(int x, int y, int w, int h)
{
  // Value to determine normalization
  uint   max_value      = 0;
  double duration_scale = w / (1.1*Time::GetInstance()->Read());
  std::vector<TeamResults*>::const_iterator it;

  for (it=results.begin(); it!=results.end(); ++it)
  {
    const Team* team = (*it)->getTeam();
    if (team)
      if (team->energy.energy_list.GetMaxValue() > max_value)
      {
        max_value = team->energy.energy_list.GetMaxValue();
        MSG_DEBUG("menu", "New maximum value: %u\n", max_value);
      }
  }

  // Draw here the graph and stuff
  Surface &surface = AppWormux::GetInstance()->video->window;
  surface.BoxColor(Rectanglei(x, y, LINE_THICKNESS, h), black_color);
  surface.BoxColor(Rectanglei(x, y+h, w, LINE_THICKNESS), black_color);

  // Draw each team graph
  double energy_scale = h / (1.1*max_value);
  MSG_DEBUG("menu", "Scaling: %.1f (duration; %u) and %.1f\n",
            duration_scale, Time::GetInstance()->ReadDuration(), energy_scale);
#if 1
  static const Color clist[] =
    { white_color, primary_red_color, c_yellow, c_grey, green_color, black_color };
  uint   current_color = 0;
  for (it=results.begin(); it!=results.end(); ++it)
  {
    const Team* team = (*it)->getTeam();
    if (team)
    {
      DrawTeamGraph(team, x, y+h, duration_scale, energy_scale, clist[current_color]);
      current_color++;
    }
  }
#endif
}

void ResultsMenu::SetResult(int i)
{
  if (index == i)
    return;

  const Character* player = NULL;
  const TeamResults* res = NULL;
  std::string name;

  DrawBackground();
  b_ok->ForceRedraw();

  if (winner_box)
    winner_box->ForceRedraw();

  DrawPodium(Point2i(70,350));

  index = i;
  if (index < 0)
    index = results.size()-1;
  else if (index > (int)results.size()-1)
    index = 0;

  res = results.at(index);

  //Team header
  if (res->getTeam() == NULL) {
    name = _("All teams");
    team_logo->SetNoSurface();
  } else {
    name = res->getTeam()->GetName()+" - "+res->getTeam()->GetPlayerName();
    team_logo->SetSurface(res->getTeam()->GetFlag());
  }

  team_name->SetText(name);
  team_box->ForceRedraw();

  //Most violent
  player = res->getMostViolent();
  if (player)
    most_violent->SetIntResult(player->GetName(), player->GetDamageStats()->GetMostDamage(), player->GetTeam().GetFlag());
  else
    most_violent->SetNoResult();

  //Most useful
  player = res->getMostUseful();
  if (player)
    most_useful->SetIntResult(player->GetName(), player->GetDamageStats()->GetOthersDamage(), player->GetTeam().GetFlag());
  else
    most_useful->SetNoResult();

  //Most useless
  player = res->getMostUseless();
  if (player)
    most_useless->SetIntResult(player->GetName(), player->GetDamageStats()->GetOthersDamage(), player->GetTeam().GetFlag());
  else
    most_useless->SetNoResult();

  // Biggest sold-out
  player = res->getBiggestTraitor();
  if (player)
    biggest_traitor->SetIntResult(player->GetName(), player->GetDamageStats()->GetFriendlyFireDamage(), player->GetTeam().GetFlag());
  else
    biggest_traitor->SetNoResult();

  // Most clumsy
  player = res->getMostClumsy();
  if (player)
    most_clumsy->SetIntResult(player->GetName(), player->GetDamageStats()->GetItselfDamage(), player->GetTeam().GetFlag());
  else
    most_clumsy->SetNoResult();

  // Most accurate
  player = res->getMostAccurate();
  if (player)
    most_accurate->SetDoubleResult(player->GetName(), player->GetDamageStats()->GetAccuracy(), player->GetTeam(). GetFlag());
  else
    most_accurate->SetNoResult();

  statistics_box->ForceRedraw();
}

void ResultsMenu::OnClickUp(const Point2i &mousePosition, int button)
{
  if (team_box->Contains(mousePosition)) {

    if (button == SDL_BUTTON_WHEELDOWN ||
        (button == SDL_BUTTON_LEFT && bt_prev_team->Contains(mousePosition)))
      SetResult(index-1);
    else if (button == SDL_BUTTON_WHEELUP ||
             (button == SDL_BUTTON_LEFT && bt_next_team->Contains(mousePosition)))
      SetResult(index+1);
  }
}

void ResultsMenu::OnClick(const Point2i &/*mousePosition*/, int /*button*/)
{
  // Do nothing if user has not released the button
}

void ResultsMenu::Draw(const Point2i &/*mousePosition*/)
{
  if (index == -1)
    SetResult(results.size()-1);
  // Far from rendering properly
  //DrawGraph(GRAPH_X, GRAPH_Y, GRAPH_W, GRAPH_H);
}
