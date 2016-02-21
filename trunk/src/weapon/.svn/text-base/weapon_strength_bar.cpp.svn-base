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
 * Weapon Strength bar.
 *****************************************************************************/

#include "weapon/weapon_strength_bar.h"
#include "graphic/color.h"

#include "graphic/sprite.h"
#include "graphic/polygon_generator.h"
#include "graphic/video.h"

#include "map/map.h"
#include "tool/resource_manager.h"
#include "team/team.h"
#include "team/teams_list.h"
#include "character/character.h"
#include "graphic/colors.h"
#include "graphic/polygon.h"
#include "game/game.h"


WeaponStrengthBar::WeaponStrengthBar() :
  ProgressBar(),
  visible(false),
  m_box(NULL),
  last_fire(NULL),
  m_item_last_fire(NULL)
{
}

WeaponStrengthBar::~WeaponStrengthBar()
{
  delete last_fire;
  delete m_box;
}

void WeaponStrengthBar::InitPos(uint px, uint py, uint pwidth, uint pheight)
{
  ProgressBar::InitPos(px, py, pwidth, pheight);

  Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml", false);
  if (last_fire)
    delete last_fire;
  last_fire = new Sprite(LOAD_RES_IMAGE("interface/weapon_strength_bar_last_fire"));

  if (m_box)
    delete m_box;
  m_box = PolygonGenerator::GenerateDecoratedBox(pwidth, pheight);
  m_box->SetStyle(DecoratedBox::STYLE_SQUARE);
}

static uint ScaleStrengthtoUInt(Double strength)
{
  return int(100 * strength);
}

void WeaponStrengthBar::FetchData()
{
  const Weapon & weapon = ActiveTeam().GetWeapon();
  Double max_strength = weapon.GetMaxStrength();
  bool playing = (Game::GetInstance()->ReadState() == Game::PLAYING);
  visible = playing && max_strength.IsNotZero();
  if (!visible)
    return;

  Double strength = weapon.GetStrength();
  uint value = ScaleStrengthtoUInt(strength);
  uint min_value = 0;
  uint max_value = ScaleStrengthtoUInt(max_strength);

  // prepare the strength bar
  InitVal(value, min_value, max_value);

  // init stamp on the stength_bar
  Double previous_strength = ActiveCharacter().previous_strength;
  ResetTag();
  if (ZERO < previous_strength && previous_strength < max_strength) {
    uint previous_value = ScaleStrengthtoUInt(previous_strength);
    AddTag(previous_value, primary_red_color);
  }
}

// TODO pass a Surface as parameter
void WeaponStrengthBar::DrawXY(const Point2i &pos) {
  int begin, end;

  FetchData();
  if (!visible)
    return;

  // Clear image
  Color clear(0,0,0,0);
  image.Fill(clear);

  // Valeur
  if (m_use_ref_val) {
    int ref = ComputeBarValue (m_ref_val);
    if (val < m_ref_val) { // FIXME hum, this seems buggy
      begin = 1+bar_value;
      end = 1+ref;
    } else {
      begin = 1+ref;
      end = 1+bar_value;
    }
  } else {
    begin = 1;
    end = 1+bar_value;
  }

  Color bar_color = ComputeValueColor(val);

  Rectanglei r_value;
  if (orientation == PROG_BAR_HORIZONTAL) {
    r_value = Rectanglei(begin, 1, end - begin, height - 2);
  } else {
    r_value = Rectanglei(1, height - end + begin - 1, width - 2, end -1);
  }

  image.FillRect(r_value, bar_color);

  // marks
  mark_it_const it=mark.begin(), it_end = mark.end();
  for (; it != it_end; ++it) {
    Point2i p_marq(1+it->val, height/2);

    if (m_item_last_fire) {
      m_box->DelItem(0);
      delete m_item_last_fire;
    }

    PolygonItem * item = new PolygonItem(last_fire, p_marq);
    const_cast<PolygonItem *&>(m_item_last_fire) = item;
    m_box->AddItem(m_item_last_fire);
  }

  Rectanglei dst(pos.x, pos.y, width, height);
  GetMainWindow().Blit(image, pos);

  m_box->SetPosition(pos.x,pos.y);
  m_box->DrawOnScreen();

  GetWorld().ToRedrawOnScreen(dst);
}

Color WeaponStrengthBar::ComputeValueColor(int val) const
{
  Color start_color(242,239,22,255);
  Color end_color(160,0,0,255);

  int scale = (val << 16)/(max-min);

#define SCALE_COL(fun)   \
  (((end_color.fun - start_color.fun)*scale)>>16) + start_color.fun

  int r = SCALE_COL(GetRed());
  int g = SCALE_COL(GetGreen());
  int b = SCALE_COL(GetBlue());
  int a = SCALE_COL(GetAlpha());

  return Color(r,g,b,a);
}
