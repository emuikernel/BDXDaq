#----------------------------------------------------------------------------
#	%W%	%G%
#
#	Local targets 
#	GNU version
#	Requires GNU make
#----------------------------------------------------------------------------

OBJDIRS = .obj .shobj # .obj_debug .obj_profile .obj_optimize

all.local: build.objdirs build.local install.local

#----------------------------------------------------------------------------
#	C/C++ compilation targets
#----------------------------------------------------------------------------

build.local:	$(BUILD)

# Set up the suffixes for IDL.
.SUFFIXES:
.SUFFIXES: .C .idl $(SUFFIXES)

# and here's how to compile C++ files from the IDL file.
# only ONE of these rules will be run at make-time,

%S.C: %.idl
	$(IDL) $(IDLFLAGS) $<

%C.C: %.idl
	$(IDL) $(IDLFLAGS) $<

%.hh: %.idl
	$(IDL) $(IDLFLAGS) $<

# C++ related targets

$(VDIR)%.o:	%.c
	$(COMPILE.c) -o $@ $<

$(VDIR)%.o:	%.C
	$(COMPILE.cc) -o $@ $<

$(VSHDIR)%.so:	%.C
	$(SOBUILD)

#----------------------------------------------------------------------------
#	Library generation targets
#----------------------------------------------------------------------------

.PRECIOUS:	$(VLIB)

#----------------------------------------------------------------------------
#	Installation targets
#----------------------------------------------------------------------------

install.local: 	$(INSTALL)

deinstall.local:
	$(RM) $(INSTALL) $(LIB:%.a=$(INSLIB)/%*.a) $(SHLIB:%.so=$(INSLIB)/%*.so)

$(INSBIN)/%$(VAR) \
$(INSINC)/% \
$(INSLIB)/%$(VAR).a \
$(INSLIB)/%$(VAR).so \
$(INSMAN)/man1/% \
$(INSMAN)/man2/% \
$(INSMAN)/man3/% \
$(INSMAN)/man4/% \
$(INSMAN)/man5/% \
$(INSMAN)/man6/% \
$(INSMAN)/man7/% \
$(INSMAN)/man8/% \
$(INSMAN)/manl/% \
$(INSMAN)/mann/% :
	@if test -s $@ ; then \
		echo "$(@F) already installed" ; \
        else \
		echo "Installing $(@F) -> $(@D)" ; \
		ln -s $(shell pwd)/$(@F) $@ ; \
	fi

#----------------------------------------------------------------------------
#	Cleanup targets
#----------------------------------------------------------------------------

clean.local:
	-$(RM) -f *.o *~ *.bak Makefile.old core
	-$(RM) -rf $(OBJDIRS) ptrepository Templates.DB

realclean.local: clean.local
	-$(RM) -f $(BIN:%=%) $(BIN:%=%_debug) $(BIN:%=%_profile) $(BIN:%=%_optimize)
	-$(RM) -f $(LIB:%=%) $(LIB:%=%_debug) $(LIB:%=%_profile) $(LIB:%=%_optimize)
	-$(RM) -f $(SHLIB:%=%) $(SHLIB:%=%_debug) $(SHLIB:%=%_profile) $(SHLIB:%=%_optimize)

#----------------------------------------------------------------------------
#	Dependency generation target
#----------------------------------------------------------------------------

depend.local: Makefile
	@$(RM) -f Makefile.old
	@cp Makefile Makefile.old
	$(WRAPPER_ROOT)/bin/g++dep $(CPPFLAGS) $(LSRC) $(SRC)
	@cat Makefile | \
		sed -e "s;$(WRAPPER_ROOT);$$(WRAPPER_ROOT);g" \
		    -e "s; /[-a-zA-Z0-9_./]*\.h;;g" \
		    -e "/:$$/d" \
		    -e "s;\([-a-zA-Z0-9_]*\)\.o:;.obj/\1.o .shobj/\1.so:;" \
	> Makefile.new
	@mv Makefile.new Makefile 
	@if	cmp -s Makefile Makefile.old ;\
	then	echo "Makefile dependencies unchanged." ;\
	else \
		echo "Makefile dependencies updated." ;\
	fi ;\
	$(RM) -f Makefile.old ;
#----------------------------------------------------------------------------
#	RCS info target
#----------------------------------------------------------------------------

rcs_info.local:
	@rcs info

#----------------------------------------------------------------------------
#	Variant targets and conditional macros
#----------------------------------------------------------------------------

build.objdirs: $(OBJDIRS)

$(OBJDIRS):
	test -d $@ || mkdir $@
