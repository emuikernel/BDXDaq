@echo off
rem
rem
rem Copyright 1984-1999 Wind River Systems, Inc
rem
rem modification history:
rem ---------------------
rem 01a,13nov01,rbl  written.
rem

rem setting the TCL_LIBRARY environment variable 
set TCL_LIBRARY=%WIND_BASE%\host\tcl\tcl

set args=%1 %2 %3 %4 %5 %6 %7 %8 %9 

rem use - for comparison so that we can have lists quoted with ""

if -%9-==-- goto gotArgs 

:getNextArg 

rem if we have more than 8 arguments shift arguments into the "9" slot until there
rem are no more...

shift 

rem use - for comparison so that we can have lists quoted with ""

if -%9-==-- goto gotArgs 
set args=%args% %9 
goto getNextArg 

:gotArgs 

rem now invoke wtxtcl shell, and source in makegen.tcl

wtxtcl %WIND_BASE%\host\resource\tcl\app-config\Project\makegen.tcl %args% 
