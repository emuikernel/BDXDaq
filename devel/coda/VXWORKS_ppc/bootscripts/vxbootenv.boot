# vxbootenv.boot
#
# Defines standard ROC env variables
#

# get CODA env vars from Unix
envGetRemote("EXPID",0,0)

# get CLON env vars from Unix
envGetRemote("CLON_PARMS",0,0)

# for mysql, coda, cmlog
#notgoodforiocon167putenv "MYSQL_HOST=clondb1-daq1"
putenv "MYSQL_HOST=jdaq4"
putenv "CMLOG_HOST=clon10"
putenv "CMLOG_PORT=8102"
putenv "TOKEN_PORT=5678"

# for raw data banks and scalers
putenv "RAW=FALSE"
putenv "SCALER=FALSE"


# MV processors with one IP port
putenv "dccntrl=dccntrl"
putenv "camac1=camac1"
putenv "camac2=camac2"
putenv "camac3=camac3"
putenv "pretrig1=pretrig1"
putenv "pretrig2=pretrig2"

 
# CLAS processors

putenv "sc1=sc1"
putenv "cc1=cc1"
putenv "cc2=cc2"
putenv "ec1=ec1"
putenv "ec2=ec2"
putenv "lac1=lac1"
putenv "tage=tage"
putenv "tlv1=tlv1"

putenv "dc1=dc1"
putenv "dc2=dc2"
putenv "dc3=dc3"
putenv "dc4=dc4"
putenv "dc5=dc5"
putenv "dc6=dc6"
putenv "dc7=dc7"
putenv "dc8=dc8"
putenv "dc9=dc9"
putenv "dc10=dc10"
putenv "dc11=dc11"

putenv "scaler1=scaler1"
putenv "scaler2=scaler2"
putenv "scaler3=scaler3"
putenv "scaler4=scaler4"
putenv "polar=polar"

# test processors
putenv "croctest1=croctest1"
putenv "croctest2=croctest2"
putenv "croctest3=croctest3"
putenv "croctest4=croctest4"
putenv "croctest5=croctest5"

# primex
#putenv "primexroc1=primexroc1"
#putenv "primexroc2=primexroc2"
#putenv "primexts=primexts"

#


