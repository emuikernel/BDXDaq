      subroutine write(lun,str1,str2,str3)
      
      integer lun,len1,len2,len3
      character*(*) str1,str2,str3
      character*80 string,str4
      
      len1 = lenocc(str1)
      len2 = lenocc(str2)
      len3 = lenocc(str3)

      str4 = str2
      if (str3.eq.'END') str4 = str2(1:len2)//''''//')'
      if (str3.eq.'BEG') str4 = ''''//str2(2:len2)

      len2 = lenocc(str4)

      if(len2.ne.0) then
        string = str1(1:len1)//str4(1:len2)
      else
        string = str1(1:len1)
      endif
      len = lenocc(string)
      write(lun,*) string(1:len)
      
      end
      
      
