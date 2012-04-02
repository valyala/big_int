# Microsoft Developer Studio Project File - Name="php_big_int" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=php_big_int - Win32 Debug_php5
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "php_big_int.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "php_big_int.mak" CFG="php_big_int - Win32 Debug_php5"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "php_big_int - Win32 Debug_php5" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "php_big_int - Win32 Release_php5" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "php_big_int - Win32 Debug_php4" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "php_big_int - Win32 Release_php4" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "php_big_int - Win32 Debug_php5"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_php5"
# PROP BASE Intermediate_Dir "Debug_php5"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_php5"
# PROP Intermediate_Dir "Debug_php5"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /I "libbig_int/include" /I "../.." /I "../../main" /I "../../Zend" /I "../../TSRM" /ZI /W3 /Od /D "HAVE_BIG_INT" /D "COMPILE_DL_BIG_INT" /D "ZTS" /D "NDEBUG" /D "ZEND_WIN32" /D "PHP_WIN32" /D "WIN32" /D "ZEND_DEBUG=1" /D "_MBCS" /Gm /GZ /c /GX 
# ADD CPP /nologo /MTd /I "libbig_int/include" /I "../.." /I "../../main" /I "../../Zend" /I "../../TSRM" /ZI /W3 /Od /D "HAVE_BIG_INT" /D "COMPILE_DL_BIG_INT" /D "ZTS" /D "NDEBUG" /D "ZEND_WIN32" /D "PHP_WIN32" /D "WIN32" /D "ZEND_DEBUG=1" /D "_MBCS" /Gm /GZ /c /GX 
# ADD BASE MTL /nologo /win32 
# ADD MTL /nologo /win32 
# ADD BASE RSC /l 1033 
# ADD RSC /l 1033 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo 
# ADD BSC32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5ts.lib /nologo /dll /out:"Debug_php5\php_big_int.dll" /incremental:yes /libpath:"../../Release_TS" /debug /pdb:"Debug_php5\php_big_int.pdb" /pdbtype:sept /subsystem:windows /implib:"$(OutDir)/php_big_int.lib" /machine:ix86 
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5ts.lib /nologo /dll /out:"Debug_php5\php_big_int.dll" /incremental:yes /libpath:"../../Release_TS" /debug /pdb:"Debug_php5\php_big_int.pdb" /pdbtype:sept /subsystem:windows /implib:"$(OutDir)/php_big_int.lib" /machine:ix86 

!ELSEIF  "$(CFG)" == "php_big_int - Win32 Release_php5"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_php5"
# PROP BASE Intermediate_Dir "Release_php5"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_php5"
# PROP Intermediate_Dir "Release_php5"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /I "libbig_int/include" /I "../.." /I "../../main" /I "../../Zend" /I "../../TSRM" /W3 /O1 /Og /Oi /Os /Oy /GT /G6 /GA /D "HAVE_BIG_INT" /D "COMPILE_DL_BIG_INT" /D "ZTS" /D "NDEBUG" /D "ZEND_WIN32" /D "PHP_WIN32" /D "WIN32" /D "ZEND_DEBUG=0" /D "_MBCS" /GF /Gy /TC /c /GX 
# ADD CPP /nologo /MD /I "libbig_int/include" /I "../.." /I "../../main" /I "../../Zend" /I "../../TSRM" /W3 /O1 /Og /Oi /Os /Oy /GT /G6 /GA /D "HAVE_BIG_INT" /D "COMPILE_DL_BIG_INT" /D "ZTS" /D "NDEBUG" /D "ZEND_WIN32" /D "PHP_WIN32" /D "WIN32" /D "ZEND_DEBUG=0" /D "_MBCS" /GF /Gy /TC /c /GX 
# ADD BASE MTL /nologo /win32 
# ADD MTL /nologo /win32 
# ADD BASE RSC /l 1033 
# ADD RSC /l 1033 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo 
# ADD BSC32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5ts.lib /nologo /dll /out:"Release_php5\php_big_int.dll" /incremental:no /libpath:"../../Release_TS" /pdbtype:sept /subsystem:windows /opt:ref /opt:icf /implib:"$(OutDir)/php_big_int.lib" /machine:ix86 
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php5ts.lib /nologo /dll /out:"Release_php5\php_big_int.dll" /incremental:no /libpath:"../../Release_TS" /pdbtype:sept /subsystem:windows /opt:ref /opt:icf /implib:"$(OutDir)/php_big_int.lib" /machine:ix86 

!ELSEIF  "$(CFG)" == "php_big_int - Win32 Debug_php4"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_php4"
# PROP BASE Intermediate_Dir "Debug_php4"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_php4"
# PROP Intermediate_Dir "Debug_php4"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /I "libbig_int/include" /I "../.." /I "../../main" /I "../../Zend" /I "../../TSRM" /ZI /W3 /Od /D "HAVE_BIG_INT" /D "COMPILE_DL_BIG_INT" /D "ZTS" /D "NDEBUG" /D "ZEND_WIN32" /D "PHP_WIN32" /D "WIN32" /D "ZEND_DEBUG=1" /D "_MBCS" /Gm /GZ /c /GX 
# ADD CPP /nologo /MTd /I "libbig_int/include" /I "../.." /I "../../main" /I "../../Zend" /I "../../TSRM" /ZI /W3 /Od /D "HAVE_BIG_INT" /D "COMPILE_DL_BIG_INT" /D "ZTS" /D "NDEBUG" /D "ZEND_WIN32" /D "PHP_WIN32" /D "WIN32" /D "ZEND_DEBUG=1" /D "_MBCS" /Gm /GZ /c /GX 
# ADD BASE MTL /nologo /win32 
# ADD MTL /nologo /win32 
# ADD BASE RSC /l 1033 
# ADD RSC /l 1033 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo 
# ADD BSC32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php4ts.lib /nologo /dll /out:"Debug_php4\php_big_int.dll" /incremental:yes /libpath:"../../Release_TS" /debug /pdb:"Debug_php4\php_big_int.pdb" /pdbtype:sept /subsystem:windows /implib:"$(OutDir)/php_big_int.lib" /machine:ix86 
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php4ts.lib /nologo /dll /out:"Debug_php4\php_big_int.dll" /incremental:yes /libpath:"../../Release_TS" /debug /pdb:"Debug_php4\php_big_int.pdb" /pdbtype:sept /subsystem:windows /implib:"$(OutDir)/php_big_int.lib" /machine:ix86 

!ELSEIF  "$(CFG)" == "php_big_int - Win32 Release_php4"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_php4"
# PROP BASE Intermediate_Dir "Release_php4"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_php4"
# PROP Intermediate_Dir "Release_php4"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /I "libbig_int/include" /I "../.." /I "../../main" /I "../../Zend" /I "../../TSRM" /W3 /O1 /Og /Oi /Os /Oy /GT /G6 /GA /D "HAVE_BIG_INT" /D "COMPILE_DL_BIG_INT" /D "ZTS" /D "NDEBUG" /D "ZEND_WIN32" /D "PHP_WIN32" /D "WIN32" /D "ZEND_DEBUG=0" /D "_MBCS" /GF /Gy /TC /c /GX 
# ADD CPP /nologo /MD /I "libbig_int/include" /I "../.." /I "../../main" /I "../../Zend" /I "../../TSRM" /W3 /O1 /Og /Oi /Os /Oy /GT /G6 /GA /D "HAVE_BIG_INT" /D "COMPILE_DL_BIG_INT" /D "ZTS" /D "NDEBUG" /D "ZEND_WIN32" /D "PHP_WIN32" /D "WIN32" /D "ZEND_DEBUG=0" /D "_MBCS" /GF /Gy /TC /c /GX 
# ADD BASE MTL /nologo /win32 
# ADD MTL /nologo /win32 
# ADD BASE RSC /l 1033 
# ADD RSC /l 1033 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo 
# ADD BSC32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php4ts.lib /nologo /dll /out:"Release_php4\php_big_int.dll" /incremental:no /libpath:"../../Release_TS" /pdbtype:sept /subsystem:windows /opt:ref /opt:icf /implib:"$(OutDir)/php_big_int.lib" /machine:ix86 
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib php4ts.lib /nologo /dll /out:"Release_php4\php_big_int.dll" /incremental:no /libpath:"../../Release_TS" /pdbtype:sept /subsystem:windows /opt:ref /opt:icf /implib:"$(OutDir)/php_big_int.lib" /machine:ix86 

!ENDIF

# Begin Target

# Name "php_big_int - Win32 Debug_php5"
# Name "php_big_int - Win32 Release_php5"
# Name "php_big_int - Win32 Debug_php4"
# Name "php_big_int - Win32 Release_php4"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;def;odl;idl;hpj;bat;asm"
# Begin Source File

SOURCE=php_big_int.c
# End Source File
# Begin Group "libbig_int"

# PROP Default_Filter ""
# Begin Source File

SOURCE=libbig_int\src\basic_funcs.c
# End Source File
# Begin Source File

SOURCE=libbig_int\src\bitset_funcs.c
# End Source File
# Begin Source File

SOURCE=libbig_int\src\memory_manager.c
# End Source File
# Begin Source File

SOURCE=libbig_int\src\modular_arithmetic.c
# End Source File
# Begin Source File

SOURCE=libbig_int\src\number_theory.c
# End Source File
# Begin Source File

SOURCE=libbig_int\src\service_funcs.c
# End Source File
# Begin Source File

SOURCE=libbig_int\src\str_funcs.c
# End Source File
# Begin Group "low_level_funcs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=libbig_int\src\low_level_funcs\add.c
# End Source File
# Begin Source File

SOURCE=libbig_int\src\low_level_funcs\and.c
# End Source File
# Begin Source File

SOURCE=libbig_int\src\low_level_funcs\andnot.c
# End Source File
# Begin Source File

SOURCE=libbig_int\src\low_level_funcs\cmp.c
# End Source File
# Begin Source File

SOURCE=libbig_int\src\low_level_funcs\div.c
# End Source File
# Begin Source File

SOURCE=libbig_int\src\low_level_funcs\mul.c
# End Source File
# Begin Source File

SOURCE=libbig_int\src\low_level_funcs\or.c
# End Source File
# Begin Source File

SOURCE=libbig_int\src\low_level_funcs\sqr.c
# End Source File
# Begin Source File

SOURCE=libbig_int\src\low_level_funcs\sub.c
# End Source File
# Begin Source File

SOURCE=libbig_int\src\low_level_funcs\xor.c
# End Source File
# End Group
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;inc"
# Begin Source File

SOURCE=php_big_int.h
# End Source File
# Begin Group "libbig_int"

# PROP Default_Filter ""
# Begin Source File

SOURCE=libbig_int\include\basic_funcs.h
# End Source File
# Begin Source File

SOURCE=libbig_int\include\big_int.h
# End Source File
# Begin Source File

SOURCE=libbig_int\include\big_int_full.h
# End Source File
# Begin Source File

SOURCE=libbig_int\include\bitset_funcs.h
# End Source File
# Begin Source File

SOURCE=libbig_int\include\get_bit_length.h
# End Source File
# Begin Source File

SOURCE=libbig_int\include\low_level_funcs.h
# End Source File
# Begin Source File

SOURCE=libbig_int\include\memory_manager.h
# End Source File
# Begin Source File

SOURCE=libbig_int\include\modular_arithmetic.h
# End Source File
# Begin Source File

SOURCE=libbig_int\include\number_theory.h
# End Source File
# Begin Source File

SOURCE=libbig_int\include\service_funcs.h
# End Source File
# Begin Source File

SOURCE=libbig_int\include\str_funcs.h
# End Source File
# Begin Source File

SOURCE=libbig_int\include\str_types.h
# End Source File
# End Group
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

