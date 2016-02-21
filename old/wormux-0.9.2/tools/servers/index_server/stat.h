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

#ifndef STAT_H
#define STAT_H
#include <stdio.h>
#include <string>
#include <map>

class VersionInfo
{
public:
  unsigned long servers;
  unsigned long fake_servers;
  unsigned long clients;
  unsigned long clients_w_empty_list; // clients for which no game has been proposed

  VersionInfo();
};

class ConnectionStats
{
  FILE* fd;
  std::string filename;
  std::map<const std::string, VersionInfo> version_stats;

  void CloseFile();
 public:
  ConnectionStats(const std::string & fn);
  ~ConnectionStats();
  void OpenFile();
  void Reset();
  void Write();
  void Rotate();

  void NewServer(const std::string& version);
  void NewFakeServer(const std::string& version);
  void NewClient(const std::string& version);
  void NewClientWoAnswer(const std::string& version);
};

class Stats
{
 public:
  // Keeps records of connections
  // done hourly. File is rotated
  // every day
  ConnectionStats hourly;

  // Keeps records of connections
  // done daily. File is never changed
  ConnectionStats daily;

  Stats();
  void Init();
  void NewServer(const std::string& version);
  void NewFakeServer(const std::string& version);
  void NewClient(const std::string& version);
  void NewClientWoAnswer(const std::string& version);
};

extern Stats stats;

#endif
