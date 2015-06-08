# Microsoft Developer Studio Generated NMAKE File, Based on vxreg32.dsp
!IF "$(CFG)" == ""
CFG=vxreg32 - Win32 Debug Command Line
!MESSAGE No configuration specified. Defaulting to vxreg32 - Win32 Debug\
 Command Line.
!ENDIF 

!IF "$(CFG)" != "vxreg32 - Win32 Release Command Line" && "$(CFG)" !=\
 "vxreg32 - Win32 Debug Command Line"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vxreg32.mak" CFG="vxreg32 - Win32 Debug Command Line"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vxreg32 - Win32 Release Command Line" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "vxreg32 - Win32 Debug Command Line" (based on\
 "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "vxreg32 - Win32 Release Command Line"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : ".\vxreg32.exe"

!ELSE 

ALL : ".\vxreg32.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vxreg32.obj"
	-@erase ".\vxreg32.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "antlrlib" /D "WIN32" /D "NDEBUG" /D\
 "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\vxreg32.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\vxreg32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /subsystem:console /incremental:no /pdb:"$(OUTDIR)\vxreg32.pdb"\
 /machine:I386 /out:".\vxreg32.exe" 
LINK32_OBJS= \
	"$(INTDIR)\vxreg32.obj"

".\vxreg32.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "vxreg32 - Win32 Debug Command Line"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\x86-win32\bin\vxreg32.exe"

!ELSE 

ALL : "..\..\..\x86-win32\bin\vxreg32.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\vxreg32.obj"
	-@erase "$(OUTDIR)\vxreg32.pdb"
	-@erase "..\..\..\x86-win32\bin\vxreg32.exe"
	-@erase "..\..\..\x86-win32\bin\vxreg32.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "antlrlib" /D "WIN32" /D "_DEBUG"\
 /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\vxreg32.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\vxreg32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\vxreg32.pdb"\
 /debug /machine:I386 /out:"..\..\..\x86-win32\bin\vxreg32.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\vxreg32.obj"

"..\..\..\x86-win32\bin\vxreg32.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "vxreg32 - Win32 Release Command Line" || "$(CFG)" ==\
 "vxreg32 - Win32 Debug Command Line"
SOURCE=.\vxreg32.cpp

"$(INTDIR)\vxreg32.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

