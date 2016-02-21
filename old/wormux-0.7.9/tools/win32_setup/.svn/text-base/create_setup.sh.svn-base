#!/bin/bash

## Programs 
PKG_CONFIG=pkg-config
MAKE_NSIS=makensis

# Path to which all others are relative
WORMUXDIR="../.."

# Path of MinGW installation
MINGWDIR=/mingw

# Windows registry software path
HKLM_PATH="SOFTWARE\Games\Wormux"

# Version number in installer
WORMUX_VERSION=`date +"%Y%m%d"`

# Set compression and in/out
COMPRESSION=lzma
DEST=tmp-wormux-win32
NSIS="$DEST/wormux.nsi"

# Clean prior stuff
rm -rf $NSIS $DEST

# Prepare stuff
WIN_WORMUXDIR=$(cd "$WORMUXDIR" && cmd /c cd)
LOCAL_PATH=$(cmd /c cd)
mkdir -p $DEST

function pkg_path
{
  if [ -z "$1" ]; then return ""; fi
  var=$($PKG_CONFIG --variable=prefix $1 2>/dev/null)
  if [ -z "$var" ]; then echo "Couldn't find $1, aborting..." 1>&2; exit 1; fi
  echo "Found $1 in $var" 1>&2
  echo "$var"
}

## Make sure all is done
echo "Checking make status"
if ! (cd "$WORMUXDIR" && make 1>/dev/null 2>&1); then
  echo "Bad return code from make; aborting"
  exit 1
fi

if ! pkg-config --help 2>/dev/null 1>&2 ; then
  echo "pkg-config not found, aborting..."
  exit 1
fi

# Create head
cat > $NSIS <<EOF
;based on MUI Welcome/Finish Page Example Script written by Joost Verburg
!define MUI_ICON  "${WIN_WORMUXDIR}\src\wormux.ico"
!define MUI_UNICON  ${LOCAL_PATH}\uninstall.ico
!define MUI_COMPONENTSPAGE_SMALLDESC
!define MUI_LANGDLL_ALWAYSSHOW
!include "MUI.nsh"
!include "Sections.nsh"
!include "LogicLib.nsh"
!include "StrFunc.nsh"

Name "Wormux"

;General
OutFile "${LOCAL_PATH}\Wormux-Setup-${WORMUX_VERSION}.exe"
SetCompressor $COMPRESSION

!define MUI_LANGDLL_REGISTRY_ROOT "HKLM" 
!define MUI_LANGDLL_REGISTRY_KEY "${HKLM_PATH}" 
!define MUI_LANGDLL_REGISTRY_VALUENAME "lang"

;--------------------------------
;Configuration

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE \$(WormuxLicense)
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!define MUI_ABORTWARNING

;Languages
!insertmacro MUI_LANGUAGE "English"
LicenseLangString WormuxLicense "English" "${WIN_WORMUXDIR}\doc\license\COPYING.en.txt"
LangString TITLE_Wormux "English" "Wormux"
LangString DESC_Wormux  "English" "Installs the game Wormux, version ${WORMUX_VERSION}"

!insertmacro MUI_LANGUAGE "French"
LicenseLangString WormuxLicense "French" "${WIN_WORMUXDIR}\doc\license\COPYING.fr.txt"
LangString TITLE_Wormux "French" "Wormux"
LangString DESC_Wormux  "French" "Installe le jeu Wormux, en version ${WORMUX_VERSION}"

!insertmacro MUI_LANGUAGE "German"
LicenseLangString WormuxLicense "German" "${WIN_WORMUXDIR}\doc\license\COPYING.de.txt"
LangString TITLE_Wormux "German" "Wormux"
LangString DESC_Wormux  "German" "Das Spiel Wormux, Version ${WORMUX_VERSION} anbringen"

!insertmacro MUI_LANGUAGE "Spanish"
LicenseLangString WormuxLicense "Spanish" "${WIN_WORMUXDIR}\doc\license\COPYING.es.txt"
LangString TITLE_Wormux "Spanish" "Wormux"
LangString DESC_Wormux  "Spanish" "Instala el juego Wormux, versión ${WORMUX_VERSION}"

!insertmacro MUI_LANGUAGE "Dutch"
LicenseLangString WormuxLicense "Dutch" "${WIN_WORMUXDIR}\doc\license\COPYING.nl.txt"
LangString TITLE_Wormux "Dutch" "Wormux"
LangString DESC_Wormux  "Dutch" "Wormux ${WORMUX_VERSION}"

!insertmacro MUI_LANGUAGE "Polish"
LicenseLangString WormuxLicense "Polish" "${WIN_WORMUXDIR}\doc\license\COPYING.pl.txt"
LangString TITLE_Wormux "Polish" "Wormux"
LangString DESC_Wormux  "Polish" "Wormux ${WORMUX_VERSION}"

!insertmacro MUI_LANGUAGE "Russian"
LicenseLangString WormuxLicense "Russian" "${WIN_WORMUXDIR}\doc\license\COPYING.ru.txt"
LangString TITLE_Wormux "Russian" "Wormux"
LangString DESC_Wormux  "Russian" "Wormux ${WORMUX_VERSION}"

!insertmacro MUI_LANGUAGE "Slovenian"
LicenseLangString WormuxLicense "Slovenian" "${WIN_WORMUXDIR}\doc\license\COPYING.sk.txt"
LangString TITLE_Wormux "Slovenian" "Wormux"
LangString DESC_Wormux  "Slovenian" "Wormux ${WORMUX_VERSION}"

;Folder-selection page
InstallDir "\$PROGRAMFILES\Wormux"
; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM ${HKLM_PATH} "pth"

AutoCloseWindow false

;--------------------------------
;Installer Sections

Section \$(TITLE_Wormux) Sec_Wormux
  ; Set output path to the installation directory.
  SetOutPath \$INSTDIR
  File "${WIN_WORMUXDIR}\src\wormux.ico"
  ; Executing in tmpdir, looking for file in folder below
  File "${LOCAL_PATH}\uninstall.ico"
  File "${WIN_WORMUXDIR}\src\wormux.exe"
EOF

# Glib (gobject, gthread, glib & gmodule)
GLIB_PATH=$(pkg_path glib-2.0)
cp "$GLIB_PATH/bin/libgobject"*.dll "$GLIB_PATH/bin/libgthread"*.dll	\
   "$GLIB_PATH/bin/libglib"*.dll "$GLIB_PATH/bin/libgmodule"*.dll "$DEST"

# All gettext (with iconv), jpeg and zlib stuff should be within
# glademm install path <=> glib path
cp "$GLIB_PATH/bin/intl.dll" "$GLIB_PATH/bin/iconv.dll"		\
   "$GLIB_PATH/bin/jpeg62.dll" "$DEST"

# Other libs
cp "$(pkg_path sigc++-2.0)/bin/libsigc"*.dll $DEST
cp "$(pkg_path libxml-2.0)/bin/libxml2"*.dll $DEST
cp "$(pkg_path libxml++-2.6)/bin/libxml++"*.dll $DEST
cp "$(pkg_path glibmm-2.4)/bin/libglibmm"*.dll $DEST
cp "$(pkg_path libpng13)/bin/libpng13.dll" $DEST

# Clean up before non-strippable files
# WARNING Stripping some dlls corrupts them beyond use
strip "$DEST/"*.dll "$WORMUXDIR/src/"*.exe

# Files that must not be stripped (all of SDL)
SDL_PATH=$(sdl-config --prefix)
cp "$SDL_PATH/bin/SDL_mixer.dll" "$SDL_PATH/bin/SDL_ttf.dll"	\
   "$SDL_PATH/bin/SDL_image.dll" "$SDL_PATH/bin/SDL.dll"	\
   "$SDL_PATH/bin/SDL_net.dll" "$GLIB_PATH/bin/intl.dll"	\
   "$GLIB_PATH/bin/iconv.dll" "$GLIB_PATH/bin/jpeg62.dll"	\
   "$GLIB_PATH/bin/zlib1.dll" $DEST

# Continue producing installer
cat >> $NSIS <<EOF
  ; Dlls and co
  File "*.dll"
EOF

## Locale
echo -e "\n  ; Locale" >> $NSIS
for gmo in "$WORMUXDIR"/po/*.gmo; do
  lg=${gmo%%.gmo}
  lg=${lg//.*\//}
  echo "  SetOutPath \$INSTDIR\\locale\\$lg\\LC_MESSAGES" >> $NSIS
  echo "  File /oname=wormux.mo \"$WIN_WORMUXDIR\\po\\$lg.gmo\"" >> $NSIS
done

## Data - I love this syntax
cat >> $NSIS <<EOF
  ; Data
  SetOutPath \$INSTDIR
  File /r /x .svn /x Makefile* /x Makefile.* "${WIN_WORMUXDIR}\\data"
EOF

## License
cat >> $NSIS <<EOF
  ; Licenses
  File /r /x .svn "${WIN_WORMUXDIR}\\doc\\license"
EOF

# End
cat >> $NSIS <<EOF

  ; Write the installation path into the registry
  WriteRegStr HKLM ${HKLM_PATH} "pth" "\$INSTDIR"
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Wormux" "DisplayName" "Wormux (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Wormux" "UninstallString" '"\$INSTDIR\uninstall.exe"'
  WriteUninstaller "uninstall.exe"
  ; Shortcuts
  SetShellVarContext all
  CreateDirectory "\$SMPROGRAMS\Wormux"
  CreateShortCut  "\$SMPROGRAMS\Wormux\Wormux.lnk" "\$INSTDIR\Wormux.exe" "" "\$INSTDIR\Wormux.exe" 0
  CreateShortCut  "\$SMPROGRAMS\Wormux\Uninstall.lnk" "\$INSTDIR\uninstall.exe" "" "\$INSTDIR\uninstall.exe" 0
  ;Write language to the registry (for the uninstaller)
  WriteRegStr HKLM ${HKLM_PATH} "Installer Language" \$LANGUAGE

  GetFullPathName /SHORT \$SHORTINSTDIR \$INSTDIR
SectionEnd

;--------------------------------
;Descriptions

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT \${Sec_Wormux} \$(DESC_Wormux)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"
  ; remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Wormux"
  DeleteRegKey HKLM ${HKLM_PATH}
  DeleteRegKey HKCU ${HKLM_PATH}
  ; remove shortcuts, if any.
  SetShellVarContext all
  Delete "\$SMPROGRAMS\Wormux\*.*"
  RMDir  "\$SMPROGRAMS\Wormux"

  ; remove files
  RMDir /r "\$INSTDIR"

  ;  !insertmacro MUI_UNFINISHHEADER
SectionEnd

Function .onInit

  ;Language selection

  !insertmacro MUI_LANGDLL_DISPLAY

  SectionSetFlags \${Sec_Wormux} 17

FunctionEnd

Function un.onInit
  !insertmacro MUI_UNGETLANGUAGE
FunctionEnd
EOF

## Compile installer
if ! $MAKE_NSIS $NSIS; then
  echo "makensis failed, aborting..."
  exit 1
fi

## Move executable to current directory and remove temporary directory
rm -rf $NSIS $DEST
