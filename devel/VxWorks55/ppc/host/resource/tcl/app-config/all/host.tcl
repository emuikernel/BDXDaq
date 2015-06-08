# host.tcl - tcl default host commands bindings
#
# Copyright 1994-1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01w,12aug98,aam  divided host.tcl into two files, gui and nonGui, the nonGui
#                   coded is being used by code that runs on MainWin.  The gui
#                   code was not compatible because it used tk.
# 01v,29may98,p_m  set Copyright date to 1998
# 01u,27feb97,p_m  changed 1996 to 1997 in about box.
# 01t,03dec96,jco  renamed INSTALL to SETUP.
# 01s,05nov96,jco  centralizing installCDCmd into host.tcl.
# 01r,26sep96,dbt  added dtpad to collection of known editors (HPUX10 port)
# 01q,12sep96,p_m  Added -remote option attempt when the browser is Netscape
# 01p,30aug96,elp  changed wpwrVersion{} into wtxTsVersionGet{}.
# 01o,28feb96,jco  updated Copyright date in Tornado version.
# 01n,05feb96,jco  added mailSend routine, defined processListCmd global
#		    and moved some default definitions to sun4-sunos4.tcl,
#		    made the Tornado Version an info window (with OK button).
# 01m,17jan96,jco  fixed topLevelErrorHandler (spr# 5861).
# 01l,02dec95,jco  check XFILESEARCHPATH existence before its use.
# 01k,01dec95,c_s  added vuepad to collection of known editors (SPR #5586).
#                    removed obsolete routine taggedListRange.
# 01j,18nov95,jco  changed mail into /bin/mail.
# 01i,10nov95,jco  adjusted version window size for OpenWindows wm.
# 01h,25oct95,jco  added remoteShellCmd, taggedListRange and tornadoVersion.
# 01g,23oct95,c_s  fixed mistake in last checkin; was sourcing <host-type>.tcl
#                    from old location.
# 01f,05oct95,c_s  added new, centralized interfaces for starting terminal 
#                    applications, editing files, viewing files, and looking
#                    at web pages.  Takes care of environment setup for 
#                    terminals (SPR #4450).
# 01e,25aug95,jco  changed Install to INSTALL
# 01d,23aug95,jco  made use of the EDITOR environment variable, if any.
# 01c,20jun95,jco  changed overwritting files name (use of WIND_HOST_TYPE,
#		     instead of uname cmd).
# 01b,15jun95,jcf  changed install to Install.
# 01a,15jun95,jco  written.

# Global definitions

source [wtxPath host resource tcl app-config all]hostUtils.tcl
source [wtxPath host resource tcl app-config all]guiHost.tcl

