# Command line build environments
setenv WIND_HOST_TYPE sun4-solaris2
setenv WIND_BASE /usr/local/tornado2.2/ppc
setenv WIND_REGISTRY mizar
setenv PATH $WIND_BASE/host/$WIND_HOST_TYPE/bin:$PATH
if (! $?USER) then
    setenv USER $LOGNAME
endif
if ($?LD_LIBRARY_PATH) then
    setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:$WIND_BASE/host/$WIND_HOST_TYPE/lib:/usr/dt/lib:/usr/openwin/lib
else
    setenv LD_LIBRARY_PATH $WIND_BASE/host/$WIND_HOST_TYPE/lib:/usr/dt/lib:/usr/openwin/lib
endif
if ($?LM_LICENSE_FILE) then
    setenv LM_LICENSE_FILE /usr/local/tornado2.2/ppc/.wind/license/WRSLicense.lic:${LM_LICENSE_FILE}
else
    setenv LM_LICENSE_FILE /usr/local/tornado2.2/ppc/.wind/license/WRSLicense.lic
endif
