# Microsoft Developer Studio Project File - Name="XWorld" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=XWorld - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "XWorld.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XWorld.mak" CFG="XWorld - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XWorld - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "XWorld - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avanor", XIAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XWorld - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /Gr /MLd /W3 /GR /GX /Zi /O2 /I "./engine" /I "./creature" /I "./Game" /I "./Global" /I "./Item" /I "./Helpers" /I "./Magic" /I "./Other" /I "./Map" /I "./" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "__XDEBUG_NO_HERO__" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# SUBTRACT LINK32 /profile /debug /force

!ELSEIF  "$(CFG)" == "XWorld - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "./engine" /I "./creature" /I "./Game" /I "./Global" /I "./Item" /I "./Helpers" /I "./Magic" /I "./Other" /I "./Map" /I "./" /I ".././" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fr /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib luad.lib lualibd.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"LIBCMTD"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "XWorld - Win32 Release"
# Name "XWorld - Win32 Debug"
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Engine"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Engine\defs.h
# End Source File
# Begin Source File

SOURCE=.\Engine\defs01.h
# End Source File
# Begin Source File

SOURCE=.\Engine\xarchive.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\xarchive.h
# End Source File
# Begin Source File

SOURCE=.\Engine\xbaseobj.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\xbaseobj.h
# End Source File
# Begin Source File

SOURCE=.\Engine\xdebug.h
# End Source File
# Begin Source File

SOURCE=.\Engine\xfile.h
# End Source File
# Begin Source File

SOURCE=.\Engine\xlist.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\xlist.h
# End Source File
# Begin Source File

SOURCE=.\Engine\xmapobj.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\xmapobj.h
# End Source File
# Begin Source File

SOURCE=.\Engine\xobject.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\xobject.h
# End Source File
# Begin Source File

SOURCE=.\Engine\xshedule.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\xshedule.h
# End Source File
# End Group
# Begin Group "Creature"

# PROP Default_Filter ""
# Begin Group "Bestiary"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Creature\creatures.cpp
# End Source File
# Begin Source File

SOURCE=.\Creature\creatures.h
# End Source File
# Begin Source File

SOURCE=.\Creature\unique.h
# End Source File
# Begin Source File

SOURCE=.\creature\uniquem.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Creature\ai_view.cpp
# End Source File
# Begin Source File

SOURCE=.\Creature\anycr.cpp
# End Source File
# Begin Source File

SOURCE=.\Creature\anycr.h
# End Source File
# Begin Source File

SOURCE=.\Creature\bodypart.cpp
# End Source File
# Begin Source File

SOURCE=.\Creature\bodypart.h
# End Source File
# Begin Source File

SOURCE=.\Creature\cr_defs.h
# End Source File
# Begin Source File

SOURCE=.\Creature\creature.cpp
# End Source File
# Begin Source File

SOURCE=.\Creature\creature.h
# End Source File
# Begin Source File

SOURCE=.\creature\creature2.cpp
# End Source File
# Begin Source File

SOURCE=.\creature\deity.cpp
# End Source File
# Begin Source File

SOURCE=.\creature\deity.h
# End Source File
# Begin Source File

SOURCE=.\Creature\los.cpp
# End Source File
# Begin Source File

SOURCE=.\Creature\los.h
# End Source File
# Begin Source File

SOURCE=.\Creature\skeep_ai.cpp
# End Source File
# Begin Source File

SOURCE=.\Creature\skeep_ai.h
# End Source File
# Begin Source File

SOURCE=.\Creature\std_ai.cpp
# End Source File
# Begin Source File

SOURCE=.\Creature\std_ai.h
# End Source File
# Begin Source File

SOURCE=.\Creature\xhero.cpp
# End Source File
# Begin Source File

SOURCE=.\Creature\xhero.h
# End Source File
# Begin Source File

SOURCE=.\Creature\xhero2.cpp
# End Source File
# Begin Source File

SOURCE=.\creature\xhero3.cpp
# End Source File
# End Group
# Begin Group "Game"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Game\cave.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\cave.h
# End Source File
# Begin Source File

SOURCE=.\Game\cbuilder.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\cbuilder.h
# End Source File
# Begin Source File

SOURCE=.\Game\dwarfcave.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\dwarfcity.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\dwarfcity.h
# End Source File
# Begin Source File

SOURCE=.\Game\game.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\game.h
# End Source File
# Begin Source File

SOURCE=.\Game\lbuilderpalette.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\ldebug.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\ldebug.h
# End Source File
# Begin Source File

SOURCE=.\Game\location.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\location.h
# End Source File
# Begin Source File

SOURCE=.\Game\mainloc.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\mushcave.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\quest.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\quest.h
# End Source File
# Begin Source File

SOURCE=.\game\rat_cell.cpp
# End Source File
# Begin Source File

SOURCE=.\game\setting.cpp
# End Source File
# Begin Source File

SOURCE=.\game\setting.h
# End Source File
# Begin Source File

SOURCE=.\Game\shop.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\shop.h
# End Source File
# Begin Source File

SOURCE=.\game\udeadtomb.cpp
# End Source File
# Begin Source File

SOURCE=.\game\wizard_dungeon.cpp
# End Source File
# Begin Source File

SOURCE=.\Other\xgen.cpp
# End Source File
# Begin Source File

SOURCE=.\Other\xgen.h
# End Source File
# Begin Source File

SOURCE=.\Game\xtime.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\xtime.h
# End Source File
# End Group
# Begin Group "Global"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Global\global.cpp
# End Source File
# Begin Source File

SOURCE=.\Global\global.h
# End Source File
# Begin Source File

SOURCE=.\global\gmsg.h
# End Source File
# Begin Source File

SOURCE=.\global\xapi.cpp
# End Source File
# Begin Source File

SOURCE=.\global\xapi.h
# End Source File
# Begin Source File

SOURCE=.\global\xstring.cpp
# End Source File
# Begin Source File

SOURCE=.\global\xstring.h
# End Source File
# Begin Source File

SOURCE=.\global\xvector.h
# End Source File
# End Group
# Begin Group "Item"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Item\incl_i.h
# End Source File
# Begin Source File

SOURCE=.\Item\item.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\item.h
# End Source File
# Begin Source File

SOURCE=.\item\item_misc.cpp
# End Source File
# Begin Source File

SOURCE=.\item\item_misc.h
# End Source File
# Begin Source File

SOURCE=.\Item\itemdb.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\itemdb.h
# End Source File
# Begin Source File

SOURCE=.\Item\itemdef.h
# End Source File
# Begin Source File

SOURCE=.\Item\itemf.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\itemf.h
# End Source File
# Begin Source File

SOURCE=.\Item\uniquei.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\uniquei.h
# End Source File
# Begin Source File

SOURCE=.\Item\xamulet.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\xamulet.h
# End Source File
# Begin Source File

SOURCE=.\Item\xanyfood.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\xanyfood.h
# End Source File
# Begin Source File

SOURCE=.\Item\xarmor.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\xarmor.h
# End Source File
# Begin Source File

SOURCE=.\Item\xbook.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\xbook.h
# End Source File
# Begin Source File

SOURCE=.\Item\xboots.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\xboots.h
# End Source File
# Begin Source File

SOURCE=.\Item\xcap.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\xcap.h
# End Source File
# Begin Source File

SOURCE=.\Item\xcloak.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\xcloak.h
# End Source File
# Begin Source File

SOURCE=.\Item\xclothe.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\xclothe.h
# End Source File
# Begin Source File

SOURCE=.\Item\xcorpse.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\xcorpse.h
# End Source File
# Begin Source File

SOURCE=.\Item\xenhance.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\xenhance.h
# End Source File
# Begin Source File

SOURCE=.\Item\xgloves.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\xgloves.h
# End Source File
# Begin Source File

SOURCE=.\item\xherb.cpp
# End Source File
# Begin Source File

SOURCE=.\item\xherb.h
# End Source File
# Begin Source File

SOURCE=.\Item\xmissile.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\xmissile.h
# End Source File
# Begin Source File

SOURCE=.\Item\xmissileweapon.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\xmissileweapon.h
# End Source File
# Begin Source File

SOURCE=.\Item\xmoney.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\xmoney.h
# End Source File
# Begin Source File

SOURCE=.\Item\xpotion.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\xpotion.h
# End Source File
# Begin Source File

SOURCE=.\Item\xration.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\xration.h
# End Source File
# Begin Source File

SOURCE=.\Item\xring.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\xring.h
# End Source File
# Begin Source File

SOURCE=.\Item\xscroll.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\xscroll.h
# End Source File
# Begin Source File

SOURCE=.\Item\xshield.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\xshield.h
# End Source File
# Begin Source File

SOURCE=.\item\xtool.cpp
# End Source File
# Begin Source File

SOURCE=.\item\xtool.h
# End Source File
# Begin Source File

SOURCE=.\Item\xweapon.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\xweapon.h
# End Source File
# End Group
# Begin Group "Helpers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Helpers\dice.cpp
# End Source File
# Begin Source File

SOURCE=.\Helpers\dice.h
# End Source File
# Begin Source File

SOURCE=.\helpers\hiscore.cpp
# End Source File
# Begin Source File

SOURCE=.\helpers\hiscore.h
# End Source File
# Begin Source File

SOURCE=.\helpers\manual.cpp
# End Source File
# Begin Source File

SOURCE=.\helpers\manual.h
# End Source File
# Begin Source File

SOURCE=.\Helpers\msgwin.cpp
# End Source File
# Begin Source File

SOURCE=.\Helpers\msgwin.h
# End Source File
# Begin Source File

SOURCE=.\Helpers\point.h
# End Source File
# Begin Source File

SOURCE=.\Helpers\rect.cpp
# End Source File
# Begin Source File

SOURCE=.\Helpers\rect.h
# End Source File
# Begin Source File

SOURCE=.\Helpers\strproc.cpp
# End Source File
# Begin Source File

SOURCE=.\Helpers\strproc.h
# End Source File
# Begin Source File

SOURCE=.\helpers\xgui.cpp
# End Source File
# Begin Source File

SOURCE=.\helpers\xgui.h
# End Source File
# Begin Source File

SOURCE=.\Helpers\xguihtml.cpp
# End Source File
# Begin Source File

SOURCE=.\Helpers\xguihtml.re

!IF  "$(CFG)" == "XWorld - Win32 Release"

!ELSEIF  "$(CFG)" == "XWorld - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\helpers\xstr.cpp
# End Source File
# Begin Source File

SOURCE=.\helpers\xstr.h
# End Source File
# End Group
# Begin Group "Magic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Magic\effect.cpp
# End Source File
# Begin Source File

SOURCE=.\Magic\effect.h
# End Source File
# Begin Source File

SOURCE=.\Magic\magic.cpp
# End Source File
# Begin Source File

SOURCE=.\Magic\magic.h
# End Source File
# Begin Source File

SOURCE=.\Magic\mod_defs.h
# End Source File
# Begin Source File

SOURCE=.\Magic\modifer.cpp
# End Source File
# Begin Source File

SOURCE=.\Magic\modifer.h
# End Source File
# Begin Source File

SOURCE=.\Magic\modifers.cpp
# End Source File
# Begin Source File

SOURCE=.\Magic\modifers.h
# End Source File
# Begin Source File

SOURCE=.\Magic\resist.cpp
# End Source File
# Begin Source File

SOURCE=.\Magic\resist.h
# End Source File
# Begin Source File

SOURCE=.\Magic\skill.cpp
# End Source File
# Begin Source File

SOURCE=.\Magic\skill.h
# End Source File
# Begin Source File

SOURCE=.\Magic\skills.cpp
# End Source File
# Begin Source File

SOURCE=.\Magic\skills.h
# End Source File
# Begin Source File

SOURCE=.\Magic\spelldef.h
# End Source File
# Begin Source File

SOURCE=.\Magic\stats.cpp
# End Source File
# Begin Source File

SOURCE=.\Magic\stats.h
# End Source File
# Begin Source File

SOURCE=.\Magic\wskills.cpp
# End Source File
# Begin Source File

SOURCE=.\Magic\wskills.h
# End Source File
# End Group
# Begin Group "Other"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\other\other_misc.cpp
# End Source File
# Begin Source File

SOURCE=.\other\other_misc.h
# End Source File
# End Group
# Begin Group "Map"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Map\map.cpp
# End Source File
# Begin Source File

SOURCE=.\Map\map.h
# End Source File
# Begin Source File

SOURCE=.\Map\xanyplace.cpp
# End Source File
# Begin Source File

SOURCE=.\Map\xanyplace.h
# End Source File
# End Group
# Begin Group "temp"

# PROP Default_Filter ""
# End Group
# Begin Group "manual"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Manual\basic.html
# End Source File
# Begin Source File

SOURCE=.\Manual\credits.html
# End Source File
# Begin Source File

SOURCE=.\Manual\equipment.html
# End Source File
# Begin Source File

SOURCE=.\Manual\index.html
# End Source File
# Begin Source File

SOURCE=.\Manual\intro.html
# End Source File
# Begin Source File

SOURCE=.\Manual\kblayout.html
# End Source File
# Begin Source File

SOURCE=.\Manual\magic.html
# End Source File
# Begin Source File

SOURCE=.\Manual\misc.html
# End Source File
# Begin Source File

SOURCE=.\Manual\qstart.html
# End Source File
# Begin Source File

SOURCE=.\Manual\skills.html
# End Source File
# Begin Source File

SOURCE=.\Manual\tactics.html
# End Source File
# Begin Source File

SOURCE=.\Manual\testmode.html
# End Source File
# Begin Source File

SOURCE=.\Manual\wskills.html
# End Source File
# End Group
# Begin Source File

SOURCE=.\bugs.txt
# End Source File
# Begin Source File

SOURCE=.\changes.txt
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\todo.txt
# End Source File
# End Target
# End Project
