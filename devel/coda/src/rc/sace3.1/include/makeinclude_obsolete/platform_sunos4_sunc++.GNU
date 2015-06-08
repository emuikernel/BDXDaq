# SunOS 4.x (Solaris 1.x) with SunC++ 3.x
CC		= cc
CXX		= CC
DLD		= /bin/ld
LIBS		= -lnsl
PIC		= -pic
RANLIB		= ranlib
SOFLAGS		= -assert pure-text 
#	       (1) initiate a "dummy" attempt at linking; i.e.
#		call CC to link the prospective *.so object (this will
#		force instantiation of all templates; write those into
#		directory appropriately named); make sure to ignore the
#		error that CC will throw since it can't complete linking...;
#		(2) now call ld with the original object as well as all
#		the template objects in the pt directory (if there were
#		any at all);
SOBUILD		= $(COMPILE.cc) $(PIC) -o $(VSHDIR)$*.o $<; \
		  $(RM) -rf $(VSHDIR)pt$*; mkdir $(VSHDIR)pt$*; \
		  YYZ="`$(CXX) $(CCFLAGS) $(CPPFLAGS) \
		  -ptr$(VSHDIR)pt$* -ptr./ptrepository $<`"; \
		  $(RM) -rf a.out; \
		  YYZ="`echo $(VSHDIR)pt$*/*.o`"; \
		  $(SOLINK.cc) -o $@ ./$(VSHDIR)$*.o $(YYZ)

# - if test "`ls $(VSHDIR)pt$*/*.o 2>/dev/null`" != ""; \
#	then \
#	     $(SOLINK.cc) -o $@ ./$(VSHDIR)$*.o ./$(VSHDIR)pt$*/*.o; \
#	else \
#	     $(SOLINK.cc) -o $@ ./$(VSHDIR)$*.o; \
#	fi
