FILE(GLOB PO_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.po) 

SET(COPYRIGHT_HOLDER "Wormux Development Team")
SET(BUGS_ADDRESS "wormux-dev@gna.org")
SET(POT_FILE ${CMAKE_CURRENT_BINARY_DIR}/wormux.pot)

# Create PO Template file 'wormux.pot'
ADD_CUSTOM_COMMAND(
  OUTPUT ${POT_FILE}
  COMMAND ${GETTEXT_XGETTEXT_EXECUTABLE}
    ARGS -o ${POT_FILE} -s --omit-header 
       --msgid-bugs-address=${BUGS_ADDRESS} -c --keyword=_ --keyword=N_ 
       --copyright-holder=${COPYRIGHT_HOLDER} ${WORMUX_SRCS}
  # strip ${PROJECT_SOURCE_DIR} from the code pointing comments (#:)
  COMMAND sed
    ARGS -i "s,${PROJECT_SOURCE_DIR}/*,,g" ${POT_FILE}
  DEPENDS ${WORMUX_SRCS}
)

# Update .po files and compile them to binary .gmo files
GETTEXT_CREATE_TRANSLATIONS(${POT_FILE} ALL ${PO_FILES})
