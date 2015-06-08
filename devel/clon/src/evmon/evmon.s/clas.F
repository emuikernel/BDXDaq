c  clas.f

c  clas fortran steering routines for event_monitor

c  ejw, 27-may-1998


c-----------------------------------------------------------


      subroutine clas_init(no_global,global_sect_name)

c  steering routine for package initialization

c  ejw, 12-may-1998


      implicit none
      save

      include 'event_monitor.inc'

      integer space
      parameter (space=8000000)
      integer mem
      common/pawc/mem(space)

      logical no_global
      character *(*)global_sect_name


c  executable code:
c  ----------------

      if(no_global)then
         call hlimit(space)
      else
         call hlimap(space,global_sect_name)
      endif
      
      if(.not.no_scaler)call scaler_init

      if(no_hist)return

      if(.not.no_cc)call cc_init
      if(.not.no_dc)call dc_init
      if(.not.no_ec)call ec_init
      if(.not.no_ec)call ect_init
      if(.not.no_sc)call sc_init
      if(.not.no_st)call st_init
      if(.not.no_ic)call ic_init
      if(.not.no_lac)call ec1_init
      if(.not.no_trig)call trig_init
      if(.not.no_tg)call tagger_init
      if(.not.no_call)call call_init
      if(.not.no_l2)call level2_init
      if(.not.no_photon)call photon_init
      if(.not.no_rast)call rast_init
      if(.not.no_tpc)call tpc_init
      if(.not.no_tpe)call tpe_init
      print *, ' Done with clas_init' 
      return
      end


c-----------------------------------------------------------


      subroutine clas_book

c  steering routine for all histogram booking

c  ejw, 12-may-1998


      implicit none
      save

      include 'event_monitor.inc'


c  executable code:
c  ----------------


      if(no_hist)return

      if(.not.no_cc)call cc_book
      if(.not.no_dc)call dc0_book
      if(.not.no_dc)call dc_book
      if(.not.no_ec) call ec_book      
      if(.not.no_ec) call ect_book
      if(.not.no_sc)call sc_book
      if(.not.no_st)call st_book
      if(.not.no_ic)call ic_book
      if(.not.no_lac)call ec1_book
      if(.not.no_tg)call tagger_book
      if(.not.no_trig)call trig_book
      if(.not.no_photon)call photon_book
      if(.not.no_call)call call_book
      if(.not.no_l2)call level2_book
      if(.not.no_scaler)call scaler_book
      if(.not.no_rast)call rast_book
      if(.not.no_tpc)call tpc_book
      print *, 'about to book TPE stuff'
      if(.not.no_tpe)call tpe_book

      return
      end


c-----------------------------------------------------------


       subroutine clas_bor

c  called at beginning of run

c  ejw, 12-may-1998


      implicit none
      save

      include 'event_monitor.inc'


c  executable code:
c  ----------------

      if(.not.no_scaler)call scaler_bor

      if(no_hist)return

      if(.not.no_cc)call cc_bor
      if(.not.no_dc)call dc_bor
      if(.not.no_sc)call sc_bor
      if(.not.no_st)call st_bor
      if(.not.no_ic)call ic_bor
      if(.not.no_ec)call ec_bor
      if(.not.no_ec)call ect_bor
      if(.not.no_lac)call ec1_bor
      if(.not.no_trig)call trig_bor
      if(.not.no_tg)call tagger_bor
      if(.not.no_photon)call photon_bor
      if(.not.no_call)call call_bor
      if(.not.no_l2)call level2_bor
      if(.not.no_rast)call rast_bor
      if(.not.no_tpc)call tpc_bor
      if(.not.no_tpe)call tpe_bor

      return
      end


c-----------------------------------------------------------


      subroutine clas_process

c  steering routine for event processing

c  ejw, 12-may-1998

      implicit none
      save

      include 'event_monitor.inc'


c  executable code:
c  ----------------

      if(.not.no_scaler)call scalerstore

      if(no_hist)return

      if(.not.no_dc)call dc0store
      if(.not.no_cc)call ccstore
      if(.not.no_sc)call scstore
      if(.not.no_ec)call ecstore
      if(.not.no_ec)call ectstore
      if(.not.no_lac)call ec1store
      if(.not.no_st)call ststore
      if(.not.no_ic)call icstore
      if(.not.no_call)call callstore 
      if(.not.no_photon)call photonstore 
      if(.not.no_rast)call raststore 
      if(.not.no_tpc)call tpcstore
      if(.not.no_tpe)call tpestore      

      call clas_fill
      
      return
      end
      
      
c-----------------------------------------------------------
      
      
      subroutine clas_fill
      
c  fills histograms
      
c  ejw, 1-oct-96
      
      
      implicit none
      save
      
      
      include "event_monitor.inc"
      
      
c  executable code:
c  ----------------
      

      if(no_hist)return

      if(.not.no_cc)call cc_fill
      if(.not.no_dc)call dc0_fill
      if(.not.no_dc)call dc_fill
      if(.not.no_sc)call sc_fill
      if(.not.no_st)call st_fill
      if(.not.no_ic)call ic_fill
      if(.not.no_ec)call ec_fill
      if(.not.no_ec)call ect_fill
      if(.not.no_lac)call ec1_fill
      if(.not.no_tg)call tagger_fill
      if(.not.no_trig)call trig_fill
      if(.not.no_call)call call_fill
      if(.not.no_l2)call level2_fill
      if(.not.no_photon)call photon_fill
      if(.not.no_scaler)call scaler_fill
      if(.not.no_rast)call rast_fill
      if(.not.no_tpc)call tpc_fill
      if(.not.no_tpe)call tpe_fill

      return
      end


c-----------------------------------------------------------


      subroutine clas_eor

c  ejw, 12-may-1998


      implicit none
      save


      include 'event_monitor.inc'


c  executable code:
c  ----------------

      if(.not.no_scaler)call scaler_eor

      if(no_hist)return

      if(.not.no_cc)call cc_eor
      if(.not.no_dc)call dc_eor
      if(.not.no_sc)call sc_eor
      if(.not.no_st)call st_eor
      if(.not.no_ic)call ic_eor
      if(.not.no_ec)call ec_eor
      if(.not.no_ec)call ect_eor
      if(.not.no_lac)call ec1_eor
      if(.not.no_trig)call trig_eor
      if(.not.no_tg)call tagger_eor
      if(.not.no_photon)call photon_eor
      if(.not.no_call)call call_eor
      if(.not.no_l2)call level2_eor
      if(.not.no_rast)call rast_eor
      if(.not.no_tpc)call tpc_eor
      if(.not.no_tpe)call tpe_eor

      return
      end


c----------------------------------------------------------------------------------


      subroutine clas_done

c  steering routine for package cleanup
c  writes out all hist if requested

c  ejw, 12-may-1998


      implicit none
      save

      include 'event_monitor.inc'


c  executable code:
c  ----------------

      if(.not.no_scaler)call scaler_done

      if(no_hist)return

      if(.not.no_cc)call cc_done
      if(.not.no_dc)call dc_done
      if(.not.no_sc)call sc_done
      if(.not.no_st)call st_done
      if(.not.no_ic)call ic_done
      if(.not.no_ec)call ec_done
      if(.not.no_ec)call ect_done
      if(.not.no_lac)call ec1_done
      if(.not.no_trig)call trig_done
      if(.not.no_tg)call tagger_done
      if(.not.no_photon)call photon_done
      if(.not.no_call)call call_done
      if(.not.no_l2)call level2_done
      if(.not.no_rast)call rast_done
      if(.not.no_tpc)call tpc_done
      if(.not.no_tpe)call tpe_done

      return
      end


c-----------------------------------------------------------
