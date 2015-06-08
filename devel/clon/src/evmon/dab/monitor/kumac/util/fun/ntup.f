
      subroutine ntupname(id,name,chname,ldim,chtype,min,max)
      
      character*80 chform
      character*(*) name
      character*(*) chname
      character*(*) chtype
      integer ldim,min,max
      
      include 'ntup.inc'
      
      chform = ' '
      xlo = float(min)
      xhi = float(max)
      call hnform(chform,chname,ldim,chtype,xlo,xhi)
      call hbname(id,name,n,chform)
      
      end
            
      
