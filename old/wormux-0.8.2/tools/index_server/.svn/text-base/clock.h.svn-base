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
 ******************************************************************************/

#ifndef CLOCK_H
#define CLOCK_H
#include <time.h>

class Clock
{
  time_t start_time;
  time_t last_refresh;

  char time_str[1024];
  char date_str[1024];

 public:
  Clock();
  void HandleJobs(bool local=false);
  const char* TimeStr();
  const char* DateStr();
  void ShowUpTime();
};

extern Clock wx_clock;
#endif //CLOCK_H

