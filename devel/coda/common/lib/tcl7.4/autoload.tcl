#	"@(#)tclStruct:library:autoload.tcl	1.1	95/09/08"
#
# Written by Matthew Costello
# (c) 1995 AT&T Global Information Solutions, Dayton Ohio USA
#
# See the file "license.terms" for information on usage and
# redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.

# This is a 'fake' Tcl library that is used to invoke the
# "load" command when the intrinsic tclStruct commands
# have not yet been loaded into the current process.

# Put the procedures here for auto_mkindex to find... \
proc struct_info {} \
proc struct_typedef {} \
proc struct_untypedef {} \
proc struct_new {} \
proc struct_info {} \
proc struct_copy {} \
proc struct_read {} \
proc struct_write {} \

# Load the shared library
load libtclStruct[info sharedlibextension] Struct

