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
WORMUX_VERSION=r$(LC_ALL=C svn info 2> /dev/null | grep Revision | cut -d' ' -f2)

#0.8.1

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
  i586-mingw32msvc-strip ../../src/wormux.exe
else
  PKG_CONFIG=pkg-config
  SDL_CONFIG=sdl-config
  WIN_WORMUXDIR=$(cd "$WORMUXDIR" && cmd /c cd)
  LOCAL_PATH=$(cmd /c cd)
  SEP="\\"
  strip ../../src/wormux.exe
fi

mkdir -p $DEST

function pkg_path
{
  if [ -z "$1" ]; then return ""; fi
  var=$($PKG_CONFIG --variable=prefix $1 2>/dev/null || exit 1)
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
!include "MUI2.nsh"
!include "Sections.nsh"
!include "LogicLib.nsh"
!include "FileFunc.nsh"
!insertmacro GetParent


Name "Wormux"
!define WORMUX_VERSION  "${WORMUX_VERSION}"
;Version resource
VIProductVersion        "0.8.1.0"
VIAddVersionKey         "FileDescription"       "Wormux Installer"
VIAddVersionKey         "ProductName"           "Wormux"
VIAddVersionKey         "FileVersion"           "${WORMUX_VERSION}"
VIAddVersionKey         "ProductVersion"        "${WORMUX_VERSION}"
VIAddVersionKey         "LegalCopyright"        "(C) 2001-2008 The Wormux Project"

;General
OutFile "${LOCAL_PATH}\Wormux-Setup-${WORMUX_VERSION}.exe"
SetCompressor ${COMPRESSION}
ShowInstDetails show
ShowUninstDetails show
SetDateSave on
RequestExecutionLevel highest

!define WORMUX_REG_KEY          "${HKLM_PATH}"
!define WORMUX_UNINSTALL_KEY    "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Wormux"
!define HKLM_APP_PATHS_KEY      "SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\wormux.exe"
!define STARTUP_RUN_KEY         "SOFTWARE\Microsoft\Windows\CurrentVersion\Run"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Modern UI Configuration ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  !define MUI_ICON                          "${LOCAL_PATH}${SEP}install.ico"
  !define MUI_UNICON                        "${LOCAL_PATH}${SEP}uninstall.ico"
  ; Alter License section
  !define MUI_LICENSEPAGE_BUTTON            \$(WORMUX_BUTTON)
  !define MUI_LICENSEPAGE_TEXT_BOTTOM       \$(WORMUX_BOTTOM_TEXT)
  ; Language
  !define MUI_LANGDLL_ALWAYSSHOW
  !define MUI_LANGDLL_REGISTRY_ROOT         "HKCU"
  !define MUI_LANGDLL_REGISTRY_KEY          "\${WORMUX_REG_KEY}"
  !define MUI_LANGDLL_REGISTRY_VALUENAME    "Installer Language"
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

  !insertmacro MUI_LANGUAGE "French"
  LicenseLangString WormuxLicense "French" "${WIN_WORMUXDIR}\doc\license\COPYING.fr.txt"

  !insertmacro MUI_LANGUAGE "Greek"
  LicenseLangString WormuxLicense "Greek" "${WIN_WORMUXDIR}\doc\license\COPYING.el.txt"

  !insertmacro MUI_LANGUAGE "Polish"
  LicenseLangString WormuxLicense "Polish" "${WIN_WORMUXDIR}\doc\license\COPYING.pl.txt"

  !insertmacro MUI_LANGUAGE "German"
  LicenseLangString WormuxLicense "German" "${WIN_WORMUXDIR}\doc\license\COPYING.de.txt"
  LangString TITLE_Wormux "German" "Wormux"
  LangString DESC_Wormux  "German" "Wormux ${WORMUX_VERSION}"

  !insertmacro MUI_LANGUAGE "Spanish"
  LicenseLangString WormuxLicense "Spanish" "${WIN_WORMUXDIR}\doc\license\COPYING.es.txt"
  LangString TITLE_Wormux "Spanish" "Wormux"
  LangString DESC_Wormux  "Spanish" "Wormux ${WORMUX_VERSION}"

  !insertmacro MUI_LANGUAGE "Dutch"
  LicenseLangString WormuxLicense "Dutch" "${WIN_WORMUXDIR}\doc\license\COPYING.nl.txt"
  LangString TITLE_Wormux "Dutch" "Wormux"
  LangString DESC_Wormux  "Dutch" "Wormux ${WORMUX_VERSION}"

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
  !insertmacro WORMUX_MACRO_INCLUDE_LANGFILE "POLISH"   "${LOCAL_PATH}\Polish.nsh"
  !insertmacro WORMUX_MACRO_INCLUDE_LANGFILE "GREEK"    "${LOCAL_PATH}\Greek.nsh"

  ;--------------------------------
  ;Reserve Files
  ;If you are using solid compression, files that are required before
  ;the actual installation should be stored first in the data block,
  ;because this will make your installer start faster.

  !insertmacro MUI_RESERVEFILE_LANGDLL

  ;--------------------------------
  ;Folder-selection page
  InstallDir "\$PROGRAMFILES\Wormux"
  ; Registry key to check for directory (so if you install again, it will 
  ; overwrite the old one automatically)
  InstallDirRegKey HKLM "\${WORMUX_REG_KEY}" "Path"
  AutoCloseWindow false

;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Start Install Sections ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;-----------------------------------------
;Create folder only if it doesnt exist yet
!macro CreateDirectoryOnce FOLDER
  IfFileExists "\${FOLDER}\\*.*" +1
    CreateDirectory "\${FOLDER}"
!macroend

;--------------------------------
;Check (un)install rights
!macro CheckUserInstallRightsMacro UN
Function \${UN}CheckUserInstallRights
  Push \$0
  Push \$1
  ClearErrors
  UserInfo::GetName
  IfErrors Win9x
  Pop \$0
  UserInfo::GetAccountType
  Pop \$1

  StrCmp \$1 "Admin" 0 +3
    StrCpy \$1 "HKLM"
    Goto done
  StrCmp \$1 "Power" 0 +3
    StrCpy \$1 "HKLM"
    Goto done
  StrCmp \$1 "User" 0 +3
    StrCpy \$1 "HKCU"
    Goto done
  StrCmp \$1 "Guest" 0 +3
    StrCpy \$1 "NONE"
    Goto done
  ; Unknown error
  StrCpy \$1 "NONE"
  Goto done

  Win9x:
    StrCpy \$1 "HKLM"

  done:
    Exch \$1
    Exch
    Pop \$0
FunctionEnd
!macroend
!insertmacro CheckUserInstallRightsMacro ""
!insertmacro CheckUserInstallRightsMacro "un."

;--------------------------------
; Uninstall any old version of Wormux

; Section hidden because automatically selected by the installer
Section \$(WORMUX_REMOVE_TITLE) SecUninstallOldWormux
  ; Check install rights..
  StrCpy \$R3 \${WORMUX_REG_KEY}
  StrCpy \$R4 \${wORMUX_UNINSTALL_KEY}
  StrCpy \$R5 "uninstall.exe"
  Call CheckUserInstallRights
  Pop \$R0
  ; "NONE" case already handled at start
  StrCmp \$R0 "HKCU" _hkcu
    ReadRegStr \$R1 HKLM \$R3 ""
    ReadRegStr \$R2 HKLM "\$R4" "UninstallString"
    Goto try_uninstall

  _hkcu:
    ReadRegStr \$R1 HKCU \$R3 ""
    ReadRegStr \$R2 HKCU "\$R4" "UninstallString"

  ; If a previous version exists, remove it
  try_uninstall:
    ; If first string is unavailable, Wormux was probably not installed
    StrCmp \$R1 "" done
      ; Check if we have uninstall string..
      IfFileExists \$R2 0 no_file
        ; Have uninstall string, go ahead and uninstall.
        SetOverwrite on
        ; Need to copy uninstaller outside of the install dir
        ClearErrors
        CopyFiles /SILENT \$R2 "\$TEMP\\\$R5"
        SetOverwrite off
        IfErrors uninstall_problem
          ; Ready to uninstall..
          ClearErrors
          ExecWait '"\$TEMP\\\$R5" /S _?=\$R1'
          IfErrors exec_error
            Delete "\$TEMP\\\$R5"
            Goto done

          exec_error:
            Delete "\$TEMP\\\$R5"
            Goto uninstall_problem

  no_file:
    MessageBox MB_OK "No uninstaller exe found" /SD IDOK IDOK done

  uninstall_problem:
    ; We cant uninstall. Either the user must manually uninstall or
    ; we ignore and reinstall over it.
    MessageBox MB_OKCANCEL \$(WORMUX_PROMPT_CONTINUE) /SD IDOK IDCANCEL done
    Quit

  done:
SectionEnd

;--------------------------------
; Installer Sections

Section \$(TITLE_Wormux) Sec_Wormux
  ; Create install and config folders
  CreateDirectory "\$INSTDIR"
  CreateDirectory "\$APPDATA\\Wormux"
  ; Set output path to the installation directory.
  SetOutPath "\$INSTDIR"
  File "${WIN_WORMUXDIR}\src\wormux.ico"
  ; Executing in tmpdir, looking for file in folder below
  File "${LOCAL_PATH}\uninstall.ico"
  File "${WIN_WORMUXDIR}\src\wormux.exe"
  WriteUninstaller "uninstall.exe"
  ; data
  File /r /x .svn /x Makefile* /x Makefile.* "${WIN_WORMUXDIR}\\data"
  ; Licenses
  File /r /x .svn "${WIN_WORMUXDIR}\\doc\\license"
  ; Howto-play PDFs
  File /r /x .svn "${WIN_WORMUXDIR}\\doc\\howto_play"
EOF

# Glib (gobject, gthread, glib & gmodule)
XML_PATH=$(pkg_path libxml-2.0)

# Other libs
cp "$XML_PATH/bin/libxml2.dll" "$DEST"

# Files that must not be stripped (all MSVC, mainly SDL and vorbis)
# Make sure freetype, libpng and jpeg dll are matching your libs.
SDL_PATH=$($SDL_CONFIG --prefix)
cp "$SDL_PATH/bin/"SDL{,_mixer,_ttf,_image,_net}.dll       \
   "$XML_PATH/bin/"{intl,iconv,zlib1,jpeg62,freetype6}.dll \
   "$XML_PATH/bin/"lib{png12,tiff3}.dll                    \
   "$SDL_PATH/bin/"lib{ogg-0,vorbis-0,vorbisfile-3,curl-4}.dll "$DEST"

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
  echo "  File /oname=wormux.mo \"${WIN_WORMUXDIR}${SEP}po${SEP}$lg.gmo\"" >> $NSIS
done

## Various files
cat >> $NSIS <<EOF
  Call CheckUserInstallRights
  Pop \$R0
  ; "NONE" case already handled at start
  StrCmp \$R0 "HKCU" _hkcu
    WriteRegStr HKLM "\${HKLM_APP_PATHS_KEY}" "" "\$INSTDIR\wormux.exe"
    WriteRegStr HKLM "\${WORMUX_REG_KEY}" "" "\$INSTDIR"
    WriteRegStr HKLM "\${WORMUX_REG_KEY}" "Version" "${WORMUX_VERSION}"
    WriteRegStr HKLM "\${WORMUX_UNINSTALL_KEY}" "DisplayName" "Wormux"
    WriteRegStr HKLM "\${WORMUX_UNINSTALL_KEY}" "DisplayVersion" "${WORMUX_VERSION}"
    WriteRegStr HKLM "\${WORMUX_UNINSTALL_KEY}" "UninstallString" "\$INSTDIR\uninstall.exe"
    ;Write language to the registry (for the uninstaller)
    WriteRegStr HKLM "\${WORMUX_REG_KEY}" "Installer Language" \$LANGUAGE
    ; Sets scope of the desktop and Start Menu entries for all users.
    SetShellVarContext "all"
    Goto _next

  _hkcu:
    WriteRegStr HKCU "\${WORMUX_REG_KEY}" "" "\$INSTDIR"
    WriteRegStr HKCU "\${WORMUX_REG_KEY}" "Version" "${WORMUX_VERSION}"
    WriteRegStr HKCU "\${WORMUX_UNINSTALL_KEY}" "DisplayName" "Pidgin"
    WriteRegStr HKCU "\${WORMUX_UNINSTALL_KEY}" "DisplayVersion" "${WORMUX_VERSION}"
    WriteRegStr HKCU "\${WORMUX_UNINSTALL_KEY}" "UninstallString" "\$INSTDIR\\uninstall.exe"
    ;Write language to the registry (for the uninstaller)
    WriteRegStr HKCU "\${WORMUX_REG_KEY}" "Installer Language" \$LANGUAGE
    ;SetShellVarContext "current"

  _next:
SectionEnd ; Installer section

;--------------------------------
;Shortcuts
SectionGroup /e \$(WORMUX_SHORCUTS_TITLE) Sec_Shortcuts
  ; Shortcut to config folder
  Section \$(WORMUX_CONFIG_SC_DESC) Sec_ConfigShortcut
    SetOutPath "\$INSTDIR"
    SetOverwrite on
    !insertmacro CreateDirectoryOnce "\$SMPROGRAMS\\Wormux"
    CreateShortcut  "\$SMPROGRAMS\\Wormux\\Config.lnk" "\$APPDATA\\Wormux" "" "" 0
    SetOverwrite off
  SectionEnd

  ; Group of shortcuts for pdfs
  SectionGroup /e \$(WORMUX_PDF_SC_DESC) Sec_PdfShortcut
EOF

## PDF stuff
lang=$(ls ../../doc/howto_play/*.pdf)
lang=${lang//.pdf}
lang=${lang//..\/..\/doc\/howto_play\/}
for f in $lang; do
  cat >> $NSIS <<EOF
    ; PDF for language $f
    Section $f
      SetOutPath "\$INSTDIR"
      SetOverwrite on
      File "/oname=help-$f.pdf" "${WIN_WORMUXDIR}${SEP}doc${SEP}howto_play${SEP}${f}.pdf"
      !insertmacro CreateDirectoryOnce "\$SMPROGRAMS\\Wormux"
      CreateShortcut  "\$SMPROGRAMS\\Wormux\\howto-$f.lnk" "\$INSTDIR\\help-$f.pdf" "" "" 0
      SetOverwrite off
    SectionEnd
EOF
done

cat >> $NSIS <<EOF
  SectionGroupEnd

  ; Desktop shortcut
  Section /o \$(WORMUX_DESKTOP_SC_DESC) Sec_DesktopShortcut
    SetOverwrite on
    CreateShortCut "\$DESKTOP\\WORMUX.lnk" "\$INSTDIR\\wormux.exe" \
      "" "\$INSTDIR\\wormux.exe" 0
    SetOverwrite off
  SectionEnd

  ; Wormux shortcut in start menu
  Section \$(WORMUX_STARTM_SC_DESC) Sec_StartMenuShortcut
    SetOverwrite on
    !insertmacro CreateDirectoryOnce "\$SMPROGRAMS\\Wormux"
    CreateShortCut "\$SMPROGRAMS\\Wormux\\Wormux.lnk" \
      "\$INSTDIR\\wormux.exe" "" "\$INSTDIR\\wormux.exe" 0
    SetOverwrite off
  SectionEnd

  ; Wormux uninstall shortcut in start menu
  ; Might be forced if user has no install rights, because it would be complex otherwise:
  ; - No uninstall available in Windows "Program uninstall"
  ; - Folder lost in APPDATA, which can be hidden, etc
  Section \$(WORMUX_UNINST_SC_DESC) Sec_UninstallShortCut
    SetOverwrite on
    !insertmacro CreateDirectoryOnce "\$SMPROGRAMS\\Wormux"
    CreateShortCut  "\$SMPROGRAMS\\Wormux\\Uninstall.lnk" \
         "\$INSTDIR\\uninstall.exe" "" "\$INSTDIR\\uninstall.exe" 0
    SetOverwrite off
  SectionEnd
SectionGroupEnd

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
  ; Set install path according to user rights
  Call un.CheckUserInstallRights
  Pop \$R0
  StrCmp \$R0 "HKLM" _hklm

  ; Also used as fallback by HKLM case
  _hkcu:
    ReadRegStr \$R0 HKCU "\${WORMUX_REG_KEY}" ""
    StrCmp \$R0 "\$INSTDIR" 0 _next
      ; HKCU install path matches our INSTDIR so uninstall
      DeleteRegKey HKCU "\${WORMUX_REG_KEY}"
      DeleteRegKey HKCU "\${WORMUX_UNINSTALL_KEY}"
      Goto _next

  _hklm:
    ReadRegStr \$R0 HKLM "\${WORMUX_REG_KEY}" ""
    StrCmp \$R0 \$INSTDIR 0 _hkcu
      ; HKLM install path matches our INSTDIR so uninstall
      DeleteRegKey HKLM "\${HKLM_APP_PATHS_KEY}"
      DeleteRegKey HKLM "\${WORMUX_REG_KEY}"
      DeleteRegKey HKLM "\${WORMUX_UNINSTALL_KEY}"
      SetShellVarContext all

  _next:
    ; Remove Language preference info
    DeleteRegValue HKCU "\${WORMUX_REG_KEY}" "Installer Language"
    ; remove shortcuts, if any.
    Delete "\$SMPROGRAMS\\Wormux\\*.*"
    RMDir  "\$SMPROGRAMS\\Wormux"
    Delete "\$SMPROGRAMS\\Wormux.lnk"
    Delete "\$DESKTOP\\Wormux.lnk"
    ; remove files
    RMDir /r "\$INSTDIR"
SectionEnd

Function .onInit
  ;Language selection
  !insertmacro MUI_LANGDLL_DISPLAY

  IntOp \$R0 \${SF_RO} | \${SF_SELECTED}
  SectionSetFlags \${Sec_Wormux} \$R0
  SectionSetFlags \${Sec_ConfigShortcut} \$R0
  SectionSetFlags \${SecUninstallOldWormux} \$R0

  ; Set install path according to user rights
  Call CheckUserInstallRights
  Pop \$R0
  StrCmp \$R0 "NONE" _none
  StrCmp \$R0 "HKLM" 0 _hkcu
    StrCpy \$INSTDIR "\$PROGRAMFILES\\Wormux"
    Goto _done

  _hkcu:
    Push \$SMPROGRAMS
    \${GetParent} \$SMPROGRAMS \$R2
    \${GetParent} \$R2 \$R2
    StrCpy \$INSTDIR "\$R2\\Wormux"
    ; In this case uninstall shortcut *must* be available because
    ; the alternative are complex for the user
    IntOp \$R0 \${SF_RO} | \${SF_SELECTED}
    SectionSetFlags \${Sec_UninstallShortCut} \$R0
    Goto _done

  _none:
   ; Not going to bother
   MessageBox MB_OK \$(WORMUX_PROMPT_NO_RIGHTS) /SD IDOK
   Quit

  _done:
FunctionEnd

; INSTDIR will be determined by reading a registry key
Function un.onInit
  !insertmacro MUI_UNGETLANGUAGE
  ; Set install path according to user rights
  Call un.CheckUserInstallRights
  Pop \$R0
  StrCmp \$R0 "NONE" _none
    Goto _end

  _none:
   ; Not going to bother
   MessageBox MB_OK \$(WORMUX_PROMPT_NO_RIGHTS) /SD IDOK
   Quit

  _end:
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
