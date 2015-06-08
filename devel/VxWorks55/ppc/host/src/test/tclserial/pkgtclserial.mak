# Microsoft Developer Studio Generated NMAKE File, Based on pkgtclserial.dsp
!IF "$(CFG)" == ""
CFG=pkgtclserial - Win32 Debug Command Line
!MESSAGE No configuration specified. Defaulting to pkgtclserial - Win32 Debug\
 Command Line.
!ENDIF 

!IF "$(CFG)" != "pkgtclserial - Win32 Release" && "$(CFG)" !=\
 "pkgtclserial - Win32 Debug" && "$(CFG)" !=\
 "pkgtclserial - Win32 Release Command Line" && "$(CFG)" !=\
 "pkgtclserial - Win32 Debug Command Line"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pkgtclserial.mak"\
 CFG="pkgtclserial - Win32 Debug Command Line"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pkgtclserial - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pkgtclserial - Win32 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pkgtclserial - Win32 Release Command Line" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pkgtclserial - Win32 Debug Command Line" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pkgtclserial - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\x86-win32\bin\pkgtclserial.dll"

!ELSE 

ALL : "..\..\..\x86-win32\bin\pkgtclserial.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\pkgtclserial.obj"
	-@erase "$(INTDIR)\tclserial.win32.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\pkgtclserial.map"
	-@erase "..\..\..\x86-win32\bin\pkgtclserial.dll"
	-@erase "..\..\..\x86-win32\lib\pkgtclserial.exp"
	-@erase "..\..\..\x86-win32\lib\pkgtclserial.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/MD /W3 /GX /O2 /I "..\..\tcl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\pkgtclserial.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pkgtclserial.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=..\..\..\x86-win32\lib\tcldll.lib kernel32.lib user32.lib\
 gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /nologo\
 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\pkgtclserial.pdb"\
 /map:"$(INTDIR)\pkgtclserial.map" /machine:I386\
 /out:"..\..\..\x86-win32\bin\pkgtclserial.dll"\
 /implib:"..\..\..\x86-win32\lib\pkgtclserial.lib" 
LINK32_OBJS= \
	"$(INTDIR)\pkgtclserial.obj" \
	"$(INTDIR)\tclserial.win32.obj"

"..\..\..\x86-win32\bin\pkgtclserial.dll" : "$(OUTDIR)" $(DEF_FILE)\
 $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "pkgtclserial - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\x86-win32\bin\pkgtclserial-d.dll"

!ELSE 

ALL : "..\..\..\x86-win32\bin\pkgtclserial-d.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\pkgtclserial.obj"
	-@erase "$(INTDIR)\tclserial.win32.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\pkgtclserial-d.map"
	-@erase "$(OUTDIR)\pkgtclserial-d.pdb"
	-@erase "..\..\..\x86-win32\bin\pkgtclserial-d.dll"
	-@erase "..\..\..\x86-win32\bin\pkgtclserial-d.ilk"
	-@erase "..\..\..\x86-win32\lib\pkgtclserial-d.exp"
	-@erase "..\..\..\x86-win32\lib\pkgtclserial-d.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/MDd /W3 /Gm /GX /Zi /Od /I "..\..\tcl" /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /Fp"$(INTDIR)\pkgtclserial.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pkgtclserial.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=..\..\..\x86-win32\lib\tcldll-d.lib kernel32.lib user32.lib\
 gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /nologo\
 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\pkgtclserial-d.pdb"\
 /map:"$(INTDIR)\pkgtclserial-d.map" /debug /machine:I386\
 /out:"..\..\..\x86-win32\bin\pkgtclserial-d.dll"\
 /implib:"..\..\..\x86-win32\lib\pkgtclserial-d.lib" 
LINK32_OBJS= \
	"$(INTDIR)\pkgtclserial.obj" \
	"$(INTDIR)\tclserial.win32.obj"

"..\..\..\x86-win32\bin\pkgtclserial-d.dll" : "$(OUTDIR)" $(DEF_FILE)\
 $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "pkgtclserial - Win32 Release Command Line"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\x86-win32\bin\pkgtclserial.dll"

!ELSE 

ALL : "..\..\..\x86-win32\bin\pkgtclserial.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\pkgtclserial.obj"
	-@erase "$(INTDIR)\tclserial.win32.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\pkgtclserial.map"
	-@erase "..\..\..\x86-win32\bin\pkgtclserial.dll"
	-@erase "..\..\..\x86-win32\lib\pkgtclserial.exp"
	-@erase "..\..\..\x86-win32\lib\pkgtclserial.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/MD /W3 /GX /O2 /I "..\..\tcl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pkgtclserial.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=..\..\..\x86-win32\lib\tcldll.lib kernel32.lib user32.lib\
 gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /nologo\
 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\pkgtclserial.pdb"\
 /map:"$(INTDIR)\pkgtclserial.map" /machine:I386\
 /out:"..\..\..\x86-win32\bin\pkgtclserial.dll"\
 /implib:"..\..\..\x86-win32\lib\pkgtclserial.lib" 
LINK32_OBJS= \
	"$(INTDIR)\pkgtclserial.obj" \
	"$(INTDIR)\tclserial.win32.obj"

"..\..\..\x86-win32\bin\pkgtclserial.dll" : "$(OUTDIR)" $(DEF_FILE)\
 $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "pkgtclserial - Win32 Debug Command Line"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\x86-win32\bin\pkgtclserial-d.dll"

!ELSE 

ALL : "..\..\..\x86-win32\bin\pkgtclserial-d.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\pkgtclserial.obj"
	-@erase "$(INTDIR)\tclserial.win32.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\pkgtclserial-d.map"
	-@erase "$(OUTDIR)\pkgtclserial-d.pdb"
	-@erase "..\..\..\x86-win32\bin\pkgtclserial-d.dll"
	-@erase "..\..\..\x86-win32\bin\pkgtclserial-d.ilk"
	-@erase "..\..\..\x86-win32\lib\pkgtclserial-d.exp"
	-@erase "..\..\..\x86-win32\lib\pkgtclserial-d.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/MDd /W3 /Gm /GX /Zi /Od /I "..\..\tcl" /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pkgtclserial.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=..\..\..\x86-win32\lib\tcldll-d.lib kernel32.lib user32.lib\
 gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /nologo\
 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\pkgtclserial-d.pdb"\
 /map:"$(INTDIR)\pkgtclserial-d.map" /debug /machine:I386\
 /out:"..\..\..\x86-win32\bin\pkgtclserial-d.dll"\
 /implib:"..\..\..\x86-win32\lib\pkgtclserial-d.lib" 
LINK32_OBJS= \
	"$(INTDIR)\pkgtclserial.obj" \
	"$(INTDIR)\tclserial.win32.obj"

"..\..\..\x86-win32\bin\pkgtclserial-d.dll" : "$(OUTDIR)" $(DEF_FILE)\
 $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(CFG)" == "pkgtclserial - Win32 Release" || "$(CFG)" ==\
 "pkgtclserial - Win32 Debug" || "$(CFG)" ==\
 "pkgtclserial - Win32 Release Command Line" || "$(CFG)" ==\
 "pkgtclserial - Win32 Debug Command Line"
SOURCE=.\pkgtclserial.c
DEP_CPP_PKGTC=\
	"..\..\tcl\tcl.h"\
	".\tclserial.h"\
	

"$(INTDIR)\pkgtclserial.obj" : $(SOURCE) $(DEP_CPP_PKGTC) "$(INTDIR)"


SOURCE=.\tclserial.win32.c
DEP_CPP_TCLSE=\
	".\tclserial.h"\
	

"$(INTDIR)\tclserial.win32.obj" : $(SOURCE) $(DEP_CPP_TCLSE) "$(INTDIR)"



!ENDIF 

