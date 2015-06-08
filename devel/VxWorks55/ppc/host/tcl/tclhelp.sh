#!/bin/sh
#
TCL_LIBRARY=$WIND_BASE/host/tcl/tcl; export TCL_LIBRARY
TCLX_LIBRARY=$WIND_BASE/host/tcl/tclX; export TCLX_LIBRARY
TK_LIBRARY=$WIND_BASE/host/tcl/tk; export TK_LIBRARY
TKX_LIBRARY=$WIND_BASE/host/tcl/tkX; export TKX_LIBRARY
wishx /vobs/wpwr/host/tcl/tclhelp
