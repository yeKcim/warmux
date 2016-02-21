#!/bin/sh

CXXFLAGS=""

LIBS=""

# libxml++ 1.0
#LIBS="$LIBS -lxml++-1.0"

# Game deps
LIBS="$LIBS -lxml2 -lpthread -ljpeg -lpng12 -lvorbis -logg"

# X deps
LIBS="$LIBS -L/usr/X11R6/lib -L/usr/lib -lX11 -lGLU -lXext -lGL -lXi -lXt"
# -lXxf86vm

# Wormux objects 
OBJ="`ls src/*.o`"
SDL_OBJ=
OBJ=" $OBJ $SDL_OBJ /usr/X11R6/lib/libXxf86vm.a"

# Output
echo "LIBS = "
echo $LIBS
echo

echo "CMD ="
set -x

g++ -Wall -Werror -O2 -o wormux $OBJ /usr/lib/libxml++-1.0.a $CXXFLAGS $LIBS 2>&1
strip wormux
