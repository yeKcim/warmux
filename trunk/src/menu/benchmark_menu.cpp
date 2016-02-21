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
 * Benchmark menu, where to measure various aspects of the game performance.
 *****************************************************************************/

#include "game/config.h"
#include "game/game.h"
#include "game/game_mode.h"
#include "game/stopwatch.h"
#include "game/game_time.h"
#include "graphic/video.h"
#include "gui/button.h"
#include "gui/graph_canvas.h"
#include "gui/horizontal_box.h"
#include "gui/label.h"
#include "gui/vertical_box.h"
#include "include/app.h"
#include "map/maps_list.h"
#include "menu/benchmark_menu.h"
#include "menu/options_menu.h"
#include "network/randomsync.h"
#include "sound/jukebox.h"
#include "team/team.h"
#include "team/teams_list.h"

class BenchItem : public HBox
{
  Label     *bench;
  Label     *score;
  Button    *run;
public:
  BenchType type;

  BenchItem(BenchType t, const std::string& name, uint height)
    : HBox(height, false, false, true /* use full height */)
    , type(t)
  {
    Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml", false);

    bench = new Label(name, 0, Font::FONT_BIG, Font::FONT_BOLD,
                      dark_gray_color, Text::ALIGN_CENTER);
    AddWidget(bench);

    score = new Label("N/A", 300, Font::FONT_BIG, Font::FONT_BOLD,
                      primary_red_color, Text::ALIGN_CENTER);
    AddWidget(score);

    run = new Button(res, "menu/really_big_plus", true);
    AddWidget(run);
  }

  void SetScore(const char* format, float value)
  {
    char buffer[64];

    snprintf(buffer, sizeof(buffer), format, value);
    score->SetText(buffer);
  }

  Widget* ClickUp(const Point2i & mousePosition, uint /*button*/)
  {
    if (run->Contains(mousePosition))
      return this;
    return NULL;
  }

  virtual void Pack()
  {
    int width  = size.x - run->GetSizeX() - 2*5 -2*5;
    int height = run->GetSizeY();

    size.y = height + 2*border_size;
    bench->SetSizeX(width/2);
    score->SetSizeX(width/2);
    //run->SetPositionX(position.x + width);

    // Call first HBox::Pack to set positions
    HBox::Pack();

    // Set proper height now
    for (wit it = widget_list.begin(); it != widget_list.end(); ++it)
      (*it)->SetSizeY(height);
  }
};

#define FACTOR 0.9f

BenchmarkMenu::BenchmarkMenu() :
  Menu("menu/bg_option", vOk)
{
  const Surface& window = GetMainWindow();
  tests = new VBox(window.GetWidth()*FACTOR, true, true, true);
  tests->SetPosition(window.GetSize()* (1.0f - FACTOR)/2);

  tests->AddWidget(new BenchItem(BENCH_MENU, _("Menu"), 48));
  tests->AddWidget(new BenchItem(BENCH_GRAPHICS, _("Graphics"), 48));
  graph = new GraphCanvas(Point2i(window.GetWidth(), (window.GetHeight()-50)*FACTOR - 2*(48+5)), _("Time"), _("FPS"));
  tests->AddWidget(graph);

  widgets.AddWidget(tests);
  widgets.Pack();
}

BenchmarkMenu::~BenchmarkMenu()
{
}

bool BenchmarkMenu::Launch(BenchItem *b)
{
  if (!b)
    return false;

  bool        ok    = false;
  float       score = 0.0f;
  const char *fmt   = "";

  switch (b->type) {
  case BENCH_MENU:
    {
      Stopwatch   clock;
      delete (new OptionMenu());
      score = 1000.0f / clock.GetValue();
      fmt   = "%.3f";
      ok    = true;
      break;
    }
  case BENCH_GRAPHICS:
    {
      graph->UnsetResults();

      // Backup and set playing teams
      std::vector<Team*> list_bak = GetTeamsList().GetPlayingList();
      std::list<uint> sel; sel.push_back(1); sel.push_back(2);
      GetTeamsList().ChangeSelection(sel);

      // Backup and set team configuration - make it quick ;)
      std::vector<Team*>& list = GetTeamsList().GetPlayingList();
      for (uint i=0; i<list.size(); i++) {
        printf("Setting %s\n", list[i]->GetName().c_str());
        list[i]->SetPlayerName("CPU");
        list[i]->SetNbCharacters(10);
        list[i]->SetAIName(STRONG_AI_NAME);
        list[i]->SetGroup(i);
      }

      // Backup and set game mode
      Config *cfg = Config::GetInstance();
      std::string game_mode = cfg->GetGameMode();
      cfg->SetGameMode("benchmark");
      GameMode::GetInstance()->Load();

      // Backup and set some config options
      uint wind_particles = cfg->GetWindParticlesPercentage(); cfg->SetWindParticlesPercentage(100);
      bool display_energy = cfg->GetDisplayEnergyCharacter(); cfg->SetDisplayEnergyCharacter(true);
      bool display_multisky = cfg->GetDisplayMultiLayerSky(); cfg->SetDisplayMultiLayerSky(false);

      // Mute all sounds
      JukeBox *jbox = JukeBox::GetInstance();
      bool music = cfg->GetSoundMusic(); jbox->ActiveMusic(false);
      bool sfx = cfg->GetSoundEffects(); cfg->SetSoundEffects(false);

      // Backup and set default map - should I save the config?
      std::string map_name = cfg->GetMapName();
      MapsList *maps = MapsList::GetInstance();
      int map_id_bak = maps->GetActiveMapIndex(); maps->SelectMapByName("banquise");
      if (!maps->lst[maps->GetActiveMapIndex()]->LoadBasicInfo()) {
        fmt = "Error!";
        break;
      }

      // Set max FPS
      Video *video = AppWarmux::GetInstance()->video;
      int fps = video->GetMaxFps(); video->SetMaxFps(60);

      // Set seeds - we'll set random ones afterwards
      RandomLocal().SetSeed(0xABADCAFE);
      RandomSync().SetSeed(0xABADCAFE);

      // All set, run the game!
      float num  = Game::UpdateGameRules()->Start(true);
      if (num) {
        GraphCanvas::Result res;
        res.list = Game::GetInstance()->GetBenchResults();
        GraphCanvas::FindMax(res);

        float  time = res.xmax - res.list[0].first;
        score = (num * video->window.GetWidth()*video->window.GetHeight())
              / (1000.0f * time);
        fmt = "%.0f";

        res.item = NULL;
        res.color = primary_red_color;
        graph->AddResult(res);
      } else {
        fmt = "Aborted";
      }
      graph->NeedRedrawing();

      // Restore all!
      video->SetMaxFps(fps);
      jbox->ActiveMusic(music);
      cfg->SetSoundEffects(sfx);
      maps->SelectMapByIndex(map_id_bak);
      cfg->SetMapName(map_name);
      cfg->SetDisplayMultiLayerSky(display_multisky);
      cfg->SetDisplayEnergyCharacter(display_energy);
      cfg->SetWindParticlesPercentage(wind_particles);
      cfg->SetGameMode(game_mode);
      GetTeamsList().SetPlayingList(list_bak);

      ok = num == 0;
      break;
    }
  default: break;
  }

  b->SetScore(fmt, score);
  return ok;
}

void BenchmarkMenu::OnClickUp(const Point2i &mousePosition, int button)
{
  if (tests->Contains(mousePosition)) {
    Widget *widget = tests->ClickUp(mousePosition, button);
    if (widget && widget!=graph) {
      Launch(static_cast<BenchItem*>(widget));
      RedrawMenu();
    } else if (widget) {
      graph->NeedRedrawing();
    }
  }
}
