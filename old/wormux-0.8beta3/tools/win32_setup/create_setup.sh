#!/bin/bash

## Programs
MAKE_NSIS=makensis

# Path to which all others are relative
WORMUXDIR="../.."

# Root path for other dlls (oggvorbis, curse, ...)
BINDIR=/mingw

# Windows registry software path
HKLM_PATH="SOFTWARE\Games\Wormux"

# Version number in installer
WORMUX_VERSION=`date +"%Y%m%d"`

# Set compression and in/out
COMPRESSION="/solid lzma"
DEST=tmp-wormux-win32
NSIS="$DEST/wormux.nsi"

# Clean prior stuff
rm -rf $NSIS $DEST

# Prepare stuff
if [ "$OSTYPE" == "linux-gnu" ]; then
  PKG_CONFIG=i586-mingw32msvc-pkg-config
  SDL_CONFIG=i586-mingw32msvc-sdl-config
  WIN_WORMUXDIR="$PWD/../.."
  LOCAL_PATH="$PWD"
  SEP="/"
else
  PKG_CONFIG=pkg-config
  SDL_CONFIG=sdl-config
  WIN_WORMUXDIR=$(cd "$WORMUXDIR" && cmd /c cd)
  LOCAL_PATH=$(cmd /c cd)
  SEP="\\"
fi

mkdir -p $DEST

function pkg_path
{
  if [ -z "$1" ]; then return ""; fi
  var=$($PKG_CONFIG --variable=prefix $1 2>/dev/null)
  if [ -z "$var" ]; then echo "Couldn't find $1, aborting..." 1>&2; exit 1; fi
  echo "Found $1 in $var" 1>&2
  echo "$var"
}

if ! pkg-config --help 2>/dev/null 1>&2 ; then
  echo "pkg-config not found, aborting..."
  exit 1
fi

# Create head
cat > $NSIS <<EOF
;based on MUI Welcome/Finish Page Example Script written by Joost Verburg
!include "MUI.nsh"
!include "Sections.nsh"
!include "LogicLib.nsh"

Name "Wormux"
!define WORMUX_VERSION  "${WORMUX_VERSION}"
;Version resource
VIProductVersion        "0.8.0.2"
VIAddVersionKey         "FileDescription"       "Wormux Installer"
VIAddVersionKey         "ProductName"           "Wormux"
VIAddVersionKey         "FileVersion"           "${WORMUX_VERSION}"
VIAddVersionKey         "ProductVersion"        "${WORMUX_VERSION}"
VIAddVersionKey         "LegalCopyright"        "(C) 2001-2007 The Wormux Project"

;General
OutFile "${LOCAL_PATH}\Wormux-Setup-${WORMUX_VERSION}.exe"
SetCompressor ${COMPRESSION}

;--------------------------------
;Modern UI Configuration
  !define MUI_ICON                          "${LOCAL_PATH}${SEP}install.ico"
  !define MUI_UNICON                        "${LOCAL_PATH}${SEP}uninstall.ico"
  ; Alter License section
  !define MUI_LICENSEPAGE_BUTTON            \$(WORMUX_BUTTON)
  !define MUI_LICENSEPAGE_TEXT_BOTTOM       \$(WORMUX_BOTTOM_TEXT)
  ; Language
  !define MUI_LANGDLL_ALWAYSSHOW
  !define MUI_LANGDLL_REGISTRY_ROOT         "HKLM"
  !define MUI_LANGDLL_REGISTRY_KEY"         ${HKLM_PATH}"
  !define MUI_LANGDLL_REGISTRY_VALUENAME    "lang"
  ; Misc stuff
  !define MUI_COMPONENTSPAGE_SMALLDESC
  !define MUI_ABORTWARNING
  ; Do not close dialogs, allow to check installation result
  !define MUI_FINISHPAGE_NOAUTOCLOSE
  !define MUI_UNFINISHPAGE_NOAUTOCLOSE
  ;Finish Page config
  !define MUI_FINISHPAGE_RUN                "\$INSTDIR\wormux.exe"
  !define MUI_FINISHPAGE_RUN_NOTCHECKED
  !define MUI_FINISHPAGE_LINK               \$(WORMUX_VISIT)
  !define MUI_FINISHPAGE_LINK_LOCATION      "http://www.wormux.org"

;--------------------------------
;Pages
  ; Install
  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE \$(WormuxLicense)
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH
  ; Uninstall
  !insertmacro MUI_UNPAGE_WELCOME
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH

;--------------------------------
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

  ;--------------------------------
  ;Installer translations
  !define WORMUX_DEFAULT_LANGFILE "${LOCAL_PATH}\English.nsh"
  !include "${LOCAL_PATH}\langmacros.nsh"
  !insertmacro WORMUX_MACRO_INCLUDE_LANGFILE "ENGLISH"  "${LOCAL_PATH}\English.nsh"
  !insertmacro WORMUX_MACRO_INCLUDE_LANGFILE "FRENCH"   "${LOCAL_PATH}\French.nsh"

;--------------------------------
;Folder-selection page
InstallDir "\$PROGRAMFILES\Wormux"
; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM ${HKLM_PATH} "Path"
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
cp "$GLIB_PATH"/bin/libg{object,thread,module,lib}-2.0-0.dll "$DEST"

# Other libs
cp "$(pkg_path sigc++-2.0)/bin/libsigc"*.dll $DEST
cp "$(pkg_path libxml-2.0)/bin/libxml2"*.dll $DEST
cp "$(pkg_path libxml++-2.6)/bin/libxml++"*.dll $DEST
cp "$(pkg_path glibmm-2.4)/bin/libglibmm"*.dll $DEST

# Files that must not be stripped (all MSVC, mainly SDL and vorbis)
SDL_PATH=$($SDL_CONFIG --prefix)
cp "$SDL_PATH/bin/"SDL{,_mixer,_ttf,_image,_net}.dll    \
   "$GLIB_PATH/bin/"{intl,iconv,zlib1,libpng12}.dll     \
   "$SDL_PATH/bin/"lib{curl-4,freetype-6,png12-0}.dll   \
   "$SDL_PATH/bin/jpeg.dll"                             \
   "$SDL_PATH/bin/"lib{ogg-0,vorbis-0,vorbisfile-3}.dll "$DEST"

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
  echo "  File /oname=wormux.mo \"$WIN_WORMUXDIR${SEP}po${SEP}$lg.gmo\"" >> $NSIS
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
  WriteRegStr HKLM ${HKLM_PATH} "Path" "\$INSTDIR"
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
;Shortcuts
SubSection /e \$(WORMUX_SHORCUTS_TITLE) Sec_Shortcuts
  Section /o \$(WORMUX_DESKTOP_SC_DESC) Sec_DesktopShortcut
    SetOverwrite on
    CreateShortCut "\$DESKTOP\WORMUX.lnk" "\$INSTDIR\wormux.exe" \
      "" "\$INSTDIR\wormux.exe" 0
    SetOverwrite off
  SectionEnd
  Section \$(WORMUX_STARTM_SC_DESC) Sec_StartMenuShortcut
    SetOverwrite on
    CreateDirectory "\$SMPROGRAMS\Wormux"
    CreateShortCut "\$SMPROGRAMS\Wormux\Wormux.lnk" \
      "\$INSTDIR\wormux.exe" "" "\$INSTDIR\wormux.exe" 0
    SetOverwrite off
  SectionEnd
  Section \$(WORMUX_UNINST_SC_DESC) Sec_UninstallShortCut
    SetOverwrite on
    CreateShortCut  "\$SMPROGRAMS\Wormux\Uninstall.lnk" \
      "\$INSTDIR\uninstall.exe" "" "\$INSTDIR\uninstall.exe" 0
    SetOverwrite off
  SectionEnd
SubSectionEnd

;--------------------------------
;Descriptions

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT     \${Sec_Wormux}             \$(WORMUX_DESC)
  !insertmacro MUI_DESCRIPTION_TEXT     \${Sec_Shortcuts}          \$(WORMUX_SHORTCUTS_DESC)
    !insertmacro MUI_DESCRIPTION_TEXT   \${Sec_DesktopShortcut}    \$(WORMUX_DESKTOP_SC_DESC)
    !insertmacro MUI_DESCRIPTION_TEXT   \${Sec_StartMenuShortcut}  \$(WORMUX_STARTM_SC_DESC)
    !insertmacro MUI_DESCRIPTION_TEXT   \${Sec_UninstallShortcut}  \$(WORMUX_UNINST_SC_DESC)
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
  Delete "\$SMPROGRAMS\Wormux.lnk"
  Delete "\$DESKTOP\Wormux.lnk"
  ; remove files
  RMDir /r "\$INSTDIR"
SectionEnd

Function .onInit
  ;Language selection
  !insertmacro MUI_LANGDLL_DISPLAY
  IntOp \$R0 \${SF_RO} | \${SF_SELECTED}
  SectionSetFlags \${Sec_Wormux} \$R0
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
mv $NSIS .
rm -rf $DEST
