# ultrix 4.4 with gnu c++ 2.6
CC 		= cc
CXX		= c++
DLD		= 
PIC		=
RANLIB 		= ranlib
LDFLAGS		= -L$(WRAPPER_ROOT)/lib
CCFLAGS		= -I$(WRAPPER_ROOT)/include -I/usr/local/gnu/g++include -DRLIMIT_NOFILE -DULTRIX -DHAS_NO_DLD
SOFLAGS		= -b
SOBUILD		= 
