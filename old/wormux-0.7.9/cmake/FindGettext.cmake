# Search for gettext binaries
#
# This module defines
#  XGETTEXT_EXECUTABLE
#  MSGFMT_EXECUTABLE
#  MSGMERGE_EXECUTABLE
#  GETTEXT_FOUND, set whether the gettext binaries have been found


FIND_PROGRAM(XGETTEXT_EXECUTABLE xgettext)
FIND_PROGRAM(MSGFMT_EXECUTABLE msgfmt)
FIND_PROGRAM(MSGMERGE_EXECUTABLE msgmerge)

IF(XGETTEXT_EXECUTABLE AND MSGFMT_EXECUTABLE AND MSGMERGE_EXECUTABLE)
  SET(GETTEXT_FOUND TRUE)
ENDIF(XGETTEXT_EXECUTABLE AND MSGFMT_EXECUTABLE AND MSGMERGE_EXECUTABLE)


#MACRO(COMPILE_POFILES _pofiles)
#  SET(_gmofiles)
#  FOREACH(_current_po ${_pofiles})
#    GET_FILENAME_COMPONENT(_basename ${_current_po} NAME_WE)
#    SET(_gmofile ${_basename}.gmo)
#    ADD_CUSTOM_COMMAND(OUTPUT ${_gmofile} COMMAND ${MSGFMT_EXECUTABLE} ARGS "-o" "${_gmofile}" "${_current_po}" DEPENDS ${_current_po})
#    SET(_gmofiles ${_gmofiles} ${_gmofile})
#  ENDFOREACH(_current_po ${_pofiles})
#  ADD_CUSTOM_TARGET(po ALL DEPENDS ${_gmofiles})
#ENDMACRO(COMPILE_POFILES _pofiles)
