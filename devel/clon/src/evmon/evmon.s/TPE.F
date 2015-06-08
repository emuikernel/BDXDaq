c  tpe.F

c  ejw, 13-may-98
c  puneetk, 20-nov-2010

c-----------------------------------------------------------


      subroutine tpe_book


      implicit none
      save


c  executable code:
c  ----------------
      include "TPEnt.inc"

c      print *, 'Booking TPE histograms'      

      call hmdir('//PAWC/TPE','S')

      call hbook2(1,'TPE ADC population',6,0.5,6.5,6,0.5,6.5,0.)
      call hbook2(2,'TPE TDC population',6,0.5,6.5,6,0.5,6.5,0.)
      call hbook1(3,'TPE IDs',32, 0.,31.,0.) 

      call hbook2(4, 'TPE;ADC;ID', 800,0.,8000.,31,0.5,31.5,0.)
      call hbook2(5, 'TPE;TDC;ID', 700,0.,7000.,31,0.5,31.5,0.)
      
      call hbook1(101, 'ADC1', 200, 0., 8000., 0.);
      call hbook1(102, 'ADC2', 200, 0., 8000., 0.);
      call hbook1(103, 'ADC3', 200, 0., 8000., 0.);
      call hbook1(104, 'ADC4', 200, 0., 8000., 0.);
      call hbook1(105, 'ADC5', 200, 0., 8000., 0.);
      call hbook1(106, 'ADC6', 200, 0., 8000., 0.);
      call hbook1(107, 'ADC7', 200, 0., 8000., 0.);
      call hbook1(108, 'ADC8', 200, 0., 8000., 0.);
      call hbook1(109, 'ADC9', 200, 0., 8000., 0.);
      call hbook1(110, 'ADC10', 200, 0., 8000., 0.);
      call hbook1(111, 'ADC11', 200, 0., 8000., 0.);
      call hbook1(112, 'ADC12', 200, 0., 8000., 0.);
      call hbook1(113, 'ADC13', 200, 0., 8000., 0.);
      call hbook1(114, 'ADC14', 200, 0., 8000., 0.);
      call hbook1(115, 'ADC15', 200, 0., 8000., 0.);
      call hbook1(116, 'ADC16', 200, 0., 8000., 0.);
      call hbook1(117, 'ADC17', 200, 0., 8000., 0.);
      call hbook1(118, 'ADC18', 200, 0., 8000., 0.);
      call hbook1(119, 'ADC19', 200, 0., 8000., 0.);
      call hbook1(120, 'ADC20', 200, 0., 8000., 0.);
      call hbook1(121, 'ADC21', 200, 0., 8000., 0.);
      call hbook1(122, 'ADC22', 200, 0., 8000., 0.);
      call hbook1(123, 'ADC23', 200, 0., 8000., 0.);
      call hbook1(124, 'ADC24', 200, 0., 8000., 0.);
      call hbook1(125, 'ADC25', 200, 0., 8000., 0.);
      call hbook1(126, 'ADC26', 200, 0., 8000., 0.);
      call hbook1(127, 'ADC27', 200, 0., 8000., 0.);
      call hbook1(128, 'ADC28', 200, 0., 8000., 0.);
      call hbook1(129, 'ADC29', 200, 0., 8000., 0.);
      call hbook1(130, 'ADC30', 200, 0., 8000., 0.);
      call hbook1(131, 'ADC31', 200, 0., 8000., 0.);

      call hbook1(201, 'TDC1', 200, 0., 7000., 0.);
      call hbook1(202, 'TDC2', 200, 0., 7000., 0.);
      call hbook1(203, 'TDC3', 200, 0., 7000., 0.);
      call hbook1(204, 'TDC4', 200, 0., 7000., 0.);
      call hbook1(205, 'TDC5', 200, 0., 7000., 0.);
      call hbook1(206, 'TDC6', 200, 0., 7000., 0.);
      call hbook1(207, 'TDC7', 200, 0., 7000., 0.);
      call hbook1(208, 'TDC8', 200, 0., 7000., 0.);
      call hbook1(209, 'TDC9', 200, 0., 7000., 0.);
      call hbook1(210, 'TDC10', 200, 0., 7000., 0.);
      call hbook1(211, 'TDC11', 200, 0., 7000., 0.);
      call hbook1(212, 'TDC12', 200, 0., 7000., 0.);
      call hbook1(213, 'TDC13', 200, 0., 7000., 0.);
      call hbook1(214, 'TDC14', 200, 0., 7000., 0.);
      call hbook1(215, 'TDC15', 200, 0., 7000., 0.);
      call hbook1(216, 'TDC16', 200, 0., 7000., 0.);
      call hbook1(217, 'TDC17', 200, 0., 7000., 0.);
      call hbook1(218, 'TDC18', 200, 0., 7000., 0.);
      call hbook1(219, 'TDC19', 200, 0., 7000., 0.);
      call hbook1(220, 'TDC20', 200, 0., 7000., 0.);
      call hbook1(221, 'TDC21', 200, 0., 7000., 0.);
      call hbook1(222, 'TDC22', 200, 0., 7000., 0.);
      call hbook1(223, 'TDC23', 200, 0., 7000., 0.);
      call hbook1(224, 'TDC24', 200, 0., 7000., 0.);
      call hbook1(225, 'TDC25', 200, 0., 7000., 0.);
      call hbook1(226, 'TDC26', 200, 0., 7000., 0.);
      call hbook1(227, 'TDC27', 200, 0., 7000., 0.);
      call hbook1(228, 'TDC28', 200, 0., 7000., 0.);
      call hbook1(229, 'TDC29', 200, 0., 7000., 0.);
      call hbook1(230, 'TDC30', 200, 0., 7000., 0.);
      call hbook1(231, 'TDC31', 200, 0., 7000., 0.);




c      call hbpro(1,0.)
c      call hbpro(2,0.)
c      call hbpro 

      call hcdir('//PAWC',' ')

      return
      end


c------------------------------------------------------------------------------


      subroutine tpe_bor

c  called at bor

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

c      print *, 'TPE beginning of run'

      return
      end


c-----------------------------------------------------------


      subroutine tpe_done

c  called at done

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

c      print *, 'TPE Done'

      return
      end


c-----------------------------------------------------------


      subroutine tpe_eor

c  called at eor

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

c      print *, 'TPE end of run'

      return
      end


c-----------------------------------------------------------


      subroutine tpe_fill

      implicit none
      save

      include 'bcs.inc'
      include 'TPEnt.inc'

      integer i
      real x,y
      integer row, col, val
      integer mod

c  executable code:
c  ----------------
      call hcdir('//PAWC/TPE',' ')    

c      print *, 'TPE Filling histograms...'
  

c fills ADC hists
c      print *, 'nadchits ', nadchits
      y = 1.5
      if (nadchits .gt. 0) then         
         do i=1,nadchits
            row = (idadc(i)-1)/MAXX + 1
            col = mod(idadc(i),MAXX)
            if (col .eq. 0) col = MAXX
            call hf2(1,col*1.,row*1.,1.)
            call hf2(4,iadc(i)*1.,idadc(i)*1.,1.)
            call hf1(idadc(i)+100,iadc(i)*1., 1.)
         enddo
      endif
c     fills TDC hists
      if (ntdchits .gt. 0) then
         do i=1,ntdchits
            row = (idtdc(i)-1)/MAXX + 1
            col = mod(idtdc(i),MAXX)
            if (col .eq. 0) col = MAXX
            call hf2(2,col*1.,row*1.,1.)
            call hf2(5,itdc(i)*1.,idtdc(i)*1.,1.)
            call hf1(idtdc(i)+200,itdc(i)*1., 1.)
         enddo
      endif

      call hcdir('//PAWC',' ')

      return
      end


c------------------------------------------------------------------------------


      subroutine tpe_init

c  called at init

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

c      print *, 'TPE initializing'

      return
      end


c-----------------------------------------------------------

      subroutine tpestore
      implicit none
      include 'bcs.inc'
      include 'TPEnt.inc'
      include "event_monitor.inc"

      integer mamind
      
      integer name_i,ind,nrow,nword,ncol,i,j,k,sector,ifirst,mlink
      integer name_r
      integer name_tpe0, name_tpe1
      data ifirst/0/ 
      save name_tpe0, name_tpe1, ifirst 


c      print *, 'TPE Storing values'

c initialize arrays and parameters
      nadchits = 0
      ntdchits = 0
      do i=1,MAXADC
        iadc(i) = 0
        idadc(i) = 0
        itdc(i) = 0
        idtdc(i) = 0
      enddo

c get ADCs
      
      if (ifirst.eq.0) then
        name_tpe1 = mamind(iw,'TPE1',0)
        if (name_tpe1.eq.0) then
          print *,' NO TPE1 bank registered'
        endif
        name_tpe0 = mamind(iw,'TPE0',0)
        if (name_tpe0.eq.0) then
          print *,' NO TPE0 bank registered'
        endif
        ifirst = 1
      endif
      
      ind = iw(name_tpe1)
      if (ind.ne.0) then      
        nword   = iw(ind)
        nrow    = iwrow(ind)
        k = 2*ind
        do i=1, nrow 
          nadchits = nadchits + 1
          idadc(i) = IW16(k+1)
          iadc(i) = IW16(k+2)
          k = k + 2
        enddo
        
      endif

c get TDCs
      
     
      ind = iw(name_tpe0)
      if (ind.ne.0) then      
        nword   = iw(ind)
        nrow    = iwrow(ind)
        k = 2*ind
        do i = 1, nrow 
          ntdchits = ntdchits + 1
          idtdc(ntdchits) = IW16(k+1)
          itdc(ntdchits) = IW16(k+2)
          k = k + 2
        enddo
        
      endif

      
      return
      end


c-----------------------------------------------------------









