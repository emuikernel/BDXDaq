TOP=..
include $(TOP)/configure/CONFIG

TARGETS = $(CONFIG_TARGETS)
CONFIGS += $(subst ../,,$(wildcard $(CONFIG_INSTALLS)))

include $(TOP)/configure/RULES

#$(TOP)/configure/RELEASE_2_INCLUDE:
#	ln -s ${EPICS_SUPPORT}/RELEASE_2_INCLUDE

