;;
;; Windows Wormux NSIS installer language macros
;; Modification of the Gaim version
;;

!macro WORMUX_MACRO_DEFAULT_STRING LABEL VALUE
  !ifndef "${LABEL}"
    !define "${LABEL}" "${VALUE}"
    !ifdef INSERT_DEFAULT
      !warning "${LANG} lang file missing ${LABEL}, using default..."
    !endif
  !endif
!macroend

!macro WORMUX_MACRO_LANGSTRING_INSERT LABEL LANG
  LangString "${LABEL}" "${LANG_${LANG}}" "${${LABEL}}"
  !undef "${LABEL}"
!macroend

!macro WORMUX_MACRO_LANGUAGEFILE_BEGIN LANG
  !define CUR_LANG "${LANG}"
!macroend

!macro WORMUX_MACRO_LANGUAGEFILE_END
  ;!define INSERT_DEFAULT
  ;!include "${WORMUX_DEFAULT_LANGFILE}"
  ;!undef INSERT_DEFAULT

  ; WORMUX Language file
  ; String labels should match those from the default language file.

  ; License Page
  !insertmacro WORMUX_MACRO_LANGSTRING_INSERT WORMUX_BUTTON         ${CUR_LANG}
  !insertmacro WORMUX_MACRO_LANGSTRING_INSERT WORMUX_BOTTOM_TEXT    ${CUR_LANG}
  !insertmacro WORMUX_MACRO_LANGSTRING_INSERT WORMUX_TITLE          ${CUR_LANG}
  !insertmacro WORMUX_MACRO_LANGSTRING_INSERT WORMUX_DESC           ${CUR_LANG}

  ; Components Page
  !insertmacro WORMUX_MACRO_LANGSTRING_INSERT WORMUX_SHORTCUTS_DESC     ${CUR_LANG}
  !insertmacro WORMUX_MACRO_LANGSTRING_INSERT WORMUX_DESKTOP_SC_DESC    ${CUR_LANG}
  !insertmacro WORMUX_MACRO_LANGSTRING_INSERT WORMUX_STARTM_SC_DESC     ${CUR_LANG}
  !insertmacro WORMUX_MACRO_LANGSTRING_INSERT WORMUX_UNINST_SC_DESC     ${CUR_LANG}
  !insertmacro WORMUX_MACRO_LANGSTRING_INSERT WORMUX_SHORCUTS_TITLE     ${CUR_LANG}

  ; Installer Finish Page
  !insertmacro WORMUX_MACRO_LANGSTRING_INSERT WORMUX_VISIT              ${CUR_LANG}

  !undef CUR_LANG
!macroend

!macro WORMUX_MACRO_INCLUDE_LANGFILE LANG FILE
  !insertmacro WORMUX_MACRO_LANGUAGEFILE_BEGIN "${LANG}"
  !include "${FILE}"
  !insertmacro WORMUX_MACRO_LANGUAGEFILE_END
!macroend
