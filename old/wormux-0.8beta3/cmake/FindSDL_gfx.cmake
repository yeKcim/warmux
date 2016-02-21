# Locate SDL_gfx library
# Based on FindSDL_image.cmake included in cmake
#
# This module defines
#  SDLGFX_LIBRARY, the library to link against
#  SDLGFX_FOUND, if false, do not try to link to SDL_gfx
#  SDLGFX_INCLUDE_DIR, where to find SDL_gfxPrimitives.h

FIND_PATH(SDLGFX_INCLUDE_DIR SDL_gfxPrimitives.h
  $ENV{SDLDIR}/include
  ~/Library/Frameworks/SDL_gfx.framework/Headers
  /Library/Frameworks/SDL_gfx.framework/Headers
  /usr/local/include/SDL
  /usr/include/SDL
  /usr/local/include/SDL12
  /usr/local/include/SDL11 # FreeBSD ports
  /usr/include/SDL12
  /usr/include/SDL11
  /usr/local/include
  /usr/include
  /sw/include/SDL # Fink
  /sw/include
  /opt/local/include/SDL # DarwinPorts
  /opt/local/include
  /opt/csw/include/SDL # Blastwave
  /opt/csw/include
  /opt/include/SDL
  /opt/include
)

# Look at FindSDL.cmake included in cmake for further comments
IF(${SDLGFX_INCLUDE_DIR} MATCHES ".framework")
  STRING(REGEX REPLACE "(.*)/.*\\.framework/.*" "\\1" SDLGFX_FRAMEWORK_PATH_TEMP ${SDLGFX_INCLUDE_DIR})
  IF("${SDLGFX_FRAMEWORK_PATH_TEMP}" STREQUAL "/Library/Frameworks" OR "${SDLGFX_FRAMEWORK_PATH_TEMP}" STREQUAL "/System/Library/Frameworks")
    SET(SDLGFX_LIBRARY_TEMP "-framework SDL_gfx")
  ELSE("${SDLGFX_FRAMEWORK_PATH_TEMP}" STREQUAL "/Library/Frameworks" OR "${SDLGFX_FRAMEWORK_PATH_TEMP}" STREQUAL "/System/Library/Frameworks")
    SET(SDLGFX_LIBRARY_TEMP "-F${SDLGFX_FRAMEWORK_PATH_TEMP} -framework SDL_gfx")
  ENDIF("${SDLGFX_FRAMEWORK_PATH_TEMP}" STREQUAL "/Library/Frameworks" OR "${SDLGFX_FRAMEWORK_PATH_TEMP}" STREQUAL "/System/Library/Frameworks")
  SET(SDLGFX_FRAMEWORK_PATH_TEMP "" CACHE INTERNAL "")
ELSE(${SDLGFX_INCLUDE_DIR} MATCHES ".framework")
  FIND_LIBRARY(SDLGFX_LIBRARY
    NAMES SDL_gfx
    PATHS
    $ENV{SDLDIR}/lib
    /usr/local/lib
    /usr/lib
    /sw/lib
    /opt/local/lib
    /opt/csw/lib
    /opt/lib
    )
ENDIF(${SDLGFX_INCLUDE_DIR} MATCHES ".framework")

SET(SDLGFX_FOUND "NO")
IF(SDLGFX_LIBRARY)
  SET(SDLGFX_FOUND "YES")
ENDIF(SDLGFX_LIBRARY)

