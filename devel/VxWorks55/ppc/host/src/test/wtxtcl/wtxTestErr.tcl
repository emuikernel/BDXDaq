# wtxTestErr.tcl - Tcl script, contains the testcase units error messages
#
# Copyright 1995-1996 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 02j,08jan99,fle  added wtxRegister, wtxUnregister and wtxInfoQ error
#                  messages
# 02i,14may98,p_b  Replace test?.o by objSampleWtxtclTest?.o
# 02h,12may98,fle  added errMiscMsg(35 -> 37)
# 02g,23jan98,c_c  added errMemMsg(33->37) for wtxMemDisassemble
# 02f,05dec97,fle  added registry test messages
# 02e,21jul97,f_l  added errHostCache(1-10)
# 02d,01oct96,jmp  added errObjMsg(31), errEvptMsg(11-12).
# 02e,22sep97,jmp  Modified a Gopher error message.
# 02c,25jul96,jmp  added errObjMsg(28->30). updated Copyright.
# 02b,13mar96,jmp  added errSymMsg(16->18), errObjMsg(19->27).
# 02a,26feb96,jmp  added errObjMsg(18), errMiscMsg(32->34), agentErrMsg(13).
# 	     +l_p  corrected English mistakes in a few messages.
# 01z,18oct95,jmp  added errObjMsg(11->16), errEvMsg(7),errVioMsg(27),
#		   errMiscMsg(27->30), errSymMsg(15).
# 01y,12sep95,jmp  added errMiscMsg(23->26), errRegsMsg(3).
# 01x,21jul95,jmp  added errMemMsg(42->45), errCtxMsg(15),
#		   changed agentErrMsg(12).
# 01w,23jun95,f_v  added errSymMsg(12)
# 01v,09jun95,f_v  added errMiscMsg(14),errMsg(3)
# 01u,24may95,f_v  added errEvptMsg(10),agentErrMsg(11->12),errMemMsg(23)
#		   added errVioMsg(3)
# 01t,22may95,f_v  added errSymMsg(10),errSymMsg(11)
# 01s,22may95,jcf  name revision.
# 01r,17may95,f_v  changed errSymMsg(8)
# 01q,15may95,f_v  added errMemMsg(2),errMemMsg(20)
# 01p,03may95,f_v  added errCtxMsg(14)
# 01o,28apr95,f_v  added agentErrMsg(10)
# 01n,12apr95,f_v  added errObjMsg(12)
# 01m,04apr95,f_v  added gopher error message errMiscMsg(16 -> 24)
#		   added errCtxMsg(10->14)
# 01l,28mar95,f_v  added errMemMsg(36->41)
# 		   changed pomme to test1
# 01k,22feb95,f_v  changed errMemMsg(35)
# 01j,22feb95,f_v  added errEvMsg(),added errSymMsg(9)
# 01i,22feb95,kpn  added errVioMsg(24),  deleted errMemMsg(1)(2)(4)(5)(6)
#		   errMemMsg(20)(23)(28).
#		   added errCtxMsg(9).
# 01h,17feb95,f_v  add errObjMsg(9->11),errSymMsg(8)
# 01g,13feb95,kpn  added errVioMsg(18 -> 23), errMemMsg(32 -> 36)
# 01f,06feb95,f_v  replace VxWorks by objSampleWtxtclTest5.o in Obj messages
#		   add errRegsMsg(0->2)
#		   add event management section
#	 	   change errEvMsg to errEvptMsg in eventpoint section 	
#		   move errEvptMsg(0) to errEvMsg(0)
#		   add errEvMsg (0->3)
# 01e,06feb95,kpn  added errMemMsg (25 -> 31), errVioMsg (13 -> 17)
# 01d,02feb95,f_v  replace multiple uplevel by an unique
#	           added errSymMsg(4->7),errObjMsg(6)(7)
# 01c,31jan95,kpn  added errVioMsg (9 -> 12)
# 01b,30jan95,kpn  added errMiscMsg (1 -> 15), errMemMsg (1 -> 24),
#		   errVioMsg (1-> 8), errSessionMsg (1 -> 3).	
#		   documentation.
# 01a,27Jan95,f_v  written.
#




#
# DESCRIPTION
#
# Initialization of testcase units error messages
# The error messages are classified by service category.
#
# When a testcase has failed at the time of its execution these messages allow
# to display the reason of the failure.
#

uplevel #0 {

#================
# global messages
#================

     set errMsg(0) 	"ERROR : Not detached from Target Server"
     set errMsg(1) 	"ERROR : Wrong arguments weren't handled correctly"
     set errMsg(2)      "ERROR : WTX request wasn't supported\
				  wrong arguments"
     set errMsg(3)      "ERROR : WTX request failed in timeout"

#================
# agent messages
#================

     set agentErrMsg(1)  "ERROR : Agent unsupported request get" 
     set agentErrMsg(2)  "ERROR : Agent invalid  context"
     set agentErrMsg(3)  "ERROR : Agent external mode error"
     set agentErrMsg(4)  "ERROR : Agent internal mode error"
     set agentErrMsg(5)  "ERROR : Agent communication error"
     set agentErrMsg(6)  "ERROR : Agent memory access error"
     set agentErrMsg(7)  "ERROR : Agent gopher wasn't truncated"
     set agentErrMsg(8)  "ERROR : Agent gopher fault wasn't get"
     set agentErrMsg(9)  "ERROR : Agent gopher fault"
     set agentErrMsg(10) "ERROR : Agent invalid context wasn't detected"
     set agentErrMsg(11) "ERROR : Agent task mode set failed"
     set agentErrMsg(12) "\nWARNING : Agent does not support external mode"
     set agentErrMsg(13) "ERROR : wtxAgentModeGet doesn't returned\
			the right mode"


#============================
# session management messages
#============================

     set errSessionMsg(1) "ERROR : The given target server name \
                                    is unknown"
     set errSessionMsg(2) "ERROR : Can't detach tool, invalid tool\
                                    ID"
     set errSessionMsg(3) "ERROR : You must detach existing\
                                    connection before making this test"
     set errSessionMsg(4) "ERROR : Information was wrong"
     set errSessionMsg(5) "ERROR : Invalid arguments are not managed"
     set errSessionMsg(6) "ERROR : wtxToolDetach does not report that\
				no handle is found"

#=========================
# context control messages
#=========================

     set errCtxMsg(0)  	 "ERROR : Context wasn't continued"
     set errCtxMsg(1)  	 "ERROR : Task uncreated"
     set errCtxMsg(2)  	 "ERROR : Task has not been killed"
     set errCtxMsg(3)  	 "ERROR : Test task doesn't exist"
     set errCtxMsg(4)  	 "ERROR : tCallTask task did not resume"
     set errCtxMsg(5)  	 "ERROR : tCallTask task did not exist"
     set errCtxMsg(6)  	 "ERROR : Context wasn't single stepped"
     set errCtxMsg(7)  	 "ERROR : tCallTask Task has not been created"
     set errCtxMsg(8)  	 "ERROR : tCallTask Task has not been suspended"
     set errCtxMsg(9)    "ERROR : context create not supported by debug agent"
     set errCtxMsg(10)   "ERROR : wtxContextCont request failed"
     set errCtxMsg(13)   "ERROR : Context was not created"
     set errCtxMsg(14)   "ERROR : Bad parameters are not managed"
     set errCtxMsg(15)   "ERROR : wtxContextCreate request failed"
     set errCtxMsg(16)   "ERROR : Task did not return the right value"

#===========================
# memory operations messages
#===========================

     set errMemMsg(1)  "ERROR : Bad <source address> arguments are not managed"
     set errMemMsg(2)	"ERROR : Bad <destination address> arguments are not\
				managed"
     set errMemMsg(3)  "ERROR : Bad <size> arguments are not managed"
     set errMemMsg(4)	"ERROR : Memory blocks have not been moved correctly"
     set errMemMsg(5)	"ERROR : The read block has not the right value"
     set errMemMsg(6)	"ERROR : Bad <starting address> arguments are not\
				managed"
     set errMemMsg(7)  "ERROR : memory allocated by wtxMemRealloc\
				was not freed"
     set errMemMsg(8)  "ERROR : Bad <address> arguments are not managed"
     set errMemMsg(9)  "ERROR : Memory block has not been found"
     set errMemMsg(10) "ERROR : -memblk option did not work correctly"
     set errMemMsg(11) "ERROR : -string option did not work correctly"
     set errMemMsg(12) "ERROR : The set block has not the right value"
     set errMemMsg(13) "ERROR : wtxMemSet did not clear the right bytes"
     set errMemMsg(14) "ERROR : The written block has not the right value"
     set errMemMsg(15) "ERROR : wtxMemWrite did not write in the\
				right required block of memory"
     set errMemMsg(16) "ERROR : Bad <blockId> arguments are not managed"
     set errMemMsg(17) "ERROR : Memory has not been added to target\
				memory pool"
     set errMemMsg(18) "ERROR : Returned address was not a multiple of\
				alignment"
     set errMemMsg(19) "ERROR : Bad <alignment> arguments are not managed"
     set errMemMsg(20) "ERROR : wtxMemAlloc call failed"
     set errMemMsg(21) "ERROR : The returned checksum is not the right value"
     set errMemMsg(22) "ERROR : Memory has not been freed"
     set errMemMsg(23) "ERROR : Current number of free bytes is wrong"
     set errMemMsg(24) "ERROR : Current number of allocated \
				bytes is wrong"
     set errMemMsg(25) "ERROR : Cumulative number of allocated\
				bytes is wrong"
     set errMemMsg(26) "ERROR : Current number of free blocks is wrong"
     set errMemMsg(27) "ERROR : Current number of allocated\
				blocks is wrong"
     set errMemMsg(28) "ERROR : Cumulative number of allocated\
				blocks is wrong"
     set errMemMsg(29) "ERROR : The average size of free blocks is wrong"
     set errMemMsg(30) "ERROR : The average size of allocated blocks\
				is wrong"
     set errMemMsg(31) "ERROR : The cumulative average size of\
				allocated blocks is wrong"
     set errMemMsg(32) "ERROR : It is possible to allocate a block of memory\
				whose\n\tsize is bigger than the biggest free\
				block size !"
     set errMemMsg(33) "ERROR : wtxMemDisassemble didn't disassemble anything"
     set errMemMsg(34) "ERROR : wtxMemDisassemble didn't get the required \
                                 symbol name"
     set errMemMsg(35) "ERROR : wtxMemDisassemble wrong prepended addresses"
     set errMemMsg(36) "ERROR : wtxMemDisassemble wrong prepended opcodes"
     set errMemMsg(37) "ERROR : wtxMemDisassemble didn't get <nInst> \
                                 instructions"
     set errMemMsg(38) "ERROR : wtxMemDisassemble <endAddr> made result go \
                                 wrong"


#==================================
# object module management messages
#==================================

     set errObjMsg(1) 	"ERROR : Bad object module Ids are not managed"
     set errObjMsg(2)   "ERROR : Bad object module names are not managed"
     set errObjMsg(3)   "ERROR : wtxObjModuleInfoGet does not provide good\
				informations"
     set errObjMsg(4)   "ERROR : Loaded module is not listed"
     set errObjMsg(5)   "ERROR : Module objSampleWtxtclTest1.o was already loaded"
     set errObjMsg(6)   "ERROR : Module objSampleWtxtclTest1.o has not been loaded"
     set errObjMsg(7)	"ERROR : Module returned a wrong value"
     set errObjMsg(8)   "ERROR : Module objSampleWtxtclTest2.o has not been loaded"
     set errObjMsg(9)	"ERROR : .rms and .ref files were different"
     set errObjMsg(10)	"ERROR : Memory allocated by the module has not\
				been correctly freed"
     set errObjMsg(11)	"ERROR : Unable to change C++ static constructor\
				calling strategy"
     set errObjMsg(12)  "ERROR : Constructor calling strategy not respected"
     set errObjMsg(13)  "ERROR : Null size object modules are not managed"
     set errObjMsg(14)  "ERROR : Invalid object module formats are not managed"
     set errObjMsg(15)  "ERROR : Conflicting loading flags are not managed"
     set errObjMsg(16)  "ERROR : Invalid segment addresses are not managed"
     set errObjMsg(17)  "ERROR : Relocation in bss segment not correct"
     set errObjMsg(18)  "ERROR : No exception generated when there isn't\
				enought memory"
     set errObjMsg(19)	"ERROR : segment informations are wrong"
     set errObjMsg(20)	"ERROR : C++ application returns wrong results"
     set errObjMsg(21)	"ERROR : LOAD_GLOBAL_SYMBOLS flag not respected"
     set errObjMsg(22)	"ERROR : LOAD_LOCAL_SYMBOLS flag not respected"
     set errObjMsg(23)	"ERROR : LOAD_ALL_SYMBOLS flag not respected"
     set errObjMsg(24)	"ERROR : LOAD_NO_SYMBOLS flag not respected"
     set errObjMsg(25)	"ERROR : LOAD_HIDDEN_MODULE flag not respected"
     set errObjMsg(26)	"ERROR : LOAD_CPLUS_XTOR_MANUAL flag not respected"
     set errObjMsg(27)	"ERROR : LOAD_CPLUS_XTOR_AUTO flag not respected"
     set errObjMsg(28)	"ERROR : LOAD_COMMON_MATCH_NONE flag not respected"
     set errObjMsg(29)	"ERROR : LOAD_COMMON_MATCH_USER flag not respected"
     set errObjMsg(30)	"ERROR : LOAD_COMMON_MATCH_ALL flag not respected"
     set errObjMsg(31)	"ERROR : module group number management incorrect"


#=================================
# symbol table operations messages
#=================================

     set errSymMsg(0)	"ERROR : Symbol symTest not added"
     set errSymMsg(1)	"ERROR : Symbol WTXsym not found"
     set errSymMsg(2)	"ERROR : Symbol symTest not listed"
     set errSymMsg(3)	"ERROR : Symbol symTest not removed"
     set errSymMsg(4)	"ERROR : Non existing symbols are not properly managed"
     set errSymMsg(5)	"ERROR : Non existing lists are not properly managed"
     set errSymMsg(6)	"ERROR : In regular expressions {} ? and \[\] symbols\
		        	 don't work properly"
     set errSymMsg(7)	"ERROR : In regular expressions ^ * and $ symbols don't\
		        	 work properly"
     set errSymMsg(8)	"ERROR : A 200 characters symbol can not be added"
     set errSymMsg(9)	"ERROR : Information has not been printed"
     set errSymMsg(10)  "ERROR : -module option did not work properly"
     set errSymMsg(11)  "ERROR : -unknown option did not work properly"
     set errSymMsg(12)  "ERROR : no such module"
     set errSymMsg(13)	"ERROR : Symbol has not a right value"
     set errSymMsg(14)	"ERROR : wtxSymListGet did not return the whole\
				symbol list"
     set errSymMsg(15)	"ERROR : symbol names are not correctly truncated\
				when they are too long"
     set errSymMsg(16)	"ERROR : symbol types are wrong"
     set errSymMsg(17)	"ERROR : all symbols are not loaded"
     set errSymMsg(18)	"ERROR : absolute symbols are not correctly managed"



#===============================
# eventpoint management messages
#===============================

     set errEvptMsg(1)	"ERROR : Breakpoint has not been added"
     set errEvptMsg(2)	"ERROR : Breakpoint has not been deleted"
     set errEvptMsg(3)	"ERROR : EventpointList does not return 4\
				    parameters"
     set errEvptMsg(4)	"ERROR : Detection_type is wrong"
     set errEvptMsg(5)	"ERROR : Event_type is wrong"
     set errEvptMsg(6)	"ERROR : Breakpoint address is wrong"
     set errEvptMsg(7)	"ERROR : Context_type is wrong"
     set errEvptMsg(8)	"ERROR : Task address is wrong"
     set errEvptMsg(9)	"ERROR : Action_type is wrong"
     set errEvptMsg(10)	"ERROR : Breakpoint has not been added on system"
     set errEvptMsg(11)	"ERROR : Invalid eventpoint id are not managed"
     set errEvptMsg(12) "ERROR : ACTION_CALL functionality does not work"


#===============================
# event management messages
#===============================

     set errEvMsg(0)	"ERROR : Cannot obtain Event breakpoint"
     set errEvMsg(1)	"ERROR : All events are not enabled"
     set errEvMsg(2)	"ERROR : EVENTTEST events are not masked"
     set errEvMsg(3)	"ERROR : EVENTTEST events are not unmasked"
     set errEvMsg(4)	"ERROR : EVENTTEST event has not been added"
     set errEvMsg(5)	"ERROR : Cannot add 256 characters event"
     set errEvMsg(6)	"ERROR : Cannot get EVENTTEST test"
     set errEvMsg(7)	"ERROR : The obtained exception number is not the\
				expected event"
     set errEvMsg(8)    "ERROR : Cannot get one or more of the three messages"


#==================
# register messages
#==================

     set errRegsMsg(0)	"ERROR : wtxRegsGet gives wrong informations"
     set errRegsMsg(1)	"ERROR : Cannot obtain registers"
     set errRegsMsg(2)	"ERROR : bad context ids are not managed properly"
     set errRegsMsg(3)	"ERROR : wtxRegsSet does not set registers correctly"

#=====================
# virtual I/O messages
#=====================

     set errVioMsg(1)  "\n>>> WARNING >>> : Your WIND_BASE environment\
                                 variable DOES NOT EXIST. \nTo execute this test\
                                 YOU MUST set it to a value :\n(1) exit this \
                                 wtxtcl shell\n(2) setenv WIND_BASE <wpwr_base\
                                 value> \n(3) call the wtxtcl shell and try\
                                 this test again."
     set errVioMsg(2)  "ERROR : Flag parameter (O_RDONLY = 0) of wtxOpen\
                                 is invalid"
     set errVioMsg(3)  "ERROR : Could not obtain Virtual I/O channel"
     set errVioMsg(4)  "ERROR : File descriptor returned by wtxOpen\
				 is invalid"
     set errVioMsg(5)  "ERROR : wtxOpen result is wrong"
     set errVioMsg(6)  "ERROR : Not enough memory to allocate room for\
                                 data read from VIO channel 0"
     set errVioMsg(7)  "ERROR : Datas read from VIO channel 0 have not the\
                                 the right values"
     set errVioMsg(8)  "ERROR : VIO channel 0 device not available on Target"
     set errVioMsg(9)  "ERROR : wtxClose return value is wrong"
     set errVioMsg(10) "ERROR : wtxClose did not close the file, its fd is\
				still existing"
     set errVioMsg(11) "ERROR : wtxClose did not close the pipe, its fd is\
				 still existing"
     set errVioMsg(12) "ERROR : Target server invalid file descriptor"
     set errVioMsg(13) "ERROR : <control> argument is invalid"
     set errVioMsg(14) "ERROR : Invalid ioctl request"
     set errVioMsg(15) "ERROR : ioctl call failed"
     set errVioMsg(16) "ERROR : Invalid File Descriptor"
     set errVioMsg(17) "ERROR : fd is not an active descriptor"
     set errVioMsg(18) "ERROR : Bytes contained in VIO are not equal to 0"
     set errVioMsg(19) "FAILURE of the vioEmpty{} procedure execution"
     set errVioMsg(20) "FAILURE of the vioOpen{} procedure execution"
     set errVioMsg(22) "ERROR : number of written bytes not right"
     set errVioMsg(23) "ERROR : write() : number of written bytes \
				 not right"
     set errVioMsg(24) "FAILURE of the readContextCreate{} procedure\
			 execution"
     set errVioMsg(25) "ERROR : Informations returned by wtxVioFileList are\
			wrong"
     set errVioMsg(26) "ERROR : invalid <fd> parameters are not managed"
     set errVioMsg(27) "ERROR : invalid <VIO_CTL_REQUEST> parameters\
			are not managed"


#==================
# registry messages
#==================

    set errRegistryMsg(0)	"ERROR: Could not register item :"
    set errRegistryMsg(1)	"ERROR: registry item duplicated"
    set errRegistryMsg(2)	"ERROR: registry item not in list"
    set errRegistryMsg(3)	"ERROR: registry found and registered names\
				 differ"
    set errRegistryMsg(4)	"ERROR: registry found and registered types\
				 differ"
    set errRegistryMsg(5)	"ERROR: registry found and registered keys\
				 differ"
    set errRegistryMsg(6)	"ERROR: Could not unregister item :"
    set errRegistryMsg(7)	"ERROR: registry could overwrite a registered\
				 name"
    set errRegistryMsg(8)	"ERROR: REGISTRY_NAME_CLASH error awaited, but\
				 got :"
    set errRegistryMsg(9)	"ERROR: wtxInfoQ exception"
    set errRegistryMsg(10)	"ERROR: item normally removed still in database"
    set errRegistryMsg(11)	"ERROR: wtxInfoQ unable to retrieve info by name"
    set errRegistryMsg(12)	"ERROR: wtxInfoQ unable to retrieve info by type"
    set errRegistryMsg(13)	"ERROR: wtxInfoQ unable to retrieve info by key"


#=======================
# miscellaneous messages
#=======================

     set errMiscMsg(1) "\n>>> WARNING >>> : Your DISPLAY environment\
                                variable DOESN'T EXIST. \nTo execute this test\
                                YOU MUST set it to a value :\n(1) exit this \
                                wtxtcl shell\n(2) setenv DISPLAY <display\
                                value> \n(3) call the wtxtcl shell and try this\
                                test again."
     set errMiscMsg(2) "\n>>> WARNING >>> : Your DISPLAY environment\
                                variable contains an invalid value. \nTo\
                                execute this test you must set this\
                                variable to a VALID VALUE : \n(1) exit this wtxtcl\
                                shell \n(2) setenv DISPLAY <display value>\n(3)\
                                call the wtxtcl shell and try this test\
                                again."
     set errMiscMsg(3)  "ERROR : Console name returned value not valid"
     set errMiscMsg(4)  "ERROR : DISPLAY returned value is not valid"
     set errMiscMsg(5)  "ERROR : Console id returned value is not valid"
     set errMiscMsg(6)  "ERROR : wtxConsoleKill result not right"
     set errMiscMsg(7)  "ERROR : Invalid <console-pid> parameters are not\
				managed"
     set errMiscMsg(8)  "ERROR : Module funcCallTest.o not found"
     set errMiscMsg(9)  "ERROR : Result of the called function not right"
     set errMiscMsg(10) "ERROR : A function can be called even if\
                                  wtxFuncCall has too much parameters"
     set errMiscMsg(11) "ERROR : Unload object module failed"
     set errMiscMsg(12) "ERROR : module address returned by\
                                  wtxObjModuleFind is wrong"
     set errMiscMsg(13) "ERROR : module name returned by\
                                  wtxObjModuleFind is wrong"
     set errMiscMsg(14) "ERROR : Server can not start VIO console"
     set errMiscMsg(17) "ERROR : Gopher returned information was wrong"
     set errMiscMsg(21) "ERROR : WTX request was not supported\
				  wrong paremeters"
     set errMiscMsg(22) "ERROR : Invalid display parameters are not managed"
     set errMiscMsg(23) "\nWARNING : Floating point can not be used on\
				this board\n"
     set errMiscMsg(24) "ERROR : File redirection does not work properly"
     set errMiscMsg(25) "\nWARNING : Test not available, symbol printf\
				not found\n"
     set errMiscMsg(26) "\nWARNING : Test not available, symbol scanf\
				not found\n"
     set errMiscMsg(27) "ERROR : wtxClientTest does not return the right\
				value"
     set errMiscMsg(28) "ERROR : Non-existent object files are not managed"
     set errMiscMsg(29) "ERROR : Non-existent service names are not managed"
     set errMiscMsg(30) "ERROR : Non-existent input or output procedures\
				are not managed"
     set errMiscMsg(31) "ERROR : wtxTargetAttach does not return the right\
				value when target is already attached"
     set errMiscMsg(32) "ERROR : wtxTsLock failed"
     set errMiscMsg(33) "ERROR : wtxTsUnlock failed"
     set errMiscMsg(34) "ERROR : wtxDirectCall can be called with more\
			than ten parameters"
     set errMiscMsg(35) "ERROR : no CPU info returned for this target server"
     set errMiscMsg(36) "ERROR : wrong number of returned parameters"
     set errMiscMsg(37) "ERROR : parameter is wrong with option"

#==================================
# command Send error Messages
#==================================
    set errCommandSendMsg(1) "ERROR : File ./wtxLog is missing"
    set errCommandSendMsg(2) "ERROR : WTX Logging not stopped"
    set errCommandSendMsg(3) "ERROR : File ./wdbLog is missing"
    set errCommandSendMsg(4) "ERROR : WDB Logging not stopped"
    set errCommandSendMsg(5) "ERROR : Should have failed"


#==================================
# host cache test error messages 
#==================================



    set errHostCache(1)   "ERROR : Read access on target\
				object module text segment"
    set errHostCache(2)  "ERROR : Unexpected Write access on target memory "
    set errHostCache(3)  "ERROR : no Write access on target\
				object module text segment"
    set errHostCache(4)  "ERROR : Write access on target\
				but wrong address or number of bytes"
    set errHostCache(5)  "ERROR : Unexpected Read access on target memory "
    set errHostCache(6)  "ERROR : no Read access on target\
				object module data segment"
    set errHostCache(7)  "ERROR : Read access on target\
				but wrong address or number of bytes"
    set errHostCache(8)  "ERROR : no Read access on target\
				object module bss segment"
    set errHostCache(9)  "ERROR : no Write access on target\
 				object module data segment"
    set errHostCache(10)  "ERROR : no Write access on target\
				object module bss segment"

#end of uplevel
}	
return
