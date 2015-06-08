# configGen.tcl - configuration file generator
#
# modification history
# --------------------
# 01a,12may98,ms    written
#
#
# DESCRIPTION
# This file generates the configuration files config.h, linkSyms.c, and
# sysConfig.c from a configuration descriptor file (.wcf).
#
# USAGE
#        wtxtcl configGen.tcl prjFile
#                prjFile	project file
#        RETURNS: An error string, which is empty on success
#        PRODUCES: files config.h, sysConfig.c, and linkSyms.c
#

# parse and check arguments

set usage "wtxtcl configGen.tcl prjFile"
if {$argc != 1} {
    error "wrong # args. Usage: $usage"
    }

# generate the configuration files

source [wtxPath]/host/resource/tcl/app-config/Project/prjLib.tcl
set prjFile [lindex $argv 0]
set hProj      [prjOpen $prjFile]
::prj_vxWorks_hidden::configGen $hProj
prjClose $hProj

