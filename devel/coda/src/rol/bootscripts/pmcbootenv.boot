# pmcbootenv.boot
#
# Defines standard PMC env variables
#

# get CODA env vars from Unix
envGetRemote("EXPID",0,0)

# get CLON env vars from Unix
envGetRemote("CLON_PARMS",0,0)

# for mysql, coda, cmlog
##putenv "MYSQL_HOST=clondb1"
putenv "MYSQL_HOST=clondb1-daq1"
putenv "CMLOG_HOST=clon10"
putenv "CMLOG_PORT=8102"
putenv "TOKEN_PORT=5678"

# for raw data banks and scalers
putenv "RAW=FALSE"
putenv "SCALER=FALSE"

