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
 * Download a file using libcurl
 *****************************************************************************/

#include <map>
#include <fstream>
#include <curl/curl.h>
#include "game/config.h"
#include "include/base.h"
#include "network/download.h"
#include "tool/debug.h"
#include "tool/error.h"

Downloader * Downloader::singleton = NULL;

Downloader * Downloader::GetInstance()
{
  if (singleton == NULL) {
    singleton = new Downloader();
  }
  return singleton;
}

Downloader::Downloader():
  curl(curl_easy_init())
{
}

Downloader::~Downloader()
{
  curl_easy_cleanup(curl);
}

size_t download_callback(void* buf, size_t size, size_t nmemb, void* fd)
{
  fwrite(buf, size, nmemb, (FILE*)fd);
  return nmemb;
}

bool Downloader::Get(const char* url, const char* save_as)
{
  FILE* fd = fopen( save_as, "w");
  if (fd == NULL) {
    perror("Downloader::Get");
    printf("\t%s\n\n", save_as);
    return false;
  }
  curl_easy_setopt(curl, CURLOPT_FILE, fd);
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, download_callback);
  CURLcode r = curl_easy_perform(curl);
  fclose(fd);

  return (r == CURLE_OK);
}

static ssize_t getline(std::string& line, std::ifstream& file)
{
	  line.clear();
	    std::getline(file, line);
	      if(file.eof())
		          return -1;
	        return line.size();
}

std::map<std::string, int> Downloader::GetServerList(std::string list_name)
{
  std::map<std::string, int> server_lst;
  MSG_DEBUG("downloader", "Retrieving server list: %s", list_name.c_str());

  // Download the list of server
  const std::string server_file = Config::GetInstance()->GetPersonalDir() + list_name;
  const std::string list_url = "http://www.wormux.org/" + list_name;

  if( !Get(list_url.c_str(), server_file.c_str()) )
    return server_lst;

  // Parse the file
  std::ifstream fin;
  fin.open(server_file.c_str(), std::ios::in);
  if(!fin)
   return server_lst;

  /*char * line = NULL;
  size_t len = 0;*/
  std::string line;

  // GNU getline isn't available on *BSD and Win32, so we use a new function, see getline above
  while (getline(line, fin) >= 0)
  {
    if(line.at(0) == '#' || line.at(0) == '\n' || line.at(0) == '\0')
      continue;

    std::string::size_type port_pos = line.find(':', 0);
    if(port_pos == std::string::npos)
      continue;

    std::string hostname = line.substr(0, port_pos);
    std::string portstr = line.substr(port_pos+1);
    int port = atoi(portstr.c_str());

    server_lst[ hostname ] = port;
  }

  fin.close();

  MSG_DEBUG("downloader", "Server list retrieved. %i servers are running", server_lst.size());

  return server_lst;
}

