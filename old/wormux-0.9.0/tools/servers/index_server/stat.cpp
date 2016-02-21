/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Wormux Team.
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
#include <WSERVER_debug.h>
#include "stat.h"
#include "clock.h"
#include "config.h"

Stats stats;

VersionInfo::VersionInfo():
	      servers(0), fake_servers(0), clients(0), clients_w_empty_list(0)
{}

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
  snprintf(time_str, 1024, "%4i-%02i-%02i_",
           1900 + t->tm_year, 1 + t->tm_mon,t->tm_mday);

  full_name = std::string(time_str) + full_name + '_' + filename;

  if (fd) {
    DPRINT(INFO, "Closing previous logfile");
    fclose(fd);
  }

  DPRINT(INFO, "Opening logfile : %s",full_name.c_str());
  fd = fopen(full_name.c_str(), "a+");

  if (fd == NULL)
    PRINT_FATAL_ERROR;

  fprintf(fd, "# YYYY-MM-DD hh-mm-ss "
	  "fake_servers, servers, clients, clients_w_empty_list "
	  "nb_version version-1 fake_servers, servers, clients, clients_w_empty_list "
	  "version-2 fake_servers, servers, clients, clients_w_empty_list "
	  "... version-N fake_servers, servers, clients, clients_w_empty_list\n");
  fflush(fd);
}

void ConnectionStats::CloseFile()
{
  if (fd)
    fclose(fd);
  fd = NULL;
}

void ConnectionStats::Reset()
{
  version_stats.clear();
}

void ConnectionStats::Rotate()
{
  Write();
  CloseFile();
  OpenFile();
}

void ConnectionStats::Write()
{
  if (!fd)
    return;

  struct tm* t;
  time_t now = time(NULL);
  t = localtime(&now);

  fprintf(fd, "%4i-%02i-%02i %02i:%02i:%02i ", 1900 + t->tm_year,t->tm_mon + 1, t->tm_mday,
	  t->tm_hour, t->tm_min, t->tm_sec);

  // Global stats
  unsigned long fake_servers = 0;
  unsigned long servers = 0;
  unsigned long clients = 0;
  unsigned long clients_w_empty_list = 0;
  for (std::map<const std::string, VersionInfo>::const_iterator
	 it = version_stats.begin();
       it != version_stats.end();
       it++) {
    fake_servers += it->second.fake_servers;
    servers += it->second.servers;
    clients += it->second.clients;
    clients_w_empty_list += it->second.clients_w_empty_list;
  }
  fprintf(fd, "%lu %lu %lu %lu", fake_servers, servers, clients, clients_w_empty_list);

  // Stats per version
  fprintf(fd, " %zu ", version_stats.size());

  for (std::map<const std::string, VersionInfo>::const_iterator
	 it = version_stats.begin();
       it != version_stats.end();
       it++) {

    fprintf(fd, "%s %lu %lu %lu %lu ", it->first.c_str(),
	    it->second.fake_servers,
	    it->second.servers,
	    it->second.clients,
	    it->second.clients_w_empty_list);
  }
  fprintf(fd, "\n");

  fflush(fd);
  Reset();
}

void ConnectionStats::NewServer(const std::string& version)
{
  version_stats[version].servers++;
}

void ConnectionStats::NewFakeServer(const std::string& version)
{
  version_stats[version].fake_servers++;
}

void ConnectionStats::NewClient(const std::string& version)
{
  version_stats[version].clients++;
}

void ConnectionStats::NewClientWoAnswer(const std::string& version)
{
  version_stats[version].clients_w_empty_list++;
}


Stats::Stats() : hourly("hourly"), daily("daily")
{
}

void Stats::Init()
{
  daily.OpenFile();
  hourly.OpenFile();
}

void Stats::NewServer(const std::string& version)
{
  hourly.NewServer(version);
  daily.NewServer(version);
}

void Stats::NewFakeServer(const std::string& version)
{
  hourly.NewFakeServer(version);
  daily.NewFakeServer(version);
}

void Stats::NewClient(const std::string& version)
{
  hourly.NewClient(version);
  daily.NewClient(version);
}

void Stats::NewClientWoAnswer(const std::string& version)
{
  hourly.NewClientWoAnswer(version);
  daily.NewClientWoAnswer(version);
}
