/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
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
 *****************************************************************************
 * Retrieve string pasted depending on OS mechanisms.
 *****************************************************************************/
#ifdef _MSC_VER
#  include "msvc/config.h"
#elif defined(HAVE_CONFIG_H)
#  include "config.h"
#endif

#include <SDL_syswm.h>
#include "copynpaste.h"

#ifdef WIN32
bool RetrieveBuffer(std::string& text, std::string::size_type& pos)
{
  bool  ret = false;

  if (!OpenClipboard(NULL))
    return false;

  HANDLE  h = GetClipboardData(CF_UNICODETEXT);
  if (h)
  {
    LPCWSTR data = (LPCWSTR)GlobalLock(h);

    if (data)
    {
      int len = WideCharToMultiByte(CP_UTF8, 0, data, -1, NULL, 0, NULL, NULL);
      if (len > 0)
      {
        // Convert from UTF-16 to UTF-8
        void *temp = malloc(len);
        if (WideCharToMultiByte(CP_UTF8, 0, data, -1, (LPSTR)temp, len, NULL, NULL))
        {
          text.insert(pos, (char*)temp);
          pos += len-1;
        }
        free(temp);
        ret = true;
      }
    }
    GlobalUnlock(h);
  }
  else
  {
    h = GetClipboardData(CF_TEXT);

    if (h)
    {
      const char *data = (char*)GlobalLock(h);
      if (data)
      {
        text.insert(pos, data);
        pos += strlen(data);
        ret = true;
      }
      GlobalUnlock(h);
    }
  }

  CloseClipboard();
  return ret;
}
#elif defined(__APPLE__)

#ifdef Status
#undef Status
#endif

#include <Carbon/Carbon.h>

bool RetrieveBuffer(std::string& text, std::string::size_type& pos)
{
  ScrapRef scrap;
  if (::GetCurrentScrap(&scrap) != noErr)
    return false;

  SInt32   byteCount = 0;
  OSStatus status    = ::GetScrapFlavorSize(scrap, kScrapFlavorTypeText, &byteCount);
  if (status != noErr)
    return false;

  char *buffer = new char[byteCount];
  bool ret     = ::GetScrapFlavorData(scrap, kScrapFlavorTypeText, &byteCount, buffer) == noErr;
  if (ret)
  {
    text.insert(pos, buffer); 
    pos += strlen(buffer);
  }
  delete[] buffer;
  return ret;
}

#elif USE_X11
static char* getSelection(Display *dpy, Window us, Atom selection)
{
  int    max_events = 50;
  Window owner      = XGetSelectionOwner (dpy, selection);
  int    ret;

  //printf("XConvertSelection on %s\n", XGetAtomName(dpy, selection));
  if (owner == None)
  {
    //printf("No owner\n");
    return NULL;
  }
  XConvertSelection(dpy, selection, XA_STRING, XA_PRIMARY, us, CurrentTime);
  XFlush(dpy);

  while (max_events--)
  {
    XEvent        e;

    XNextEvent(dpy, &e);
    if(e.type == SelectionNotify)
    {
      //printf("Received %s\n", XGetAtomName(dpy, e.xselection.selection));
      if(e.xselection.property == None)
      {
        //printf("Couldn't convert\n");
        return NULL;
      }

      long unsigned len, left, dummy;
      int           format;
      Atom          type;
      unsigned char *data = NULL;

      XGetWindowProperty(dpy, us, e.xselection.property, 0, 0, False,
                         AnyPropertyType, &type, &format, &len, &left, &data);
      if (left < 1)
        return NULL;

      ret = XGetWindowProperty(dpy, us, e.xselection.property, 0, left, False,
                               AnyPropertyType, &type, &format, &len, &dummy, &data);
      if (ret != Success)
      {
        //printf("Failed to get property: %p on %lu\n", data, len);
        return NULL;
      }

      //printf(">>>  Got %s: len=%lu left=%lu (event %i)\n", data, len, left, 50-max_events);
      return (char*)data;
    }
  }
  printf("Timeout\n");
  return NULL;
}

bool RetrieveBuffer(std::string& text, std::string::size_type& pos)
{
  SDL_SysWMinfo info;

  //printf("Retrieving buffer...\n");
  SDL_VERSION(&info.version);
  if ( SDL_GetWMInfo(&info) )
  {
    Display *dpy  = info.info.x11.display;
    Window  us    = info.info.x11.window;
    char    *data = NULL;

    if (!data)
    {
      data = getSelection(dpy, us, XA_PRIMARY);
    }
    if (!data)
    {
      data = getSelection(dpy, us, XA_SECONDARY);
    }
    if (!data)
    {
      Atom XA_CLIPBOARD = XInternAtom(dpy, "CLIPBOARD", 0);
      data = getSelection(dpy, us, XA_CLIPBOARD);
    }
    if (data)
    {
      // check cursor position
      if (pos > text.size()) {
        pos = text.size();
      }

      text.insert(pos, data);
      pos += strlen(data);
      XFree(data);
    }
  }
  return false;
}
#else
bool RetrieveBuffer(std::string&, std::string::size_type&) { return false; }
#endif
