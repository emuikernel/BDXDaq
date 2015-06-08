c  scaler.f

c  ejw, 27-may-98
c  marchand claude, 31-may-98
c  e.anciant 18-july 1999
c  ejw, 22-oct-99 bug fixes
c  RHFIII, 16-nov-99 fixed conversion of unsigned 32 bit intger
c                    to double precision.

c  Michel Garcon, 27-oct-01: in subroutine scaler_file for eor print-out 
c       - introduced distinction between electron and photon runs
c              (not finished)
c       - introduced faraday_cup and current


c-----------------------------------------------------------


      subroutine scaler_book

      implicit none
      save

      integer i
      character*20 text


c  executable code:
c  ----------------

      call hmdir('//PAWC/SCALER','S')

c      book tagger scalers

       call hbook1(401,'Raw T counter scalers',64,.5,64.5,0.)
       call hbook1(402,'T scalers set 1 odd/raw',61,.5,61.5,0.)
       call hbook1(403,'T scalers set 1 even/raw',61,.5,61.5,0.)
       call hbook1(404,'T scalers set 2 odd/raw',61,.5,61.5,0.)
       call hbook1(405,'T scalers set 2 even/raw',61,.5,61.5,0.)
       call hbook1(410,'Transmission TAC.T/RAW T (norm runs)',61,.5,61.5,0.)
c       call hbook1(411,'PC efficiency PC.T/TAC.T (norm runs)',
c     + 61,.5,61.5,0.)
       call hbook1(412,'PS efficiency (PS.T)/(TAC.T) (norm runs)',
     + 61,.5,61.5,0.)
c       call hbook1(421,'PC/MOR corr. for accidentals (prod runs)',
c     + 61,.5,61.5,0.)
       call hbook1(421,'(PS Left/right accidentals.T)/(RAW T) (prod runs)',
     + 61,.5,61.5,0.)
       call hbook1(422,'(PS.T)/(RAW T) corrected for PS.T accidentals (prod runs)',
     + 61,.5,61.5,0.)

c      book TOF scalers
       
       text='TOF scalers sector x'
       do i=1,6
        write(text(20:20),'(I1)')i
        call hbook1(500+i,text,32,.5,32.5,0.)
       enddo

       text='TOF rates   sector x'
       do i=1,6
        write(text(20:20),'(I1)')i
        call hbook1(600+i,text,32,.5,32.5,0.)
       enddo

      return
      end


c------------------------------------------------------------------------------


      subroutine scaler_bor

c  called at bor


      implicit none
      save

      include 'SCALERnt.inc'

c  executable code:
c  ----------------

      integer i

      no_scal=.true.
      last_scaler_event=0

      iscal=0

      do i=1,n_trgs
       nov_trgs(i)=0
       tab_trgs(i)=0
       sum_trgs(i)=0
       tot_trgs(i)=0.
      enddo

      do i=1,n_s1st
       nov_s1st(i)=0
       tab_s1st(i)=0
       sum_s1st(i)=0
       tot_s1st(i)=0.
      enddo

      do i=1,n_tgs
       nov_tgs(i)=0
       tab_tgs(i)=0
       sum_tgs(i)=0
       tot_tgs(i)=0.
      enddo

      do i=1,n_ecs
       nov_ecs(i)=0
       tab_ecs(i)=0
       sum_ecs(i)=0
       tot_ecs(i)=0.
      enddo

      do i=1,n_ccs
       nov_ccs(i)=0
       tab_ccs(i)=0
       sum_ccs(i)=0
       tot_ccs(i)=0.
      enddo

      do i=1,n_scs
       nov_scs(i)=0
       tab_scs(i)=0
       sum_scs(i)=0
       tot_scs(i)=0.
      enddo

      do i=1,n_sts
       nov_sts(i)=0
       tab_sts(i)=0
       sum_sts(i)=0
       tot_sts(i)=0.
      enddo

      do i=1,61
        gt1_odd(i)=0.
        gt1_eve(i)=0.
        gt2_odd(i)=0.
        gt2_eve(i)=0.
      enddo

      return
      end


c-----------------------------------------------------------


      subroutine scaler_done

c  called at done


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine scaler_eor

c  called at eor


      implicit none
      save
      integer istat,systemf
      character*256 filename



c  executable code:
c  ----------------
      
c  returns filename
      call scaler_file(filename)
ccc      istat = systemf('lp ' // filename)

      return
      end


c-----------------------------------------------------------


      subroutine scaler_file(filename)

c  prints scalers


      implicit none
      save

      include 'SCALERnt.inc'
      include "event_monitor.inc"

C sergey: systemf below does not like variable length      character*(*) filename
      character*128 filename

      character*25 clon_parms,runnum
      integer i,j,lenocc,systemf,istat
      real tot_time,rgt_time,lgt_time,live_clk,srs_time,live_evt
      real rat_trig,rat_evnt,rat_mor,rat_pc,rat_ps,rat_tac
      real rat_morst,rat_morpc,rat_morps,rat_mortac,live_mor
      real rat_sts,rat_st1,rat_st2,rat_st3,rat_sc(6),rat_ec(6)
      real pc_mor,ps_mor,tac_mor,st_mor
      real rat_morx,rat_stx,rat_morstx
      double precision tot_sc(6),tot_ec(6)
      real faraday_cup,current
      logical photon_run


c  executable code:
c  ----------------

      call getenvf('CLON_PARMS',clon_parms)
      write(runnum,'(i6.6)')current_run


c  following should be retrieved from /usr/local/clas/parms/runsheet/archiv.txt
c  for now hard coded as electron run
      photon_run = .false.


c     now print out all scalers
 
      filename=clon_parms(1:lenocc(clon_parms)) // '/scalers/archive/evmon_scaler_'
     1     // runnum(1:lenocc(runnum)) // '.allnew'
      open(unit=20,file=filename,status='unknown')
      
      write(20,*)'TRGS scalers for run ',current_run
      write(20,*)' '
      write(20,*)' scaler  no gate    run gate   live gate',
     +'    trg bits    s1st'
      write(20,*)' '
      do i=1,16
        write(20,'(i5,1x,4(1x,f11.0),1x,f11.0)')
     +i,(tot_trgs(j),j=i,i+48,16),tot_s1st(i)
      enddo
      write(20,*)' '
      write(20,*)''
      write(20,*)'ECS scalers for run ',current_run
      write(20,*)' '
      do i=1,16
        write(20,'(i5,1x,6(1x,f11.0))')i,(tot_ecs(j),j=i,i+80,16)
      enddo
      write(20,*)' '
      write(20,*)''
      write(20,*)'CCS scalers for run ',current_run
      write(20,*)' '
      do i=1,16
        write(20,'(i5,1x,6(1x,f11.0))')i,(tot_ccs(j),j=i,i+80,16)
      enddo
      write(20,*)' '
      write(20,*)''
      write(20,*)'SCS scalers for run ',current_run
      write(20,*)' '
      do i=1,32
        write(20,'(i5,1x,6(1x,f11.0))')i,(tot_scs(j),j=i,i+160,32)
      enddo
      write(20,*)' '
      write(20,*)''
      write(20,*)'STS scalers for run ',current_run
      write(20,*)' '
      do i=1,16
        write(20,'(i5,1x,f11.0)')i,tot_sts(i)
      enddo
      close(20)

c     now compute most important rates for short printout

      tot_time=tot_trgs(1)/clock_rate
      rgt_time=tot_trgs(17)/clock_rate
      lgt_time=tot_trgs(33)/clock_rate
      live_clk=lgt_time/rgt_time
      live_mor=tot_trgs(41)/tot_trgs(25)
      srs_time=tot_ecs(9)/clock_rate
      rat_trig=tot_trgs(61)/tot_time
      rat_evnt=tot_s1st(13)/tot_time
      live_evt=rat_evnt/rat_trig
      rat_mor=tot_trgs(9)/tot_time/1000.
      rat_pc=tot_trgs(10)/tot_time/1000.
      rat_ps=tot_trgs(11)/tot_time/1000.
      rat_tac=tot_trgs(12)/tot_time/1000.
      rat_morst=tot_trgs(5)/tot_time/1000.
      rat_morpc=tot_trgs(6)/tot_time/1000.
      rat_morps=tot_trgs(7)/tot_time/1000.
      rat_mortac=tot_trgs(8)/tot_time/1000.
      rat_sts=tot_trgs(13)/tot_time/1000.
      rat_st1=tot_sts(1)/tot_time/1000.
      rat_st2=tot_sts(2)/tot_time/1000.
      rat_st3=tot_sts(3)/tot_time/1000.
      if(lgt_time.ne.0.)then
        rat_morx=tot_trgs(36)/lgt_time/1000.
        rat_stx=tot_trgs(37)/lgt_time/1000.
        rat_morstx=tot_trgs(35)/lgt_time/1000.
      endif
      pc_mor=rat_pc/rat_mor
      ps_mor=rat_ps/rat_mor
      tac_mor=rat_tac/rat_mor
      st_mor=rat_sts/rat_mor
      faraday_cup = tot_trgs(18)/9264.
      current = faraday_cup/rgt_time

      do i=1,6
        tot_sc(i)=0
        tot_ec(i)=0
        do j=1,32
          if(j.le.16)tot_sc(i)=tot_sc(i)+tot_scs((i-1)*32+j)/2.
          if(j.gt.16)tot_sc(i)=tot_sc(i)+tot_scs((i-1)*32+j)/1.67
        enddo
        do j=1,6
          tot_ec(i)=tot_ec(i)+tot_ecs((i-1)*16+j)
        enddo
        rat_sc(i)=tot_sc(i)/tot_time/1000.
        rat_ec(i)=tot_ec(i)/tot_time/1000.
      enddo


      filename=clon_parms(1:lenocc(clon_parms)) // '/scalers/archive/evmon_scaler_'
     1     // runnum(1:lenocc(runnum)) // '.sumnew'
      open(unit=21,file=filename,status='unknown')

      write(21,*)' '
      write(21,*)'      Summary of Scalers for RUN ',current_run
      write(21,*)' '
      write(21,*)'Total number of scaler events    :',iscal
      write(21,*)'Total number of triggers         :',tot_trgs(61)
      write(21,*)'Total number of events read      :',tot_s1st(13)
      write(21,*)'Trigger rate                     :',rat_trig,' Hz'
      write(21,*)'Event   rate                     :',rat_evnt,' Hz'
      write(21,*)' '
      write(21,*)'Non-reset clock ungated          :',tot_time,' s'
      write(21,*)'Non-reset clock run gated        :',rgt_time,' s'
      write(21,*)'Reset clock run gated (in ECS)   :',srs_time,' s'
      write(21,*)'Non-reset clock live gated       :',lgt_time,' s'
      write(21,*)' '
      write(21,'(a,f8.3)')'Live time percentage (by clock) :',live_clk
      write(21,'(a,f8.3)')'Live time percentage (by events):',live_evt

      if(photon_run) then
         write(21,*)' '
         write(21,*)'      Ungated Scaler Rates (kHz)'
         write(21,*)' '
c     write(21,*)'MOR rate:',rat_mor,' kHz'
c     write(21,*)'ST  rate: ',rat_sts,' kHz   ST/MOR =',st_mor,  
c     +'  MOR.ST  rate:',rat_morst,' kHz'
c      write(21,*)'PC  rate: ',rat_pc,' kHz   PC/MOR =',pc_mor, 
c     +'  MOR.PC  rate: ',rat_morpc,' kHz'
c      write(21,*)'PS  rate: ',rat_ps,' kHz   PS/MOR =',ps_mor, 
c     +'  MOR.PS  rate: ',rat_morps,' kHz'
c      write(21,*)'TAC rate: ',rat_tac,' kHz  TAC/MOR =',tac_mor,
c     +'  MOR.TAC rate: ',rat_mortac,' kHz'
         write(21,'(a,f8.2,a)')'MOR rate: ',rat_mor,' kHz'
         write(21,'(a,f8.2,a,f6.4,a,f8.2,a)')'ST  rate: ',rat_sts,
     +        ' kHz   ST/MOR =',st_mor,  
     +        '  MOR.ST  rate: ',rat_morst,' kHz'
         write(21,'(a,f8.2,a,f6.4,a,f8.2,a)')'PC  rate: ',rat_pc,
     +        ' kHz   PC/MOR =',pc_mor, 
     +        '  MOR.PC  rate: ',rat_morpc,' kHz'
         write(21,'(a,f8.2,a,f6.4,a,f8.2,a)')'PS  rate: ',rat_ps,
     +        ' kHz   PS/MOR =',ps_mor, 
     +        '  MOR.PS  rate: ',rat_morps,' kHz'
         write(21,'(a,f8.2,a,f6.4,a,f8.2,a)')'TAC rate: ',rat_tac,
     +        ' kHz  TAC/MOR =',tac_mor,
     +        '  MOR.TAC rate: ',rat_mortac,' kHz'
         
         write(21,*)' '
         write(21,*)'      Run gated Scaler Rates (kHz)'
         write(21,*)' '
         
         write(21,'(a,f8.2,a,f8.4,a,f8.2,a)')'xMOR rate:',rat_morx,
     +        ' kHz xST rate =',rat_stx,
     +        ' kHz xMOR.ST rate = ',rat_morstx,' kHz'
         
         write(21,*)' '
         write(21,*)'Start Counter 1:',rat_st1,' kHz'
         write(21,*)'Start Counter 2:',rat_st2,' kHz'
         write(21,*)'Start Counter 3:',rat_st3,' kHz'
      else
         write(21,*)' '
         write(21,'(a,f11.3)')'Faraday cup        :',faraday_cup,' nC'
         write(21,'(a,f11.3)')'Average current    :',current,' nA'
         write(21,*)' '
      endif 

      write(21,*)' '
      do i=1,6
        write(21,*)'TOF sector',i,' :',rat_sc(i),' kHz'
      enddo
      write(21,*)' '
      do i=1,6
        write(21,*)'EC  sector',i,' :',rat_ec(i),' kHz'
      enddo

c  following info already printed
c      write(21,*)' '
c      write(21,*)'   Run     Trig/s     Events/s      Live    Mor(kHz)'
c      write(21,*)current_run,rat_trig,rat_evnt,live_clk,rat_mor

      close(21)


c  copy file, only print at eor
      istat = systemf('cp ' // filename // ' ' // clon_parms(1:lenocc(clon_parms))
     1     // '/scalers/evmon_scaler.sum')


      return
      end


c-----------------------------------------------------------


      subroutine scalerstore


      implicit none
      save

      include 'bcs.inc'
      include 'SCALERnt.inc'

      integer head,mlink,nword,i

      integer ind_trgs,ind_s1st,ind_tgs
      integer ind_ecs,ind_ccs,ind_scs,ind_sts
      double precision conscal


c  executable code:
c  ----------------


c  ignore non-scaler events, events with no HEAD bank, 
c    out of order scaler events, etc.
      head=mlink(iw,'HEAD',0)
      if(head.le.0)then
         no_scal=.true.
         return
      elseif (iw(head+3).le.last_scaler_event) then
         no_scal=.true.
         return
      elseif (iw(head+7).ne.0) then
         no_scal=.true.
         return
      elseif (iw(head+5).ne.10) then
         no_scal=.true.
         return
      endif


      iscal=iscal+1
      no_scal=.false.
      last_scaler_event=iw(head+3)


c  get adresses of scaler banks
c  use standard BOS method...ejw
      ind_trgs = mlink(iw,'TRGS',0)
      ind_tgs  = mlink(iw,'TGS ',0)
      ind_s1st = mlink(iw,'S1ST',0)
      ind_ecs  = mlink(iw,'ECS ',0)
      ind_ccs  = mlink(iw,'CCS ',0)
      ind_scs  = mlink(iw,'SCS ',0)
      ind_sts  = mlink(iw,'STS ',0)

      
c  READ ALL SCALER BANKS
    
c  read TRGS bank (non-reset trigger scalers)
      if(ind_trgs.gt.0) then
         nword=min(iw(ind_trgs),n_trgs)
         do i=1,nword
            if((iw(ind_trgs+i).lt.tab_trgs(i)).and.(tab_trgs(i).gt.0)) then
               nov_trgs(i)=nov_trgs(i)+1
ccc               print *,'trgs overflow',i,iw(ind_trgs+i),tab_trgs(i),nov_trgs(i)
            endif
            tab_trgs(i)=iw(ind_trgs+i)
            sum_trgs(i)=tab_trgs(i)
            tot_trgs(i)=conscal(sum_trgs(i))
         enddo
      endif
      

c  read S1ST bank (non reseted trigger latch scalers)
      if(ind_s1st.gt.0) then
         nword=iw(ind_s1st)
         nword=min(nword,n_s1st)
         
         do i=1,nword
            if(iw(ind_s1st+i).gt.tab_s1st(i))then
               tab_s1st(i)=iw(ind_s1st+i)
               sum_s1st(i)=tab_s1st(i)
            else if(iw(ind_s1st+i).eq.tab_s1st(i)) then
               continue
            else if((iw(ind_s1st+1).lt.tab_s1st(i)).and.
     +              (iw(ind_s1st+1).ne.0)) then
               nov_s1st(i)=nov_s1st(i)+1
ccc               print *,'s1st overflow',i,iw(ind_s1st+i),tab_s1st(i),nov_s1st(i)
               tab_s1st(i)=iw(ind_s1st+i)
               sum_s1st(i)=tab_s1st(i)
            endif
            tot_s1st(i)=conscal(sum_s1st(i))+conscal(nov_s1st(i))*2.**32
         enddo
      else
         continue
      endif
      
      
c  read TGS bank (reseted tagger scalers)
      if(ind_tgs.gt.0) then
         nword=iw(ind_tgs)
         nword=min(nword,n_tgs)
         
         do i=1,nword
            tab_tgs(i)=iw(ind_tgs+i)
            if((sum_tgs(i)+tab_tgs(i)).gt.sum_tgs(i)) then
               sum_tgs(i)=sum_tgs(i)+tab_tgs(i)
            else if(tab_tgs(i).eq.0) then
               continue
            else if((sum_tgs(i)+tab_tgs(i)).lt.sum_tgs(i)) then
               nov_tgs(i)=nov_tgs(i)+1
ccc               print *,'tgs overflow ',i,iw(ind_tgs+i),tab_tgs(i),nov_tgs(i)
               sum_tgs(i)=sum_tgs(i)+tab_tgs(i)
            endif
            tot_tgs(i)=conscal(sum_tgs(i))+conscal(nov_tgs(i))*2.**32
         enddo
      else
         continue
      endif


c  translate in usable form
      do i=65,80
         gt1_odd(2*(i-64)-1)=tot_tgs(i)
      enddo
      do i=97,112
         gt1_odd(2*(i-80)-1)=tot_tgs(i)
      enddo
      do i=81,96
         gt1_eve(2*(i-80))=tot_tgs(i)
      enddo
      do i=113,128
         gt1_eve(2*(i-96))=tot_tgs(i)
      enddo
       do i=129,144
         gt2_odd(2*(i-128)-1)=tot_tgs(i)
       enddo
       do i=161,176
         gt2_odd(2*(i-144)-1)=tot_tgs(i)
       enddo
       do i=145,160
         gt2_eve(2*(i-144))=tot_tgs(i)
       enddo
       do i=177,192
         gt2_eve(2*(i-160))=tot_tgs(i)
       enddo


c  read ECS bank (reseted fwd calo scalers)
       if(ind_ecs.gt.0) then
          nword=iw(ind_ecs)
          nword=min(nword,n_ecs)
          
          do i=1,nword
             tab_ecs(i)=iw(ind_ecs+i)
             if((sum_ecs(i)+tab_ecs(i)).gt.sum_ecs(i)) then
                sum_ecs(i)=sum_ecs(i)+tab_ecs(i)
             else if(tab_ecs(i).eq.0) then
                continue
             else if((sum_ecs(i)+tab_ecs(i)).lt.sum_ecs(i)) then
                nov_ecs(i)=nov_ecs(i)+1
ccc                print *,'ecs overflow',i,iw(ind_ecs+i),tab_ecs(i),nov_ecs(i)
                sum_ecs(i)=sum_ecs(i)+tab_ecs(i)
             endif
             tot_ecs(i)=conscal(sum_ecs(i))+conscal(nov_ecs(i))*2.**32
          enddo
       else
          continue
       endif
       
       
c  read CCS bank (reseted cerenkov scalers)
       if(ind_ccs.gt.0) then
          nword=iw(ind_ccs)
          nword=min(nword,n_ccs)
          
          do i=1,nword
             tab_ccs(i)=iw(ind_ccs+i)
             if((sum_ccs(i)+tab_ccs(i)).gt.sum_ccs(i)) then
                sum_ccs(i)=sum_ccs(i)+tab_ccs(i)
             else if(tab_ccs(i).eq.0) then
                continue
             else if((sum_ccs(i)+tab_ccs(i)).lt.sum_ccs(i)) then
                nov_ccs(i)=nov_ccs(i)+1
ccc                print *,'ccs overflow',i,iw(ind_ccs+i),tab_ccs(i),nov_ccs(i)
               sum_ccs(i)=sum_ccs(i)+tab_ccs(i)
             endif
             tot_ccs(i)=conscal(sum_ccs(i))+conscal(nov_ccs(i))*2.**32
          enddo
       else
          continue
       endif
       
       
c read SCS bank (reseted TOF scalers)
       if(ind_scs.gt.0) then
          nword=iw(ind_scs)
          nword=min(nword,n_scs)
          
          do i=1,nword
             tab_scs(i)=iw(ind_scs+i)
             if((sum_scs(i)+tab_scs(i)).gt.sum_scs(i)) then
                sum_scs(i)=sum_scs(i)+tab_scs(i)
             else if(tab_scs(i).eq.0) then
                continue
             else if((sum_scs(i)+tab_scs(i)).lt.sum_scs(i)) then
                nov_scs(i)=nov_scs(i)+1
ccc                print *,'scs overflow',i,iw(ind_scs+i),tab_scs(i),nov_scs(i)
                sum_scs(i)=sum_scs(i)+tab_scs(i)
             endif
             tot_scs(i)=conscal(sum_scs(i))+conscal(nov_scs(i))*2.**32
          enddo
       else
          continue
       endif
       
       
c  read STS bank (reseted start counter scalers)
       if(ind_sts.gt.0) then
          nword=iw(ind_sts)
          nword=min(nword,n_sts)
          
          do i=1,nword
             tab_sts(i)=iw(ind_sts+i)
             if((sum_sts(i)+tab_sts(i)).gt.sum_sts(i)) then
                sum_sts(i)=sum_sts(i)+tab_sts(i)
             else if(tab_sts(i).eq.0) then
                continue
             else if((sum_sts(i)+tab_sts(i)).lt.sum_sts(i)) then
                nov_sts(i)=nov_sts(i)+1
ccc                print *,'sts overflow',i,iw(ind_sts+i),tab_sts(i),nov_sts(i)
                sum_sts(i)=sum_sts(i)+tab_sts(i)
             endif
             tot_sts(i)=conscal(sum_sts(i))+conscal(nov_sts(i))*2.**32
          enddo
       else
          continue
       endif



c  flush pending output
       call flush(6)
       
       
       return
       end

         
c-----------------------------------------------------------


      subroutine scaler_fill


      implicit none
      save

      include 'SCALERnt.inc'

      integer i,j,k
      real rgt1_odd(61),rgt1_eve(61),rgt2_odd(61),rgt2_eve(61)
      character*256 filename

      double precision conscal
      real xconscal


c  executable code:
c  ----------------

       if (no_scal) return

c  FILL SCALER HISTOGRAMMS  

c  switch to scaler directory

      call hcdir('//PAWC/SCALER',' ')


c  fill tagger scaler histos

c
c     First handle the raw data banks
c
       do i=1,61

csergey: does not like it          xconscal = float(conscal(tab_tgs(i)))
          xconscal = conscal(tab_tgs(i))

          call hf1( 401, float(i), xconscal )
       enddo

       call hreset(402,' ')
       call hreset(403,' ')
       call hreset(404,' ')
       call hreset(405,' ')
       call hreset(410,' ')
c       call hreset(411,' ')
       call hreset(412,' ')
       call hreset(421,' ')
       call hreset(422,' ')

       if(tot_trgs(1).eq.0.) goto 567

c     now compute PC,PS,TAC efficiencies

       do i=1,61
         if(tot_tgs(i).ne.0.)
     +rgt1_odd(i)=gt1_odd(i)/tot_tgs(i)
         if(tot_tgs(i).ne.0.)
     +rgt1_eve(i)=gt1_eve(i)/tot_tgs(i)
         if(tot_tgs(i).ne.0.)
     +rgt2_odd(i)=gt2_odd(i)/tot_tgs(i)
         if(tot_tgs(i).ne.0.)
     +rgt2_eve(i)=gt2_eve(i)/tot_tgs(i)

         if(tot_tgs(i).ne.0.)
     +eff_tac(i)=(gt1_odd(i)+gt2_eve(i))/tot_tgs(i)

c         if(gt2_odd(i).ne.0.)
c     +eff_pclow(i)=gt1_odd(i)/gt2_odd(i)
c         if(gt2_eve(i).ne.0.)
c     +eff_pslow(i)=gt1_eve(i)/gt2_eve(i)
          if((gt2_eve(i)+gt1_odd(i)).ne.0)
     +eff_pslow(i)=(gt1_eve(i)+gt2_odd(i))/(gt2_eve(i)+gt1_odd(i))
         if(tot_tgs(i).ne.0.)
     +eff_pchig(i)=(gt1_odd(i)-gt2_odd(i))/tot_tgs(i)
         if(tot_tgs(i).ne.0.)
     +eff_pshig(i)=(gt1_eve(i)-gt2_eve(i))/tot_tgs(i)

         if(tot_tgs(i).ne.0.)
     +call hf1(402,float(i),rgt1_odd(i))
         if(tot_tgs(i).ne.0.)
     +call hf1(403,float(i),rgt1_eve(i))
         if(tot_tgs(i).ne.0.)
     +call hf1(404,float(i),rgt2_odd(i))
         if(tot_tgs(i).ne.0.)
     +call hf1(405,float(i),rgt2_eve(i))
         if(tot_tgs(i).ne.0.)
     +call hf1(410,float(i),eff_tac(i))
c         if(gt2_odd(i).ne.0.)
c     +call hf1(411,float(i),eff_pclow(i))
         if((gt2_eve(i)+gt1_odd(i)).ne.0.)
     +call hf1(412,float(i),eff_pslow(i))
         if(tot_tgs(i).ne.0.)
     +call hf1(421,float(i),eff_pchig(i))
         if(tot_tgs(i).ne.0.)
     +call hf1(422,float(i),eff_pshig(i))
        enddo
             

      do i=1,6
         call hreset(600+i,' ')
      enddo


      do i=1,6
       do j=1,32
cc      print *,'uu->',tot_scs((i-1)*32+j),tot_trgs(1),clock_rate
        if(j.le.16) then
         k = ((tot_scs((i-1)*32+j))*0.5)/(tot_trgs(1)/clock_rate)
         call hf1(600+i,float(j),float(k))
        endif
        if(j.gt.16) then
         k = ((tot_scs((i-1)*32+j))*0.33)/(tot_trgs(1)/clock_rate)
         call hf1(600+i,float(j),float(k))
        endif
       enddo
      enddo


 567   continue

c     now fill the TOF scaler histograms

      do i=1,6
       do j=1,32
      if(j.le.16) call hf1(500+i,float(j),
     +float(tab_scs((i-1)*32+j))*0.5)
      if(j.gt.16) call hf1(500+i,float(j),
     +float(tab_scs((i-1)*32+j))*0.33)
       enddo
      enddo


c  write scaler file
      call scaler_file(filename)


c  switch back to main directory
      call hcdir('//PAWC',' ')


      return
      end


c-----------------------------------------------------------------


      subroutine scaler_init

c  called at init


      implicit none
      save


c  executable code:
c  ----------------

      return
      end

c-----------------------------------------------------------------


      double precision function conscal(i)

c  convert an integer*4 into double precision
c  while treating the integer*4 as unsigned

      implicit none
      integer*4 i
      double precision a,b


c  executable code:
c  ----------------


      if (i.ge.0) Then
         conscal = i * 1.D+00
      else if (i.eq.-2147483648) Then
         conscal = 2.147483648D+09
      else
         a = i * -1.D+00
         b = 2.147483649D+09-a
         conscal = a + 2.147483648D+09
      end if


      return
      end

         
c-----------------------------------------------------------
