# SunOS 5.x (Solaris 2.x) with SunC++ 4.x
CC		= cc
CXX		= CC
DLD		= $(CXX)
LDFLAGS		+= -R $(WRAPPER_ROOT)/lib
LIBS		= -lsocket -ldl -lthread -lnsl
PIC		= -pic
RANLIB		= echo
SOFLAGS		= -G $(CPPFLAGS) $(PTDIRS)
SOBUILD		= $(COMPILE.cc) $(PIC) -o $(VSHDIR)$*.o $<; \
		  $(SOLINK.cc) -o $@ -h $@ $(LDFLAGS) $(VSHDIR)$*.o
