# Use "make xmlcheck" to test if XML files are valid

FILE(GLOB_RECURSE XMLFILES data/*.xml)
SET(FILELIST)
FOREACH(_xml ${XMLFILES})
  SET(FILELIST ${FILELIST} ${_xml})
ENDFOREACH(_xml ${XMLFILES})
FIND_PROGRAM(XMLLINT xmllint)
IF(XMLLINT)
  ADD_CUSTOM_TARGET(xmlcheck ${XMLLINT} --valid --noout ${FILELIST})
ENDIF(XMLLINT)
