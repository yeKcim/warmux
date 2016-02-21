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
 * energy bar of each team
 *****************************************************************************/

#include "team_energy.h"
#include <sstream>
#include <math.h>
#include "map/camera.h"
#include "map/map.h"
#include "game/time.h"
#include "graphic/text.h"
#include "graphic/sprite.h"
#include "team.h"
#include "teams_list.h"
#include "include/app.h"

const uint BAR_WIDTH = 13;
const uint BAR_SPACING = 30;
const uint BAR_HEIGHT = 50;
// XXX Not used
//const uint SPACING = 3;

const uchar ALPHA = 127;
const uchar BACK_ALPHA = 0;

const float MOVE_DURATION = 750.0;

void EnergyList::Reset()
{
  for (EnergyList::iterator it = begin(); it != end(); ++it)
    delete *(it);

  clear();
}

// Let's assume it is sorted, in spite of uint wrap-around
void EnergyList::AddValue(uint value)
{
  if (value == m_last_value)
    return;

  EnergyValue *eval = new EnergyValue(Time::GetInstance()->Read(), value);
  if (value > m_max_value) m_max_value = value;
  m_last_value = value;
  EnergyList::push_back(eval);
}


TeamEnergy::TeamEnergy(Team * _team):
  energy_bar(),
  value(0),
  new_value(0),
  max_value(0),
  team(_team),
  icon(NULL),
  t_team_energy(new Text("None", black_color,
                         Font::FONT_SMALL, Font::FONT_NORMAL, false)),
  dx(0),
  dy(0),
  rank(0),
  new_rank(0),
  team_name("not initialized"),
  move_start_time(0),
  rank_tmp(0),
  status(EnergyStatusOK),
  energy_list()
{
  energy_bar.InitPos(0, 0, BAR_WIDTH, BAR_HEIGHT);
  energy_bar.SetBorderColor(Color(255, 255, 255, ALPHA));
  energy_bar.SetBackgroundColor(Color(255*6/10, 255*6/10, 255*6/10, BACK_ALPHA));
}

TeamEnergy::~TeamEnergy()
{
  if(icon) delete icon;
  if(t_team_energy) delete t_team_energy;
}

void TeamEnergy::Config(uint _current_energy,
                        uint _max_energy)
{
  max_value = _max_energy;

  value = _current_energy;
  new_value = _current_energy;
  ASSERT(max_value != 0)
  energy_bar.InitVal(value, 0, max_value, ProgressBar::PROG_BAR_VERTICAL);
  icon = NULL;
  SetIcon(team->GetFlag());
  energy_list.Reset();
}

void TeamEnergy::SetIcon(const Surface & new_icon)
{
  if(icon)
    delete icon;
  icon = new Sprite(new_icon);
  icon->Scale(0.8,0.8);
}

void TeamEnergy::Refresh()
{
  switch(status)
  {
    // energy value from one team have changed
    case EnergyStatusValueChange:
      if(new_value > value)
        value = new_value;
      if(value > new_value)
        --value;
      if(value == new_value)
        status = EnergyStatusWait;
      break;

    // ranking is changing
    case EnergyStatusRankChange:
      Move();
      break;

      // Currently no move
    case EnergyStatusOK:
      if( value != new_value && !IsMoving())
        status = EnergyStatusValueChange;
      else
        if( rank != new_rank )
          status = EnergyStatusRankChange;
      break;

    // This energy bar wait others bar before moving
    case EnergyStatusWait:
      break;
  }
}

void TeamEnergy::Draw(const Point2i& pos)
{
  energy_bar.Actu(value);
  Point2i tmp = pos + Point2i(BAR_SPACING / 2 + rank * (BAR_WIDTH + BAR_SPACING) + dx, dy);
  energy_bar.DrawXY(tmp);
  icon->DrawXY(tmp + Point2i(energy_bar.GetWidth() / 2, 0));
}

void TeamEnergy::SetValue(uint new_energy)
{
  if(new_energy == 0)
    SetIcon(team->GetDeathFlag());
  new_value = new_energy;
  energy_list.AddValue(new_energy);
}

void TeamEnergy::SetRanking(uint _rank)
{
  rank = _rank;
  new_rank = _rank;
}

void TeamEnergy::NewRanking(uint _new_rank)
{
  new_rank = _new_rank;
}

// Move energy bar (change in ranking)
void TeamEnergy::Move()
{
  if( value != new_value && !IsMoving()) {
    // Other energy bar are moving so waiting for others to move
    status = EnergyStatusWait;
    return;
  }

  if( rank == new_rank && !IsMoving()) {
    // Others energy bar are moving
    status = EnergyStatusWait;
    return;
  }

  // teams ranking have changed
  Time * global_time = Time::GetInstance();
  if( rank != new_rank )
  {
    if(move_start_time == 0)
      move_start_time = global_time->Read();

    dx = (int)(((float)new_rank - rank) * (BAR_WIDTH + BAR_SPACING) * ((global_time->Read() - move_start_time) / MOVE_DURATION));

    // displacement in arc of circle only when losing place ranking
    if( new_rank > rank ) {
      dy = (int)((BAR_HEIGHT * ((float)rank - new_rank)) * 0.5 *
           sin( M_PI * ((global_time->Read() - move_start_time) / MOVE_DURATION)));
    } else {
      dy = (int)((BAR_HEIGHT * ((float)rank - new_rank)) * 0.5 *
          sin( M_PI * ((global_time->Read() - move_start_time) / MOVE_DURATION)));
    }
    // End of movement ?
    if( (global_time->Read() - move_start_time) > MOVE_DURATION)
      FinalizeMove();
  } else {
    // While moving, it came back to previous place in ranking
    dy = (int)((float)dy - ((global_time->Read() - move_start_time) / MOVE_DURATION) * dy);
    dx = (int)((float)dx - ((global_time->Read() - move_start_time) / MOVE_DURATION) * dx);
  }
}

// Move energy bar immediatly to there final destination
void TeamEnergy::FinalizeMove()
{
  dy = 0;
  dx = 0;
  rank = new_rank;
  move_start_time = 0;
  status = EnergyStatusWait;
  return;
}

bool TeamEnergy::IsMoving () const
{
  if( dx != 0 || dy != 0 )
    return true;
  return false;
}
