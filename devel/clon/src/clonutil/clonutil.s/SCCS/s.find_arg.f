h24708
s 00005/00003/00074
d D 1.2 96/10/30 16:10:28 wolin 3 1
c Forced exact match
e
s 00000/00000/00000
d R 1.2 96/09/26 17:36:43 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libutil/s/find_arg.f
c Name history : 1 0 s/find_arg.f
e
s 00077/00000/00000
d D 1.1 96/09/26 17:36:42 wolin 1 0
c For decoding command line args
e
u
U
f e 0
t
T
I 1
      integer function find_arg(arg)

c_begin_doc

c  Documentation for integer function find_arg(arg)

c  $Id: find_arg.F 1.3 1996/02/26 20:58:39 wolin Exp $

c  Purpose:
c  --------
c     searchs command line for arg, returns index, assumes leading "-"

c  Input:
c  ------
c     arg      arg to search for, assumes leading "-"

c  Output:
c  -------
c     return value is index of arg, 0 if not found

c  Author:
c  -------
c     Elliott Wolin, W&M, 7-jul-95

c  Major revisions:
c  ----------------
c   Arne Freyberg truncated string comparison to lenocc(arg)
c_end_doc


      implicit none
      save



c  input/output variables:
c  -----------------------
      character*(*) arg
      
c  local variables:
c  ----------------
      integer lenocc,arg_len,narg,iarg,iargc
      character*100 cmd_arg,temp
c_end_var


c  executable code:
c  ----------------

      find_arg=0

c  check for matching arg on command line
      narg=iargc()
      arg_len=lenocc(arg)
      if((narg.gt.0).and.(arg_len.gt.0))then

         do iarg=1,narg
            call getarg(iarg,cmd_arg)

            temp='-'//arg(1:arg_len)

D 3
c  add (1:arg_len+1) to the following test, this allows for trucation of
c  command line arguments
            if(cmd_arg(1:arg_len+1).eq.temp(1:lenocc(temp)))then
E 3
I 3
c  undo this temporarily...ejw
c   add (1:arg_len+1) to the following test, this allows for trucation of
c   command line arguments
            if(cmd_arg.eq.temp(1:lenocc(temp)))then
ccc            if(cmd_arg(1:arg_len+1).eq.temp(1:lenocc(temp)))then
E 3
               find_arg=iarg
               return
            endif

         enddo

      endif

      return
      end

c---------------------------------------------------------------------------------

E 1
