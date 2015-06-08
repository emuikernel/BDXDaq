#
# et top level Makefile
#

MAKEFILE = Makefile

# if using vxworks, use different set of the lowest level makefiles
ifeq ($(BMS_OS), vxworks)
  ifdef ARCH
    MAKEFILE = Makefile.$(BMS_OS)-$(BMS_ARCH)
  else
    $(error "Need to define BMS_ARCH if using OS = vxworks")
  endif
endif

LOCAL_DIR = $(notdir $(shell pwd))

# define TOPLEVEL for use in making doxygen docs
TOPLEVEL = .

# list directories in which there are makefiles to be run (relative to this one)
SRC_DIRS = src/libsrc src/examples

# declaring a target phony skips the implicit rule search and saves time
.PHONY : first help java javaClean javaDistClean doc tar


first: all

help:
	@echo "make [option]"
	@echo "      env       - list env variables"
	@echo "      doc       - generate java doc for java ET"
	@echo "      tar       - create ET tar file"
	@echo "      mkdirs    - make necessary directories for C,C++"
	@echo "      install   - install all headers and compiled files for C,C++"
	@echo "      uninstall - uninstall all headers and compiled files for C,C++"
	@echo "      relink    - delete libs and executables, and relink object files"
	@echo "      clean     - delete all exec, library, object, and dependency files"
	@echo "      distClean - clean and remove hidden OS directory"
	@echo "      execClean - delete all exec and library files"
	@echo "      java      - make java files"
	@echo "      javaClean - remove java class files"
	@echo "      javaDistClean - remove java class files and generated documentation"

java:
	ant;

javaClean:
	ant clean;

javaDistClean:
	ant cleanall;

doc:
	ant javadoc;

tar:
	-$(RM) tar/et-11.0.tar.gz;
	tar -X tar/tarexclude -C .. -c -z -f tar/et-11.0.tar.gz $(LOCAL_DIR)

# Use this pattern rule for all other targets
%:
	@for i in $(SRC_DIRS); do \
	   $(MAKE) -C $$i -f $(MAKEFILE) $@; \
	done;
