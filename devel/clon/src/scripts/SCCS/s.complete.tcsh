h17799
s 00487/00000/00000
d D 1.1 06/07/01 09:29:23 boiarino 1 0
c date and time created 06/07/01 09:29:23 by boiarino
e
u
U
f e 0
t
T
I 1
#
# $Id: complete.tcsh,v 1.32 1999/06/09 19:09:12 christos Exp $
# adapted for Solaris 8 
# example file using the new completion code
#

onintr -
if (! $?prompt) goto end

echo "-"
echo "- start $CLON/common/scripts/complete.tcsh -"

if ($?tcsh) then
  if ($tcsh != 1) then
     set rev=$tcsh:r
    set rel=$rev:e
    set pat=$tcsh:e
    set rev=$rev:r
  endif
  if ($rev > 5 && $rel > 1) then
    set complete=1
  endif
  unset rev rel pat
endif

if ($?complete) then
  set noglob
  set hosts
  foreach f ($HOME/.hosts /usr/local/etc/csh.hosts $HOME/.rhosts /etc/hosts.equiv)
    if ( -r $f ) then
      set hosts = ($hosts `grep -v "+" $f | tr -s " " "	" | cut -f 1`)
    endif
  end
  if ( -r $HOME/.netrc ) then
    set f=`awk '/machine/ { print $2 }' < $HOME/.netrc` >& /dev/null
    set hosts=($hosts $f)
  endif
  unset f
  if ( ! $?hosts ) then
    set hosts=(mirror.aarnet.edu.au dra.com) 
#	set hosts=(hyperion.ee.cornell.edu phaeton.ee.cornell.edu)
  endif

  complete {cd,chdir,pushd,popd}  	p/1/d:^lost+found/ # Directories only
  complete complete 	p/1/X/		# Completions only
  complete uncomplete	n/*/X/
  complete exec 	p/1/c/		# Commands only
  complete {trace,strace,truss} 	p/1/c/
  complete {which,where}	n/*/c/
  complete set	'c/*=/f/' 'p/1/s/=' 'n/=/f/'
  complete unset	n/*/s/
  complete setenv	'p/1/e/' 'c/*:/f/'
  if ( -r /etc/shells ) then
    complete setenv	p@1@e@ n@DISPLAY@\$hosts@: \
      n@SHELL@'`cat /etc/shells`'@ 'c/*:/f/'
  else
    complete setenv	p@1@e@ n@DISPLAY@\$hosts@: 'c/*:/f/'
  endif
  complete unsetenv	n/*/e/
  complete printenv	'n/*/e/'
  complete alias 	p/1/a/		# only aliases are valid
  complete unalias	n/*/a/
  complete rm 	c/-/"(f i r R)"/ \
      n/*/f:^*.{c,cc,C,h,in}/	# Protect precious files
  complete vi 	n/*/f:^*.[oa]/
  complete ls c/-/"(a A b c C d f F g i l L m n o p q r R s t u x 1)"/ \
    p/*/f:^*lost+found*/
# ls still doesn't ignore lost+found !!!

  complete find 	n/-fstype/"(nfs 4.2)"/ n/-name/f/ \
    n/-type/"(c b d f p l s)"/ n/-user/u/ n/-group/g/ \
    n/-exec/c/ n/-ok/c/ n/-cpio/f/ n/-ncpio/f/ n/-newer/f/ \
    c/-/"(depth exec follow fstype group local ls \
    mount name newer nogroup nouser ok print prune type user xdev cpio ncpio \
    atime ctime mtime size inum links perm not a and o or)"/ n/*/d/

#     -atime n -ctime n -mtime n -size n[c] -inum n -links n -perm [-]mode
# gnu find
#    complete find 	n/-fstype/"(nfs 4.2)"/ n/-name/f/ \
#		  	n/-type/"(c b d f p l s)"/ n/-user/u/ n/-group/g/ \
#			n/-exec/c/ n/-ok/c/ n/-cpio/f/ n/-ncpio/f/ n/-newer/f/ \
#		  	c/-/"(fstype name perm prune type user nouser \
#		  	     group nogroup size inum atime mtime ctime exec \
#			     ok print ls cpio ncpio newer xdev depth \
#			     daystart follow maxdepth mindepth noleaf version \
#			     anewer cnewer amin cmin mmin true false uid gid \
#			     ilname iname ipath iregex links lname empty path \
#			     regex used xtype fprint fprint0 fprintf \
#			     print0 printf not a and o or)"/ \
#			     n/*/d/

  complete -%*	c/%/j/			# fill in the jobs builtin
  complete {fg,bg,stop}	c/%/j/ p/1/"(%)"//

  complete limit	c/-/"(h)"/ n/*/l/
  complete unlimit	c/-/"(h)"/ n/*/l/

  complete ywho  	n/*/\$hosts/	# argument from list in $hosts
  complete rsh p/1/\$hosts/ c/-/"(l n)"/ n/-l/u/ N/-l/c/ n/-/c/ p/2/c/ p/*/f/
  complete ssh p/1/\$hosts/ c/-/"(l n)"/ n/-l/u/ N/-l/c/ n/-/c/ p/2/c/ p/*/f/
  complete xrsh	p/1/\$hosts/ c/-/"(l 8 e)"/ n/-l/u/ N/-l/c/ n/-/c/ p/2/c/ p/*/f/
  complete rlogin 	p/1/\$hosts/ c/-/"(l 8 e)"/ n/-l/u/
  complete telnet 	p/1/\$hosts/ p/2/x:'<port>'/ n/*/n/
  complete finger	c/*@/\$hosts/ n/*/u/@ 
  complete ping	p/1/\$hosts/
  complete traceroute	p/1/\$hosts/
  complete nslookup   p/1/x:'<host>'/ p/2/\$hosts/
  complete {talk,ntalk,phone}	p/1/'`users | tr " " "\012" | uniq`'/ \
       n/*/\`who\ \|\ grep\ \$:1\ \|\ awk\ \'\{\ print\ \$2\ \}\'\`/

  complete ftp	c/-/"(d i g n v)"/ n/-/\$hosts/ p/1/\$hosts/ n/*/n/
  # this one is simple...
  #complete rcp c/*:/f/ C@[./\$~]*@f@ n/*/\$hosts/:
  # From Michael Schroeder <mlschroe@immd4.informatik.uni-erlangen.de> 
  # This one will rsh to the file to fetch the list of files!
  complete rcp 'c%*@*:%`set q=$:-0;set q="$q:s/@/ /";set q="$q:s/:/ /";set q=($q " ");rsh $q[2] -l $q[1] ls -dp $q[3]\*`%' 'c%*:%`set q=$:-0;set q="$q:s/:/ /";set q=($q " ");rsh $q[1] ls -dp $q[2]\*`%' 'c%*@%$hosts%:' 'C@[./$~]*@f@'  'n/*/$hosts/:'

  complete chown c/-/"(f h R)"/ C@[./\$~]@f@ c/*[:]/g/ \
     n/-/u/: p/1/u/: n/*/f/
  complete chgrp	c/-/"(f h R)"/ n/-/g/ p/1/g/ n/*/f/
  complete chmod	c/-/"(f R)"/ p/1/x:'<mode>'/ n/*/f/
  complete cat	c/-/"(b e n s t u v)"/ n/*/f/
  complete mv		c/-/"(f i)"/ n/*/f/
#  complete mv c/-/"(f i)"/ n/-/f/ N/-/d/ p/1/f/ p/2/d/ n/*/f/
  complete cp		c/-/"(f i p R r)"/ n/*/f/
#  complete cp c/-/"(f i p R r)"/ n/-*r/d/ n/-/f/ N/-/d/ p/1/f/ p/2/d/ n/*/f/
  complete ln		c/-/"(f n s)"/ n/-/f/ p/1/f/ 
  complete touch c/-/"(a c m r t -)"/ n/-r/f/ n/-t/x:'<time_stamp>'/ n/*/f/
  complete mkdir	c/-/"(p s m)"/ n/-m/x:'<mode>'/ n/*/d/
  complete rmdir	c/-/"(p s)"/ n/*/d/

  # these and interrupt handling from Jaap Vermeulen <jaap@sequent.com>
  complete {rexec,rxexec,rxterm,rmterm} \
     'p/1/$hosts/' 'c/-/(l L E)/' 'n/-l/u/' 'n/-L/f/' \
     'n/-E/e/' 'n/*/c/'
  complete kill	'c/-/S/' 'c/%/j/' \
     'n/*/`ps -u $LOGNAME | awk '"'"'{print $1}'"'"'`/'
  complete su c/-/"(c r)"/ n/-[cr]/c/ 

#  complete skill 	p/1/c/
  complete bindkey    N/-a/b/ N/-c/c/ n/-[ascr]/'x:<key-sequence>'/ \
    n/-[svedlr]/n/ c/-[vedl]/n/ c/-/"(a s k c v e d l r)"/\
    n/-k/"(left right up down)"/ p/2-/b/ \
    p/1/'x:<key-sequence or option>'/


###  complete -co*	p/0/"(compress)"/	# make compress completion
###            # not ambiguous

  complete dd c/--/"(help version)"/ c/[io]f=/f/ \
    c/conv=*,/"(ascii ebcdic ibm block unblock \
      lcase notrunc ucase swab noerror sync)"/,\
    c/conv=/"(ascii ebcdic ibm block unblock \
      lcase notrunc ucase swab noerror sync)"/,\
      c/*=/x:'<number>'/ \
    n/*/"(if of conv ibs obs bs cbs files skip file seek count)"/=

  complete gs 'c/-sDEVICE=/(x11 cdjmono cdj550 epson eps9high epsonc \
            dfaxhigh dfaxlow laserjet ljet4 sparc pbm \
            pbmraw pgm pgmraw ppm ppmraw bit)/' \
    'c/-sOutputFile=/f/' 'c/-s/(DEVICE OutputFile)/=' \
    'c/-d/(NODISPLAY NOPLATFONTS NOPAUSE)/' 'n/*/f/'
  complete perl	'n/-S/c/'
  complete sccs	p/1/"(admin cdc check clean comb deledit delget \
      delta diffs edit enter fix get help info \
      print prs prt rmdel sccsdiff tell unedit \
      unget val what)"/

  complete man      c/-/"(a d F l r t M T s k)"/ n/-M/d/ n/-f/c/ \
    n/-s/"(1 2 3 4 5 6 7 8 9 0 n o l p)"/ n/-k/x:'<keyword>'/ n/*/c/

#  complete man	    n@1@'`\ls -1 /usr/man/man1 | sed s%\\.1.\*\$%%`'@ \
#	    n@2@'`\ls -1 /usr/man/man2 | sed s%\\.2.\*\$%%`'@ \
#	    n@3@'`\ls -1 /usr/man/man3 | sed s%\\.3.\*\$%%`'@ \
#	    n@4@'`\ls -1 /usr/man/man4 | sed s%\\.4.\*\$%%`'@ \
#	    n@5@'`\ls -1 /usr/man/man5 | sed s%\\.5.\*\$%%`'@ \
#	    n@6@'`\ls -1 /usr/man/man6 | sed s%\\.6.\*\$%%`'@ \
#	    n@7@'`\ls -1 /usr/man/man7 | sed s%\\.7.\*\$%%`'@ \
#	    n@8@'`\ls -1 /usr/man/man8 | sed s%\\.8.\*\$%%`'@ \
#    n@9@'`[ -r /usr/man/man9 ] && \ls -1 /usr/man/man9 | sed s%\\.9.\*\$%%`'@ \
#    n@0@'`[ -r /usr/man/man0 ] && \ls -1 /usr/man/man0 | sed s%\\.0.\*\$%%`'@ \
#  n@new@'`[ -r /usr/man/mann ] && \ls -1 /usr/man/mann | sed s%\\.n.\*\$%%`'@ \
#  n@old@'`[ -r /usr/man/mano ] && \ls -1 /usr/man/mano | sed s%\\.o.\*\$%%`'@ \
#n@local@'`[ -r /usr/man/manl ] && \ls -1 /usr/man/manl | sed s%\\.l.\*\$%%`'@ \
#n@public@'`[ -r /usr/man/manp ]&& \ls -1 /usr/man/manp | sed s%\\.p.\*\$%%`'@ \
#		c/-/"(- f k M P s t)"/ n/-f/c/ n/-k/x:'<keyword>'/ n/-[MP]/d/ \
#		N@-[MP]@'`\ls -1 $:-1/man? | sed s%\\..\*\$%%`'@ n/*/c/

#   ps [ -aAcdefjlLPy ]  [ -g grplist ]   [  -n namelist  ]    [
#     -o format ]  ...  [ -p proclist ]  [ -s sidlist ]  [ -t term
#     ]  [ -u uidlist ]  [ -U uidlist ]  [ -G gidlist ]
# SVR4 style
  complete ps	 c/-/"(a A c d e f g G j l L n p P s t u U y)"/ \
     'n@-t@pts/`ls /dev/pts`@' n/-[uU]/u/ n/-[gG]/g/ 

#  complete ps        c/-t/x:'<tty>'/ c/-/"(a c C e g k l S t u v w x)"/ \
#			n/-k/x:'<kernel>'/ N/-k/x:'<core_file>'/ n/*/x:'<PID>'/
#
  complete xhost	c/[+-]/\$hosts/ n/*/\$hosts/

  complete compress	c/-/"(c f v b)"/ n/-b/x:'<max_bits>'/ n/*/f:^*.Z/
  complete uncompress	c/-/"(c f v)"/                        n/*/f:*.Z/
  complete zcat	n/*/f:*.{Z,z,gzi,tgz,taz}/
  complete gzcat	c/--/"(force help license quiet version)"/ \
     c/-/"(f h L q V -)"/ n/*/f:*.{gz,Z,z,zip}/
  complete gzip	c/--/"(stdout to-stdout decompress uncompress \
       force help list license no-name quiet recurse \
       suffix test verbose version fast best)"/ \
     c/-/"(c d f h l L n q r S t v V 1 2 3 4 5 6 7 8 9 -)"/\
     n/{-S,--suffix}/x:'<file_name_suffix>'/ \
     n/{-d,--{de,un}compress}/f:*.{gz,Z,z,zip,taz,tgz}/ \
     N/{-d,--{de,un}compress}/f:*.{gz,Z,z,zip,taz,tgz}/ \
     n/*/f:^*.{gz,Z,z,zip,taz,tgz}/
  complete {gunzip,ungzip} c/--/"(stdout to-stdout force help list license \
       no-name quiet recurse suffix test verbose version)"/ \
     c/-/"(c f h l L n q r S t v V -)"/ \
     n/{-S,--suffix}/x:'<file_name_suffix>'/ \
     n/*/f:*.{gz,Z,z,zip,taz,tgz}/
  complete zgrep	c/-*A/x:'<#_lines_after>'/ c/-*B/x:'<#_lines_before>'/\
     c/-/"(A b B c C e f h i l n s v V w x)"/ \
     p/1/x:'<limited_regular_expression>'/ N/-*e/f/ \
     n/-*e/x:'<limited_regular_expression>'/ n/-*f/f/ n/*/f/
  complete zegrep	c/-*A/x:'<#_lines_after>'/ c/-*B/x:'<#_lines_before>'/\
     c/-/"(A b B c C e f h i l n s v V w x)"/ \
    p/1/x:'<full_regular_expression>'/ N/-*e/f/ \
    n/-*e/x:'<full_regular_expression>'/ n/-*f/f/ n/*/f/
  complete zfgrep	c/-*A/x:'<#_lines_after>'/ c/-*B/x:'<#_lines_before>'/\
      c/-/"(A b B c C e f h i l n s v V w x)"/ \
      p/1/x:'<fixed_string>'/ N/-*e/f/ \
      n/-*e/x:'<fixed_string>'/ n/-*f/f/ n/*/f/
  complete znew	c/-/"(f t v 9 P K)"/ n/*/f:*.Z/
  complete zmore	n/*/f:*.{gz,Z,z,zip}/
  complete zfile	n/*/f:*.{gz,Z,z,zip,taz,tgz}/
  complete ztouch	n/*/f:*.{gz,Z,z,zip,taz,tgz}/
  complete zforce	n/*/f:^*.{gz,tgz}/

  complete {grep,egrep,fgrep}	c/-/"(b c E e F h i l n q s v w)"/ \
      p/1/x:'<limited_regular_expression>'/ N/-*e/f/ \
      n/-*e/x:'<limited_regular_expression>'/ n/-*f/f/ n/*/f/

  complete users	p/1/x:'<accounting_file>'/
#    complete users	c/--/"(help version)"/ p/1/x:'<accounting_file>'/
#     /usr/bin/who [ -abdHlmpqrstTu ]  [ file ]
#     /usr/bin/who -q  [ -n x ]  [ file ]
#     /usr/bin/who am  i
#     /usr/bin/who am  I

  complete who	c/-/"(a b d H l m n p q r s t T u)"/ \
                        c/a/"(m)"/ p/1/x:'<accounting_file>'/ n/am/"(i)"/ 

  complete df		c/-/"(a b e g k l n  o t V)"/
  complete du		c/-/"(a d k L o r s x)"/ n/*/f/

#     tar c  [  bBeEfFhiklnopPqvwX  [ 0-7 ]  ]  [ block ]  [  tar-
#         file  ]   [ exclude-file ]  { -I include-file | -C directory
#         | file  | file }  ...
#     tar r   [   bBeEfFhiklnqvw   [  0-7  ]   ]   [  block  ]   {
#         -I include-file | -C directory  | file  | file }  ...
#     tar t  [  BefFhiklnqvX  [ 0-7 ]  ]  [ tarfile ]  [  exclude-
#         file ]  { -I include-file | file }  ...
#     tar u  [  bBeEfFhiklnqvw  [ 0-7 ]  ]  [ block ]  [ tarfile ]
#         file ...
#     tar x  [  BefFhiklmnopqvwX  [ 0-7  ]   ]   [  tarfile  ]   [
#         exclude-file ]  [ file ... ]
   complete tar c/-[crtux]*/"(b B e E f F h i k l n o p P q v w X)"/ \
    n/-c*f/x:'<new_tar_file, device_file, or "-">'/ \
    n/-[Adrtux]*f/f:*.tar/ \
    N/-x*f/'`tar -tf $:-1`'/ 
        
#gnu tar
#    complete tar	c/-[Acru]*/"(b B C f F g G h i l L M N o P \
#			R S T v V w W X z Z)"/ \
#			c/-[dtx]*/"( B C f F g G i k K m M O p P \
#			R s S T v w x X z Z)"/ \
#			p/1/"(A c d r t u x -A -c -d -r -t -u -x \
#			--catenate --concatenate --create --diff --compare \
#			--delete --append --list --update --extract --get \
#			--help --version)"/ \
#			c/--/"(catenate concatenate create diff compare \
#			delete append list update extract get atime-preserve \
#			block-size read-full-blocks directory checkpoint file \
#			force-local info-script new-volume-script incremental \
#			listed-incremental dereference ignore-zeros \
#			ignore-failed-read keep-old-files starting-file \
#			one-file-system tape-length modification-time \
#			multi-volume after-date newer old-archive portability \
#			to-stdout same-permissions preserve-permissions \
#			absolute-paths preserve record-number remove-files \
#			same-order preserve-order same-owner sparse \
#			files-from null totals verbose label version \
#			interactive confirmation verify exclude exclude-from \
#			compress uncompress gzip ungzip use-compress-program \
#			block-compress help version)"/ \
#			c/-/"(b B C f F g G h i k K l L m M N o O p P R s S \
#			T v V w W X z Z 0 1 2 3 4 5 6 7 -)"/ \
#			n/-c*f/x:'<new_tar_file, device_file, or "-">'/ \
#			n/{-[Adrtux]*f,--file}/f:*.tar/ \
#			N/{-x*f,--file}/'`tar -tf $:-1`'/ \
#			n/--use-compress-program/c/ \
#			n/{-b,--block-size}/x:'<block_size>'/ \
#			n/{-V,--label}/x:'<volume_label>'/ \
#			n/{-N,--{after-date,newer}}/x:'<date>'/ \
#			n/{-L,--tape-length}/x:'<tape_length_in_kB>'/ \
#			n/{-C,--directory}/d/ \
#			N/{-C,--directory}/'`\ls $:-1`'/ \
#			n/-[0-7]/"(l m h)"/

    # SVR4 filesystems
  complete  mount	c/-/"(a F m o O p r v V)"/ n/-p/n/ n/-v/n/ \
    n/-o/x:'<FSType_options>'/ \
    n@-F@'`\ls -1 /usr/lib/fs`'@ \
    n@*@'`grep -v "^#" /etc/vfstab | tr -s " " "	 " | cut -f 3`'@
  complete umount	c/-/"(a o V)"/ n/-o/x:'<FSType_options>'/ \
    n/*/'`mount | cut -d " " -f 1`'/
  complete  mountall	c/-/"(F l r)"/ n@-F@'`\ls -1 /usr/lib/fs`'@
  complete umountall	c/-/"(F h k l r s)"/ n@-F@'`\ls -1 /usr/lib/fs`'@ \
    n/-h/'`df -k | cut -s -d ":" -f 1 | sort -u`'/
  # BSD 4.3 filesystems
  #complete  mount	c/-/"(a r t v)"/ n/-t/"(4.2 nfs)"/ \
  #	n@*@'`grep -v "^#" /etc/fstab | tr -s " " "	" | cut -f 2`'@
  #complete umount	c/-/"(a h t v)"/ n/-t/"(4.2 nfs)"/ \
  #	n/-h/'`df | cut -s -d ":" -f 1 | sort -u`'/ \
  #	n/*/'`mount | cut -d " " -f 3`'/
  # BSD 4.2 filesystems
  #complete  mount	c/-/"(a r t v)"/ n/-t/"(ufs nfs)"/ \
  #			n@*@'`cut -d ":" -f 2 /etc/fstab`'@
  #complete umount	c/-/"(a h t v)"/ n/-t/"(ufs nfs)"/ \
  #			n/-h/'`df | cut -s -d ":" -f 1 | sort -u`'/ \
  #			n/*/'`mount | cut -d " " -f 3`'/

    # these from Tom Warzeka <waz@quahog.npt.nuwc.navy.mil>
    # you may need to set the following variables for your host
  set _elispdir = /usr/local/share/emacs/20.2/lisp # GNU Emacs lisp directory
  set _maildir = /var/spool/mail  # Post Office: /var/spool/mail or /usr/mail
  set _ypdir  = /var/yp	# directory where NIS (YP) maps are kept
  set _domain = "`domainname`"

#  if (-r $HOME/.mailrc) then
#    complete mail	c/-/"(e i f n s u v)"/ c/*@/\$hosts/ \
#			c@+@F:$HOME/Mail@ C@[./\$~]@f@ n/-s/x:'<subject>'/ \
#			n@-u@T:$_maildir@ n/-f/f/ \
#			n@*@'`sed -n s/alias//p $HOME/.mailrc | tr -s " " "	" | cut -f 2`'@
#  else
#    complete mail	c/-/"(e i f n s u v)"/ c/*@/\$hosts/ \
#			c@+@F:$HOME/Mail@ C@[./\$~]@f@ n/-s/x:'<subject>'/ \
#			n@-u@T:$_maildir@ n/-f/f/ n/*/u/
#  endif

    # these conform to the latest GNU versions available at press time ...
    # updates by John Gotts <jgotts@engin.umich.edu>

  complete emacs	c/-/"(batch d f funcall i insert kill l load \
       no-init-file nw q t u user)"/ c/+/x:'<line_number>'/ \
       n/-d/x:'<display>'/ n/-f/x:'<lisp_function>'/ n/-i/f/ \
       n@-l@F:$_elispdir@ n/-t/x:'<terminal>'/ \
       n/-u/u/ n/*/f:^*[\#~]/

  # these deal with NIS (formerly YP); if it's not running you don't need 'em
#  complete domainname	p@1@D:$_ypdir@" " n@*@n@
#  complete ypcat	c@-@"(d k t x)"@ n@-x@n@ n@-d@D:$_ypdir@" " \
#	    N@-d@\`\\ls\ -1\ $_ypdir/\$:-1\ \|\ sed\ -n\ s%\\\\.pag\\\$%%p\`@ \
#	  n@*@\`\\ls\ -1\ $_ypdir/$_domain\ \|\ sed\ -n\ s%\\\\.pag\\\$%%p\`@
#  complete ypmatch	c@-@"(d k t x)"@ n@-x@n@ n@-d@D:$_ypdir@" " \
#   N@-d@x:'<key ...>'@ n@-@x:'<key ...>'@ p@1@x:'<key ...>'@ \
#	  n@*@\`\\ls\ -1\ $_ypdir/$_domain\ \|\ sed\ -n\ s%\\\\.pag\\\$%%p\`@
#  complete ypwhich	c@-@"(d m t x V1 V2)"@ n@-x@n@ n@-d@D:$_ypdir@" " \
#	 n@-m@\`\\ls\ -1\ $_ypdir/$_domain\ \|\ sed\ -n\ s%\\\\.pag\\\$%%p\`@ \
#			N@-m@n@ n@*@\$hosts@

    # there's no need to clutter the user's shell with these
  unset _elispdir _maildir _ypdir _domain


  if ( ! $?printers ) set printers=""
  if ( -f /etc/printcap ) then
    set printers=(`sed -n -e "/^[^     #].*:/s/:.*//p" /etc/printcap`)
  else
    if (-X lpstat) then
#      set printers=(`lpstat -a|awk '{print $1}'`)
      set printers=`printenv LPDEST`
    endif
  endif
 
  if ( "x$printers" != "x" ) then
#     /usr/ucb/lpr [ -P destination ]  [ -# number ]  [ -C class ]
#     [ -J job ]  [ -T title ]  [  -i  [ indent ]  ]  [ -1 | -2  |
#     -3  | -4 font ]  [ -w cols ]  [ -m ]  [ -h  ]   [  -s  ]   [
#     -filter_option ]  [ file ... ]

    complete lpr    'c/-P/$printers/' 'n/-P/$printers/' \
      n/-T/x:'<title>'/ 'n/-#/x:"<number>"/' \
      n/-C/x:'<class>'/ n/-i/x:'<indent>'/ \
      n/-[1234]/x:'<font>'/ n/-w/x:'<columns>'/ \
      'c/-/(1 2 3 4 C h i J m P s T w #)/'
    complete lpq    'c/-P/$printers/'
    complete lprm   'c/-P/$printers/'
    complete lpquota        'p/1/(-Qprlogger)/' 'c/-P/$printers/'
    complete dvips  'c/-P/$printers/' 'n/-o/f:*.{ps,PS}/' 'n/*/f:*.dvi/'

    complete lp    'c/-d/$printers/'
    complete cancel c/-/"(u)"/ n/-u/u/ 'n/*/$printers/'
  endif


#  complete dde	p/1/c/ 
  complete adb	c/-I/d/ n/-/c/ N/-/"(core)"/ p/1/c/ p/2/"(core)"/
#  complete sdb	p/1/c/
#  complete dbx	c/-I/d/ n/-/c/ N/-/"(core)"/ p/1/c/ p/2/"(core)"/
#  complete xdb	p/1/c/
#  complete gdb	n/-d/d/ n/*/c/
#  complete ups	p/1/c/
  complete xdvi 	n/*/f:*.dvi/	# Only files that match *.dvi
#  complete dvips 	n/*/f:*.dvi/
  complete tex 	n/*/f:*.tex/	# Only files that match *.tex
  complete latex 	n/*/f:*.{tex,ltx}/
  complete cc 	c/-[IL]/d/ \
      c@-l@'`\ls -1 /usr/lib/lib*.a | sed s%^.\*/lib%%\;s%\\.a\$%%`'@ \
     c/-/"(o l c g L I D U)"/ n/*/f:*.[coasi]/
  complete acc 	c/-[IL]/d/ \
     c@-l@'`\ls -1 /usr/lang/SC1.0/lib*.a | sed s%^.\*/lib%%\;s%\\.a\$%%`'@ \
     c/-/"(o l c g L I D U)"/ n/*/f:*.[coasi]/
  complete gcc 	c/-[IL]/d/ \
    c/-f/"(caller-saves cse-follow-jumps delayed-branch \
           elide-constructors expensive-optimizations \
          float-store force-addr force-mem inline \
          inline-functions keep-inline-functions \
          memoize-lookups no-default-inline \
          no-defer-pop no-function-cse omit-frame-pointer \
          rerun-cse-after-loop schedule-insns \
          schedule-insns2 strength-reduce \
          thread-jumps unroll-all-loops \
         unroll-loops syntax-only all-virtual \
         cond-mismatch dollars-in-identifiers \
         enum-int-equiv no-asm no-builtin \
         no-strict-prototype signed-bitfields \
         signed-char this-is-variable unsigned-bitfields \
         unsigned-char writable-strings call-saved-reg \
         call-used-reg fixed-reg no-common \
         no-gnu-binutils nonnull-objects \
         pcc-struct-return pic PIC shared-data \
         short-enums short-double volatile)"/ \
     c/-W/"(all aggregate-return cast-align cast-qual \
           comment conversion enum-clash error format \
           id-clash-len implicit missing-prototypes \
           no-parentheses pointer-arith return-type shadow \
           strict-prototypes switch uninitialized unused \
           write-strings)"/ \
     c/-m/"(68000 68020 68881 bitfield fpa nobitfield rtd \
           short c68000 c68020 soft-float g gnu unix fpu \
           no-epilogue)"/ \
     c/-d/"(D M N)"/ \
     c/-/"(f W vspec v vpath ansi traditional \
          traditional-cpp trigraphs pedantic x o l c g L \
          I D U O O2 C E H B b V M MD MM i dynamic \
          nodtdlib static nostdinc undef)"/ \
     c/-l/f:*.a/ \
       n/*/f:*.{c,C,cc,o,a,s,i}/
  complete g++ 	n/*/f:*.{C,cc,o,s,i}/
  complete CC 	n/*/f:*.{C,cc,o,s,i}/

  complete make \
    'n/-f/f/' 'c/*=/f/' \
    'n@*@`cat -s GNUmakefile Makefile makefile |& sed -n -e "/No such file/d" -e "/^[^     #].*:/s/:.*//p"`@'

  complete nm		n/*/f:^*.{h,C,c,cc}/
  complete ar c/[dmpqrtx]/"(c l o u v a b i)"/ p/1/"(d m p q r t x)"// \
    p/2/f:*.a/ p/*/f:*.o/

    # these from E. Jay Berkenbilt <ejb@ERA.COM>
    # = isn't always followed by a filename or a path anymore - jgotts
  complete ./configure 'c/--*=/f/' \
   'c/--{cache-file,prefix,exec-prefix,bindir,sbindir,libexecdir,datadir,sysconfdir,sharedstatedir,localstatedir,libdir,includedir,oldincludedir,infodir,mandir,srcdir}/(=)//' \
   'c/--/(cache-file verbose prefix exec-prefix bindir \
       sbindir libexecdir datadir sysconfdir \
      sharedstatedir localstatedir libdir \
      includedir oldincludedir infodir mandir \
      srcdir)//'

  unset noglob
  unset complete
endif

echo "- finish $CLON/common/scripts/complete.tcsh -"
echo "-"

end:
  onintr
E 1
