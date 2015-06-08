#
# Makefile for cg graphics
#
#include ../../../Makefile.include

SHAPE2D_DIR = ../shape2d

ifeq ("$(OSTYPE)","SunOS")

CXX = CC
CC = cc
CXXFLAGS = -xO5 -KPIC -mt -I. -I/usr/openwin/include \
	-I/usr/openwin/include/X11 \
	-I/usr/dt/include -I/usr/local/include -I$(SHAPE2D_DIR) \
	-DMOTIF -DDEBUGGING  \
	-DCACHE_BITMAPS -DCACHE_SIZE_LIMIT=300 -DCACHE_FID

endif

ifeq ("$(OSTYPE)","Linux")

CXX = g++
CC = gcc

CXXFLAGS = -O -I. -I/usr/include/X11 -I/usr/X11R6/include\
	-I$(SHAPE2D_DIR) \
	-DMOTIF -DDEBUG \
	-DCACHE_BITMAPS -DCACHE_SIZE_LIMIT=300 -DCACHE_FID \
	-DNeedFunctionProtoTypes
endif


CFLAGS = $(CXXFLAGS)


XLIBS = -R/usr/openwin/lib:/usr/dt/lib \
	-L/usr/dt/lib -L/usr/openwin/lib -L/usr/local/lib -lXpm -lXm -lXmu -lXt -lX11 -lm

OBJS =  cgDispDev.o cgColorCxt.o cgContext.o cgXdrawing.o \
	cgPanel.o cgXpm.o \
	AppPlusS.o SubPlusS.o rotated.o

SHAPE2DOBJS = $(SHAPE2D_DIR)/cgPrim.o \
	$(SHAPE2D_DIR)/cgElist.o \
	$(SHAPE2D_DIR)/cgCircle.o \
	$(SHAPE2D_DIR)/cgArc.o \
	$(SHAPE2D_DIR)/cgPieArc.o \
	$(SHAPE2D_DIR)/cgConvex.o \
	$(SHAPE2D_DIR)/cgEdge.o \
	$(SHAPE2D_DIR)/cgGlist.o \
	$(SHAPE2D_DIR)/cgLine.o \
	$(SHAPE2D_DIR)/cgPattern.o \
	$(SHAPE2D_DIR)/cgPoint.o \
	$(SHAPE2D_DIR)/cgPolygon.o \
	$(SHAPE2D_DIR)/cgScene.o \
	$(SHAPE2D_DIR)/cgVertex.o \
	$(SHAPE2D_DIR)/cgWinView.o \
	$(SHAPE2D_DIR)/cgRec.o \
	$(SHAPE2D_DIR)/cgFrameRec.o \
	$(SHAPE2D_DIR)/cgGauge.o \
	$(SHAPE2D_DIR)/cgString.o \
	$(SHAPE2D_DIR)/cgXpmImage.o \
	$(SHAPE2D_DIR)/cgVmeCrate.o \
	$(SHAPE2D_DIR)/cgComputer.o \
	$(SHAPE2D_DIR)/cgTapeDrive.o \
	$(SHAPE2D_DIR)/cgTrashCan.o \
	$(SHAPE2D_DIR)/cg2DRtPlot.o \
	$(SHAPE2D_DIR)/cgRotString.o	

.c.o:
	rm -f $@
	$(CC)  $(CFLAGS) -c $< -o $@

.cc.o:
	rm -f $@
	$(CXX)  $(CXXFLAGS) -c $< -o $@

all: libcg.a

cgtest: main.o $(OBJS)
	rm -f $@
	$(CXX) -o $@ main.o $(OBJS) $(SHAPE2DOBJS) $(XLIBS)

libcg.a: $(OBJS) $(SHAPE2DOBJS)
	rm -f $@
	ar ruv $@ $(SHAPE2DOBJS) $(OBJS)
	ranlib $@

appsTest: AppPlusST.o AppPlusS.o SubPlusS.o XCC.o
	rm -f $@
	$(CC) -o $@ AppPlusST.o AppPlusS.o SubPlusS.o XCC.o $(XLIBS)

clean:
	rm -f *.o cgtest core *~ *.a
	rm -f ../shape2d/*.o
