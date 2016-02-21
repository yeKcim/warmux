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

#include <time.h>
#include <stdio.h>
#include "clock.h"
#include "sync_slave.h"
#include "debug.h"
#include "stat.h"

Clock wx_clock;

Clock::Clock()
{
  start_time = time(NULL);
  last_refresh = time(NULL);
}

void Clock::HandleJobs(bool local)
{
  if(time(NULL) == last_refresh)
    return;


  // Every day
  if(time(NULL) % (60 * 60 * 24) == 0)
    {
      ShowUpTime();
      DPRINT(INFO, "Day changed to : %s", DateStr());
      stats.hourly.Rotate();
      stats.daily.Write();
    }
  // Every hour
  else if(time(NULL) % (60 * 60) == 0)
    {
      stats.hourly.Write();
    }
  // Refresh connections to the servers every minutes
  else if(time(NULL) % 60 == 0 && !local)
    {
      sync_slave.Start();
    }

  last_refresh = time(NULL);
}

const char* Clock::TimeStr()
{
  struct tm* t;
  time_t now = time(NULL);
  t = localtime(&now);
  snprintf(time_str, 1024, "%2i:%02i:%02i", t->tm_hour,
           t->tm_min, t->tm_sec);
  return time_str;
}

const char* Clock::DateStr()
{
  struct tm* t;
  time_t now = time(NULL);
  t = localtime(&now);
  snprintf(date_str, 1024, "%2i/%02i/%04i", t->tm_mday, t->tm_mon+1, t->tm_year+1900);
  return date_str;
}

void Clock::ShowUpTime()
{
  double d = difftime(time(NULL), start_time);

  unsigned long t = (unsigned long) d;
  unsigned short sec, min, hr, day;
  sec = (unsigned short) t % 60;
  t /= 60;
  min = (unsigned short) t % 60;
  t /= 60;
  hr = (unsigned short) t % 24;
  t /= 24;
  day = (unsigned short) t % 31;
  t /= 31;
  DPRINT(INFO, "Up since %i months, %i days and %i:%02i:%02i hours (%i days)",
         (int)t,
         (int)day,
         (int)hr,
         (int)min,
         (int)sec,
         (int)t*31+day);
}

