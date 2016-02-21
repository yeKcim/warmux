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
#include <cerrno>
#include <cstdio>
#include <map>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <ctype.h>

#include <WARMUX_debug.h>
#include <WARMUX_download.h>
#include <WARMUX_error.h>
#include <WARMUX_i18n.h>
#include <WARMUX_file_tools.h>

static size_t dummy_callback(void*, size_t size, size_t nmemb, void*)
{
  return size*nmemb;
}
#ifdef HAVE_LIBCURL
# include <curl/curl.h>

static size_t download_callback(void* buf, size_t size, size_t nmemb, void* str)
{
  std::string* out = (std::string*)str;
  size_t       sz  = size*nmemb;

  out->append((char*)buf, sz);
  return sz;
}

Downloader::Downloader()
{
#ifdef HAVE_FACEBOOK
  fb_logged = false;
#endif
#ifdef HAVE_TWITTER
  twitter_logged = false;
#endif

#if defined(HAVE_FACEBOOK) || defined(HAVE_TWITTER)
  curl_global_init(CURL_GLOBAL_ALL);
#else
#  ifdef WIN32
  curl_global_init(CURL_GLOBAL_WIN32);
#  else
  curl_global_init(CURL_GLOBAL_NOTHING);
#  endif
#endif

  curl = curl_easy_init();
  curl_error_buf = new char[CURL_ERROR_SIZE];
  curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_error_buf);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
  curl_easy_setopt(curl, CURLOPT_CAINFO, NULL);
  curl_easy_setopt(curl, CURLOPT_CAPATH, NULL); 
  curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.1.3) Gecko/20070309 Firefox/2.0.0.3");
}

Downloader::~Downloader()
{
  curl_easy_cleanup(curl);

  curl_global_cleanup();
  delete[] curl_error_buf;
}

void Downloader::FillCurlError(int r)
{
  error = std::string(curl_error_buf);
  if (error.empty()) {
    snprintf(curl_error_buf, CURLOPT_ERRORBUFFER-1, "Unknown Curl error no.%i", r);
    error = std::string(curl_error_buf);
  }
}

bool Downloader::HttpMethod(const std::string& url, std::string* out, int option)
{
  curl_easy_setopt(curl, (CURLoption)option, 1);
  if (out) {
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, out);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, download_callback);
  } else {
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, dummy_callback);
  }
  int r = curl_easy_perform(curl);
  if (CURLE_OK == r)
    return true;

  FillCurlError(r);
  return false;
}

bool Downloader::GetUrl(const std::string& url, std::string* out)
{
  return HttpMethod(url, out, CURLOPT_HTTPGET);
}

bool Downloader::Post(const std::string& url, std::string* out, const std::string& fields)
{
  if (!fields.empty())
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, fields);
  return HttpMethod(url, out, CURLOPT_POST);
}

#elif defined(ANDROID)
#include <cctype>
# include <jni.h>

#ifndef SDL_JAVA_PACKAGE_PATH
# error SDL_JAVA_PACKAGE_PATH undefined!
#endif
#define JAVA_EXPORT_NAME2(name,package) Java_##package##_##name
#define JAVA_EXPORT_NAME1(name,package) JAVA_EXPORT_NAME2(name,package)
#define JAVA_EXPORT_NAME(name) JAVA_EXPORT_NAME1(name,SDL_JAVA_PACKAGE_PATH)

extern "C" {

static JavaVM    *vm        = NULL;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *_vm, void *reserved)
{
  vm = _vm;
  return JNI_VERSION_1_2;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *_vm, void *reserved)
{
  vm = _vm;
}

static JNIEnv    *env       = NULL;
static jmethodID  FetchURL  = NULL;
static jmethodID  Post      = NULL;
static jobject    dler      = NULL;

void
JAVA_EXPORT_NAME(URLDownloader_nativeInitCallbacks)(JNIEnv * libEnv, jobject thiz)
{
  env  = libEnv;
  dler = env->NewGlobalRef(thiz); //Never freed!
  jclass dlerClass = env->GetObjectClass(dler);
  FetchURL = env->GetMethodID(dlerClass, "FetchURL", "(Ljava/lang/String;)[B");
  Post     = env->GetMethodID(dlerClass, "PostURL", "(Ljava/lang/String;Ljava/lang/String;)[B");
}

};

Downloader::Downloader() { }
Downloader::~Downloader() { }
bool Downloader::GetUrl(const std::string& url, std::string* out)
{
  bool       ret    = false;
  jboolean   isCopy = JNI_FALSE;
  int        written;

  // Attach to avoid: "JNI ERROR: non-VM thread making JNI calls"
  // Now make sure to properly detach even in case of error
  vm->AttachCurrentThread(&env, NULL);

  // No explicit deallocation needed
  jstring    jurl   = env->NewStringUTF(url.c_str());
  jbyteArray buffer = (jbyteArray)env->CallObjectMethod(dler, FetchURL, jurl);
  int        len    = env->GetArrayLength(buffer);
  jbyte     *ptr;

  if (!len) {
    error = Format(_("Read only %i bytes"), len);
    goto end;
  }

  ptr = env->GetByteArrayElements(buffer, &isCopy);
  if (!ptr) {
    error = _("No pointer");
    goto end;
  }

  if (len) {
    out->append((char*)ptr, sizeof(jbyte)*len);
    if (isCopy == JNI_TRUE)
      env->ReleaseByteArrayElements(buffer, ptr, 0);
    ret = true;
  } else {
    error = Format(_("Wrote %i/%i bytes"), written, len);
  }

end:
  // Done with JNI calls, detach
  vm->DetachCurrentThread();
  return ret;
}

bool Downloader::Post(const std::string& url, std::string* out, const std::string& fields)
{
  bool       ret    = false;
  jboolean   isCopy = JNI_FALSE;
  int        written;

  // Attach to avoid: "JNI ERROR: non-VM thread making JNI calls"
  // Now make sure to properly detach even in case of error
  vm->AttachCurrentThread(&env, NULL);

  // No explicit deallocation needed
  jstring    jurl   = env->NewStringUTF(url.c_str());
  jstring    jfield = env->NewStringUTF((fields.empty()) ? "" : fields.c_str());
  jbyteArray buffer = (jbyteArray)env->CallObjectMethod(dler, FetchURL, jurl, jfield);
  int        len    = env->GetArrayLength(buffer);
  jbyte     *ptr;

  if (!len) {
    error = Format(_("Read only %i bytes"), len);
    goto end;
  }

  ptr = env->GetByteArrayElements(buffer, &isCopy);
  if (!ptr) {
    error = _("No pointer");
    goto end;
  }

  if (len) {
    out->append((char*)ptr, sizeof(jbyte)*len);
    if (isCopy == JNI_TRUE)
      env->ReleaseByteArrayElements(buffer, ptr, 0);
    ret = true;
  } else {
    error = Format(_("Wrote %i/%i bytes"), written, len);
  }

end:
  // Done with JNI calls, detach
  vm->DetachCurrentThread();
  return ret;
}

#else  // waiting for an alternate implementation
Downloader::Downloader() { }
Downloader::~Downloader() { }
bool Downloader::GetUrl(const std::string& /*url*/, std::string* /*out*/) { return false; }
bool Downloader::GetPost(const std::string& /*url*/, std::string* /*out*/, const std::string& /*fields*/) { return false; }
#endif

std::string Downloader::UrlEncode(const std::string& str)
{
  std::string ret;
  uint        len = str.size();
#ifdef HAVE_CURL
  char       *ue_str;
  ue_str = curl_easy_escape(curl, str.c_str(), str.size());
  ret = ue_str;
  free(ue_str);
  return ret;
#else
  static const char digits[] = "0123456789ABCDEF";
  const char*       src      = str.c_str();

  while (len && *src) {
    unsigned char ch = (unsigned char)*src;
    if (*src == ' ') {
      ret += '+';
    }
    else if (isalnum(ch) || strchr("-_.!~*'()", ch)) {
      ret += *src;
    }
    else {
      ret += '%';
      ret += digits[(ch >> 4) & 0x0F];
      ret += digits[       ch & 0x0F];
    }  
    src++;
  }
  ret += '\0';
#endif
  return ret;
}

bool Downloader::GetLatestVersion(std::string& line)
{
  static const char url[] = "http://www.wormux.org/last";
  error.clear();
  if (!GetUrl(url, &line)) {
    if (error.empty())
      error = Format(_("Couldn't fetch last version from %s"), url);
    fprintf(stderr, "%s\n", error.c_str());
    return false;
  }

  return true;
}

bool Downloader::GetServerList(std::map<std::string, int>& server_lst, const std::string& list_name)
{
  MSG_DEBUG("downloader", "Retrieving server list: %s", list_name.c_str());

  // Download the list of server
  const std::string list_url = "http://www.wormux.org/" + list_name;
  std::string       list_line;

  error.clear();

  if (!GetUrl(list_url.c_str(), &list_line))
    return false;
  MSG_DEBUG("downloader", "Received '%s'", list_line.c_str());

  // Parse the file
  std::stringstream list(list_line);
  std::string       line;

  while (std::getline(list, line)) {
    if (line.at(0) == '#'
        || line.at(0) == '\n'
        || line.at(0) == '\0')
      continue;

    std::string::size_type port_pos = line.find(':', 0);
    if (port_pos == std::string::npos)
      continue;

    std::string hostname = line.substr(0, port_pos);
    std::string portstr = line.substr(port_pos+1);
    int port = atoi(portstr.c_str());
    MSG_DEBUG("downloader", "Received %s:%i", hostname.c_str(), port);

    server_lst[ hostname ] = port;
  }

  MSG_DEBUG("downloader", "Server list retrieved. %u servers are running",
            (uint)server_lst.size());

  return true;
}

bool Downloader::FindPair(std::string& value, const std::string& value_name,
                          const std::string& name, const std::string& html)
{
  size_t s = html.find(name);
  if (s == std::string::npos)
    return false;
  s += name.size();
  s = html.find(value_name + "=\"", s);
  if (s == std::string::npos)
    return false;
  s += value_name.size() + 2;
  size_t e = html.find('"', s);
  if (e == std::string::npos || e <= s)
    return false;
  value = html.substr(s, e-s);
  return true;
}
bool Downloader::FindNameValue(std::string& value, const std::string& name, const std::string& html)
{
  return FindPair(value, "value", "name=\"" + name + "\"", html);
}


#ifdef HAVE_FACEBOOK
bool Downloader::FacebookLogin(const std::string& email, const std::string& pwd)
{
  if (fb_logged)
    return true;
  std::string html, fields;

  fb_logged = false;
  if (!GetUrl("http://m.facebook.com/login.php?http&refsrc=http%3A%2F%2Fm.facebook.com%2F&no_next_msg&refid=8", &html)) {
    goto end;
  }
  MSG_DEBUG("downloader", "Login connect success!");
  
  // Find m_ts value
  if (!FindNameValue(m_ts, "m_ts", html)) {
    error = "Can't find m_ts";
    goto end;
  }
  MSG_DEBUG("downloader", "m_ts=%s", m_ts.c_str());

  // Find post_form_id value
  if (!FindNameValue(post_form_id, "post_form_id", html)) {
    error = "Can't find post_form_id";
    goto end;
  } 
   MSG_DEBUG("downloader", "post_form_id=%s", post_form_id.c_str());
  
  // Find form
  if (!FindPair(form, "action", "id=\"login_form\"", html)) {
    error = "Can't find form";
    goto end;
  } 
  MSG_DEBUG("downloader", "form=%s", form.c_str());

  html.clear();

  form = "http://m.facebook.com" + form;
  fields = "lsd=&post_form_id=" + post_form_id +
           "&version=1&ajax=0&pxr=0&gps=0&email=" + UrlEncode(email) +
           "&pass=" + UrlEncode(pwd) + "&m_ts=" + m_ts + "&login=Login";
  MSG_DEBUG("downloader", "Fields: %s\n", fields.c_str());
  if (!Post(form.c_str(), &html, fields.c_str())) {
    goto end;
  }
  if (html.find("abb acr aps") != std::string::npos) {
    error = _("Login error, probably invalid email or password");
    goto end;
  }
  MSG_DEBUG("downloader", "Login success!\n");

  // Find form
  if (!FindPair(form, "action", "id=\"composer_form\"", html)) {
    error = "Can't find form";
    goto end;
  } 
  MSG_DEBUG("downloader", "form=%s", form.c_str());
  
  // Find fb_dtsg
  if (!FindNameValue(fb_dtsg, "fb_dtsg", html)) {
    error = "Can't find fb_dtsg";
    goto end;
  } 
  MSG_DEBUG("downloader", "fb_dtsg=%s", fb_dtsg.c_str());

  // Find post_form_id value
  if (!FindNameValue(post_form_id, "post_form_id", html)) {
    error = "Can't find post_form_id";
    goto end;
  } 
  MSG_DEBUG("downloader", "post_form_id=%s", post_form_id.c_str());
  
  form = "http://m.facebook.com" + form;
  html.clear();

  fb_logged = true;

end:
  if (!fb_logged && IsLOGGING("download")) {
    FILE *f = fopen("out.htm", "wt");
    fwrite(html.c_str(), html.size(), 1, f);
    fclose(f);
    MSG_DEBUG("downloader", "Login failed: %s\n", error.c_str());
  }

  return fb_logged;
}

bool Downloader::FacebookStatus(const std::string& text)
{
  if (!fb_logged)
    return false;
  std::string txt = "fb_dtsg=" + fb_dtsg + "&post_form_id=" + post_form_id +
                    "&status=" + UrlEncode(text) + "&update=Update&target=";
  MSG_DEBUG("downloader", "fields=%s", txt.c_str());
  return Post(form.c_str(), NULL, txt.c_str());
}
#endif

#ifdef HAVE_TWITTER
bool Downloader::TwitterLogin(const std::string& user, const std::string& pwd)
{
  if (twitter_logged)
    return true;
  std::string html, fields;

  twitter_logged = false;
  if (!GetUrl("http://mobile.twitter.com/session/new", &html)) {
    goto end;
  }
  MSG_DEBUG("downloader", "Login connect success!");

  // Find authenticity_token value
  if (!FindNameValue(auth, "authenticity_token", html)) {
    error = _("Can't find authenticity_token");
    goto end;
  }
  MSG_DEBUG("downloader", "authenticity_token=%s", auth.c_str());

  html.clear();

  fields = "authenticity_token=" + auth + "&username=" + UrlEncode(user) +
           "&password=" + UrlEncode(pwd);
  MSG_DEBUG("downloader", "Fields: %s\n", fields.c_str());
  if (!Post("http://mobile.twitter.com/session", &html, fields)) {
    goto end;
  }
#if 0
  if (html.find("class=\"warning\"") != std::string::npos) {
    printf("Login failed, probably incorrect credentials\n");
    goto end;
  }
#endif
  MSG_DEBUG("downloader", "Login success!\n");
  
  html.clear();

  twitter_logged = true;

end:
  if (!twitter_logged && IsLOGGING("download")) {
    FILE *f = fopen("out.htm", "wt");
    fwrite(html.c_str(), html.size(), 1, f);
    fclose(f);
    MSG_DEBUG("downloader", "Login failed: %s\n", error.c_str());
  }

  return twitter_logged;
}

bool Downloader::Tweet(const std::string& text)
{
  if (!twitter_logged)
    return false;
  std::string txt = "authenticity_token=" + auth + "&tweet%5Btext%5D=" + UrlEncode(text);
  MSG_DEBUG("downloader", "fields=%s", txt.c_str());
  return Post("https://mobile.twitter.com/", NULL, txt);
}
#endif
