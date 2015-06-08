# Command line build environments
WIND_HOST_TYPE=sun4-solaris2
export WIND_HOST_TYPE
WIND_BASE=/usr/local/tornado2.2/ppc
export WIND_BASE
WIND_REGISTRY=mizar
export WIND_REGISTRY
PATH=$WIND_BASE/host/$WIND_HOST_TYPE/bin:$PATH
export PATH
if [ "$USER" = "" ] ; then
    USER=$LOGNAME
fi
export USER
if [ "$LD_LIBRARY_PATH" = "" ] ; then
    LD_LIBRARY_PATH=$WIND_BASE/host/$WIND_HOST_TYPE/lib:/usr/dt/lib:/usr/openwin/lib
else
    LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$WIND_BASE/host/$WIND_HOST_TYPE/lib:/usr/dt/lib:/usr/openwin/lib
fi
export LD_LIBRARY_PATH
if [ "$LM_LICENSE_FILE" = "" ] ; then
    LM_LICENSE_FILE=/usr/local/tornado2.2/ppc/.wind/license/WRSLicense.lic
else
    LM_LICENSE_FILE=/usr/local/tornado2.2/ppc/.wind/license/WRSLicense.lic:$LM_LICENSE_FILE
fi
export LM_LICENSE_FILE
