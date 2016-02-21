;;
;; Windows Warmux NSIS installer language macros
;; Modification of the Gaim version
;;

!macro WARMUX_MACRO_DEFAULT_STRING LABEL VALUE
  !ifndef "${LABEL}"
    !define "${LABEL}" "${VALUE}"
    !ifdef INSERT_DEFAULT
      !warning "${LANG} lang file missing ${LABEL}, using default..."
    !endif
  !endif
!macroend

!macro WARMUX_MACRO_LANGSTRING_INSERT LABEL LANG
  LangString "${LABEL}" "${LANG_${LANG}}" "${${LABEL}}"
  !undef "${LABEL}"
!macroend

!macro WARMUX_MACRO_LANGUAGEFILE_BEGIN LANG
  !define CUR_LANG "${LANG}"
!macroend

!macro WARMUX_MACRO_LANGUAGEFILE_END
  ;!define INSERT_DEFAULT
  ;!include "${WARMUX_DEFAULT_LANGFILE}"
  ;!undef INSERT_DEFAULT

  ; WARMUX Language file
  ; String labels should match those from the default language file.

  ; License Page
  !insertmacro WARMUX_MACRO_LANGSTRING_INSERT WARMUX_BUTTON             ${CUR_LANG}
  !insertmacro WARMUX_MACRO_LANGSTRING_INSERT WARMUX_BOTTOM_TEXT        ${CUR_LANG}
  !insertmacro WARMUX_MACRO_LANGSTRING_INSERT WARMUX_TITLE              ${CUR_LANG}
  !insertmacro WARMUX_MACRO_LANGSTRING_INSERT WARMUX_DESC               ${CUR_LANG}

  ; Components Page
  !insertmacro WARMUX_MACRO_LANGSTRING_INSERT WARMUX_SHORTCUTS_DESC     ${CUR_LANG}
  !insertmacro WARMUX_MACRO_LANGSTRING_INSERT WARMUX_DESKTOP_SC_DESC    ${CUR_LANG}
  !insertmacro WARMUX_MACRO_LANGSTRING_INSERT WARMUX_STARTM_SC_DESC     ${CUR_LANG}
  !insertmacro WARMUX_MACRO_LANGSTRING_INSERT WARMUX_UNINST_SC_DESC     ${CUR_LANG}
  !insertmacro WARMUX_MACRO_LANGSTRING_INSERT WARMUX_RESET_SC_DESC      ${CUR_LANG}
  !insertmacro WARMUX_MACRO_LANGSTRING_INSERT WARMUX_SHORCUTS_TITLE     ${CUR_LANG}
  !insertmacro WARMUX_MACRO_LANGSTRING_INSERT WARMUX_CONFIG_SC_DESC     ${CUR_LANG}
  !insertmacro WARMUX_MACRO_LANGSTRING_INSERT WARMUX_PDF_SC_DESC        ${CUR_LANG}

  ; Misc
  !insertmacro WARMUX_MACRO_LANGSTRING_INSERT WARMUX_PROMPT_NO_RIGHTS   ${CUR_LANG}
  !insertmacro WARMUX_MACRO_LANGSTRING_INSERT WARMUX_PROMPT_CONTINUE    ${CUR_LANG}

  ; Force uninstall
  !insertmacro WARMUX_MACRO_LANGSTRING_INSERT WARMUX_REMOVE_TITLE       ${CUR_LANG}

  ; Installer Finish Page
  !insertmacro WARMUX_MACRO_LANGSTRING_INSERT WARMUX_VISIT              ${CUR_LANG}

  !undef CUR_LANG
!macroend

!macro WARMUX_MACRO_INCLUDE_LANGFILE LANG FILE
  !insertmacro WARMUX_MACRO_LANGUAGEFILE_BEGIN "${LANG}"
  !include "${FILE}"
  !insertmacro WARMUX_MACRO_LANGUAGEFILE_END
!macroend
