/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
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
 * Download a file using libcurl
 *****************************************************************************/

#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <stdio.h> // for FILE...
#include <map>
#include <string>

// Load config about download librart (curl, ...)
#include "WARMUX_config.h"

#include <WARMUX_singleton.h>

class Downloader : public Singleton<Downloader>
{
  std::string error;

#ifdef HAVE_LIBCURL
  void* curl;
  char* curl_error_buf;
#endif

  // Return true if the download was successful
  bool Get(const char* url, FILE* file);

protected:
  friend class Singleton<Downloader>;
  Downloader();
  ~Downloader();

public:
  bool GetLatestVersion(std::string& line);
  bool GetServerList(std::map<std::string, int>& server_lst, const std::string& list_name);

  const std::string& GetLastError() const { return error; };
};

#endif

