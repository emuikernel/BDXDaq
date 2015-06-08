TOP = ..
include $(TOP)/configure/CONFIG
DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard *driverOld*))
DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard *iocShellOld*))
#DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard *db*))
#DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard *Db*))
include $(TOP)/configure/RULES_DIRS

 