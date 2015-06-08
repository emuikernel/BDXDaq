


2013/06/14

VERSION 1.

see also:

 doc/smi_dim_new.txt

//==================================================================
//==================================================================
//==================== OPERATION PROD (ver 1) ============================
//==================================================================
//==================================================================


operation order:

1) add the following to the ~/.cshrc fole:
#=============== smi++ and dim =====================================

setenv SMIDIR /home/sytnik/smixx
setenv SMIRTLDIR /home/sytnik/smixx
setenv OS Linux
setenv DIM_DNS_NODE clonpc3
setenv DIMDIR /home/sytnik/dim
setenv ODIR linux
#setenv LD_LIBRARY_PATH "$LD_LIBRARY_PATH":/opt/sfw/lib
setenv LD_LIBRARY_PATH "$DIMDIR/$ODIR":"$SMIDIR/$ODIR":"$LD_LIBRARY_PATH"
setenv PATH "$PATH":"$DIMDIR"/"$ODIR"
setenv PATH "$PATH":"$SMIDIR"/"$ODIR"

#//==================================================================

2) source ~/.cshrc

3) dns &
4) make -i

5) did &

6) cd0 
  (or cd /usr/local/clas/clas12/R3.14.12.3/epicsB/baseB/src/CAEN_HV/level0/IocShell)

7) mye
   (or O.linux-x86/ioccaen startup.all)
   (in this version (version 1)) the mye should be started after the "make -i"

//==================================================================
//==================================================================
//================== TRICKS ===============================
//==================================================================
//==================================================================

1) use e.g. from command line: 
     smiSendCommand CLAS12::b_hv000_0_p1 SET_OFF
   for the debugging.

2) In the boardclass.cxx the function for actions (e.g. void BOARDCLASS::SWITCH_ON())
   must contain setting of some state  (e.g. setOFF()), although the finale state 
   is defined by messages (smiSendCommand) coming from the epics handlers.
   Otherwise we have "undefined" situation with some node (e.g. HV_TEST::b_hv000_0_p1)
   after issuing e.g. smiSendCommand CLAS12::b_hv000_0_p1 SET_OFF 
   or doing the same from the did gui. If we do that from did GUI it 
   a) works from proxy server (CLAS12::HV)
   b) does not work from the logical server (CLAS12_SMI)
   
3) class: BOARDCLASS/associated in hv_test.smi
   may have different name than in the boardclass.cxx





