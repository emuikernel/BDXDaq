c  ec1.f

c  ejw, 13-may-98


c-----------------------------------------------------------


      subroutine ec1_book
      
      character*80 title
      character*5 title2,title3,title4
      character*12 fstring(2)
      character*5 title1
      character*8 text1,text2,chann
      integer i

      do i = 1,2
        write(fstring(i),20) 'LAC sect. ',i
      enddo
      
      call hmdir('//PAWC/EC1','S')
      title1 = ' ADCL'
      title2 = ' ADCR'
      title3 = ' TDCL'
      title4 = ' TDCR'
      text1  = ' Inner'
      text2  = ' Outer'
      chann  = ';channel'
      do i = 1,2
        title = fstring(i)//text1//title1//chann
        call hbook1(i*10+1, title,64,1.,65.,0.)
        title = fstring(i)//text1//';'//title1//chann
        call hbook2(i*100+1,title,100,0.,4095.,64,1.,65.,0.)
        title = fstring(i)//text1//title2//chann
        call hbook1(i*10+2, title,64,1.,65.,0.)
        title = fstring(i)//text1//';'//title2//chann
        call hbook2(i*100+2,title,100,0.,4095.,64,1.,65.,0.)
        title = fstring(i)//text2//title1//chann
        call hbook1(i*10+3, title,64,1.,65.,0.)
        title = fstring(i)//text2//';'//title1//chann
        call hbook2(i*100+3,title,100,0.,4095.,64,1.,65.,0.)
        title = fstring(i)//text2//title2//chann
        call hbook1(i*10+4, title,64,1.,65.,0.)
        title = fstring(i)//text2//';'//title2//chann
        call hbook2(i*100+4,title,100,0.,4095.,64,1.,65.,0.)
        title = fstring(i)//text1//title3//chann
        call hbook1(i*10+5, title,64,1.,65.,0.)
        title = fstring(i)//text1//';'//title3//chann
        call hbook2(i*100+5,title,100,0.,4095.,64,1.,65.,0.)
        title = fstring(i)//text1//title4//chann
        call hbook1(i*10+6, title,64,1.,65.,0.)
        title = fstring(i)//text1//';'//title4//chann
        call hbook2(i*100+6,title,100,0.,4095.,64,1.,65.,0.)
        title = fstring(i)//text2//title3//chann
        call hbook1(i*10+7, title,64,1.,65.,0.)
        title = fstring(i)//text2//';'//title3//chann
        call hbook2(i*100+7,title,100,0.,4095.,64,1.,65.,0.)
        title = fstring(i)//text2//title4//chann
        call hbook1(i*10+8, title,64,1.,65.,0.)
        title = fstring(i)//text2//';'//title4//chann
        call hbook2(i*100+8,title,100,0.,4095.,64,1.,65.,0.)
      enddo
      title = 'Overall ADC population;channel;board'
      call hbook2(1000,title,64,1.,65.,8,1.,9.,0.)
      title = 'Overall TDC population;channel;board' 
      call hbook2(2000,title,64,1.,65.,8,1.,9.,0.)
      call hcdir('//PAWC',' ')
     
20    format(a10,i1)

      end
       

c-----------------------------------------------------------


      subroutine ec1_bor

c  called at bor

c  ejw, 10-oct-96


      implicit none
      save


      include "event_monitor.inc"


c  executable code:
c  ----------------

      print *,'Read EC1 pedestals from Map - Run ',current_run
c
c    read the pedestal from $CLAS_PARMS/LAC_PEDESTALS.map
c
      print *
      print *
      print *,' *** NOT calling ec1readpedmap ***'
      print *
      print *

cccc???      call  ec1readpedmap(current_run)   ???
c
c      to read the pedestal from /usr/local/clas/parms/initfiles/LAC.spar
c      uncomment the following 3 lines
c
c       print *,'Read EC1 pedestals from LAC.spar - Run ',current_run 
c       ec1readpedfile_(current_run)

      return
      end



c-----------------------------------------------------------


      subroutine ec1_done

c  called at done

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine ec1_eor

c  called at eor

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine ec1_fill
      
      include 'EC1nt.inc'
      
      real sector,layer,strip,chan,tdcl,adcl,tdcr,adcr
      integer id1,id2,j,ec1off(4),ichan
      
      data ec1off/0,24,0,24/
            
      if (nEC1.gt.0) then
      call hcdir('//PAWC/EC1',' ')

      do j = 1, nEC1
        sector = float(secec1(j))
        layer  = float(layerec1(j))
        strip  = float(stripec1(j))
        ichan = strip + ec1off(layer)
        chan   = float(ichan)
        tdcl   = float(tdclec1(j))
        adcl   = float(adclec1(j))
        tdcr   = float(tdcrec1(j))
        adcr   = float(adcrec1(j))
        id1    =  10*sector
        id2    = 100*sector

                
c 1- and 2-dim histograms 
c  Layers 1+2 --> ADC board n
        if(adcl.gt.0..and.layer.lt.2.5) then
           call hf1(id1+1,chan,1.)
           call hf2(id2+1,adcl,chan,1.)
           call hf2(1000,chan,(sector-1)*4+1.,1.)
        endif
        if(adcr.gt.0..and.layer.lt.2.5) then
           call hf1(id1+2,chan,1.)
           call hf2(id2+2,adcr,chan,1.)
           call hf2(1000,chan,(sector-1)*4+2.,1.)
        end if
c  Layers 3+4 --> ADC board n
        if(adcl.gt.0..and.layer.gt.2.5) then
          call hf1(id1+3,chan,1.)  
          call hf2(id2+3,adcl,chan,1.)
          call hf2(1000,chan,(sector-1)*4+3.,1.)
        end if
        if(adcr.gt.0..and.layer.gt.2.5) then
          call hf1(id1+4,chan,1.)
          call hf2(id2+4,adcr,chan,1.)
          call hf2(1000,chan,(sector-1)*4+4.,1.)
        end if
c  Layer 1+2 --> TDC board 
        if(tdcl.gt.0..and.layer.lt.2.5) then
          call hf1(id1+5,chan,1.)
          call hf2(id2+5,tdcl,chan,1.)
          call hf2(2000,chan,(sector-1)*4+1.,1.)
        end if
        if(tdcr.gt.0..and.layer.lt.2.5) then
          call hf1(id1+6,chan,1.)
          call hf2(id2+6,tdcr,chan,1.)
          call hf2(2000,chan,(sector-1)*4+2.,1.)
        end if
c  Layer 3+4 --> TDC board 
        if(tdcl.gt.0..and.layer.gt.2.5) then
          call hf1(id1+7,chan,1.) 
          call hf2(id2+7,tdcl,chan,1.)
          call hf2(2000,chan,(sector-1)*4+3.,1.)
        end if
        if(tdcr.gt.0..and.layer.gt.2.5) then
          call hf1(id1+8,chan,1.)
          call hf2(id2+8,tdcr,chan,1.)
          call hf2(2000,chan,(sector-1)*4+4.,1.)
        end if

      enddo
      
      call hcdir('//PAWC',' ')
      endif
      
      end
       

c-----------------------------------------------------------


      subroutine ec1_init

c  called at init

c  ejw, 10-oct-96


      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c-----------------------------------------------------------


      subroutine ec1store
      include 'bcs.inc'
      include 'EC1nt.inc'
      include 'ec1_pedstl.inc'
      
      integer name_i,ind,nrow,nrec,i,k,sector,ifirst,mamind,id
      data ifirst/0/ 
      save name_i
      
      if (ifirst.eq.0) then
        name_i = mamind(iw,'EC1 ')
        if (name_i.eq.0) then
          print *,' NO EC1 bank registered'
          return
        endif
        ifirst = 1
      endif
      
      ind = iw(name_i)      
      nEC1    = 0

30    if (ind.ne.0) then
      
        nrec    = iw(ind)
        sector  = iw(ind-2)
        nrow    = iwrow(ind)
       
        k = 2*ind             ! k is pointed before data words for iw16 array
        do i = 1, nrow
          nEC1  = nEC1+1
          k     = k +1
          ID    = IW16(k)
          layerEC1(nEC1) = ID/256
          stripEC1(nEC1) = mod(ID,256)
          tdcLEC1 (nEC1) = IW16(k+1)
          adcLEC1 (nEC1) = IW16(k+2)-ec1_pedl(stripEC1(nEC1),layerEC1(nEC1),sector)
          tdcREC1 (nEC1) = IW16(k+3)
          adcREC1 (nEC1) = IW16(k+4)-ec1_pedr(stripEC1(nEC1),layerEC1(nEC1),sector)
          secEC1  (nEC1) = sector
          k     = k +4
         enddo
       
        ind = iw(ind-1)
        goto 30
        
      endif
      
      return
      end

