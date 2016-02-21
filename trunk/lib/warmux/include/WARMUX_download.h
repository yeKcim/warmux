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
  void FillCurlError(int r);
#endif

  static bool FindPair(std::string& value, const std::string& value_name,
                       const std::string& name, const std::string& html);
  static bool FindNameValue(std::string& value, const std::string& name, const std::string& html);

#ifdef HAVE_FACEBOOK
  bool        fb_logged;
  std::string fb_dtsg, post_form_id, form, m_ts;
#endif
#ifdef HAVE_TWITTER
  bool        twitter_logged;
  std::string auth;
#endif

  // Return true if the download was successful
  bool HttpMethod(const std::string& url, std::string* out, int option);
  bool GetUrl(const std::string& url, std::string* out);
  bool Post(const std::string& url, std::string* out, const std::string& fields = "");
  std::string UrlEncode(const std::string& str);

protected:
  friend class Singleton<Downloader>;
  Downloader();
  ~Downloader();

public:
#ifdef HAVE_FACEBOOK
  bool FacebookLogin(const std::string& email, const std::string& pwd);
  bool FacebookStatus(const std::string& text);
#endif
#ifdef HAVE_FACEBOOK
  bool TwitterLogin(const std::string& user, const std::string& pwd);
  bool Tweet(const std::string& text);
#endif
  bool GetLatestVersion(std::string& line);
  bool GetServerList(std::map<std::string, int>& server_lst, const std::string& list_name);

  const std::string& GetLastError() const { return error; };
};

#endif

