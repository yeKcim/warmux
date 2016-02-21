# - Find GNU gettext tools
# This module looks for the GNU gettext tools. This module defines the 
# following values:
#  GETTEXT_MSGMERGE_EXECUTABLE: the full path to the msgmerge tool.
#  GETTEXT_MSGFMT_EXECUTABLE: the full path to the msgfmt tool.
#  GETTEXT_FOUND: True if gettext has been found.
#  GETTEXT_INCLUDE_DIR: the full path to libintl.h
#  GETTEXT_LIBRARY: the full path to libintl.so (if needed)
#
# Additionally it provides the following macros:
# GETTEXT_CREATE_TRANSLATIONS ( outputFile [ALL] file1 ... fileN )
#    This will create a target "translations" which will convert the 
#    given input po files into the binary output mo file. If the 
#    ALL option is used, the translations will also be created when
#    building the default target.

INCLUDE(CheckIncludeFiles)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckFunctionExists)

FIND_PROGRAM(GETTEXT_MSGMERGE_EXECUTABLE msgmerge)
FIND_PROGRAM(GETTEXT_XGETTEXT_EXECUTABLE xgettext)
FIND_PROGRAM(GETTEXT_MSGFMT_EXECUTABLE msgfmt)

# additional macro
MACRO(GETTEXT_CREATE_TRANSLATIONS _potFile _firstPoFile)

   SET(_gmoFiles)
   GET_FILENAME_COMPONENT(_potBasename ${_potFile} NAME_WE)
   GET_FILENAME_COMPONENT(_absPotFile ${_potFile} ABSOLUTE)

   SET(_addToAll)
   IF(${_firstPoFile} STREQUAL "ALL")
      SET(_addToAll "ALL")
      SET(_firstPoFile)
   ENDIF(${_firstPoFile} STREQUAL "ALL")

   FOREACH (_currentPoFile ${ARGN})
      GET_FILENAME_COMPONENT(_absFile ${_currentPoFile} ABSOLUTE)
      GET_FILENAME_COMPONENT(_abs_PATH ${_absFile} PATH)
      GET_FILENAME_COMPONENT(_lang ${_absFile} NAME_WE)
      SET(_gmoFile ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.gmo)

      ADD_CUSTOM_COMMAND( 
         OUTPUT ${_gmoFile} 
         COMMAND ${GETTEXT_MSGMERGE_EXECUTABLE} --quiet --update --backup=none -s ${_absFile} ${_absPotFile}
         COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_gmoFile} ${_absFile}
         DEPENDS ${_absPotFile} ${_absFile} 
      )

      INSTALL(FILES ${_gmoFile} DESTINATION share/locale/${_lang}/LC_MESSAGES RENAME ${_potBasename}.mo) 
      SET(_gmoFiles ${_gmoFiles} ${_gmoFile})

   ENDFOREACH (_currentPoFile )

   ADD_CUSTOM_TARGET(translations ${_addToAll} DEPENDS ${_gmoFiles})

ENDMACRO(GETTEXT_CREATE_TRANSLATIONS )

# Actual search logic
FIND_PATH(GETTEXT_INCLUDE_DIR
  NAMES libintl.h
  PATH /usr/local/include /usr/pkg/include /usr/include
)

SET(CMAKE_REQUIRED_INCLUDES ${GETTEXT_INCLUDE_DIR})
CHECK_INCLUDE_FILES(libintl.h HAVE_LIBINTL_H)

IF(HAVE_LIBINTL_H)
  CHECK_FUNCTION_EXISTS(dgettext LIBC_HAS_DGETTEXT)
  IF(LIBC_HAS_DGETTEXT)
    SET(GETTEXT_FOUND TRUE)
  ELSE(LIBC_HAS_DGETTEXT)
    FIND_LIBRARY(GETTEXT_LIBRARY NAMES intl
      PATHS
      /usr/local/lib
      /usr/lib
      )
    IF(GETTEXT_LIBRARY)
      IF(${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
        SET(CMAKE_REQUIRED_LIBRARIES "iconv")
        CHECK_LIBRARY_EXISTS(${GETTEXT_LIBRARY} "libintl_dgettext" "" LIBINTL_HAS_DGETTEXT)
      ELSE(${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
        CHECK_LIBRARY_EXISTS(${GETTEXT_LIBRARY} "dgettext" "" LIBINTL_HAS_DGETTEXT)
      ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
      
      IF(LIBINTL_HAS_DGETTEXT)
	SET(GETTEXT_FOUND TRUE)
      ENDIF(LIBINTL_HAS_DGETTEXT)
    ENDIF(GETTEXT_LIBRARY)
  ENDIF(LIBC_HAS_DGETTEXT)
ELSE(HAVE_LIBINTL_H)
  SET(GETTEXT_FOUND FALSE)
ENDIF(HAVE_LIBINTL_H)

IF (NOT GETTEXT_MSGMERGE_EXECUTABLE OR NOT GETTEXT_MSGFMT_EXECUTABLE)
   SET(GETTEXT_FOUND FALSE)
ENDIF (NOT GETTEXT_MSGMERGE_EXECUTABLE OR NOT GETTEXT_MSGFMT_EXECUTABLE)

IF (NOT GETTEXT_FOUND)
   IF (GetText_REQUIRED)
      MESSAGE(FATAL_ERROR "GetText not found")
   ENDIF (GetText_REQUIRED)
ENDIF (NOT GETTEXT_FOUND)
