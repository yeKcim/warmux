dnl ========================================================================
dnl =                   W A R M U X  -  configure.ac                       =
dnl =                                                                      =
dnl = Process this file with autoconf to produce a configure script.       =
dnl ========================================================================


dnl ========================================================================
dnl === Initial configuration                                              =
dnl ========================================================================

dnl = Require at least automake 2.52
AC_PREREQ(2.52)

AC_INIT([Warmux], [svn], [warmux-dev@gna.org], [warmux])
AC_CONFIG_SRCDIR([src/main.cpp])

dnl Detect the canonical host and target build environment
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

#AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([foreign 1.5])

AM_GNU_GETTEXT([external], [need-ngettext])
AM_CONDITIONAL([NLS], test "$USE_NLS" = "yes")

dnl ========================================================================
dnl === Check for tools                                                    =
dnl ========================================================================

AC_PROG_MAKE_SET
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_RANLIB
AC_CHECK_TOOL(STRIP, strip)
AC_CHECK_TOOL(WINDRES, windres)

AC_CHECK_PROG(cxx_present, $CXX, "yes", "no")
if test "x$cxx_present" != "xyes"; then
  AC_MSG_ERROR([*** No C++ compiler can be found!])
fi

AX_CFLAGS_WARN_ALL([CXXFLAGS])

# ==========================================================================
# == Set flags for various environments                                    =
# ==========================================================================
WIN32="no"
GEKKO="no"
OSX="no"
case "${host}" in
  i[[3456789]]86-*-mingw32*) WIN32="yes" ;;
  *freebsd*)
    LIBS="$LIBS -lintl" # a temporary hackish workaround
    ;;
  *cygwin*) WIN32="yes" ;;
  *-apple-darwin*)
    OSX="yes"
    LIBS="$LIBS -Wl,-framework,Carbon"
    ;;
  powerpc-unknown-eabi)
    # Supposedly Gekko system (wii/gamecube), so set proper abi flags
    # so as to not get strange link errors
    CFLAGS="$CFLAGS -DGEKKO -mrvl -mcpu=750 -meabi -mhard-float"
    GEKKO="yes"
    ;;
esac
AM_CONDITIONAL([WIN32], test "${WIN32}" = "yes")

dnl ========================================================================
dnl === Options for users                                                  =
dnl ========================================================================

dnl === Non-windows options
AC_ARG_ENABLE([servers],
              [AS_HELP_STRING([--enable-servers],
                              [Enable building the index and game servers @<:@default=no@:>@])],
              [],
              [enable_servers=no])
AM_CONDITIONAL([BUILD_SERVERS], [test "$enable_servers" = "yes"])

if ! test "$enable_servers" = "yes"; then
  AC_MSG_WARN([Building of servers is disabled. If you want build the index and servers, please use configure with this option : --enable-servers])
fi

AC_ARG_ENABLE([static],
              [  --enable-static         Enable static building of warmux],
              [static=$enableval],
              [static=no])

dnl === Regular options
AC_ARG_ENABLE([debug],
              [  --enable-debug          Enable debug in Warmux],
              debug=$enableval,
              debug="")
AC_ARG_ENABLE([logging],
              [  --enable-logging        Enable logging in Warmux],
              logging=$enableval,
              logging="")


AC_ARG_ENABLE([profile],
              [  --enable-profile        Enable profiling],
              [if test "x${enableval}" = "xyes" ; then
                   CXXFLAGS="$CXXFLAGS -pg"
               fi])

AC_ARG_ENABLE(vkeybd, AC_HELP_STRING([--enable-vkeybd], [Enable Virtual Keyboard]) )
if test "${enable_vkeybd}" = "yes"; then
       AC_DEFINE([ENABLE_VKEYBD], [1], [Enable Virtual Keyboard])
fi

# Check whether user supplied the option to statically link binaries.
if test "$static" = yes; then
# if we're using gcc, add `-static' to LDFLAGS
       if test -n "$GCC" || test "$ac_cv_prog_gcc" = "yes"; then
               STATIC_LD="-static"
               LDFLAGS="$LDFLAGS -static"
               LIBCURL="$(curl-config --libs)"
               LIBCURL="${LIBCURL:--lcurl} -lssl -lcrypto -ldl -lz"
       fi
fi

if test "x${prefix}" = "xNONE"; then
  prefix="${ac_default_prefix}"
fi

if test "${WIN32}" = "yes"; then DATADIR="data"
elif test "${GEKKO}" = "yes"; then DATADIR="sd:/apps/warmux/data"
else DATADIR="${prefix}/share/warmux"; fi

AC_ARG_WITH([datadir-name],
            [AS_HELP_STRING([--with-datadir-name=DIR],
                            [specify where datas are installed (default: ${prefix}/share/warmux)])],
            [DATADIR="$withval"])
AC_SUBST([DATADIR])
# ensure a trailing slash (r4255)
DATADIR="${DATADIR}/"


if test "${WIN32}" = "yes"; then LOCALEDIR="locale"
elif test "${GEKKO}" = "yes"; then LOCALEDIR="sd:/apps/warmux/locale"
else LOCALEDIR="${prefix}/share/locale"; fi
AC_ARG_WITH([localedir-name],
            [AS_HELP_STRING([--with-localedir-name=DIR],
                            [specify where locales are installed (default: ${prefix}/share/locale)])],
            [LOCALEDIR="$withval"])
AC_SUBST([LOCALEDIR])

FONTFILE="\${DATADIR}/font/Ubuntu-R.ttf"
AC_ARG_WITH([font-path],
            [AS_HELP_STRING([--with-font-path=FILE],
                            [specify the font file (default: ${datadir}/font/Ubuntu-R.ttf)])],
            [FONTFILE="$withval"])
AC_SUBST([FONTFILE])

AM_CONDITIONAL([STATIC], [test x$static = xyes])
AM_CONDITIONAL([GCC], [test x$GXX = xyes])


dnl ========================================================================
dnl === Checks for required libraries to be present                        =
dnl ========================================================================

SDL_VERSION=1.2.6

dnl === Set compiler flags =================================================

if test "x$GCC" = "xyes"; then
  dnl get gcc version
  AC_MSG_CHECKING([gcc version])
  gccver=$($CC -dumpversion)
  gccvermajor=$(echo $gccver | cut -d . -f1)
  gccverminor=$(echo $gccver | cut -d . -f2)
  gccvernum=$(expr $gccvermajor "*" 100 + $gccverminor)
  AC_MSG_RESULT($gccver)

  dnl Enable all warnings
  GCC_FLAGS="-Wall"

  dnl Enable *more* warnings
  if test "$gccvernum" -ge "400"; then
    dnl gcc 4.0 or later
    GCC_FLAGS="$GCC_FLAGS -Wextra"
  else
    GCC_FLAGS="$GCC_FLAGS -W"
  fi

  if test "x${debug}" = "xyes"; then
    GCC_FLAGS="$GCC_FLAGS -O0 -g -DDEBUG -DWMX_LOG"
  else
    GCC_FLAGS="$GCC_FLAGS -O2"
  fi

  if test "x${logging}" = "xyes"; then
    GCC_FLAGS="$GCC_FLAGS -DWMX_LOG"
  fi

  dnl Let's *not* ignore flags set from outside world
  CFLAGS="$GCC_FLAGS $CFLAGS"
  CXXFLAGS="$GCC_FLAGS $CFLAGS"

else
  if test "x${debug}" = "xyes"; then
    CFLAGS="$CFLAGS -DDEBUG"
    CXXFLAGS="$CXXFLAGS -DDEBUG"
  fi

  if test "x${logging}" = "xyes"; then
    CFLAGS="$CFLAGS -DWMX_LOG"
    CXX_FLAGS="$CXXFLAGS -DWMX_LOG"
  fi
fi

dnl === Check for SDL ======================================================

AM_PATH_SDL($SDL_VERSION,
            :,
            AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!]))
CXXFLAGS="$CXXFLAGS $SDL_CFLAGS"
CFLAGS="$CFLAGS $SDL_CFLAGS"
CPPFLAGS="$CPPFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"

dnl === Check for X11 ======================================================
dnl Deactivated on purpose under OSX (in case X11 SDK is installed)
if test "x$OSX" != "xyes" ; then
  AC_CHECK_HEADER(X11/Xlib.h, check_x11="yes",check_x11="no")
  if test x${check_x11} = xno ; then
    AC_CHECK_HEADER(X11R6/Xlib.h,
                    [ check_x11="yes"
                      LDFLAGS="-L/usr/X11R6/include $CFLAGS"],
                    check_x11="no")
  fi
  if test x${check_x11} = xyes ; then
    AC_CHECK_LIB(X11, XOpenDisplay,
                 [ LIBS="$LIBS -lX11"
                   AC_DEFINE(USE_X11, 1, [Define to use X11 copy'n'paste]) ],
                 [])
  fi
fi

dnl === Check for libxml2 and libpng using pkg-confg =======================
PKG_PROG_PKG_CONFIG()
PKG_CHECK_MODULES(LIBXML2, libxml-2.0, [], [AC_MSG_FAILURE([libxml2 not found])])
PKG_CHECK_MODULES(LIBPNG, libpng, [], [AC_MSG_FAILURE([libpng not found])])
CFLAGS="$CFLAGS $LIBPNG_CFLAGS $LIBXML2_CFLAGS"
CXXFLAGS="$CXXFLAGS $LIBPNG_CFLAGS $LIBXML2_CFLAGS"
LIBS="$LIBS $LIBPNG_LIBS $LIBXML2_LIBS"

dnl === Check for FriBidi ==================================================
AC_ARG_ENABLE(fribidi, AC_HELP_STRING([--enable-fribidi], [enable bi-directional unicode support]) )
if test "${enable_fribidi}" = "yes"; then
   if test "x$USE_NLS" != "xyes"; then
     AC_MSG_WARN([NLS disabled, not enabling fribidi support])
   else
     PKG_CHECK_MODULES(FRIBIDI, [fribidi],
                       [ use_fribidi="true"
                         AC_DEFINE(USE_FRIBIDI, 1, [Define to use FriBidi]) ],
                       [ use_fribidi="false"
                         AC_MSG_WARN([fribidi is disabled. If you want bi-directional text rendering, please install fribidi dev package.]) ])
   fi
   # Will be empty if not found
   LIBS="$LIBS $FRIBIDI_LIBS"
   CFLAGS="$CFLAGS $FRIBIDI_CFLAGS"
   CXXFLAGS="$CXXFLAGS $FRIBIDI_CFLAGS"
else
  if test "x$USE_NLS" = "xyes"; then
    AC_MSG_WARN([fribidi is disabled. If you want bi-directional text rendering, please use configure with this option : --enable-fribidi])
  fi
fi

dnl === Check for SDL_gfx ==================================================

AC_ARG_WITH(sdl-gfx-prefix,
            [  --with-sdl-gfx-prefix=DIR       specify where SDL_gfx library is installed],
            [SDL_GFX_PREFIX="$withval"])
AC_SUBST(SDL_GFX_PREFIX)
if test "x$SDL_GFX_PREFIX" != "x"; then
  CPPFLAGS="$CPPFLAGS -I$SDL_GFX_PREFIX/include/SDL"
  CXXFLAGS="$CXXFLAGS -I$SDL_GFX_PREFIX/include/SDL"
  LIBS="-L$SDL_GFX_PREFIX/lib $LIBS"
fi
if test "$static" = yes; then
  LIBS="$LIBS -lm -ldl -lfreetype -lz"
fi
AC_CHECK_HEADER(SDL_rotozoom.h,,AC_MSG_ERROR(Please install SDL_gfx))
AC_CHECK_LIB(SDL_gfx, rotozoomSurfaceXY,, AC_MSG_ERROR(Please install SDL_gfx))
AC_MSG_CHECKING([whether SDL_gfx - version >= 2.0.22 ])
AC_RUN_IFELSE([
#include <stdio.h>
#include <SDL_gfxPrimitives.h>
#undef main
int main(void)
{
  FILE *f = fopen("conftest.out", "wt+");
  fprintf(f, "%i.%i.%i\n",
         SDL_GFXPRIMITIVES_MAJOR, SDL_GFXPRIMITIVES_MINOR, SDL_GFXPRIMITIVES_MICRO);
  fclose(f);
  int ver = SDL_GFXPRIMITIVES_MAJOR*1000+SDL_GFXPRIMITIVES_MINOR*100+SDL_GFXPRIMITIVES_MICRO;
  return ver < 2022;
}],
  [echo "`cat conftest.out`, OK"],
  [cat conftest.out
   AC_MSG_WARN(This version of SDL_gfx is outdated and prone to cause crashes, please update it.)])

dnl === Check for curl =====================================================

LIBCURL_CHECK_CONFIG([yes],[7.0],[check_curl=yes],[check_curl=no])
if test "${check_curl}" = "yes"; then
  if test "${libcurl_protocol_HTTPS}" != "yes"; then
    AC_MSG_WARN([No HTTPS support in curl, Twitter support off])
    social="Facebook"
    CXXFLAGS="$CXXFLAGS -DHAVE_FACEBOOK"
  else
    CXXFLAGS="$CXXFLAGS -DHAVE_FACEBOOK -DHAVE_TWITTER"
    social="Facebook Twitter"
  fi
else
  AC_MSG_WARN(Absolutely no network support!)
  social="None"
fi

dnl === Check for Maemo ====================================================

AC_ARG_ENABLE(maemo, AC_HELP_STRING([--enable-maemo], [enable Maemo support]) )
if test "${enable_maemo}" = "yes"; then
  AC_DEFINE([MAEMO], [yes], [Maemo support])
  PKG_CHECK_MODULES(GLIB, glib-2.0, [], [AC_MSG_FAILURE([libglib-2.0 not found])])
  PKG_CHECK_MODULES(OSSO, libosso, [AC_DEFINE([HAVE_LIBOSSO], [1], [Use libosso])] use_osso=yes, [:])
  PKG_CHECK_MODULES(RESOURCE, libresource0-glib, [AC_DEFINE([HAVE_LIBRESOURCE], [1], ["Use libresource"])] use_resource=yes, [:])
  PKG_CHECK_MODULES(CONIC, conic, [AC_DEFINE([HAVE_LIBCONIC], [1], [Use libconic])] use_conic=yes, [:])
  CFLAGS="$CFLAGS $GLIB_CFLAGS $RESOURCE_CFLAGS $CONIC_CFLAGS $OSSO_CFLAGS"
  CXXFLAGS="$CXXFLAGS $GLIB_CFLAGS $RESOURCE_CFLAGS $CONIC_CFLAGS $OSSO_CFLAGS"
  LIBS="$LIBS $GLIB_LIBS $RESOURCE_LIBS $CONIC_LIBS $OSSO_LIBS"
fi

AM_CONDITIONAL(MAEMO, test "x${enable_maemo}" = "xyes")
AM_CONDITIONAL(HAVE_LIBOSSO, test "x${use_osso}" = "xyes")
AM_CONDITIONAL(HAVE_LIBRESOURCE, test x${use_resource} = xyes)
AM_CONDITIONAL(HAVE_LIBCONIC, test "x${use_conic}" = "xyes")

dnl === Check for SDL image, mixer, etc. ===================================

AC_CHECK_LIB(SDL_image, IMG_Load,,check_sdl_image="no")
AC_CHECK_LIB(SDL_mixer, Mix_OpenAudio,,check_sdl_mixer="no")
AC_CHECK_LIB([SDL_ttf], [TTF_OpenFont],,check_sdl_ttf="no")
AC_CHECK_LIB([SDL_net], [SDLNet_Init],,check_sdl_net="no")
if test "${WIN32}" = "yes"; then
  LIBS="$LIBS -lwsock32 -lws2_32 -luser32"
  CXXFLAGS="$CXXFLAGS -D_WIN32_WINNT=0x501"
fi

dnl === OSX uses a specific detection method for file locations ============
if test "${OSX}" = "yes"; then
  CXXFLAGS="$CXXFLAGS -DOSX_BUNDLE"
fi

dnl === Display error if SDL image, mixer, etc. is missing =================

if test \
     x${check_sdl_image} = xno \
  -o x${check_sdl_mixer} = xno \
  -o x${check_sdl_ttf} = xno \
  -o x${check_sdl_net} = xno \
  -o x${check_libxml} = xno \
  ; then
  AC_MSG_RESULT([])
  if test x${check_sdl_image} = xno; then
    AC_MSG_RESULT([ERROR: SDL_image library required])
  fi
  if test x${check_sdl_mixer} = xno; then
    AC_MSG_RESULT([ERROR: SDL_mixer library required])
  fi
  if test x${check_sdl_ttf} = xno; then
    AC_MSG_RESULT([ERROR: SDL_ttf library required])
  fi
  if test x${check_sdl_net} = xno; then
    AC_MSG_RESULT([ERROR: SDL_net library required])
  fi
  exit 1
fi

dnl === Add Werror flag for gcc/g++ if debug mode is enabled, ==============
dnl === but allow RTTI (ie dynamic_cast, typeid/typeinfo unavailable) ======
if test "x${debug}" = "xyes"; then
  CFLAGS="$CFLAGS -Werror"
  CXXFLAGS="$CXXFLAGS -Werror"
else
  CXXFLAGS="$CXXFLAGS -fno-rtti"
fi

dnl === Forbid exceptions (Android platform requirement) ===================
CXXFLAGS="$CXXFLAGS -fno-exceptions"

dnl ========================================================================
dnl === Check for header files                                           ===
dnl ========================================================================

dnl TODO: remove them when we don't need them any more ;)

AC_HEADER_DIRENT
AC_HEADER_STDC


dnl ========================================================================
dnl === Data file substitution.                                          ===
dnl ========================================================================

DATA_FILES=`cd data ; find . \( -name CVS -o -name ".svn*" -o -name "Makefile*" -o -name "warmux*.desktop" \) -prune -o -name ".#*" -o -type d -o -print`
DATA_FILES=`echo $DATA_FILES`

AC_SUBST([DATA_FILES])
AC_SUBST([BUILD_ENV])
AC_CONFIG_FILES([Makefile
                 data/Makefile
                 src/Makefile
                 lib/Makefile
                 lib/fixedpoint/Makefile
                 lib/warmux/Makefile
                 po/Makefile
                 ], [AM_POSTPROCESS_PO_MAKEFILE])
if test "${WIN32}" = "yes"; then
AC_CONFIG_FILES([build/win32_setup/create_setup.sh],
                [chmod +x build/win32_setup/create_setup.sh])
else
AC_CONFIG_FILES([lib/warmux-server/Makefile
                 doc/Makefile
                 tools/Makefile
                 tools/list_games/Makefile
                 tools/servers/Makefile
                 tools/servers/game_server/Makefile
                 tools/servers/index_server/Makefile
                 ])
fi

AM_CONFIG_HEADER(lib/warmux/include/config_autotools.h)

AC_OUTPUT

echo "***********************************************************"
echo "Configuration complete"
echo ""
echo "* Game data will be installed into $DATADIR"
echo "* Game locale will be installed into $LOCALEDIR"
echo "* Font file will be $FONTFILE"
echo ""
echo "* [[Optional]] DEBUG                              : ${debug:-no}"
if test "$debug" != "yes"; then
   echo "* [[Optional]] LOGGING feature                    : ${logging:-no}"
fi
echo ""
echo "* [[Optional]] Bi-directional rendering (fribidi) : ${enable_fribidi:-no}"
echo "* [[Optional]] Internationalisation (gettext)     : ${USE_NLS:-no}"
echo "* [[Optional]] Index and game servers             : ${enable_servers:-no}"
echo "* [[Optional]] Publishing to social services      : ${social}"
echo "***********************************************************"
echo ""
echo "Execute make to compile then execute make install to install..."

if test "${WIN32}" = "yes"; then
   echo "Alternatively, type make setup to build an installer"
fi
