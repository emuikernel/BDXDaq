# wtxTestMsg.tcl - Tcl script, description of the testcase units.
#
# Copyright 1995-1996 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 02j,18jan99,p_b  added tabs to wtxRegister, wtxUnregister, wtxInfoQ messages
# 02i,08jan99,fle  added wtxRegister, wtxUnregister and wtxInfoQ messages
# 02h,27may98,p_b  WTX 2.0 new changes
# 02g,14may98,p_b  Replace test?.o by objSampleWtxtclTest?.o
# 02f,12may98,fle  added cpuInfoGetMsg
# 02e,25apr98,p_b  added WTX 2.0 Msg, rename all Msge in Msg, 
#	           rename wtxTestMsge in wtxTestMsg	      
# 02d,23jan98,c_c  created memDisassembleMsge(1->8) for wtxMemDisassembleTest
# 02c,05dec97,fle  added test messages for wtxRegistryTest.tcl
# 02c,21jul97,f_l  added hostCacheMsg(1-6)
# 02b,28nov96,elp  updated vSymMsg(12).
# 02a,01oct96,jmp  added vObjMsg(32), vEvptMsg(5-6).
# 01z,25jul96,jmp  added vObjMsg(29->31). updated Copyright.
# 01y,11jul96,jmp  modified wtxGopherEvalTest messages.
# 01x,13mar96,jmp  added vSymMsg(22), vObjMsg(22->28), gopherEvalMsg(3),
#		   modified vObjMsg(3).
# 01w,26feb96,l_p  corrected English mistakes in a few messages.
#            +jmp  added vObjMsg(21), directCallMsg(1,2),
#		   agentModeGetMsg(1).
# 01u,16oct95,jmp  added tsLockMsg(1,2),memScanMsg(4,5), vObjMsg(15->19),
#		   vSymMsg(18->20), vEvptMsg(4), vEvMsg(9),tgtAttachMsg(1),
#		   vioCtlMsg(3), serviceAddMsg(1->4).
# 01t,12sep95,jmp  added funcCallMsg(3->5), vObjMsg(13->14), vSymMsg(17).
# 01s,25jul95,jmp  added memReadMsg(3->5), added memAddToPoolMsg(2)
#		   added memReallocMsg(1->3), changed memReallocMsg(4->5)
#		   added memZeroMsg(4)
# 01r,30may95,f_v  added memScanMsg(1->2),vioChanGetMsg(1)
# 01q,19may95,f_v  added vSymMsg(14),vSymMsg(15),agentModeSetMsg(1)
# 01p,17may95,f_v  changed vSymMsg(12)
# 01o,15may95,f_v  added tsInfoGetMsg(1),memAlignMsg(2)
# 01n,28apr95,f_v  added vCtxMsg(9) 
# 01m,19apr95,f_v  added vCtxMsg(8) 
# 01l,12apr95,f_v  added vObjMsg(12) 
# 01k,04apr95,f_v  added gopher section
#		   added vCtxMsg(6->7)
# 01j,21mar95,f_v  change few english expressions
#		   added vioFileList(1->3)
#		   added wtxMemMove,wtxMemAlign,wtxMemAddToPool,
#		   wtxMemRealloc,wtxVioFileList sections
#    		   changed pomme to test1
# 01i,20mar95.f_v  change printLogo by user task
# 01h,22feb95,kpn  modified some messages.
# 01g,17feb95,f_v  add vObjMsg(8->11),vSymMsg(12)
# 01f,14feb95,kpn  added openMsg(6)(7), vioWriteMsg(2), vioCltMsg(2),
#		   memWriteMsg(4), memReadMsg(2), memZeroMsg(3),
#		   memFreeMsg(2)
# 01e,06feb95,f_v  add tabulation in vXxxMsg to align PASS or FAIL messages
#		   add vRegsMsg(0->1),RegsMsg(0->1)
#		   change vEvMsg to vEvptMsg to in eventpoint section
#		   change EvMsg to EvptMsg to in eventpoint section
#		   add event management section
#		   move vEvptMsg(0->4),EvptMsg(0) to event section
#		   add EvMsg(1), vEvMsg(1)
# 01d,02feb95,f_v  replace multiple uplevel by one
#		   added vSymMsg(6->11), symMsg(4->7),change SymMsg(1)
#		   added vObjMsg(5->7),ObjMsg(5->6)
# 01c,31jan95,kpn  added closeMsg(1)(2)(3), vioCltMsg(1), openMsg(5)
# 01b,30jan95,kpn  added targetResetMsg(1), toolAttachMsg (1)(2)(3),
#		   toolDetachMsg(1)(2), memAllocMsg(1)(2), 
#		   memChecksumMsg(1)(2), memFreeMsg(1), memReadMsg(1),
#		   memInfoMsg(1)(2)(3), memWriteMsg(1)(2)(3),
#		   memZeroMsg(1)(2), openMsg(1)(2)(3)(4), vioWriteMsg(1),
#		   csleCreateMsg(1)(2)(3), csleKillMsg(1)(2),
#		   funcCallMsg(1)(2).
#		   documentation.
# 01a,27Jan95,f_v  written.
#



#
# DESCRIPTION
#
# Initialization of messages that contains the testcase units description.
# The messages are classified by service category and according to the wtx
# service they test.
#
# Each message corresponds to the description of one testcase unit. 
# These messages are used to display the description of the testcase units that 
# have been executed, so they  are used to inform the user of the wtx test tool
# which kind of test has been performed and which test has failed or succeeded. 
#
# In fact this set of messages provides the description of all tests that have
# been written to check the WTX protocol.
#



#============================
# session management messages
#============================

uplevel #0 {

# wtxTargetReset service tests
# ---------------------------- 

    set targetResetMsg(1)  "verify wtxTargetReset behavior\t\t\t\t\t"


# wtxToolAttach service tests
# --------------------------- 

    set toolAttachMsg(1) "wtxToolAttach with valid target\
                                   server name\t\t\t"
    set toolAttachMsg(2) "wtxToolAttach with invalid target\
                                   server name\t\t\t"
    set toolAttachMsg(3) "wtxToolAttach result when a\
                                   connection already exists\t\t"

# wtxToolDetach service tests
# ---------------------------

    set toolDetachMsg(1) "wtxToolDetach result after calling\
                                   wtxToolAttach\t\t"
    set toolDetachMsg(2) "wtxToolDetach when no tool is\
                                   attached\t\t\t\t"

# wtxTsInfoGet service tests
# ---------------------------

    set tsInfoGetMsg(1)	"Returns valid target information\t\t\t\t"

# wtxTsLock/wtxTsUnlock service tests
# ---------------------------

    set tsLockMsg(1)	"wtxTsLock locks target server\t\t\t\t\t"
    set tsLockMsg(2)	"wtxTsUnlock unlocks target server\t\t\t\t"

# wtxCpuInfoGet service tests
# ---------------------------

    set cpuInfoGetMsg(1) "wtxCpuInfoGet gets all parameters\t\t\t\t"
    set cpuInfoGetMsg(2) "wtxCpuInfoGet gets different options\t\t\t\t"

#=========================
# context control messages
#=========================

     set vCtxMsg(0) 	"wtxContextCont continues execution of\
				user tasks\t\t"
     set vCtxMsg(1) 	"wtxContextCreate creates a new context\t\t\t\t"
     set vCtxMsg(2) 	"wtxContextKill kills the task Test pid\
				and its context\t\t"
     set vCtxMsg(3) 	"wtxContextResume resumes tCallTask\
				task\t\t\t\t"
     set vCtxMsg(4) 	"wtxContextStep steps execution of\
				user tasks\t\t\t"
     set vCtxMsg(5) 	"wtxContextSuspend suspends tCallTask\
				task\t\t\t"
     set vCtxMsg(6) 	"Manages properly bad addresses and events\t\t\t"
     set vCtxMsg(7) 	"Check if bad arguments are handled correctly\t\t\t"
     set vCtxMsg(8) 	"Steps over non adjacent instructions\t\t\t\t"
     set vCtxMsg(9) 	"Manages properly bad context types\t\t\t\t"
     set vCtxMsg(10)	"wtxContextCreate manages file redirection\t\t\t"



#===========================
# memory operations messages
#===========================


# wtxMemAlloc service tests
# ------------------------- 

    set memAllocMsg(1)    "Allocates memory on target with valid\
				parameters\t\t"
    set memAllocMsg(2)    "Allocates memory on target with invalid\
                            parameters\t\t"

# wtxMemChecksum service tests
# ---------------------------- 

    set memChecksumMsg(1) "wtxMemChecksum on a Block of Memory\t\t\t\t"
    set memChecksumMsg(2) "wtxMemChecksum with invalid address parameter\t\t\t"
    set memChecksumMsg(3) "wtxMemChecksum with invalid bytes number\
	parameter\t\t"

# wtxMemDisassemble service test
# ------------------------------
 
    set memDisassembleMsg(1) "wtxMemDisassemble on a valid address\t\t\t\t"
    set memDisassembleMsg(2) "wtxMemDisassemble on invalid address\t\t\t\t"
    set memDisassembleMsg(3) \
        "wtxMemDisassemble -address prepends addresses\t\t\t"
    set memDisassembleMsg(4) \
        "wtxMemDisassemble -opcodes prepends opcodes\t\t\t"
    set memDisassembleMsg(5) "wtxMemDisassemble <nInst> accuracy test\t\t\t\t"
    set memDisassembleMsg(6) "wtxMemDisassemble with <endAddr> set\t\t\t\t"
    set memDisassembleMsg(7) "wtxMemDisassemble with invalid <endAddr>\t\t\t"
    set memDisassembleMsg(8) \
        "wtxMemDisassemble with <endAddr> smaller than <startAddr>\t"


# wtxMemFree service tests
# ------------------------ 

    set memFreeMsg(1)     "Frees a block of target memory allocated\
				with wtxMemAlloc\t"
    set memFreeMsg(2)	   "wtxMemFree() with invalid <address>\
			    argument		             : "

# wtxMemInfoGet service tests
# --------------------------- 

    set memInfoMsg(1)     "wtxMemInfoGet result after a wtxMemAlloc\t\t\t"
    set memInfoMsg(2)     "wtxMemInfoGet result after a wtxMemFree\t\t\t\t"
    set memInfoMsg(3)     "Validity of the biggest free block size\t\t\t\t"

# wtxMemRead service tests
# ------------------------ 

    set memReadMsg(1)     "wtxMemRead reads the right block of memory\t\t\t"
    set memReadMsg(2)     "wtxMemRead with invalid <addr> argument\t\t\t\t"
    set memReadMsg(3)     "wtxMemRead after wtxMemWrite on a object\
	module\t\t\t"
    set memReadMsg(4)     "wtxMemRead after wtxMemSet on a object\
	module\t\t\t"
    set memReadMsg(5)     "wtxMemRead after wtxMemMove on a object\
	module\t\t\t"
				

# wtxMemWrite service tests
# ------------------------- 

    set memWriteMsg(1)    "The memory block has been filled with\
                            the right value\t\t"
    set memWriteMsg(2)    "wtxMemWrite writes bytes in the right memory\
				block\t\t"
    set memWriteMsg(3)    "wtxMemWrite() operates with invalid <blockId>\
			    argument\t\t"
    set memWriteMsg(4)    "wtxMemWrite() operates with invalid <address>\
			    argument\t\t"
# wtxMemWidthRead service tests
# ------------------------
 
    set memWidthReadMsg(1)     "wtxMemWidthRead reads the right block of memory\t\t\t"
    set memWidthReadMsg(2)     "wtxMemWidthRead with invalid <addr> argument\t\t\t"
    set memWidthReadMsg(3)     "wtxMemWidthRead after wtxMemWrite on a object\
        module\t\t"
    set memWidthReadMsg(4)     "wtxMemWidthRead after wtxMemSet on a object\
        module\t\t"
    set memWidthReadMsg(5)     "wtxMemWidthRead after wtxMemMove on a object\
        module\t\t"
 
 
# wtxMemWidthWrite service tests
# -------------------------
 
    set memWidthWriteMsg(1)    "wtxMemWidthWrite : the memory block\nhas been filled with\
                            the right value\t\t\t\t"
    set memWidthWriteMsg(2)    "wtxMemWidthWrite writes bytes in the right memory\
                                block\t\t"
    set memWidthWriteMsg(3)    "wtxMemWidthWrite() operates with invalid <blockId>\
                            argument\t"
    set memWidthWriteMsg(4)    "wtxMemWidthWrite() operates with invalid <address>\
                            argument\t"

# wtxMemZero service tests
# ------------------------ 

    set memZeroMsg(1)     "wtxMemSet clears a memory block of 100\
                            bytes on the target\t"
    set memZeroMsg(2)     "wtxMemSet clears the right number of bytes in\
                            memory block\t"
    set memZeroMsg(3)	   "wtxMemSet operates with invalid <starting address>\
			    argument\t"
    set memZeroMsg(4)	   "wtxMemSet operates with invalid <size>\
			    argument\t\t\t"

# wtxMemAlign service tests
# ------------------------- 

    set memAlignMsg(1)     "wtxMemAlign aligns a memory block on a specified\
				boundary\t"
    set memAlignMsg(2)     "wtxMemAlign manages bad arguments\t\t\t\t"

# wtxMemAddToPool service tests
# ----------------------------- 

    set memAddToPoolMsg(1)     "wtxMemAddToPool added memory\
				to the target memory pool\t\t"
    set memAddToPoolMsg(2)     "Verifies if it is possible to use added\
				agent pool memory\t\t"


# wtxMemRealloc service tests
# --------------------------- 

    set memReallocMsg(1)     "Re-allocates memory on target with valid \
	<address>\n\tand valid <size> smaller than previous value\t\t"
    set memReallocMsg(2)     "Re-allocates memory on target with valid \
	<address>\n\tand valid <size> equal to previous value\t\t"
    set memReallocMsg(3)     "Re-allocates memory on target with valid \
	<address>\n\tand valid <size> bigger than previous value\t\t"
    set memReallocMsg(4)     "wtxReallocate manages invalid <size> as\
			      parameter\t\t"
    set memReallocMsg(5)     "wtxReallocate manages invalid <address> as\
			      parameter\t\t"

# wtxMemMove service tests
# ------------------------ 

    set memMoveMsg(1)        "Moves a block of memory with valid parameters\
	\t\t\t"
    set memMoveMsg(2)        "wtxMemMove manages invalid parameters\t\t\t\t"

# wtxMemScan service tests
# ------------------------ 

    set memScanMsg(1)        "Scans memory with -memblk option\t\t\t\t"
    set memScanMsg(2)        "Scans memory with -string option\t\t\t\t"
    set memScanMsg(3)        "wtxMemScan with invalid parameters\t\t\t\t"
    set memScanMsg(4)        "Scans memory with -memblk and\
				-nomatch options\t\t\t"
    set memScanMsg(5)        "Scans memory with -string and\
				-nomatch options\t\t\t"

# wtxCacheTextUpdateMsg service tests
# ------------------------
 
    set cacheTextUpdateMsg(1)       "wtxCacheTextUpdate ... \t\t\t" 


#==================================
# object module management messages
#==================================

     set vObjMsg(0) 	"wtxObjModuleFindTest finds objSampleWtxtclTest1.o module\t"
     set vObjMsg(1) 	"wtxObjModuleInfoGetTest gives right\
			information\t\t\t"
     set vObjMsg(2) 	"wtxObjModuleListTest lists objSampleWtxtclTest1.o module\t"
     set vObjMsg(3)	"wtxObjModuleLoad loads objSampleWtxtclTest6.o with\n\
			\t\tLOAD_GLOBAL_SYMBOLS flag\t\t\t"
     set vObjMsg(4) 	"wtxObjModuleUnload unloads a module with only\
			one text segment\t"
     set vObjMsg(5)	"wtxObjModuleLoad loads objSampleWtxtclTest2.o and\n\
     			\t\tgets back the right message\t\t\t"
     set vObjMsg(6)	"wtxObjModuleLoad loads modules" 
     set vObjMsg(7) 	"wtxObjModuleLoad loads objSampleWtxtclTest3.o,\n\
                        \t\tobjSampleWtxtclTest4.o and test4 returns a\n\
                        \t\t\tright value calculated by test3\t\t"
     set vObjMsg(8)     "wtxObjModuleLoad manages correctly bad\
			 object module name\t"
     set vObjMsg(9)     "wtxObjModuleInfoGetTest manages bad object\
			 module names and Ids\t"
     set vObjMsg(10)    "wtxObjModuleFind manages bad object module names\
                        and Ids\t"
     set vObjMsg(11)    "wtxObjModuleUnload manages correctly bad object\
                        module Ids\t"
     set vObjMsg(12)    "Creates and checks .rms file in loopBack mode\t\t\t"
     set vObjMsg(13)	"wtxObjModuleUnload unloads a module with only\
			one data segment\t"
     set vObjMsg(14)    "wtxObjModuleUnload unloads a module with text\
			and data segments\t"
     set vObjMsg(15)	"wtxObjModuleLoad manages correctly C++ object\
			modules\t\t"
     set vObjMsg(16)	"wtxObjModuleLoad manages correctly null size\
			object modules\t"
     set vObjMsg(17)	"wtxObjModuleLoad manages correctly invalid\
			object file formats\t"
     set vObjMsg(18)	"wtxObjModuleLoad manages correctly illegal\
			flags combinations\t"
     set vObjMsg(19)	"wtxObjModuleLoad manages correctly invalid\
			segment addresses\t"
     set vObjMsg(20)	"wtxObjModuleLoad manages correctly bss segments\t\t\t"
     set vObjMsg(21)	"wtxObjModuleLoad manages correctly cases\
			where there isn't\n enough memory to\
			load an object module\t\t\t"
     set vObjMsg(22)	"wtxObjModuleInfoGet provides good segment\
			information\t\t"
     set vObjMsg(23)	"wtxObjModuleLoad loads objSampleWtxtclTest6.o with\n\
			\t\tLOAD_LOCAL_SYMBOLS flag\t\t\t\t"
     set vObjMsg(24)	"wtxObjModuleLoad loads objSampleWtxtclTest6.o with\n\
			\t\tLOAD_ALL_SYMBOLS flag\t\t\t\t"
     set vObjMsg(25)	"wtxObjModuleLoad loads objSampleWtxtclTest6.o with\n\
			\t\tLOAD_NO_SYMBOLS flag\t\t\t\t"
     set vObjMsg(26)	"wtxObjModuleLoad loads objSampleWtxtclTest6.o with\n\
			\t\tLOAD_HIDDEN_MODULE flag\t\t\t\t"
     set vObjMsg(27)	"wtxObjModuleLoad loads cplus.o with\
			LOAD_CPLUS_XTOR_MANUAL flag\t"
     set vObjMsg(28)	"wtxObjModuleLoad loads cplus.o with\
			LOAD_CPLUS_XTOR_AUTO flag\t"
     set vObjMsg(29)	"wtxObjModuleLoad with LOAD_COMMON_MATCH_NONE\
			flag\t\t"
     set vObjMsg(30)	"wtxObjModuleLoad with LOAD_COMMON_MATCH_USER\
			flag\t\t"
     set vObjMsg(31)	"wtxObjModuleLoad with LOAD_COMMON_MATCH_ALL\
			flag\t\t"
     set vObjMsg(32)	"verify the module group number management\t\t\t"

     set vObjMsg(103)   "wtxObjModuleLoad loads objSampleWtxtclTest6.o with\n\
                        \t\t\t\tLOAD_GLOBAL_SYMBOLS flag\t"
     set vObjMsg(104)   "wtxObjModuleUnload unloads a module\n\
			\t\t\t\twith only one text segment\t"
     set vObjMsg(105)   "wtxObjModuleLoad loads objSampleWtxtclTest2.o and\
                        gets back the right message \t"
     set vObjMsg(106)   "wtxObjModuleLoad loads modules"
     set vObjMsg(107)   "wtxObjModuleLoad loads objSampleWtxtclTest3.o,objSampleWtxtclTest4.o\n and test4\
                        returns a right value calculated by test3\t\t"
     set vObjMsg(108)   "wtxObjModuleLoad manages correctly bad\
                        object module name\t"
     set vObjMsg(111)   "wtxObjModuleUnload manages\
		 	correctly bad object module Ids\t"
     set vObjMsg(113)   "wtxObjModuleUnload unloads a module\n\
			\t\t\t\twith only one data segment\t"
     set vObjMsg(114)   "wtxObjModuleUnload unloads a module\n\
			\t\t\t\twith text and data segments\t"
     set vObjMsg(115)   "wtxObjModuleLoad manages correctly C++ object\
                        modules\t\t"
     set vObjMsg(116)   "wtxObjModuleLoad manages correctly null size\
                        object modules\t"
     set vObjMsg(117)   "wtxObjModuleLoad manages correctly invalid\n\
                        \t\t\t\tobject file formats\t\t"
     set vObjMsg(118)   "wtxObjModuleLoad manages correctly illegal\
                        flags combinations\t"
     set vObjMsg(119)   "wtxObjModuleLoad manages correctly invalid\
                        segment addresses\t"
     set vObjMsg(120)   "wtxObjModuleLoad manages correctly\n\
			\t\t\t\tbss segments\t\t\t"
     set vObjMsg(121)   "wtxObjModuleLoad manages correctly cases\
                        where there isn't\n enough memory to\
                        load an object module\t\t\t"
     set vObjMsg(123)   "wtxObjModuleLoad loads objSampleWtxtclTest6.o with\
                        LOAD_LOCAL_SYMBOLS flag\t"
     set vObjMsg(124)   "wtxObjModuleLoad loads objSampleWtxtclTest6.o with\
                        LOAD_ALL_SYMBOLS flag\t"
     set vObjMsg(125)   "wtxObjModuleLoad loads objSampleWtxtclTest6.o with\
                        LOAD_NO_SYMBOLS flag\t"
     set vObjMsg(126)   "wtxObjModuleLoad loads objSampleWtxtclTest6.o with\
                        LOAD_HIDDEN_MODULE flag\t"
     set vObjMsg(127)   "wtxObjModuleLoad loads cplus.o with\n\
                        \t\t\t\tLOAD_CPLUS_XTOR_MANUAL flag\t"
     set vObjMsg(128)   "wtxObjModuleLoad loads cplus.o with\
                        LOAD_CPLUS_XTOR_AUTO flag\t"
     set vObjMsg(129)   "wtxObjModuleLoad with LOAD_COMMON_MATCH_NONE\
                        flag\t\t"
     set vObjMsg(130)   "wtxObjModuleLoad with LOAD_COMMON_MATCH_USER\
                        flag\t\t"
     set vObjMsg(131)   "wtxObjModuleLoad with LOAD_COMMON_MATCH_ALL\
                        flag\t\t"
     set vObjMsg(133)   "wtxObjModuleLoad copies and loads local\n\
                         \[tmp\]objSampleWtxtclTest6.o with LOAD_GLOBAL_SYMBOLS\
                         flag\t"
     set vObjMsg(135)   "wtxObjModuleLoadStart loads objSampleWtxtclTest2.o\n\
                         and checks if correctly loaded\t\t\t\t\t"
     set vObjMsg(136)   "wtxObjModuleLoadProgressReport gets a report of a\
                         load\t\t"
     set vObjMsg(137)   "wtxObjModuleLoadCancel cancels a load while loading\t\t"
     set vObjMsg(138)   "wtxObjModuleLoadCancel returns error code (load done)\t\t"
     set vObjMsg(139)   "wtxObjModuleLoadProgressReport returns error code (no load)\
                         \t"
     set vObjMsg(140)   "wtxObjModuleLoadProgressReport returns error code (load done)\
                         \t"
     set vObjMsg(141)   "wtxObjModuleInfoAndPathGet returns right info and path\ 
       (on a loaded module, wtxObjModuleInfoGetTest1.o)\t\t"
     set vObjMsg(142)   "wtxObjModuleInfoAndPathGet returns right path (on vxWorks)\t"


#=================================
# symbol table operations messages
#=================================

     set vSymMsg(0)	"wtxSymAddTest adds symTest symbol\t\t\t\t"
     set vSymMsg(1)	"wtxSymFindTest finds WTXsym symbol\t\t\t\t"
     set vSymMsg(2)	"wtxSymListGetTest -name lists symTest symbol\t\t\t"
     set vSymMsg(3)	"wtxSymListGetTest -value lists symTest symbol\t\t\t"
     set vSymMsg(4)	"wtxSymListGetTest lists symTest symbol\t\t"
     set vSymMsg(5)	"wtxSymRemoveTest removes symTest symbol\t\t\t\t"
     set vSymMsg(6)	"wtxSymFindTest works properly"
     set vSymMsg(7)	"wtxSymFindTest manages non existing symbols correctly\t\t"
     set vSymMsg(8)	"wtxSymRemoveTest removes symbol"
     set vSymMsg(9)	"wtxSymRemoveTest manages bad symbol names correctly\t\t"
     set vSymMsg(10)	"wtxSymListGetTest manages empty lists\t\t\t\t"
     set vSymMsg(11)	"wtxSymListGetTest manages regular expressions\t\t\t"
     set vSymMsg(12)	"wtxSymAdd adds a 500 characters symbol\t\t\t\t"
     set vSymMsg(13)	"wtxSymTblInfoGet prints right information\t\t\t"
     set vSymMsg(14)	"wtxSymListGetTest -module lists symbols of a specific\
			module\t"
     set vSymMsg(15)	"wtxSymListGetTest -unknown lists unknown symbols\t\t"
     set vSymMsg(16)	"wtxSymListGetTest loads a file that contains 1050\
			symbols\n and verifies if their values are correct\
			\t\t\t"
     set vSymMsg(17)	"wtxSymListGet manages situations where the\
			number of symbol\n frames is a multiple of\
			the symbol frame size\t\t\t"
     set vSymMsg(18)	"wtxSymListGetTest -module manages non-existent\
			modules\t\t"
     set vSymMsg(19)	"wtxSymListGetTest -name manages non-existent\
			symbols\t\t"
     set vSymMsg(20)	"wtxSymListGetTest -name and -module options\
			and invalid strings\t"
     set vSymMsg(21)	"wtxSymAdd truncates the symbol name in SYM_ADDED\
			\n event when symbols are too long\t\t\t\t"
     set vSymMsg(22)	"wtxSymListGet gets correct symbol types\t\t\t\t"


#===============================
# eventpoint management messages
#===============================

     set vEvptMsg(1)	"wtxEventpointAdd adds a breakpoint on user task\t\t\t"
     set vEvptMsg(2)	"wtxEventpointdelete deletes a breakpoint\t\t\t"
     set vEvptMsg(3)	"wtxEventpointList lists breakpoints\t\t\t\t"
     set vEvptMsg(4)	"wtxEventpointdelete deletes a specific breakpoint\
				in a list\t"
     set vEvptMsg(5)	"wtxEventpointdelete manages invalid eventpoint id\t\t"
     set vEvptMsg(6)	"wtxEventpointAdd manages ACTION_CALL functionality\t\t"
     set vEvptMsg(7)    "wtxEventpointListGet lists breakpoints\t\t\t\t"
     set vEvptMsg(8)    "wtxEventpointAdd with more than 1 entry point\t\t\t"


#===============================
# event management messages
#===============================

     set vEvMsg(0)	"wtxEventGet gets information about event\
			breakpoint\t\t"
     set vEvMsg(1)	"wtxRegisterForEvent enables all events\t\t\t\t"
     set vEvMsg(2)	"wtxRegisterForEvent masks all events except\
				EVENTTEST event\t" 
     set vEvMsg(4)	"wtxRegisterForEvent manages regular expressions\t" 
     set vEvMsg(5)	"wtxEventAdd adds EVENTTEST event\t\t\t\t" 
     set vEvMsg(6)	"wtxEventAdd adds an event of 256 characters\t\t\t" 
     set vEvMsg(7)	"wtxEventAdd adds event" 
     set vEvMsg(8)	"wtxEventGet gets EVENTTEST event\t\t\t\t" 
     set vEvMsg(9)	"wtxEventGet gets EXCEPTION event when an exception\
			occured\t"
     set vEvMsg(10)     "wtxEventListGet receives 3 events\n\
			     (EVENTTEST01, EVENTTEST02 , EVENTTEST03)\t\t\t"


#==================
# register messages
#==================

     set vRegsMsg(0)	"wtxRegsGets gets registers\t\t\t\t\t"
     set vRegsMsg(1)	"wtxRegsSets sets registers \t\t\t\t\t"
     set vRegsMsg(2)	"wtxRegsSets manages bad context Ids\t\t\t\t"
     
#==================
# registry messages
#==================
 
     set registerMsg(0)	"wtxRegister registers an item and check its validity\t\t"
     set registerMsg(1)	"wtxRegister verifies the NAME_CLASH error ability\t\t"
     set registerMsg(2)	"wtxUnregister unregisters an item\t\t\t\t"
     set registerMsg(3)	"wtxInfoQ gets all registry items\t\t\t\t"
     set registerMsg(4)	"wtxInfoQ gets items by name\t\t\t\t\t"
     set registerMsg(5)	"wtxInfoQ gets items by type\t\t\t\t\t"
     set registerMsg(6) "wtxInfoQ gets items by key\t\t\t\t\t"


#=====================
# virtual I/O messages
#=====================


# wtxOpen service tests
# --------------------- 

set openMsg(1) "Opening existing file for reading with no VIO redirection -\
\n returns valid file descriptor, nothing is written in the VIO\t"

set openMsg(2) "\nOpening non-existent file for reading with no VIO redirection -\
\n Opening non-existent file for reading with VIO redirection   -\
\n generates right error code, nothing is written in the VIO\t"

set openMsg(3) "\nOpening pipe for reading with no VIO redirection -\
\n returns valid file descriptor, nothing is written in the VIO\t"

set openMsg(4) "\nOpening existing file for reading with VIO redirection -\
\n the redirection is performed into the right channel, the file\
\n has been passed on entirely\t\t\t\t\t"

set openMsg(5) "\nOpening pipe for reading with VIO redirection -\
\n the redirection is performed into the right channel, pipe\
\n command result has been passed on entirely\t\t\t"

set openMsg(6) "\nOpening pipe for reading with no VIO redirection \
\n / with VIO redirection and with invalid flag - \
\n generates right error code, nothing is written in the VIO\t"

set openMsg(7) "\nOpening an existing file for reading with an\
invalid <channel>\t\t\t"



# wtxVioWrite service tests
# ------------------------- 

    set vioWriteMsg(1) "wtxVioWrite writes the right memory block\
			 to a VIO channel\t"

    set vioWriteMsg(2) "wtxVioWrite writes the right string\
			 to a VIO channel\t\t" 


# wtxClose service tests
# ----------------------

    set closeMsg(1) "wtxClose with valid file descriptor\t\t\t\t"
    set closeMsg(2) "wtxClose with valid pipe descriptor\t\t\t\t"
    set closeMsg(3) "wtxClose with invalid file descriptor\t\t\t\t"


# wtxVioCtl service tests
# -----------------------

    set vioCtlMsg(1) "wtxVioCtl with valid parameters\t\t\t\t\t"
    set vioCtlMsg(2) "wtxVioCtl with invalid <fd> parameter\t\t\t\t"
    set vioCtlMsg(3) "wtxVioCtl with invalid <VIO_CTL_REQUEST> parameter\t\t"


# wtxVioRead service tests
# ------------------------

    set vioReadMsg(1) "wtxEventGet() routine gets all VIO data :\t\t"

# wtxVioFileList service tests
# ----------------------------

    set vioFileList(1) "wtxVioFileList returns right parameters\n file uses\
			no vio redirection and file exists\t\t\t"
    set vioFileList(2) "wtxVioFileList returns right parameters\n file uses\
			no vio redirection and uses a pipe\t\t\t"
    set vioFileList(3) "wtxVioFileList returns right parameters\n file uses\
			vio redirection and file is new\t\t\t"

# wtxVioChanGet service tests
# ---------------------------
    set vioChanGetMsg(1) "Virtual I/O has been obtained correctly\t\t\t\t"

#=======================
# miscellaneous messages
#=======================


# wtxConsoleCreate service tests
# ------------------------------ 

    set csleCreateMsg(1) "wtxConsoleCreate result with valid\
                                   DISPLAY parameter\t\t"
    set csleCreateMsg(2) "wtxConsoleCreate called without the\
                                   <display> parameter\t\t"
    set csleCreateMsg(3) "wtxConsoleCreate result with invalid\
                                   DISPLAY parameter\t\t"

# wtxConsoleKill service tests
# ---------------------------- 

    set csleKillMsg(1)   "wtxConsoleKill with valid\
                                   <console-pid> parameter\t\t"
    set csleKillMsg(2)   "wtxConsoleKill with invalid\
                                   <console-pid> parameter\t\t"

# wtxFuncCall service tests
# -------------------------

    set funcCallMsg(1)   "wtxFuncCall -int with integer arguments\t\t\t\t"
    set funcCallMsg(2)   "wtxFuncCall with too many routine arguments\t\t\t"
    set funcCallMsg(3)   "wtxFuncCall -float with floating arguments\t\t\t"
    set funcCallMsg(4)   "wtxFuncCall -double with double arguments\t\t\t"
    set funcCallMsg(5)	 "wtxFuncCall manages file redirections\t\t\t\t"

# wtxDirectCall service tests
# -------------------------

    set directCallMsg(1) "wtxDirectCall with valid arguments\t\t\t\t"
    set directCallMsg(2) "wtxDirectCall with too many routine arguments\t\t\t"

# wtxGopherEval service tests
# ---------------------------

    set gopherEvalMsg(1)  "Get a gopher tape in one shot (tape size < 1400)\t\t"
    set gopherEvalMsg(2)  "wtxGopherEval with bad arguments\t\t\t\t"
    set gopherEvalMsg(3)  "Get AGENT_GOPHER_TRUNCATED error when the\
		communication\n buffer is too small for the request\t\t\t\t"
    set gopherEvalMsg(4)  "Get a multiple pieces gopher tape (1400\
    			< tape size < 14000)\t"

# wtxAgentModeSet service tests
# -----------------------------

    set agentModeSetMsg(1)  "Set agent mode correctly\t\t\t\t\t"

# wtxAgentModeGet service tests
# -----------------------------

    set agentModeGetMsg(1)  "wtxAgentModeGet returns right information\t\t\t"

# wtxServiceAddTest service tests
#--------------------------------

    set serviceAddMsg(1)	"wtxServiceAdd adds and verifies\
				wtxClientTest service\t\t"
    set serviceAddMsg(2)	"wtxServiceAdd manages non-existent\
				object files\t\t\t"
    set serviceAddMsg(3)	"wtxServiceAdd manages non-existent\
				service names\t\t\t"
    set serviceAddMsg(4)	"wtxServiceAdd manages non-existent\
				input or output procedures\t"

# wtxTargetAttachTest service tests
#----------------------------------

    set tgtAttachMsg(1)	"wtxTargetAttach return right value\t\t\t\t"

# wtxLoggingTest service tests
#----------------------------------

    set loggingMsg(1)   "Create a wtx logging file and check creation\t\t\t"
    set loggingMsg(2)   "Create a wdb logging file and check creation\t\t\t"
    set loggingMsg(3)   "Stop all logging\t\t\t\t\t\t"
    set loggingMsg(4)   "Sends WTX commands, fills log file and check limit\t\t" 
    set loggingMsg(5)   "Sends WDB commands, fills log file and check limit\t\t"

# wtxUnregisterForEventTest service tests
#----------------------------------
 
    set UnevMsg(1)   "wtxUnregisterForEventTest disables no event\t\t\t"
    set UnevMsg(2)   "wtxUnregisterForEventTest masks EVENTTEST events\t\t"

# wtxCommandSend service tests
#----------------------------------
 
    set commandSendMsg(1) "wtxCommandSend wtxTsUnlock\t\t\t\t\t"
    set commandSendMsg(2) "wtxCommandSend wtxLoggingOn\t\t\t\t\t"
    set commandSendMsg(3) "wtxCommandSend wtxTsLock_20 (wait 20s)\t\t\t\t"
    set commandSendMsg(4) "wtxCommandSend wtxLoggingOff\t\t\t\t\t"
    set commandSendMsg(5) "wtxCommandSend wtxGnaGnaGna\t\t\t\t\t"
    set commandSendMsg(6) "wtxCommandSend wdbLoggingOn\t\t\t\t\t"
    set commandSendMsg(7) "wtxCommandSend wdbLoggingOff\t\t\t\t\t"
    set commandSendMsg(8) "wtxCommandSend allLoggingOff\t\t\t\t\t"
    set commandSendMsg(9) "wtxCommandSend ( no param. )\t\t\t\t\t"

# wtxObjModuleChecksum service tests
#----------------------------------
 
    set checksumMsg(1)  "wtxObjModuleChecksum <file>    \t\t\t\t\t"
    set checksumMsg(2)  "wtxObjModuleChecksum <moduleId>\t\t\t\t\t"
    set checksumMsg(3)  "wtxObjModuleChecksum 1         \t\t\t\t\t"
    set checksumMsg(4)  "wtxObjModuleChecksum WTX_ALL_MODULE_CHECK\t\t\t"
    set checksumMsg(5)  "wtxObjModuleChecksum with bad parameter\t\t\t\t"


# wtxAsyncNotifyEnable & Disable service tests
#----------------------------------
    set asyncNotifyMsg(1) "wtxAsyncNotifyEnable  \t\t\t\t\t\t"
    set asyncNotifyMsg(2) "wtxAsyncNotifyDisable \t\t\t\t\t\t"

#===========================
#  host cache test messages
#===========================

    set hostCacheMsg(1) "Read a memory block on the text segment\
			of a loaded module\n (only access to Target\
			 Server memory cache)\t\t\t"
    set hostCacheMsg(2) "Write a memory block on the text segment\
			of a loaded module\n (access to target\
			 memory)\t\t\t\t\t"
    set hostCacheMsg(3) "Read a memory block on the data segment\
			of a loaded module\n (access to target\
			 memory)\t\t\t\t\t"
    set hostCacheMsg(4) "Write a memory block on the data segment\
                        of a loaded module\n (access to target\
                         memory)\t\t\t\t\t"
    set hostCacheMsg(5)  "Read a memory block on the bss segment\
                        of a loaded module\n (access to target\
                         memory)\t\t\t\t\t"
    set hostCacheMsg(6) "Write a memory block on the bss segment\
                        of a loaded module\n (access to target\
                         memory)\t\t\t\t\t"

# end of uplevel 
}

