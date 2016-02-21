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

#include <stdio.h>
#include <string>
#include "stat.h"
#include "clock.h"
#include "debug.h"
#include "config.h"

Stats stats;

ConnectionStats::ConnectionStats(const std::string & fn)
  : fd(NULL)
{
  Reset();
  filename = fn;
}

ConnectionStats::~ConnectionStats()
{
  Write();
  CloseFile();
}

void ConnectionStats::OpenFile()
{
  std::string full_name;
  config.Get("my_hostname", full_name);

  struct tm* t;
  time_t now = time(NULL);
  t = localtime(&now);
  char time_str[1024];
  snprintf(time_str, 1024, "%4i-%02i-%02i_", 1900 + t->tm_year, 1 + t->tm_mon,t->tm_mday);

  full_name = std::string(time_str) + full_name + '_' + filename;

  if(fd)
    {
      DPRINT(INFO, "Closing previous logfile");
      fclose(fd);
    }

  DPRINT(INFO, "Opening logfile : %s",full_name.c_str());
  fd = fopen(full_name.c_str(), "a+");

  if(fd == NULL)
    TELL_ERROR;
}

void ConnectionStats::CloseFile()
{
  if (fd)
    fclose(fd);
  fd = NULL;
}

void ConnectionStats::Reset()
{
  servers = 0;
  fake_servers = 0;
  clients = 0;
}

void ConnectionStats::Rotate()
{
  Write();
  CloseFile();
  OpenFile();
}

void ConnectionStats::Write()
{
  if(!fd)
    return;
  struct tm* t;
  time_t now = time(NULL);
  t = localtime(&now);
  fprintf(fd, "%4i-%02i-%02i %i:%02i:%02i %lu %lu %lu\n", 1900 + t->tm_year,t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, fake_servers, servers, clients);

  fflush(fd);
  Reset();
}

Stats::Stats() : hourly("hourly"), daily("daily")
{
}

void Stats::Init()
{
  daily.OpenFile();
  hourly.OpenFile();
}

void Stats::NewServer()
{
  hourly.servers++;
  daily.servers++;
}

void Stats::NewFakeServer()
{
  hourly.fake_servers++;
  daily.fake_servers++;
}

void Stats::NewClient()
{
  hourly.clients++;
  daily.clients++;
}

