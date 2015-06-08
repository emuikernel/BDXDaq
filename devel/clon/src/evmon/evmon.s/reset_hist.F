      subroutine reset_hist(inflag)

c  resets hist, depending on flag

c  ejw, 4-oct-96


      implicit none
      save

      include 'event_monitor.inc'

      character*(*) inflag
      character*120 flag
      integer id,last_slash,myrindex,lenocc
      logical all


c  executable code:
c  ----------------


c  copy flag
      flag=inflag

c  convert to upper case
      call cltou(flag)

c  reset all hist if flag is null or 'ALL' or '0'
      all=(lenocc(flag).le.0).or.(flag.eq.'ALL').or.(flag.eq.'0')

c  reset all hist or a complete sub-dir
      if(all.or.(flag.eq.'PAWC'))then
         call hcdir('//PAWC',' ')
         call hreset(0,' ')
         if(.not.no_dc) then
            call hcdir('//PAWC/DC0',' ')
            call hreset(0,' ')
            call hcdir('//PAWC/DC',' ')
            call hreset(0,' ')
         endif
         if(.not.no_ec) then
            call hcdir('//PAWC/EC',' ')
            call hreset(0,' ')
            call hcdir('//PAWC/ECT',' ')
            call hreset(0,' ')
         endif
         if(.not.no_sc) then
            call hcdir('//PAWC/SC',' ')
            call hreset(0,' ')
         endif
         if(.not.no_cc) then
            call hcdir('//PAWC/CC',' ')
            call hreset(0,' ')
         endif
         if(.not.no_rast) then
            call hcdir('//PAWC/RAST',' ')
            call hreset(0,' ')
         endif
         if(.not.no_lac) then
            call hcdir('//PAWC/EC1',' ')
            call hreset(0,' ')
         endif
         if(.not.no_st) then
            call hcdir('//PAWC/ST',' ')
            call hreset(0,' ')
         endif
         if(.not.no_tpc) then
            call hcdir('//PAWC/TPC',' ')
            call hreset(0,' ')
         endif
         if(.not.no_tpe) then
            call hcdir('//PAWC/TPE',' ')
            call hreset(0,' ')
         endif
         if(.not.no_trig) then
            call hcdir('//PAWC/TRIG',' ')
            call hreset(0,' ')
         endif
         if(.not.no_tg) then
            call hcdir('//PAWC/TAG',' ')
            call hreset(0,' ')
         endif
         if(.not.no_call) then
            call hcdir('//PAWC/CALL',' ')
            call hreset(0,' ')
         endif
         if(.not.no_photon) then
            call hcdir('//PAWC/PHOTON',' ')
            call hreset(0,' ')
         endif
         if(.not.no_scaler) then
            call hcdir('//PAWC/SCALER',' ')
            call hreset(0,' ')
         endif
         if(.not.no_l2) then
            call hcdir('//PAWC/LEVEL2',' ')
            call hreset(0,' ')
         endif

      elseif(flag.eq.'DC0')then
         call hcdir('//PAWC/DC0',' ')
         call hreset(0,' ')
         
      elseif(flag.eq.'EC')then
         call hcdir('//PAWC/EC',' ')
         call hreset(0,' ')
         call hcdir('//PAWC/ECT',' ')
         call hreset(0,' ')
         
      elseif(flag.eq.'SC')then
         call hcdir('//PAWC/SC',' ')
         call hreset(0,' ')
         
      elseif(flag.eq.'CC')then
         call hcdir('//PAWC/CC',' ')
         call hreset(0,' ')
         
      elseif(flag.eq.'RAST')then
         call hcdir('//PAWC/RAST',' ')
         call hreset(0,' ')
         
      elseif(flag.eq.'EC1')then
         call hcdir('//PAWC/EC1',' ')
         call hreset(0,' ')
         
      elseif(flag.eq.'DC')then
         call hcdir('//PAWC/DC',' ')
         call hreset(0,' ')
         
      elseif(flag.eq.'ST')then
         call hcdir('//PAWC/ST',' ')
         call hreset(0,' ')
         
      elseif(flag.eq.'TPC')then
         call hcdir('//PAWC/TPC',' ')
         call hreset(0,' ')

      elseif(flag.eq.'TRIG')then
         call hcdir('//PAWC/TRIG',' ')
         call hreset(0,' ')
         
      elseif(flag.eq.'TAG')then
         call hcdir('//PAWC/TAG',' ')
         call hreset(0,' ')
         
      elseif(flag.eq.'CALL')then
         call hcdir('//PAWC/CALL',' ')
         call hreset(0,' ')
         
      elseif(flag.eq.'SCALER')then
         call hcdir('//PAWC/SCALER',' ')
         call hreset(0,' ')
         
      elseif(flag.eq.'PHOTON')then
         call hcdir('//PAWC/PHOTON',' ')
         call hreset(0,' ')
         
      elseif(.not.no_l2) then
         call hcdir('//PAWC/LEVEL2',' ')
         call hreset(0,' ')

c  reset an individual hist...must parse full path spec
c   also prepend "//PAWC/" if it's not there already
      else
         last_slash=myrindex(flag,'/')
         read(flag(last_slash+1:len(flag)),'(i)')id
         if(flag(1:2).eq.'//')then
            call hcdir(flag(1:last_slash-1),' ')
         elseif(flag(1:1).eq.'/')then
            call hcdir('//PAWC' // flag(1:last_slash-1),' ')
         else
            call hcdir('//PAWC/' // flag(1:last_slash-1),' ')
         endif
         call hreset(id,' ')
      endif


c  back to main dir
      call hcdir('//PAWC',' ')
      
      
      return
      end


c-----------------------------------------------------------------------------------


