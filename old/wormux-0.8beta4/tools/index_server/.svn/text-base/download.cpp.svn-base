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
 ******************************************************************************
 * Download a file using libcurl
 *****************************************************************************/

#include <curl/curl.h>
#include <sys/types.h>
#include <unistd.h>
#include "download.h"
#include "debug.h"
#include "config.h"
#include "../../src/network/index_svr_msg.h"

Downloader downloader;

Downloader::Downloader()
{
  curl = curl_easy_init();
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
  curl_easy_setopt(curl, CURLOPT_FILE, fd);
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, download_callback);
  CURLcode r = curl_easy_perform(curl);
  fclose(fd);

  return (r == CURLE_OK);
}

void DownloadServerList()
{
	// Download the server list from wormux.org
	// and contact every server
	const std::string server_fn = "./server_list";
	int gid, uid;
	bool chroot;
	bool do_fork = true;
	pid_t child = 0;
	config.Get("chroot", chroot);
	config.Get("chroot_gid", gid);
	config.Get("chroot_uid", uid);
	Downloader dl;

	DPRINT(CONN, "Forking process to download the server list");

	do
	{
		const std::string server_fn = "./server_list";
		dl.Get( server_list_url.c_str(), server_fn.c_str() );

		if(chroot)
		{
			if(chown(server_fn.c_str(), (uid_t)uid, (gid_t)gid) == -1)
				TELL_ERROR;
		}

		if(do_fork)
		{
			do_fork = false;
			child = fork();
			if( child == -1)
				TELL_ERROR
			else
			if(child == 0)
				break;
		}
		// Wait 1 day
		sleep(60 * 60 * 24);
	}
	while(child != 0);
}
 
