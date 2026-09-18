;
; avanor.nsi - the Windows installer for Avanor
;
; Built by the Makefile's `installer` target, which generates the file
; lists this includes. Do not run makensis on it by hand without them.
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;

; Unicode installer
Unicode True

; Modern UI
!include "MUI2.nsh"

!ifndef VERSION
  !error "VERSION has not been defined"
!endif

Name "Avanor ${VERSION}"
OutFile "avanor-${VERSION}.exe"

SetCompressor /SOLID lzma

InstallDir $PROGRAMFILES64\Avanor
InstallDirRegKey HKLM "Software\Avanor" "Install_Dir"

RequestExecutionLevel admin

;--------------------------------
; Interface

!define MUI_ICON "resources\avanor.ico"
!define MUI_UNICON "resources\avanor.ico"

;--------------------------------
; Pages

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "COPYING"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

;--------------------------------
; Version information on the installer itself

VIProductVersion "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}.0"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "Avanor"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "Vadim Gaidukevich"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "(C) 2000-2006 Vadim Gaidukevich, GPLv2+"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "Avanor Installation Program"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${VERSION}"

;--------------------------------
; What gets installed

Section "Avanor (required)"

  SectionIn RO

  ; The executable, its libraries and the documents beside them.
  SetOutPath $INSTDIR
  !include "mainfiles.nsh"

  ; The world is script, read at startup from ./world, and the manual is
  ; HTML read from ./manual - neither is optional, and both keep their
  ; directory layout.
  SetOutPath $INSTDIR
  !include "datafiles.nsh"

  WriteRegStr HKLM SOFTWARE\Avanor "Install_Dir" "$INSTDIR"

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Avanor" "DisplayIcon" '"$INSTDIR\avanor.exe"'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Avanor" "DisplayName" "Avanor ${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Avanor" "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Avanor" "Publisher" "Vadim Gaidukevich"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Avanor" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Avanor" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Avanor" "NoRepair" 1
  WriteUninstaller "uninstall.exe"

SectionEnd

Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\Avanor"
  CreateShortCut "$SMPROGRAMS\Avanor\Avanor.lnk" "$INSTDIR\avanor.exe" "" "$INSTDIR\avanor.exe" 0
  CreateShortCut "$SMPROGRAMS\Avanor\Manual.lnk" "$INSTDIR\manual\index.html"
  CreateShortCut "$SMPROGRAMS\Avanor\Uninstall.lnk" "$INSTDIR\uninstall.exe"

SectionEnd

;--------------------------------
; Uninstaller

Section "Uninstall"

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Avanor"
  DeleteRegKey HKLM SOFTWARE\Avanor

  ; The world and the manual are whole directory trees, so they go the
  ; same way they came - recursively. Nothing the player writes lives
  ; here: saves and scores are kept in the user's own profile.
  RMDir /r "$INSTDIR\world"
  RMDir /r "$INSTDIR\manual"

  Delete "$INSTDIR\*.*"

  Delete "$SMPROGRAMS\Avanor\*.*"
  RMDir "$SMPROGRAMS\Avanor"
  RMDir "$INSTDIR"

SectionEnd
