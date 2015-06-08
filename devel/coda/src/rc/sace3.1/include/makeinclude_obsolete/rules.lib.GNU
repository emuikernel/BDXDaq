#----------------------------------------------------------------------------
#	%W%	%G%
#
#	Build libraries (i.e., contain no binary executables)
#	GNU version
#	Requires GNU make
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# The following targets arrange to build both unshared and shared libraries
#----------------------------------------------------------------------------

VSHLIB  = $(SHLIB:%.so=%$(VAR).so)
SHOBJS 	= $(LSRC:%.C=$(VSHDIR)%.so)
VOBJS   = $(LSRC:%.C=$(VDIR)%.o)

INSTALL = \
	$(DEFS:%=$(INSINC)/%) \
	$(VLIB:%.a=$(INSLIB)/%.a) \
	$(VSHLIB:%.so=$(INSLIB)/%.so)

# Comment these two lines out if you want to build both *.a and *.so libraries...
#$(VSHLIB): $(SHOBJS) 
#	$(SOLINK.cc) -o $@ $(LDFLAGS) $(VSHDIR)*.o

# Uncomment the remaining lines if you want to build both *.a and *.so libraries...
VLIB	= $(LIB:%.a=%$(VAR).a)

$(VLIB): $(VOBJS)
	$(AR) $(ARFLAGS) $@ $?
	-$(RANLIB) $@

# Note that if you don't want to build shared libraries, just remove the $(SHOBJS)
$(VSHLIB): $(VLIB)
	$(SOLINK.cc) -o $@ $(LDFLAGS) $(VSHDIR)*.o

