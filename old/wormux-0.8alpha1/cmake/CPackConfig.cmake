# see http://www.cmake.org/Wiki/CMake:Packaging_With_CPack

INCLUDE(InstallRequiredSystemLibraries)  # seems to be only used for win32

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Wormux - a convivial mass murder game")
SET(CPACK_PACKAGE_NAME "Wormux")
SET(CPACK_PACKAGE_VENDOR "Wormux Development Team")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")
SET(CPACK_PACKAGE_VERSION_MAJOR ${WORMUX_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${WORMUX_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${WORMUX_PATCH})
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "Wormux ${WORMUX_MAJOR}.${WORMUX_MINOR}")
SET(CPACK_GENERATOR "TGZ")
SET(CPACK_SOURCE_GENERATOR "TGZ")
SET(CPACK_PACKAGE_EXECUTABLES "wormux" "Wormux")
SET(CPACK_STRIP_FILES "bin/wormux")
#SET(CPACK_SOURCE_IGNORE_FILES "/CVS/;/\\.svn/;\\.swp$;\\.#;/#;.*~")

INCLUDE(CPack)

#IF(WIN32 AND NOT UNIX)
#  # There is a bug in NSI that does not handle full unix paths properly. Make
#  # sure there is at least one set of four (4) backlasshes.
#  SET(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/data\\\\InstallIcon.bmp")   # TODO: Need InstallIcon !!!
#  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
#  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} Wormux")
#  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.wormux.org")
#  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.wormux.org")
#  SET(CPACK_NSIS_CONTACT "wormux-dev@gna.org")
#  SET(CPACK_NSIS_MODIFY_PATH ON)
#ELSE(WIN32 AND NOT UNIX)
#  SET(CPACK_STRIP_FILES "bin/wormux")
#  SET(CPACK_SOURCE_STRIP_FILES "")
#ENDIF(WIN32 AND NOT UNIX)

