cmake_minimum_required(VERSION 2.4.4)

# Project name
PROJECT(Wormux)
SET(WORMUX_MAJOR "0")
SET(WORMUX_MINOR "8.1")
SET(WORMUX_PATCH "")
SET(VERSION "${WORMUX_MAJOR}.${WORMUX_MINOR}${WORMUX_PATCH}")

# change the default install prefix to /usr
SET(CMAKE_INSTALL_PREFIX /usr)

# Some aliases of cmake variables
IF(BUILD)
  SET(BUILD CACHE STRING "Abbreviation for CMAKE_BUILD_TYPE.")
  SET(CMAKE_BUILD_TYPE ${BUILD})
ENDIF(BUILD)
IF(PREFIX)
  SET(PREFIX CACHE PATH "Abbreviation for CMAKE_INSTALL_PREFIX.")
  SET(CMAKE_INSTALL_PREFIX ${PREFIX})
ENDIF(PREFIX)

# Set some global variables
SET(WORMUX_CFLAGS "-Wall")	# additional compile flags for wormux
SET(WORMUX_LDFLAGS "")		# additional linking flags for wormux
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

IF(BIN_PATH)
  # If user specified a path for binary, use it
  SET(BIN_DESTINATION ${BIN_PATH})
ELSE(BIN_PATH)
  # Otherwise use defaults
  IF(UNIX AND NOT WIN32)
    SET(BIN_DESTINATION "bin")
  ELSE(UNIX AND NOT WIN32)
    SET(BIN_DESTINATION ${CMAKE_BINARY_DIR})
  ENDIF(UNIX AND NOT WIN32)
ENDIF(BIN_PATH)

IF(DATA_PATH)
  # If user specified a path for data, use it
  
  SET(INSTALL_DATADIR ${DATA_PATH}/data/)
  SET(INSTALL_LOCALEDIR ${DATA_PATH}/locale/)
  SET(DATA_DESTINATION ${DATA_PATH}/data)
  SET(PO_DESTINATION ${DATA_PATH}/locale)
  SET(FONT_FILE ${DATA_PATH}/data/font/DejaVuSans.ttf)
  
ELSE(DATA_PATH)

  # Otherwise use defaults
  IF(UNIX AND NOT WIN32)
    SET(INSTALL_DATADIR ${CMAKE_INSTALL_PREFIX}/share/wormux/)
    SET(INSTALL_LOCALEDIR ${CMAKE_INSTALL_PREFIX}/share/locale/)
    SET(DATA_DESTINATION "share/wormux")
    SET(PO_DESTINATION "share/locale")
    SET(FONT_FILE ${INSTALL_DATADIR}/font/DejaVuSans.ttf)
  ELSE(UNIX AND NOT WIN32)
    SET(INSTALL_DATADIR data/)
    SET(INSTALL_LOCALEDIR locale/)
    SET(DATA_DESTINATION "data")
    SET(PO_DESTINATION "locale")
    SET(FONT_FILE data/font/DejaVuSans.ttf)
  ENDIF(UNIX AND NOT WIN32)
  
ENDIF(DATA_PATH)

# Optional package CPack
INCLUDE(CPackConfig OPTIONAL)
INCLUDE(XmlValidityTest OPTIONAL)

# Use "make run" and "make debug" to run/debug wormux
ADD_CUSTOM_TARGET(run WORMUX_DATADIR=${PROJECT_SOURCE_DIR}/data/ WORMUX_FONT_PATH=${PROJECT_SOURCE_DIR}/data/font/DejaVuSans.ttf src/wormux DEPENDS src/wormux)
ADD_CUSTOM_TARGET(debug WORMUX_DATADIR=${PROJECT_SOURCE_DIR}/data/ WORMUX_FONT_PATH=${PROJECT_SOURCE_DIR}/data/font/DejaVuSans.ttf gdb src/wormux DEPENDS src/wormux)


# Required libraries
FIND_PACKAGE(Gettext REQUIRED)
FIND_PACKAGE(SDL REQUIRED)
FIND_PACKAGE(SDL_image REQUIRED)
FIND_PACKAGE(SDL_mixer REQUIRED)
FIND_PACKAGE(SDL_ttf REQUIRED)
FIND_PACKAGE(SDL_gfx REQUIRED)
FIND_PACKAGE(SDL_net REQUIRED)
FIND_PACKAGE(LibXml2 REQUIRED)
FIND_PACKAGE(CURL REQUIRED)

IF(APPLE)
  # to ease packaging, we'll use a static libintl
  # we also need to link against the SDL_main library (required by the mac version of SDL)
  SET(WORMUX_LDFLAGS "-L${PROJECT_SOURCE_DIR} -lintl -liconv -framework Carbon -lSDLmain_UB ${WORMUX_LDFLAGS}")
  # to ease packaging, we'll use a framework version of libpng
  # (with a different name so there is no confusion with the UNIX version of libpng)
  SET(WORMUX_LDFLAGS "-framework wmxlibpng -I/Library/Frameworks/wmxlibpng.framework/Headers/ ${WORMUX_LDFLAGS}")
ELSE(APPLE)
  FIND_PACKAGE(PNG REQUIRED)
ENDIF(APPLE)

# new stuff in 2.6; remove when it is widespread enough and we switch (see top)
# CMP0005 - ADD_DEFINITION is now properly autoescaped
IF(COMMAND cmake_policy)
  cmake_policy(SET CMP0005 OLD)
ENDIF(COMMAND cmake_policy)

ADD_DEFINITIONS(${LIBXML2_DEFINITIONS})


# config.h and compile-flags
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/src/config.h.cmake ${PROJECT_SOURCE_DIR}/src/config.h)
ADD_DEFINITIONS(-DHAVE_CONFIG_H)

ADD_DEFINITIONS(-DINSTALL_DATADIR=\\"${INSTALL_DATADIR}\\")
ADD_DEFINITIONS(-DINSTALL_LOCALEDIR=\\"${INSTALL_LOCALEDIR}\\")
ADD_DEFINITIONS(-DFONT_FILE=\\"${FONT_FILE}\\")
ADD_DEFINITIONS(-DPACKAGE_VERSION=\\"${VERSION}\\")

IF(NOT WIN32)
  INCLUDE(CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG("-Wextra" COMPILER_SUPPORTS_WEXTRA)
  IF(COMPILER_SUPPORTS_WEXTRA)
    SET(WORMUX_CFLAGS "-Wextra ${WORMUX_CFLAGS}")
  ENDIF(COMPILER_SUPPORTS_WEXTRA)
  SET(WORMUX_CFLAGS "-Werror ${WORMUX_CFLAGS}")
ENDIF(NOT WIN32)
IF(CMAKE_BUILD_TYPE MATCHES "Debug" OR CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
  ADD_DEFINITIONS(-DDEBUG)
  IF(PROFILE)
    SET(WORMUX_CFLAGS "-pg ${WORMUX_CFLAGS}")
    SET(WORMUX_LDFLAGS "-pg ${WORMUX_LDFLAGS}")
  ENDIF(PROFILE)
ELSE(CMAKE_BUILD_TYPE MATCHES "Debug" OR CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
  # optimise nondebug builds
  SET(WORMUX_CFLAGS "-O2 ${WORMUX_CFLAGS}")
  IF(NOT WIN32 AND NOT APPLE)
    SET(WORMUX_CFLAGS "-march=native ${WORMUX_CFLAGS}") # native only works on x86 and x86_64
  ENDIF(NOT WIN32 AND NOT APPLE)
ENDIF(CMAKE_BUILD_TYPE MATCHES "Debug" OR CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")


# Subdirectories
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(po)


# install data/
INSTALL(DIRECTORY data/ DESTINATION ${DATA_DESTINATION}
  FILE_PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
  DIRECTORY_PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ 
                        GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  PATTERN ".svn" EXCLUDE
  PATTERN "Makefile.am" EXCLUDE
  PATTERN "wormux.desktop" EXCLUDE
  PATTERN ".dtd" EXCLUDE
)

# Install desktop file and icon
IF(UNIX AND NOT APPLE)
  INSTALL(FILES data/wormux.desktop DESTINATION share/applications)
  INSTALL(FILES data/wormux_128x128.png DESTINATION share/pixmaps)
ENDIF(UNIX AND NOT APPLE)
