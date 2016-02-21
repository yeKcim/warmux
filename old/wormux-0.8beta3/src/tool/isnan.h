#ifndef __ISNAN_H__
#define __ISNAN_H__

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
 * isNan macro, taken from Inkscape. Thx!
 *****************************************************************************/

/*
 * Temporary fix for various misdefinitions of isnan().
 * isnan() is becoming undef'd in some .h files. 
 * #include this last in your .cpp file to get it right.
 *
 * The problem is that isnan and isfinite are part of C99 but aren't part of
 * the C++ standard (which predates C99).
 *
 * Authors:
 *   Inkscape groupies and obsessive-compulsives
 *
 * Copyright (C) 2004 authors
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 *
 * 2005 modification hereby placed in public domain.  Probably supercedes the 2004 copyright
 * for the code itself.
 */

#include <math.h>
/* You might try changing the above to <cmath> if you have problems.
 * Whether you use math.h or cmath, you may need to edit the .cpp file
 * and/or other .h files to use the same header file.
 */

#if defined(__isnan)
# define isNaN(_a) (__isnan(_a))
#elif defined(__APPLE__) && __GNUC__ == 3
# define isNaN(_a) (__isnan(_a))    /* MacOSX/Darwin definition < 10.4 */
#elif defined(WIN32) || defined(_isnan)
# define isNaN(_a) (_isnan(_a))     /* Win32 definition */
#elif defined(isnan) || defined(__FreeBSD__) || defined(__osf__)
# define isNaN(_a) (isnan(_a))      /* GNU definition */
#elif defined (SOLARIS_2_8) && __GNUC__ == 3 && __GNUC_MINOR__ == 2
# define isNaN(_a) (isnan(_a))      /* GNU definition */
#else
# define isNaN(_a) (std::isnan(_a))
#endif
/* If the above doesn't work, then try (a != a).
 * Also, please report a bug as per http://www.inkscape.org/report_bugs.php,
 * giving information about what platform and compiler version you're using.
 */


#if defined(__isfinite)
# define isFinite(_a) (__isfinite(_a))
#elif defined(__APPLE__) && __GNUC__ == 3
# define isFinite(_a) (__isfinite(_a))  /* MacOSX/Darwin definition < 10.4 */
#elif defined(__sgi)
# define isFinite(_a) (_isfinite(_a))
#elif defined(isfinite)
# define isFinite(_a) (isfinite(_a))
#elif defined(__osf__)
# define isFinite(_a) (finite(_a) && !isNaN(_a))
#elif defined (SOLARIS_2_8) && __GNUC__ == 3 && __GNUC_MINOR__ == 2
#include  <ieeefp.h>
#define isFinite(_a) (finite(_a) && !isNaN(_a))
#else
# define isFinite(_a) (std::isfinite(_a))
#endif
/* If the above doesn't work, then try (finite(_a) && !isNaN(_a)) or (!isNaN((_a) - (_a))).
 * Also, please report a bug as per http://www.inkscape.org/report_bugs.php,
 * giving information about what platform and compiler version you're using.
 */


#endif /* __ISNAN_H__ */

