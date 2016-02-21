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
 * Calculate frame per second.
 *****************************************************************************/

#include "fps.h"
#include <SDL.h>
#include <sstream>
#include <iomanip>
#include "colors.h"
#include "video.h"
#include "text.h"
#include "include/app.h"
#include "tool/i18n.h"

const uint FramePerSecond::MIN_NB_VALUES = 4;

FramePerSecond::~FramePerSecond(){
  delete text;
}

FramePerSecond::FramePerSecond():
  nb_valid_values(-1),
  average(-1),
  nb_frames(),
  time_in_second(0),
  text(NULL),
  display(true)
{
  for( uint i=0; i<=MIN_NB_VALUES; ++i )
    nb_frames.push_back (0);
}

void FramePerSecond::Reset(){
  average = -1;
  nb_frames.clear();

  for( uint i=0; i<=MIN_NB_VALUES; ++i )
    nb_frames.push_back (0);

  time_in_second = SDL_GetTicks()+1000;
  nb_valid_values = -1;

  if(text == NULL)
    text = new Text("");
}

void FramePerSecond::AddOneFrame(){
  ++nb_frames.front();
}

void FramePerSecond::Refresh()
{
  uint nv_temps = SDL_GetTicks();

  // Pas encore l'heure de recalculer : exit !
  if (nv_temps <= time_in_second)
    return;

  // On décale !
  while (time_in_second < nv_temps){
    time_in_second += 1000;
    nb_frames.pop_back();
    nb_frames.push_front(0);
    if (nb_valid_values < (int)nb_frames.size()-1)
      nb_valid_values++;
  }

  // Recalcule la average
  if (0 < nb_valid_values){
    average = 0;
    std::list<uint>::const_iterator it=nb_frames.begin();
    ++it;
    for (int i=1; i<=nb_valid_values; ++i, ++it)
      average += *it;
    average /= nb_valid_values;
  }
}

void FramePerSecond::Draw(){
  if( !display )
    return;
  if( average < 0 )
    return;

  char buffer[20];

  snprintf(buffer, sizeof(buffer)-1, "%.1f", average);
  buffer[sizeof(buffer)-1] = '\0';
  text->Set (Format(_("%s fps"), buffer));
  text->DrawTopRight(AppWormux::GetInstance()->video.window.GetWidth()-1,0);
}

