# Makefile - for wtxtest executable
#
# modification history
# --------------------
# 01l,12apr99,fle  added intdoc rule and related variables for internal doc
#                  generation
# 01k,22sep98,fle  doc : changed doc files book belonging
# 01j,09sep98,fle  doc : changed refgen options since refgen.tcl
# 01i,03aug98,fle  doc : redirected output to DOCS_DIR
# 01h,27may98,fle  doc now uses refgen instead of mangen
# 01g,06May98,p_b  added wtxtest.sh to run "tclsh wtxtest.tcl".
# 01f,28jan98,p_b  WTX 2.0 adapted. (Replaced wtxtest.sh by wtxtest.tcl).
# 01e,08jun95,p_m  added documentation building support.
# 01d,22may95,jcf  name revision.
# 01c,11may95,p_m  changed chmod flags to $(CHMODFLAGS) to make shell 
#		   scripts writable avoiding clearmake to fail.
#		   removed LOCAL_CLEAN.
# 01b,14apr95,p_m  updated for new tree.
# 01a,10mar95,jcf  written.
#
# DESCRIPTION
# This file contains the makefile rules for building the wtxtest library
#
# SUBSTITUTION
#
# INCLUDES
#	$(WIND_BASE)/host/include/make/generic.mh
#	$(WIND_BASE)/host/include/make/$(HOST).mh
#	$(WIND_BASE)/host/include/make/generic2.mh
#

include         $(WIND_BASE)/host/include/make/generic.mh
include         $(WIND_BASE)/host/include/make/$(HOST).mh

WTXTEST		= $(WIND_BIN)/wtxtest

PROGRAMS	= $(WTXTEST)

# doc related variables

DOCS		= doc
DOC_FILES	= wtxtest.tcl
INT_DOCS	= intdoc
INT_DOC_FILES	= $(DOC_FILES)

default:	prog

$(WTXTEST):	wtxtest.sh
		$(CP) wtxtest.sh $(WTXTEST)
		$(CHMOD) $(CHMODFLAGS) $(WTXTEST)

# doc build

doc:		$(DOC_FILES)
		$(REFGEN_EXE) $(REFGEN_OPT) -book Tornado_Reference \
		    -chapter Tornado_Tools -out $(DOCS_ROOT)/tornado/tools \
		    $(DOC_FILES)
		$(HTMLLINK_EXE) $(DOCS_ROOT)/tornado/tools

# internal doc build

intdoc:		$(INT_DOC_FILES)
		$(REFGEN_EXE) $(REFGEN_INT_OPT) -book Tornado_Reference \
		    -chapter Tornado_Tools -out $(INT_DOCS_ROOT)/tornado/tools \
		    $(INT_DOC_FILES)
		$(HTMLLINK_EXE) $(INT_DOCS_ROOT)/tornado/tools

include         $(WIND_BASE)/host/include/make/generic2.mh
