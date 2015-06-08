TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#----------------------------------------------------
#  Optimization of db files using dbst (DEFAULT: NO)
#DB_OPT = YES

#----------------------------------------------------
# Create and install (or just install)
# databases, templates, substitutions like this

#DB += dbExample1.db
#DB += dbExample2.db
#DB += dbSubExample.db
#DB += user.substitutions
#DB += userHost.substitutions

MOTOR_DB_LOC=$(MOTOR)/db

DB += $(MOTOR_DB_LOC)/motor.db
DB += $(MOTOR_DB_LOC)/asyn_motor.db 
DB += $(MOTOR_DB_LOC)/basic_motor.db
DB += $(MOTOR_DB_LOC)/basic_asyn_motor.db
DB += $(MOTOR_DB_LOC)/motorUtil.db

DB += omsMotor.substitutions

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

