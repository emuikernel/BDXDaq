h33324
s 00000/00000/00000
d R 1.2 97/07/29 10:44:38 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 libutil/s/get_next_line_.f
e
s 00053/00000/00000
d D 1.1 97/07/29 10:44:37 wolin 1 0
c Fortran versions of c++ routines
e
u
U
f e 0
t
T
I 1
      logical function get_next_line(unit,buffer)

c  fortran version of get_next_line.cc

c  ejw, 29-jul-97



      implicit none

      integer unit
      character *(*) buffer

      integer iostat,ind


c  executable code
c  ---------------


c  return next non-comment line
      get_next_line=.false.
      do while(.true.)
         read(unit,'(a)',iostat=iostat)buffer

c  check for i/o error or end of data
         if(iostat.ne.0)then                     !i/o error
            return
         elseif(buffer(1:1).eq.'*')then          !end of data
            return
         endif

c  find first non-blank char         
         ind=1
         do while((ind.le.len(buffer)).and.(buffer(ind:ind).eq.' '))
            ind=ind+1
         enddo

c  check for comments
         if(ind.gt.len(buffer))then              !blank line
            continue
         elseif(buffer(ind:ind).eq.'#')then      !comment
            continue
         else                                    !found data line
            get_next_line=.true.
            return
         endif
      enddo      

      end


c-------------------------------------------------------------
E 1
