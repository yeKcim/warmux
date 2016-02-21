/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2009 Wormux Team.
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
#include <WSERVER_clock.h>

static char time_str[16];
static char date_str[16];

BasicClock::BasicClock()
{
  start_time = time(NULL);
}

const char* BasicClock::TimeStr()
{
  struct tm* t;
  time_t now = time(NULL);
  t = localtime(&now);
  snprintf(time_str, 16, "%2i:%02i:%02i", t->tm_hour,
           t->tm_min, t->tm_sec);
  return time_str;
}

const char* BasicClock::DateStr()
{
  struct tm* t;
  time_t now = time(NULL);
  t = localtime(&now);
  snprintf(date_str, 16, "%2i/%02i/%04i", t->tm_mday, t->tm_mon+1, t->tm_year+1900);
  return date_str;
}

const char* BasicClock::UpTimeStr()
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
  snprintf(uptime_str, 256, "Up since %i months, %i days and %i:%02i:%02i hours (%i days)",
	   (int)t,
	   (int)day,
	   (int)hr,
	   (int)min,
	   (int)sec,
	   (int)t*31+day);
  return uptime_str;
}

