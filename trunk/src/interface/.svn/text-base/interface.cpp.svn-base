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
 * Graphical interface showing various information about the game.
 *****************************************************************************/

#include "include/action_handler.h"
#include "interface/interface.h"
#include "interface/weapon_help.h"
#include "interface/keyboard.h"
#include "interface/mouse.h"
#include "character/character.h"
#include "game/config.h"
#include "game/game.h"
#include "game/game_mode.h"
#include "game/game_time.h"
#include "graphic/text.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "gui/energy_bar.h"
#include "include/app.h"
#include "map/camera.h"
#include "map/map.h"
#include "object/objects_list.h"
#include "object/objbox.h"
#include "replay/replay.h"
#include "sound/jukebox.h"
#include "team/macro.h"
#include "team/team.h"
#include "tool/resource_manager.h"
#include "tool/string_tools.h"
#include "weapon/weapon.h"
#include "weapon/weapon_strength_bar.h"

#define BORDER_POSITION 5
#define MARGIN          4
#define WIND_ICON_WIDTH 79

void Interface::LoadDataInternal(Profile *res)
{
  Surface tmp     = LOAD_RES_IMAGE("interface/background_interface");

  FreeDrawElements();

  // Clocks should be kept, as information difficult to retrieve is there
  if (!clock_normal)
    clock_normal    = LOAD_RES_SPRITE("interface/clock_normal");
  if (!clock_emergency)
    clock_emergency = LOAD_RES_SPRITE("interface/clock_emergency");

  bool replay = Replay::GetConstInstance()->IsPlaying();
  last_width = AppWarmux::GetInstance()->video->window.GetWidth();
  if (last_width < tmp.GetWidth()+20) {
    zoom            = last_width / (float)(tmp.GetWidth()+20);
    default_toolbar = tmp.RotoZoom(0.0, zoom, zoom);
    small_interface = LOAD_RES_IMAGE("interface/small_background_interface").RotoZoom(0.0, zoom, zoom);
    if (replay) {
      control_toolbar.AutoFree();
      replay_toolbar = LOAD_RES_IMAGE("interface/background_replay").RotoZoom(0.0, zoom, zoom);
    } else {
      replay_toolbar.AutoFree();
      control_toolbar = LOAD_RES_IMAGE("interface/background_control_interface").RotoZoom(0.0, zoom, zoom);
    }
    clock_normal->Scale(zoom, zoom);
    clock_emergency->Scale(zoom, zoom);
  }
  else {
    zoom            = 1.0f;
    default_toolbar = tmp;
    if (replay) {
      control_toolbar.AutoFree();
      replay_toolbar = LOAD_RES_IMAGE("interface/background_replay");
    } else {
      replay_toolbar.AutoFree();
      control_toolbar = LOAD_RES_IMAGE("interface/background_control_interface");
    }
    small_interface = LOAD_RES_IMAGE("interface/small_background_interface");
  }
  clock_width = 70*zoom+0.5f;
#if defined(ANDROID) || defined (__SYMBIAN32__)
  // The optimization below depends on fastpath being implemented
  // for RGB565 with surface alpha *and* colorkey
  default_toolbar = default_toolbar.DisplayFormatColorKey(64);
  small_interface = small_interface.DisplayFormatColorKey(64);
  default_toolbar.SetAlpha(SDL_SRCALPHA, 128);
  small_interface.SetAlpha(SDL_SRCALPHA, 128);
#endif

  // energy bar
  energy_bar = new EnergyBar(0, 0, 150*zoom, 15*zoom, 0, 0,
                             GameMode::GetInstance()->character.init_energy);

  // Labels
  uint fsize = Font::FONT_SMALL*powf(zoom, 0.85f)+0.5f;
  if (fsize < 10) fsize = 10;
  t_character_name = new Text("None", m_text_color, fsize);
  t_team_name = new Text("None", m_text_color, fsize);
  t_player_name = new Text("None", m_text_color, fsize);
  t_weapon_name = new Text("None", m_text_color, fsize);
  t_weapon_stock = new Text("0", m_text_color, fsize);
  t_character_energy = new Text("Dead", m_energy_text_color, fsize, Font::FONT_BOLD, true);

  // Replay labels
  if (replay) {
    char tmp[] = { 'x', '1', 0 };
    tmp[1] = 48+(uint)GameTime::GetConstInstance()->GetSpeed();
    t_speed = new Text(tmp, primary_red_color, Font::FONT_HUGE*zoom+0.5f, Font::FONT_BOLD, true);
  }

  // Timer
  global_timer = new Text("0", gray_color, Font::FONT_BIG*zoom+0.5f);
  timer = new Text("0", black_color, Font::FONT_MEDIUM*zoom+0.5f);

  wind_bar.InitPos(0, 0, 82*zoom-1.5f, 15*zoom-1.5f);
}

void Interface::LoadData()
{
  Profile *res   = GetResourceManager().LoadXMLProfile("graphism.xml", false);
  LoadDataInternal(res);
}

Interface::Interface()
  : global_timer(NULL)
  , timer(NULL)
  , energy_bar(NULL)
  , t_character_name(NULL)
  , t_team_name(NULL)
  , t_player_name(NULL)
  , t_character_energy(NULL)
  , t_weapon_name(NULL)
  , t_weapon_stock(NULL)
  , t_speed(NULL)
  , mode(MODE_NORMAL)
  , start_hide_display(0)
  , start_show_display(0)
  , display_minimap(true)
  , clock(NULL)
  , clock_normal(NULL)
  , clock_emergency(NULL)
  , zoom(1.0f)
  , minimap(NULL)
  , m_last_minimap_redraw(0)
  , m_last_preview_size(0, 0)
  , mask(NULL)
  , scratch(NULL)
{
  Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml", false);

  m_text_color = LOAD_RES_COLOR("interface/text_color");
  m_energy_text_color = LOAD_RES_COLOR("interface/energy_text_color");

  // wind bar
  wind_bar.SetMinMaxValueColor(LOAD_RES_COLOR("interface/wind_color_min"),
                               LOAD_RES_COLOR("interface/wind_color_max"));
  wind_bar.InitVal(0, -100, 100);

  wind_bar.border_color.SetColor(0, 0, 0, 0);
  wind_bar.background_color.SetColor(0, 0, 0, 0);
  //wind_bar.value_color = c_red;

  wind_bar.SetReferenceValue(true, 0);

  // strength bar initialisation
  weapon_strength_bar.InitPos(0, 0, 300, 15);
  weapon_strength_bar.InitVal(0, 0, 100);

  weapon_strength_bar.SetValueColor(LOAD_RES_COLOR("interface/weapon_strength_bar_value"));
  weapon_strength_bar.SetBorderColor(LOAD_RES_COLOR("interface/weapon_strength_bar_border"));
  weapon_strength_bar.SetBackgroundColor(LOAD_RES_COLOR("interface/weapon_strength_bar_background"));

  m_camera_preview_color = LOAD_RES_COLOR("interface/camera_preview_color");

  m_playing_character_preview_color = LOAD_RES_COLOR("interface/playing_character_preview_color");

  // Weapon help
  help = new WeaponHelp();
}

Interface::~Interface()
{
  FreeDrawElements();

  if (minimap) delete minimap;
  if (mask) delete mask;
  if (scratch) delete scratch;

  if (energy_bar) delete energy_bar;

  if (clock_normal) delete clock_normal;
  if (clock_emergency) delete clock_emergency;

  delete help;
}

void Interface::FreeDrawElements()
{
  if (global_timer) delete global_timer;
  if (timer) delete timer;
  if (t_character_name) delete t_character_name;
  if (t_team_name) delete t_team_name;
  if (t_player_name) delete t_player_name;
  if (t_character_energy) delete t_character_energy;
  if (t_weapon_name) delete t_weapon_name;
  if (t_weapon_stock) delete t_weapon_stock;
  if (t_speed) {
    delete t_speed;
    t_speed = NULL;
  }
}

void Interface::Reset()
{
  delete minimap;
  minimap = NULL;
  m_last_minimap_redraw = 0;
  start_hide_display = 0;
  start_show_display = 0;
  character_under_cursor = NULL;
  weapon_under_cursor = NULL;

  LoadDataInternal(GetResourceManager().LoadXMLProfile("graphism.xml", false));
  mode = Replay::GetConstInstance()->IsPlaying() ? MODE_REPLAY : MODE_NORMAL;

  weapons_menu.Reset();
  help->Reset();
  energy_bar->InitVal(0, 0, GameMode::GetInstance()->character.init_energy);
  TeamEnergy::SetSpacing((174-MARGIN)*zoom / TeamsList::GetInstance()->GetPlayingList().size());
  FOR_EACH_TEAM(tmp_team)
    (*tmp_team)->GetEnergyBar().SetHeight(default_toolbar.GetHeight());
}

void Interface::DrawCharacterInfo()
{
  Surface& window = GetMainWindow();

  // Get the character
  if (!character_under_cursor)
    character_under_cursor = &ActiveCharacter();
  const Team &team = character_under_cursor->GetTeam();

  // Display energy bar
  Point2i energy_bar_offset = Point2i(MARGIN + team.GetFlag().GetWidth(),
                                      team.GetFlag().GetHeight()>>1) + BORDER_POSITION;
  energy_bar->DrawXY(bottom_bar_pos + energy_bar_offset);

  // Display team logo
  if (energy_bar->GetCurrentValue() == energy_bar->GetMinValue())
    window.Blit(team.GetDeathFlag(), bottom_bar_pos + BORDER_POSITION);
  else
    window.Blit(team.GetFlag(), bottom_bar_pos + BORDER_POSITION);

  // Display team name
  t_team_name->SetText(team.GetName());
  Point2i team_name_offset = energy_bar_offset + Point2i(energy_bar->GetWidth()>>1,
                                                         energy_bar->GetHeight() + (t_team_name->GetHeight()>>1));
  t_team_name->DrawCenter(bottom_bar_pos + team_name_offset);

  // Display character's name
  t_character_name->SetText(character_under_cursor->GetName());
  Point2i character_name_offset = energy_bar_offset + Point2i((energy_bar->GetWidth() > t_character_name->GetWidth() ? energy_bar->GetWidth() : t_character_name->GetWidth()) / 2, -t_character_name->GetHeight() / 2);
  t_character_name->DrawCenter(bottom_bar_pos + character_name_offset);

  // Display player's name
  if (window.GetHeight() > 480) {
    t_player_name->SetText(_("Head commander") + std::string(": ") + team.GetPlayerName());
    Point2i player_name_offset = energy_bar_offset
      + Point2i((energy_bar->GetWidth()>>1), t_team_name->GetHeight() + t_player_name->GetHeight() + MARGIN);
    t_player_name->DrawCenter(bottom_bar_pos + player_name_offset);
  }

  // Display energy
  if (!character_under_cursor->IsDead()) {
    t_character_energy->SetText(uint2str(character_under_cursor->GetEnergy())+"%");
    energy_bar->Actu(character_under_cursor->GetEnergy());
  } else {
    t_character_energy->SetText(_("(dead)"));
    energy_bar->Actu(0);
  }

  t_character_energy->DrawCenter(bottom_bar_pos + energy_bar_offset + (energy_bar->GetSize()>>1));
}

void Interface::DrawWeaponInfo() const
{
  Weapon* weapon = &ActiveTeam().AccessWeapon();
  int nbr_munition = ActiveTeam().ReadNbAmmos();
  Sprite& icon = weapon->GetIcon();

  icon.Scale(zoom, zoom);

  int offset = ((default_toolbar.GetWidth() - clock_width)>>1) - icon.GetWidth();
  // The control interface doesn't look good with texts overlayed on it
  if (mode == MODE_NORMAL) {
    // Draw weapon name
    t_weapon_name->SetText(weapon->GetName());
    t_weapon_name->DrawRightCenter(bottom_bar_pos + Point2i(offset, 70*zoom));

    // Display number of ammo
    t_weapon_stock->SetText(nbr_munition ==  INFINITE_AMMO ? _("(unlimited)")
                                                           : _("Stock:") + Format("%i", nbr_munition));
    t_weapon_stock->DrawCenterTop(bottom_bar_pos + Point2i(offset+(icon.GetWidth()>>1), MARGIN));
  }

  // Draw weapon icon
  Point2i weapon_icon_offset(offset, default_toolbar.GetHeight() - icon.GetHeight() - MARGIN);
  icon.DrawXY(bottom_bar_pos + weapon_icon_offset);
}

void Interface::DrawTimeInfo() const
{
  Surface& window = GetMainWindow();
  Point2i turn_time_pos((window.GetWidth() - clock_width)>>1,
                        window.GetHeight()  - GetHeight());
  Rectanglei dr(turn_time_pos, Point2i(clock_width, default_toolbar.GetHeight()));

  // Draw background interface
  GetWorld().ToRedrawOnScreen(dr);
  DrawClock(turn_time_pos + clock->GetHeight());
}

// display time left in a turn
void Interface::DrawClock(const Point2i &time_pos) const
{
  // Draw turn time
  if (display_timer)
    timer->DrawCenter(time_pos - Point2i(0, default_toolbar.GetHeight()/3));

  // Draw clock
  Point2i tmp_point = time_pos - (clock->GetSize()>>1);
  clock->Update();
  clock->DrawXY(tmp_point);

  // Draw global timer
  std::string tmp(GameTime::GetInstance()->GetString());
  global_timer->SetText(tmp);
  global_timer->DrawCenter(time_pos + Point2i(0, default_toolbar.GetHeight()/3));
}

// draw wind indicator
void Interface::DrawWindIndicator(const Point2i &wind_bar_pos) const
{
  wind_bar.DrawXY(wind_bar_pos);
  GetWorld().ToRedrawOnScreen(Rectanglei(wind_bar_pos, wind_bar.GetSize()));
}

// display wind info
void Interface::DrawWindInfo() const
{
  // The hardcoded values are from the wind indicator position in the image
  Point2i wind_pos_offset(348*zoom, 49*zoom+0.5f);
  DrawWindIndicator(bottom_bar_pos + wind_pos_offset);
}

// display replay info
void Interface::DrawReplayInfo() const
{
  t_speed->DrawCenter(bottom_bar_pos + Point2i(576*zoom+0.5f,default_toolbar.GetHeight()>>1));
  float len = (GameTime::GetConstInstance()->Read() * 128.0f) / Replay::GetConstInstance()->GetDuration();
  GetMainWindow().BoxColor(Rectanglei(bottom_bar_pos + Point2i(149*zoom, 35*zoom),
                                      Point2i(len*zoom+0.5f, 11*zoom+0.5f)),
                           primary_red_color);
}

// draw mini info when hidding interface
void Interface::DrawSmallInterface() const
{
  if (mode != MODE_SMALL)
    return;
  Surface& window = GetMainWindow();
  int height = ((int)GameTime::GetInstance()->Read() - start_hide_display - 1000) / 3 - 30;
  height = height > 0 ? height : 0;
  height = (height < small_interface.GetHeight()) ? height : small_interface.GetHeight();
  Point2i position((window.GetWidth() - small_interface.GetWidth())>>1,
                   window.GetHeight() - height);
  window.Blit(small_interface, position);
  // The wind indicator can be zoomed and no longer centered, so
  DrawWindIndicator(position + Point2i(9, 11)*zoom);
  if (display_timer) {
    timer->DrawLeftTop(position + Point2i(MARGIN * 4 + wind_bar.GetWidth(), 2*MARGIN+2));
  }
  GetWorld().ToRedrawOnScreen(Rectanglei(position, small_interface.GetSize()));
}

// draw team energy
void Interface::DrawTeamEnergy() const
{
  Point2i team_bar_offset(430*zoom, 0);
  FOR_EACH_TEAM(tmp_team) {
    Team* team = *tmp_team;
    if (!IsDisplayed()) // Fix bug #7753 (Team energy bar visible when the interface is hidden)
      team->GetEnergyBar().FinalizeMove();
    team->DrawEnergy(bottom_bar_pos + team_bar_offset);
  }
}

// Draw map preview
void Interface::DrawMapPreview()
{
  Surface   &window  = GetMainWindow();
  Ground    &ground  = GetWorld().ground;
  Point2i    offset(window.GetWidth() - ground.GetPreviewSize().x - 2*MARGIN,
                    2*MARGIN);
  Rectanglei rect_preview(offset, ground.GetPreviewSize());

  Rectanglei clip = rect_preview;
  SwapWindowClip(clip);

  if (!ground.IsPreviewHQ()) {
    window.Blit(*ground.GetPreview(),
                offset-ground.GetPreviewRect().GetPosition());

    // Draw water
    if (GetWorld().water.IsActive()) {
      Color color = *GetWorld().water.GetColor();

      // Scale water height according to preview size
      int y = GetWorld().GetSize().GetY() - GetWorld().water.GetSelfHeight();
      int h = ground.PreviewCoordinates(Point2i(0, y)).GetY();

      color.SetAlpha(200);
      window.BoxColor(Rectanglei(Point2i(0, h)+offset, rect_preview.GetSize() - Point2i(0, h)),
                      color);
    }
  } else {
    if (minimap == NULL ||
        ground.GetLastPreviewRedrawTime() > m_last_minimap_redraw ||
        GetWorld().water.GetLastPreviewRedrawTime() > m_last_minimap_redraw) {

      m_last_minimap_redraw = GameTime::GetInstance()->Read();
      const Point2i& preview_size = ground.GetPreviewSize();

      // Check whether the whole minimap must be updated
      if (m_last_preview_size != preview_size) {
        if (mask) {
          delete mask;
          mask = NULL;
        }
        if (minimap) {
          delete minimap;
          minimap = NULL;
        }
        if (scratch) {
          delete scratch;
          scratch = NULL;
        }
      }

      if (!minimap)
        minimap = new Surface(preview_size, SDL_SWSURFACE, true);

      // Recreate the scratch buffer
      if (!scratch)
        scratch = new Surface(preview_size, SDL_SWSURFACE, true);

      Point2i mergePos = -ground.GetPreviewRect().GetPosition();
      scratch->Blit(*ground.GetPreview(), mergePos);

      // Draw water
      if (GetWorld().water.IsActive()) {
        Color color = *GetWorld().water.GetColor();

        // Scale water height according to preview size
        int y = GetWorld().GetSize().GetY() - GetWorld().water.GetSelfHeight();
        int h = ground.PreviewCoordinates(Point2i(0, y)).GetY();

        color.SetAlpha(200);
        scratch->BoxColor(Rectanglei(Point2i(0, h), rect_preview.GetSize() - Point2i(0, h)),
                          color);
      }

      //scratch->SetAlpha(SDL_SRCALPHA, 0);
      if (!mask) {
        m_last_preview_size = ground.GetPreviewSize();
        mask = new Surface(m_last_preview_size, SDL_SWSURFACE, true);

        GenerateStyledBorder(*mask, DecoratedBox::STYLE_ROUNDED);

        mask->SetAlpha(0, 0);
      }

      // Compose
      minimap->Blit(*mask);
      minimap->Blit(*scratch);
    }

    window.Blit(*minimap, offset);
  }

  Point2i coord;

  // Add objects like medkits or bonus boxes
  FOR_EACH_OBJECT(object) {
    if ((*object)->GetName()=="medkit" || (*object)->GetName()=="bonus_box") {
      ObjBox* box = static_cast<ObjBox*>(*object);
      const Surface* icon = box->GetIcon();

      // The real icon
      coord = ground.PreviewCoordinates(box->GetPosition())
            + offset - Point2i(icon->GetWidth()>>1, (3*icon->GetHeight())>>2);
      window.Blit(*icon, coord);
      GetWorld().ToRedrawOnScreen(Rectanglei(coord, icon->GetSize()));
    }
  }

  // Add team characters
  FOR_EACH_TEAM(team) {
    const Surface & icon = (*team)->GetMiniFlag();

    for (Team::iterator character = (*(team))->begin(), end_character = (*(team))->end();
         character != end_character;
         ++character) {

      if (character->IsDead()) {
        continue;
      }

      coord = ground.PreviewCoordinates(character->GetPosition()) + offset;
      Point2i icoord = coord - (icon.GetSize()>>1);
      window.Blit(icon, icoord);

      if (character->IsActiveCharacter()) {
        uint radius = (icon.GetSize().x < icon.GetSize().y) ? icon.GetSize().y : icon.GetSize().x;
        radius = (radius/2) + 1;
        window.CircleColor(coord.x, coord.y, radius, m_playing_character_preview_color);
        GetWorld().ToRedrawOnScreen(Rectanglei(coord.x-radius-1, coord.y-radius-1, 2*radius+2, 2*radius+2));
      } else {
        GetWorld().ToRedrawOnScreen(Rectanglei(icoord, icon.GetSize()));
      }

    }
  }

  const Camera* cam = Camera::GetConstInstance();
  Point2i TopLeft = ground.PreviewCoordinates(cam->GetPosition());
  Point2i BottomR = ground.PreviewCoordinates(cam->GetPosition()+cam->GetSize());

  GetMainWindow().RectangleColor(Rectanglei(TopLeft + offset, BottomR-TopLeft),
                                 m_camera_preview_color);
  SwapWindowClip(clip);

  GetWorld().ToRedrawOnScreen(rect_preview);
}

void Interface::Draw()
{
  Surface &window  = GetMainWindow();

  bottom_bar_pos.SetValues((window.GetWidth() - GetWidth())>>1,
                           window.GetHeight() - GetHeight());
  // Has the display size changed? Then reload data
  if (last_width != window.GetWidth()) {
    LoadData();
  }

  if (display_minimap)
    DrawMapPreview();

  // Position on the screen
  Point2i barPos((window.GetWidth() - weapon_strength_bar.GetWidth())>>1,
                 window.GetHeight() - weapon_strength_bar.GetHeight() - default_toolbar.GetHeight() - MARGIN);

  // Drawing on the screen
  weapon_strength_bar.DrawXY(barPos);

  weapons_menu.Draw();

  // Display the background
  if (IsDisplayed()) {
    Rectanglei dr(bottom_bar_pos, default_toolbar.GetSize());
    if (mode==MODE_CONTROL || mode==MODE_NORMAL) {
      if (mode==MODE_CONTROL) {
        window.Blit(control_toolbar, bottom_bar_pos);
      } else {
        window.Blit(default_toolbar, bottom_bar_pos);
        // And both Character info and weapon info
        DrawCharacterInfo();
        DrawTeamEnergy();
      }

      // Now display wind and time info
      DrawWeaponInfo();
    } else {
      window.Blit(replay_toolbar, bottom_bar_pos);
      DrawReplayInfo();
    }
    DrawWindInfo();
    DrawTimeInfo();
    GetWorld().ToRedrawOnScreen(dr);
  } else {
    DrawSmallInterface();
    // The rectangle to redraw is already set there
  }

  help->Draw();
}

int Interface::GetHeight() const
{
  if (!IsDisplayed()) {
    int height = GetMenuHeight() - ((int)GameTime::GetInstance()->Read() - start_hide_display)/3;
    height = (height > 0 ? height : 0);
    return (height < GetMenuHeight() ? height : GetMenuHeight());
  } else if (start_show_display != 0) {
    int height = ((int)GameTime::GetInstance()->Read() - start_show_display)/3;
    height = (height < GetMenuHeight() ? height : GetMenuHeight());
    return (height < GetMenuHeight() ? height : GetMenuHeight());
  }
  return GetMenuHeight();
}

int Interface::GetMenuHeight() const
{
  return default_toolbar.GetHeight() + MARGIN;
}

void Interface::Show()
{
  if (IsDisplayed())
    return;
  mode = MODE_NORMAL;
  uint now = GameTime::GetInstance()->Read();
  if (start_show_display + 1000 < (int)now)
    start_show_display = now;
  else
    start_show_display = now - (1000 - ((int)now - start_show_display));
}

void Interface::Hide()
{
  if (mode==MODE_SMALL || mode==MODE_REPLAY)
    return;
  mode = MODE_SMALL;
  uint now = GameTime::GetInstance()->Read();
  if (start_hide_display + 1000 < (int)now)
    start_hide_display = now;
  else
    start_hide_display = now - (1000 - ((int)now - start_hide_display));
}

void Interface::UpdateTimer(uint utimer, bool emergency, bool reset_anim)
{
  Sprite *prev_clock = clock;

  if (emergency) {
    clock = clock_emergency;
    timer->SetColor(primary_red_color);
  } else {
    clock = clock_normal;
    timer->SetColor(black_color);
  }

  timer->SetText(uint2str(utimer));
  remaining_turn_time = utimer;

  if (prev_clock != clock || reset_anim) {

    if (clock == clock_normal) {
      uint frame_delay = utimer * 1000 / clock->GetFrameCount();
      clock->SetFrameSpeed(frame_delay);
    }

    clock->animation.SetLoopMode(true);
    clock->SetCurrentFrame(0);
    clock->Start();
  }
}

void Interface::UpdateWindIndicator(int wind_value)
{
  wind_bar.UpdateValue(wind_value);
};

void AbsoluteDraw(const Surface &s, const Point2i& pos)
{
  Rectanglei rectSurface(pos, s.GetSize());

  if (!rectSurface.Intersect(*Camera::GetInstance()))
    return;

  GetWorld().ToRedrawOnMap(rectSurface);

  rectSurface.Clip(*Camera::GetInstance());

  Rectanglei rectSource(rectSurface.GetPosition() - pos, rectSurface.GetSize());
  Point2i ptDest = rectSurface.GetPosition() - Camera::GetInstance()->GetPosition();

  GetMainWindow().Blit(s, rectSource, ptDest);
}

bool Interface::ControlClick(const Point2i &mouse_pos, ClickType type, Point2i old_mouse_pos)
{
  Character *active_char = &ActiveCharacter();
  Game      *game        = Game::GetInstance();

  // Make sure we don't go in there while we shouldn't
  if (!ActiveTeam().IsLocalHuman()
      || active_char->IsDead()
      || (game->ReadState() != Game::PLAYING
          // movement should be possible just after shooting
          && game->ReadState() != Game::HAS_PLAYED))
    return false;

  Point2i button_size(56*zoom, control_toolbar.GetHeight());
  Point2i mouse_rel_pos = mouse_pos-bottom_bar_pos;
  int mods = Keyboard::GetModifierBits();

  old_mouse_pos -= bottom_bar_pos;

  Rectanglei left_button(Point2i(3*zoom, 0), button_size);
  if (left_button.Contains(mouse_rel_pos)) {
    switch (type) {
      case CLICK_TYPE_LONG: break;
      case CLICK_TYPE_DOWN:
        active_char->HandleKeyPressed_MoveLeft(mods & SHIFT_BIT);
        if (game->ReadState() == Game::PLAYING) {
          ActiveTeam().AccessWeapon().HandleKeyPressed_MoveLeft(mods & SHIFT_BIT);
        }
        break;
      case CLICK_TYPE_UP:
        active_char->HandleKeyReleased_MoveLeft(mods & SHIFT_BIT);
        if (game->ReadState() == Game::PLAYING) {
          ActiveTeam().AccessWeapon().HandleKeyReleased_MoveLeft(mods & SHIFT_BIT);
        }
        break;
    }
    return true;
  }

  Rectanglei right_button(Point2i(60*zoom, 0), button_size);
   if (right_button.Contains(mouse_rel_pos)) {
    switch (type) {
      case CLICK_TYPE_LONG: break;
      case CLICK_TYPE_DOWN:
        active_char->HandleKeyPressed_MoveRight(mods & SHIFT_BIT);
        if (game->ReadState() == Game::PLAYING) {
          ActiveTeam().AccessWeapon().HandleKeyPressed_MoveRight(mods & SHIFT_BIT);
        }
        break;

      case CLICK_TYPE_UP:
        active_char->HandleKeyReleased_MoveRight(mods & SHIFT_BIT);
        if (game->ReadState() == Game::PLAYING) {
          ActiveTeam().AccessWeapon().HandleKeyReleased_MoveRight(mods & SHIFT_BIT);
        }
        break;
    }
    return true;
  }

  Rectanglei jump_button(Point2i(120*zoom, 0), button_size);
   if (jump_button.Contains(mouse_rel_pos)) {
    switch (type) {
      case CLICK_TYPE_LONG:
        if (!jump_button.Contains(old_mouse_pos))
          return false;
        active_char->HandleKeyPressed_HighJump();
        if (game->ReadState() == Game::PLAYING) {
          ActiveTeam().AccessWeapon().HandleKeyPressed_HighJump();
        }
        break;
      case CLICK_TYPE_DOWN: return false; // Needed to allow long clicks
      case CLICK_TYPE_UP:
        // Active character
        if (mods & SHIFT_BIT)
          active_char->HandleKeyPressed_HighJump();
        else if (mods & CONTROL_BIT)
          active_char->HandleKeyPressed_BackJump();
        else
          active_char->HandleKeyPressed_Jump();

        // Check for active team
        if (game->ReadState() == Game::PLAYING) {
          Weapon& current = ActiveTeam().AccessWeapon();
          if (mods & SHIFT_BIT)
            current.HandleKeyPressed_HighJump();
          else if (mods & CONTROL_BIT)
            current.HandleKeyPressed_BackJump();
          else
            current.HandleKeyPressed_Jump();
        }
        break;
    }
    return true;
  }

  Rectanglei timerup_button(Point2i(180*zoom, 0), Point2i(40*zoom, control_toolbar.GetHeight()>>1));
   if (timerup_button.Contains(mouse_rel_pos)) {
    switch (type) {
      case CLICK_TYPE_LONG: break;
      case CLICK_TYPE_DOWN: break;
      case CLICK_TYPE_UP: ActiveTeam().AccessWeapon().HandleKeyReleased_More(); break;
    }
    return true;
  }
  Rectanglei timerdown_button(Point2i(180*zoom, control_toolbar.GetHeight()>>1),
                              Point2i(40*zoom, control_toolbar.GetHeight()>>1));
   if (timerdown_button.Contains(mouse_rel_pos)) {
    switch (type) {
      case CLICK_TYPE_LONG: break;
      case CLICK_TYPE_DOWN: break;
      case CLICK_TYPE_UP: ActiveTeam().AccessWeapon().HandleKeyReleased_Less(); break;
    }
    return true;
  }

  Rectanglei up_button(Point2i(433*zoom, 0), button_size);
   if (up_button.Contains(mouse_rel_pos)) {
    switch (type) {
      case CLICK_TYPE_LONG: break;
      case CLICK_TYPE_DOWN:
        active_char->HandleKeyPressed_Up(mods & SHIFT_BIT);
        if (game->ReadState() == Game::PLAYING) {
          ActiveTeam().AccessWeapon().HandleKeyPressed_Up(mods & SHIFT_BIT);
        }
        break;

      case CLICK_TYPE_UP:
        active_char->HandleKeyReleased_Up(mods & SHIFT_BIT);
        if (game->ReadState() == Game::PLAYING) {
          ActiveTeam().AccessWeapon().HandleKeyReleased_Up(mods & SHIFT_BIT);
        }
        break;
    }
    return true;
  }

  Rectanglei down_button(Point2i(490*zoom, 0), button_size);
   if (down_button.Contains(mouse_rel_pos)) {
    switch (type) {
      case CLICK_TYPE_LONG: break;
      case CLICK_TYPE_DOWN:
        active_char->HandleKeyPressed_Down(mods & SHIFT_BIT);
        if (game->ReadState() == Game::PLAYING) {
          ActiveTeam().AccessWeapon().HandleKeyPressed_Down(mods & SHIFT_BIT);
        }
        break;

      case CLICK_TYPE_UP:
        active_char->HandleKeyReleased_Down(mods & SHIFT_BIT);
        if (game->ReadState() == Game::PLAYING) {
          ActiveTeam().AccessWeapon().HandleKeyReleased_Down(mods & SHIFT_BIT);
        }
        break;
    }
    return true;
  }

  // Check if we clicked the shoot icon: start firing!
  Rectanglei shoot_button(Point2i(546*zoom, 0), button_size);
  if (shoot_button.Contains(mouse_rel_pos)) {
    if (game->ReadState() == Game::END_TURN) {
      game->RequestBonusBoxDrop();
      return true;
    }
    // state can only be Game::PLAYING now

    switch (type) {
      case CLICK_TYPE_LONG: break;
      case CLICK_TYPE_DOWN:
        ActiveTeam().AccessWeapon().HandleKeyPressed_Shoot();
        break;
      case CLICK_TYPE_UP: {
        ActiveTeam().AccessWeapon().HandleKeyReleased_Shoot();
        break;
      }
    }
    return true;
  }

  // No actual button clicked, just allow long clicks
  return (type == CLICK_TYPE_DOWN) ? false : true;
}

void Interface::SetSpeed(const Double& speed)
{
  if (speed<ONE || speed>8)
    return;

  // Set text only once
  char tmp[] = { 'x', '1', 0 };
  tmp[1] = 48+(uint)speed;
  t_speed->SetText(tmp);

  Game::GetInstance()->RequestSpeed(speed);
}

bool Interface::ReplayClick(const Point2i &mouse_pos, ClickType type, Point2i old_mouse_pos)
{
  Game     *game = Game::GetInstance();
  Point2i mouse_rel_pos = mouse_pos-bottom_bar_pos;
  Point2i size(64*zoom, replay_toolbar.GetHeight());

  old_mouse_pos -= bottom_bar_pos;

  Rectanglei play_button(Point2i(10*zoom, 0), size);
  if (play_button.Contains(mouse_rel_pos)) {
    // We have to go through the game loop to pause
    if (type == CLICK_TYPE_DOWN)
      game->RequestPause(!GameTime::GetConstInstance()->IsPaused());
    return true;
  }

  Rectanglei stop_button(Point2i(72*zoom, 0), size);
  if (stop_button.Contains(mouse_rel_pos)) {
    // We have to go through the game loop to pause
    if (type == CLICK_TYPE_DOWN)
      game->UserAsksForEnd();
    return true;
  }

  Rectanglei skip_button(Point2i(149*zoom, 0), Point2i(128*zoom, replay_toolbar.GetHeight()));
  if (skip_button.Contains(mouse_rel_pos)) {
    uint time;
    switch (type) {
      case CLICK_TYPE_LONG: time = GameTime::GetConstInstance()->Read() + 60000; break;
      case CLICK_TYPE_DOWN: return false; // Needed to allow long clicks
      case CLICK_TYPE_UP: time = GameTime::GetConstInstance()->Read() + 30000; break;
      default: return false;
    }
    game->RequestTime(time);
    return true;
  }

  Rectanglei slow_button(Point2i(433*zoom, 0), Point2i(49*zoom, replay_toolbar.GetHeight()));
  if (slow_button.Contains(mouse_rel_pos)) {
    if (type == CLICK_TYPE_DOWN)
      SetSpeed(GameTime::GetConstInstance()->GetSpeed()-ONE);
    return true;
  }

  Rectanglei fast_button(Point2i(482*zoom, 0), size);
  if (fast_button.Contains(mouse_rel_pos)) {
    if (type == CLICK_TYPE_DOWN && Game::GetInstance()->GetLastFrameRate()>10)
      SetSpeed(GameTime::GetConstInstance()->GetSpeed()+ONE);
    return true;
  }

  return false;
}

bool Interface::DefaultClick(const Point2i &mouse_pos, ClickType type, Point2i old_mouse_pos)
{
  Point2i mouse_rel_pos = mouse_pos-bottom_bar_pos;
  const Team& team = ActiveTeam();

  Rectanglei character_button(0, 0, 36*zoom, default_toolbar.GetHeight());

  old_mouse_pos -= bottom_bar_pos;

  // Check if we clicked the character icon:
  if (character_button.Contains(mouse_rel_pos)) {
    switch (type) {
      case CLICK_TYPE_LONG:
        if (!character_button.Contains(old_mouse_pos))
          return false;
        Camera::GetInstance()->CenterOnActiveCharacter();
        return true;
      case CLICK_TYPE_DOWN: return false; // Needed to allow long clicks
      case CLICK_TYPE_UP:
        if (!team.IsLocalHuman() || !GameMode::GetInstance()->AllowCharacterSelection())
          return false;
        ActiveTeam().NextCharacter();
        ActionHandler::GetInstance()->NewActionActiveCharacter();
        return true;
    }
  }

  // No actual button clicked, just allow long clicks
  return (type == CLICK_TYPE_DOWN) ? false : true;
}

int Interface::AnyClick(const Point2i &mouse_pos, ClickType type, Point2i old_mouse_pos)
{
  Point2i mouse_rel_pos = mouse_pos - bottom_bar_pos;
  const Team& ateam = ActiveTeam();

  old_mouse_pos -= bottom_bar_pos;

  // Check if we clicked the clock icon: toggle control toolbar
  Rectanglei clock_button((default_toolbar.GetWidth() - clock_width)>>1, 0,
                          clock_width, default_toolbar.GetHeight());
  if (clock_button.Contains(mouse_pos-bottom_bar_pos)) {
    switch (type) {
      case CLICK_TYPE_LONG: break;
      case CLICK_TYPE_DOWN: return 0;
      case CLICK_TYPE_UP:
        if (clock_button.Contains(old_mouse_pos))
          Game::GetInstance()->UserAsksForMenu();
        break;
    }
    return 1;
  }

  // Positions are somewhat from Interface::DrawWeaponInfo()
  Point2i BR((default_toolbar.GetWidth() - clock_width)>>1,
             default_toolbar.GetHeight());
  Point2i TL(BR.GetX()- int(36*zoom)-3*MARGIN, 0);
  Rectanglei weapon_button(TL, BR-TL);
  // Check if we clicked the weapon icon: toggle weapon menu
  if (weapon_button.Contains(mouse_rel_pos)) {
    if (mode == MODE_REPLAY)return -1; // This button isn't active in replay
    switch (type) {
      case CLICK_TYPE_LONG:
        if (weapon_button.Contains(old_mouse_pos)) {
          help->SetWeapon(ateam.AccessWeapon());
          help->SwitchDisplay();
        }
        break;
      case CLICK_TYPE_DOWN: return 0; // Needed to allow long clicks
      case CLICK_TYPE_UP:
        if (!ateam.IsLocalHuman() || ActiveCharacter().IsDead() ||
            Game::GetInstance()->ReadState() != Game::PLAYING ||
            Replay::GetConstInstance()->IsPlaying())
          return 1;
        if (weapon_button.Contains(old_mouse_pos))
          weapons_menu.SwitchDisplay();
    }
    return 1;
  }

  Rectanglei wind_button(343*zoom, 0, 86*zoom, default_toolbar.GetHeight());
  if (wind_button.Contains(mouse_rel_pos)) {
    switch (type) {
      case CLICK_TYPE_LONG:
      case CLICK_TYPE_DOWN: return 1;
      case CLICK_TYPE_UP:
        if (Replay::GetConstInstance()->IsPlaying()) {
          mode = (mode==MODE_REPLAY) ? MODE_NORMAL : MODE_REPLAY;
        } else if (ateam.IsLocalHuman() && !ActiveCharacter().IsDead() &&
                   Game::GetInstance()->ReadState() == Game::PLAYING &&
                   wind_button.Contains(old_mouse_pos)) {
          mode = (mode==MODE_CONTROL) ? MODE_NORMAL : MODE_CONTROL;
        }
        return 1;
    }
  }

  return -1;
}

bool Interface::ActionClickDown(const Point2i &mouse_pos)
{
  Surface& window = GetMainWindow();

  if (IsDisplayed()) {
    Rectanglei menu_button(Point2i(), default_toolbar.GetSize());
    if (menu_button.Contains(mouse_pos-bottom_bar_pos)) {
      switch (AnyClick(mouse_pos, CLICK_TYPE_DOWN)) {
        case -1: break;
        case 0: return false;
        default: return true;
      }

      switch (mode) {
        case MODE_CONTROL: return ControlClick(mouse_pos, CLICK_TYPE_DOWN);
        case MODE_NORMAL: return DefaultClick(mouse_pos, CLICK_TYPE_DOWN);
        case MODE_REPLAY: return ReplayClick(mouse_pos, CLICK_TYPE_DOWN);
        default: exit(1);
      }
    }
  } else {
    // Mini-interface drawn, check if we clicked on it
    if (ActiveTeam().IsLocalHuman()) {
      Rectanglei small_button((window.GetWidth() - small_interface.GetWidth())>>1,
                              window.GetHeight() - small_interface.GetHeight(),
                              small_interface.GetWidth(),
                              small_interface.GetHeight());
      if (small_button.Contains(mouse_pos)) {
        return true;
      }
    }
  }

  return false;
}

bool Interface::ActionLongClick(const Point2i &mouse_pos, const Point2i& old_mouse_pos)
{
  if (IsDisplayed()) {
    switch (AnyClick(mouse_pos, CLICK_TYPE_LONG, old_mouse_pos)) {
      case -1: break;
      case 0: return false;
      default: return true;
    }

    switch (mode) {
      case MODE_CONTROL: return ControlClick(mouse_pos, CLICK_TYPE_LONG);
      case MODE_NORMAL: return DefaultClick(mouse_pos, CLICK_TYPE_LONG);
      case MODE_REPLAY: return ReplayClick(mouse_pos, CLICK_TYPE_LONG);
      default: exit(1);
    }
  }

  return false;
}

bool Interface::ActionClickUp(const Point2i &mouse_pos, const Point2i &old_click_pos)
{
  Surface &  window  = GetMainWindow();

  if (IsDisplayed()) {
    if (mode == MODE_CONTROL) {
      ActiveCharacter().HandleKeyReleased_MoveLeft(false);
      ActiveCharacter().HandleKeyReleased_MoveRight(false);
      ActiveCharacter().HandleKeyReleased_Up(false);
      ActiveCharacter().HandleKeyReleased_Down(false);
      if (weapon_strength_bar.IsVisible())
        ActiveTeam().AccessWeapon().HandleKeyReleased_Shoot();
    }

    Rectanglei menu_button(Point2i(), default_toolbar.GetSize());
    if (menu_button.Contains(mouse_pos-bottom_bar_pos)) {
      switch (AnyClick(mouse_pos, CLICK_TYPE_UP, old_click_pos)) {
        case -1: break;
        case 0: return false;
        default: return true;
      }

      switch (mode) {
        case MODE_CONTROL: return ControlClick(mouse_pos, CLICK_TYPE_UP);
        case MODE_NORMAL: return DefaultClick(mouse_pos, CLICK_TYPE_UP);
        case MODE_REPLAY: return ReplayClick(mouse_pos, CLICK_TYPE_UP);
        default: exit(1);
      }
    } else if (ActiveTeam().IsLocalHuman() && weapons_menu.ActionClic(mouse_pos)) {
      // Process click on weapon menu before minimap as it should be
      // overlayed on top of it.
      return true;
    }
    // No button clicked, continue
  } else {
    // Mini-interface drawn, check if we clicked on it
    if (ActiveTeam().IsLocalHuman()) {
      Rectanglei small_button((window.GetWidth() - small_interface.GetWidth())>>1,
                              window.GetHeight() - small_interface.GetHeight(),
                              small_interface.GetWidth(),
                              small_interface.GetHeight());
      if (small_button.Contains(mouse_pos)) {
        return true;
      }
    }
  }

  if (display_minimap) {
    Point2i    offset(window.GetWidth() - GetWorld().ground.GetPreviewSize().x - 2*MARGIN, 2*MARGIN);
    Rectanglei rect_preview(offset, GetWorld().ground.GetPreviewSize());
    if (rect_preview.Contains(mouse_pos)) {
      offset = GetWorld().ground.FromPreviewCoordinates(mouse_pos - offset);
      Camera *cam = Camera::GetInstance();
      cam->SetAutoCrop(false);
      cam->SetXYabs(offset - (cam->GetSize()>>1));
      return true;
    }
  }

  return false;
}

bool Interface::ActionDoubleClick(const Point2i &/*mouse_pos*/) {
  // Drop Bonusbox if double click happened outside interface area
  if (Game::GetInstance()->GetCurrentBox()) {
    Game::GetInstance()->RequestBonusBoxDrop();
  }

  return true;
}

bool Interface::Intersect(const Point2i &mouse_pos)
{
  if (!IsDisplayed())
    return false;

  return Rectanglei(GetMenuPosition(), GetSize()).Contains(mouse_pos);
}

void Interface::MinimapSizeDelta(int delta)
{
  GetWorld().ground.SetPreviewSizeDelta(delta);
}


void HideGameInterface()
{
  Interface *interf = Interface::GetInstance();
  if (interf->GetWeaponsMenu().IsDisplayed() ||
      interf->GetMode()==Interface::MODE_CONTROL ||
      interf->GetMode()==Interface::MODE_REPLAY)
    return;
  Mouse::GetInstance()->Hide();
  interf->Hide();
}
