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
 * Function used to format string.
 * Eg. : Format("Hello %s", "world") returns "Hello World".
 *****************************************************************************/

#ifndef WARMUX_FORMAT_H
#define WARMUX_FORMAT_H

#include <string>

#include "WARMUX_config.h"

#ifdef ENABLE_NLS /* gettext */
#  include <libintl.h>
void InitI18N(const std::string &dir, const std::string &default_language);
#  ifdef USE_FRIBIDI
#    define _(X) localization(X)
char * localization(const char * buffer);
#  else
#    define _(X) gettext(X)
#  endif /* USE_FRIBIDI */
#else
#  define _(X) X
#  define ngettext(sing_, plur_, val) ((val<2) ? (sing_) : (plur_))
#endif /* ENABLE_NLS aka gettext */

#if defined(__GNUC__)
#  ifdef WIN32
#    define ATTRIBUTE_FORMAT(fun, fmt, params) __attribute__((format(ms_ ## fun, fmt, params)))
#  else
#    define ATTRIBUTE_FORMAT(fun, fmt, params) __attribute__((format(fun, fmt, params)))
#  endif
#else
#  define ATTRIBUTE_FORMAT(fun, fmt, params)
#endif

std::string Format (const char *format, ...) ATTRIBUTE_FORMAT(printf, 1, 2);

#endif /* FORMAT_H */
