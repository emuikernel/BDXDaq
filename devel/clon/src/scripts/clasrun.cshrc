# @(#)Cshrc 1.6 91/09/05 SMI

#  .cshrc for clasrun account
#
#  do not modify this file without permission from the online group!
#

# define CLON software location
setenv CLON /usr/local/clas/release/current

if (($HOST == clon10)||($HOST == clon00)||($HOST == clon01)||($HOST == clon02)||($HOST == clon03)||($HOST == clon04)||($HOST == clon05)||($HOST == clon06)) then

#define PATH
  setenv PATH /bin
  setenv PATH "$PATH":/usr/bin
  setenv PATH "$PATH":/usr/sbin
  setenv PATH "$PATH":/opt/SUNWspro/bin
  setenv PATH "$PATH":/usr/ucb
  setenv PATH "$PATH":/usr/ccs/bin
  setenv PATH "$PATH":/usr/local/bin
  setenv PATH "$PATH":/apps/bin
  setenv PATH "$PATH":/usr/openwin/bin
  setenv PATH "$PATH":/usr/local/system
#mark's staff
  setenv PATH "$PATH":/usr/local/clas/etc/dab

#define LD_LIBRARY_PATH
  setenv LD_LIBRARY_PATH /usr/dt/lib
  setenv LD_LIBRARY_PATH "$LD_LIBRARY_PATH":/usr/openwin/lib
  setenv LD_LIBRARY_PATH "$LD_LIBRARY_PATH":/usr/lib
  setenv LD_LIBRARY_PATH "$LD_LIBRARY_PATH":/opt/SUNWspro/lib

#setup mysql
  setenv MYSQL /usr/local/clas/offline/mysql
  setenv LD_LIBRARY_PATH "$LD_LIBRARY_PATH":$MYSQL/lib/mysql


#setup smartsockets
  source /apps/smartsockets/ss65/bin/rtinit.csh

#setup ingres
  setenv ING_SET "set lockmode session where readlock=nolock"
  setenv TERM_INGRES vt100fx
  source /apps/ingres/.setup

endif

source $CLON/.setup

setenv EXPID clasrun
setenv SESSION clasprod

setenv PATH .:"$PATH"

# do we need it ? here ?
setenv RECON_HIST_DIR /work/hist/recon
alias xt "xterm -sb -sl 5000 -ls"
alias ll "ls -al"

#  aliases that can't be in .alias or .login...ejw
alias account "/apps/account/account"
alias onlb "online_buttons -a clasprod -session clasprod >& /dev/null &"

## replaced by script
##alias reset_all_hist 'tclipc -a clasprod -c "reset 0" mon_clasprod'
##alias reset_hist     'tclipc -a clasprod -c "reset 0" mon_clasprod'

# very important for shared files...ejw
umask 002

# default printer in counting house...ejw
setenv LPDEST clonhp

# misc tcsh stuff...ejw
set prompt="%m:%C> "
set rmstar
set history=1000
set savehist=1000
set autologout=0
unset autologout
set ignoreeof
set notify
set autolist


# for editor...ejw
setenv EDITOR 'emacs'
setenv VERSION_CONTROL t

#
#cernlib
#
  setenv CERN /usr/local/clas/cern_SunOS_sun4u
  setenv CERN_LEVEL 2005
  setenv CERN_ROOT /usr/local/clas/cern_SunOS_sun4u/2005
  setenv LAPACKDIR /usr/local/clas/cern_SunOS_sun4u/LAPACK
  setenv CVSCOSRC $CERN_ROOT/src
  setenv ROOTSYS /usr/local/clas/root_SunOS_sun4u
  setenv PATH ${CERN_ROOT}/bin:${PATH}

#
#ROOT
#
  setenv PATH ${ROOTSYS}/bin:${PATH}
  setenv LD_LIBRARY_PATH ${ROOTSYS}/lib:${LD_LIBRARY_PATH}
#
#
#







