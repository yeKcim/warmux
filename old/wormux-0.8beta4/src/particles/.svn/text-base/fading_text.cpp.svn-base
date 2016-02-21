
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
 * Particle Engine
 *****************************************************************************/

#include "particles/fading_text.h"
#include "particles/explosion_smoke.h"
#include "game/time.h"
#include "graphic/font.h"
#include "graphic/sprite.h"
#include "tool/random.h"

const uint INIT_DISPLAY_TIME = 200;
const uint TEXT_FADE_TIME = 1000;

FadingText::FadingText(const std::string & text) : ExplosionSmoke(20)
{
  delete image;
  image = new Sprite(Font::GetInstance(Font::FONT_MEDIUM)->CreateSurface(text, Color(0, 0, 0, 255)));
  start_to_fade = Time::GetInstance()->Read() + INIT_DISPLAY_TIME;
}

void FadingText::Refresh()
{
  if (m_left_time_to_live <= 0) return ;
  uint current_time = Time::GetInstance()->Read();
  m_left_time_to_live--;
  image->Scale(1.0, 1.0);
  if(start_to_fade < current_time) {
    m_left_time_to_live = start_to_fade + TEXT_FADE_TIME - current_time;
    m_left_time_to_live = (m_left_time_to_live > 0 ? m_left_time_to_live : 0);
    SetXY(GetPosition() + Point2i(0, -4));
    image->SetAlpha(1.0 - ((float)(current_time - start_to_fade)) / TEXT_FADE_TIME);
  }
}
