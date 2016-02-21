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

// Sorry for the very long code, all nicer OS X APIs are coded in Objective C and not C!
// Also it does very thorough error handling
bool GetDataFromPasteboard( PasteboardRef inPasteboard, char* flavorText /* out */, const int bufSize )
{
    OSStatus            err = noErr;
    PasteboardSyncFlags syncFlags;
    ItemCount           itemCount;

    syncFlags = PasteboardSynchronize( inPasteboard );

    //require_action( syncFlags & kPasteboardModified, PasteboardOutOfSync,
    //               err = badPasteboardSyncErr );

    err = PasteboardGetItemCount( inPasteboard, &itemCount );
    require_noerr( err, CantGetPasteboardItemCount );

    for (UInt32 itemIndex = 1; itemIndex <= itemCount; itemIndex++)
    {
        PasteboardItemID    itemID;
        CFArrayRef          flavorTypeArray;
        CFIndex             flavorCount;

        err = PasteboardGetItemIdentifier( inPasteboard, itemIndex, &itemID );
        require_noerr( err, CantGetPasteboardItemIdentifier );

        err = PasteboardCopyItemFlavors( inPasteboard, itemID, &flavorTypeArray );
        require_noerr( err, CantCopyPasteboardItemFlavors );

        flavorCount = CFArrayGetCount( flavorTypeArray );

        for (CFIndex flavorIndex = 0; flavorIndex < flavorCount; flavorIndex++)
        {
            CFStringRef             flavorType;
            CFDataRef               flavorData;
            CFIndex                 flavorDataSize;
            flavorType = (CFStringRef)CFArrayGetValueAtIndex(flavorTypeArray, flavorIndex);

            // we're only interested by text...
            if (UTTypeConformsTo(flavorType, CFSTR("public.utf8-plain-text")))
            {
                err = PasteboardCopyItemFlavorData( inPasteboard, itemID,
                                                   flavorType, &flavorData );
                require_noerr( err, CantCopyFlavorData );
                flavorDataSize = CFDataGetLength( flavorData );
                flavorDataSize = (flavorDataSize<254) ? flavorDataSize : 254;

                if (flavorDataSize+2 > bufSize)
                {
                    fprintf(stderr, "Cannot copy clipboard, contents is too big!\n");
                    return false;
                }

                for (short dataIndex = 0; dataIndex <= flavorDataSize; dataIndex++)
                {
                    char byte = *(CFDataGetBytePtr( flavorData ) + dataIndex);
                    flavorText[dataIndex] = byte;
                }

                flavorText[flavorDataSize] = '\0';
                flavorText[flavorDataSize+1] = '\n';

                CFRelease (flavorData);
                return true;
            }

            continue;
            CantCopyFlavorData:   fprintf(stderr, "Cannot copy clipboard, CantCopyFlavorData!\n");
        }

        CFRelease (flavorTypeArray);
        continue;

    CantCopyPasteboardItemFlavors:   fprintf(stderr, "Cannot copy clipboard, CantCopyPasteboardItemFlavors!\n");   continue;
    CantGetPasteboardItemIdentifier: fprintf(stderr, "Cannot copy clipboard, CantGetPasteboardItemIdentifier!\n"); continue;
    }
    fprintf(stderr, "Cannot copy clipboard, found no acceptable flavour!\n");
    return false;

    CantGetPasteboardItemCount:  fprintf(stderr, "Cannot copy clipboard, CantGetPasteboardItemCount!\n"); return false;
    //PasteboardOutOfSync:         fprintf(stderr, "Cannot copy clipboard, PasteboardOutOfSync!\n");        return false;
}

bool getClipBoard(char* text /* out */, const int bufSize )
{
    OSStatus err = noErr;

    PasteboardRef theClipboard;
    err = PasteboardCreate( kPasteboardClipboard, &theClipboard );
    require_noerr( err, PasteboardCreateFailed );

    if (!GetDataFromPasteboard(theClipboard, text, bufSize))
    {
        fprintf(stderr, "Cannot copy clipboard, GetDataFromPasteboardFailed!\n");
        return false;
    }

    CFRelease(theClipboard);

    return true;

    // ---- error handling
    PasteboardCreateFailed:       fprintf(stderr, "Cannot copy clipboard, PasteboardCreateFailed!\n");
    CFRelease(theClipboard);
    return false;
}

bool RetrieveBuffer(std::string& text, std::string::size_type& pos)
{
    const int bufSize = 512;
    char buffer[bufSize];

    if (getClipBoard(buffer, bufSize))
    {
        text = buffer;
        pos += strlen(buffer);
        return true;
    }
    else
    {
        return false;
    }
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

      return true;
    }
  }
  return false;
}
#else
bool RetrieveBuffer(std::string&, std::string::size_type&) { return false; }
#endif
