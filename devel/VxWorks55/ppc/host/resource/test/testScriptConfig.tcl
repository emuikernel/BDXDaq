# testScriptConfig.tcl - Tcl script, test scripts configuration.
#
# Copyright 1994-1997 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01r,08jan99,fle  added wtxRegsiterTest, wtxUnregisterTest and wtxInfoQTest
# 01q,23sep98,p_b  added wtxObjModuleInfoAndPathGetTest.tcl
# 01p,03jul98,p_b  added new Load tests
# 01o,27may98,p_b  New WTX 2.0 changes
# 01n,12may98,fle  added wtxCpuInfoGetTest
# 01m,28apr98,p_b  removed Async. Notif. references.
# 01l,15feb98,p_b  added hostCacheTest 
# 01k,15feb98,p_b  added WTX 2.0 tests
# 01j,07nov97,fle  added wtxMemDisassembleTest
# 01i,12jul96,jmp  simplified tests list format.
# 01h,29feb96,jmp  renamed wtxzAgentModeSetTest.tcl.
# 01g,19jan96,jmp  added wtxAgentModeGetTest, wtxDirectCallTest.
# 01f,04oct95,jmp  added wtxTargetAttachTest, wtxTsLock, wtxTsUnlock.
# 01e,22aug95,jmp  updated test list.
# 01d,22may95,jcf  name revision.
# 01c,06feb95,kpn  added wtxCloseTest.tcl and wtxVioCtlTest.tcl
# 01b,26Jan95,kpn  removed wtxFunct(0) that was reserved for the file 
#		   wtxError.tcl
# 01a,07Nov94,kpn  written.


#################################################
#						#
# CONFIGURATION FILE for WTX test scripts	# 
#						#
#################################################


# get the wpwr base directory

global env
set windBase $env(WIND_BASE)

# Path of the test scripts

  if { $env(WIND_HOST_TYPE) == "x86-win32" } {

   set PATH $env(WIND_BASE)\\host\\src\\test\\wtxtcl\\

  } else {

   set PATH $env(WIND_BASE)/host/src/test/wtxtcl/

  }

#
# TESTS TO RUN 
#
#    (1) wtxFunct() is an array, the first index number must be equal to 1. 
#	 The index numbers must be incremented by 1.
#        If you decide to remove a script or to add a new script you must
#        verify that the index numbers are always consecutive.
#
#        Example :   RIGHT              WRONG
#                  wtxFunct(1)        wtxFunct(1)
#                  wtxFunct(2)        wtxFunct(3)
#                  wtxFunct(3)        wtxFunct(4)
#
#    (2) The index numbers can be in the right order or can be mixed.
#
#        Example :             is the same than
#                  wtxFunct(1)                   wtxFunct(1)
#                  wtxFunct(2)                   wtxFunct(3)
#                  wtxFunct(3)                   wtxFunct(2)
#


set wtxFunct(1)  wtxToolAttachTest
set wtxFunct(2)  wtxToolDetachTest

set wtxFunct(3)  wtxGopherEvalTest

set wtxFunct(4)  wtxMemAllocTest
set wtxFunct(5)  wtxMemWriteTest
set wtxFunct(6)  wtxMemReadTest
set wtxFunct(7)  wtxMemSetTest
set wtxFunct(8)  wtxMemFreeTest
set wtxFunct(9)  wtxMemChecksumTest
set wtxFunct(10) wtxMemInfoGetTest
set wtxFunct(11) wtxMemAlignTest
set wtxFunct(12) wtxMemReallocTest
set wtxFunct(13) wtxMemScanTest
set wtxFunct(14) wtxMemMoveTest
set wtxFunct(15) wtxMemAddToPoolTest
set wtxFunct(16) wtxMemDisassembleTest

set wtxFunct(17) wtxContextCreateTest
set wtxFunct(18) wtxContextKillTest
set wtxFunct(19) wtxContextContTest
set wtxFunct(20) wtxContextSuspendTest
set wtxFunct(21) wtxContextResumeTest
set wtxFunct(22) wtxContextStepTest

set wtxFunct(23) wtxEventpointAddTest
set wtxFunct(24) wtxEventpointDeleteTest
set wtxFunct(25) wtxEventpointListTest
set wtxFunct(26) wtxEventGetTest
set wtxFunct(27) wtxEventAddTest
set wtxFunct(28) wtxRegisterForEventTest

set wtxFunct(29) wtxConsoleCreateTest
set wtxFunct(30) wtxConsoleKillTest

set wtxFunct(31) wtxOpenTest
set wtxFunct(32) wtxCloseTest

set wtxFunct(33) wtxObjModuleFindTest
set wtxFunct(34) wtxObjModuleInfoGetTest
set wtxFunct(35) wtxObjModuleInfoAndPathGetTest
set wtxFunct(36) wtxObjModuleListTest
set wtxFunct(37) wtxObjModuleLoadTest
set wtxFunct(38) wtxObjModuleUnloadTest
set wtxFunct(39) wtxObjModuleLoadStartTest
set wtxFunct(40) wtxObjModuleLoadProgressReportTest
set wtxFunct(41) wtxObjModuleLoadCancelTest

set wtxFunct(42) wtxSymAddTest
set wtxFunct(43) wtxSymFindTest
set wtxFunct(44) wtxSymListGetTest
set wtxFunct(45) wtxSymRemoveTest
set wtxFunct(46) wtxSymTblInfoGetTest

set wtxFunct(47) wtxVioCtlTest
set wtxFunct(48) wtxVioWriteTest
set wtxFunct(49) wtxVioChanGetTest
set wtxFunct(50) wtxVioFileListTest

set wtxFunct(51) wtxTsInfoGetTest

set wtxFunct(52) wtxFuncCallTest
set wtxFunct(53) wtxDirectCallTest

set wtxFunct(54) wtxRegsGetTest
set wtxFunct(55) wtxRegsSetTest

set wtxFunct(56) wtxTsLockTest
set wtxFunct(57) wtxTsUnlockTest

set wtxFunct(58) wtxAgentModeSetTest
set wtxFunct(59) wtxAgentModeGetTest

set wtxFunct(60) wtxTargetAttachTest

set wtxFunct(61) wtxEventListGetTest 
set wtxFunct(62) wtxObjModuleChecksumTest
set wtxFunct(63) wtxEventpointListGetTest 
set wtxFunct(64) wtxMemWidthReadTest 
set wtxFunct(65) wtxMemWidthWriteTest 
set wtxFunct(66) hostCacheTest
set wtxFunct(67) wtxCpuInfoGetTest
set wtxFunct(68) wtxUnregisterForEventTest 
set wtxFunct(69) wtxLoggingTest 

set wtxFunct(70) wtxRegisterTest
set wtxFunct(71) wtxUnregisterTest
set wtxFunct(72) wtxInfoQTest

# set wtxFunct() wtxContextStatusGetTest
# set wtxFunct() wtxAsyncNotifyEnableTest
# set wtxFunct() wtxAsyncNotifyDisableTest

set LAST_TEST 73
set wtxFunct($LAST_TEST) wtxTargetResetTest


# This variable contains regexps, to specify
# which tests are not run under NT.

set testsNotToRunUnderNT(1) wtxVioFileList
set testsNotToRunUnderNT(2) wtxVioCtl
set testsNotToRunUnderNT(3) wtxOpen
set testsNotToRunUnderNT(4) wtxClose
set LAST_TEST_NOT_NT 5
set testsNotToRunUnderNT($LAST_TEST_NOT_NT) wtxConsole

