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
 * Proxy header including the actual config header for the host system.
 *****************************************************************************/

#ifndef WARMUX_CONFIG_H
#define WARMUX_CONFIG_H

#ifdef _MSC_VER
#  include "config_visual.h"
#elif defined(ANDROID)
#  include "config_android.h"
#elif defined(__SYMBIAN32__)
#  include "config_symbian.h"
#elif defined(HAVE_CONFIG_H)
#  include "config_autotools.h"
#else
#  error "Please add a config_${your_system}.h and include it properly"
#endif

#ifdef MAEMO
#  define HAVE_TOUCHSCREEN
#  define HAVE_HANDHELD
#endif

#ifdef GEKKO
#  define HAVE_HANDHELD // Not really true, but saves memory and CPU
#endif

#endif // WARMUX_CONFIG_H
