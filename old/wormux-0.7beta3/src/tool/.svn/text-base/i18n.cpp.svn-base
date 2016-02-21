/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
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
 * Function used to format string.
 * Eg. : Format("Hello %s", "world") returns "Hello World".
 *****************************************************************************/

#include "../tool/i18n.h"
//-----------------------------------------------------------------------------
#include <iostream>
#include <sstream>
#include <stdarg.h>
#include "../game/config.h"
#include "../config.h"
//-----------------------------------------------------------------------------

// Package is defined by autotools
#ifdef WIN32
#define PACKAGE "Wormux"
#endif

#define GETTEXT_DOMAIN PACKAGE
//-----------------------------------------------------------------------------

void CopyString (std::ostream &os, 
                 const char *&txt, const char *&sauve_txt, 
                 ulong &lg)
{
  std::string prefix(sauve_txt,0,lg);
  os << prefix;
  lg = 0;
  sauve_txt = txt;
  ++sauve_txt;
}

//-----------------------------------------------------------------------------

std::string Format (const char *txt, ...)
{
  va_list ap;
  va_start (ap,txt);

  std::ostringstream ss;
  const char *sauve_txt = txt;
  ulong lg=0;
  for (; *txt != 0; ++txt)
  {
    if (*txt == '%')
    {
      ++txt;
      bool error = false;

      switch (*txt)
      {
      case 's':
      {
        const char *x = va_arg (ap,const char *);
        CopyString (ss, txt, sauve_txt, lg);
        ss << x;
        break;
      }
      case 'i':
      {
        int x = va_arg (ap,int);
        CopyString (ss, txt, sauve_txt, lg);
        ss << x;
        break;
      }
      case 'u':
      {
        uint x = va_arg (ap,int);
        CopyString (ss, txt, sauve_txt, lg);
        ss << x;
        break;
      }
      case 'l':
        if (*(txt+1) == 'u') {
          ulong x = va_arg(ap,ulong);
          ++txt;
          CopyString (ss, txt, sauve_txt, lg);
          ss << x;
        } else if (*(txt+1) == 'i') {
          long x = va_arg(ap,long);
          CopyString (ss, txt, sauve_txt, lg);
          ss << x;
          ++txt;
        } else {
          error = true;
        }
        break;
      default:
        error = true;
      }

      if (error) {
        ss.str("");
        ss << "Format error : unknown '%" << *txt << "' !";
        Error(ss.str());
      }
    } else {
      ++lg;
    }
  }
  if (lg != 0) CopyString (ss, txt, sauve_txt, lg);

  va_end (ap);
  return ss.str();
}

//-----------------------------------------------------------------------------

void I18N_SetDir(const std::string &dir)
{
  bindtextdomain(GETTEXT_DOMAIN, dir.c_str());
  bind_textdomain_codeset (GETTEXT_DOMAIN, "UTF-8");
}

//-----------------------------------------------------------------------------

void InitI18N()
{
  setlocale (LC_ALL, "");
  I18N_SetDir (Wormux::config.locale_dir.c_str());
  textdomain(GETTEXT_DOMAIN);
}

//-----------------------------------------------------------------------------
