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
 * Results menu
 *****************************************************************************/

#include "menu/results_menu.h"
#include <algorithm>  //std::sort

#include "character/character.h"
#include "character/damage_stats.h"
#include "game/time.h"
#include "graphic/font.h"
#include "graphic/sprite.h"
#include "graphic/text.h"
#include "graphic/video.h"
#include "gui/box.h"
#include "gui/button.h"
#include "gui/label.h"
#include "gui/list_box.h"
#include "gui/null_widget.h"
#include "gui/picture_widget.h"
#include "gui/tabs.h"
#include "gui/talk_box.h"
#include "include/app.h"
#include "include/action_handler.h"
#include "network/network.h"
#include "sound/jukebox.h"
#include "team/results.h"
#include "team/team.h"
#include "tool/debug.h"
#include "tool/i18n.h"
#include "tool/math_tools.h"
#include "tool/resource_manager.h"

#define DEF_BORDER      8
#define DEF_SIZE       32
#define LINE_THICKNESS  2

#define GRAPH_BORDER        20
#define GRAPH_START_Y       400

static const Point2i BorderSize(DEF_BORDER, DEF_BORDER);
static const Point2i DefSize(DEF_SIZE, DEF_SIZE);

class ResultBox : public HBox
{
  void SetWidgets(uint size, const std::string& type, const char* buffer, const Character* player)
  {
    margin = DEF_BORDER;
    border = BorderSize;
    size -= 4*DEF_BORDER + 40;
    // Should resize more depending on font size
    Font::font_size_t font = (size > 400) ? Font::FONT_BIG : Font::FONT_MEDIUM;
    //printf("Size=%u\n", size);

    AddWidget(new Label(type, (size*TypeW)/TotalW, font, Font::FONT_NORMAL));

    AddWidget(new Label((player) ? player->GetName() : _("Nobody!"),
                        (size*NameW)/TotalW, font, Font::FONT_NORMAL));

    std::string score_str(buffer);
    AddWidget(new Label(score_str, (size*ScoreW)/TotalW, font, Font::FONT_NORMAL));

    if (player)
    {
      PictureWidget *team_picture = new PictureWidget(DefSize);
      team_picture->SetSurface(player->GetTeam().GetFlag());
      AddWidget(team_picture);
    }
    else
    {
      AddWidget(new NullWidget(DefSize));
    }
  }
public:
  // Label widthes and font sizes should be inferred from the resolution
  static const uint TypeW  = 180;
  static const uint NameW  = 160;
  static const uint ScoreW = 50;
  static const uint TotalW = TypeW + NameW + ScoreW;

  ResultBox(uint size, const std::string& type)
    : HBox(W_UNDEF, false, false)
  {
    SetWidgets(size, type, "?", NULL);
  }
  ResultBox(uint size, const std::string& type, uint score, const Character* player)
    : HBox(W_UNDEF, false, false)
  {
    char buffer[16];
    snprintf(buffer, 16, "%i", score);
    SetWidgets(size, type, buffer, player);
  }
  ResultBox(uint size, const std::string& type, double score, const Character* player)
    : HBox(W_UNDEF, false, false)
  {
    char buffer[16];
    if (score+0.05<100.0) snprintf(buffer, 16, "%.1f", score);
    else                  snprintf(buffer, 16, "%.0f", score);
    SetWidgets(size, type, buffer, player);
  }
  void Draw(const Point2i &mousePosition) const
  {
    HBox::Draw(mousePosition);
    for (std::list<Widget*>::const_iterator it = widget_list.begin(); it != widget_list.end(); ++it)
      (*it)->Draw(mousePosition);
  }
};

class ResultListBox : public BaseListBox
{
public:
  ResultListBox(const TeamResults* res, const Point2i &size, bool force = true)
    : BaseListBox(size, force)
  {
    ResultBox       *box;
    const Character *player = res->getMostViolent();

    //Most violent
    if (player)
      box = new ResultBox(size.x, _("Most violent"), player->GetDamageStats()->GetMostDamage(), player);
    else
      box = new ResultBox(size.x, _("Most violent"));
    AddWidgetItem(false, box);

    //Most useful
    player = res->getMostUseful();
    if (player)
      box = new ResultBox(size.x, _("Most useful"), player->GetDamageStats()->GetOthersDamage(), player);
    else
      box = new ResultBox(size.x, _("Most useful"));
    AddWidgetItem(false, box);

    //Most useless
    player = res->getMostUseless();
    if (player)
      box = new ResultBox(size.x, _("Most useless"), player->GetDamageStats()->GetOthersDamage(), player);
    else
      box = new ResultBox(size.x, _("Most useless"));
    AddWidgetItem(false, box);

    // Biggest sold-out
    player = res->getBiggestTraitor();
    if (player)
      box = new ResultBox(size.x, _("Most sold-out"), player->GetDamageStats()->GetFriendlyFireDamage(), player);
    else
      box = new ResultBox(size.x, _("Most sold-out"));
    AddWidgetItem(false, box);

    // Most clumsy
    player = res->getMostClumsy();
    if (player)
      box = new ResultBox(size.x, _("Clumsiest"), player->GetDamageStats()->GetItselfDamage(), player);
    else
      box = new ResultBox(size.x, _("Clumsiest"));
    AddWidgetItem(false, box);

    // Most accurate
    player = res->getMostAccurate();
    if (player)
      box = new ResultBox(size.x, _("Most accurate"), player->GetDamageStats()->GetAccuracy(), player);
    else
      box = new ResultBox(size.x, _("Most accurate"));
    AddWidgetItem(false, box);
  }
};


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

  // Same energy, probably 0, compare death_time
  return (a->GetDeathTime() > b->GetDeathTime());
}

//=========================================================

class CanvasTeamsGraph : public Widget
{
private:
  std::vector<TeamResults*>& results;

public:
  CanvasTeamsGraph(const Point2i& size,
		   std::vector<TeamResults*>& results);
  virtual ~CanvasTeamsGraph() {};
  virtual void Draw(const Point2i&) const;

  virtual void DrawTeamGraph(const Team *team,
			     int x, int y,
			     double duration_scale,
			     double energy_scale,
                             uint   max_duration,
			     const Color& color) const;
  virtual void DrawGraph(int x, int y, int w, int h) const;

  virtual void Pack() {};
};

CanvasTeamsGraph::CanvasTeamsGraph(const Point2i& size,
				   std::vector<TeamResults*>& _results) :
  Widget(size), results(_results)
{}

void CanvasTeamsGraph::Draw(const Point2i& /*mousePosition*/) const
{
  DrawGraph(position.x+DEF_BORDER, position.y+DEF_BORDER,
            size.x-2*DEF_BORDER, size.y-2*DEF_BORDER);
}

void CanvasTeamsGraph::DrawTeamGraph(const Team *team,
				     int x, int y,
				     double duration_scale,
				     double energy_scale,
                                     uint   max_duration,
				     const Color& color) const
{
  EnergyList::const_iterator it = team->energy.energy_list.begin(),
    end = team->energy.energy_list.end();

  MSG_DEBUG("menu", "Drawing graph for team %s\n", team->GetName().c_str());

  if (it == end)
    return;

  int sx = x+lround((*it)->GetDuration()*duration_scale)+LINE_THICKNESS,
    sy = y-lround((*it)->GetValue()*energy_scale);
  Surface &surface = GetMainWindow();
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

  // Missing point
  --it;
  if ((*it)->GetDuration() < max_duration)
  {
    surface.BoxColor(Rectanglei(sx, sy, x+lround(max_duration*duration_scale)-sx, LINE_THICKNESS), color);
  }
}

void CanvasTeamsGraph::DrawGraph(int x, int y, int w, int h) const
{
  // Value to determine normalization
  uint   max_value      = 0;
  uint   max_duration   = 0;
  uint   graph_h        = h-32;
  uint   graph_w        = w-32;
  uint   graph_x        = x+32;
  std::vector<TeamResults*>::const_iterator it;

  for (it=results.begin(); it!=results.end(); ++it)
  {
    const Team* team = (*it)->getTeam();
    if (team)
    {
      if (team->energy.energy_list.GetMaxValue() > max_value)
        max_value = team->energy.energy_list.GetMaxValue();
      if (team->energy.energy_list.GetDuration() > max_duration)
        max_duration = team->energy.energy_list.GetDuration();
    }
  }

  // Draw here the graph and stuff
  Surface &surface = GetMainWindow();
  surface.BoxColor(Rectanglei(graph_x, y, LINE_THICKNESS, graph_h), black_color);
  surface.BoxColor(Rectanglei(graph_x, y+graph_h, graph_w, LINE_THICKNESS), black_color);
  //DrawTmpBoxText(Font::GetInstance()->, Point2i(w/2, y+graph_h+8), _("Time"), 0);
  surface.Blit(Font::GetInstance(Font::FONT_MEDIUM, Font::FONT_BOLD)->CreateSurface(_("Time"), black_color),
               Point2i(graph_x+graph_w/2, y+graph_h+8));
  surface.Blit(Font::GetInstance(Font::FONT_MEDIUM, Font::FONT_BOLD)->CreateSurface(_("Energy"), black_color).RotoZoom(M_PI/2, 1.0, 1.0, false),
               Point2i(x+4, graph_h/2));
  char buffer[16];
  snprintf(buffer, 16, "%.1f", max_duration/1000.0);
  surface.Blit(Font::GetInstance(Font::FONT_MEDIUM, Font::FONT_BOLD)->CreateSurface(buffer, black_color),
               Point2i(x+graph_w-20, y+graph_h+8));

  // Draw each team graph
  double energy_scale = graph_h / (1.05*max_value);
  double duration_scale = graph_w / (1.05*max_duration);
  MSG_DEBUG("menu", "Scaling: %.1f (duration; %u) and %.1f\n",
            duration_scale, Time::GetInstance()->ReadDuration(), energy_scale);

  uint               index   = 0;
  static const Color clist[] =
    { black_color, primary_red_color, gray_color, primary_green_color, black_color, primary_blue_color };
  for (it=results.begin(); it!=results.end(); ++it)
  {
    const Team* team = (*it)->getTeam();
    if (team)
    {
      // Legend line
      surface.BoxColor(Rectanglei(x+w-112, y+12+index*40,
                                  56, LINE_THICKNESS), clist[index]);
      // Legend icon
      surface.Blit(team->GetFlag(), Point2i(x+w-48, y+12+index*40-20));
      DrawTeamGraph(team, graph_x, y+graph_h, duration_scale, energy_scale, max_duration, clist[index]);
      index++;
    }
  }
}

//=========================================================

ResultsMenu::ResultsMenu(std::vector<TeamResults*>& v, bool disconnected)
  : Menu("menu/bg_results", vOk)
  , results(v)
  , first_team(NULL)
  , second_team(NULL)
  , third_team(NULL)
  , msg_box(NULL)
  , winner_box(NULL)
{
  Profile *res = resource_manager.LoadXMLProfile("graphism.xml", false);
  uint x = 20;
  uint y = 20;

  if (!disconnected)
    ComputeTeamsOrder();

  // And the winner is :
  if (first_team) {
    JukeBox::GetInstance()->Play("share","victory");

    winner_box = new VBox(240, true);
    winner_box->AddWidget(new Label(_("Winner"), 240, Font::FONT_BIG, Font::FONT_BOLD,
                                    white_color, true));
    PictureWidget* winner_logo = new PictureWidget(Point2i(64, 64));
    winner_logo->SetSurface(first_team->GetBigFlag());
    winner_box->AddWidget(winner_logo);
    winner_box->AddWidget(new Label(first_team->GetName(), 240, Font::FONT_BIG, Font::FONT_NORMAL,
                                    white_color, true));

    std::string tmp = _("Controlled by: ") + first_team->GetPlayerName();
    winner_box->AddWidget(new Label(tmp, 240, Font::FONT_MEDIUM, Font::FONT_NORMAL,
                                    white_color, true));

    winner_box->SetPosition(x, y);
    widgets.AddWidget(winner_box);
    widgets.Pack();
  }

  // Load the podium img
  podium_img = resource_manager.LoadImage(res, "menu/podium");
  resource_manager.UnLoadXMLProfile(res);

  x+=260;
  const Point2i& wsize = GetMainWindow().GetSize();

  Point2i tab_size = wsize - Point2i(x+16, y+70);

  VBox* tmp_box = new VBox(tab_size.x, false, false);
  tmp_box->SetNoBorder();

  // Are we in network ? yes, so display a talkbox
  if (Network::IsConnected()) {
    msg_box = new TalkBox(Point2i(tab_size.x, 120), Font::FONT_SMALL, Font::FONT_NORMAL);
    tab_size.y -= 125;
  }

  tabs = new MultiTabs(tab_size);

  // Create tabs for each team result
  stats = new MultiTabs(tab_size - 2*BorderSize);
  stats->SetMaxVisibleTabs(1);
  for (uint i=0; i<v.size(); i++)
  {
    const Team* team = v[i]->getTeam();
    const char* name = (team) ? team->GetName().c_str() : _("All teams");
    stats->AddNewTab(name, name, new ResultListBox(v[i], tab_size - 4*BorderSize));
  }
  tabs->AddNewTab("TAB_team", _("Team stats"), stats);

  tabs->AddNewTab("TAB_canvas", _("Team graphs"),
                  new CanvasTeamsGraph(tab_size - 2*BorderSize, results));

  tmp_box->AddWidget(tabs);

  if (msg_box != NULL) {
    tmp_box->AddWidget(msg_box);
  }
  tmp_box->SetPosition(x, y);

  widgets.AddWidget(tmp_box);
  widgets.Pack();
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

  GetMainWindow().Blit(team_character, position);
}

void ResultsMenu::DrawPodium(const Point2i& position) const
{
  GetMainWindow().Blit(podium_img, position);

  if (first_team)
    DrawTeamOnPodium(*first_team, position, Point2i(60,8));

  if (second_team)
    DrawTeamOnPodium(*second_team, position, Point2i(20,20));

  if (third_team)
    DrawTeamOnPodium(*third_team, position, Point2i(98,42));
}

void ResultsMenu::OnClickUp(const Point2i &mousePosition, int button)
{
  widgets.ClickUp(mousePosition, button);
}

void ResultsMenu::OnClick(const Point2i &mousePosition, int button)
{
  // Do nothing if user has not released the button
  widgets.Click(mousePosition, button);
}

void ResultsMenu::key_ok()
{
  // return was pressed while chat texbox still had focus (player wants to send his msg)
  if (msg_box != NULL && msg_box->TextHasFocus())
  {
    msg_box->SendChatMsg();
    return;
  }
  Menu::key_ok();
}

void ResultsMenu::Draw(const Point2i &/*mousePosition*/)
{
  DrawPodium(Point2i(70,250));

  if (Network::IsConnected()) {
    ActionHandler * action_handler = ActionHandler::GetInstance();
    action_handler->ExecActions();
  }
}

void ResultsMenu::ReceiveMsgCallback(const std::string& msg)
{
  if (msg_box) {
    msg_box->NewMessage(msg);
  }
}
