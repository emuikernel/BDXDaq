c----------------------------- 
c  TPC histograms, 
c  R. De Vita, Oct 2009
c-----------------------------
c
c=================================================================
      subroutine TPC_book
c=================================================================

      implicit none
      save

      call hmdir('//PAWC/TPC','S')
c
      call hbook2(101,'TPC Hits '  ,40,0.,40,80,0.,80.,0.)
      call hbook2(102,'TPC Charge ',40,0.,40,80,0.,80.,0.)
      call hbook1(103,'TPC Hits '  ,3200,0.,3200.,0.)
      call hbook1(104,'TPC Charge'  ,3200,0.,3200.,0.)

c
      call hcdir('//PAWC',' ')
      return
      end     


c=================================================================
      subroutine TPC_bor
c=================================================================

c  called at done

      implicit none
      save


c  executable code:
c  ----------------

      return
      end

c=================================================================
      subroutine TPC_done
c=================================================================

c  called at done

      implicit none
      save


c  executable code:
c  ----------------

      return
      end


c=================================================================
      subroutine TPC_eor
c=================================================================

c  called at eor


      implicit none
      save


c  executable code:
c  ----------------

      return
      end
c


c=================================================================
      subroutine TPC_fill
c=================================================================
      
      implicit none
      save
      
      include "TPC.inc"
      integer i
c    --------------------

      if(iTPC_ok.eq.1) then   
c
         call hcdir('//PAWC/TPC',' ')
c
         do i=1,nTPC
            call hf2(101, float(colTPC(i)),float(rowTPC(i)),1.)
            call hf2(102, float(colTPC(i)),float(rowTPC(i)),float(adcTPC(i)))
            call hf1(103, float(idTPC(i)),1.)
            call hf1(104, float(idTPC(i)),float(adcTPC(i)))
         enddo
c
        call hcdir('//PAWC',' ')
      end if
c
      return
      end
c-----------------------------------------------------------


c=================================================================
      subroutine TPC_init
c=================================================================
c
c  called at initial stage
c 
      implicit none
      save


c  executable code:
c  ----------------

      END


c=================================================================
      subroutine TPCstore
c=================================================================

c
      implicit none
      save
c
      include "bcs.inc"
      include "TPC.inc"
c      
      integer i,ii,jj,ind,nrow,ncol,ix,indx
      integer mlink
      integer TPCword
      integer ihit,nhit,ncha,time
      integer seq_col(2,16)
      integer seq_row(2,16)
      integer conn_num,conn_row,conn_col,pad_seq,odd
      data seq_col/4,1,5,3,2,4,3,6,7,2,7,0,1,6,0,5,1,4,3,5,4,2,6,3,2,7,0,7,6,1,5,0/
      data seq_row/0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,0/

      itpc_ok=0
      i=0
      ii=0
      nTPC=0
      DO jj=0,3
         ind = MLINK(IW,'TPC6',jj)
         IF ( ind.NE.0 ) THEN
            nrow = iw(ind-4)
            ncol = iw(ind-5)
            if(nrow.ge.15000) then
               nrow=15000       ! maximum number of hits per sector
            endif
            indx=2*ind
            Do While(indx.lt.2*ind+nrow)
               indx=indx+1
               ncha = iw16(indx)
               indx=indx+1
               time = iw16(indx)
               indx = indx+1
               nhit = iw16(indx)
               do ihit=1,nhit
                  i=i+1
                  indx=indx+1
                  secTPC(i) = jj
                  idTPC (i) = ncha                                 
                  tdcTPC(i) = time-(ihit-1)
                  adcTPC(i) = iw16(indx)
                  sideTPC(i)= idTPC(i)/1600     ! left=0, right=1
                  padTPC(i) = idTPC(i)-sideTPC(i)*1600 ! now 0-1599
                  conn_num= padTPC(i)/16 ! 100 (0-99) connectors on each rtpc
                  conn_row= conn_num/5 ! 5 connectors per row (20 row: 0-19)
                  conn_col= conn_num-5*conn_row ! which of the 20 connector columns (0-4)
                  pad_seq=  padTPC(i)-16*conn_num +1 ! sequence number within connector: 0-15
                  odd= conn_num-2*int(conn_num/2)+1 ! even/odd connectors use different channel orders
                  colTPC(i)=  8*conn_col + seq_col(odd,pad_seq) ! ( (0-32) + (0-7) )
                  rowTPC(i)=  2*conn_row + seq_row(odd,pad_seq)+40*sidetpc(i) ! ( (0-38) + (0-1) )                  
                  nTPC=i
               enddo
               if(nTPC.gt.0) itpc_ok=1
            EndDo
         ENDIF
      ENDDO

  

      return
      end




