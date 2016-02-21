dnl ========================================================================
dnl =                   W O R M U X  -  confiure.ac                        =
dnl =                                                                      =
dnl = Process this file with autoconf to produce a configure script.       =
dnl ========================================================================


dnl ========================================================================
dnl === Initial configuration                                              =
dnl ========================================================================

dnl = Require at least automake 2.52
AC_PREREQ(2.52)

AC_INIT([Wormux], [0.7.3], [wormux-dev@gna.org], [wormux])
AC_CONFIG_SRCDIR([src/main.cpp])

dnl Detect the canonical host and target build environment
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

#AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([foreign 1.5])

AM_MAINTAINER_MODE
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

DATADIR="${prefix}/share/wormux"
AC_ARG_WITH(datadir-name,
    [  --with-datadir-name=DIR       specify where datas are installed (default: ${prefix}/share/wormux)],
    [DATADIR="$withval"])
AC_SUBST([DATADIR])


LOCALEDIR="${prefix}/share/locale"
AC_ARG_WITH(localedir-name,
    [  --with-localedir-name=DIR       specify where locales are installed (default: ${prefix}/share/locale)],
    [LOCALEDIR="$withval"])
AC_SUBST([LOCALEDIR])

FONTFILE="\${DATADIR}/font/DejaVuSans.ttf"
AC_ARG_WITH(font-path,
    [  --with-font-path=FILE       specify the font file (default: ${datadir}/font/DejaVuSans.ttf)],
    [FONTFILE="$withval"])
AC_SUBST([FONTFILE])

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

    dnl Skip 'unused parameter' warning
    GCC_FLAGS="$GCC_FLAGS -Wno-unused-parameter"

    if test "${debug}" = "yes"; then
        GCC_FLAGS="$GCC_FLAGS -O0 -g -DDEBUG"
    else
        GCC_FLAGS="$GCC_FLAGS -O2"
    fi

    CFLAGS="$CFLAGS $GCC_FLAGS"
    CXXFLAGS="$CXXFLAGS $GCC_FLAGS"
else
    if test "${debug}" = "yes"; then
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

dnl === Check for SDL_image ================================================

AC_CHECK_LIB(SDL_image, IMG_Load,,
	AC_MSG_ERROR([SDL_image library required]))
LIBS="$LIBS -lSDL_image"


dnl === Check for SDL_mixer ================================================

AC_CHECK_LIB(SDL_mixer, Mix_OpenAudio,,
	AC_MSG_ERROR([SDL_mixer library required]))
LIBS="$LIBS -lSDL_mixer"


dnl === Check for SDL_ttf ==================================================

AC_CHECK_LIB([SDL_ttf], [TTF_OpenFont],,
        AC_MSG_ERROR([SDL_ttf library is required]))
LIBS="$LIBS -lSDL_ttf"

dnl === Check for SDL_net ==================================================

AC_CHECK_LIB([SDL_net], [SDLNet_Init],,
        AC_MSG_ERROR([SDL_net library is required]))
LIBS="$LIBS -lSDL_net"

dnl === Check for SDL_gfx ==================================================

AC_ARG_WITH(sdl-gfx-prefix,
    [  --with-sdl-gfx-prefix=DIR       specify where SDL_gfx library is installed],
    [SDL_GFX_PREFIX="$withval"])
AC_SUBST(SDL_GFX_PREFIX)
if test "x$SDL_GFX_PREFIX" != "x"; then
  CPPFLAGS="$CPPFLAGS -I$SDL_GFX_PREFIX/include/SDL"
  CXXFLAGS="$CXXFLAGS -I$SDL_GFX_PREFIX/include/SDL"
fi
AC_CHECK_LIB(SDL_gfx, rotozoomSurfaceXY,,
	AC_MSG_ERROR([SDL_gfx library >= 2.0.13 required]))
AC_CHECK_HEADERS(SDL_rotozoom.h,,
    AC_MSG_ERROR([SDL_gfx library headers are missing]))
LIBS="$LIBS -lSDL_gfx"


#dnl === Check for paragui-config  ==========================================

#AC_PATH_PROGS([PGUI_CONFIG], [paragui-config], [none])
#if test "x$PGUI_CONFIG" = "xnone"; then
#	AC_MSG_ERROR([*** paragui-config has not been found.])
#fi
#AC_MSG_CHECKING(for Paragui)
#if $PGUI_CONFIG --libs > /dev/null 2>&1; then
#	AC_MSG_RESULT(yes)
#else
#	AC_MSG_RESULT(no)
#	AC_MSG_ERROR([*** Paragui not found!])
#fi
#PARAGUI_CFLAGS=`$PGUI_CONFIG --cflags`
#PARAGUI_LIBS=`$PGUI_CONFIG --libs`
#CFLAGS="$CFLAGS $PARAGUI_CFLAGS"
#CXXFLAGS="$CXXFLAGS $PARAGUI_CFLAGS"
#LIBS="$LIBS $PARAGUI_LIBS"


dnl === Check for libxml++ 2.6 =============================================

dnl Check first for pkg-config

AC_PATH_PROGS([PACKAGE_CONFIG], [pkg-config], [none])
if test "x$PACKAGE_CONFIG" = "xnone"; then

	AC_MSG_ERROR([*** pkg-config, needed to check for libxml++ existence
has not been found.])
fi

AC_MSG_CHECKING(for libxml++)

if $PACKAGE_CONFIG libxml++-2.6 --libs > /dev/null 2>&1; then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
	AC_MSG_ERROR([*** libxml++ version 2.6 not found!])
fi
LIBXMLPP_CFLAGS=`$PACKAGE_CONFIG libxml++-2.6 --cflags`
LIBXMLPP_LIBS=`$PACKAGE_CONFIG libxml++-2.6 --libs`
CFLAGS="$CFLAGS $LIBXMLPP_CFLAGS"
CXXFLAGS="$CXXFLAGS $LIBXMLPP_CFLAGS"
LIBS="$LIBS $LIBXMLPP_LIBS"

dnl ========================================================================
dnl == Set flags for various environments                                  =
dnl ========================================================================
case "${host}" in
  i[[3456789]]86-*-mingw32*) WIN32="yes" ;;
  *cygwin*) WIN32="yes" ;;
  *) WIN32="no" ;;
esac
AM_CONDITIONAL([WIN32], test "$WIN32" = "yes")


dnl ========================================================================
dnl == Check for header files                                              =
dnl ========================================================================

dnl TODO: remove them when we don't need them any more ;)

AC_HEADER_DIRENT
AC_HEADER_STDC


dnl ========================================================================
dnl === Data file substitution.                                          ===
dnl ========================================================================

DATA_FILES=`cd data ; find . -name CVS -prune -o -name ".svn*" -prune -o -name "Makefile*" -o -name ".#*" -o -type d -o -print`
DATA_FILES=`echo $DATA_FILES`

AC_SUBST([DATA_FILES])
AC_SUBST([BUILD_ENV])

AM_PO_SUBDIRS

AC_CONFIG_FILES([Makefile
		 data/Makefile
		 src/Makefile
		 doc/Makefile
		 ])

AC_CONFIG_FILES([po/Makefile], [AM_POSTPROCESS_PO_MAKEFILE])

AM_CONFIG_HEADER(src/config.h)

AC_OUTPUT

AC_MSG_RESULT([

Configuration complete

* Game data will be installed into $DATADIR
* Game locale will be installed into $LOCALEDIR
* Font file will be $FONTFILE

Execute make to compile then execute make install to install...

])
