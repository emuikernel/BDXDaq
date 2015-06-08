#
# ET libsrc makefile
#

ifndef BMS_HOME
  $(error "Need to define BMS_HOME")
endif

ifeq ($(BMS_OS), vxworks)
  $(error "This makefile is for unix, use Makefile.vxworks-ppc")
endif

# Setting TOPLEVEL tells Makefile.common where to find Makefile.local
# and to include it if make is run from this dir or the top-level package
# directory.
TOPLEVEL = ../..

MODULE_NAME = et

# filter this out of the regular libraries
NO_OBJS = et_remoteclient.o

# Things common to all builds live in Makefile.common
include $(BMS_HOME)/Makefile.common


# objects for making a remote library
ROBJS = et_statconfig.o \
	et_openconfig.o \
	et_init.o \
	et_sysconfig.o \
	et_remote.o \
	et_network.o \
	et_remoteclient.o

RDOBJS = $(addsuffix $(DEBUG_SUFFIX).o,$(basename $(ROBJS)))

OTHER_TARGETS += $(LIB_DIR)/libet_remote$(DEBUG_SUFFIX).$(SHLIB_SUFFIX)

RLINK_OBJS = $(addprefix $(OBJ_DIR)/,$(RDOBJS))

# if you don't want either of these libs, comment out the definition(s)
MAKE_SHARED_LIB = make_me_please
MAKE_STATIC_LIB = make_me_please

# This new target must come AFTER the makefile include above or else it's the first target.
# The other stuff must come BEFORE since the included makefiles have a target defined.
$(LIB_DIR)/libet_remote$(DEBUG_SUFFIX).$(SHLIB_SUFFIX): $(RDOBJS)
	$(LD) $(LD_SO_FLAGS) $(LD_DIRS) $(RLINK_OBJS) $(LD_LIBS) -o $@

# Things for building libs
include $(BMS_HOME)/Makefile.libs

