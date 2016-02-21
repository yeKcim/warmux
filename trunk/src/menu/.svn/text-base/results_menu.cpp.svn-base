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
 * Results menu
 *****************************************************************************/

#include <algorithm>  //std::sort
#include <time.h>

#include <WARMUX_debug.h>
#include <WARMUX_download.h>

#include "menu/results_menu.h"

#include "character/character.h"
#include "character/damage_stats.h"
#include "game/config.h"
#include "game/game_time.h"
#include "graphic/font.h"
#include "graphic/sprite.h"
#include "graphic/text.h"
#include "graphic/video.h"
#include "gui/box.h"
#include "gui/button.h"
#include "gui/figure_widget.h"
#include "gui/file_list_box.h"
#include "gui/graph_canvas.h"
#include "gui/label.h"
#include "gui/null_widget.h"
#include "gui/question.h"
#include "gui/scroll_box.h"
#include "gui/social_panel.h"
#include "gui/tabs.h"
#include "gui/talk_box.h"
#include "gui/text_box.h"
#include "include/app.h"
#include "include/action_handler.h"
#include "network/network.h"
#include "replay/replay.h"
#include "sound/jukebox.h"
#include "team/results.h"
#include "team/team.h"
#include "tool/ansi_convert.h"
#include "tool/math_tools.h"
#include "tool/resource_manager.h"
#include "tool/string_tools.h"

#define DEF_BORDER      8
#define DEF_SIZE       32
#define LINE_THICKNESS  2

#define GRAPH_BORDER        20
#define GRAPH_START_Y       400

static const Point2i DefSize(DEF_SIZE, DEF_SIZE);

#define REPLAY_ID "TAB_replay"

class ResultBox : public HBox
{
  Label  *category;
  Label  *character;
  Label  *score;
  Widget *img;
  std::string score_str;

  void SetWidgets(const std::string& type, const char* buffer, const Character* player)
  {
    margin = DEF_BORDER;
    border_size = DEF_BORDER;

    Font::font_size_t font = Font::FONT_SMALL;

    category = new Label(type, W_UNDEF, font, Font::FONT_BOLD);
    AddWidget(category);
    character = new Label((player) ? player->GetName() : _("Nobody!"),
                          W_UNDEF, font, Font::FONT_BOLD);
    AddWidget(character);

    score_str = buffer;
    score = new Label(score_str, 30, font, Font::FONT_BOLD);
    AddWidget(score);

    if (player) {
      PictureWidget *team_picture = new PictureWidget(DefSize);
      team_picture->SetSurface(player->GetTeam().GetFlag());
      AddWidget(team_picture);
      img = team_picture;
    } else {
      img = new NullWidget(DefSize);
      AddWidget(img);
    }
  }

public:
  ResultBox(uint size, const std::string& type)
    : HBox(size, false, false, false)
  {
    SetWidgets(type, "?", NULL);
    Widget::SetBackgroundColor(transparent_color);
  }
  ResultBox(uint size, const std::string& type, uint score, const Character* player)
    : HBox(size, false, false, false)
  {
    char buffer[16];
    snprintf(buffer, 16, "%i", score);
    SetWidgets(type, buffer, player);
  }
  ResultBox(uint size, const std::string& type, float score, const Character* player)
    : HBox(size, false, false, false)
  {
    char buffer[16];
    snprintf(buffer, 16, "%.1f", score);
    SetWidgets(type, buffer, player);
  }
  void Pack()
  {
    int width = size.x - (4*margin + img->GetSizeX() + score->GetSizeX());
    category->SetSizeX(width/2);
    character->SetSizeX(width/2);

    HBox::Pack();
  }
};

class ResultListBox : public ScrollBox
{
public:
  ResultListBox(const TeamResults* res, const Point2i &size)
    : ScrollBox(size)
  {
    ResultBox       *box;
    const Character *player = res->getMostViolent();

    //Most violent
    if (player)
      box = new ResultBox(size.x, _("Most violent"), player->GetDamageStats()->GetMostDamage(), player);
    else
      box = new ResultBox(size.x, _("Most violent"));
    AddWidget(box);

    //Most useful
    player = res->getMostUseful();
    if (player)
      box = new ResultBox(size.x, _("Most useful"), player->GetDamageStats()->GetOthersDamage(), player);
    else
      box = new ResultBox(size.x, _("Most useful"));
    AddWidget(box);

    //Most useless
    player = res->getMostUseless();
    if (player)
      box = new ResultBox(size.x, _("Most useless"), player->GetDamageStats()->GetOthersDamage(), player);
    else
      box = new ResultBox(size.x, _("Most useless"));
    AddWidget(box);

    // Biggest sold-out
    player = res->getBiggestTraitor();
    if (player)
      box = new ResultBox(size.x, _("Most sold-out"), player->GetDamageStats()->GetFriendlyFireDamage(), player);
    else
      box = new ResultBox(size.x, _("Most sold-out"));
    AddWidget(box);

    // Most clumsy
    player = res->getMostClumsy();
    if (player)
      box = new ResultBox(size.x, _("Clumsiest"), player->GetDamageStats()->GetItselfDamage(), player);
    else
      box = new ResultBox(size.x, _("Clumsiest"));
    AddWidget(box);

    // Most accurate
    player = res->getMostAccurate();
    if (player)
      box = new ResultBox(size.x, _("Most accurate"), player->GetDamageStats()->GetAccuracy(), player);
    else
      box = new ResultBox(size.x, _("Most accurate"));
    AddWidget(box);
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

static bool IsPodiumSeparate()
{
  return GetMainWindow().GetSize() >= Point2i(640, 480);
}

ResultsMenu::ResultsMenu(std::vector<TeamResults*>& v, bool disconnected)
  : Menu("menu/bg_results", vOkCancel)
  , results(v)
  , first_team(NULL)
  , second_team(NULL)
  , third_team(NULL)
  , msg_box(NULL)
  , winner_box(NULL)
  , save(NULL)
{
  Profile *res  = GetResourceManager().LoadXMLProfile("graphism.xml", false);
  Point2i wsize = GetMainWindow().GetSize();
  bool    small = !IsPodiumSeparate();
  uint x        = wsize.GetX() * 0.02f;
  uint tab_x    = small ? x : 260+16+x;
  uint y        = wsize.GetY() * 0.02f;

  // Not showing it yet
  b_cancel->SetVisible(false);

  if (!disconnected)
    ComputeTeamsOrder();

  // Load the podium img
  podium_img = GetResourceManager().LoadImage(res, "menu/podium");
  podium_img.SetAlpha(0, 0);

  Point2i tab_size = wsize - Point2i(tab_x + x, y+actions_buttons->GetSizeY());

  // Are we in network ? yes, so display a talkbox
  if (Network::IsConnected()) {
    int talk_size = tab_size.y/4;
    msg_box = new TalkBox(Point2i(tab_size.x, talk_size), Font::FONT_SMALL, Font::FONT_BOLD);
    tab_size.y -= talk_size;
  }

  tabs = new MultiTabs(tab_size);

  // Podium, drawn as first tab
  if (second_team)
    DrawTeamOnPodium(*second_team, Point2i(20,30));

  if (third_team)
    DrawTeamOnPodium(*third_team, Point2i(98,52));

  winner_box = new VBox(240, false, true, true);
  winner_box->SetNoBorder();
  if (first_team) {
    Font::font_size_t title = (small) ? Font::FONT_MEDIUM : Font::FONT_BIG;
    Font::font_size_t txt   = (small) ? Font::FONT_SMALL : Font::FONT_MEDIUM;
    JukeBox::GetInstance()->Play("default", "victory");

    DrawTeamOnPodium(*first_team, Point2i(60,20));

    winner_box->AddWidget(new Label(_("Winner"), 240, title, Font::FONT_BOLD,
                                    dark_gray_color, Text::ALIGN_CENTER_TOP));
    PictureWidget* winner_logo = new PictureWidget(Point2i(64, 64)/(small+1));
    winner_logo->SetSurface(first_team->GetFlag());
    winner_box->AddWidget(winner_logo);
    winner_box->AddWidget(new Label(first_team->GetName(), 240, title, Font::FONT_BOLD,
                                    dark_gray_color, Text::ALIGN_CENTER_TOP));

    std::string tmp = _("Controlled by: ") + first_team->GetPlayerName();
    winner_box->AddWidget(new Label(tmp, 240, txt, Font::FONT_BOLD,
                                    dark_gray_color, Text::ALIGN_CENTER_TOP));
  }

  podium_img.SetAlpha(SDL_SRCALPHA, 0);
  podium = new PictureWidget(podium_img);
  winner_box->AddWidget(podium);
  if (IsPodiumSeparate()) {
    winner_box->SetPosition(x, y);
    widgets.AddWidget(winner_box);
    tabs->SetPosition(tab_x, y);
  } else {
    tabs->AddNewTab("TAB_podium", _("Podium"), winner_box);
  }

  // Create tabs for each team result
  stats = new MultiTabs(tab_size - 2*DEF_BORDER);
  stats->SetMaxVisibleTabs(1);
  stats->SetNoBorder();
  for (uint i=0; i<v.size(); i++) {
    const Team* team = v[i]->getTeam();
    const std::string name = (team) ? team->GetName() : _("All teams");
    stats->AddNewTab(name, name, new ResultListBox(v[i], tab_size - 4*DEF_BORDER));
  }
  tabs->AddNewTab("TAB_team", _("Team stats"), stats);

  // Convert TeamResults to GraphCanvas::Results
  std::vector<GraphCanvas::Result> team_results;
  for (std::vector<TeamResults*>::const_iterator it = results.begin(); it != results.end(); ++it) {
    const Team* team = (*it)->getTeam();
    if (team) {
      const EnergyList& list = team->energy.energy_list;

      uint duration = list.GetDuration();
      if (duration) {
        GraphCanvas::Result r;
        r.ymax = list.GetMaxValue();
        r.xmax = duration*0.001f;
        r.color = team->GetColor();
        r.item = &team->GetFlag();
        for (uint i=0; i<list.size(); i++)
          r.list.push_back(std::make_pair(list[i]->GetDuration()*0.001f, list[i]->GetValue()));
        team_results.push_back(r);
      }
    }
  }
  tabs->AddNewTab("TAB_canvas", _("Team graphs"),
                  new GraphCanvas(tab_size - 2*DEF_BORDER, _("Time"), _("Energy"), team_results));

  // Save replay tab
  if (Replay::GetInstance()->IsRecording()) {
#define BOX_HEIGHT 24
#define LABEL_SIZE 88
#define BORDER      5
    VBox* vbox  = new VBox(tab_size.x-2*BORDER, false, false); // Keep the 5,5 border
    save = new Button(res, "menu/save", false); save->SetBorder(defaultOptionColorRect, 2);
    HBox* hbox  = new HBox(save->GetSizeY(), false, false); hbox->SetNoBorder();
    hbox->AddWidget(save);

    VBox* vbox2 = new VBox(tab_size.x - 3*BORDER - save->GetSizeX(), false, false); vbox2->SetNoBorder();

    HBox* hbox2 = new HBox(BOX_HEIGHT, false, false); hbox2->SetNoBorder();
    hbox2->AddWidget(new Label(_("Filename:"), LABEL_SIZE, Font::FONT_MEDIUM));
    char buffer[32];
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime (&rawtime);
    strftime(buffer, sizeof(buffer), "Record %Y-%m-%d %HH%Mm%S.wrf", timeinfo);
    replay_name = new TextBox(buffer, tab_size.x -6*BORDER -LABEL_SIZE -save->GetSizeX());
    hbox2->AddWidget(replay_name);
    vbox2->AddWidget(hbox2);

    hbox2 = new HBox(BOX_HEIGHT, false, false); hbox2->SetNoBorder();
    hbox2->AddWidget(new Label(_("Comment:"), LABEL_SIZE, Font::FONT_MEDIUM));
    comment = new TextBox(_("I like WarMUX :)"), tab_size.x -6*BORDER -LABEL_SIZE -save->GetSizeX());
    hbox2->AddWidget(comment);
    vbox2->AddWidget(hbox2);
    hbox->AddWidget(vbox2);
    vbox->AddWidget(hbox);
    folders = new FileListBox(Point2i(tab_size.x-2*BORDER,
                              tab_size.y -tabs->GetHeaderHeight() -4*BORDER -hbox->GetSizeY()));
    // Windows may use SFN => uppercase extensions
    folders->AddExtensionFilter("WRF");
    folders->AddExtensionFilter("wrf");
    folders->StartListing();
    vbox->AddWidget(folders);
    tabs->AddNewTab(REPLAY_ID, _("Save replay?"), vbox);
  }

#if defined(HAVE_FACEBOOK) || defined(HAVE_TWITTER)
  social_panel = new SocialPanel(tab_size.x-2*BORDER, (tab_size.x-2*BORDER)/720.0f, false);
  tabs->AddNewTab("unused", _("Social"), social_panel);
#endif

  // Final box
  VBox *vbox = new VBox(tab_size.x, false, false, false);
  vbox->SetNoBorder();
  vbox->AddWidget(tabs);

  if (msg_box) {
    vbox->AddWidget(msg_box);
  }
  vbox->SetPosition(tab_x, y);

  widgets.AddWidget(vbox);
  widgets.Pack();
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

void ResultsMenu::DrawTeamOnPodium(const Team& team, const Point2i& relative_position)
{
  Point2i flag_pos = team.GetFlag().GetSize()/2;
  Point2i position = relative_position - flag_pos;

  Surface tmp(team.GetFlag());
  podium_img.MergeSurface(tmp, position);
}

#if defined(HAVE_FACEBOOK) || defined(HAVE_TWITTER)
std::string ResultsMenu::StringMessagge()
{
  std::vector<std::string> remote;
  std::string              local;
  uint rank = results.size(), max = 0, nb = 0;
  uint others = 0, self = 0;
  // There's a dummy team inserted for best of game
  for (uint i=0; i<results.size()-1; i++) {
    const Team *team = results.at(i)->getTeam();
    if (team->IsRemote()) {
      remote.push_back(team->GetPlayerName());
      others += team->GetNbCharacters();
    } else {
      if (i < rank) {
        local = team->GetPlayerName();
        rank = i;
      }
      nb++;
      self += team->GetNbCharacters();
    }
    if (results.at(i)->GetDeathTime() > max)
      max = results.at(i)->GetDeathTime();
  }
  std::string l;
  if (remote.empty())
    l = _("Himself");
  else if (remote.size() == 1)
    l = remote[0];
  else {
    for (uint i=0; i<remote.size()-1; i++)
      l += Format("%s, ", remote[i].c_str());
    l += Format("and %s", remote.back().c_str());
  }
  return Format("%s played a game of WarMUX, using %u characters in %u teams, against %s for %.1fs and reached rank %u.",
                local.c_str(), self, nb, l.c_str(), max/1000.0f, rank+1);
}
#endif

#ifdef HAVE_FACEBOOK
void ResultsMenu::Facepalm(const std::string& email, const std::string& pwd)
{
  Downloader* dl = Downloader::GetInstance();
  std::string msg; 
  if (dl->FacebookLogin(email, pwd)) {
    std::string txt = StringMessagge();

    if (dl->FacebookStatus(txt))
      msg = Format(_("*** Published: %s"), txt.c_str());
    else
      msg = Format(_("*** Publishing failed: %s"), dl->GetLastError().c_str());
  } else
    msg = Format(_("*** Publishing failed: %s"), dl->GetLastError().c_str());
  msg_box->NewMessage(msg, c_red);
  RedrawMenu();
}
#endif

#ifdef HAVE_TWITTER
void ResultsMenu::Tweet(const std::string& email, const std::string& pwd)
{
  Downloader* dl = Downloader::GetInstance();
  std::string msg; 
  if (dl->TwitterLogin(email, pwd)) {
    std::string txt = StringMessagge();

    if (dl->Tweet(txt))
      msg = Format(_("*** Tweeted: %s"), txt.c_str());
    else
      msg = Format(_("*** Tweet failed: %s"), dl->GetLastError().c_str());
  } else
    msg = Format(_("*** Tweet failed: %s"), dl->GetLastError().c_str());
  msg_box->NewMessage(msg, c_red);
  RedrawMenu();
}
#endif

bool ResultsMenu::SaveReplay()
{
  std::string filename = replay_name->GetText();
  if (filename.empty() || filename == "") {
    Question question(Question::WARNING);
    question.Set(_("Invalid filename"), true, 0);
    question.Ask();
    return false;
  }

#  ifdef _WIN32
  std::string name = UTF8ToUTF16(folders->GetCurrentFolder(), filename);
#  else
  std::string name = folders->GetCurrentFolder() + filename;
#endif
  if (!Replay::GetInstance()->SaveReplay(name, comment->GetText().c_str())) {
    Question question(Question::WARNING);
    question.Set(_("Failed to save replay"), true, 0);
    question.Ask();
    return false;
  }

  folders->StartListing(folders->GetCurrentFolder().c_str());
  Question question(Question::NO_TYPE);
  question.Set(_("Replay saved"), true, 0);
  question.Ask();

  return true;
}

bool ResultsMenu::signal_ok()
{
  if (tabs->GetCurrentTabId() == REPLAY_ID)
    return SaveReplay();
  return true;
}

void ResultsMenu::key_ok()
{
  // return was pressed while chat texbox still had focus (player wants to send his msg)
  if (msg_box && msg_box->TextHasFocus()) {

#if defined(HAVE_FACEBOOK) || defined(HAVE_TWITTER)
    Config  *cfg      = Config::GetInstance();
    TextBox *text_box = msg_box->GetTextBox();
    const std::string &msg = text_box->GetText();

#  ifdef HAVE_FACEBOOK
    if (msg.compare(0, 3, "/fb") == 0) {
      std::string email, pwd;
      cfg->GetFaceBookCreds(email, pwd);
      // Always attempt to look in the command line for the password
      if (msg.size() > 4) {
        pwd = msg.substr(3);
        // Save the credentials for the session
        cfg->SetFaceBookCreds(email, pwd);
      }
      if (!email.empty() && !pwd.empty()) {
        Facepalm(email, pwd);
      } else {
        msg_box->NewMessage(_("*** Publishing failed: incomplete credentials"), c_red);
      }
      text_box->SetText("");
    } else
#  endif

#  ifdef HAVE_TWITTER
    if (msg.compare(0, 6, "/tweet") == 0) {
      std::string user, pwd;
      cfg->GetFaceBookCreds(user, pwd);
      // Always attempt to look in the command line for the password
      if (msg.size() > 7) {
        pwd = msg.substr(6);
        // Save the credentials for the session
        cfg->SetTwitterCreds(user, pwd);
      }
      if (!user.empty() && !pwd.empty()) {
        Tweet(user, pwd);
      } else {
        msg_box->NewMessage(_("*** Publishing failed: incomplete credentials"), c_red);
      }
      text_box->SetText("");
    } else
#  endif
#endif

      msg_box->SendChatMsg();
    return;
  }
  Menu::key_ok();
}

void ResultsMenu::Draw(const Point2i &/*mousePosition*/)
{
  if (Network::IsConnected()) {
    ActionHandler * action_handler = ActionHandler::GetInstance();
    action_handler->ExecFrameLessActions();
  }
}

void ResultsMenu::ReceiveMsgCallback(const std::string& msg, const Color& color)
{
  if (msg_box) {
    msg_box->NewMessage(msg, color);
  }
}

void ResultsMenu::OnClickUp(const Point2i &mousePosition, int button)
{
  Widget *w = widgets.ClickUp(mousePosition, button);

  b_cancel->SetVisible(tabs->GetCurrentTabId() == REPLAY_ID);

  // Are we recording?
  if (save) {
    // Are we requested to save?
    if (w == save) {
      SaveReplay();
    } else if (w == folders) {
      const char* file = folders->GetSelectedName();
      // This is a file, use that filename
      if (file) {
        replay_name->SetText(file);
      }
    }
  }

#if defined(HAVE_FACEBOOK) || defined(HAVE_TWITTER)
  std::string user, pwd;
#endif
#if defined(HAVE_FACEBOOK)
  if (social_panel->FacebookButtonPushed(w, user, pwd))
    Facepalm(user, pwd);
#endif
#if defined(HAVE_TWITTER)
  if (social_panel->TwitterButtonPushed(w, user, pwd))
    Tweet(user, pwd);
#endif

}
