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
#include <WSERVER_debug.h>
#include "clock.h"
#include "sync_slave.h"
#include "stat.h"

Clock wx_clock;

Clock::Clock() : BasicClock()
{
  last_refresh = time(NULL);
}

void Clock::HandleJobs(bool local)
{
  if (time(NULL) == last_refresh)
    return;

  // Every day
  if (time(NULL) % (60 * 60 * 24) == 0) {
    DPRINT(INFO, "%s", UpTimeStr());
    DPRINT(INFO, "Day changed to : %s", DateStr());
    stats.hourly.Rotate();
    stats.daily.Write();
  }
  // Every hour
  else if (time(NULL) % (60 * 60) == 0) {
    stats.hourly.Write();
  }

  // Refresh connections to the servers every minutes
  else if (time(NULL) % 60 == 0 && !local) {
    sync_slave.Start();
  }

  last_refresh = time(NULL);
}
