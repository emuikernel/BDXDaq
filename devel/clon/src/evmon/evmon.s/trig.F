c  trig.f

c  ejw, 12-nov-1999


c-----------------------------------------------------------


      subroutine trig_init

c  called at init

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------


      return
      end


c-----------------------------------------------------------


      subroutine trig_book

      implicit none
      integer sect,time
      include 'event_monitor.inc'
      include 'trig.inc'
      save


c  executable code:
c  ----------------

      call hmdir('//PAWC/TRIG','S')

      call hbook1(1,'Event type',17,-0.5,16.5,0)
      call hbook1(2,'Trigger bits',13,-0.5,12.5,0)
      call hbook1(3,'Trigger word',4097,-0.5,4096.5,0)

      
c  hist words per event, detector, sector, etc.
      call hbook1(90,'Total words per event',200,0.,2000.,0)
      call hbook1(91,'Data words per event',200,0.,2000.,0)
      call hbook1(100,'Detector words per event',200,0.,2000.,0)
      call hbook1(110,'SC data words',251,-0.5,250.5,0)
      call hbook1(120,'EC data words',301,-0.5,300.5,0)
      call hbook1(130,'CC data words',51,-0.5,50.5,0)
      call hbook1(140,'DC data words',150,-0.5,1500.5,0)
      call hbook1(150,'LAC data words',151,-0.5,150.5,0)
      call hbook1(160,'TAGE data words',151,-0.5,150.5,0)
      do sect=1,6
         call hbook1(100+sect,'Data words per sector',201,0.,500.5,0)
         call hbook1(110+sect,'SC data words per sector',51,-0.5,50.5,0)
         call hbook1(120+sect,'EC data words per sector',201,-0.5,200.5,0)
         call hbook1(130+sect,'CC data words per sector',26,-0.5,25.5,0)
         call hbook1(140+sect,'DC data words per sector',451,-0.5,450.5,0)
         call hbook1(150+sect,'LAC data words per sector',101,-0.5,100.5,0)
         call hbook1(160+sect,'TAGE data words per sector',101,-0.5,100.5,0)
      enddo


c  init tlv1 stuff
      call trig_book_tlv1


c  reset timeline update
      last_update=time()


      call flush(6)
      return
      end


c------------------------------------------------------------------------------


      subroutine trig_bor

c  called at bor

c  ejw, 10-oct-96


      implicit none
      include 'trig.inc'
      integer i,j,time
      save


c  executable code:
c  ----------------

      last_update=time()
      sum_1=0.
      sum_tot=0.
      sum_tot2=0.
      do i=1,7
         do j=1,7
            sum_v(i,j)=0.
            sum_v2(i,j)=0.
         enddo
      enddo

      return
      end


c-----------------------------------------------------------


      subroutine trig_fill


      implicit none
      include 'trig.inc'
      include 'event_monitor.inc'
      include 'bcs.inc'
      
      integer ndet
      parameter (ndet=6)
      integer head,mlink,i,j,sect,bank,det

      integer ev_ct, det_ct(ndet), sect_ct(ndet), det_sect_ct(ndet,6)
      character *4 detname(ndet)/'SC  ','EC  ','CC  ','DC0 ','EC1 ','TAGE'/

      integer time
      double precision var

      integer msg,ipc_msg_create

      save



c  executable code:
c  ----------------


c  ignore scaler events and events with no HEAD bank
      head=mlink(iw,'HEAD',0)
      if(head.le.0)then
         return
      elseif ((iw(head+5).le.0).or.(iw(head+5).ge.10))then
         return
      endif


c  switch to trigger directory
      call hcdir('//PAWC/TRIG',' ')


c  hist event type, trigger bits (n.b. bit 0 is lsb)
      call hf1(1,real(iw(head+7)),1.)
      call hf1(3,real(iw(head+8)),1.)
      do i=1,12
         if(btest(iw(head+8),i-1))call hf1(2,real(i),1.)
      enddo


c  loop over all banks in event and sum data words


c  get event,det,sect,det_sect word counts
      ev_ct=0
      call vzero(det_ct,ndet)
      call vzero(sect_ct,6)
      call vzero(det_sect_ct,6*ndet)
      do det=1,ndet
         do sect=1,6
            bank=mlink(iw,detname(det),sect)
            if(bank.gt.0)then
               ev_ct=ev_ct+iw(bank)
               det_ct(det)=det_ct(det)+iw(bank)
c  TAGE only reports in sector 1 so ignore here
               if(det.ne.6) then
                  sect_ct(sect)=sect_ct(sect)+iw(bank)
                  det_sect_ct(det,sect)=det_sect_ct(det,sect)+iw(bank)
               endif
            endif
         enddo
      enddo



c  hist word counts
      call hf1(90,real(event_length),1.)
ccc      call hf1(91,??,1.)
      call hf1(100,real(ev_ct),1.)
      do sect=1,6
         call hf1(100+sect,real(sect_ct(sect)),1.)
      enddo
      do det=1,ndet
         call hf1(100+10*det,real(det_ct(det)),1.)
      enddo
      do sect=1,6
         do det=1,ndet
            call hf1(100+10*det+sect,real(det_sect_ct(det,sect)),1.)
         enddo
      enddo



c  fill tlv1 stuff
      call trig_fill_tlv1


c  stats for timeline hist
      if(no_timeline)return


      sum_1       = sum_1       + 1.
      sum_tot     = sum_tot     + real(event_length)
      sum_tot2    = sum_tot2    + real(event_length**2)
      sum_v(1,1)  = sum_v(1,1)  + real(ev_ct)
      sum_v2(1,1) = sum_v2(1,1) + real(ev_ct**2)
      do i=2,7
         sum_v(i,1)  = sum_v(i,1)  + real(det_ct(i-1))
         sum_v2(i,1) = sum_v2(i,1) + real(det_ct(i-1)**2)
         sum_v(1,i)  = sum_v(1,i)  + real(sect_ct(i-1))
         sum_v2(1,i) = sum_v2(1,i) + real(sect_ct(i-1)**2)
      enddo
      do i=2,7
         do j=2,7
            sum_v(i,j)  = sum_v(i,j)  + real(det_sect_ct(i-1,j-1))
            sum_v2(i,j) = sum_v2(i,j) + real(det_sect_ct(i-1,j-1)**2)
         enddo
      enddo


c  update timeline hist, calc means and sigmas (probably off by 1)
c  require min 30 secs between entries, min 2000 events
      if(((time()-last_update).gt.30).and.(int(sum_1).gt.2000))then
         trig_evt_mean=sum_tot/sum_1
         trig_evt_width=sqrt(sum_tot2/sum_1-(sum_tot/sum_1)**2)
         trig_evt_mean_err=trig_evt_width/sqrt(sum_1)
         trig_evt_width_err=trig_evt_width/sqrt(2.*(sum_1-1.))

         do i=1,7
            do j=1,7
               trig_mean(i,j)=sum_v(i,j)/sum_1
               var=(sum_v2(i,j)/sum_1-(sum_v(i,j)/sum_1)**2)
               if(var.lt.0) then
                  trig_mean_err(i,j)=0.0
                  trig_width(i,j)=0.0
                  trig_width_err(i,j)=0.0
               else
                  trig_mean_err(i,j)=sqrt(var/sum_1)
                  trig_width(i,j)=sqrt(var)
                  trig_width_err(i,j)=trig_width(i,j)/sqrt(2.*(sum_1-1.))
               endif

            enddo
         enddo


#ifdef SSIPC
c  info server msg
         msg=ipc_msg_create('info_server','info_server/in/event_monitor')
         call ipc_msg_add_string(msg,'event_monitor')
         call ipc_msg_add_real8(msg,sum_1)
         call ipc_msg_add_real8(msg,trig_evt_mean)
         call ipc_msg_add_real8(msg,trig_evt_mean_err)
         call ipc_msg_add_real8(msg,trig_evt_width)
         call ipc_msg_add_real8(msg,trig_evt_width_err)
         call ipc_msg_add_real8_array(msg,trig_mean,49)
         call ipc_msg_add_real8_array(msg,trig_mean_err,49)
         call ipc_msg_add_real8_array(msg,trig_width,49)
         call ipc_msg_add_real8_array(msg,trig_width_err,49)
         call ipc_msg_send(msg)
         call ipc_msg_flush()
         call ipc_msg_destroy(msg)
#endif

c  clear arrays
         sum_1=0.
         sum_tot=0.
         sum_tot2=0.
         do i=1,7
            do j=1,7
               sum_v(i,j)=0.
               sum_v2(i,j)=0.
            enddo
         enddo
         last_update=time()
      endif


c  switch back to main directory
      call hcdir('//PAWC',' ')

      return
      end


c------------------------------------------------------------------------------


      subroutine trig_eor

c  called at eor

c  ejw, 12-jan-1999


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine trig_done

c  called at done

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine trig_book_tlv1

c  books tlv1 hist

      implicit none
      include 'trig.inc'
      character*132 file_in,dummy_c
      integer dummy_i,sect,counter
      double precision sigmal,sigmar
      integer id,sector,pedl,pedr
      save

      file_in='/usr/local/clas/parms/pedman/Tfiles/sc.tped'

      open(unit=12,file=file_in,status='old')

      read(12,*) dummy_c
      read(12,*) dummy_c
      read(12,*) dummy_i
      do sect=1,6
         read(12,*) dummy_c
         do counter=1,57
            read(12,*) id,sector,pedl,sigmal,pedr,sigmar
            scpedl((sect-1)*57+id)=pedl
            scpedr((sect-1)*57+id)=pedr
         enddo
      enddo

      close(12)

      call hbook1(1001,'Sector 1 w/o trigger bit > 100',50,0,50,0)
      call hbook1(1002,'Sector 2 w/o trigger bit > 100',50,0,50,0)
      call hbook1(1003,'Sector 3 w/o trigger bit > 100',50,0,50,0)
      call hbook1(1004,'Sector 4 w/o trigger bit > 100',50,0,50,0)
      call hbook1(1005,'Sector 5 w/o trigger bit > 100',50,0,50,0)
      call hbook1(1006,'Sector 6 w/o trigger bit > 100',50,0,50,0)
      call hbook1(1011,'Sector 1 w/o trigger bit > 200',50,0,50,0)
      call hbook1(1012,'Sector 2 w/o trigger bit > 200',50,0,50,0)
      call hbook1(1013,'Sector 3 w/o trigger bit > 200',50,0,50,0)
      call hbook1(1014,'Sector 4 w/o trigger bit > 200',50,0,50,0)
      call hbook1(1015,'Sector 5 w/o trigger bit > 200',50,0,50,0)
      call hbook1(1016,'Sector 6 w/o trigger bit > 200',50,0,50,0)
      call hbook1(1021,'Sector 1 w/o trigger bit > 300',50,0,50,0)
      call hbook1(1022,'Sector 2 w/o trigger bit > 300',50,0,50,0)
      call hbook1(1023,'Sector 3 w/o trigger bit > 300',50,0,50,0)
      call hbook1(1024,'Sector 4 w/o trigger bit > 300',50,0,50,0)
      call hbook1(1025,'Sector 5 w/o trigger bit > 300',50,0,50,0)
      call hbook1(1026,'Sector 6 w/o trigger bit > 300',50,0,50,0)
      call hbook1(1031,'Sector 1 w/o trigger bit > 400',50,0,50,0)
      call hbook1(1032,'Sector 2 w/o trigger bit > 400',50,0,50,0)
      call hbook1(1033,'Sector 3 w/o trigger bit > 400',50,0,50,0)
      call hbook1(1034,'Sector 4 w/o trigger bit > 400',50,0,50,0)
      call hbook1(1035,'Sector 5 w/o trigger bit > 400',50,0,50,0)
      call hbook1(1036,'Sector 6 w/o trigger bit > 400',50,0,50,0)

      call hbook1(2001,'Sector 1 w trigger bit > 100',50,0,50,0)
      call hbook1(2002,'Sector 2 w trigger bit > 100',50,0,50,0)
      call hbook1(2003,'Sector 3 w trigger bit > 100',50,0,50,0)
      call hbook1(2004,'Sector 4 w trigger bit > 100',50,0,50,0)
      call hbook1(2005,'Sector 5 w trigger bit > 100',50,0,50,0)
      call hbook1(2006,'Sector 6 w trigger bit > 100',50,0,50,0)
      call hbook1(2011,'Sector 1 w trigger bit > 200',50,0,50,0)
      call hbook1(2012,'Sector 2 w trigger bit > 200',50,0,50,0)
      call hbook1(2013,'Sector 3 w trigger bit > 200',50,0,50,0)
      call hbook1(2014,'Sector 4 w trigger bit > 200',50,0,50,0)
      call hbook1(2015,'Sector 5 w trigger bit > 200',50,0,50,0)
      call hbook1(2016,'Sector 6 w trigger bit > 200',50,0,50,0)
      call hbook1(2021,'Sector 1 w trigger bit > 300',50,0,50,0)
      call hbook1(2022,'Sector 2 w trigger bit > 300',50,0,50,0)
      call hbook1(2023,'Sector 3 w trigger bit > 300',50,0,50,0)
      call hbook1(2024,'Sector 4 w trigger bit > 300',50,0,50,0)
      call hbook1(2025,'Sector 5 w trigger bit > 300',50,0,50,0)
      call hbook1(2026,'Sector 6 w trigger bit > 300',50,0,50,0)
      call hbook1(2031,'Sector 1 w trigger bit > 400',50,0,50,0)
      call hbook1(2032,'Sector 2 w trigger bit > 400',50,0,50,0)
      call hbook1(2033,'Sector 3 w trigger bit > 400',50,0,50,0)
      call hbook1(2034,'Sector 4 w trigger bit > 400',50,0,50,0)
      call hbook1(2035,'Sector 5 w trigger bit > 400',50,0,50,0)
      call hbook1(2036,'Sector 6 w trigger bit > 400',50,0,50,0)

      call hbook2(3001,'wo trigger bit > 100',50,0,50,6,0.5,6.5,0)
      call hbook2(3002,'wo trigger bit > 200',50,0,50,6,0.5,6.5,0)
      call hbook2(3003,'wo trigger bit > 300',50,0,50,6,0.5,6.5,0)
      call hbook2(3004,'wo trigger bit > 400',50,0,50,6,0.5,6.5,0)

      call hbook2(4001,'w trigger bit > 100',50,0,50,6,0.5,6.5,0)
      call hbook2(4002,'w trigger bit > 200',50,0,50,6,0.5,6.5,0)
      call hbook2(4003,'w trigger bit > 300',50,0,50,6,0.5,6.5,0)
      call hbook2(4004,'w trigger bit > 400',50,0,50,6,0.5,6.5,0)


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine trig_fill_tlv1

c  fills tlv1 hist


      implicit none
      include 'bcs.inc'
      include 'trig.inc'
      integer itlv1(32) 
      integer tlv1,sc,sect,mlink
      integer word,nrow,ncol,j,k,m
      integer channel,det,offset,hist
      real tdcl,tdcr,adcl,adcr
      real adcval
      save


c  executable code:
c  ----------------

c
c Loop through 6 sectors
c
      do sect=1,6
         data itlv1 /32*0/       ! clean out tlv1 array
         sc=mlink(iw,'SC  ',sect)
         tlv1=mlink(iw,'TLV1',sect)
c
c Get trigger information
c
         if(tlv1.gt.0)then
            nrow=iw(tlv1-4)
            ncol=iw(tlv1-5)
            do j=1,nrow
               k=(j-1)*ncol
               word=iw16(2*tlv1+k+1)
               channel=mod(word,256)
               det=(word-channel)/256
               if(det.eq.2)then
                  itlv1(channel)=1
               endif
            enddo
         endif
c
c Get sc information
c
         if(sc.gt.0)then
            nrow=iw(sc-4)
            ncol=iw(sc-5)
            do j=1,nrow

               k=(j-1)*ncol
               word=iw16(2*sc+k+1)
               tdcl=iw16(2*sc+k+2)
               adcl=iw16(2*sc+k+3)
               tdcr=iw16(2*sc+k+4)
               adcr=iw16(2*sc+k+5)
c
c subtract pedestal values
c
               adcl=adcl-scpedl((sect-1)*57+word)
               adcr=adcr-scpedr((sect-1)*57+word)
c     
c fill w/o trigger bit information
c check adc > 40
c check 10 < tdc < overflow
c the value of hist is used by the 1d histograms for 
c     > 100, > 200, > 300 and > 400
c
               if((adcl.ge.40).and.(adcr.ge.40).and.
     1             (tdcl.ge.10).and.(tdcr.ge.10).and.
     1             (tdcl.le.4000).and.(tdcr.le.4000))then
                  adcval=sqrt(adcl*adcr)
                  hist=0
                  do m=1,4
                     if(adcval.ge.(100*m))then
                        call hf1(1000+hist+sect,real(word),1.)
                        call hf2(3000+m,real(word),real(sect),1.)
                     endif
                     hist=hist+10
                  enddo
c
c Check counter 1
c counter 1 will have only one entry
c fill with trigger bit information
c
                  if((word.eq.1).and.(itlv1(1).eq.1))then
                     hist=0
                     do m=1,4
                        if(adcval.ge.(100*m))then
                           call hf1(2000+hist+sect,real(word),1.)
                           call hf2(4000+m,real(word),real(sect),1.)
                        endif
                        hist=hist+10
                     enddo
c
c Check counters 2 through 16
c 2 through 16 will have two entries
c fill with trigger bit information
c
                  elseif((word.le.17).and.((itlv1(word).eq.1).or.
     1                   (itlv1(word-1).eq.1)))then
                     hist=0
                     do m=1,4
                        if(adcval.ge.(100*m))then
                           call hf1(2000+hist+sect,real(word),1.)
                           call hf2(4000+m,real(word),real(sect),1.)
                        endif
                        hist=hist+10
                     enddo
c
c Check counters 17 through 48 even 
c if even counter, then it will only have one entry
c fill with trigger bit information
c
                     offset=(word/2)-8
                  elseif((mod(word,2).eq.0).and.
     1                   (itlv1(16+offset).eq.1))then
                     hist=0
                     do m=1,4
                        if(adcval.ge.(100*m))then
                           call hf1(2000+hist+sect,real(word),1.)
                           call hf2(4000+m,real(word),real(sect),1.)
                        endif
                        hist=hist+10
                     enddo
c
c Check counters 17 through 48 odd
c if odd counter, then it will have two entries
c
                     offset=((word+1)/2)-2
                     channel=16+offset
                  elseif((itlv1(channel).eq.1).or.
     1                   (itlv1(channel-1).eq.1))then
                     hist=0
                     do m=1,4
                        if(adcval.ge.(100*m))then
                           call hf1(2000+hist+sect,real(word),1.)
                           call hf2(4000+m,real(word),real(sect),1.)
                        endif
                        hist=hist+10
                     enddo

                  endif
               endif
            enddo
         endif

      enddo
      
      return
      end


c-----------------------------------------------------------


