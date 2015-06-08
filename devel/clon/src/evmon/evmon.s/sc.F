c  sc.f

c  ejw, 13-may-98


c-----------------------------------------------------------

      subroutine sc_book
          
      integer i
      character*80 title
      character*80 t10,t20
      character*80 t1001,t1002,t1003,t1004
      character*80 t102,t103,t104,t105,t106,t107
      character*8 fstring(6)
      real tminraw, tmaxraw, tmincal, tmaxcal
      real aminraw, amaxraw, amincal, amaxcal
      data tminraw,tmaxraw,tmincal,tmaxcal/0.,4095.,0.,4095./
      data aminraw,amaxraw,amincal,amaxcal/0.,2000.,0.,2000./
c      tminraw,..., are the hbook ranges for raw and calculated TDCs
c      aminraw,..., are the hbook ranges for raw and calculated ADCs

      
      do i = 1,6
        write(fstring(i),20) 'SECTOR ',i
      enddo
      
      call hmdir('//PAWC/SC','S')
      
      call hbook2(1,'SC;SECTOR;',6,1.,7.,4,5.,9.,0.)
      call hbook2(2,'SC TDC, Occupancy;PMT;',57,1.,58.,12,1.,13.,0.)
      call hbook2(3,'SC TDC, wt by TDC val;PMT;',57,1.,58.,12,1.,13.,0.)
      call hbook2(4,'SC ADC, Occupancy;PMT;',57,1.,58.,12,1.,13.,0.)
      call hbook2(5,'SC ADC, wt by ADC val;PMT;',57,1.,58.,12,1.,13.,0.)
c      call hbook2(6,'SC MULTIPLICITY',49,0.,49.,48,1.,49.,0.)

      call hbook1(7,'SC multiplicity', 24, 0.,24.,0.)


      call hbook2(10,'SCT occupancy;PMT;',57,1.,58.,12,1.,13.,0.)
c      call hbook2(9,'SCTx; occupancy;PMT;',512,1.,513.,6,1.,7.,0.)

      t10 = ' SCT;TDCL;PMT;'
      t20 = ' SCT;TDCR;PMT;'


      do i = 1,6
         title=fstring(i)//t10
         call hbook2(10+i,title,200,10.,8000.,57,1.,58.,0.)
         title=fstring(i)//t20
         call hbook2(20+i,title,200,10.,8000.,57,1.,58.,0.)
      enddo

      
      t102  = ' SC;TDCL;PMT'
      t103  = ' SC;ADCL;PMT'
      t104  = ' SC;TDCR;PMT'
      t105  = ' SC;ADCR;PMT'
      t106  = ' SC;ADCL LO ;PMT'
      t107  = ' SC;ADCR LO ;PMT'
      t1001 = ' SC;SQRT(ADCL*ADCR);PMT'
      t1002 = ' SC;150*LOG(ADCR/ADCL);PMT'
      t1003 = ' SC;0.36*(TDCL-TDCR);PMT'
      t1004 = ' SC;0.36*(TDCL-TDCR) + 150*LOG(ADCL/ADCR);PMT'
      
      do i = 1,6
        title = fstring(i)//t102
        call hbook2(i*100+2,title,200,10.,8000.,57,1.,58.,0.)
        title = fstring(i)//t103 
        call hbook2(i*100+3,title,100,0.,2500.,57,1.,58.,0.)
        title = fstring(i)//t104
        call hbook2(i*100+4,title,200,10.,8000.,57,1.,58.,0.)
        title = fstring(i)//t105
        call hbook2(i*100+5,title,100,0.,2500.,57,1.,58.,0.)
        title = fstring(i)//t106
        call hbook2(i*100+6,title,100,0.,250.,57,1.,58.,0.)
        title = fstring(i)//t107
        call hbook2(i*100+7,title,100,0.,250.,57,1.,58.,0.)
        title = fstring(i)//t1001
        call hbook2(i*1000+1,title,100,0.,2500.,57,1.,58.,0.)
        title = fstring(i)//t1002
        call hbook2(i*1000+2,title,100,-1.8,+1.8,57,1.,58.,0.)
        title = fstring(i)//t1003
        call hbook2(i*1000+3,title,100,-1.8,+1.8,57,1.,58.,0.)
        title = fstring(i)//t1004
        call hbook2(i*1000+4,title,100,-.25,+.25,57,1.,58.,0.)
      enddo
      
      call hcdir('//PAWC',' ')
     
20    format(a7,i1)
       
      end


c-----------------------------------------------------------


      subroutine sc_bor

      include "sccal.inc"
      
      
      character*132 file_in,dumc
      integer pedl,pedr,id,sec,s,i,dumi
      real rms
      
      file_in='/usr/local/clas/parms/pedman/Tfiles/sc.tped'

      open(unit=12,file=file_in,status='old')

      read(12,*) dumc
      read(12,*) dumc
      read(12,*) dumi
c->
c      read(12,*) dumc

      do s=1,6
        read(12,*) dumc
c->
c        read(12,*) dumc

c        do i=1,48
c        write (*,*) ' EP sector ',s
        do i=1,57
          read(12,*) id,sec,pedl,rms,pedr,rms
          scped(id,1,sec+1) = pedl
          scped(id,2,sec+1) = pedr
c          write(*,*)id,sec,pedl,pedr
c          write(*,*) scped(id,1,sec+1),scped(id,2,sec+1)
       enddo
      enddo

      close(12)
      
      return
      end


c-----------------------------------------------------------


      subroutine sc_done

c  called at done

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine sc_eor

c  called at eor

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine sc_fill
      
      include 'SCnt.inc'
      include 'sccal.inc'
      
      real sector,bar,tdcl,adcl,tdcr,adcr, tdclr, slrid
      real sclposa,sclpost,ampmean,posaohl,postohl,posdfta
      real sl,sr
      integer id1,id2,j,id3
      integer lrid

      logical logposa,logpost
      integer maxpaddl
c      parameter( maxpaddl = 48 )
      parameter( maxpaddl = 57 )
      real scaleposa(maxpaddl), scalepost(maxpaddl)
      real tmincut, tmaxcut, amincut, amaxcut
      data tmincut,tmaxcut,amincut,amaxcut/500.,8000.,30.,8191./
      
c      tmincut...., are the cut ranges for using calculated TDCs&/ADCs
c      scaleposa=ATNEFF/LENGTH(id) is the scale for pos. from ADC
c      scalepost=SCLEFF*VELEFF/LENGTH(id) scale for pos. from TDC
c      IF the numbers are exact then positions vary from -1 to +1
c      SCLEFF is the effective nsec/counts for the TDCs; .045 ns/ct
c      VELEFF is the effective velocity scintillator; 16 cm/ns
c      ATNEFF is the effective attenuation length; 300 cm
c      LENGTH(id) is the length of scintillator in cm with id=1:48

c      The scintillator lengths for id=1:48 are respectively::
c      32.29, 48.14, 64.00, 79.85, 95.71, 106.56,122.42,138.27,
c      154.13,169.98,185.84,201.69,217.55,233.40,249.26,265.11,
c      280.97,296.82,312.67,328.53,344.38,360.24,376.10,371.30,
c      378.16,385.01,391.87,398.73,405.61,412.47,419.33,426.19,
c      433.05,439.90,445.06,439.32,433.55,427.81,422.05,413.42,
c      401.90,390.40,371.79,338.70,305.59,272.49,241.60,218.36

c 0ld 48 paddle data
c      data scaleposa/ 9.291, 6.232, 4.688, 3.757, 3.134, 2.815,
c     & 2.451, 2.170, 1.946, 1.765, 1.614, 1.487, 1.379, 1.285,
c     & 1.204, 1.132, 1.068, 1.011, .9595, .9132, .8711, .8328,
c     & .7977, .8080, .7933, .7792, .7656, .7524, .7396, .7273,
c     & .7154, .7039, .6928, .6820, .6741, .6829, .6920, .7012,
c     & .7108, .7257, .7465, .7684, .8069, .8857, .9817, 1.101,
c     & 1.242, 1.374/

c new 57 paddle data
      data scaleposa/ 9.291, 6.232, 4.688, 3.757, 3.134, 2.815,
     & 2.451, 2.170, 1.946, 1.765, 1.614, 1.487, 1.379, 1.285,
     & 1.204, 1.132, 1.068, 1.011, .9595, .9132, .8711, .8328,
     & .7977, .8080, .7933, .7792, .7656, .7524, .7396, .7273,
     & .7154, .7039, .6928, .6820, .6741, .6829, .6920, .7012,
     & .7108, 
     & .7257, .7257, .7465, .7465, .7684, .7684, .8069, .8069,
     & .8857, .8857, .9817, .9817, 1.101, 1.101,
     & 1.242, 1.242, 1.374, 1.374/



c 0ld 48 paddle data     
c      data scalepost/ .02230, .01496, .01125, .009017, .007523,
c     & .006757, .005882, .005208, .004670, .004236, .003874,
c     & .003698, .003310, .003084, .002890, .002717, .002563,
c     & .002426, .002303, .002192, .002091, .001999, .001914,
c     & .001939, .001904, .001870, .001837, .001806, .001775,
c     & .001746, .001717, .001689, .001663, .001637, .001618,
c     & .001639, .001661, .001683, .001706, .001742, .001791,
c     & .001844, .001937, .002126, .002356, .002642, .002980,
c     & .003297/
 
c new 57 paddle data
      data scalepost/ .02230, .01496, .01125, .009017, .007523,
     & .006757, .005882, .005208, .004670, .004236, .003874,
     & .003698, .003310, .003084, .002890, .002717, .002563,
     & .002426, .002303, .002192, .002091, .001999, .001914,
     & .001939, .001904, .001870, .001837, .001806, .001775,
     & .001746, .001717, .001689, .001663, .001637, .001618,
     & .001639, .001661, .001683, .001706,
     & .001742, .001742, .001791, .001791,
     & .001844, .001844, .001937, .001937, .002126, .002126,
     & .002356, .002356, .002642, .002642, .002980, .002980,
     & .003297, .003297/


    
      if (nSCT.gt.0) then
         call hcdir('//PAWC/SC',' ')
         do j=1,nSCT
            sector = float(secSCT(j))
            if(idSCT(j).gt.57) then
               bar = float(idSCT(j)-256)
               lrID=1           !right
            else if(idSCT(j).le.57) then
               bar = float(idSCT(j))
               lrID=0           !left
            endif
            tdclr=float(TDCSCT(j))
            slrid=(sector-1)*2.+lrID+1
c            call hf2(9,1.*idSCT(j),sector,1.)
            call hf2(10,bar,slrid,1.)
            id1 = 10*(lrID+1)+sector
            call hf2(id1,tdclr,bar,1.)
         enddo
         call hcdir('//PAWC',' ')
      endif
      
        
      if (nSC.gt.0) then
      call hcdir('//PAWC/SC',' ')

      call hf1(7,nSC*1.,1.)
      
      do j = 1,nSC
        sector = float(secsc(j))
        bar    = float(idsc(j))
        tdcl   = float(tdclsc(j)) 
        adcl   = float(adclsc(j))-scped(bar,1,sector) 
        tdcr   = float(tdcrsc(j)) 
        adcr   = float(adcrsc(j))-scped(bar,2,sector)
        
        id1    = 10*sector
        id2    = 100*sector
        id3    = 1000*sector
        
        sl     = (sector-1)*2+1
        sr     = sl+1
        
        if (tdcl.gt.tmincut.and.tdcl.lt.tmaxcut) then
          call hf2(2,bar,sl,1.)
          call hf2(3,bar,sl,tdcl)
          call hf2(1,sector,5.,1.)
        endif
        if (adcl.gt.amincut.and.adcl.lt.amaxcut) then
          call hf2(4,bar,sl,1.)
          call hf2(5,bar,sl,adcl)
          call hf2(1,sector,7.,1.)
        endif
        if (tdcr.gt.tmincut.and.tdcr.lt.tmaxcut) then
          call hf2(2,bar,sr,1.)
          call hf2(3,bar,sr,tdcr)
          call hf2(1,sector,6.,1.)
        endif
        if (adcr.gt.amincut.and.adcr.lt.amaxcut) then
          call hf2(4,bar,sr,1.)
          call hf2(5,bar,sr,adcr)
          call hf2(1,sector,8.,1.)
        endif
        
        call hf2(id2+2,tdcl,bar,1.)
        call hf2(id2+3,adcl,bar,1.)
        call hf2(id2+4,tdcr,bar,1.)
        call hf2(id2+5,adcr,bar,1.)
        call hf2(id2+6,adcl,bar,1.)
        call hf2(id2+7,adcr,bar,1.)
        
        logposa = .FALSE.
        logpost = .FALSE.
        
        sclposa = scaleposa(idsc(j))
        sclpost = scalepost(idsc(j))        

        if(adcl.GT.amincut.AND.adcl.LT.amaxcut.AND.
     &     adcr.GT.amincut.AND.adcr.LT.amaxcut) then
          logposa = .TRUE.
          ampmean = SQRT(adcl*adcr)
          posaohl = sclposa*LOG(adcr/adcl)
          call hf2(id3+1,ampmean,bar,1.)
          call hf2(id3+2,posaohl,bar,1.)
        endif
  
        if(tdcl.GT.tmincut.AND.tdcl.LT.tmaxcut.AND.
     &     tdcr.GT.tmincut.AND.tdcr.LT.tmaxcut) then 
          logpost = .TRUE.
          postohl = sclpost*(tdcl-tdcr)
          call hf2(id3+3,postohl,bar,1.)
        endif
          
        if (logposa.AND.logpost) then
          posdfta = postohl - posaohl
          call hf2(id3+4,posdfta,bar,1.)
        endif
      enddo 
      
      call hcdir('//PAWC',' ')
      endif
      
      end
      subroutine sc_init

c  called at init

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine scstore
      include 'bcs.inc'
      include 'SCnt.inc'      
      integer name_i,ind,nrow,nword,i,j,k,sector,ifirst,mamind
      integer name_it,ind_t
      integer*2 I16(2000)            ! 16 bits
      integer*4 I32(1000)            ! 32 bits

      integer IAND

      data ifirst/0/ 
      equivalence (I16(1), I32(1))
      save name_i, name_it
      
      if (ifirst.eq.0) then
         name_i = mamind(iw,'SC  ')
         name_it= mamind(iw,'SCT ')
         if (name_i.eq.0) then
            print *,' NO SC bank registered'
         endif
         if (name_it.eq.0) then
            print *,' NO SCT bank registered'
         endif
         ifirst = 1
      endif
      
cccccccccccccccccccc

      if(name_it.ne.0) then
         ind=iw(name_it)
         nSCT = 0
         
 50      if (ind.ne.0) then
            
            nword   = iw(ind)
            sector  = iw(ind-2)
            nrow    = iw(ind-4)
            do j=1,nword
               I32(j)=IW(ind+j) ! bos stored in 32 bit
            enddo
            
            k = 0            
            do i = 1, nrow 
               nSCT 		= nSCT + 1
               secSCT(nSCT )	= sector
               k                = k + 1
               idSCT(nSCT) 	= I16( k )
               idSCT(nSCT) 	= IAND(idSCT(nSCT),'000001FF'x)
               k                = k + 1
               TDCSCT(nSCT )    = I16( k )
            enddo
            ind = iw(ind-1)
            goto 50
            
         endif
      endif

cccccccccccccc

      if(name_i.ne.0) then
         ind = iw(name_i)
         nSC = 0
         
 30      if (ind.ne.0) then
            
            nword   = iw(ind)
            sector  = iw(ind-2)
            nrow    = iw(ind-4)
            
            do j=1,nword
               I32(j)=IW(ind+j) ! bos stored in 32 bit
            end do
            
            k = 0
            
            do i = 1, nrow 
               nSC 		= nSC + 1
               secSC(nSC )	= sector
               k 		= k + 1              
c              idSC (nSC ) 	= min(48,max(1,I16( k ))) 
               idSC (nSC) 	= I16( k )
               idSC (nSC) 	= IAND(idSC(nSC),'000000FF'x)

               k 		= k + 1
               TDCLSC (nSC ) = min(8000,max(1,I16( k )))
               k 		= k + 1
               ADCLSC (nSC ) = min(8191,max(1,I16( k )))
               k 		= k + 1
               TDCRSC (nSC ) = min(8000,max(1,I16( k )))
               k 		= k + 1
               ADCRSC (nSC ) = min(8191,max(1,I16( k ))) 
            enddo
            
            ind = iw(ind-1)
            goto 30
            
         endif
      endif


      
      return
      end
      

c-----------------------------------------------------------
