h32543
s 00006/00002/00122
d D 1.4 07/11/03 13:28:08 boiarino 5 4
c always call Bogdan's calib file for DC, do not use maps
c 
e
s 00002/00002/00122
d D 1.3 07/11/02 17:08:59 boiarino 4 3
c *** empty log message ***
e
s 00015/00502/00109
d D 1.2 03/04/17 16:52:10 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 01/11/19 19:02:00 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sda.s/sda_calib.F
e
s 00611/00000/00000
d D 1.1 01/11/19 19:01:59 boiarino 1 0
c date and time created 01/11/19 19:01:59 by boiarino
e
u
U
f e 0
t
T
I 1
C======================================================================
      SUBROUTINE sda_calib(RunNo)
C----------------------------------------------------------------------
C-
C-  Purpose and Methods : The initialization of the SDA Calibration Constants.
C-                        Reads the Data Base and stores in the "sdacalib.inc".
C-
C-   Inputs  : Calibration Data Base
C-   Outputs : Initializes the SDA include file "sdacalib.inc"
C-   Controls:
C-
C-   Library belongs: libsda.a
C-
C-   Calls: none
C-
C-   Created   5-JUN-1996   Bogdan Niczyporuk
C-
C-    Called by sda_brun
C-
C----------------------------------------------------------------------
      IMPLICIT NONE
C----------------------------------------------------------------------
C
      SAVE
C
#include "sdakeys.inc"
#include "sdageom.inc"
#include "sdacalib.inc"
D 3
**      real vflt
**      parameter (vflt=29.97925)
**      real cal_sc_(24,48,6), Elos_sc_, Ethr_sc_
**      real cal_ec_(6,36,6,6), Ethr_ec_
E 3
C
      COMMON ptc_dc
      REAL ptc_dc(192,36,6)
C
C Subroutine parameters
      INTEGER RunNo
C
C External
      INTEGER iucomp, ETNCREATE, ETNLINK
C
C Local variables
      INTEGER i,j, il, id, iv, is, ir, nsect, NR, iret, iwmin,iwmax
C
D 3
*       REAL sc_del, sc_delin
*       COMMON/SCDEL/sc_del(13,48,6), sc_delin(9,48,6)
C
E 3
      INTEGER dcst(6912), dc_st(192,36)
      REAL del,ddel,val, dc2tof, reg4pul(3), t0(6912),t0_dc(192,36)
      real delDC(6,6)
      EQUIVALENCE (t0(1),t0_dc(1,1)), (dcst(1),dc_st(1,1))
C
D 3
*      INTEGER statL(48,6), statR(48,6)
*      REAL pedL(48,6), pedR(48,6), t1L(48,6), t1R(48,6)
*     1    ,delL(48,6), delR(48,6), sc_p2p(48,6),sc_lr(48,6)
*     2    ,w0L(48,6),w0R(48,6), w1L(48,6),w1R(48,6), w2L(48,6),w2R(48,6)
*     3    ,t0L(48,6),t0R(48,6), t2L(48,6),t2R(48,6)
      REAL st_del(6), st_t1(6)
     1    ,st2tof, stdel(6), veff(6)          
E 3
I 3
      REAL st_del(6), st_t1(6), st2tof, stdel(6), veff(6)          
E 3
C
      INTEGER ind,utime,ierr,version,isec, nwtrg,nwst,nwdc,nwsc,nwec
      PARAMETER (nwtrg=4, nwst= 32, nwdc= 82952, nwsc= 6914, nwec =7777)
      REAL ctrig(nwtrg), cst(nwst), cdc(nwdc), csc(nwsc), cec(nwec)
      EQUIVALENCE (ctrig(1), Tdel)
      EQUIVALENCE (cst(1)  , cal_st(1,1))
      EQUIVALENCE (cdc(1)  , tc_dc(1,1,1))
      EQUIVALENCE (csc(1)  , cal_sc(1,1,1))
D 3
**      EQUIVALENCE (cec(1)  , cal_ec(1,1,1,1))
E 3
C
      INTEGER Np, Ng, nwtd
      PARAMETER (Ng = 200)
      PARAMETER (nwtd = 2*Ng*36+3*36)
      REAL ftd(nwtd)
      EQUIVALENCE (td_h(1,1),ftd(1))
      REAL ECped(36,6,6)
C
      CHARACTER*100 fname
      CHARACTER*24 utimes
D 3
*      CHARACTER*8  kname
E 3
      CHARACTER*7  SecNo(6)
      DATA SecNo /'sector1','sector2','sector3'
     1           ,'sector4','sector5','sector6'/
C
D 3
*      DATA delL /23*48.3, 25*38.2,  23*34.7, 25*27.6,  23*36.9,25*31.1,
*     1           23*41.5, 25*36.8,  23*35.0, 25*22.9,  23*37.1,25*28.1/
*     
*      DATA delR /23*44.3, 25*38.4,  23*36.0, 25*24.7,  23*34.3, 25*31.9,
*     1           23*37.6, 25*33.6,  23*38.2, 25*22.6,  23*34.1, 25*29.2/
C Dec97 Runs >= 7841
*      DATA delDC /1020., 1020.,  2150., 2140.,  3135., 3135.,
*     2            1020., 1020.,  2150., 2150.,  3135., 3150.,
*     3            1020., 1020.,  2050., 2050.,  3035., 3040.,
*     4            1020., 1020.,  2050., 2050.,  3035., 3105.,
*     5            1030., 1030.,  2050., 2050.,  3125., 3130.,
*     6            1020., 1020.,  2150., 2150.,  3135., 3140. /
C
*      DATA version /4/
*      DATA nsect   /6/

C bosio
*      INTEGER ndimb
*      PARAMETER (ndimb = 700000)
*      INTEGER IW(ndimb)
*      REAL RW(ndimb)
*      EQUIVALENCE (IW(1),RW(1))
*      CALL BOS(IW,ndimb)
C bosio

*      kname   = 'CSDA vs.'
C
C
E 3
C *** Initialization of calibration constants (temporary stuff)
C
I 3
      print *,'s1'
E 3
      IF(iucomp(4HINIT,lcali,10) .NE. 0) THEN
C
C Constants related to the TRIG
C
I 3
      print *,'s2'
E 3
        call triggetcalib(ltrig(1),Tdel,Tjit,DL1,vflt)
D 3
*        Tdel = 0.
*        Tjit = 8.0                   ! Sigma of TRIG jitter [ns]
*        DL1  = 1000.                 ! time delay for DC common TDC stop [ns]
*        vflt = 29.97925
E 3
I 3
      print *,'s3'
E 3
C
C Constants for ST
C
        call stgetcalib(ltrig(1),Elos_st,Ethr_st,cal_st)
D 3
*        Elos_st = 2.018592           ! energy loss [Mev/cm]
*        Ethr_st = 0.05               ! energy threshold [MeV]
*C for Start Counter "id" is a sector number
*        CALL map_get_float('/group/clas/parms/Maps/ST_CALIB.map'          
*     1  ,'delta_T', 'pair2pair',3, st_del   , 11578, iret)
*        CALL map_get_float('/group/clas/parms/Maps/ST_CALIB.map'          
*     1  ,'delta_T', 'side2side',3, st_del(4), 11578, iret)
*        CALL map_get_float('/group/clas/parms/Maps/ST_CALIB.map'          
*     1  ,'T1', 'value',6, st_t1, 11578, iret)
*        CALL map_get_float('/group/clas/parms/Maps/ST_CALIB.map'          
*     1  ,'st2tof', 'value',1, st2tof, 11578, iret)
*        CALL map_get_float('/group/clas/parms/Maps/ST_CALIB.map'          
*     1  ,'veff_leg', 'value',6, veff, 11578, iret)
*C
*        stdel(1) =st_del(1) + st2tof
*        stdel(2) = stdel(1) + st_del(4)
*        stdel(3) = stdel(1) + st_del(2)
*        stdel(4) = stdel(3) + st_del(5)
*        stdel(5) = stdel(1) + st_del(3)
*        stdel(6) = stdel(5) + st_del(6)
*        WRITE(6,11) stdel
* 11     FORMAT(/'  MAP ST Cal =',6F8.2)
*C BN Calibration of ST 
*C New (Dec.16,1998) due to that ToF calib. moved by ~1ns
*        stdel(1) = 85.54
*        stdel(2) = 96.38
*        stdel(3) = 88.21
*        stdel(4) = 88.15
*        stdel(5) = 90.61
*        stdel(6) = 86.78 
*C Old
*c        stdel(1) = 86.75
*c        stdel(2) = 97.77
*c        stdel(3) = 89.44
*c        stdel(4) = 89.48
*c        stdel(5) = 92.00
*c        stdel(6) = 87.99 
*        WRITE(6,12) stdel
* 12     FORMAT('  BN  ST Cal =',6F8.2)
*        DO id = 1,nsect
*          cal_st(1,id) = stdel(id)     ! cable length [ns]
*          cal_st(2,id) = 1./st_t1(id)  ! slope [cnts/ns] 
*          cal_st(3,id) = 30.           ! ADC conversion [cnts/MeV] 
*          cal_st(4,id) = veff(id)      ! velocity [cm/ns] of signal propagation
*          cal_st(5,id) = 300.          ! attenuation length [cm] in a slab
*        ENDDO
E 3
I 3
      print *,'s4'
E 3
C
C Constants for DC
C
C
C Bogdan calibrations
D 5
        call dcgetcalib(ltrig(1),tc_dc,dc_stat,vprp_dc,Tsl_dc,
     &                  vgas,td_h,td_f)
E 5
I 5
ccc always read files        call dcgetcalib(ltrig(1),tc_dc,dc_stat,vprp_dc,Tsl_dc,
ccc always read files     &                  vgas,td_h,td_f)
        call dcreadcalib(ltrig(1),tc_dc,dc_stat,vprp_dc,Tsl_dc,
     &                   vgas,td_h,td_f)


E 5
D 3
*      print *,'1>',tc_dc(55,13,3),dc_stat(22,2,5),vprp_dc(1),Tsl_dc,vgas,
*     &             td_h(2,25),td_f(1,75,35)
*        ddel= 70.
*        DO isec = 1,nsect
*         DO il = 1,36
*          is = (il - 1)/6 + 1
*          del = delDC(is,isec)
*          DO i = 1,192
*            val = del
*            IF(isec.EQ.6) THEN
*              IF(il.EQ.1 .AND. (i.GE.8 .AND.i.LE.23)) val = del - ddel
*              IF(il.EQ.2 .AND. (i.GE.7 .AND.i.LE.22)) val = del - ddel
*              IF(il.EQ.3 .AND. (i.GE.7 .AND.i.LE.22)) val = del - ddel
*              IF(il.EQ.4 .AND. (i.GE.6 .AND.i.LE.21)) val = del - ddel
*              IF(il.EQ.7 .AND. (i.GE.12.AND.i.LE.27)) val = del - ddel
*              IF(il.EQ.8 .AND. (i.GE.11.AND.i.LE.26)) val = del - ddel
*              IF(il.EQ.9 .AND. (i.GE.11.AND.i.LE.26)) val = del - ddel
*              IF(il.EQ.10.AND. (i.GE.10.AND.i.LE.25)) val = del - ddel
*              IF(il.EQ.11.AND. (i.GE.10.AND.i.LE.25)) val = del - ddel
*              IF(il.EQ.12.AND. (i.GE. 9.AND.i.LE.24)) val = del - ddel
*            ENDIF
*            tc_dc(i,il,isec) = val                 ! DL1 - Tc  [ns] for each wire
*          ENDDO
*         ENDDO
*        ENDDO
*C
*C Read pulser constants (MAP) for DC
*C
*        CALL map_get_float('/group/clas/parms/Maps/DC_TDlY.map'
*     1      ,'Delays','DC2TOF',  1, dc2tof, ltrig(1), iret)
*        CALL map_get_float('/group/clas/parms/Maps/DC_TDlY.map'
*     1      ,'Delays','Region4Pulser',  3, reg4pul, ltrig(1), iret)
*C
*C E-beam
*c        dc2tof     =  dc2tof    - 0. 
*c        reg4pul(1) = reg4pul(1) - 10.
*c        reg4pul(2) = reg4pul(2) - 10.
*c        reg4pul(3) = reg4pul(3) - 10.
*C Photon beam
*c        dc2tof     =  dc2tof    - 0. 
*c        reg4pul(1) = reg4pul(1) - 5.
*c        reg4pul(2) = reg4pul(2) - 5.
*c        reg4pul(3) = reg4pul(3) - 5.
*        DO isec = 1,nsect
*          CALL map_get_float('/group/clas/parms/Maps/DC_TDlY.map'
*     1        ,SecNo(isec),'T0',  6912, t0, ltrig(1), iret)
*          CALL map_get_int('/group/clas/parms/Maps/DC_STATUS.map'
*     1        ,SecNo(isec),'status',  6912, dcst, ltrig(1), iret)
*          DO il = 1,36
*            ir = (il - 1)/12 + 1
*            iwmin = sda_pln(15,il+3,isec)
*            iwmax = sda_pln(16,il+3,isec)
*            DO i = 1,192
*              tc_dc(i,il,isec)   = t0_dc(i,il) + reg4pul(ir) + dc2tof
*              dc_stat(i,il,isec) = dc_st(i,il)
*              IF(i.LT.iwmin .OR. i.GT.iwmax) dc_stat(i,il,isec) = 1
*            ENDDO
*          ENDDO
*        ENDDO
*C
*        DO is = 1,6
*          vprp_dc(is) = 16.          ! velocity along a wire [cm/ns] per SLayer
*        ENDDO
*        Tsl_dc = 2.                  ! slope [cnts/ns]
*        vgas   = 0.005               ! drift velocity [cm/ns]
*C
*      print *,'open'
*        OPEN (UNIT=44,FILE='../PARMS/DCtd.dat',STATUS='OLD')
*        DO il = 1,36
*          READ(44,*) (td_h(i,il),i=1,3)
*          Np = td_h(1,il)
*          READ(44,*) ((td_f(i,j,il),i=1,2),j=1,Np)
*        ENDDO
*        CLOSE(44)
*      print *,'2>',tc_dc(55,13,3),dc_stat(22,2,5),vprp_dc(1),Tsl_dc,vgas,
*     &             td_h(2,25),td_f(1,75,35)
E 3
I 3
      print *,'s5'
E 3
C
C Constants for SC
C
D 4
        call scgetcalib(ltrig(1), Elos_sc, Ethr_sc, cal_sc)
E 4
I 4
cmovedto sclib.c        call scgetcalib(ltrig(1), Elos_sc, Ethr_sc, cal_sc)
E 4
D 3
*
*        Elos_sc = 2.018592           ! energy loss [Mev/cm]
*        Ethr_sc = 1.0                ! energy threshold [MeV]
*C
*       CALL map_get_int('/group/clas/parms/Maps/SC_CALIBRATIONS.map'
*     1     ,'status','left',  288, statL, ltrig(1), iret)
*       CALL map_get_int('/group/clas/parms/Maps/SC_CALIBRATIONS.map'
*     1	   ,'status','right', 288, statR, ltrig(1), iret)
*C
*       CALL map_get_float('/group/clas/parms/Maps/SC_CALIBRATIONS.map'
*     1     ,'pedestals','left',  288, pedL, ltrig(1), iret)
*       CALL map_get_float('/group/clas/parms/Maps/SC_CALIBRATIONS.map'
*     1	   ,'pedestals','right', 288, pedR, ltrig(1), iret)
*C
*       CALL map_get_float('/group/clas/parms/Maps/SC_CALIBRATIONS.map'
*     1	   ,'T0_TDC', 'left',  288, t0L, ltrig(1), iret)
*       CALL map_get_float('/group/clas/parms/Maps/SC_CALIBRATIONS.map'
*     1     ,'T0_TDC', 'right', 288, t0R, ltrig(1), iret)
*C
*       CALL map_get_float('/group/clas/parms/Maps/SC_CALIBRATIONS.map'
*     1	   ,'T1', 'left',  288, t1L, ltrig(1), iret)
*       CALL map_get_float('/group/clas/parms/Maps/SC_CALIBRATIONS.map'
*     1     ,'T1', 'right', 288, t1R, ltrig(1), iret)
*C
*       CALL map_get_float('/group/clas/parms/Maps/SC_CALIBRATIONS.map'
*     1	   ,'T2', 'left',  288, t2L, ltrig(1), iret)
*       CALL map_get_float('/group/clas/parms/Maps/SC_CALIBRATIONS.map'
*     1     ,'T2', 'right', 288, t2R, ltrig(1), iret)
*C
*       CALL map_get_float('/group/clas/parms/Maps/SC_CALIBRATIONS.map'
*     1     ,'delta_T', 'left_right',    288,  sc_lr, ltrig(1), iret)
*       CALL map_get_float('/group/clas/parms/Maps/SC_CALIBRATIONS.map'
*     1     ,'delta_T', 'paddle2paddle', 288, sc_p2p, ltrig(1), iret)
*C
*       CALL map_get_float('/group/clas/parms/Maps/SC_CALIBRATIONS.map'
*     1     ,'WALK_A0', 'left', 288, w0L, ltrig(1), iret)
*       CALL map_get_float('/group/clas/parms/Maps/SC_CALIBRATIONS.map'
*     1     ,'WALK_A0', 'right', 288, w0R, ltrig(1), iret)
*C
*       CALL map_get_float('/group/clas/parms/Maps/SC_CALIBRATIONS.map'
*     1     ,'WALK1', 'left', 288, w1L, ltrig(1), iret)
*       CALL map_get_float('/group/clas/parms/Maps/SC_CALIBRATIONS.map'
*     1     ,'WALK1', 'right', 288, w1R, ltrig(1), iret)
*C
*       CALL map_get_float('/group/clas/parms/Maps/SC_CALIBRATIONS.map'
*     1     ,'WALK2', 'left', 288, w2L, ltrig(1), iret)
*       CALL map_get_float('/group/clas/parms/Maps/SC_CALIBRATIONS.map'
*     1     ,'WALK2', 'right', 288, w2R, ltrig(1), iret)
*C
*        DO isec = 1,nsect
*         DO id = 1,48
*          cal_sc(1,id,isec) = delL(id,isec)   ! cable length [ns] for  left PT
*          cal_sc(2,id,isec) = delR(id,isec)   ! cable length [ns] for right PT
*          cal_sc(3,id,isec) = 1./t1L(id,isec) ! slope [cnts/ns] for  left PT 
*          cal_sc(4,id,isec) = 1./t1R(id,isec) ! slope [cnts/ns] for right PT
*          cal_sc(5,id,isec) = 60.             ! ADC conv. [cnts/MeV] for  left PT 
*          cal_sc(6,id,isec) = 60.             ! ADC conv. [cnts/MeV] for right PT
*          cal_sc(7,id,isec) = 16.             ! velocity of propagation [cm/ns]
*          cal_sc(8,id,isec) = 300.            ! attenuation length [cm] in a slab
*          cal_sc(9,id,isec) = statL(id,isec)  ! StatusL (0-OK,1-ADC,2-TDC,3-both)
*          cal_sc(10,id,isec)= statR(id,isec)  ! StatusR (0-OK,1-ADC,2-TDC,3-both)
*          cal_sc(11,id,isec)= pedL(id,isec)   ! PED for left  PT [cnts]
*          cal_sc(12,id,isec)= pedR(id,isec)   ! PED for right PT [cnts]
*          cal_sc(13,id,isec)= w0L(id,isec)    ! time walk constant "w0" left 
*          cal_sc(14,id,isec)= w0R(id,isec)    ! time walk constant "w0" right 
*          cal_sc(15,id,isec)= w1L(id,isec)    ! time walk constant "w1" left 
*          cal_sc(16,id,isec)= w1R(id,isec)    ! time walk constant "w1" right 
*          cal_sc(17,id,isec)= w2L(id,isec)    ! time walk constant "w2" left 
*          cal_sc(18,id,isec)= w2R(id,isec)    ! time walk constant "w2" right 
*          cal_sc(19,id,isec)= sc_lr(id,isec)  ! (twL - twR)/2  Laser
*          cal_sc(20,id,isec)= sc_p2p(id,isec) ! (twL + twR)/2  Data or Cosmic
*          cal_sc(21,id,isec)= t0L(id,isec)    ! T0_TDC -offset to Lslope
*          cal_sc(22,id,isec)= t0R(id,isec)    ! T0_TDC -offset to Rslope
*          cal_sc(23,id,isec)= t2R(id,isec)    ! T2_TDC -quadratic term to Lslope
*          cal_sc(24,id,isec)= t2R(id,isec)    ! T2_TDC -quadratic term to Rslope
*         ENDDO
*        ENDDO
*C
*C
*C Constants for EC
*C
*        Ethr_ec = 1.0                                ! Evisible threshold [MeV]
*C Read pedestals
*        CALL ec_r_map(ltrig(1), ECped, 'EC_PEDESTALS.map')
*C
*        DO isec = 1,nsect
*         DO iv = 1,6                                 ! iv = ui,vi,wi,uo,vo,wo
*          DO id = 1,36                               ! 36 slab towers
*            cal_ec(1,id,iv,isec) = 30.               ! cable length [ns]
*            cal_ec(2,id,iv,isec) = 22.               ! slope [cnts/ns] 
*            cal_ec(3,id,iv,isec) = 10.               ! ADC conversion [cnts/MeV]
*            cal_ec(4,id,iv,isec) = 18.1              ! velocity of prop. [cm/ns]
*            cal_ec(5,id,iv,isec) = 376.              ! attenuation length [cm]
*            cal_ec(6,id,iv,isec) = ECped(id,iv,isec) ! PED [cnts]
*          ENDDO
*         ENDDO 
*        ENDDO     
C
C *** Writing "sdacalib.bos"
C 
*        IF(iucomp(4HWRIT,lcali,10) .NE. 0) THEN
*          NR = 1
*          CALL ETNFORMAT(IW,'CHDR','8I')
*          CALL ETNFORMAT(IW,'CTRG','F')
*          CALL ETNFORMAT(IW,'CST ','F')
*          CALL ETNFORMAT(IW,'CDC ','F')
*          CALL ETNFORMAT(IW,'CSC ','F')
*          CALL ETNFORMAT(IW,'CEC ','F')
*          CALL ETNFORMAT(IW,'DCTD','F')
*          CALL ETLCTL(IW,'E=','CHDRCTRGCST CDC CSC CEC DCTD ')
*C
*          CALL FPARM(' OPEN SDACAL UNIT=57 FILE="sdacal.bos"'//
*     1      'RECL=36000 ACTION=WRITE ACCESS=SEQ STATUS=NEW FORM=BINARY')
*C Store Header
*          ind = ETNCREATE(IW,'CHDR',0,8,1)
*          CALL GetUnixTime(utime)
*          IW(ind+1) = version
*          IW(ind+2) = utime
*          DO i = 3,8
*            IW(ind+i) = 0
*          ENDDO
*C Store Trigger
*          ind = ETNCREATE(IW,'CTRG',NR,nwtrg,1)
*          DO i = 1,nwtrg
*            RW(ind+i) = ctrig(i)
*          ENDDO
*C Store ST
*          ind = ETNCREATE(IW,'CST ',NR,nwst,1)
*          DO i = 1,nwst
*            RW(ind+i) = cst(i)
*          ENDDO
*C Store DC
*          ind = ETNCREATE(IW,'CDC ',NR,nwdc,1)
*          DO i = 1,nwdc
*            RW(ind+i) = cdc(i)
*          ENDDO
*C Store SC
*          ind = ETNCREATE(IW,'CSC ',NR,nwsc,1)
*          DO i = 1,nwsc
*            RW(ind+i) = csc(i)
*          ENDDO
*C Store EC
*          ind = ETNCREATE(IW,'CEC ',NR,nwec,1)
*          DO i = 1,nwec
*            RW(ind+i) = cec(i)
*          ENDDO
*C Store DC grids for Dm = f(Td)
*          ind = ETNCREATE(IW,'DCTD',NR,nwtd,1)
*          DO i = 1,nwtd
*            RW(ind+i) = ftd(i)
*          ENDDO
*C
*C Write BOS banks          
*          CALL FWBOS(IW,57,'E',ierr)
*C Close
*          CALL ETLDROP(IW,'E')
*          CALL ETNGARB(IW)
*          CALL FWBOS(IW,57,'0',ierr)
*          CALL FPARM('CLOSE SDACAL')
*        ENDIF
E 3
I 3
      print *,'s6'
E 3
      ENDIF     
C
C *** Reading the "sdacalib.bos"
C 
I 3
      print *,'s7'
E 3
      IF(iucomp(4HREAD,lcali,10) .NE. 0) THEN
D 3
*        NR = 1
*        CALL getenvf('CBOS',fname)
*        CALL FPARM('OPEN SDACAL UNIT=56 FILE="' // fname // '"')
*        CALL FRBOS(IW,56,'E',ierr)
*C Get Header
*        ind = ETNLINK(IW,'CHDR',0)
*        IF(ind.NE.0) THEN
*          version = IW(ind+1)
*          utime   = IW(ind+2)
*          CALL GetASCIITime(utime,utimes)
*c*        WRITE(6,201) kname,version,utimes
*c* 201  FORMAT( ' Read "sdacal.bos"  file for ',A8,I2,' created on  ',A24)
*        ENDIF
E 3
C Get trigger constants
I 3
      print *,'s8'
E 3
        call trigreadcalib(ltrig(1),Tdel,Tjit,DL1,vflt)
D 3
*        ind = ETNLINK(IW,'CTRG',NR)
*        IF(ind.NE.0) THEN
*          DO i = 1,nwtrg
*            ctrig(i) = RW(ind+i)
*          ENDDO
*        ENDIF
E 3
I 3
      print *,'s9'
E 3
C Get ST constants
        call streadcalib(ltrig(1),Elos_st,Ethr_st,cal_st)
D 3
*        ind = ETNLINK(IW,'CST ',NR)
*        IF(ind.NE.0) THEN
*          DO i = 1,nwst
*            cst(i) = RW(ind+i)
*          ENDDO
*        ENDIF
E 3
I 3
      print *,'s10'
E 3
C Get DC constants
        call dcreadcalib(ltrig(1),tc_dc,dc_stat,vprp_dc,Tsl_dc,
     &                   vgas,td_h,td_f)
D 3
*        ind = ETNLINK(IW,'CDC ',NR)
*        IF(ind.NE.0) THEN
*          DO i = 1,nwdc
*            cdc(i) = RW(ind+i)
*          ENDDO
*        ENDIF
E 3
I 3
      print *,'s11'
E 3
C Get SC constants
D 4
        call screadcalib(ltrig(1),Elos_sc,Ethr_sc,cal_sc)
E 4
I 4
cmovedto sclib.c        call screadcalib(ltrig(1),Elos_sc,Ethr_sc,cal_sc)
E 4
D 3
*        ind = ETNLINK(IW,'CSC ',NR)
*        IF(ind.NE.0) THEN
*          DO i = 1,nwsc
*            csc(i) = RW(ind+i)
*          ENDDO
*        ENDIF
*      print *,'TTT->',Elos_sc_,Ethr_sc_,cal_sc_(2,3,4)
*      print *,'SSS->',Elos_sc,Ethr_sc,cal_sc(2,3,4)
*C Get EC constants
*        call ecreadcalib(ltrig(1),Ethr_ec_,cal_ec_)
*        ind = ETNLINK(IW,'CEC ',NR)
*        IF(ind.NE.0) THEN
*          DO i = 1,nwec
*            cec(i) = RW(ind+i)
*          ENDDO
*        ENDIF
*      print *,'EEE->',Ethr_ec_,cal_ec_(2,3,4,1)
C Get DC grids for Dm = f(Td)
*        ind = ETNLINK(IW,'DCTD',NR)
*        IF(ind.NE.0) THEN
*          DO i = 1,nwtd
*            ftd(i) = RW(ind+i)
*          ENDDO
*        ENDIF
C Close
*        CALL ETLDROP(IW,'E')
*        CALL ETNGARB(IW)
*        CALL FPARM('CLOSE SDACAL')
E 3
I 3
      print *,'s12'
E 3
      ENDIF
I 3
      print *,'s13'
E 3
C
 999  CONTINUE
I 3
      print *,'s14'
E 3
      RETURN
      END
D 3
*C======================================================================
*      SUBROUTINE ec_r_map(RunNo,array,fname)
*C----------------------------------------------------------------------
*C-
*C-   Purpose and Methods : Reads EC calibration constants into the "array".
*C-
*C-   Inputs  : RunNo and FileName in the DIRECTORY "/group/clas/parms/Maps"
*C-   Outputs : Fills the array "array(islb,iview,isec)"
*C-   Controls:
*C-
*C-   Library belongs: libsda.a
*C-
*C-   Calls: map_get_float
*C-
*C-   Created   16-SEP-1997   Bogdan Niczyporuk
*C-
*C-   Called by sda_calib
*C-
*C----------------------------------------------------------------------
*      IMPLICIT NONE
*C----------------------------------------------------------------------
*C
*      SAVE
*C
*C Subroutine parameters
*      INTEGER RunNo
*      REAL    array(36,6,6)
*C Max length of "fname" 16 characters
*      CHARACTER*(*) fname
*C
*C Local variables
*      INTEGER islab, iv, isec, iret
*      REAL    temp(36,6)
*      CHARACTER*39 fpath
*      CHARACTER*1  view(6)
*      CHARACTER*5  inout(6)
*C                                        2
*C                  1                     3
*      DATA fpath /'/group/clas/parms/Maps/'/
*      DATA view / 'u',     'v',     'w',     'u',     'v',     'w'/
*      DATA inout /'inner', 'inner', 'inner', 'outer', 'outer', 'outer'/
*C
*C
*      WRITE (fpath(24:39),11) fname
* 11   FORMAT(A)
*      DO iv = 1,6
*       CALL map_get_float(fpath,inout(iv),view(iv),216,temp,RunNo,iret)
*       DO isec = 1,6
*         DO islab = 1,36
*           array(islab,iv,isec) = temp(islab,isec)
*         ENDDO
*       ENDDO
*      ENDDO       
*C
*      RETURN
*      END
*C======================================================================
*      SUBROUTINE ec_w_map(RunNo,array,fname)
*C----------------------------------------------------------------------
*C-
*C-   Purpose and Methods : Writes the "array" (EC calib. constants) into the DB.
*C-
*C-   Inputs  : RunNo, FileName in the DIRECTORY "/group/clas/parms/Maps"
*C-             and "array(islb,iview,isec)" with new constants
*C-   Outputs : Updated file in the DIRECTORY "/group/clas/parms/Maps" 
*C-   Controls:
*C-
*C-   Library belongs: libsda.a
*C-
*C-   Calls: map_put_float
*C-
*C-   Created   17-SEP-1997   Bogdan Niczyporuk
*C-
*C-   Called by sda_calib
*C-
*C----------------------------------------------------------------------
*      IMPLICIT NONE
*C----------------------------------------------------------------------
*C
*      SAVE
*C
*C Subroutine parameters
*      INTEGER RunNo
*      REAL    array(36,6,6)
*C Max length of "fname" 16 characters
*      CHARACTER*(*) fname
*C
*C Local variables
*      INTEGER islab, iv, isec, iret
*      REAL    temp(36,6)
*      CHARACTER*39 fpath
*      CHARACTER*1  view(6)
*      CHARACTER*5  inout(6)
*C                                        2
*C                  1                     3
*      DATA fpath /'/group/clas/parms/Maps/'/
*      DATA view / 'u',     'v',     'w',     'u',     'v',     'w'/
*      DATA inout /'inner', 'inner', 'inner', 'outer', 'outer', 'outer'/
*C
*C
*      WRITE (fpath(24:39),11) fname
* 11   FORMAT(A)
*      DO iv = 1,6
*       DO isec = 1,6
*         DO islab = 1,36
*           temp(islab,isec) = array(islab,iv,isec)
*         ENDDO
*       ENDDO
*       CALL map_put_float(fpath,inout(iv),view(iv),216,temp,RunNo,iret)
*      ENDDO       
*C
*      RETURN
*      END
E 3
E 1
