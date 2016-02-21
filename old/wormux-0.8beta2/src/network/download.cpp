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

#include <curl/curl.h>
#include "include/base.h"
#include "tool/error.h"
#include "download.h"

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
