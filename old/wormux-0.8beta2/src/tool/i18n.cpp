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
 * Function used to format string.
 * Eg. : Format("Hello %s", "world") returns "Hello World".
 *****************************************************************************/

#include "i18n.h"
#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "debug.h"
#ifdef _MSC_VER
#  include "msvc/config.h"
#else
#  include "config.h"
#endif
#include "game/config.h"

std::string Format(const char *format, ...)
{
  const int bufferSize = 256;
  char buffer[bufferSize];
  va_list argp;
  std::string result;

  va_start(argp, format);

  int size = vsnprintf(buffer, bufferSize, format, argp);

  if(size < 0)
    Error("Error formating string...");

  if(size < bufferSize) {
    result = std::string(buffer);
  } else {
    char *bigBuffer = (char *)malloc((size + 1) * sizeof(char));
    if(bigBuffer == NULL)
      Error( "Out of memory !");

    size = vsnprintf(bigBuffer, size + 1, format, argp);
    if(size < 0)
      Error( "Error formating string...");

    result = std::string(bigBuffer);
    free(bigBuffer);
  }

  va_end(argp);

  return result;
}

static void I18N_SetDir(const std::string &dir)
{
  printf("o Bind text domain to: %s\n"
         "o Codeset: %s\n"
         "o Text domain: %s\n",
         bindtextdomain(PACKAGE, dir.c_str()), bind_textdomain_codeset(PACKAGE, "UTF-8"), textdomain(PACKAGE));
}

void InitI18N(const std::string &dir, const std::string &default_language)
{
  setlocale(LC_ALL, "");
#ifdef _WIN32
  std::string variable = "LANGUAGE=";
  variable += default_language;
  _putenv(variable.c_str());
#else
  setenv("LANGUAGE", default_language.c_str(), 1);
#endif
  I18N_SetDir(dir);
}
