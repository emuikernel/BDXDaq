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
