h02844
s 00000/00000/00000
d R 1.2 97/07/29 10:44:37 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 libutil/s/find_tag_line_.f
e
s 00040/00000/00000
d D 1.1 97/07/29 10:44:36 wolin 1 0
c Fortran versions of c++ routines
e
u
U
f e 0
t
T
I 1
      logical function find_tag_line(unit,tag,buffer)

c  fortran version of find_tag_line.cc

c  ejw, 29-jul-97



      implicit none

      integer unit
      character *(*) tag
      character *(*) buffer

      integer iostat
      

c  executable code
c  ---------------

c  back to beginning of file
      rewind(unit)


c  scan lines for tag
      find_tag_line=.false.
      do while(.true.)
         read(unit,'(a)',iostat=iostat)buffer
         if(iostat.ne.0)then
            return
         elseif(buffer.eq.tag)then
            find_tag_line=.true.
            return
         endif
      enddo      

      end


c-------------------------------------------------------------
E 1
