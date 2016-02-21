dnl ========================================================================
dnl =                   W O R M U X  -  configure.ac                       =
dnl =                                                                      =
dnl = Process this file with autoconf to produce a configure script.       =
dnl ========================================================================


dnl ========================================================================
dnl === Initial configuration                                              =
dnl ========================================================================

dnl = Require at least automake 2.52
AC_PREREQ(2.52)

AC_INIT([Wormux], [0.8beta3], [wormux-dev@gna.org], [wormux])
AC_CONFIG_SRCDIR([src/main.cpp])

dnl Detect the canonical host and target build environment
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

#AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([foreign 1.5])

AM_GNU_GETTEXT([external])

dnl ========================================================================
dnl === Check for tools                                                    =
dnl ========================================================================

AC_PROG_MAKE_SET
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_INSTALL

AC_CHECK_PROG(cxx_present, $CXX, "yes", "no")
if test "x$cxx_present" != "xyes"; then
  AC_MSG_ERROR([*** No C++ compiler can be found!])
fi

dnl =========================================================================
dnl === Options for users                                                   =
dnl =========================================================================

AX_CFLAGS_WARN_ALL([CXXFLAGS])

AC_ARG_ENABLE([debug],
              [  --enable-debug          Enable debug in Wormux],
              debug=$enableval,
              debug="")

AC_ARG_ENABLE([profile],
              [  --enable-profile        Enable profiling],
              [if test "x${enableval}" = "xyes" ; then
                   CXXFLAGS="$CXXFLAGS -pg"
               fi])

AC_ARG_ENABLE([static],
              [  --enable-static         Enable static building of wormux],
              [static=$enableval],
              [static=no])

# Check whether user supplied the option to statically link binaries.
#if test "$static" = yes; then
# if we're using gcc, add `-static' to LDFLAGS
#       if test -n "$GCC" || test "$ac_cv_prog_gcc" = "yes"; then
#               STATIC_LD="-static"
#               LDFLAGS="$LDFLAGS -static"
#       fi
#fi

if test "x${prefix}" = "xNONE"; then
  prefix="${ac_default_prefix}"
fi

# ========================================================================
# == Set flags for various environments                                  =
# ========================================================================
case "${host}" in
  i[[3456789]]86-*-mingw32*)
     WIN32="yes"
     AC_PATH_PROGS([WINDRES], [windres $host_alias-windres $host_os-windres])
     if test "x$WINDRES" = "x"; then
       AC_MSG_ERROR([windres could not be found within your path.])
     fi
     AC_SUBST(WINDRES)
     ;;
  *cygwin*) WIN32="yes" ;;
  *) WIN32="no" ;;
esac
AM_CONDITIONAL([WIN32], test "$WIN32" = "yes")


if test "${WIN32}" = "yes"; then DATADIR="data"
else DATADIR="${prefix}/share/wormux"; fi
AC_ARG_WITH([datadir-name],
            [AS_HELP_STRING([--with-datadir-name=DIR],
                            [specify where datas are installed (default: ${prefix}/share/wormux)])],
            [DATADIR="$withval"])
AC_SUBST([DATADIR])


if test "${WIN32}" = "yes"; then LOCALEDIR="locale"
else LOCALEDIR="${prefix}/share/locale"; fi
AC_ARG_WITH([localedir-name],
            [AS_HELP_STRING([--with-localedir-name=DIR],
                            [specify where locales are installed (default: ${prefix}/share/locale)])],
            [LOCALEDIR="$withval"])
AC_SUBST([LOCALEDIR])

FONTFILE="\${DATADIR}/font/DejaVuSans.ttf"
AC_ARG_WITH([font-path],
            [AS_HELP_STRING([--with-font-path=FILE],
                            [specify the font file (default: ${datadir}/font/DejaVuSans.ttf)])],
            [FONTFILE="$withval"])
AC_SUBST([FONTFILE])

AC_ARG_ENABLE([autopackage],
              [AS_HELP_STRING([--enable-autopackage],
                              [Enable autopackage @<:@default=disable@:>@])],
              [],
              [enable_autopackage=no])
AM_CONDITIONAL([AUTOPACKAGE], [test "$enable_autopackage" = "yes"])

AM_CONDITIONAL([STATIC], [test x$static = xyes])
AM_CONDITIONAL([GCC], [test x$GXX = xyes])
AM_CONDITIONAL([INCLUDEDINTL], [test x$nls_cv_use_gnu_gettext = xyes])


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
    GCC_FLAGS="$GCC_FLAGS -O0 -g -DDEBUG"
  else
    GCC_FLAGS="$GCC_FLAGS -O2"
  fi

  dnl Ignore default options set by autoconf
  dnl CFLAGS="$CFLAGS $GCC_FLAGS"
  CFLAGS="$GCC_FLAGS"
  dnl CXXFLAGS="$CXXFLAGS $GCC_FLAGS"
  CXXFLAGS="$GCC_FLAGS"

else
  if test "x${debug}" = "xyes"; then
    CFLAGS="$CFLAGS -DDEBUG"
    CXXFLAGS="$CXXFLAGS -DDEBUG"
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

dnl === Check for pkg-config ===============================================

AC_PATH_PROGS([PACKAGE_CONFIG], [$host_alias-pkg-config pkg-config], [none])
if test "x$PACKAGE_CONFIG" = "xnone"; then
  AC_MSG_ERROR([*** pkg-config, needed to check for libxml++ existence has not been found.])
fi

dnl === Check for PNG ==================================================

AC_MSG_CHECKING(for libpng)
for PNG in png png12 png13 dummypng; do
  if $PACKAGE_CONFIG --exists lib$PNG > /dev/null 2>&1 ; then
    LIBPNG_CFLAGS=`$PACKAGE_CONFIG --cflags lib$PNG`
    LIBPNG_LIB=`$PACKAGE_CONFIG --libs lib$PNG`
    CFLAGS="$CFLAGS $LIBPNG_CFLAGS"
    LIBS="$LIBS $LIBPNG_LIB"
    break
  fi
done
if test "$LIBPNG_LIB" = "dummypng"; then
  echo "Got $LIBPNG_CFLAGS $LIBPNG_LIB"
  AC_MSG_ERROR([*** libpng not found])
else
  AC_MSG_RESULT(lib$PNG)
fi

dnl === Check for SDL_gfx ===========================================================

AC_ARG_WITH(sdl-gfx-prefix,
            [  --with-sdl-gfx-prefix=DIR       specify where SDL_gfx library is installed],
            [SDL_GFX_PREFIX="$withval"])
AC_SUBST(SDL_GFX_PREFIX)
if test "x$SDL_GFX_PREFIX" != "x"; then
  CPPFLAGS="$CPPFLAGS -I$SDL_GFX_PREFIX/include/SDL"
  CXXFLAGS="$CXXFLAGS -I$SDL_GFX_PREFIX/include/SDL"
  LIBS="-L$SDL_GFX_PREFIX/lib $LIBS"
fi
AC_CHECK_LIB(SDL_gfx, rotozoomSurfaceXY,,check_sdl_gfx="no")
AC_CHECK_HEADERS(SDL_rotozoom.h,,check_sdl_gfx="no")

dnl === Check for curl ==============================================================

AC_ARG_WITH(curl-prefix,
            [  --with-curl-prefix=DIR       specify where curl library is installed],
            [CURL_PREFIX="$withval"])
if test "x$CURL_PREFIX" != "x"; then CURL_BIN="$CURL_PREFIX/bin/curl-config"
else                                 CURL_BIN=curl-config; fi
AC_SUBST(CURL_PREFIX)
AC_CHECK_LIB(curl, curl_easy_init,check_curl="yes",check_curl="no")

dnl === Check for SDL image, mixer, etc., libxml++, libcurl and libpng ==============

AC_CHECK_LIB($PNG, png_error,,,)
AC_CHECK_LIB(SDL_image, IMG_Load,,check_sdl_image="no")
AC_CHECK_LIB(SDL_mixer, Mix_OpenAudio,,check_sdl_mixer="no")
AC_CHECK_LIB([SDL_ttf], [TTF_OpenFont],,check_sdl_ttf="no")
AC_CHECK_LIB([SDL_net], [SDLNet_Init],,check_sdl_net="no")
if test "${WIN32}" = "yes"; then LIBS="$LIBS -lwsock32 -lws2_32"; fi

AC_MSG_CHECKING(for libxml++)

if $PACKAGE_CONFIG libxml++-2.6 --libs > /dev/null 2>&1; then
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
  check_libxmlpp="no"
fi

AC_MSG_CHECKING(for libcurl)

if test x$check_curl = xyes; then
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
  check_libcurl="no"
  AC_MSG_ERROR([*** No libCurl found. Please install a valid libcurl ***])
fi

dnl === Display error if SDL image, mixer, etc. is missing ==================

if test \
     x${check_sdl_image} = xno \
  -o x${check_sdl_mixer} = xno \
  -o   x${check_sdl_ttf} = xno \
  -o   x${check_sdl_gfx} = xno \
  -o   x${check_sdl_net} = xno \
  -o  x${check_libxmlpp} = xno \
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
  if test x${check_sdl_gfx} = xno; then
    AC_MSG_RESULT([ERROR: SDL_gfx library version >= 2.0.13 required])
  fi
  if test x${check_sdl_net} = xno; then
    AC_MSG_RESULT([ERROR: SDL_net library required])
  fi
  if test x${check_libxmlpp} = xno; then
    AC_MSG_RESULT([ERROR: libxml++ version 2.6 not found!])
  fi
  exit 1
fi

dnl === Add libxml++, libcurl flags and libpng =================================================
LIBCURL_CFLAGS=`$CURL_BIN --cflags`
LIBCURL_LIBS=`$CURL_BIN --libs`

LIBXMLPP_CFLAGS=`$PACKAGE_CONFIG libxml++-2.6 --cflags`
LIBXMLPP_LIBS=`$PACKAGE_CONFIG libxml++-2.6 --libs`

LIBPNG_CFLAGS=`$PACKAGE_CONFIG lib$PNG --cflags`
LIBPNG_LIBS=`$PACKAGE_CONFIG lib$PNG --libs`

CFLAGS="$CFLAGS $LIBXMLPP_CFLAGS $LIBCURL_CFLAGGS $LIBPNG_CFLAGS"
CXXFLAGS="$CXXFLAGS $LIBXMLPP_CFLAGS $LIBCURL_CFLAGS $LIBPNG_CFLAGS"
LIBS="$LIBS $LIBXMLPP_LIBS $LIBCURL_LIBS $LIBPNG_LIBS"

dnl ========================================================================
dnl == Check for header files                                              =
dnl ========================================================================

dnl TODO: remove them when we don't need them any more ;)

AC_HEADER_DIRENT
AC_HEADER_STDC


dnl ========================================================================
dnl === Data file substitution.                                          ===
dnl ========================================================================

DATA_FILES=`cd data ; find . \( -name CVS -o -name ".svn*" -o -name "Makefile*" -o -name wormux.desktop \) -prune -o -name ".#*" -o -type d -o -print`
DATA_FILES=`echo $DATA_FILES`

AC_SUBST([DATA_FILES])
AC_SUBST([BUILD_ENV])

AC_CONFIG_FILES([Makefile
                 data/Makefile
                 src/Makefile
                 doc/Makefile
                 po/Makefile
                 ], [AM_POSTPROCESS_PO_MAKEFILE])

AM_CONFIG_HEADER(src/config.h)

AC_OUTPUT

if test "${WIN32}" = "yes"; then INSTALL_OP="build NSIS installer..."
else INSTALL_OP="install..."; fi

   echo "Configuration complete"
   echo ""
if test "$enable_autopackage" = "yes"; then
   echo "Use autopackage for file locations"
else
   echo "* Game data will be installed into $DATADIR"
   echo "* Game locale will be installed into $LOCALEDIR"
   echo "* Font file will be $FONTFILE"
fi
echo ""
echo "Execute make to compile then execute make install to $INSTALL_OP"
