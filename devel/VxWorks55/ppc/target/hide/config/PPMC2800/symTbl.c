/* symTbl.c - standalone symbol tables wrapper */

/* CREATED BY /usr/local/clas/devel/VxWorks55/ppc/host/src/hutils/makeSymTbl.tcl
 *  WITH ARGS ppc tmp.o symTbl.c
 *         ON Wed Apr 01 15:52:17 EDT 2009
 */

#include "vxWorks.h"
#include "symbol.h"

IMPORT int Bbase__9streambuf ();
IMPORT int Bptr__9streambuf ();
IMPORT int DCACHE_CTRL;
IMPORT int DCD_regs_dump ();
IMPORT int DELAY ();
IMPORT int EEPROMDetect ();
IMPORT int FlashCmdE;
IMPORT int FlashCmdS;
IMPORT int GET_THRM1 ();
IMPORT int GET_THRM3 ();
IMPORT int Gbase__9streambuf ();
IMPORT int IntLatencyTotal;
IMPORT int IntTimeBaseEnd;
IMPORT int IntTimeBaseSt;
IMPORT int MV64360_regs_dump ();
IMPORT int Nbase__9streambuf ();
IMPORT int PCI_DCD_regs_dump ();
IMPORT int PCI_REMAP_regs_dump ();
IMPORT int SDMA_RXBD_regs_dump ();
IMPORT int SDMA_TXBD_regs_dump ();
IMPORT int SET_THRM1 ();
IMPORT int SET_THRM3 ();
IMPORT int ShowCfdRing ();
IMPORT int TestIntConnect ();
IMPORT int _$_10bad_typeid ();
IMPORT int _$_11logic_error ();
IMPORT int _$_11range_error ();
IMPORT int _$_12domain_error ();
IMPORT int _$_12length_error ();
IMPORT int _$_12out_of_range ();
IMPORT int _$_12streammarker ();
IMPORT int _$_13bad_exception ();
IMPORT int _$_13runtime_error ();
IMPORT int _$_14__si_type_info ();
IMPORT int _$_14overflow_error ();
IMPORT int _$_15underflow_error ();
IMPORT int _$_16__user_type_info ();
IMPORT int _$_16invalid_argument ();
IMPORT int _$_17__class_type_info ();
IMPORT int _$_22_IO_istream_withassign ();
IMPORT int _$_22_IO_ostream_withassign ();
IMPORT int _$_3ios ();
IMPORT int _$_7filebuf ();
IMPORT int _$_7istream ();
IMPORT int _$_7ostream ();
IMPORT int _$_8bad_cast ();
IMPORT int _$_8iostream ();
IMPORT int _$_9bad_alloc ();
IMPORT int _$_9exception ();
IMPORT int _$_9streambuf ();
IMPORT int _$_9type_info ();
IMPORT int _$_Q2t24__default_alloc_template2b1i0_5_Lock ();
IMPORT int _$_t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int _7filebuf$openprot;
IMPORT int _GLOBAL_$D$_IO_stdin_ ();
IMPORT int _GLOBAL_$D$_t24__default_alloc_template2b1i0$_S_end_free ();
IMPORT int _GLOBAL_$D$_un_link__9streambuf ();
IMPORT int _GLOBAL_$I$_IO_stdin_ ();
IMPORT int _GLOBAL_$I$_t24__default_alloc_template2b1i0$_S_end_free ();
IMPORT int _GLOBAL_$I$_un_link__9streambuf ();
IMPORT int _IO_adjust_column ();
IMPORT int _IO_cleanup ();
IMPORT int _IO_cleanup_registration_needed;
IMPORT int _IO_default_doallocate ();
IMPORT int _IO_default_finish ();
IMPORT int _IO_default_pbackfail ();
IMPORT int _IO_default_read ();
IMPORT int _IO_default_seek ();
IMPORT int _IO_default_seekoff ();
IMPORT int _IO_default_seekpos ();
IMPORT int _IO_default_setbuf ();
IMPORT int _IO_default_stat ();
IMPORT int _IO_default_sync ();
IMPORT int _IO_default_uflow ();
IMPORT int _IO_default_underflow ();
IMPORT int _IO_default_write ();
IMPORT int _IO_default_xsgetn ();
IMPORT int _IO_default_xsputn ();
IMPORT int _IO_do_write ();
IMPORT int _IO_doallocbuf ();
IMPORT int _IO_dtoa ();
IMPORT int _IO_file_attach ();
IMPORT int _IO_file_close ();
IMPORT int _IO_file_close_it ();
IMPORT int _IO_file_doallocate ();
IMPORT int _IO_file_finish ();
IMPORT int _IO_file_fopen ();
IMPORT int _IO_file_init ();
IMPORT int _IO_file_jumps;
IMPORT int _IO_file_overflow ();
IMPORT int _IO_file_read ();
IMPORT int _IO_file_seek ();
IMPORT int _IO_file_seekoff ();
IMPORT int _IO_file_setbuf ();
IMPORT int _IO_file_stat ();
IMPORT int _IO_file_sync ();
IMPORT int _IO_file_underflow ();
IMPORT int _IO_file_write ();
IMPORT int _IO_file_xsputn ();
IMPORT int _IO_flush_all ();
IMPORT int _IO_flush_all_linebuffered ();
IMPORT int _IO_free_backup_area ();
IMPORT int _IO_getc ();
IMPORT int _IO_getline ();
IMPORT int _IO_getline_info ();
IMPORT int _IO_init ();
IMPORT int _IO_init_marker ();
IMPORT int _IO_link_in ();
IMPORT int _IO_list_all;
IMPORT int _IO_marker_delta ();
IMPORT int _IO_marker_difference ();
IMPORT int _IO_outfloat ();
IMPORT int _IO_padn ();
IMPORT int _IO_peekc_locked ();
IMPORT int _IO_putc ();
IMPORT int _IO_remove_marker ();
IMPORT int _IO_seekmark ();
IMPORT int _IO_seekoff ();
IMPORT int _IO_seekpos ();
IMPORT int _IO_setb ();
IMPORT int _IO_sgetn ();
IMPORT int _IO_sputbackc ();
IMPORT int _IO_stderr_;
IMPORT int _IO_stdin_;
IMPORT int _IO_stdout_;
IMPORT int _IO_strtod ();
IMPORT int _IO_sungetc ();
IMPORT int _IO_switch_to_backup_area ();
IMPORT int _IO_switch_to_get_mode ();
IMPORT int _IO_switch_to_main_get_area ();
IMPORT int _IO_un_link ();
IMPORT int _IO_ungetc ();
IMPORT int _IO_unsave_markers ();
IMPORT int _IO_vfscanf ();
IMPORT int _Randseed;
IMPORT int _S_chunk_alloc__t24__default_alloc_template2b1i0UiRi ();
IMPORT int _S_freelist_index__t24__default_alloc_template2b1i0Ui ();
IMPORT int _S_oom_malloc__t23__malloc_alloc_template1i0Ui ();
IMPORT int _S_oom_realloc__t23__malloc_alloc_template1i0PvUi ();
IMPORT int _S_refill__t24__default_alloc_template2b1i0Ui ();
IMPORT int _S_round_up__t24__default_alloc_template2b1i0Ui ();
IMPORT int __10RBString_T ();
IMPORT int __10RBString_TPCc ();
IMPORT int __10RBString_TR10RBString_T ();
IMPORT int __10bad_typeid ();
IMPORT int __11logic_errorRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int __11range_errorRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int __12domain_errorRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int __12length_errorRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int __12out_of_rangeRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int __12streammarkerP9streambuf ();
IMPORT int __13bad_exception ();
IMPORT int __13runtime_error ();
IMPORT int __13runtime_errorRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int __14__si_type_infoPCcRC16__user_type_info ();
IMPORT int __14overflow_errorRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int __15underflow_errorRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int __16__user_type_infoPCc ();
IMPORT int __16invalid_argumentRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int __17__class_type_infoPCcPCQ217__class_type_info9base_infoUi ();
IMPORT int __18RBStringIterator_TRC10RBString_T ();
IMPORT int __3iosP9streambufP7ostream ();
IMPORT int __7filebuf ();
IMPORT int __7filebufi ();
IMPORT int __7filebufiPci ();
IMPORT int __7istreami ();
IMPORT int __7istreamiP9streambufP7ostream ();
IMPORT int __7ostreami ();
IMPORT int __7ostreamiP9streambufP7ostream ();
IMPORT int __8bad_cast ();
IMPORT int __8iostreami ();
IMPORT int __8iostreamiP9streambufP7ostream ();
IMPORT int __9exception ();
IMPORT int __9streambufi ();
IMPORT int __9type_infoPCc ();
IMPORT int __AltiVec_runtime;
IMPORT int __DIAB_rd_pk_db ();
IMPORT int __DIAB_rd_pk_fl ();
IMPORT int __DIAB_rd_pk_ld ();
IMPORT int __DIAB_rd_sw_db ();
IMPORT int __DIAB_rd_sw_fl ();
IMPORT int __DIAB_rd_sw_ld ();
IMPORT int __DIAB_wr_pk_db ();
IMPORT int __DIAB_wr_pk_fl ();
IMPORT int __DIAB_wr_pk_ld ();
IMPORT int __DIAB_wr_sw_db ();
IMPORT int __DIAB_wr_sw_fl ();
IMPORT int __DIAB_wr_sw_ld ();
IMPORT int __Q23ios4Init ();
IMPORT int __Q2t24__default_alloc_template2b1i0_5_Lock ();
IMPORT int __READ_FLASH16 ();
IMPORT int __READ_FLASH32 ();
IMPORT int __READ_FLASH64 ();
IMPORT int __WRITE_FLASH16 ();
IMPORT int __WRITE_FLASH32 ();
IMPORT int __WRITE_FLASH64 ();
IMPORT int ___x_diab_comljmp_o;
IMPORT int ___x_diab_hfpfitof_o;
IMPORT int ___x_diab_hfpfutof_o;
IMPORT int ___x_diab_libg_o;
IMPORT int ___x_diab_rd_pk_db_o;
IMPORT int ___x_diab_rd_pk_fl_o;
IMPORT int ___x_diab_rd_pk_ld_o;
IMPORT int ___x_diab_rd_sw_db_o;
IMPORT int ___x_diab_rd_sw_fl_o;
IMPORT int ___x_diab_rd_sw_ld_o;
IMPORT int ___x_diab_restfprs_o;
IMPORT int ___x_diab_restfprx_o;
IMPORT int ___x_diab_restfreg_o;
IMPORT int ___x_diab_restgprx_o;
IMPORT int ___x_diab_rtahooks_o;
IMPORT int ___x_diab_savefprs_o;
IMPORT int ___x_diab_savefprx_o;
IMPORT int ___x_diab_savefreg_o;
IMPORT int ___x_diab_savegprx_o;
IMPORT int ___x_diab_sfpddtoll_o;
IMPORT int ___x_diab_sfpddtoull_o;
IMPORT int ___x_diab_sfpdlltod_o;
IMPORT int ___x_diab_sfpdnorm_o;
IMPORT int ___x_diab_sfpdulltod_o;
IMPORT int ___x_diab_sfpfftoll_o;
IMPORT int ___x_diab_sfpfftoull_o;
IMPORT int ___x_diab_sfpflltof_o;
IMPORT int ___x_diab_sfpfnorm_o;
IMPORT int ___x_diab_sfpfulltof_o;
IMPORT int ___x_diab_svrs_vpr_o;
IMPORT int ___x_diab_sxpadd64_o;
IMPORT int ___x_diab_sxpasr64_o;
IMPORT int ___x_diab_sxpcmp64_o;
IMPORT int ___x_diab_sxpdiv32_o;
IMPORT int ___x_diab_sxpdiv64_o;
IMPORT int ___x_diab_sxplsl64_o;
IMPORT int ___x_diab_sxplsr64_o;
IMPORT int ___x_diab_sxpmul64_o;
IMPORT int ___x_diab_sxprem32_o;
IMPORT int ___x_diab_sxprem64_o;
IMPORT int ___x_diab_torn_syncatomic_o;
IMPORT int ___x_diab_torn_syncmutex_o;
IMPORT int ___x_diab_torn_syncsem_o;
IMPORT int ___x_diab_va_arg_o;
IMPORT int ___x_diab_wr_pk_db_o;
IMPORT int ___x_diab_wr_pk_fl_o;
IMPORT int ___x_diab_wr_pk_ld_o;
IMPORT int ___x_diab_wr_sw_db_o;
IMPORT int ___x_diab_wr_sw_fl_o;
IMPORT int ___x_diab_wr_sw_ld_o;
IMPORT int ___x_gnu___dummy_o;
IMPORT int ___x_gnu___gcc_bcmp_o;
IMPORT int ___x_gnu__ashldi3_o;
IMPORT int ___x_gnu__ashrdi3_o;
IMPORT int ___x_gnu__bb_o;
IMPORT int ___x_gnu__clear_cache_o;
IMPORT int ___x_gnu__cmpdi2_o;
IMPORT int ___x_gnu__divdi3_o;
IMPORT int ___x_gnu__eh_o;
IMPORT int ___x_gnu__eprintf_o;
IMPORT int ___x_gnu__ffsdi2_o;
IMPORT int ___x_gnu__fixdfdi_o;
IMPORT int ___x_gnu__fixsfdi_o;
IMPORT int ___x_gnu__fixtfdi_o;
IMPORT int ___x_gnu__fixunsdfdi_o;
IMPORT int ___x_gnu__fixunsdfsi_o;
IMPORT int ___x_gnu__fixunssfdi_o;
IMPORT int ___x_gnu__fixunssfsi_o;
IMPORT int ___x_gnu__fixunstfdi_o;
IMPORT int ___x_gnu__fixunsxfdi_o;
IMPORT int ___x_gnu__fixunsxfsi_o;
IMPORT int ___x_gnu__fixxfdi_o;
IMPORT int ___x_gnu__floatdidf_o;
IMPORT int ___x_gnu__floatdisf_o;
IMPORT int ___x_gnu__floatditf_o;
IMPORT int ___x_gnu__floatdixf_o;
IMPORT int ___x_gnu__lshrdi3_o;
IMPORT int ___x_gnu__moddi3_o;
IMPORT int ___x_gnu__muldi3_o;
IMPORT int ___x_gnu__negdi2_o;
IMPORT int ___x_gnu__shtab_o;
IMPORT int ___x_gnu__trampoline_o;
IMPORT int ___x_gnu__ucmpdi2_o;
IMPORT int ___x_gnu__udiv_w_sdiv_o;
IMPORT int ___x_gnu__udivdi3_o;
IMPORT int ___x_gnu__udivmoddi4_o;
IMPORT int ___x_gnu__umoddi3_o;
IMPORT int ___x_gnu_cleanup_o;
IMPORT int ___x_gnu_cmathi_o;
IMPORT int ___x_gnu_cstdlibi_o;
IMPORT int ___x_gnu_cstrio_o;
IMPORT int ___x_gnu_cstrmain_o;
IMPORT int ___x_gnu_dp_bit_o;
IMPORT int ___x_gnu_exception_o;
IMPORT int ___x_gnu_filebuf_o;
IMPORT int ___x_gnu_filedoalloc_o;
IMPORT int ___x_gnu_fileops_o;
IMPORT int ___x_gnu_floatconv_o;
IMPORT int ___x_gnu_fp_bit_o;
IMPORT int ___x_gnu_frame_o;
IMPORT int ___x_gnu_genops_o;
IMPORT int ___x_gnu_ioassign_o;
IMPORT int ___x_gnu_iogetc_o;
IMPORT int ___x_gnu_iogetline_o;
IMPORT int ___x_gnu_iopadn_o;
IMPORT int ___x_gnu_ioputc_o;
IMPORT int ___x_gnu_ioseekoff_o;
IMPORT int ___x_gnu_ioseekpos_o;
IMPORT int ___x_gnu_iostream_o;
IMPORT int ___x_gnu_ioungetc_o;
IMPORT int ___x_gnu_iovfscanf_o;
IMPORT int ___x_gnu_isgetline_o;
IMPORT int ___x_gnu_isscan_o;
IMPORT int ___x_gnu_new_o;
IMPORT int ___x_gnu_opdel_o;
IMPORT int ___x_gnu_opdelnt_o;
IMPORT int ___x_gnu_opnew_o;
IMPORT int ___x_gnu_opnewnt_o;
IMPORT int ___x_gnu_opvdel_o;
IMPORT int ___x_gnu_opvdelnt_o;
IMPORT int ___x_gnu_opvnew_o;
IMPORT int ___x_gnu_opvnewnt_o;
IMPORT int ___x_gnu_outfloat_o;
IMPORT int ___x_gnu_peekc_o;
IMPORT int ___x_gnu_sbscan_o;
IMPORT int ___x_gnu_stdexcepti_o;
IMPORT int ___x_gnu_stdstrbufs_o;
IMPORT int ___x_gnu_stdstreams_o;
IMPORT int ___x_gnu_stlinst_o;
IMPORT int ___x_gnu_streambuf_o;
IMPORT int ___x_gnu_tinfo2_o;
IMPORT int ___x_gnu_tinfo_o;
IMPORT int ___x_gnu_vrsave_o;
IMPORT int __add64 ();
IMPORT int __adddf3 ();
IMPORT int __addsf3 ();
IMPORT int __apl__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCc ();
IMPORT int __apl__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int __apl__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0c ();
IMPORT int __as__10RBString_TR10RBString_T ();
IMPORT int __as__22_IO_istream_withassignR22_IO_istream_withassign ();
IMPORT int __as__22_IO_istream_withassignR7istream ();
IMPORT int __as__22_IO_ostream_withassignR22_IO_ostream_withassign ();
IMPORT int __as__22_IO_ostream_withassignR7ostream ();
IMPORT int __as__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCc ();
IMPORT int __as__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int __as__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0c ();
IMPORT int __ashldi3 ();
IMPORT int __ashrdi3 ();
IMPORT int __asr64 ();
IMPORT int __assert ();
IMPORT int __builtin_delete ();
IMPORT int __builtin_new ();
IMPORT int __builtin_vec_delete ();
IMPORT int __builtin_vec_new ();
IMPORT int __check_eh_spec ();
IMPORT int __clear_cache ();
IMPORT int __clocale;
IMPORT int __cmpdf2 ();
IMPORT int __cmpdi2 ();
IMPORT int __cmpsf2 ();
IMPORT int __common_intrinsicsInit ();
IMPORT int __common_long_jmp ();
IMPORT int __costate;
IMPORT int __cp_eh_info ();
IMPORT int __cp_exception_info ();
IMPORT int __cp_pop_exception ();
IMPORT int __cp_push_exception ();
IMPORT int __cplusCore_o;
IMPORT int __cplusDem_o;
IMPORT int __cplusLibgccObjs;
IMPORT int __cplusLoadObjFiles;
IMPORT int __cplusLoad_o;
IMPORT int __cplusObjFiles;
IMPORT int __cplusStlObjs;
IMPORT int __cplusStr_o;
IMPORT int __cplusStringIoObjs;
IMPORT int __cplusStringObjs;
IMPORT int __cplusUsr_o;
IMPORT int __cplusXtors_o;
IMPORT int __cplus_type_matcher ();
IMPORT int __ctype;
IMPORT int __daysSinceEpoch ();
IMPORT int __default_terminate ();
IMPORT int __default_unexpected__Fv ();
IMPORT int __deregister_frame ();
IMPORT int __deregister_frame_info ();
IMPORT int __diab_alloc_mutex ();
IMPORT int __diab_alloc_semaphore ();
IMPORT int __diab_atomic_enter ();
IMPORT int __diab_atomic_restore ();
IMPORT int __diab_free_mutex ();
IMPORT int __diab_free_semaphore ();
IMPORT int __diab_intrinsics_impfpInit ();
IMPORT int __diab_intrinsics_implInit ();
IMPORT int __diab_intrinsics_tornInit ();
IMPORT int __diab_lock_mutex ();
IMPORT int __diab_lock_semaphore ();
IMPORT int __diab_trylock_mutex ();
IMPORT int __diab_trylock_semaphore ();
IMPORT int __diab_unlock_mutex ();
IMPORT int __diab_unlock_semaphore ();
IMPORT int __div32 ();
IMPORT int __div64 ();
IMPORT int __divdf3 ();
IMPORT int __divdi3 ();
IMPORT int __divsf3 ();
IMPORT int __dl__FPvRC9nothrow_t ();
IMPORT int __dl__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepPv ();
IMPORT int __dtoll ();
IMPORT int __dtoull ();
IMPORT int __dummy ();
IMPORT int __dynamic_cast ();
IMPORT int __eabi ();
IMPORT int __eh_alloc ();
IMPORT int __eh_free ();
IMPORT int __eh_rtime_match ();
IMPORT int __empty ();
IMPORT int __eq64 ();
IMPORT int __eq__C10RBString_TR10RBString_T ();
IMPORT int __eq__C9type_infoRC9type_info ();
IMPORT int __eq__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_PCX01RCt12basic_string3ZX01ZX11ZX21_b ();
IMPORT int __eq__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21PCX01_b ();
IMPORT int __eq__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21T0_b ();
IMPORT int __eqdf2 ();
IMPORT int __eqsf2 ();
IMPORT int __errno ();
IMPORT int __exp10 ();
IMPORT int __extendsfdf2 ();
IMPORT int __ffsdi2 ();
IMPORT int __fixdfdi ();
IMPORT int __fixdfsi ();
IMPORT int __fixsfdi ();
IMPORT int __fixsfsi ();
IMPORT int __fixunsdfdi ();
IMPORT int __fixunsdfsi ();
IMPORT int __fixunssfdi ();
IMPORT int __fixunssfsi ();
IMPORT int __floatdidf ();
IMPORT int __floatdisf ();
IMPORT int __floatsidf ();
IMPORT int __floatsisf ();
IMPORT int __fpcmp_parts_d ();
IMPORT int __fpcmp_parts_f ();
IMPORT int __frame_state_for ();
IMPORT int __ftoll ();
IMPORT int __ftoull ();
IMPORT int __gcc_bcmp ();
IMPORT int __gcc_intrinsicsInit ();
IMPORT int __ge64 ();
IMPORT int __ge__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_PCX01RCt12basic_string3ZX01ZX11ZX21_b ();
IMPORT int __ge__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21PCX01_b ();
IMPORT int __ge__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21T0_b ();
IMPORT int __gedf2 ();
IMPORT int __gesf2 ();
IMPORT int __getDstInfo ();
IMPORT int __getTime ();
IMPORT int __getZoneInfo ();
IMPORT int __get_dynamic_handler_chain ();
IMPORT int __get_eh_context ();
IMPORT int __get_eh_info ();
IMPORT int __get_eh_table_language ();
IMPORT int __get_eh_table_version ();
IMPORT int __gt64 ();
IMPORT int __gt__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_PCX01RCt12basic_string3ZX01ZX11ZX21_b ();
IMPORT int __gt__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21PCX01_b ();
IMPORT int __gt__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21T0_b ();
IMPORT int __gtdf2 ();
IMPORT int __gtsf2 ();
IMPORT int __includeDiabIntrinsics;
IMPORT int __includeGnuIntrinsics;
IMPORT int __is_pointer__FPv ();
IMPORT int __julday ();
IMPORT int __le64 ();
IMPORT int __le__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_PCX01RCt12basic_string3ZX01ZX11ZX21_b ();
IMPORT int __le__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21PCX01_b ();
IMPORT int __le__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21T0_b ();
IMPORT int __ledf2 ();
IMPORT int __length_error__FPCc ();
IMPORT int __lesf2 ();
IMPORT int __lltod ();
IMPORT int __lltof ();
IMPORT int __locale;
IMPORT int __loctime;
IMPORT int __ls__7ostreamP9streambuf ();
IMPORT int __ls__7ostreamPCSc ();
IMPORT int __ls__7ostreamPCUc ();
IMPORT int __ls__7ostreamPCc ();
IMPORT int __ls__7ostreamPFR3ios_R3ios ();
IMPORT int __ls__7ostreamPFR7ostream_R7ostream ();
IMPORT int __ls__7ostreamSc ();
IMPORT int __ls__7ostreamUc ();
IMPORT int __ls__7ostreamUi ();
IMPORT int __ls__7ostreamUl ();
IMPORT int __ls__7ostreamUs ();
IMPORT int __ls__7ostreamUx ();
IMPORT int __ls__7ostreamb ();
IMPORT int __ls__7ostreamc ();
IMPORT int __ls__7ostreamd ();
IMPORT int __ls__7ostreamf ();
IMPORT int __ls__7ostreami ();
IMPORT int __ls__7ostreaml ();
IMPORT int __ls__7ostreamr ();
IMPORT int __ls__7ostreams ();
IMPORT int __ls__7ostreamx ();
IMPORT int __ls__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_R7ostreamRCt12basic_string3ZX01ZX11ZX21_R7ostream ();
IMPORT int __lshrdi3 ();
IMPORT int __lsl64 ();
IMPORT int __lsr64 ();
IMPORT int __lt64 ();
IMPORT int __lt__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_PCX01RCt12basic_string3ZX01ZX11ZX21_b ();
IMPORT int __lt__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21PCX01_b ();
IMPORT int __lt__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21T0_b ();
IMPORT int __ltdf2 ();
IMPORT int __ltsf2 ();
IMPORT int __make_dp ();
IMPORT int __make_fp ();
IMPORT int __moddi3 ();
IMPORT int __mul64 ();
IMPORT int __muldf3 ();
IMPORT int __muldi3 ();
IMPORT int __mulsf3 ();
IMPORT int __ne64 ();
IMPORT int __ne__C9type_infoRC9type_info ();
IMPORT int __ne__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_PCX01RCt12basic_string3ZX01ZX11ZX21_b ();
IMPORT int __ne__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21PCX01_b ();
IMPORT int __ne__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21T0_b ();
IMPORT int __nedf2 ();
IMPORT int __negdf2 ();
IMPORT int __negdi2 ();
IMPORT int __negsf2 ();
IMPORT int __nesf2 ();
IMPORT int __new_handler;
IMPORT int __nt__C3ios ();
IMPORT int __nw__FUiPv ();
IMPORT int __nw__FUiRC9nothrow_t ();
IMPORT int __nw__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepUiUi ();
IMPORT int __opPv__C3ios ();
IMPORT int __out_of_range__FPCc ();
IMPORT int __overflow ();
IMPORT int __overflow__FP9streambufi ();
IMPORT int __pack_d ();
IMPORT int __pack_f ();
IMPORT int __pl__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_PCX01RCt12basic_string3ZX01ZX11ZX21_t12basic_string3ZX01ZX11ZX21 ();
IMPORT int __pl__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21PCX01_t12basic_string3ZX01ZX11ZX21 ();
IMPORT int __pl__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21T0_t12basic_string3ZX01ZX11ZX21 ();
IMPORT int __pl__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21X01_t12basic_string3ZX01ZX11ZX21 ();
IMPORT int __pl__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_X01RCt12basic_string3ZX01ZX11ZX21_t12basic_string3ZX01ZX11ZX21 ();
IMPORT int __pure_virtual ();
IMPORT int __pure_virtual_called ();
IMPORT int __register_frame ();
IMPORT int __register_frame_info ();
IMPORT int __register_frame_info_table ();
IMPORT int __register_frame_table ();
IMPORT int __rem32 ();
IMPORT int __rem64 ();
IMPORT int __rethrow ();
IMPORT int __rs__7istreamP9streambuf ();
IMPORT int __rs__7istreamPFR3ios_R3ios ();
IMPORT int __rs__7istreamPFR7istream_R7istream ();
IMPORT int __rs__7istreamPSc ();
IMPORT int __rs__7istreamPUc ();
IMPORT int __rs__7istreamPc ();
IMPORT int __rs__7istreamRSc ();
IMPORT int __rs__7istreamRUc ();
IMPORT int __rs__7istreamRUi ();
IMPORT int __rs__7istreamRUl ();
IMPORT int __rs__7istreamRUs ();
IMPORT int __rs__7istreamRUx ();
IMPORT int __rs__7istreamRb ();
IMPORT int __rs__7istreamRc ();
IMPORT int __rs__7istreamRd ();
IMPORT int __rs__7istreamRf ();
IMPORT int __rs__7istreamRi ();
IMPORT int __rs__7istreamRl ();
IMPORT int __rs__7istreamRr ();
IMPORT int __rs__7istreamRs ();
IMPORT int __rs__7istreamRx ();
IMPORT int __rs__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_R7istreamRt12basic_string3ZX01ZX11ZX21_R7istream ();
IMPORT int __rta_longjmp;
IMPORT int __rtc_alloc_ptr;
IMPORT int __rtc_blkinfo_ptr;
IMPORT int __rtc_free_ptr;
IMPORT int __rtti_array ();
IMPORT int __rtti_attr ();
IMPORT int __rtti_class ();
IMPORT int __rtti_func ();
IMPORT int __rtti_ptmd ();
IMPORT int __rtti_ptmf ();
IMPORT int __rtti_ptr ();
IMPORT int __rtti_si ();
IMPORT int __rtti_user ();
IMPORT int __saverest_o;
IMPORT int __sclose ();
IMPORT int __set_malloc_handler__t23__malloc_alloc_template1i0PFv_v ();
IMPORT int __sflags ();
IMPORT int __sflush ();
IMPORT int __sfvwrite ();
IMPORT int __shtab;
IMPORT int __sjpopnthrow ();
IMPORT int __sjthrow ();
IMPORT int __smakebuf ();
IMPORT int __sread ();
IMPORT int __srefill ();
IMPORT int __srget ();
IMPORT int __sseek ();
IMPORT int __start_cp_handler ();
IMPORT int __stderr ();
IMPORT int __stdin ();
IMPORT int __stdout ();
IMPORT int __strxfrm ();
IMPORT int __sub64 ();
IMPORT int __subdf3 ();
IMPORT int __subsf3 ();
IMPORT int __swbuf ();
IMPORT int __swrite ();
IMPORT int __swsetup ();
IMPORT int __t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int __t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCc ();
IMPORT int __t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi ();
IMPORT int __t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int __t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUi ();
IMPORT int __t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Uic ();
IMPORT int __terminate ();
IMPORT int __terminate_func;
IMPORT int __tf10bad_typeid ();
IMPORT int __tf11logic_error ();
IMPORT int __tf11range_error ();
IMPORT int __tf12domain_error ();
IMPORT int __tf12length_error ();
IMPORT int __tf12out_of_range ();
IMPORT int __tf13bad_exception ();
IMPORT int __tf13runtime_error ();
IMPORT int __tf14__si_type_info ();
IMPORT int __tf14overflow_error ();
IMPORT int __tf15underflow_error ();
IMPORT int __tf16__user_type_info ();
IMPORT int __tf16invalid_argument ();
IMPORT int __tf17__class_type_info ();
IMPORT int __tf22_IO_istream_withassign ();
IMPORT int __tf22_IO_ostream_withassign ();
IMPORT int __tf3ios ();
IMPORT int __tf7filebuf ();
IMPORT int __tf7istream ();
IMPORT int __tf7ostream ();
IMPORT int __tf8bad_cast ();
IMPORT int __tf8iostream ();
IMPORT int __tf9bad_alloc ();
IMPORT int __tf9exception ();
IMPORT int __tf9streambuf ();
IMPORT int __tf9type_info ();
IMPORT int __tfSc ();
IMPORT int __tfUc ();
IMPORT int __tfUi ();
IMPORT int __tfUl ();
IMPORT int __tfUs ();
IMPORT int __tfUx ();
IMPORT int __tfb ();
IMPORT int __tfc ();
IMPORT int __tfd ();
IMPORT int __tff ();
IMPORT int __tfi ();
IMPORT int __tfl ();
IMPORT int __tfr ();
IMPORT int __tfs ();
IMPORT int __tfv ();
IMPORT int __tfw ();
IMPORT int __tfx ();
IMPORT int __throw ();
IMPORT int __throw_bad_cast ();
IMPORT int __throw_bad_typeid ();
IMPORT int __throw_type_match ();
IMPORT int __throw_type_match_rtti ();
IMPORT int __ti10bad_typeid;
IMPORT int __ti11_ios_fields;
IMPORT int __ti11logic_error;
IMPORT int __ti11range_error;
IMPORT int __ti12domain_error;
IMPORT int __ti12length_error;
IMPORT int __ti12out_of_range;
IMPORT int __ti13bad_exception;
IMPORT int __ti13runtime_error;
IMPORT int __ti14__si_type_info;
IMPORT int __ti14overflow_error;
IMPORT int __ti15underflow_error;
IMPORT int __ti16__attr_type_info;
IMPORT int __ti16__func_type_info;
IMPORT int __ti16__ptmd_type_info;
IMPORT int __ti16__ptmf_type_info;
IMPORT int __ti16__user_type_info;
IMPORT int __ti16invalid_argument;
IMPORT int __ti17__array_type_info;
IMPORT int __ti17__class_type_info;
IMPORT int __ti19__builtin_type_info;
IMPORT int __ti19__pointer_type_info;
IMPORT int __ti22_IO_istream_withassign;
IMPORT int __ti22_IO_ostream_withassign;
IMPORT int __ti3ios;
IMPORT int __ti7filebuf;
IMPORT int __ti7istream;
IMPORT int __ti7ostream;
IMPORT int __ti8_IO_FILE;
IMPORT int __ti8bad_cast;
IMPORT int __ti8iostream;
IMPORT int __ti9bad_alloc;
IMPORT int __ti9exception;
IMPORT int __ti9streambuf;
IMPORT int __ti9type_info;
IMPORT int __tiSc;
IMPORT int __tiUc;
IMPORT int __tiUi;
IMPORT int __tiUl;
IMPORT int __tiUs;
IMPORT int __tiUx;
IMPORT int __tib;
IMPORT int __tic;
IMPORT int __tid;
IMPORT int __tif;
IMPORT int __tii;
IMPORT int __til;
IMPORT int __tir;
IMPORT int __tis;
IMPORT int __tiv;
IMPORT int __tiw;
IMPORT int __tix;
IMPORT int __truncdfsf2 ();
IMPORT int __ucmpdi2 ();
IMPORT int __udiv32 ();
IMPORT int __udiv64 ();
IMPORT int __udiv_w_sdiv ();
IMPORT int __udivdi3 ();
IMPORT int __udivmoddi4 ();
IMPORT int __uflow ();
IMPORT int __uge64 ();
IMPORT int __ugt64 ();
IMPORT int __ule64 ();
IMPORT int __ulltod ();
IMPORT int __ulltof ();
IMPORT int __ult64 ();
IMPORT int __umoddi3 ();
IMPORT int __uncatch_exception ();
IMPORT int __underflow ();
IMPORT int __unpack_d ();
IMPORT int __unpack_f ();
IMPORT int __unwinding_cleanup ();
IMPORT int __urem32 ();
IMPORT int __urem64 ();
IMPORT int __va_arg ();
IMPORT int __vc__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui ();
IMPORT int __vc__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepUi ();
IMPORT int __vc__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui ();
IMPORT int __vd__FPvRC9nothrow_t ();
IMPORT int __vn__FUiPv ();
IMPORT int __vn__FUiRC9nothrow_t ();
IMPORT int __wdbEventListIsEmpty;
IMPORT int __wdbEvtptDeleteAll;
IMPORT int _archHelp_msg;
IMPORT int _arpCmd ();
IMPORT int _authenticate ();
IMPORT int _clockRealtime;
IMPORT int _cplusDemangle ();
IMPORT int _d_dtoll ();
IMPORT int _d_dtoull ();
IMPORT int _d_lltod ();
IMPORT int _d_ulltod ();
IMPORT int _db_show_arptab ();
IMPORT int _dbgArchInit ();
IMPORT int _dbgBrkDisplayHard ();
IMPORT int _dbgDsmInstRtn;
IMPORT int _dbgFuncCallCheck ();
IMPORT int _dbgInfoPCGet ();
IMPORT int _dbgInstSizeGet ();
IMPORT int _dbgRetAdrsGet ();
IMPORT int _dbgTaskPCGet ();
IMPORT int _dbgTaskPCSet ();
IMPORT int _f_ftoll ();
IMPORT int _f_ftoull ();
IMPORT int _f_lltof ();
IMPORT int _f_ulltof ();
IMPORT int _find__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCccUiUi ();
IMPORT int _frcPci0ReadConfigReg ();
IMPORT int _frcPci0WriteConfigReg ();
IMPORT int _func_altivecProbe;
IMPORT int _func_altivecProbeRtn;
IMPORT int _func_altivecTaskRegsGet;
IMPORT int _func_altivecTaskRegsShow;
IMPORT int _func_bdall;
IMPORT int _func_breakpoint;
IMPORT int _func_dbgHostNotify;
IMPORT int _func_dbgTargetNotify;
IMPORT int _func_dspMregsHook;
IMPORT int _func_dspRegsListHook;
IMPORT int _func_dspTaskRegsShow;
IMPORT int _func_evtLogM0;
IMPORT int _func_evtLogM1;
IMPORT int _func_evtLogM2;
IMPORT int _func_evtLogM3;
IMPORT int _func_evtLogO;
IMPORT int _func_evtLogOIntLock;
IMPORT int _func_evtLogPoint;
IMPORT int _func_evtLogReserveTaskName;
IMPORT int _func_evtLogString;
IMPORT int _func_evtLogT0;
IMPORT int _func_evtLogT0_noInt;
IMPORT int _func_evtLogT1;
IMPORT int _func_evtLogT1_noTS;
IMPORT int _func_evtLogTSched;
IMPORT int _func_excBaseHook;
IMPORT int _func_excInfoShow;
IMPORT int _func_excIntHook;
IMPORT int _func_excJobAdd;
IMPORT int _func_excPanicHook;
IMPORT int _func_excTrapRtn;
IMPORT int _func_fastUdpErrorNotify;
IMPORT int _func_fastUdpInput;
IMPORT int _func_fastUdpPortTest;
IMPORT int _func_fclose;
IMPORT int _func_fileDoesNotExist;
IMPORT int _func_fppTaskRegsShow;
IMPORT int _func_ftpLs;
IMPORT int _func_ftpTransientFatal;
IMPORT int _func_intConnectRtn;
IMPORT int _func_intDisableRtn;
IMPORT int _func_intEnableRtn;
IMPORT int _func_intLevelSetRtn;
IMPORT int _func_intVecBaseGetRtn;
IMPORT int _func_intVecBaseSetRtn;
IMPORT int _func_intVecGetRtn;
IMPORT int _func_intVecSetRtn;
IMPORT int _func_ioTaskStdSet;
IMPORT int _func_ipsecFilterHook;
IMPORT int _func_ipsecInput;
IMPORT int _func_ipsecOutput;
IMPORT int _func_logMsg;
IMPORT int _func_memalign;
IMPORT int _func_netLsByName;
IMPORT int _func_printErr;
IMPORT int _func_pthread_setcanceltype;
IMPORT int _func_remCurIdGet;
IMPORT int _func_remCurIdSet;
IMPORT int _func_selPtyAdd;
IMPORT int _func_selPtyDelete;
IMPORT int _func_selTyAdd;
IMPORT int _func_selTyDelete;
IMPORT int _func_selWakeupAll;
IMPORT int _func_selWakeupListInit;
IMPORT int _func_selWakeupListTerm;
IMPORT int _func_sigExcKill;
IMPORT int _func_sigTimeoutRecalc;
IMPORT int _func_sigprocmask;
IMPORT int _func_smObjObjShow;
IMPORT int _func_spy;
IMPORT int _func_spyClkStart;
IMPORT int _func_spyClkStop;
IMPORT int _func_spyReport;
IMPORT int _func_spyStop;
IMPORT int _func_spyTask;
IMPORT int _func_sseTaskRegsShow;
IMPORT int _func_symFindByValue;
IMPORT int _func_symFindByValueAndType;
IMPORT int _func_symFindSymbol;
IMPORT int _func_symNameGet;
IMPORT int _func_symTypeGet;
IMPORT int _func_symValueGet;
IMPORT int _func_taskCreateHookAdd;
IMPORT int _func_taskDeleteHookAdd;
IMPORT int _func_taskRegsShowRtn;
IMPORT int _func_tmrConnect;
IMPORT int _func_tmrDisable;
IMPORT int _func_tmrEnable;
IMPORT int _func_tmrFreq;
IMPORT int _func_tmrPeriod;
IMPORT int _func_tmrStamp;
IMPORT int _func_tmrStampLock;
IMPORT int _func_trace;
IMPORT int _func_trgCheck;
IMPORT int _func_valloc;
IMPORT int _func_vxMemProbeHook;
IMPORT int _func_wdbIsNowExternal;
IMPORT int _func_wvNetAddressFilterTest;
IMPORT int _func_wvNetPortFilterTest;
IMPORT int _icmpErrorHook;
IMPORT int _igmpJoinAlertHook;
IMPORT int _igmpJoinGrpHook;
IMPORT int _igmpLeaveAlertHook;
IMPORT int _igmpLeaveGrpHook;
IMPORT int _igmpMessageHook;
IMPORT int _igmpQuerierTimeUpdateHook;
IMPORT int _initialize_dbg_uart ();
IMPORT int _insque ();
IMPORT int _ipCfgFlags;
IMPORT int _ipFilterHook;
IMPORT int _itof ();
IMPORT int _landAttackSafe;
IMPORT int _link_in__9streambuf ();
IMPORT int _m2IfTableUpdate;
IMPORT int _m2SetIfLastChange;
IMPORT int _mCastRouteCmdHook;
IMPORT int _mCastRouteFwdHook;
IMPORT int _netFree ();
IMPORT int _netMalloc ();
IMPORT int _null_auth;
IMPORT int _pNetBufCollect;
IMPORT int _pNetDpool;
IMPORT int _pNetPoolFuncTbl;
IMPORT int _pNetSysPool;
IMPORT int _pSigQueueFreeHead;
IMPORT int _pSysBatInitFunc;
IMPORT int _pSysL2CacheDisable;
IMPORT int _pSysL2CacheEnable;
IMPORT int _pSysL2CacheFlush;
IMPORT int _pSysL2CacheInvFunc;
IMPORT int _pSysL3CacheFlushDisableFunc;
IMPORT int _pSysL3CacheInvalEnableFunc;
IMPORT int _pTcpPcbHead;
IMPORT int _pTcpPcbPrint;
IMPORT int _pUdpPcbHead;
IMPORT int _panicHook;
IMPORT int _pingTxInterval;
IMPORT int _pingTxLen;
IMPORT int _pingTxTmo;
IMPORT int _presolvHostLibGetByAddr;
IMPORT int _presolvHostLibGetByName;
IMPORT int _procNumWasSet;
IMPORT int _protoSwIndex;
IMPORT int _remque ();
IMPORT int _restf14 ();
IMPORT int _restf15 ();
IMPORT int _restf16 ();
IMPORT int _restf17 ();
IMPORT int _restf18 ();
IMPORT int _restf19 ();
IMPORT int _restf20 ();
IMPORT int _restf21 ();
IMPORT int _restf22 ();
IMPORT int _restf23 ();
IMPORT int _restf24 ();
IMPORT int _restf25 ();
IMPORT int _restf26 ();
IMPORT int _restf27 ();
IMPORT int _restf28 ();
IMPORT int _restf29 ();
IMPORT int _restf30 ();
IMPORT int _restf31 ();
IMPORT int _restfpr_14_l ();
IMPORT int _restfpr_15_l ();
IMPORT int _restfpr_16_l ();
IMPORT int _restfpr_17_l ();
IMPORT int _restfpr_18_l ();
IMPORT int _restfpr_19_l ();
IMPORT int _restfpr_20_l ();
IMPORT int _restfpr_21_l ();
IMPORT int _restfpr_22_l ();
IMPORT int _restfpr_23_l ();
IMPORT int _restfpr_24_l ();
IMPORT int _restfpr_25_l ();
IMPORT int _restfpr_26_l ();
IMPORT int _restfpr_27_l ();
IMPORT int _restfpr_28_l ();
IMPORT int _restfpr_29_l ();
IMPORT int _restfpr_30_l ();
IMPORT int _restfpr_31_l ();
IMPORT int _restfprs_14_l ();
IMPORT int _restfprs_15_l ();
IMPORT int _restfprs_16_l ();
IMPORT int _restfprs_17_l ();
IMPORT int _restfprs_18_l ();
IMPORT int _restfprs_19_l ();
IMPORT int _restfprs_20_l ();
IMPORT int _restfprs_21_l ();
IMPORT int _restfprs_22_l ();
IMPORT int _restfprs_23_l ();
IMPORT int _restfprs_24_l ();
IMPORT int _restfprs_25_l ();
IMPORT int _restfprs_26_l ();
IMPORT int _restfprs_27_l ();
IMPORT int _restfprs_28_l ();
IMPORT int _restfprs_29_l ();
IMPORT int _restfprs_30_l ();
IMPORT int _restfprs_31_l ();
IMPORT int _restfprx_14_l ();
IMPORT int _restfprx_15_l ();
IMPORT int _restfprx_16_l ();
IMPORT int _restfprx_17_l ();
IMPORT int _restfprx_18_l ();
IMPORT int _restfprx_19_l ();
IMPORT int _restfprx_20_l ();
IMPORT int _restfprx_21_l ();
IMPORT int _restfprx_22_l ();
IMPORT int _restfprx_23_l ();
IMPORT int _restfprx_24_l ();
IMPORT int _restfprx_25_l ();
IMPORT int _restfprx_26_l ();
IMPORT int _restfprx_27_l ();
IMPORT int _restfprx_28_l ();
IMPORT int _restfprx_29_l ();
IMPORT int _restfprx_30_l ();
IMPORT int _restfprx_31_l ();
IMPORT int _restgpr_14 ();
IMPORT int _restgpr_14_l ();
IMPORT int _restgpr_15 ();
IMPORT int _restgpr_15_l ();
IMPORT int _restgpr_16 ();
IMPORT int _restgpr_16_l ();
IMPORT int _restgpr_17 ();
IMPORT int _restgpr_17_l ();
IMPORT int _restgpr_18 ();
IMPORT int _restgpr_18_l ();
IMPORT int _restgpr_19 ();
IMPORT int _restgpr_19_l ();
IMPORT int _restgpr_20 ();
IMPORT int _restgpr_20_l ();
IMPORT int _restgpr_21 ();
IMPORT int _restgpr_21_l ();
IMPORT int _restgpr_22 ();
IMPORT int _restgpr_22_l ();
IMPORT int _restgpr_23 ();
IMPORT int _restgpr_23_l ();
IMPORT int _restgpr_24 ();
IMPORT int _restgpr_24_l ();
IMPORT int _restgpr_25 ();
IMPORT int _restgpr_25_l ();
IMPORT int _restgpr_26 ();
IMPORT int _restgpr_26_l ();
IMPORT int _restgpr_27 ();
IMPORT int _restgpr_27_l ();
IMPORT int _restgpr_28 ();
IMPORT int _restgpr_28_l ();
IMPORT int _restgpr_29 ();
IMPORT int _restgpr_29_l ();
IMPORT int _restgpr_30 ();
IMPORT int _restgpr_30_l ();
IMPORT int _restgpr_31 ();
IMPORT int _restgpr_31_l ();
IMPORT int _restgprx_14_l ();
IMPORT int _restgprx_15_l ();
IMPORT int _restgprx_16_l ();
IMPORT int _restgprx_17_l ();
IMPORT int _restgprx_18_l ();
IMPORT int _restgprx_19_l ();
IMPORT int _restgprx_20_l ();
IMPORT int _restgprx_21_l ();
IMPORT int _restgprx_22_l ();
IMPORT int _restgprx_23_l ();
IMPORT int _restgprx_24_l ();
IMPORT int _restgprx_25_l ();
IMPORT int _restgprx_26_l ();
IMPORT int _restgprx_27_l ();
IMPORT int _restgprx_28_l ();
IMPORT int _restgprx_29_l ();
IMPORT int _restgprx_30_l ();
IMPORT int _restgprx_31_l ();
IMPORT int _restv20 ();
IMPORT int _restv21 ();
IMPORT int _restv22 ();
IMPORT int _restv23 ();
IMPORT int _restv24 ();
IMPORT int _restv25 ();
IMPORT int _restv26 ();
IMPORT int _restv27 ();
IMPORT int _restv28 ();
IMPORT int _restv29 ();
IMPORT int _restv30 ();
IMPORT int _restv31 ();
IMPORT int _restvr20 ();
IMPORT int _restvr21 ();
IMPORT int _restvr22 ();
IMPORT int _restvr23 ();
IMPORT int _restvr24 ();
IMPORT int _restvr25 ();
IMPORT int _restvr26 ();
IMPORT int _restvr27 ();
IMPORT int _restvr28 ();
IMPORT int _restvr29 ();
IMPORT int _restvr30 ();
IMPORT int _restvr31 ();
IMPORT int _savef14 ();
IMPORT int _savef15 ();
IMPORT int _savef16 ();
IMPORT int _savef17 ();
IMPORT int _savef18 ();
IMPORT int _savef19 ();
IMPORT int _savef20 ();
IMPORT int _savef21 ();
IMPORT int _savef22 ();
IMPORT int _savef23 ();
IMPORT int _savef24 ();
IMPORT int _savef25 ();
IMPORT int _savef26 ();
IMPORT int _savef27 ();
IMPORT int _savef28 ();
IMPORT int _savef29 ();
IMPORT int _savef30 ();
IMPORT int _savef31 ();
IMPORT int _savefpr_14_l ();
IMPORT int _savefpr_15_l ();
IMPORT int _savefpr_16_l ();
IMPORT int _savefpr_17_l ();
IMPORT int _savefpr_18_l ();
IMPORT int _savefpr_19_l ();
IMPORT int _savefpr_20_l ();
IMPORT int _savefpr_21_l ();
IMPORT int _savefpr_22_l ();
IMPORT int _savefpr_23_l ();
IMPORT int _savefpr_24_l ();
IMPORT int _savefpr_25_l ();
IMPORT int _savefpr_26_l ();
IMPORT int _savefpr_27_l ();
IMPORT int _savefpr_28_l ();
IMPORT int _savefpr_29_l ();
IMPORT int _savefpr_30_l ();
IMPORT int _savefpr_31_l ();
IMPORT int _savefprs_14_l ();
IMPORT int _savefprs_15_l ();
IMPORT int _savefprs_16_l ();
IMPORT int _savefprs_17_l ();
IMPORT int _savefprs_18_l ();
IMPORT int _savefprs_19_l ();
IMPORT int _savefprs_20_l ();
IMPORT int _savefprs_21_l ();
IMPORT int _savefprs_22_l ();
IMPORT int _savefprs_23_l ();
IMPORT int _savefprs_24_l ();
IMPORT int _savefprs_25_l ();
IMPORT int _savefprs_26_l ();
IMPORT int _savefprs_27_l ();
IMPORT int _savefprs_28_l ();
IMPORT int _savefprs_29_l ();
IMPORT int _savefprs_30_l ();
IMPORT int _savefprs_31_l ();
IMPORT int _savefprx_14_l ();
IMPORT int _savefprx_15_l ();
IMPORT int _savefprx_16_l ();
IMPORT int _savefprx_17_l ();
IMPORT int _savefprx_18_l ();
IMPORT int _savefprx_19_l ();
IMPORT int _savefprx_20_l ();
IMPORT int _savefprx_21_l ();
IMPORT int _savefprx_22_l ();
IMPORT int _savefprx_23_l ();
IMPORT int _savefprx_24_l ();
IMPORT int _savefprx_25_l ();
IMPORT int _savefprx_26_l ();
IMPORT int _savefprx_27_l ();
IMPORT int _savefprx_28_l ();
IMPORT int _savefprx_29_l ();
IMPORT int _savefprx_30_l ();
IMPORT int _savefprx_31_l ();
IMPORT int _savegpr_14 ();
IMPORT int _savegpr_14_l ();
IMPORT int _savegpr_15 ();
IMPORT int _savegpr_15_l ();
IMPORT int _savegpr_16 ();
IMPORT int _savegpr_16_l ();
IMPORT int _savegpr_17 ();
IMPORT int _savegpr_17_l ();
IMPORT int _savegpr_18 ();
IMPORT int _savegpr_18_l ();
IMPORT int _savegpr_19 ();
IMPORT int _savegpr_19_l ();
IMPORT int _savegpr_20 ();
IMPORT int _savegpr_20_l ();
IMPORT int _savegpr_21 ();
IMPORT int _savegpr_21_l ();
IMPORT int _savegpr_22 ();
IMPORT int _savegpr_22_l ();
IMPORT int _savegpr_23 ();
IMPORT int _savegpr_23_l ();
IMPORT int _savegpr_24 ();
IMPORT int _savegpr_24_l ();
IMPORT int _savegpr_25 ();
IMPORT int _savegpr_25_l ();
IMPORT int _savegpr_26 ();
IMPORT int _savegpr_26_l ();
IMPORT int _savegpr_27 ();
IMPORT int _savegpr_27_l ();
IMPORT int _savegpr_28 ();
IMPORT int _savegpr_28_l ();
IMPORT int _savegpr_29 ();
IMPORT int _savegpr_29_l ();
IMPORT int _savegpr_30 ();
IMPORT int _savegpr_30_l ();
IMPORT int _savegpr_31 ();
IMPORT int _savegpr_31_l ();
IMPORT int _savegprx_14_l ();
IMPORT int _savegprx_15_l ();
IMPORT int _savegprx_16_l ();
IMPORT int _savegprx_17_l ();
IMPORT int _savegprx_18_l ();
IMPORT int _savegprx_19_l ();
IMPORT int _savegprx_20_l ();
IMPORT int _savegprx_21_l ();
IMPORT int _savegprx_22_l ();
IMPORT int _savegprx_23_l ();
IMPORT int _savegprx_24_l ();
IMPORT int _savegprx_25_l ();
IMPORT int _savegprx_26_l ();
IMPORT int _savegprx_27_l ();
IMPORT int _savegprx_28_l ();
IMPORT int _savegprx_29_l ();
IMPORT int _savegprx_30_l ();
IMPORT int _savegprx_31_l ();
IMPORT int _savev20 ();
IMPORT int _savev21 ();
IMPORT int _savev22 ();
IMPORT int _savev23 ();
IMPORT int _savev24 ();
IMPORT int _savev25 ();
IMPORT int _savev26 ();
IMPORT int _savev27 ();
IMPORT int _savev28 ();
IMPORT int _savev29 ();
IMPORT int _savev30 ();
IMPORT int _savev31 ();
IMPORT int _savevr20 ();
IMPORT int _savevr21 ();
IMPORT int _savevr22 ();
IMPORT int _savevr23 ();
IMPORT int _savevr24 ();
IMPORT int _savevr25 ();
IMPORT int _savevr26 ();
IMPORT int _savevr27 ();
IMPORT int _savevr28 ();
IMPORT int _savevr29 ();
IMPORT int _savevr30 ();
IMPORT int _savevr31 ();
IMPORT int _sb_readline__FP9streambufRlc ();
IMPORT int _sch_istable;
IMPORT int _sch_tolower;
IMPORT int _sch_toupper;
IMPORT int _seterr_reply ();
IMPORT int _setjmpSetup ();
IMPORT int _sigCtxLoad ();
IMPORT int _sigCtxRtnValSet ();
IMPORT int _sigCtxSave ();
IMPORT int _sigCtxSetup ();
IMPORT int _sigCtxStackEnd ();
IMPORT int _sigfaulttable;
IMPORT int _skip_ws__7istream ();
IMPORT int _svcauth_null ();
IMPORT int _svcauth_short ();
IMPORT int _svcauth_unix ();
IMPORT int _sysInit ();
IMPORT int _t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0$nilRep;
IMPORT int _t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0$npos;
IMPORT int _t23__malloc_alloc_template1i0$__malloc_alloc_oom_handler;
IMPORT int _t24__default_alloc_template2b1i0$_S_end_free;
IMPORT int _t24__default_alloc_template2b1i0$_S_free_list;
IMPORT int _t24__default_alloc_template2b1i0$_S_heap_size;
IMPORT int _t24__default_alloc_template2b1i0$_S_node_allocator_lock;
IMPORT int _t24__default_alloc_template2b1i0$_S_start_free;
IMPORT int _throw_failure__C3ios ();
IMPORT int _un_link__9streambuf ();
IMPORT int _utof ();
IMPORT int _vt$10bad_typeid;
IMPORT int _vt$11logic_error;
IMPORT int _vt$11range_error;
IMPORT int _vt$12domain_error;
IMPORT int _vt$12length_error;
IMPORT int _vt$12out_of_range;
IMPORT int _vt$13bad_exception;
IMPORT int _vt$13runtime_error;
IMPORT int _vt$14__si_type_info;
IMPORT int _vt$14overflow_error;
IMPORT int _vt$15underflow_error;
IMPORT int _vt$16__user_type_info;
IMPORT int _vt$16invalid_argument;
IMPORT int _vt$17__class_type_info;
IMPORT int _vt$22_IO_istream_withassign$3ios;
IMPORT int _vt$22_IO_ostream_withassign$3ios;
IMPORT int _vt$3ios;
IMPORT int _vt$7filebuf;
IMPORT int _vt$7istream$3ios;
IMPORT int _vt$7ostream$3ios;
IMPORT int _vt$8bad_cast;
IMPORT int _vt$8iostream$3ios;
IMPORT int _vt$9bad_alloc;
IMPORT int _vt$9exception;
IMPORT int _vt$9streambuf;
IMPORT int _vt$9type_info;
IMPORT int _vtable__9streambuf ();
IMPORT int _wdbTaskBpAdd;
IMPORT int _wdbTaskBpBreakpoint;
IMPORT int _wdbTaskBpTrace;
IMPORT int _wdbTaskCont;
IMPORT int _wdbTaskStep;
IMPORT int abort ();
IMPORT int abs ();
IMPORT int abs__Fd ();
IMPORT int abs__Ff ();
IMPORT int abs__Fl ();
IMPORT int abs__Fr ();
IMPORT int accept ();
IMPORT int accepted ();
IMPORT int acos ();
IMPORT int activeQHead;
IMPORT int addCacheControlBits ();
IMPORT int addDomain ();
IMPORT int addSegNames ();
IMPORT int alloc__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Uib ();
IMPORT int allocate__9streambuf ();
IMPORT int allocate__t23__malloc_alloc_template1i0Ui ();
IMPORT int allocate__t24__default_alloc_template2b1i0Ui ();
IMPORT int allocbuf__9streambuf ();
IMPORT int altivecArchInit ();
IMPORT int altivecArchTaskCreateInit ();
IMPORT int altivecCreateHookRtn;
IMPORT int altivecCtxToRegs ();
IMPORT int altivecDisplayHookRtn;
IMPORT int altivecInit ();
IMPORT int altivecProbe ();
IMPORT int altivecRegsToCtx ();
IMPORT int altivecRestore ();
IMPORT int altivecSave ();
IMPORT int altivecShowInit ();
IMPORT int altivecTaskRegsGet ();
IMPORT int altivecTaskRegsSet ();
IMPORT int altivecTaskRegsShow ();
IMPORT int am29BotOffset;
IMPORT int am29TopOffset;
IMPORT int append__10RBString_TPCci ();
IMPORT int append__10RBString_TR10RBString_T ();
IMPORT int append__10RBString_Tc ();
IMPORT int append__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCc ();
IMPORT int append__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi ();
IMPORT int append__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUi ();
IMPORT int append__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Uic ();
IMPORT int arpEntryDelete ();
IMPORT int arpMaxEntries;
IMPORT int arpRxmitTicks;
IMPORT int arpShow ();
IMPORT int arp_allocated;
IMPORT int arp_intimer;
IMPORT int arp_inuse;
IMPORT int arp_maxtries;
IMPORT int arp_rtrequest ();
IMPORT int arpinit_done;
IMPORT int arpintr ();
IMPORT int arpintrq;
IMPORT int arpioctl ();
IMPORT int arpresolve ();
IMPORT int arpt_down;
IMPORT int arpt_keep;
IMPORT int arpt_prune;
IMPORT int arptabShow ();
IMPORT int arptfree ();
IMPORT int arpwhohas ();
IMPORT int asctime ();
IMPORT int asctime_r ();
IMPORT int asin ();
IMPORT int assertFiles;
IMPORT int assign__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCc ();
IMPORT int assign__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi ();
IMPORT int assign__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUi ();
IMPORT int assign__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Uic ();
IMPORT int assign__t18string_char_traits1ZcRcRCc ();
IMPORT int at__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui ();
IMPORT int at__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui ();
IMPORT int atan ();
IMPORT int atan2 ();
IMPORT int atexit ();
IMPORT int atof ();
IMPORT int atoi ();
IMPORT int atol ();
IMPORT int atox ();
IMPORT int attach__7filebufi ();
IMPORT int attrib ();
IMPORT int authnone_create ();
IMPORT int authunix_create ();
IMPORT int b ();
IMPORT int bad__C3ios ();
IMPORT int base__C9streambuf ();
IMPORT int bcmp ();
IMPORT int bcopy ();
IMPORT int bcopyBytes ();
IMPORT int bcopyLongs ();
IMPORT int bcopyWords ();
IMPORT int bcopy_to_mbufs ();
IMPORT int bd ();
IMPORT int bdall ();
IMPORT int before__C9type_infoRC9type_info ();
IMPORT int begin__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int begin__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int bfill ();
IMPORT int bfillBytes ();
IMPORT int bh ();
IMPORT int bibFreeRtn;
IMPORT int bibInit ();
IMPORT int bibMallocRtn;
IMPORT int bibRead ();
IMPORT int bibReadIntf ();
IMPORT int bibWrite ();
IMPORT int bibWriteIntf ();
IMPORT int bibgIntf;
IMPORT int binaryOptionsTsfsDrv;
IMPORT int bind ();
IMPORT int bindresvport ();
IMPORT int binvert ();
IMPORT int blen__C9streambuf ();
IMPORT int bootBpAnchorExtract ();
IMPORT int bootChange ();
IMPORT int bootEraseOrProgramOk ();
IMPORT int bootFlashAutoSelect ();
IMPORT int bootFlashErase ();
IMPORT int bootFlashGetProtect ();
IMPORT int bootFlashGetSectorOff ();
IMPORT int bootFlashParamRead ();
IMPORT int bootFlashParamWrite ();
IMPORT int bootFlashParamsProgram ();
IMPORT int bootFlashProgram1byte ();
IMPORT int bootLeaseExtract ();
IMPORT int bootNetmaskExtract ();
IMPORT int bootParamsErrorPrint ();
IMPORT int bootParamsPrompt ();
IMPORT int bootParamsShow ();
IMPORT int bootScanNum ();
IMPORT int bootStringToStruct ();
IMPORT int bootStructToString ();
IMPORT int bootrom_pciregfile;
IMPORT int bootrom_sramregfile;
IMPORT int bootrom_userflregfile;
IMPORT int bp1;
IMPORT int bp2;
IMPORT int bpFreeList;
IMPORT int bpList;
IMPORT int brgDbg ();
IMPORT int brgInit ();
IMPORT int brgSetCdv ();
IMPORT int brgStart ();
IMPORT int brg_regs_dump ();
IMPORT int bsdAccept ();
IMPORT int bsdBind ();
IMPORT int bsdConnect ();
IMPORT int bsdConnectWithTimeout ();
IMPORT int bsdGetpeername ();
IMPORT int bsdGetsockname ();
IMPORT int bsdGetsockopt ();
IMPORT int bsdListen ();
IMPORT int bsdRecv ();
IMPORT int bsdRecvfrom ();
IMPORT int bsdRecvmsg ();
IMPORT int bsdSend ();
IMPORT int bsdSendmsg ();
IMPORT int bsdSendto ();
IMPORT int bsdSetsockopt ();
IMPORT int bsdShutdown ();
IMPORT int bsdSock43ApiFlag;
IMPORT int bsdSockFunc;
IMPORT int bsdSockLibInit ();
IMPORT int bsdSocket ();
IMPORT int bsdZbufSockRtn ();
IMPORT int bsearch ();
IMPORT int bspEndIntConnect;
IMPORT int bspEndIntDisable;
IMPORT int bspEndIntDisconnect;
IMPORT int bspEndIntEnable;
IMPORT int bswap ();
IMPORT int buf;
IMPORT int bufAlloc ();
IMPORT int bufFree ();
IMPORT int bufPoolInit ();
IMPORT int buffer;
IMPORT int bufferedRtShow;
IMPORT int build_cluster ();
IMPORT int busIoMemRanges;
IMPORT int bzero ();
IMPORT int c ();
IMPORT int c_str__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int cacheArchDisable ();
IMPORT int cacheArchDisableFromMmu ();
IMPORT int cacheArchDmaFree ();
IMPORT int cacheArchDmaMalloc ();
IMPORT int cacheArchEnable ();
IMPORT int cacheArchFlush ();
IMPORT int cacheArchInvalidate ();
IMPORT int cacheArchLibInit ();
IMPORT int cacheArchPipeFlush ();
IMPORT int cacheArchTextUpdate ();
IMPORT int cacheClear ();
IMPORT int cacheDToEnable;
IMPORT int cacheDataEnabled;
IMPORT int cacheDataMode;
IMPORT int cacheDisable ();
IMPORT int cacheDmaFree ();
IMPORT int cacheDmaFreeRtn;
IMPORT int cacheDmaFuncs;
IMPORT int cacheDmaMalloc ();
IMPORT int cacheDmaMallocRtn;
IMPORT int cacheDrvFlush ();
IMPORT int cacheDrvInvalidate ();
IMPORT int cacheDrvPhysToVirt ();
IMPORT int cacheDrvVirtToPhys ();
IMPORT int cacheEnable ();
IMPORT int cacheErrnoSet ();
IMPORT int cacheFlush ();
IMPORT int cacheFuncsSet ();
IMPORT int cacheIToEnable;
IMPORT int cacheInvalidate ();
IMPORT int cacheLib;
IMPORT int cacheLibInit ();
IMPORT int cacheLock ();
IMPORT int cacheMmuAvailable;
IMPORT int cacheNullFuncs;
IMPORT int cachePipeFlush ();
IMPORT int cachePpcDisable ();
IMPORT int cachePpcEnable ();
IMPORT int cachePpcReadOrigin;
IMPORT int cacheTextUpdate ();
IMPORT int cacheUnlock ();
IMPORT int cacheUserFuncs;
IMPORT int calloc ();
IMPORT int callrpc ();
IMPORT int capacity__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int cbioBlkCopy ();
IMPORT int cbioBlkRW ();
IMPORT int cbioBytesRW ();
IMPORT int cbioClass;
IMPORT int cbioClassId;
IMPORT int cbioDevCreate ();
IMPORT int cbioDevVerify ();
IMPORT int cbioIoctl ();
IMPORT int cbioLibInit ();
IMPORT int cbioLock ();
IMPORT int cbioModeGet ();
IMPORT int cbioModeSet ();
IMPORT int cbioMutexSemOptions;
IMPORT int cbioParamsGet ();
IMPORT int cbioRdyChgdGet ();
IMPORT int cbioRdyChgdSet ();
IMPORT int cbioRdyChk ();
IMPORT int cbioShow ();
IMPORT int cbioUnlock ();
IMPORT int cbioWrapBlkDev ();
IMPORT int cd ();
IMPORT int ceil ();
IMPORT int cenr;
IMPORT int cerr;
IMPORT int cfree ();
IMPORT int changeReg ();
IMPORT int chdir ();
IMPORT int checkInetAddrField ();
IMPORT int checkStack ();
IMPORT int check_realloc__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui ();
IMPORT int check_trailer ();
IMPORT int checksum ();
IMPORT int chkdsk ();
IMPORT int cin;
IMPORT int clDescTbl;
IMPORT int clDescTblNumEnt;
IMPORT int classClassId;
IMPORT int classCreate ();
IMPORT int classDestroy ();
IMPORT int classInit ();
IMPORT int classInstConnect ();
IMPORT int classInstrument ();
IMPORT int classLibInit ();
IMPORT int classMemPartIdSet ();
IMPORT int classShow ();
IMPORT int classShowConnect ();
IMPORT int classShowInit ();
IMPORT int clearIntFlag ();
IMPORT int clear__10RBString_T ();
IMPORT int clear__3iosi ();
IMPORT int clearerr ();
IMPORT int clnt_create ();
IMPORT int clnt_genericInclude ();
IMPORT int clnt_pcreateerror ();
IMPORT int clnt_perrno ();
IMPORT int clnt_perror ();
IMPORT int clnt_rawInclude ();
IMPORT int clnt_simpleInclude ();
IMPORT int clnt_spcreateerror ();
IMPORT int clnt_sperrno ();
IMPORT int clnt_sperror ();
IMPORT int clntraw_create ();
IMPORT int clnttcp_create ();
IMPORT int clntudp_bufcreate ();
IMPORT int clntudp_create ();
IMPORT int clntudp_freeres ();
IMPORT int clock ();
IMPORT int clockLibInit ();
IMPORT int clock_getres ();
IMPORT int clock_gettime ();
IMPORT int clock_setres ();
IMPORT int clock_settime ();
IMPORT int clock_show ();
IMPORT int clog;
IMPORT int clone__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3Rep ();
IMPORT int close ();
IMPORT int close__3ios ();
IMPORT int close__7filebuf ();
IMPORT int closedir ();
IMPORT int compare__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi ();
IMPORT int compare__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUiUi ();
IMPORT int compare__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUi ();
IMPORT int compare__t18string_char_traits1ZcPCcT1Ui ();
IMPORT int compare_error ();
IMPORT int connect ();
IMPORT int connectWithTimeout ();
IMPORT int connectors;
IMPORT int consoleFd;
IMPORT int consoleName;
IMPORT int copy ();
IMPORT int copyFromMbufs ();
IMPORT int copyStreams ();
IMPORT int copy__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PcUiUi ();
IMPORT int copy__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepUiPCcUi ();
IMPORT int copy__t18string_char_traits1ZcPcPCcUi ();
IMPORT int copyright_wind_river;
IMPORT int copysign ();
IMPORT int correctaddr ();
IMPORT int cos ();
IMPORT int cosh ();
IMPORT int count;
IMPORT int cout;
IMPORT int cp ();
IMPORT int cpldGppDisablePin13 ();
IMPORT int cplusArraysInit ();
IMPORT int cplusCallCtors ();
IMPORT int cplusCallDtors ();
IMPORT int cplusCallNewHandler__Fv ();
IMPORT int cplusCtors ();
IMPORT int cplusCtorsLink ();
IMPORT int cplusDemangle ();
IMPORT int cplusDemangleFunc;
IMPORT int cplusDemanglerInit ();
IMPORT int cplusDemanglerMode;
IMPORT int cplusDemanglerSet ();
IMPORT int cplusDemanglerStyle;
IMPORT int cplusDemanglerStyleSet ();
IMPORT int cplusDtors ();
IMPORT int cplusDtorsLink ();
IMPORT int cplusLibInit ();
IMPORT int cplusLibMinInit ();
IMPORT int cplusLoadFixup ();
IMPORT int cplusMatchMangled ();
IMPORT int cplusNewHandlerExists__Fv ();
IMPORT int cplusNewHdlMutex;
IMPORT int cplusTerminate__Fv ();
IMPORT int cplusUnloadFixup ();
IMPORT int cplusXtorSet ();
IMPORT int cplusXtorStrategy;
IMPORT int cplus_demangle ();
IMPORT int cplus_demangle_name_to_style ();
IMPORT int cplus_demangle_opname ();
IMPORT int cplus_demangle_set_style ();
IMPORT int cplus_mangle_opname ();
IMPORT int creat ();
IMPORT int create__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepUi ();
IMPORT int creationDate;
IMPORT int cret ();
IMPORT int ctime ();
IMPORT int ctime_r ();
IMPORT int ctohex ();
IMPORT int ctohex1 ();
IMPORT int ctypeFiles;
IMPORT int cunitInit ();
IMPORT int cunitSetAccessControl ();
IMPORT int cur_ptr__7filebuf ();
IMPORT int currentContext;
IMPORT int current_demangling_style;
IMPORT int d ();
IMPORT int data__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int data__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3Rep ();
IMPORT int dater;
IMPORT int dayr;
IMPORT int db_print_sa ();
IMPORT int db_show_arptab ();
IMPORT int dbgHelp ();
IMPORT int dbgInit ();
IMPORT int dbgLockUnbreakable;
IMPORT int dbgPrintAltivec;
IMPORT int dbgPrintCall ();
IMPORT int dbgPrintDsp;
IMPORT int dbgPrintFpp;
IMPORT int dbgPrintSimd;
IMPORT int dbgSafeUnbreakable;
IMPORT int dbgTaskBpBreakpoint ();
IMPORT int dbgTaskBpHooksInstall ();
IMPORT int dbgTaskBpTrace ();
IMPORT int dbgTaskCont ();
IMPORT int dbgTaskStep ();
IMPORT int dbgUnbreakableOld;
IMPORT int dcacheCtrl;
IMPORT int dcacheDevCreate ();
IMPORT int dcacheDevDisable ();
IMPORT int dcacheDevEnable ();
IMPORT int dcacheDevMemResize ();
IMPORT int dcacheDevTune ();
IMPORT int dcacheShow ();
IMPORT int dcacheUpd ();
IMPORT int dcacheUpdTaskId;
IMPORT int dcacheUpdTaskOptions;
IMPORT int dcacheUpdTaskPriority;
IMPORT int dcacheUpdTaskStack;
IMPORT int dcast__C14__si_type_infoRC9type_infoiPvPC9type_infoT3 ();
IMPORT int dcast__C16__user_type_infoRC9type_infoiPvPC9type_infoT3 ();
IMPORT int dcast__C17__class_type_infoRC9type_infoiPvPC9type_infoT3 ();
IMPORT int deallocate__t23__malloc_alloc_template1i0PvUi ();
IMPORT int deallocate__t24__default_alloc_template2b1i0PvUi ();
IMPORT int debug_flash_width;
IMPORT int dec__FR3ios ();
IMPORT int decode_identifier ();
IMPORT int decode_identifier_styled ();
IMPORT int delta__12streammarker ();
IMPORT int delta__12streammarkerR12streammarker ();
IMPORT int detectRAMsize ();
IMPORT int detectUserFlash ();
IMPORT int devs ();
IMPORT int difftime ();
IMPORT int dirList ();
IMPORT int diskFormat ();
IMPORT int diskInit ();
IMPORT int div ();
IMPORT int div_r ();
IMPORT int dllAdd ();
IMPORT int dllCount ();
IMPORT int dllCreate ();
IMPORT int dllDelete ();
IMPORT int dllEach ();
IMPORT int dllGet ();
IMPORT int dllInit ();
IMPORT int dllInsert ();
IMPORT int dllRemove ();
IMPORT int dllTerminate ();
IMPORT int do_osfx__7ostream ();
IMPORT int do_protocol_with_type ();
IMPORT int doallocate__7filebuf ();
IMPORT int doallocate__9streambuf ();
IMPORT int doallocbuf__9streambuf ();
IMPORT int domaininit ();
IMPORT int domains;
IMPORT int dosChkDebug;
IMPORT int dosChkDsk ();
IMPORT int dosChkEntryMark ();
IMPORT int dosChkLibInit ();
IMPORT int dosChkMinDate;
IMPORT int dosDirHdlrsList;
IMPORT int dosDirOldDebug;
IMPORT int dosDirOldLibInit ();
IMPORT int dosFatHdlrsList;
IMPORT int dosFsCacheSizeDefault;
IMPORT int dosFsChkDsk ();
IMPORT int dosFsChkRtn;
IMPORT int dosFsChkTree ();
IMPORT int dosFsConfigGet ();
IMPORT int dosFsConfigInit ();
IMPORT int dosFsConfigShow ();
IMPORT int dosFsDateTimeInstall ();
IMPORT int dosFsDebug;
IMPORT int dosFsDevCreate ();
IMPORT int dosFsDevInit ();
IMPORT int dosFsDevInitOptionsSet ();
IMPORT int dosFsDrvNum;
IMPORT int dosFsFatInit ();
IMPORT int dosFsFmtLibInit ();
IMPORT int dosFsHdlrInstall ();
IMPORT int dosFsInit ();
IMPORT int dosFsLastAccessDateEnable ();
IMPORT int dosFsLibInit ();
IMPORT int dosFsMkfs ();
IMPORT int dosFsMkfsOptionsSet ();
IMPORT int dosFsModeChange ();
IMPORT int dosFsReadyChange ();
IMPORT int dosFsShow ();
IMPORT int dosFsVolDescGet ();
IMPORT int dosFsVolFormat ();
IMPORT int dosFsVolFormatRtn;
IMPORT int dosFsVolIsFat12 ();
IMPORT int dosFsVolOptionsGet ();
IMPORT int dosFsVolOptionsSet ();
IMPORT int dosFsVolUnmount ();
IMPORT int dosSetVolCaseSens ();
IMPORT int dosVDirDebug;
IMPORT int dosVDirLibInit ();
IMPORT int drem ();
IMPORT int drvCtrl;
IMPORT int drvTable;
IMPORT int dsmInst ();
IMPORT int e ();
IMPORT int eBptr__9streambuf ();
IMPORT int eGptr__9streambuf ();
IMPORT int eNptr__9streambuf ();
IMPORT int eback__C9streambuf ();
IMPORT int ebuf__C9streambuf ();
IMPORT int egptr__C9streambuf ();
IMPORT int elfRelocRelEntryRd ();
IMPORT int elfRelocRelaEntryRd ();
IMPORT int empty__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int end8023AddressForm ();
IMPORT int endDevName ();
IMPORT int endDevTbl;
IMPORT int endEtherAddressForm ();
IMPORT int endEtherPacketAddrGet ();
IMPORT int endEtherPacketDataGet ();
IMPORT int endFindByName ();
IMPORT int endFlagsClr ();
IMPORT int endFlagsGet ();
IMPORT int endFlagsSet ();
IMPORT int endMibIfInit ();
IMPORT int endMultiLstCnt ();
IMPORT int endMultiLstFirst ();
IMPORT int endMultiLstNext ();
IMPORT int endObjFlagSet ();
IMPORT int endObjInit ();
IMPORT int endObjectUnload ();
IMPORT int endPollStatsInit ();
IMPORT int endRcvRtnCall ();
IMPORT int endTxSemGive ();
IMPORT int endTxSemTake ();
IMPORT int end__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int end__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int end_addr;
IMPORT int endl__FR7ostream ();
IMPORT int ends__FR7ostream ();
IMPORT int envLibInit ();
IMPORT int envPrivateCreate ();
IMPORT int envPrivateDestroy ();
IMPORT int envShow ();
IMPORT int eof__C3ios ();
IMPORT int eos__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int eos__t18string_char_traits1Zc ();
IMPORT int epptr__C9streambuf ();
IMPORT int eq__t18string_char_traits1ZcRCcT1 ();
IMPORT int erase__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Pc ();
IMPORT int erase__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PcT1 ();
IMPORT int erase__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUi ();
IMPORT int errno;
IMPORT int errnoGet ();
IMPORT int errnoOfTaskGet ();
IMPORT int errnoOfTaskSet ();
IMPORT int errnoSet ();
IMPORT int ethBCopy ();
IMPORT int ethClearMibCounters ();
IMPORT int ethPortInit ();
IMPORT int ethPortMcAddr ();
IMPORT int ethPortReadSmiReg ();
IMPORT int ethPortReceive ();
IMPORT int ethPortReset ();
IMPORT int ethPortSend ();
IMPORT int ethPortSetRxCoal ();
IMPORT int ethPortSetTxCoal ();
IMPORT int ethPortStart ();
IMPORT int ethPortUcAddrSet ();
IMPORT int ethPortWriteSmiReg ();
IMPORT int ethReadMibCounter ();
IMPORT int ethRxReturnBuff ();
IMPORT int ethTxReturnDesc ();
IMPORT int etherInitRxDescRing ();
IMPORT int etherInitTxDescRing ();
IMPORT int etherMultiAdd ();
IMPORT int etherMultiDebug;
IMPORT int etherMultiDel ();
IMPORT int etherMultiGet ();
IMPORT int ether_addmulti ();
IMPORT int ether_attach ();
IMPORT int ether_delmulti ();
IMPORT int ether_ifattach ();
IMPORT int ether_input ();
IMPORT int ether_ipmulticast_max;
IMPORT int ether_ipmulticast_min;
IMPORT int ether_output ();
IMPORT int ether_sprintf ();
IMPORT int etherbroadcastaddr;
IMPORT int ethernetGetConfigReg ();
IMPORT int ethernetPhyReset ();
IMPORT int ethernetResetConfigReg ();
IMPORT int ethernetSetConfigReg ();
IMPORT int eventClear ();
IMPORT int eventEvtRtn;
IMPORT int eventInit ();
IMPORT int eventLibInit ();
IMPORT int eventReceive ();
IMPORT int eventRsrcSend ();
IMPORT int eventRsrcShow ();
IMPORT int eventSend ();
IMPORT int eventStart ();
IMPORT int eventTaskShow ();
IMPORT int eventTerminate ();
IMPORT int evtAction;
IMPORT int evtBufferBind ();
IMPORT int evtBufferId;
IMPORT int evtLogFuncBind ();
IMPORT int evtLogM0 ();
IMPORT int evtLogM1 ();
IMPORT int evtLogM2 ();
IMPORT int evtLogM3 ();
IMPORT int evtLogO ();
IMPORT int evtLogOInt ();
IMPORT int evtLogPoint ();
IMPORT int evtLogString ();
IMPORT int evtLogT0 ();
IMPORT int evtLogT0_noInt ();
IMPORT int evtLogT1 ();
IMPORT int evtLogT1_noTS ();
IMPORT int evtLogTasks ();
IMPORT int evtObjLogFuncBind ();
IMPORT int evtTimeStamp;
IMPORT int evtsched ();
IMPORT int excConnect ();
IMPORT int excEPSet ();
IMPORT int excEnt ();
IMPORT int excExcHandle ();
IMPORT int excExcepHook;
IMPORT int excExit ();
IMPORT int excExtendedVectors;
IMPORT int excHookAdd ();
IMPORT int excInit ();
IMPORT int excIntConnect ();
IMPORT int excIntHandle ();
IMPORT int excJobAdd ();
IMPORT int excMsgQId;
IMPORT int excRelocConnect ();
IMPORT int excRelocIntConnect ();
IMPORT int excShowInit ();
IMPORT int excTask ();
IMPORT int excTaskId;
IMPORT int excTaskOptions;
IMPORT int excTaskPriority;
IMPORT int excTaskStackSize;
IMPORT int excVecBaseGet ();
IMPORT int excVecBaseSet ();
IMPORT int excVecGet ();
IMPORT int excVecInit ();
IMPORT int excVecSet ();
IMPORT int exceptions__3iosi ();
IMPORT int exceptions__C3ios ();
IMPORT int excess_slop__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepUiUi ();
IMPORT int execute ();
IMPORT int exit ();
IMPORT int exp ();
IMPORT int exp__E ();
IMPORT int expm1 ();
IMPORT int extendedTestError ();
IMPORT int extractCString__10RBString_TPci ();
IMPORT int fabs ();
IMPORT int fail__C3ios ();
IMPORT int fastUdpInitialized;
IMPORT int fat16ClustValueGet ();
IMPORT int fat16ClustValueSet ();
IMPORT int fat16Debug;
IMPORT int fat16VolMount ();
IMPORT int fatClugFac;
IMPORT int fclose ();
IMPORT int fd;
IMPORT int fdTable;
IMPORT int fd__C7filebuf ();
IMPORT int fdopen ();
IMPORT int fdprintf ();
IMPORT int fei82557Int ();
IMPORT int fei82557MDIRead ();
IMPORT int fei82557MDIWrite ();
IMPORT int feiDevices;
IMPORT int feiEndIntConnect;
IMPORT int feiEndIntDisconnect;
IMPORT int feiResources;
IMPORT int feiShow ();
IMPORT int feiTxStartup ();
IMPORT int feidevices;
IMPORT int feof ();
IMPORT int ferror ();
IMPORT int fflush ();
IMPORT int ffsMsb ();
IMPORT int fgetc ();
IMPORT int fgetpos ();
IMPORT int fgets ();
IMPORT int fieldSzIncludeSign;
IMPORT int fileUpPathDefaultPerm;
IMPORT int fileUploadPathClose ();
IMPORT int fileUploadPathCreate ();
IMPORT int fileUploadPathLibInit ();
IMPORT int fileUploadPathWrite ();
IMPORT int file_ptr__7filebuf ();
IMPORT int fileno ();
IMPORT int fill__3ioss ();
IMPORT int fill__C3ios ();
IMPORT int find__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi ();
IMPORT int find__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUiUi ();
IMPORT int find__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui ();
IMPORT int find__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0cUi ();
IMPORT int find_first_not_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi ();
IMPORT int find_first_not_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUiUi ();
IMPORT int find_first_not_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui ();
IMPORT int find_first_not_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0cUi ();
IMPORT int find_first_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi ();
IMPORT int find_first_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUiUi ();
IMPORT int find_first_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui ();
IMPORT int find_first_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0cUi ();
IMPORT int find_last_not_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi ();
IMPORT int find_last_not_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUiUi ();
IMPORT int find_last_not_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui ();
IMPORT int find_last_not_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0cUi ();
IMPORT int find_last_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi ();
IMPORT int find_last_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUiUi ();
IMPORT int find_last_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui ();
IMPORT int find_last_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0cUi ();
IMPORT int finite ();
IMPORT int fioFltInstall ();
IMPORT int fioFormatV ();
IMPORT int fioLibInit ();
IMPORT int fioRdString ();
IMPORT int fioRead ();
IMPORT int fioScanV ();
IMPORT int flags__3iosUl ();
IMPORT int flags__C3ios ();
IMPORT int flashAutoDetect ();
IMPORT int flashBlocks;
IMPORT int flashBootSectorInit ();
IMPORT int flashBuf;
IMPORT int flashBufNV;
IMPORT int flash_index;
IMPORT int flash_read;
IMPORT int flash_width;
IMPORT int flash_write;
IMPORT int floatInit ();
IMPORT int flocalBuf;
IMPORT int floor ();
IMPORT int flush__7ostream ();
IMPORT int flush__FR7ostream ();
IMPORT int flush_all__9streambuf ();
IMPORT int flush_all_linebuffered__9streambuf ();
IMPORT int fmod ();
IMPORT int fopen ();
IMPORT int fpClassId;
IMPORT int fpCtlRegName;
IMPORT int fpExcMsgTbl;
IMPORT int fpRegName;
IMPORT int fpTypeGet ();
IMPORT int fppArchInit ();
IMPORT int fppArchTaskCreateInit ();
IMPORT int fppCreateHookRtn;
IMPORT int fppCtxToRegs ();
IMPORT int fppDisplayHookRtn;
IMPORT int fppInit ();
IMPORT int fppProbe ();
IMPORT int fppRegsToCtx ();
IMPORT int fppRestore ();
IMPORT int fppSave ();
IMPORT int fppShowInit ();
IMPORT int fppTaskRegsCFmt;
IMPORT int fppTaskRegsDFmt;
IMPORT int fppTaskRegsGet ();
IMPORT int fppTaskRegsSet ();
IMPORT int fppTaskRegsShow ();
IMPORT int fprintf ();
IMPORT int fputc ();
IMPORT int fputs ();
IMPORT int frcAlignedMalloc ();
IMPORT int frcBibAttach ();
IMPORT int frcBibDataShow ();
IMPORT int frcBibDataWrite ();
IMPORT int frcBibDataWriteBin ();
IMPORT int frcBibDetach ();
IMPORT int frcBibDrvShow ();
IMPORT int frcBibElemRead ();
IMPORT int frcBibElemShow ();
IMPORT int frcBibElemWrite ();
IMPORT int frcBibErrnoGet ();
IMPORT int frcBibRead ();
IMPORT int frcBibShow ();
IMPORT int frcBibShowRtn;
IMPORT int frcBibWrite ();
IMPORT int frcBoardIdInit ();
IMPORT int frcBootFlashFile ();
IMPORT int frcBootFlashFileChipErase ();
IMPORT int frcBootFlashFileV ();
IMPORT int frcBootFlashProgram ();
IMPORT int frcBootFlashSectorErase ();
IMPORT int frcBootFlashVerify ();
IMPORT int frcBootRegFileBurn ();
IMPORT int frcCPUClockGet ();
IMPORT int frcCheckSum ();
IMPORT int frcDispBoardInfo ();
IMPORT int frcDmaCompleteAppIntHandler0 ();
IMPORT int frcDmaCompleteAppIntHandler1 ();
IMPORT int frcDmaIntConnect ();
IMPORT int frcDmaIntCtrlInit ();
IMPORT int frcDmaIntDisable ();
IMPORT int frcDmaIntEnable ();
IMPORT int frcDmaTestStart0 ();
IMPORT int frcDmaTestStart1 ();
IMPORT int frcDmaTestTaskEntry0 ();
IMPORT int frcDmaTestTaskEntry1 ();
IMPORT int frcEEPROMRead16 ();
IMPORT int frcEEPROMRead8 ();
IMPORT int frcEEPROMTestRead ();
IMPORT int frcEEPROMTestWrite ();
IMPORT int frcEEPROMWrite16 ();
IMPORT int frcEEPROMWrite8 ();
IMPORT int frcFei82557EndLoad ();
IMPORT int frcFlashAutoSelect ();
IMPORT int frcFlashBlockErase ();
IMPORT int frcFlashBlockWrite ();
IMPORT int frcFlashErase ();
IMPORT int frcFlashGetInfo ();
IMPORT int frcFlashLock ();
IMPORT int frcFlashProgram ();
IMPORT int frcFlashRead ();
IMPORT int frcFlashReadIdCommand ();
IMPORT int frcFlashReadRst ();
IMPORT int frcFlashUnlock ();
IMPORT int frcFlashUnlockComplete ();
IMPORT int frcGetBank0MemSize ();
IMPORT int frcGetBank1MemSize ();
IMPORT int frcGetExtSramSize ();
IMPORT int frcGetFlashOrg ();
IMPORT int frcGetPcbRev ();
IMPORT int frcGetUserFlashSize ();
IMPORT int frcGetVariant ();
IMPORT int frcGppCPU1IntDisable ();
IMPORT int frcGppCPU1IntEnable ();
IMPORT int frcGppIntConnect ();
IMPORT int frcGppIntCtrlInit ();
IMPORT int frcGppIntDisable ();
IMPORT int frcGppIntEnable ();
IMPORT int frcI2CWrite ();
IMPORT int frcIdmaTestInit0 ();
IMPORT int frcIdmaTestInit1 ();
IMPORT int frcMACRead ();
IMPORT int frcMACWrite ();
IMPORT int frcMemTest ();
IMPORT int frcModNumBase;
IMPORT int frcMv64360SiliconVersion ();
IMPORT int frcPCIDataWrite ();
IMPORT int frcPci0ReadConfigReg ();
IMPORT int frcPci0WriteConfigReg ();
IMPORT int frcPciConfigRead;
IMPORT int frcPciConfigReadVPD ();
IMPORT int frcPciConfigWrite;
IMPORT int frcPciConfigWriteVPD ();
IMPORT int frcPciSetActive ();
IMPORT int frcPciShow ();
IMPORT int frcPrintBank0MemSize ();
IMPORT int frcPrintBank1MemSize ();
IMPORT int frcPrintBoardInfo ();
IMPORT int frcPrintBomRev ();
IMPORT int frcPrintMv64360Version ();
IMPORT int frcPrintPcbRev ();
IMPORT int frcPrintUserFlashSize ();
IMPORT int frcPrintVariant ();
IMPORT int frcRTCDummyWrite1 ();
IMPORT int frcRTCRead ();
IMPORT int frcRTCWrite ();
IMPORT int frcSys557Init ();
IMPORT int frcSysBoardId;
IMPORT int frcSysTest ();
IMPORT int frcTestAllTests ();
IMPORT int frcTestAltivec ();
IMPORT int frcTestBIB ();
IMPORT int frcTestBaudRate ();
IMPORT int frcTestBootFlash ();
IMPORT int frcTestChkECC ();
IMPORT int frcTestConfigEthPorts ();
IMPORT int frcTestConnect ();
IMPORT int frcTestDMA ();
IMPORT int frcTestDisable ();
IMPORT int frcTestDispAllPCI0Reg ();
IMPORT int frcTestDispFreq ();
IMPORT int frcTestDispMemMap ();
IMPORT int frcTestDispReg ();
IMPORT int frcTestDualCPU ();
IMPORT int frcTestEthLinkDetails ();
IMPORT int frcTestEthModeSet ();
IMPORT int frcTestEthPHYTestMode ();
IMPORT int frcTestEthernet ();
IMPORT int frcTestGettimeUsec ();
IMPORT int frcTestI2C_EEPROM ();
IMPORT int frcTestIDMAScrubChk ();
IMPORT int frcTestIntLatHard ();
IMPORT int frcTestIntLatSoft ();
IMPORT int frcTestIntLatency ();
IMPORT int frcTestIsMonarch ();
IMPORT int frcTestLatInitialization ();
IMPORT int frcTestMACAddr ();
IMPORT int frcTestMPP18ISR_hard ();
IMPORT int frcTestMPP18ISR_soft ();
IMPORT int frcTestMV64360Semaphore ();
IMPORT int frcTestMemPerf ();
IMPORT int frcTestMemTest ();
IMPORT int frcTestPciScan ();
IMPORT int frcTestRTC ();
IMPORT int frcTestRTCRead ();
IMPORT int frcTestRTCWrite ();
IMPORT int frcTestSerial ();
IMPORT int frcTestSerialInfo ();
IMPORT int frcTestStressSerial ();
IMPORT int frcTestSubsysVendID ();
IMPORT int frcTestSummary ();
IMPORT int frcTestSwitch ();
IMPORT int frcTestTbReset ();
IMPORT int frcTestUserFlash ();
IMPORT int frcTestVPD ();
IMPORT int frcTestWatchDogAppStart ();
IMPORT int frcTestWatchDogAppStop ();
IMPORT int frcTestWdInitConnect;
IMPORT int frcTestWdInitDisconnect;
IMPORT int frcTestWhoAmI ();
IMPORT int frcTestgettimeofday ();
IMPORT int frcVPDInit ();
IMPORT int frcVPDRead ();
IMPORT int frcVPDWrite ();
IMPORT int frcWatchdogDisable ();
IMPORT int frcWatchdogEnable ();
IMPORT int frcWatchdogInit ();
IMPORT int frcWatchdogLoad ();
IMPORT int frcWatchdogNMILoad ();
IMPORT int frcWatchdogService ();
IMPORT int frc_end_of_vxworks;
IMPORT int frcbootFlashReadRst ();
IMPORT int frccachePpcEnable ();
IMPORT int fread ();
IMPORT int free ();
IMPORT int free_backup_area__9streambuf ();
IMPORT int freopen ();
IMPORT int frexp ();
IMPORT int frob_size__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepUi ();
IMPORT int fscanf ();
IMPORT int fseek ();
IMPORT int fsetpos ();
IMPORT int fst ();
IMPORT int fstat ();
IMPORT int fstatfs ();
IMPORT int ftell ();
IMPORT int ftpCommand ();
IMPORT int ftpCommandEnhanced ();
IMPORT int ftpDataConnGet ();
IMPORT int ftpDataConnInit ();
IMPORT int ftpDataConnInitPassiveMode ();
IMPORT int ftpHookup ();
IMPORT int ftpLibDebugOptionsSet ();
IMPORT int ftpLogin ();
IMPORT int ftpLs ();
IMPORT int ftpReplyGet ();
IMPORT int ftpReplyGetEnhanced ();
IMPORT int ftpTransientConfigGet ();
IMPORT int ftpTransientConfigSet ();
IMPORT int ftpTransientFatalInstall ();
IMPORT int ftpXfer ();
IMPORT int ftpdDebug;
IMPORT int ftpdDelete ();
IMPORT int ftpdInit ();
IMPORT int ftpdTaskOptions;
IMPORT int ftpdTaskPriority;
IMPORT int ftpdWindowSize;
IMPORT int ftpdWorkTaskOptions;
IMPORT int ftpdWorkTaskPriority;
IMPORT int ftpdWorkTaskStackSize;
IMPORT int ftplDebug;
IMPORT int ftplPasvModeDisable;
IMPORT int ftplTransientMaxRetryCount;
IMPORT int ftplTransientRetryInterval;
IMPORT int ftpsCurrentClients;
IMPORT int ftpsMaxClients;
IMPORT int fwrite ();
IMPORT int gbump__9streambufi ();
IMPORT int gcount__7istream ();
IMPORT int gei82543EndLoad ();
IMPORT int geiEndLoadStr;
IMPORT int geiResources;
IMPORT int geidevices;
IMPORT int getFlashBase ();
IMPORT int getOption ();
IMPORT int getPLLCoreMultiplier ();
IMPORT int getPMC280BoardInfo ();
IMPORT int getParams ();
IMPORT int get__7istream ();
IMPORT int get__7istreamPScic ();
IMPORT int get__7istreamPUcic ();
IMPORT int get__7istreamPcic ();
IMPORT int get__7istreamRSc ();
IMPORT int get__7istreamRUc ();
IMPORT int get__7istreamRc ();
IMPORT int get_column__9streambuf ();
IMPORT int get_mem ();
IMPORT int get_myaddress ();
IMPORT int get_word ();
IMPORT int getc ();
IMPORT int getchar ();
IMPORT int getcwd ();
IMPORT int getenv ();
IMPORT int gethostname ();
IMPORT int getline__7istreamPScic ();
IMPORT int getline__7istreamPUcic ();
IMPORT int getline__7istreamPcic ();
IMPORT int getline__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_R7istreamRt12basic_string3ZX01ZX11ZX21X01_R7istream ();
IMPORT int getpeername ();
IMPORT int gets ();
IMPORT int gets__7istreamPPcc ();
IMPORT int getsockname ();
IMPORT int getsockopt ();
IMPORT int gettime ();
IMPORT int getw ();
IMPORT int getwd ();
IMPORT int gmtime ();
IMPORT int gmtime_r ();
IMPORT int good__C3ios ();
IMPORT int gptr__C9streambuf ();
IMPORT int grab__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3Rep ();
IMPORT int gtCntmrDisable ();
IMPORT int gtCntmrEnable ();
IMPORT int gtCntmrIsTerminate ();
IMPORT int gtCntmrLoad ();
IMPORT int gtCntmrRead ();
IMPORT int gtCntmrSetMode ();
IMPORT int gtCntmrSetTCTclocks ();
IMPORT int gtCntmrStart ();
IMPORT int gtDmaCommand ();
IMPORT int gtDmaEngineDisable ();
IMPORT int gtDmaIsChannelActive ();
IMPORT int gtDmaSetEngineAccessProtect ();
IMPORT int gtDmaSetMemorySpace ();
IMPORT int gtDmaSetMemorySpaceAttr ();
IMPORT int gtDmaStart ();
IMPORT int gtDmaTransfer ();
IMPORT int gtDmaUpdateArbiter ();
IMPORT int gtInit ();
IMPORT int gtInitSdramSize ();
IMPORT int gtInitWindows ();
IMPORT int gtIntConnect ();
IMPORT int gtIntCtrlInit ();
IMPORT int gtIntDisable ();
IMPORT int gtIntDisconnect ();
IMPORT int gtIntEnable ();
IMPORT int gtInternalRegBaseAddr;
IMPORT int gtMemoryDisableProtectRegion ();
IMPORT int gtMemoryDisableWindow ();
IMPORT int gtMemoryEnableWindow ();
IMPORT int gtMemoryGetBankBaseAddress ();
IMPORT int gtMemoryGetBankSize ();
IMPORT int gtMemoryGetDeviceBaseAddress ();
IMPORT int gtMemoryGetDeviceParam ();
IMPORT int gtMemoryGetDeviceSize ();
IMPORT int gtMemoryGetDeviceWidth ();
IMPORT int gtMemoryGetInternalRegistersSpace ();
IMPORT int gtMemoryGetInternalSramBaseAddr ();
IMPORT int gtMemoryGetMemWindowStatus ();
IMPORT int gtMemoryMapBank0 ();
IMPORT int gtMemoryMapBank1 ();
IMPORT int gtMemoryMapBank2 ();
IMPORT int gtMemoryMapBank3 ();
IMPORT int gtMemoryMapDevice0Space ();
IMPORT int gtMemoryMapDevice1Space ();
IMPORT int gtMemoryMapDevice2Space ();
IMPORT int gtMemoryMapDevice3Space ();
IMPORT int gtMemoryMapDeviceBootSpace ();
IMPORT int gtMemoryMapInternalRegistersSpace ();
IMPORT int gtMemorySetDeviceParam ();
IMPORT int gtMemorySetInternalSramBaseAddr ();
IMPORT int gtMemorySetPciRemapValue ();
IMPORT int gtMemorySetProtectRegion ();
IMPORT int gtPci0ArbiterDisable ();
IMPORT int gtPci0ArbiterEnable ();
IMPORT int gtPci0DisableAccessRegion ();
IMPORT int gtPci0DisableBrokenAgentDetection ();
IMPORT int gtPci0EnableBrokenAgentDetection ();
IMPORT int gtPci0GetBusMode ();
IMPORT int gtPci0GetIOspaceBase ();
IMPORT int gtPci0GetIOspaceSize ();
IMPORT int gtPci0GetMemory0Base ();
IMPORT int gtPci0GetMemory0Size ();
IMPORT int gtPci0GetMemory1Base ();
IMPORT int gtPci0GetMemory1Size ();
IMPORT int gtPci0GetMemory2Base ();
IMPORT int gtPci0GetMemory2Size ();
IMPORT int gtPci0GetMemory3Base ();
IMPORT int gtPci0GetMemory3Size ();
IMPORT int gtPci0MapBootDeviceMemorySpace ();
IMPORT int gtPci0MapBootDeviceMemorySpaceDAC ();
IMPORT int gtPci0MapCPUspace ();
IMPORT int gtPci0MapCPUspaceDAC ();
IMPORT int gtPci0MapDevice0MemorySpace ();
IMPORT int gtPci0MapDevice0MemorySpaceDAC ();
IMPORT int gtPci0MapDevice1MemorySpace ();
IMPORT int gtPci0MapDevice1MemorySpaceDAC ();
IMPORT int gtPci0MapDevice2MemorySpace ();
IMPORT int gtPci0MapDevice2MemorySpaceDAC ();
IMPORT int gtPci0MapDevice3MemorySpace ();
IMPORT int gtPci0MapDevice3MemorySpaceDAC ();
IMPORT int gtPci0MapIOspace ();
IMPORT int gtPci0MapInternalRegIOSpace ();
IMPORT int gtPci0MapInternalRegSpace ();
IMPORT int gtPci0MapInternalRegSpaceDAC ();
IMPORT int gtPci0MapMemory0space ();
IMPORT int gtPci0MapMemory1space ();
IMPORT int gtPci0MapMemory2space ();
IMPORT int gtPci0MapMemory3space ();
IMPORT int gtPci0MapMemoryBank0 ();
IMPORT int gtPci0MapMemoryBank0DAC ();
IMPORT int gtPci0MapMemoryBank1 ();
IMPORT int gtPci0MapMemoryBank1DAC ();
IMPORT int gtPci0MapMemoryBank2 ();
IMPORT int gtPci0MapMemoryBank2DAC ();
IMPORT int gtPci0MapMemoryBank3 ();
IMPORT int gtPci0MapMemoryBank3DAC ();
IMPORT int gtPci0MapP2pIoSpace ();
IMPORT int gtPci0MapP2pMem0Space ();
IMPORT int gtPci0MapP2pMem0SpaceDAC ();
IMPORT int gtPci0MapP2pMem1Space ();
IMPORT int gtPci0MapP2pMem1SpaceDAC ();
IMPORT int gtPci0OverBridgeReadConfigReg ();
IMPORT int gtPci0OverBridgeWriteConfigReg ();
IMPORT int gtPci0P2PConfig ();
IMPORT int gtPci0ParkingDisable ();
IMPORT int gtPci0ScanSelfBars ();
IMPORT int gtPci0SetRegionFeatures ();
IMPORT int gtPci0SetRegionFeaturesDAC ();
IMPORT int gtPciCpuDevNum ();
IMPORT int gtPciGetByte ();
IMPORT int gtPciGetMaxBusNo ();
IMPORT int gtPciGetWord ();
IMPORT int gtPciIntrToVecNum ();
IMPORT int gtPciSetByte ();
IMPORT int gtPciSetWord ();
IMPORT int gtRegReadBlock ();
IMPORT int gtRegWriteBlock ();
IMPORT int gtSramInit ();
IMPORT int gtSramMalloc ();
IMPORT int gtSramMallocAligned ();
IMPORT int gtUartBaudRateChange ();
IMPORT int h ();
IMPORT int hashClassId;
IMPORT int hashFuncIterScale ();
IMPORT int hashFuncModulo ();
IMPORT int hashFuncMultiply ();
IMPORT int hashKeyCmp ();
IMPORT int hashKeyStrCmp ();
IMPORT int hashLibInit ();
IMPORT int hashTblCreate ();
IMPORT int hashTblDelete ();
IMPORT int hashTblDestroy ();
IMPORT int hashTblEach ();
IMPORT int hashTblFind ();
IMPORT int hashTblInit ();
IMPORT int hashTblPut ();
IMPORT int hashTblRemove ();
IMPORT int hashTblTerminate ();
IMPORT int hashinit ();
IMPORT int have_backup__9streambuf ();
IMPORT int have_markers__9streambuf ();
IMPORT int help ();
IMPORT int hex__FR3ios ();
IMPORT int hostAdd ();
IMPORT int hostDelete ();
IMPORT int hostGetByAddr ();
IMPORT int hostGetByName ();
IMPORT int hostInitFlag;
IMPORT int hostList;
IMPORT int hostListSem;
IMPORT int hostName ();
IMPORT int hostShow ();
IMPORT int hostTblInit ();
IMPORT int hostTblSearchByAddr ();
IMPORT int hostTblSearchByName ();
IMPORT int hourr;
IMPORT int i ();
IMPORT int iam ();
IMPORT int ibegin__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int icmpCfgParams;
IMPORT int icmpLibInit ();
IMPORT int icmpMaskGet ();
IMPORT int icmpShowInit ();
IMPORT int icmp_ctloutput ();
IMPORT int icmp_init ();
IMPORT int icmp_input ();
IMPORT int icmpmask;
IMPORT int icmpmaskrepl;
IMPORT int icmpstat;
IMPORT int icmpstatShow ();
IMPORT int id;
IMPORT int iend__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int ifAddrAdd ();
IMPORT int ifAddrDelete ();
IMPORT int ifAddrGet ();
IMPORT int ifAddrSet ();
IMPORT int ifAllRoutesDelete ();
IMPORT int ifBroadcastGet ();
IMPORT int ifBroadcastSet ();
IMPORT int ifDstAddrGet ();
IMPORT int ifDstAddrSet ();
IMPORT int ifFlagChange ();
IMPORT int ifFlagGet ();
IMPORT int ifFlagSet ();
IMPORT int ifIndexAlloc ();
IMPORT int ifIndexLibInit ();
IMPORT int ifIndexLibShutdown ();
IMPORT int ifIndexTest ();
IMPORT int ifIndexToIfName ();
IMPORT int ifIndexToIfp ();
IMPORT int ifMaskGet ();
IMPORT int ifMaskSet ();
IMPORT int ifMetricGet ();
IMPORT int ifMetricSet ();
IMPORT int ifNameToIfIndex ();
IMPORT int ifRouteCleanup ();
IMPORT int ifRouteDelete ();
IMPORT int ifRouteDisable ();
IMPORT int ifRouteEnable ();
IMPORT int ifShow ();
IMPORT int if_attach ();
IMPORT int if_dettach ();
IMPORT int if_down ();
IMPORT int if_qflush ();
IMPORT int if_slowtimo ();
IMPORT int if_slowtimoRestart ();
IMPORT int if_up ();
IMPORT int ifa_ifwithaddr ();
IMPORT int ifa_ifwithaf ();
IMPORT int ifa_ifwithdstaddr ();
IMPORT int ifa_ifwithnet ();
IMPORT int ifa_ifwithroute ();
IMPORT int ifafree ();
IMPORT int ifaof_ifpforaddr ();
IMPORT int ifconf ();
IMPORT int ifinit ();
IMPORT int ifioctl ();
IMPORT int ifnet;
IMPORT int ifpromisc ();
IMPORT int ifreset ();
IMPORT int ifreset2 ();
IMPORT int ifresetImmediate ();
IMPORT int ifunit ();
IMPORT int igmpLibInit ();
IMPORT int igmpShowInit ();
IMPORT int igmp_fasttimo ();
IMPORT int igmp_init ();
IMPORT int igmp_input ();
IMPORT int igmp_slowtimo ();
IMPORT int igmpstat;
IMPORT int igmpstatShow ();
IMPORT int ignore__7istreamii ();
IMPORT int ignore__9streambufi ();
IMPORT int in_addmulti ();
IMPORT int in_avail__9streambuf ();
IMPORT int in_backup__9streambuf ();
IMPORT int in_broadcast ();
IMPORT int in_canforward ();
IMPORT int in_cksum ();
IMPORT int in_control ();
IMPORT int in_delmulti ();
IMPORT int in_iaonnetof ();
IMPORT int in_ifaddr;
IMPORT int in_ifaddr_remove ();
IMPORT int in_ifinit ();
IMPORT int in_ifscrub ();
IMPORT int in_interfaces;
IMPORT int in_lnaof ();
IMPORT int in_localaddr ();
IMPORT int in_losing ();
IMPORT int in_netof ();
IMPORT int in_pcballoc ();
IMPORT int in_pcbbind ();
IMPORT int in_pcbconnect ();
IMPORT int in_pcbdetach ();
IMPORT int in_pcbdisconnect ();
IMPORT int in_pcbinshash ();
IMPORT int in_pcbladdr ();
IMPORT int in_pcblookup ();
IMPORT int in_pcblookuphash ();
IMPORT int in_pcbnotify ();
IMPORT int in_pcbrehash ();
IMPORT int in_rtchange ();
IMPORT int in_setpeeraddr ();
IMPORT int in_setsockaddr ();
IMPORT int in_socktrim ();
IMPORT int index ();
IMPORT int inet_addr ();
IMPORT int inet_aton ();
IMPORT int inet_lnaof ();
IMPORT int inet_makeaddr ();
IMPORT int inet_makeaddr_b ();
IMPORT int inet_netmatch ();
IMPORT int inet_netof ();
IMPORT int inet_netof_string ();
IMPORT int inet_network ();
IMPORT int inet_ntoa ();
IMPORT int inet_ntoa_b ();
IMPORT int inetctlerrmap;
IMPORT int inetdomain;
IMPORT int inetstatShow ();
IMPORT int inetsw;
IMPORT int init__3iosP9streambufP7ostream ();
IMPORT int init__7filebuf ();
IMPORT int initstate ();
IMPORT int input_ipaddr;
IMPORT int insert__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PcUic ();
IMPORT int insert__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Pcc ();
IMPORT int insert__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiPCc ();
IMPORT int insert__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiPCcUi ();
IMPORT int insert__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUi ();
IMPORT int insert__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUic ();
IMPORT int intCnt;
IMPORT int intConnect ();
IMPORT int intConnectHard;
IMPORT int intConnectSoft;
IMPORT int intContext ();
IMPORT int intCount ();
IMPORT int intDisable ();
IMPORT int intDisconnect ();
IMPORT int intEnable ();
IMPORT int intEnt ();
IMPORT int intEvent;
IMPORT int intEventDbg;
IMPORT int intEventIndx;
IMPORT int intExit ();
IMPORT int intLevelSet ();
IMPORT int intLock ();
IMPORT int intLockLevelGet ();
IMPORT int intLockLevelSet ();
IMPORT int intRegsLock ();
IMPORT int intRegsUnlock ();
IMPORT int intRestrict ();
IMPORT int intUnlock ();
IMPORT int intVecBaseGet ();
IMPORT int intVecBaseSet ();
IMPORT int intVecGet ();
IMPORT int intVecSet ();
IMPORT int internalSramTopAddr;
IMPORT int ioDefDevGet ();
IMPORT int ioDefDirGet ();
IMPORT int ioDefPath;
IMPORT int ioDefPathCat ();
IMPORT int ioDefPathGet ();
IMPORT int ioDefPathSet ();
IMPORT int ioFullFileNameGet ();
IMPORT int ioGlobalStdGet ();
IMPORT int ioGlobalStdSet ();
IMPORT int ioHelp ();
IMPORT int ioMaxLinkLevels;
IMPORT int ioTaskStdGet ();
IMPORT int ioTaskStdSet ();
IMPORT int ioTemp;
IMPORT int io_defs__;
IMPORT int ioctl ();
IMPORT int iosClose ();
IMPORT int iosCreate ();
IMPORT int iosDelete ();
IMPORT int iosDevAdd ();
IMPORT int iosDevDelete ();
IMPORT int iosDevFind ();
IMPORT int iosDevShow ();
IMPORT int iosDrvInstall ();
IMPORT int iosDrvRemove ();
IMPORT int iosDrvShow ();
IMPORT int iosDvList;
IMPORT int iosFdDevFind ();
IMPORT int iosFdFree ();
IMPORT int iosFdFreeHookRtn;
IMPORT int iosFdNew ();
IMPORT int iosFdNewHookRtn;
IMPORT int iosFdSet ();
IMPORT int iosFdShow ();
IMPORT int iosFdValue ();
IMPORT int iosInit ();
IMPORT int iosIoctl ();
IMPORT int iosLibInitialized;
IMPORT int iosNextDevGet ();
IMPORT int iosOpen ();
IMPORT int iosRead ();
IMPORT int iosShowInit ();
IMPORT int iosWrite ();
IMPORT int ipAttach ();
IMPORT int ipCfgParams;
IMPORT int ipDetach ();
IMPORT int ipDrvCtrl;
IMPORT int ipEtherResolvRtn ();
IMPORT int ipHeaderCreate ();
IMPORT int ipHeaderVerify ();
IMPORT int ipLibInit ();
IMPORT int ipLibMultiInit ();
IMPORT int ipMaxUnits;
IMPORT int ipMuxCookieGet ();
IMPORT int ipStrongEnded;
IMPORT int ipTkError ();
IMPORT int ipToEtherMCastMap ();
IMPORT int ip_ctloutput ();
IMPORT int ip_defttl;
IMPORT int ip_dooptions ();
IMPORT int ip_drain ();
IMPORT int ip_forward ();
IMPORT int ip_freef ();
IMPORT int ip_freemoptions ();
IMPORT int ip_getmoptions ();
IMPORT int ip_id;
IMPORT int ip_init ();
IMPORT int ip_mloopback ();
IMPORT int ip_nhops;
IMPORT int ip_optcopy ();
IMPORT int ip_output ();
IMPORT int ip_pcbopts ();
IMPORT int ip_protox;
IMPORT int ip_rtaddr ();
IMPORT int ip_slowtimo ();
IMPORT int ip_stripoptions ();
IMPORT int ipforward_rt;
IMPORT int ipfragttl;
IMPORT int ipfx0__7istream ();
IMPORT int ipfx1__7istream ();
IMPORT int ipfx__7istreami ();
IMPORT int ipintr ();
IMPORT int ipintrq;
IMPORT int ipq;
IMPORT int ipqmaxlen;
IMPORT int ipstat;
IMPORT int ipstatShow ();
IMPORT int iptime ();
IMPORT int isIntFlagSet ();
IMPORT int isPMC280BootFlashPresent ();
IMPORT int isPMC280DualCPU ();
IMPORT int isPMC280Monarch ();
IMPORT int isPMC280TwoOnBoardEth ();
IMPORT int isPMC280UsrFlashPresent ();
IMPORT int is_del__t18string_char_traits1Zcc ();
IMPORT int is_open__3ios ();
IMPORT int is_open__C7filebuf ();
IMPORT int is_reading__7filebuf ();
IMPORT int isalnum ();
IMPORT int isalpha ();
IMPORT int isatty ();
IMPORT int iscntrl ();
IMPORT int isdate ();
IMPORT int isday ();
IMPORT int isdigit ();
IMPORT int isfx__7istream ();
IMPORT int isgraph ();
IMPORT int ishour ();
IMPORT int islower ();
IMPORT int ismin ();
IMPORT int ismon ();
IMPORT int isprint ();
IMPORT int ispunct ();
IMPORT int issec ();
IMPORT int isspace ();
IMPORT int istreambuf__C7istream ();
IMPORT int isupper ();
IMPORT int isxdigit ();
IMPORT int isyear ();
IMPORT int kernelInit ();
IMPORT int kernelIsIdle;
IMPORT int kernelState;
IMPORT int kernelTimeSlice ();
IMPORT int kernelVersion ();
IMPORT int kill ();
IMPORT int ksleep ();
IMPORT int l ();
IMPORT int labs ();
IMPORT int lat_h ();
IMPORT int ld ();
IMPORT int ldCommonMatchAll;
IMPORT int ldexp ();
IMPORT int ldiv ();
IMPORT int ldiv_r ();
IMPORT int ledClose ();
IMPORT int ledControl ();
IMPORT int ledId;
IMPORT int ledOpen ();
IMPORT int ledRead ();
IMPORT int length__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int length__t18string_char_traits1ZcPCc ();
IMPORT int lexActions ();
IMPORT int lexClass;
IMPORT int lexNclasses;
IMPORT int lexStateTable;
IMPORT int libiberty_demanglers;
IMPORT int linebuffered__9streambuf ();
IMPORT int linebuffered__9streambufi ();
IMPORT int link_rtrequest ();
IMPORT int linkedCtorsInitialized;
IMPORT int listen ();
IMPORT int lkAddr ();
IMPORT int lkup ();
IMPORT int ll ();
IMPORT int llinfo_arp;
IMPORT int llr ();
IMPORT int loadCommonManage ();
IMPORT int loadCommonMatch ();
IMPORT int loadElfInit ();
IMPORT int loadModule ();
IMPORT int loadModuleAt ();
IMPORT int loadModuleAtSym ();
IMPORT int loadModuleGet ();
IMPORT int loadRoutine;
IMPORT int loadSegmentsAllocate ();
IMPORT int loattach ();
IMPORT int localToGlobalOffset;
IMPORT int localeFiles;
IMPORT int localeconv ();
IMPORT int localtime ();
IMPORT int localtime_r ();
IMPORT int lockKey;
IMPORT int lock__FR7istream ();
IMPORT int lock__FR7ostream ();
IMPORT int log ();
IMPORT int log10 ();
IMPORT int logFdAdd ();
IMPORT int logFdDelete ();
IMPORT int logFdFromRlogin;
IMPORT int logFdSet ();
IMPORT int logInit ();
IMPORT int logMsg ();
IMPORT int logShow ();
IMPORT int logTask ();
IMPORT int logTaskId;
IMPORT int logTaskOptions;
IMPORT int logTaskPriority;
IMPORT int logTaskStackSize;
IMPORT int logb ();
IMPORT int loginVerifyRtn;
IMPORT int logout ();
IMPORT int loif;
IMPORT int longjmp ();
IMPORT int looutput ();
IMPORT int ls ();
IMPORT int lseek ();
IMPORT int lsr ();
IMPORT int lstAdd ();
IMPORT int lstConcat ();
IMPORT int lstCount ();
IMPORT int lstDelete ();
IMPORT int lstExtract ();
IMPORT int lstFind ();
IMPORT int lstFirst ();
IMPORT int lstFree ();
IMPORT int lstGet ();
IMPORT int lstInit ();
IMPORT int lstInsert ();
IMPORT int lstLast ();
IMPORT int lstLibInit ();
IMPORT int lstNStep ();
IMPORT int lstNext ();
IMPORT int lstNth ();
IMPORT int lstPrevious ();
IMPORT int lt__t18string_char_traits1ZcRCcT1 ();
IMPORT int m ();
IMPORT int m2If64BitCounters;
IMPORT int mBufClGet ();
IMPORT int mCastHashInfo;
IMPORT int mCastHashTblSize;
IMPORT int mClBlkConfig;
IMPORT int mHdrClGet ();
IMPORT int mPrivRouteEntryAdd ();
IMPORT int mPrivRouteEntryDelete ();
IMPORT int mRegs ();
IMPORT int mRouteAdd ();
IMPORT int mRouteDelete ();
IMPORT int mRouteEntryAdd ();
IMPORT int mRouteEntryDelete ();
IMPORT int mRouteShow ();
IMPORT int m_adj ();
IMPORT int m_cat ();
IMPORT int m_devget ();
IMPORT int m_getclr ();
IMPORT int m_prepend ();
IMPORT int m_pullup ();
IMPORT int malloc ();
IMPORT int mask_rnhead;
IMPORT int mathFiles;
IMPORT int mathHardInit ();
IMPORT int maxDrivers;
IMPORT int maxFiles;
IMPORT int max_hdr;
IMPORT int max_keylen;
IMPORT int max_linkhdr;
IMPORT int max_protohdr;
IMPORT int max_size__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int maxruns;
IMPORT int mbinit ();
IMPORT int mblen ();
IMPORT int mbstowcs ();
IMPORT int mbtowc ();
IMPORT int mbufShow ();
IMPORT int mcastHashInit ();
IMPORT int mcastHashTblDelete ();
IMPORT int mcastHashTblInsert ();
IMPORT int mcastHashTblLookup ();
IMPORT int memAddToPool ();
IMPORT int memDefaultAlignment;
IMPORT int memFindMax ();
IMPORT int memInit ();
IMPORT int memLibInit ();
IMPORT int memOptionsSet ();
IMPORT int memPartAddToPool ();
IMPORT int memPartAlignedAlloc ();
IMPORT int memPartAlloc ();
IMPORT int memPartAllocErrorRtn;
IMPORT int memPartBlockErrorRtn;
IMPORT int memPartBlockIsValid ();
IMPORT int memPartClassId;
IMPORT int memPartCreate ();
IMPORT int memPartFindMax ();
IMPORT int memPartFree ();
IMPORT int memPartInfoGet ();
IMPORT int memPartInit ();
IMPORT int memPartInstClassId;
IMPORT int memPartLibInit ();
IMPORT int memPartOptionsDefault;
IMPORT int memPartOptionsSet ();
IMPORT int memPartRealloc ();
IMPORT int memPartSemInitRtn;
IMPORT int memPartShow ();
IMPORT int memRead ();
IMPORT int memShow ();
IMPORT int memShowInit ();
IMPORT int memSysPartId;
IMPORT int memTest_Basic ();
IMPORT int memTest_Basic_Ever ();
IMPORT int mem_perf ();
IMPORT int memalign ();
IMPORT int memboundary_check ();
IMPORT int membytes;
IMPORT int memchr ();
IMPORT int memcmp ();
IMPORT int memcpy ();
IMPORT int memmove ();
IMPORT int memoryRead ();
IMPORT int memset ();
IMPORT int memsplit;
IMPORT int memtest_extended ();
IMPORT int memtest_usage ();
IMPORT int mgiEndLoad ();
IMPORT int mib2ErrorAdd ();
IMPORT int mib2Init ();
IMPORT int miiAnCheck ();
IMPORT int miiLibInit ();
IMPORT int miiLibUnInit ();
IMPORT int miiPhyInit ();
IMPORT int miiPhyOptFuncMultiSet ();
IMPORT int miiPhyOptFuncSet ();
IMPORT int miiPhyUnInit ();
IMPORT int minr;
IMPORT int mkdir ();
IMPORT int mktime ();
IMPORT int mmuLibFuncs;
IMPORT int mmuPageBlockSize;
IMPORT int mmuPhysAddrShift;
IMPORT int mmuPpcADisable ();
IMPORT int mmuPpcAEnable ();
IMPORT int mmuPpcBatInit ();
IMPORT int mmuPpcBatInit750fx ();
IMPORT int mmuPpcBatInitMPC74x5 ();
IMPORT int mmuPpcBatInitMPC7x5 ();
IMPORT int mmuPpcDEnabled;
IMPORT int mmuPpcExtendedBlockEnableMPC74x5 ();
IMPORT int mmuPpcExtraBatEnableMPC74x5 ();
IMPORT int mmuPpcExtraBatEnableMPC7x5 ();
IMPORT int mmuPpcExtraBatInit ();
IMPORT int mmuPpcIEnabled;
IMPORT int mmuPpcLibInit ();
IMPORT int mmuPpcSdr1Get ();
IMPORT int mmuPpcSdr1Set ();
IMPORT int mmuPpcSrGet ();
IMPORT int mmuPpcSrSet ();
IMPORT int mmuPpcTlbInvalidateAll ();
IMPORT int mmuPpcTlbie ();
IMPORT int mmuStateTransArray;
IMPORT int mmuStateTransArraySize;
IMPORT int modf ();
IMPORT int moduleCheck ();
IMPORT int moduleClassId;
IMPORT int moduleCreate ();
IMPORT int moduleCreateHookAdd ();
IMPORT int moduleCreateHookDelete ();
IMPORT int moduleDelete ();
IMPORT int moduleEach ();
IMPORT int moduleFindByGroup ();
IMPORT int moduleFindByName ();
IMPORT int moduleFindByNameAndPath ();
IMPORT int moduleFlagsGet ();
IMPORT int moduleIdFigure ();
IMPORT int moduleIdListGet ();
IMPORT int moduleInfoGet ();
IMPORT int moduleInit ();
IMPORT int moduleLibInit ();
IMPORT int moduleNameGet ();
IMPORT int moduleSegAdd ();
IMPORT int moduleSegEach ();
IMPORT int moduleSegFirst ();
IMPORT int moduleSegGet ();
IMPORT int moduleSegNext ();
IMPORT int moduleShow ();
IMPORT int moduleTerminate ();
IMPORT int monarchregfile;
IMPORT int monr;
IMPORT int move__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepUiPCcUi ();
IMPORT int move__t18string_char_traits1ZcPcPCcUi ();
IMPORT int mpp18Initialized;
IMPORT int mpp24Initialized;
IMPORT int mpscChanInit ();
IMPORT int mpscChanSetCdv ();
IMPORT int mpscChanStart ();
IMPORT int mpscChanStopRx ();
IMPORT int mpscChanStopTx ();
IMPORT int mpscChanStopTxRx ();
IMPORT int mpscDbg ();
IMPORT int mpsc_cause_regs_dump ();
IMPORT int mpsc_regs_dump ();
IMPORT int msgQClassId;
IMPORT int msgQCreate ();
IMPORT int msgQDelete ();
IMPORT int msgQDistInfoGetRtn;
IMPORT int msgQDistNumMsgsRtn;
IMPORT int msgQDistReceiveRtn;
IMPORT int msgQDistSendRtn;
IMPORT int msgQDistShowRtn;
IMPORT int msgQEvLibInit ();
IMPORT int msgQEvStart ();
IMPORT int msgQEvStop ();
IMPORT int msgQInfoGet ();
IMPORT int msgQInit ();
IMPORT int msgQInstClassId;
IMPORT int msgQLibInit ();
IMPORT int msgQNumMsgs ();
IMPORT int msgQReceive ();
IMPORT int msgQSend ();
IMPORT int msgQShow ();
IMPORT int msgQShowInit ();
IMPORT int msgQSmInfoGetRtn;
IMPORT int msgQSmNumMsgsRtn;
IMPORT int msgQSmReceiveRtn;
IMPORT int msgQSmSendRtn;
IMPORT int msgQSmShowRtn;
IMPORT int msgQTerminate ();
IMPORT int mutexOptionsHostLib;
IMPORT int mutexOptionsIosLib;
IMPORT int mutexOptionsLogLib;
IMPORT int mutexOptionsMemLib;
IMPORT int mutexOptionsNetDrv;
IMPORT int mutexOptionsNfsDrv;
IMPORT int mutexOptionsSelectLib;
IMPORT int mutexOptionsSymLib;
IMPORT int mutexOptionsTsfsDrv;
IMPORT int mutexOptionsTyLib;
IMPORT int mutexOptionsUnixLib;
IMPORT int mutexOptionsVmBaseLib;
IMPORT int muxAddrResFuncAdd ();
IMPORT int muxAddrResFuncDel ();
IMPORT int muxAddrResFuncGet ();
IMPORT int muxAddressForm ();
IMPORT int muxBibLock;
IMPORT int muxBind ();
IMPORT int muxDevExists ();
IMPORT int muxDevLoad ();
IMPORT int muxDevStart ();
IMPORT int muxDevStartAll ();
IMPORT int muxDevStop ();
IMPORT int muxDevStopAll ();
IMPORT int muxDevUnload ();
IMPORT int muxError ();
IMPORT int muxIoctl ();
IMPORT int muxIterateByName ();
IMPORT int muxLibInit ();
IMPORT int muxLinkHeaderCreate ();
IMPORT int muxMCastAddrAdd ();
IMPORT int muxMCastAddrDel ();
IMPORT int muxMCastAddrGet ();
IMPORT int muxMaxBinds;
IMPORT int muxPacketAddrGet ();
IMPORT int muxPacketDataGet ();
IMPORT int muxPollDevAdd ();
IMPORT int muxPollDevDel ();
IMPORT int muxPollDevStat ();
IMPORT int muxPollEnd ();
IMPORT int muxPollReceive ();
IMPORT int muxPollSend ();
IMPORT int muxPollStart ();
IMPORT int muxPollTask ();
IMPORT int muxReceive ();
IMPORT int muxSend ();
IMPORT int muxShow ();
IMPORT int muxTaskDelayGet ();
IMPORT int muxTaskDelaySet ();
IMPORT int muxTaskPriorityGet ();
IMPORT int muxTaskPrioritySet ();
IMPORT int muxTkBibInit ();
IMPORT int muxTkBibShow ();
IMPORT int muxTkBind ();
IMPORT int muxTkBindUpdate ();
IMPORT int muxTkCookieGet ();
IMPORT int muxTkDebug;
IMPORT int muxTkDevLoadUpdate ();
IMPORT int muxTkDrvCheck ();
IMPORT int muxTkPollReceive ();
IMPORT int muxTkPollReceive2 ();
IMPORT int muxTkPollSend ();
IMPORT int muxTkReceive ();
IMPORT int muxTkSend ();
IMPORT int muxTkUnbindUpdate ();
IMPORT int muxTkUnloadUpdate ();
IMPORT int muxTxRestart ();
IMPORT int muxUnbind ();
IMPORT int mv ();
IMPORT int mvEEPROMRTCWrite ();
IMPORT int mvI2cClearIntFlag ();
IMPORT int mvI2cDelay ();
IMPORT int mvI2cGenerateStartBit ();
IMPORT int mvI2cGenerateStopBit ();
IMPORT int mvI2cIntDisable ();
IMPORT int mvI2cIntEnable ();
IMPORT int mvI2cMasterEEPROMBurstread ();
IMPORT int mvI2cMasterEEPROMread ();
IMPORT int mvI2cMasterEEPROMreadWord ();
IMPORT int mvI2cMasterEEPROMtransmitAddress ();
IMPORT int mvI2cMasterEEPROMwrite ();
IMPORT int mvI2cMasterEEPROMwriteWord ();
IMPORT int mvI2cMasterInit ();
IMPORT int mvI2cMasterRead ();
IMPORT int mvI2cMasterReadBlock ();
IMPORT int mvI2cMasterWrite ();
IMPORT int mvI2cMasterWriteBlock ();
IMPORT int mvI2cReadIntFlag ();
IMPORT int mvI2cReadStatus ();
IMPORT int mvI2cReset ();
IMPORT int mvI2cSetAckBit ();
IMPORT int mvI2cSlaveInit ();
IMPORT int mvI2cTclock;
IMPORT int mvRTCRead ();
IMPORT int mvRTCWrite ();
IMPORT int myfill ();
IMPORT int name__C9type_info ();
IMPORT int namelessPrefix;
IMPORT int ne__t18string_char_traits1ZcRCcT1 ();
IMPORT int netBufLibInit ();
IMPORT int netClBlkFree ();
IMPORT int netClBlkGet ();
IMPORT int netClBlkJoin ();
IMPORT int netClFree ();
IMPORT int netClPoolIdGet ();
IMPORT int netClusterGet ();
IMPORT int netDevCreate ();
IMPORT int netDevCreate2 ();
IMPORT int netDrv ();
IMPORT int netDrvDebugLevelSet ();
IMPORT int netDrvFileDoesNotExist ();
IMPORT int netDrvFileDoesNotExistInstall ();
IMPORT int netErrnoSet ();
IMPORT int netHelp ();
IMPORT int netJobAdd ();
IMPORT int netLibGenInitialized;
IMPORT int netLibGeneralInit ();
IMPORT int netLibInit ();
IMPORT int netLibInitialized;
IMPORT int netLsByName ();
IMPORT int netMblkChainDup ();
IMPORT int netMblkClChainFree ();
IMPORT int netMblkClFree ();
IMPORT int netMblkClGet ();
IMPORT int netMblkClJoin ();
IMPORT int netMblkDup ();
IMPORT int netMblkFree ();
IMPORT int netMblkGet ();
IMPORT int netMblkOffsetToBufCopy ();
IMPORT int netMblkToBufCopy ();
IMPORT int netPoolDelete ();
IMPORT int netPoolInit ();
IMPORT int netPoolKheapInit ();
IMPORT int netPoolShow ();
IMPORT int netShowInit ();
IMPORT int netStackDataPoolShow ();
IMPORT int netStackSysPoolShow ();
IMPORT int netTask ();
IMPORT int netTaskId;
IMPORT int netTaskOptions;
IMPORT int netTaskPriority;
IMPORT int netTaskSemId;
IMPORT int netTaskStackSize;
IMPORT int netTupleGet ();
IMPORT int netTupleGet2 ();
IMPORT int netTypeAdd ();
IMPORT int netTypeDelete ();
IMPORT int netTypeInit ();
IMPORT int netWorkInitStatus;
IMPORT int net_sysctl ();
IMPORT int nextChar__18RBStringIterator_T ();
IMPORT int nextFreeBlkPtr;
IMPORT int nfsAuthUnixGet ();
IMPORT int nfsAuthUnixPrompt ();
IMPORT int nfsAuthUnixSet ();
IMPORT int nfsAuthUnixShow ();
IMPORT int nfsAutoClose;
IMPORT int nfsCacheSize;
IMPORT int nfsClientClose ();
IMPORT int nfsDevInfoGet ();
IMPORT int nfsDevListGet ();
IMPORT int nfsDevShow ();
IMPORT int nfsDirMount ();
IMPORT int nfsDirReadOne ();
IMPORT int nfsDirUnmount ();
IMPORT int nfsDrv ();
IMPORT int nfsDrvNumGet ();
IMPORT int nfsExportFree ();
IMPORT int nfsExportRead ();
IMPORT int nfsExportShow ();
IMPORT int nfsFileAttrGet ();
IMPORT int nfsFileRead ();
IMPORT int nfsFileRemove ();
IMPORT int nfsFileWrite ();
IMPORT int nfsFsAttrGet ();
IMPORT int nfsHelp ();
IMPORT int nfsIdSet ();
IMPORT int nfsLookUpByName ();
IMPORT int nfsMaxMsgLen;
IMPORT int nfsMaxPath;
IMPORT int nfsMntDump ();
IMPORT int nfsMntUnmountAll ();
IMPORT int nfsMount ();
IMPORT int nfsMountAll ();
IMPORT int nfsReXmitSec;
IMPORT int nfsReXmitUSec;
IMPORT int nfsRename ();
IMPORT int nfsSockBufSize;
IMPORT int nfsThingCreate ();
IMPORT int nfsTimeoutSec;
IMPORT int nfsTimeoutUSec;
IMPORT int nfsUnmount ();
IMPORT int non_monarchregfile;
IMPORT int nooffeidevices;
IMPORT int noofgeidevices;
IMPORT int nothrow;
IMPORT int nullObject;
IMPORT int num_flash_dev;
IMPORT int objAlloc ();
IMPORT int objAllocExtra ();
IMPORT int objCoreInit ();
IMPORT int objCoreTerminate ();
IMPORT int objFree ();
IMPORT int objShow ();
IMPORT int oct__FR3ios ();
IMPORT int open ();
IMPORT int open__7filebufPCcT1 ();
IMPORT int open__7filebufPCcii ();
IMPORT int opendir ();
IMPORT int operation_complete ();
IMPORT int operation_fail ();
IMPORT int opfx__7ostream ();
IMPORT int option ();
IMPORT int option1 ();
IMPORT int osfx__7ostream ();
IMPORT int ostreambuf__C7ostream ();
IMPORT int out_waiting__9streambuf ();
IMPORT int overflow__7filebufi ();
IMPORT int overflow__9streambufi ();
IMPORT int pAltivecRegSetObj;
IMPORT int pAltivecTaskIdPrevious;
IMPORT int pEndPktDev;
IMPORT int pFppTaskIdPrevious;
IMPORT int pInPkt;
IMPORT int pJobPool;
IMPORT int pMibRtn;
IMPORT int pRootMemStart;
IMPORT int pSockFdMap;
IMPORT int pTaskLastAltivecTcb;
IMPORT int pTaskLastDspTcb;
IMPORT int pTaskLastFpTcb;
IMPORT int pTcpRandHook;
IMPORT int pTcpstates;
IMPORT int pWdbMemRegions;
IMPORT int pWdbRtIf;
IMPORT int pWvNetEventMap;
IMPORT int padn__9streambufci ();
IMPORT int panic ();
IMPORT int panicSuspend;
IMPORT int parse16 ();
IMPORT int parse8 ();
IMPORT int parse_opts ();
IMPORT int pathBuild ();
IMPORT int pathCat ();
IMPORT int pathCondense ();
IMPORT int pathLastName ();
IMPORT int pathLastNamePtr ();
IMPORT int pathParse ();
IMPORT int pathSplit ();
IMPORT int pause ();
IMPORT int pbackfail__9streambufi ();
IMPORT int pbase__C9streambuf ();
IMPORT int pbump__9streambufi ();
IMPORT int pc ();
IMPORT int pciActive;
IMPORT int pciAutoConfig ();
IMPORT int pciAutoConfigNonMonarch ();
IMPORT int pciBusProbe ();
IMPORT int pciConfigAddr0;
IMPORT int pciConfigAddr1;
IMPORT int pciConfigAddr2;
IMPORT int pciConfigBdfPack ();
IMPORT int pciConfigInByte ();
IMPORT int pciConfigInLong ();
IMPORT int pciConfigInWord ();
IMPORT int pciConfigLibInit ();
IMPORT int pciConfigMech;
IMPORT int pciConfigModifyByte ();
IMPORT int pciConfigModifyLong ();
IMPORT int pciConfigModifyWord ();
IMPORT int pciConfigOutByte ();
IMPORT int pciConfigOutLong ();
IMPORT int pciConfigOutWord ();
IMPORT int pciConfigSpcl;
IMPORT int pciDebug;
IMPORT int pciDevConfig ();
IMPORT int pciFindClass ();
IMPORT int pciFindDevice ();
IMPORT int pciIsr ();
IMPORT int pciLibInitStatus;
IMPORT int pciMaxBus;
IMPORT int pciScan ();
IMPORT int pciSpecialCycle ();
IMPORT int pcicfg0_regs_dump ();
IMPORT int peek__7istream ();
IMPORT int period ();
IMPORT int periodRun ();
IMPORT int perror ();
IMPORT int pfctlinput ();
IMPORT int pffindproto ();
IMPORT int pffindtype ();
IMPORT int phys;
IMPORT int ping ();
IMPORT int pingLibInit ();
IMPORT int pipeDevCreate ();
IMPORT int pipeDevDelete ();
IMPORT int pipeDrv ();
IMPORT int pipeMsgQOptions;
IMPORT int pmPartId;
IMPORT int pmap_getmaps ();
IMPORT int pmap_getmapsInclude ();
IMPORT int pmap_getport ();
IMPORT int pmap_set ();
IMPORT int pmap_unset ();
IMPORT int pmaplist;
IMPORT int portmapd ();
IMPORT int portmapdId;
IMPORT int portmapdOptions;
IMPORT int portmapdPriority;
IMPORT int portmapdStackSize;
IMPORT int pow ();
IMPORT int ppGlobalEnviron;
IMPORT int pptr__C9streambuf ();
IMPORT int precision__3iosi ();
IMPORT int precision__C3ios ();
IMPORT int prepend__10RBString_TPCci ();
IMPORT int prepend__10RBString_TR10RBString_T ();
IMPORT int prepend__10RBString_Tc ();
IMPORT int preset_val;
IMPORT int print64 ();
IMPORT int print64Fine ();
IMPORT int print64Mult ();
IMPORT int print64Row ();
IMPORT int printDramErr ();
IMPORT int printErr ();
IMPORT int printErrno ();
IMPORT int printExc ();
IMPORT int printLogo ();
IMPORT int print_status ();
IMPORT int print_summary ();
IMPORT int print_test_name ();
IMPORT int printf ();
IMPORT int protectBIB;
IMPORT int protectMAC;
IMPORT int protectPCI;
IMPORT int proxyArpHook;
IMPORT int proxyBroadcastHook;
IMPORT int prtMVReg ();
IMPORT int ptyDevCreate ();
IMPORT int ptyDevRemove ();
IMPORT int ptyDrv ();
IMPORT int pubseekoff__9streambuflQ23ios8seek_diri ();
IMPORT int pubseekpos__9streambufli ();
IMPORT int push_back__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0c ();
IMPORT int put__7ostreamSc ();
IMPORT int put__7ostreamUc ();
IMPORT int put__7ostreamc ();
IMPORT int put_mode__9streambuf ();
IMPORT int putback__7istreamc ();
IMPORT int putc ();
IMPORT int putchar ();
IMPORT int putenv ();
IMPORT int puts ();
IMPORT int putw ();
IMPORT int pwd ();
IMPORT int qAdvance ();
IMPORT int qCalibrate ();
IMPORT int qCreate ();
IMPORT int qDelete ();
IMPORT int qEach ();
IMPORT int qFifoClassId;
IMPORT int qFifoCreate ();
IMPORT int qFifoDelete ();
IMPORT int qFifoEach ();
IMPORT int qFifoGet ();
IMPORT int qFifoInfo ();
IMPORT int qFifoInit ();
IMPORT int qFifoPut ();
IMPORT int qFifoRemove ();
IMPORT int qFirst ();
IMPORT int qGet ();
IMPORT int qGetExpired ();
IMPORT int qInfo ();
IMPORT int qInit ();
IMPORT int qJobClassId;
IMPORT int qJobCreate ();
IMPORT int qJobDelete ();
IMPORT int qJobEach ();
IMPORT int qJobGet ();
IMPORT int qJobInfo ();
IMPORT int qJobInit ();
IMPORT int qJobPut ();
IMPORT int qJobTerminate ();
IMPORT int qKey ();
IMPORT int qPriBMapClassId;
IMPORT int qPriBMapCreate ();
IMPORT int qPriBMapDelete ();
IMPORT int qPriBMapEach ();
IMPORT int qPriBMapGet ();
IMPORT int qPriBMapInfo ();
IMPORT int qPriBMapInit ();
IMPORT int qPriBMapKey ();
IMPORT int qPriBMapListCreate ();
IMPORT int qPriBMapListDelete ();
IMPORT int qPriBMapPut ();
IMPORT int qPriBMapRemove ();
IMPORT int qPriBMapResort ();
IMPORT int qPriListAdvance ();
IMPORT int qPriListCalibrate ();
IMPORT int qPriListClassId;
IMPORT int qPriListCreate ();
IMPORT int qPriListDelete ();
IMPORT int qPriListEach ();
IMPORT int qPriListFromTailClassId;
IMPORT int qPriListGet ();
IMPORT int qPriListGetExpired ();
IMPORT int qPriListInfo ();
IMPORT int qPriListInit ();
IMPORT int qPriListKey ();
IMPORT int qPriListPut ();
IMPORT int qPriListPutFromTail ();
IMPORT int qPriListRemove ();
IMPORT int qPriListResort ();
IMPORT int qPriListTerminate ();
IMPORT int qPut ();
IMPORT int qRemove ();
IMPORT int qResort ();
IMPORT int qTerminate ();
IMPORT int qsort ();
IMPORT int rBuffClass;
IMPORT int rBuffClassId;
IMPORT int rBuffCreate ();
IMPORT int rBuffDestroy ();
IMPORT int rBuffFlush ();
IMPORT int rBuffInfoGet ();
IMPORT int rBuffLibInit ();
IMPORT int rBuffNBytes ();
IMPORT int rBuffRead ();
IMPORT int rBuffReadCommit ();
IMPORT int rBuffReadReserve ();
IMPORT int rBuffReset ();
IMPORT int rBuffSetFd ();
IMPORT int rBuffShow ();
IMPORT int rBuffShowInit ();
IMPORT int rBuffShowRtn;
IMPORT int rBuffUpload ();
IMPORT int rBuffVerify ();
IMPORT int rBuffWrite ();
IMPORT int raise ();
IMPORT int ramDevCreate ();
IMPORT int ramDrv ();
IMPORT int ramTest16bit ();
IMPORT int ramTest32bit ();
IMPORT int ramTest8bit ();
IMPORT int ramTestPrintDot ();
IMPORT int rand ();
IMPORT int random ();
IMPORT int rawIpLibInit ();
IMPORT int rawLibInit ();
IMPORT int rbegin__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int rbegin__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int rcmd ();
IMPORT int rdbuf__3iosP9streambuf ();
IMPORT int rdbuf__C3ios ();
IMPORT int rdreg ();
IMPORT int rdstate__C3ios ();
IMPORT int read ();
IMPORT int read__7istreamPSci ();
IMPORT int read__7istreamPUci ();
IMPORT int read__7istreamPci ();
IMPORT int read__7istreamPvi ();
IMPORT int readable__3ios ();
IMPORT int readdir ();
IMPORT int readreg ();
IMPORT int readv ();
IMPORT int readyQBMap;
IMPORT int readyQHead;
IMPORT int realloc ();
IMPORT int reallocate__t23__malloc_alloc_template1i0PvUiUi ();
IMPORT int reallocate__t24__default_alloc_template2b1i0PvUiUi ();
IMPORT int reboot ();
IMPORT int rebootHookAdd ();
IMPORT int recv ();
IMPORT int recvfrom ();
IMPORT int recvmsg ();
IMPORT int redirInFd;
IMPORT int redirOutFd;
IMPORT int reg_service ();
IMPORT int reginittab;
IMPORT int registerrpc ();
IMPORT int rejected ();
IMPORT int reld ();
IMPORT int release__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3Rep ();
IMPORT int release_mem ();
IMPORT int remCacheControlBits ();
IMPORT int remCurIdGet ();
IMPORT int remCurIdSet ();
IMPORT int remLastResvPort;
IMPORT int remLibInit ();
IMPORT int remStdErrSetupTimeout;
IMPORT int remove ();
IMPORT int rename ();
IMPORT int rend__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int rend__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int rep__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int repeat ();
IMPORT int repeatRun ();
IMPORT int replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PcT1PCc ();
IMPORT int replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PcT1PCcUi ();
IMPORT int replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PcT1RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PcT1Uic ();
IMPORT int replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUiPCc ();
IMPORT int replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUiPCcUi ();
IMPORT int replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUiRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUi ();
IMPORT int replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUiUic ();
IMPORT int replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUic ();
IMPORT int repup__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PQ2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3Rep ();
IMPORT int reschedule ();
IMPORT int reserve__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui ();
IMPORT int resize__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui ();
IMPORT int resize__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Uic ();
IMPORT int restartTaskName;
IMPORT int restartTaskOptions;
IMPORT int restartTaskPriority;
IMPORT int restartTaskStackSize;
IMPORT int rewind ();
IMPORT int rewinddir ();
IMPORT int rfind__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi ();
IMPORT int rfind__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUiUi ();
IMPORT int rfind__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui ();
IMPORT int rfind__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0cUi ();
IMPORT int rindex ();
IMPORT int rip_ctloutput ();
IMPORT int rip_init ();
IMPORT int rip_input ();
IMPORT int rip_output ();
IMPORT int rip_recvspace;
IMPORT int rip_sendspace;
IMPORT int rip_usrreq ();
IMPORT int rlogChildTask ();
IMPORT int rlogChildTaskId;
IMPORT int rlogInTask ();
IMPORT int rlogInTaskId;
IMPORT int rlogInit ();
IMPORT int rlogOutTask ();
IMPORT int rlogOutTaskId;
IMPORT int rlogShellName;
IMPORT int rlogTaskOptions;
IMPORT int rlogTaskPriority;
IMPORT int rlogTaskStackSize;
IMPORT int rlogTermType;
IMPORT int rlogin ();
IMPORT int rlogind ();
IMPORT int rlogindId;
IMPORT int rlogindSocket;
IMPORT int rm ();
IMPORT int rmdir ();
IMPORT int rn_addmask ();
IMPORT int rn_addroute ();
IMPORT int rn_delete ();
IMPORT int rn_destroyhead ();
IMPORT int rn_init ();
IMPORT int rn_inithead ();
IMPORT int rn_insert ();
IMPORT int rn_lookup ();
IMPORT int rn_match ();
IMPORT int rn_mkfreelist;
IMPORT int rn_newpair ();
IMPORT int rn_refines ();
IMPORT int rn_search ();
IMPORT int rn_search_m ();
IMPORT int rn_walksubtree ();
IMPORT int rn_walktree ();
IMPORT int rngBufGet ();
IMPORT int rngBufPut ();
IMPORT int rngCreate ();
IMPORT int rngDelete ();
IMPORT int rngFlush ();
IMPORT int rngFreeBytes ();
IMPORT int rngIsEmpty ();
IMPORT int rngIsFull ();
IMPORT int rngMoveAhead ();
IMPORT int rngNBytes ();
IMPORT int rngPutAhead ();
IMPORT int rootMemNBytes;
IMPORT int rootTaskId;
IMPORT int roundRobinOn;
IMPORT int roundRobinSlice;
IMPORT int routeAdd ();
IMPORT int routeAgeSet ();
IMPORT int routeCmd ();
IMPORT int routeDelete ();
IMPORT int routeDrain ();
IMPORT int routeEntryFill ();
IMPORT int routeMetricSet ();
IMPORT int routeNetAdd ();
IMPORT int routeShow ();
IMPORT int routeSwap ();
IMPORT int route_init ();
IMPORT int routestatShow ();
IMPORT int rpcClntErrnoSet ();
IMPORT int rpcErrnoGet ();
IMPORT int rpcInit ();
IMPORT int rpcTaskInit ();
IMPORT int rresvport ();
IMPORT int rtIfaceMsgHook;
IMPORT int rtMem;
IMPORT int rtMissMsgHook;
IMPORT int rtNewAddrMsgHook;
IMPORT int rt_fixdelete ();
IMPORT int rt_maskedcopy ();
IMPORT int rt_setgate ();
IMPORT int rt_tables;
IMPORT int rtable_init ();
IMPORT int rtalloc ();
IMPORT int rtalloc1 ();
IMPORT int rtccall ();
IMPORT int rtfree ();
IMPORT int rtinit ();
IMPORT int rtioctl ();
IMPORT int rtmodified;
IMPORT int rtredirect ();
IMPORT int rtrequest ();
IMPORT int rtrequestAddEqui ();
IMPORT int rtrequestDelEqui ();
IMPORT int rtstat;
IMPORT int rttrash;
IMPORT int run;
IMPORT int runerrors;
IMPORT int runtimeName;
IMPORT int runtimeVersion;
IMPORT int rxfei0counter;
IMPORT int rxfei1counter;
IMPORT int s ();
IMPORT int s1;
IMPORT int s2;
IMPORT int s29ALBotOffset;
IMPORT int s29ALTopOffset;
IMPORT int saving__12streammarker ();
IMPORT int sb_lock ();
IMPORT int sb_max;
IMPORT int sbappend ();
IMPORT int sbappendaddr ();
IMPORT int sbappendcontrol ();
IMPORT int sbappendrecord ();
IMPORT int sbcompress ();
IMPORT int sbdrop ();
IMPORT int sbdroprecord ();
IMPORT int sbflush ();
IMPORT int sbinsertoob ();
IMPORT int sbrelease ();
IMPORT int sbreserve ();
IMPORT int sbseldequeue ();
IMPORT int sbselqueue ();
IMPORT int sbumpc__9streambuf ();
IMPORT int sbwait ();
IMPORT int sbwakeup ();
IMPORT int scalb ();
IMPORT int scanCharSet ();
IMPORT int scanField ();
IMPORT int scan__7istreamPCce ();
IMPORT int scan__9streambufPCce ();
IMPORT int scanf ();
IMPORT int sdmaChanInit ();
IMPORT int sdmaChanReceive ();
IMPORT int sdmaChanSend ();
IMPORT int sdmaChanStart ();
IMPORT int sdmaChanStopRx ();
IMPORT int sdmaChanStopTx ();
IMPORT int sdmaChanStopTxRx ();
IMPORT int sdmaDbg ();
IMPORT int sdmaRxReturnBuff ();
IMPORT int sdmaTxReturnDesc ();
IMPORT int sdramSize;
IMPORT int secr;
IMPORT int seekg__7istreaml ();
IMPORT int seekg__7istreamlQ23ios8seek_dir ();
IMPORT int seekmark__9streambufR12streammarkeri ();
IMPORT int seekoff__7filebuflQ23ios8seek_diri ();
IMPORT int seekoff__9streambuflQ23ios8seek_diri ();
IMPORT int seekp__7ostreaml ();
IMPORT int seekp__7ostreamlQ23ios8seek_dir ();
IMPORT int seekpos__9streambufli ();
IMPORT int selNodeAdd ();
IMPORT int selNodeDelete ();
IMPORT int selTaskDeleteHookAdd ();
IMPORT int selWakeup ();
IMPORT int selWakeupAll ();
IMPORT int selWakeupListInit ();
IMPORT int selWakeupListLen ();
IMPORT int selWakeupListTerm ();
IMPORT int selWakeupType ();
IMPORT int select ();
IMPORT int selectInit ();
IMPORT int selfish__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int semBCoreInit ();
IMPORT int semBCreate ();
IMPORT int semBGive ();
IMPORT int semBGiveDefer ();
IMPORT int semBInit ();
IMPORT int semBLibInit ();
IMPORT int semBTake ();
IMPORT int semCCoreInit ();
IMPORT int semCCreate ();
IMPORT int semCGive ();
IMPORT int semCGiveDefer ();
IMPORT int semCInit ();
IMPORT int semCLibInit ();
IMPORT int semCTake ();
IMPORT int semClass;
IMPORT int semClassId;
IMPORT int semDelete ();
IMPORT int semDestroy ();
IMPORT int semEvIsFreeTbl;
IMPORT int semEvLibInit ();
IMPORT int semEvStart ();
IMPORT int semEvStop ();
IMPORT int semFlush ();
IMPORT int semFlushDefer ();
IMPORT int semFlushDeferTbl;
IMPORT int semFlushTbl;
IMPORT int semGive ();
IMPORT int semGiveDefer ();
IMPORT int semGiveDeferTbl;
IMPORT int semGiveTbl;
IMPORT int semInfo ();
IMPORT int semInstClass;
IMPORT int semInstClassId;
IMPORT int semIntRestrict ();
IMPORT int semInvalid ();
IMPORT int semLibInit ();
IMPORT int semMCoreInit ();
IMPORT int semMCreate ();
IMPORT int semMGive ();
IMPORT int semMGiveForce ();
IMPORT int semMGiveKern ();
IMPORT int semMGiveKernWork;
IMPORT int semMInit ();
IMPORT int semMLibInit ();
IMPORT int semMPendQPut ();
IMPORT int semMTake ();
IMPORT int semQFlush ();
IMPORT int semQFlushDefer ();
IMPORT int semQInit ();
IMPORT int semShow ();
IMPORT int semShowInit ();
IMPORT int semSmInfoRtn;
IMPORT int semSmShowRtn;
IMPORT int semTake ();
IMPORT int semTakeTbl;
IMPORT int semTerminate ();
IMPORT int send ();
IMPORT int sendmsg ();
IMPORT int sendto ();
IMPORT int serialChanInit ();
IMPORT int serialChanSetBaudRate ();
IMPORT int serialChanStart ();
IMPORT int serialChanStop ();
IMPORT int setI2cBusy ();
IMPORT int setI2cReady ();
IMPORT int set__3iosi ();
IMPORT int set__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepUicUi ();
IMPORT int set__t18string_char_traits1ZcPcRCcUi ();
IMPORT int set_column__9streambufi ();
IMPORT int set_cplus_marker_for_demangling ();
IMPORT int set_if_addr ();
IMPORT int set_new_handler__FPFv_v ();
IMPORT int set_offset__12streammarkeri ();
IMPORT int set_terminate__FPFv_v ();
IMPORT int set_unexpected__FPFv_v ();
IMPORT int setb__9streambufPcT1i ();
IMPORT int setbuf ();
IMPORT int setbuf__7filebufPci ();
IMPORT int setbuf__9streambufPci ();
IMPORT int setbuffer ();
IMPORT int setf__3iosUl ();
IMPORT int setf__3iosUlUl ();
IMPORT int setg__9streambufPcN21 ();
IMPORT int sethostname ();
IMPORT int setjmp ();
IMPORT int setlinebuf ();
IMPORT int setlocale ();
IMPORT int setp__9streambufPcT1 ();
IMPORT int setsockopt ();
IMPORT int setstate ();
IMPORT int setstate__3iosi ();
IMPORT int settime ();
IMPORT int setvbuf ();
IMPORT int sfpDoubleNormalize ();
IMPORT int sfpDoubleNormalize2 ();
IMPORT int sfpFloatNormalize ();
IMPORT int sgetc__9streambuf ();
IMPORT int sgetn__9streambufPci ();
IMPORT int shell ();
IMPORT int shellHistSize;
IMPORT int shellHistory ();
IMPORT int shellInit ();
IMPORT int shellIsRemoteConnectedGet ();
IMPORT int shellIsRemoteConnectedSet ();
IMPORT int shellLock ();
IMPORT int shellLogin ();
IMPORT int shellLoginInstall ();
IMPORT int shellLogout ();
IMPORT int shellLogoutInstall ();
IMPORT int shellOrigStdSet ();
IMPORT int shellPromptSet ();
IMPORT int shellRestart ();
IMPORT int shellScriptAbort ();
IMPORT int shellTaskId;
IMPORT int shellTaskName;
IMPORT int shellTaskOptions;
IMPORT int shellTaskPriority;
IMPORT int shellTaskStackSize;
IMPORT int shortbuf__9streambuf ();
IMPORT int show ();
IMPORT int showMap ();
IMPORT int shutdown ();
IMPORT int sigEvtRtn;
IMPORT int sigInit ();
IMPORT int sigPendDestroy ();
IMPORT int sigPendInit ();
IMPORT int sigPendKill ();
IMPORT int sigaction ();
IMPORT int sigaddset ();
IMPORT int sigblock ();
IMPORT int sigdelset ();
IMPORT int sigemptyset ();
IMPORT int sigfillset ();
IMPORT int sigismember ();
IMPORT int signal ();
IMPORT int sigpending ();
IMPORT int sigprocmask ();
IMPORT int sigqueue ();
IMPORT int sigqueueInit ();
IMPORT int sigreturn ();
IMPORT int sigsetjmp ();
IMPORT int sigsetmask ();
IMPORT int sigsuspend ();
IMPORT int sigtimedwait ();
IMPORT int sigvec ();
IMPORT int sigwait ();
IMPORT int sigwaitinfo ();
IMPORT int silent_test;
IMPORT int sin ();
IMPORT int sinh ();
IMPORT int size__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int skip__7istreami ();
IMPORT int skip_ws__FP9streambuf ();
IMPORT int sllCount ();
IMPORT SL_LIST *sllCreate ();
IMPORT int sllDelete ();
IMPORT SL_NODE *sllEach ();
IMPORT SL_NODE *sllGet ();
IMPORT int sllInit ();
IMPORT SL_NODE *sllPrevious ();
IMPORT void sllPutAtHead ();
IMPORT void sllPutAtTail ();
IMPORT void sllRemove ();
IMPORT int sllTerminate ();
IMPORT int smMemPartAddToPoolRtn;
IMPORT int smMemPartAllocRtn;
IMPORT int smMemPartFindMaxRtn;
IMPORT int smMemPartFreeRtn;
IMPORT int smMemPartOptionsSetRtn;
IMPORT int smMemPartReallocRtn;
IMPORT int smMemPartShowRtn;
IMPORT int smObjPoolMinusOne;
IMPORT int smObjTaskDeleteFailRtn;
IMPORT int smObjTcbFreeFailRtn;
IMPORT int smObjTcbFreeRtn;
IMPORT int snextc__9streambuf ();
IMPORT int snoopEnable;
IMPORT int sntpcInit ();
IMPORT int sntpcPort;
IMPORT int sntpcTimeGet ();
IMPORT int so ();
IMPORT int soabort ();
IMPORT int soaccept ();
IMPORT int sobind ();
IMPORT int socantrcvmore ();
IMPORT int socantsendmore ();
IMPORT int sockFdtosockFunc ();
IMPORT int sockLibAdd ();
IMPORT int sockLibInit ();
IMPORT int socket ();
IMPORT int soclose ();
IMPORT int soconnect ();
IMPORT int soconnect2 ();
IMPORT int socreate ();
IMPORT int sodisconnect ();
IMPORT int sofree ();
IMPORT int sogetopt ();
IMPORT int sohasoutofband ();
IMPORT int soisconnected ();
IMPORT int soisconnecting ();
IMPORT int soisdisconnected ();
IMPORT int soisdisconnecting ();
IMPORT int solisten ();
IMPORT int somaxconn;
IMPORT int sonewconn1 ();
IMPORT int soo_ioctl ();
IMPORT int soo_select ();
IMPORT int soo_unselect ();
IMPORT int soqinsque ();
IMPORT int soqremque ();
IMPORT int soreceive ();
IMPORT int soreserve ();
IMPORT int sorflush ();
IMPORT int sosend ();
IMPORT int sosetopt ();
IMPORT int soshutdown ();
IMPORT int sowakeup ();
IMPORT int sowakeupHook;
IMPORT int sp ();
IMPORT int spTaskOptions;
IMPORT int spTaskPriority;
IMPORT int spTaskStackSize;
IMPORT int splSemId;
IMPORT int splSemInit ();
IMPORT int splTid;
IMPORT int splimp ();
IMPORT int splnet ();
IMPORT int splnet2 ();
IMPORT int splx ();
IMPORT int sprintf ();
IMPORT int sputbackc__9streambufc ();
IMPORT int sputc__9streambufi ();
IMPORT int sputn__9streambufPCci ();
IMPORT int spy ();
IMPORT int spyClkStart ();
IMPORT int spyClkStop ();
IMPORT int spyHelp ();
IMPORT int spyReport ();
IMPORT int spyStop ();
IMPORT int spyTask ();
IMPORT int sqrt ();
IMPORT int srand ();
IMPORT int srandom ();
IMPORT int sscanf ();
IMPORT int sseekoff__9streambuflQ23ios8seek_diri ();
IMPORT int sseekpos__9streambufli ();
IMPORT int standAloneSymTbl;
IMPORT int start_addr;
IMPORT int stat ();
IMPORT int statSymTbl;
IMPORT int statTbl;
IMPORT int statTblSize;
IMPORT int statfs ();
IMPORT int stdioFiles;
IMPORT int stdioFp ();
IMPORT int stdioFpCreate ();
IMPORT int stdioFpDestroy ();
IMPORT int stdioInit ();
IMPORT int stdioShow ();
IMPORT int stdioShowInit ();
IMPORT int stdlibFiles;
IMPORT int stossc__9streambuf ();
IMPORT int strcat ();
IMPORT int strchr ();
IMPORT int strcmp ();
IMPORT int strcoll ();
IMPORT int strcpy ();
IMPORT int strcspn ();
IMPORT int strerror ();
IMPORT int strerror_r ();
IMPORT int strftime ();
IMPORT int stringFiles;
IMPORT int strlen ();
IMPORT int strncat ();
IMPORT int strncmp ();
IMPORT int strncpy ();
IMPORT int strpbrk ();
IMPORT int strrchr ();
IMPORT int strspn ();
IMPORT int strstr ();
IMPORT int strtod ();
IMPORT int strtok ();
IMPORT int strtok_r ();
IMPORT int strtol ();
IMPORT int strtoul ();
IMPORT int strxfrm ();
IMPORT int subnetsarelocal;
IMPORT int substr__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUi ();
IMPORT int substrcmp ();
IMPORT int sungetc__9streambuf ();
IMPORT int svc_find ();
IMPORT int svc_getreq ();
IMPORT int svc_getreqset ();
IMPORT int svc_rawInclude ();
IMPORT int svc_register ();
IMPORT int svc_run ();
IMPORT int svc_sendreply ();
IMPORT int svc_simpleInclude ();
IMPORT int svc_unregister ();
IMPORT int svcerr_auth ();
IMPORT int svcerr_decode ();
IMPORT int svcerr_noproc ();
IMPORT int svcerr_noprog ();
IMPORT int svcerr_progvers ();
IMPORT int svcerr_systemerr ();
IMPORT int svcerr_weakauth ();
IMPORT int svcfd_create ();
IMPORT int svcraw_create ();
IMPORT int svctcp_create ();
IMPORT int svcudp_bufcreate ();
IMPORT int svcudp_create ();
IMPORT int svcudp_enablecache ();
IMPORT int swab ();
IMPORT int swap__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Rt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int switch_to_get_mode__9streambuf ();
IMPORT int symAdd ();
IMPORT int symAlloc ();
IMPORT int symByCNameFind ();
IMPORT int symByValueAndTypeFind ();
IMPORT int symByValueFind ();
IMPORT int symEach ();
IMPORT int symFindByCName ();
IMPORT int symFindByName ();
IMPORT int symFindByNameAndType ();
IMPORT int symFindByValue ();
IMPORT int symFindByValueAndType ();
IMPORT int symFindSymbol ();
IMPORT int symFree ();
IMPORT int symGroupDefault;
IMPORT int symInit ();
IMPORT int symLibInit ();
IMPORT int symLkupPgSz;
IMPORT int symName ();
IMPORT int symNameGet ();
IMPORT int symRemove ();
IMPORT int symSAdd ();
IMPORT int symShow ();
IMPORT int symShowInit ();
IMPORT int symTblAdd ();
IMPORT int symTblClassId;
IMPORT int symTblCreate ();
IMPORT int symTblDelete ();
IMPORT int symTblDestroy ();
IMPORT int symTblInit ();
IMPORT int symTblRemove ();
IMPORT int symTblTerminate ();
IMPORT int symTypeGet ();
IMPORT int symValueGet ();
IMPORT int syncLoadRtn;
IMPORT int syncSymAddRtn;
IMPORT int syncSymRemoveRtn;
IMPORT int syncUnldRtn;
IMPORT int sync__7filebuf ();
IMPORT int sync__7istream ();
IMPORT int sync__9streambuf ();
IMPORT int sync_with_stdio__3ios ();
IMPORT int sync_with_stdio__3iosi ();
IMPORT int sys543PciInit ();
IMPORT int sys543Show ();
IMPORT int sys557IntAck ();
IMPORT int sys557IntDisable ();
IMPORT int sys557IntEnable ();
IMPORT int sys557PciInit ();
IMPORT int sys557Show ();
IMPORT int sys557eepromRead ();
IMPORT int sys557mdioRead ();
IMPORT int sys557mdioWrite ();
IMPORT int sys82543BoardInit ();
IMPORT int sysAdaEnable;
IMPORT int sysAltivecProbe ();
IMPORT int sysBatDesc;
IMPORT int sysBootFile;
IMPORT int sysBootHost;
IMPORT int sysBootLine;
IMPORT int sysBootParams;
IMPORT int sysBspRev ();
IMPORT int sysBus;
IMPORT int sysBusToLocalAdrs ();
IMPORT int sysClDescTbl;
IMPORT int sysClDescTblNumEnt;
IMPORT int sysClkConnect ();
IMPORT int sysClkDisable ();
IMPORT int sysClkEnable ();
IMPORT int sysClkRateGet ();
IMPORT int sysClkRateSet ();
IMPORT int sysCplusEnable;
IMPORT int sysCpu;
IMPORT int sysCpuCheck ();
IMPORT int sysDelay ();
IMPORT int sysExcMsg;
IMPORT int sysFlags;
IMPORT int sysGetBusSpd ();
IMPORT int sysGetCpuRev ();
IMPORT int sysGetCpuVer ();
IMPORT int sysGetL2CR ();
IMPORT int sysGetL3CR ();
IMPORT int sysGetPrid ();
IMPORT int sysGetSDR1 ();
IMPORT int sysHardReset ();
IMPORT int sysHwInit ();
IMPORT int sysHwInit2 ();
IMPORT int sysInByte ();
IMPORT int sysInLong ();
IMPORT int sysInWord ();
IMPORT int sysIntCtrlInit ();
IMPORT int sysIntEnable ();
IMPORT int sysL1DcacheDisable ();
IMPORT int sysL1DcacheEnable ();
IMPORT int sysL2BackDisable ();
IMPORT int sysL2BackEnable ();
IMPORT int sysL2BackGlobalInv ();
IMPORT int sysL2BackHWFlush ();
IMPORT int sysL2BackInit ();
IMPORT int sysL2BackSWFlush ();
IMPORT int sysL2BackSetWT ();
IMPORT int sysL2crGet ();
IMPORT int sysL2crPut ();
IMPORT int sysMachChkExcpHand ();
IMPORT int sysMachChkExcpInit ();
IMPORT int sysMclBlkConfig;
IMPORT int sysMemProbeSup ();
IMPORT int sysMemTop ();
IMPORT int sysMmuMapAdd ();
IMPORT int sysMmuPteShow ();
IMPORT int sysModel ();
IMPORT int sysMsDelay ();
IMPORT int sysNvRamGet ();
IMPORT int sysNvRamSet ();
IMPORT int sysOutByte ();
IMPORT int sysOutLong ();
IMPORT int sysOutWord ();
IMPORT int sysPci16Swap ();
IMPORT int sysPci32Swap ();
IMPORT int sysPciConfigInsertLong ();
IMPORT int sysPhysMemDesc;
IMPORT int sysPhysMemDescNumEnt;
IMPORT int sysPhysMemTop ();
IMPORT int sysPhysToVirt ();
IMPORT int sysProcNum;
IMPORT int sysProcNumGet ();
IMPORT int sysProcNumSet ();
IMPORT int sysRamDrvConfig ();
IMPORT int sysRdCpuReg ();
IMPORT int sysSerialChanGet ();
IMPORT int sysSerialHwInit ();
IMPORT int sysSerialHwInit2 ();
IMPORT int sysSioRead ();
IMPORT int sysSioWrite ();
IMPORT int sysStartType;
IMPORT int sysSymTbl;
IMPORT int sysTimeBaseLGet ();
IMPORT int sysTimeBaseLPut ();
IMPORT int sysTimestamp ();
IMPORT int sysTimestampConnect ();
IMPORT int sysTimestampDisable ();
IMPORT int sysTimestampEnable ();
IMPORT int sysTimestampFreq ();
IMPORT int sysTimestampLock ();
IMPORT int sysTimestampPeriod ();
IMPORT int sysToMonitor ();
IMPORT int sysUsDelay ();
IMPORT int sysUserLEDSet ();
IMPORT int sysVirtToPhys ();
IMPORT int sys_close__7filebuf ();
IMPORT int sys_close__9streambuf ();
IMPORT int sys_read__7filebufPci ();
IMPORT int sys_read__9streambufPci ();
IMPORT int sys_seek__7filebuflQ23ios8seek_dir ();
IMPORT int sys_seek__9streambuflQ23ios8seek_dir ();
IMPORT int sys_stat__7filebufPv ();
IMPORT int sys_stat__9streambufPv ();
IMPORT int sys_write__7filebufPCci ();
IMPORT int sys_write__9streambufPCci ();
IMPORT int system ();
IMPORT int tan ();
IMPORT int tanh ();
IMPORT int targetName ();
IMPORT int taskActivate ();
IMPORT int taskArgsGet ();
IMPORT int taskArgsSet ();
IMPORT int taskBpHook;
IMPORT int taskBpHookSet ();
IMPORT int taskClassId;
IMPORT int taskCreat ();
IMPORT int taskCreateHookAdd ();
IMPORT int taskCreateHookDelete ();
IMPORT int taskCreateHookShow ();
IMPORT int taskCreateTable;
IMPORT int taskDelay ();
IMPORT int taskDelete ();
IMPORT int taskDeleteForce ();
IMPORT int taskDeleteHookAdd ();
IMPORT int taskDeleteHookDelete ();
IMPORT int taskDeleteHookShow ();
IMPORT int taskDeleteTable;
IMPORT int taskDestroy ();
IMPORT int taskHookInit ();
IMPORT int taskHookShowInit ();
IMPORT int taskIdCurrent;
IMPORT int taskIdDefault ();
IMPORT int taskIdFigure ();
IMPORT int taskIdListGet ();
IMPORT int taskIdListSort ();
IMPORT int taskIdSelf ();
IMPORT int taskIdVerify ();
IMPORT int taskInfoGet ();
IMPORT int taskInit ();
IMPORT int taskInstClassId;
IMPORT int taskIsReady ();
IMPORT int taskIsSuspended ();
IMPORT int taskLibInit ();
IMPORT int taskLock ();
IMPORT int taskMsrDefault;
IMPORT int taskMsrSet ();
IMPORT int taskName ();
IMPORT int taskNameToId ();
IMPORT int taskOptionsGet ();
IMPORT int taskOptionsSet ();
IMPORT int taskOptionsString ();
IMPORT int taskPriRangeCheck;
IMPORT int taskPriorityGet ();
IMPORT int taskPrioritySet ();
IMPORT int taskRegName;
IMPORT int taskRegsFmt;
IMPORT int taskRegsGet ();
IMPORT int taskRegsInit ();
IMPORT int taskRegsSet ();
IMPORT int taskRegsShow ();
IMPORT int taskRestart ();
IMPORT int taskResume ();
IMPORT int taskRtnValueSet ();
IMPORT int taskSafe ();
IMPORT int taskShow ();
IMPORT int taskShowInit ();
IMPORT int taskSpawn ();
IMPORT int taskStackAllot ();
IMPORT int taskStatusString ();
IMPORT int taskSuspend ();
IMPORT int taskSwapHookAdd ();
IMPORT int taskSwapHookAttach ();
IMPORT int taskSwapHookDelete ();
IMPORT int taskSwapHookDetach ();
IMPORT int taskSwapHookShow ();
IMPORT int taskSwapReference;
IMPORT int taskSwapTable;
IMPORT int taskSwitchHookAdd ();
IMPORT int taskSwitchHookDelete ();
IMPORT int taskSwitchHookShow ();
IMPORT int taskSwitchTable;
IMPORT int taskTcb ();
IMPORT int taskTerminate ();
IMPORT int taskUndelay ();
IMPORT int taskUnlock ();
IMPORT int taskUnsafe ();
IMPORT int taskVarAdd ();
IMPORT int taskVarDelete ();
IMPORT int taskVarGet ();
IMPORT int taskVarInfo ();
IMPORT int taskVarInit ();
IMPORT int taskVarSet ();
IMPORT int tbTempHi;
IMPORT int tbTempLo;
IMPORT int tcbinfo;
IMPORT int tcpCfgParams;
IMPORT int tcpDebugShow ();
IMPORT int tcpLibInit ();
IMPORT int tcpOutRsts;
IMPORT int tcpRandHookAdd ();
IMPORT int tcpRandHookDelete ();
IMPORT int tcpReportRtn;
IMPORT int tcpShowInit ();
IMPORT int tcpTraceRtn;
IMPORT int tcp_alpha;
IMPORT int tcp_attach ();
IMPORT int tcp_backoff;
IMPORT int tcp_beta;
IMPORT int tcp_canceltimers ();
IMPORT int tcp_close ();
IMPORT int tcp_ctlinput ();
IMPORT int tcp_ctloutput ();
IMPORT int tcp_disconnect ();
IMPORT int tcp_do_rfc1323;
IMPORT int tcp_dooptions ();
IMPORT int tcp_drain ();
IMPORT int tcp_drop ();
IMPORT int tcp_fasttimo ();
IMPORT int tcp_init ();
IMPORT int tcp_input ();
IMPORT int tcp_iss;
IMPORT int tcp_keepcnt;
IMPORT int tcp_keepidle;
IMPORT int tcp_keepinit;
IMPORT int tcp_keepintvl;
IMPORT int tcp_last_inpcb;
IMPORT int tcp_maxidle;
IMPORT int tcp_maxpersistidle;
IMPORT int tcp_msl;
IMPORT int tcp_mss ();
IMPORT int tcp_mssdflt;
IMPORT int tcp_newtcpcb ();
IMPORT int tcp_notify ();
IMPORT int tcp_now;
IMPORT int tcp_outflags;
IMPORT int tcp_output ();
IMPORT int tcp_pcbhashsize;
IMPORT int tcp_pulloutofband ();
IMPORT int tcp_quench ();
IMPORT int tcp_reass ();
IMPORT int tcp_recvspace;
IMPORT int tcp_respond ();
IMPORT int tcp_rtlookup ();
IMPORT int tcp_rttdflt;
IMPORT int tcp_saveti;
IMPORT int tcp_sendspace;
IMPORT int tcp_setpersist ();
IMPORT int tcp_slowtimo ();
IMPORT int tcp_template ();
IMPORT int tcp_timers ();
IMPORT int tcp_totbackoff;
IMPORT int tcp_updatemtu ();
IMPORT int tcp_usrclosed ();
IMPORT int tcp_usrreq ();
IMPORT int tcp_xmit_timer ();
IMPORT int tcpcb;
IMPORT int tcprexmtthresh;
IMPORT int tcpstat;
IMPORT int tcpstatShow ();
IMPORT int tcpstates;
IMPORT int td ();
IMPORT int tellg__7istream ();
IMPORT int tellp__7ostream ();
IMPORT int terminate__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int terminate__Fv ();
IMPORT int testInfo;
IMPORT int testIsr ();
IMPORT int testRdReg ();
IMPORT int test_and_comparison ();
IMPORT int test_bitflip_comparison ();
IMPORT int test_bitspread_comparison ();
IMPORT int test_blockseq_comparison ();
IMPORT int test_checkerboard_comparison ();
IMPORT int test_div_comparison ();
IMPORT int test_moving_inv32 ();
IMPORT int test_mul_comparison ();
IMPORT int test_or_comparison ();
IMPORT int test_seqinc_comparison ();
IMPORT int test_solidbits_comparison ();
IMPORT int test_stuck_address ();
IMPORT int test_sub_comparison ();
IMPORT int test_verify_success ();
IMPORT int test_walkbits_comparison ();
IMPORT int test_xor_comparison ();
IMPORT int testmem_perf ();
IMPORT int tests_1arg;
IMPORT int tests_noarg;
IMPORT int tftpCopy ();
IMPORT int tftpErrorCreate ();
IMPORT int tftpGet ();
IMPORT int tftpInfoShow ();
IMPORT int tftpInit ();
IMPORT int tftpModeSet ();
IMPORT int tftpPeerSet ();
IMPORT int tftpPut ();
IMPORT int tftpQuit ();
IMPORT int tftpReXmit;
IMPORT int tftpSend ();
IMPORT int tftpTask ();
IMPORT int tftpTaskOptions;
IMPORT int tftpTaskPriority;
IMPORT int tftpTaskStackSize;
IMPORT int tftpTimeout;
IMPORT int tftpTrace;
IMPORT int tftpVerbose;
IMPORT int tftpXfer ();
IMPORT int ti ();
IMPORT int tick64Get ();
IMPORT int tick64Set ();
IMPORT int tickAnnounce ();
IMPORT int tickGet ();
IMPORT int tickQHead;
IMPORT int tickSet ();
IMPORT int tie__3iosP7ostream ();
IMPORT int tie__C3ios ();
IMPORT int time ();
IMPORT int timeFiles;
IMPORT int timex ();
IMPORT int timexClear ();
IMPORT int timexFunc ();
IMPORT int timexHelp ();
IMPORT int timexInit ();
IMPORT int timexN ();
IMPORT int timexPost ();
IMPORT int timexPre ();
IMPORT int timexShow ();
IMPORT int tkRcvRtnCall ();
IMPORT int tmpfile ();
IMPORT int tmpnam ();
IMPORT int tolower ();
IMPORT int totalerrors;
IMPORT int toupper ();
IMPORT int tr ();
IMPORT int trcDefaultArgs;
IMPORT int trcScanDepth;
IMPORT int trcStack ();
IMPORT int trgCnt;
IMPORT int trgEvtClass;
IMPORT int ts ();
IMPORT int tsfsUploadPathClose ();
IMPORT int tsfsUploadPathCreate ();
IMPORT int tsfsUploadPathLibInit ();
IMPORT int tsfsUploadPathWrite ();
IMPORT int tt ();
IMPORT int ttyDevCreate ();
IMPORT int ttyDrv ();
IMPORT int txfei0counter;
IMPORT int txfei1counter;
IMPORT int tyAbortFuncSet ();
IMPORT int tyAbortSet ();
IMPORT int tyBackspaceChar;
IMPORT int tyBackspaceSet ();
IMPORT int tyDeleteLineChar;
IMPORT int tyDeleteLineSet ();
IMPORT int tyDevInit ();
IMPORT int tyDevRemove ();
IMPORT int tyEOFSet ();
IMPORT int tyEofChar;
IMPORT int tyIRd ();
IMPORT int tyITx ();
IMPORT int tyIoctl ();
IMPORT int tyMonitorTrapSet ();
IMPORT int tyRead ();
IMPORT int tyWrite ();
IMPORT int udb;
IMPORT int udbinfo;
IMPORT int udpCfgParams;
IMPORT int udpCommIfInit ();
IMPORT int udpDoCkSumRcv;
IMPORT int udpLibInit ();
IMPORT int udpRcv ();
IMPORT int udpShowInit ();
IMPORT int udp_ctlinput ();
IMPORT int udp_in;
IMPORT int udp_init ();
IMPORT int udp_input ();
IMPORT int udp_last_inpcb;
IMPORT int udp_output ();
IMPORT int udp_pcbhashsize;
IMPORT int udp_recvspace;
IMPORT int udp_sendspace;
IMPORT int udp_ttl;
IMPORT int udp_usrreq ();
IMPORT int udpcksum;
IMPORT int udpstat;
IMPORT int udpstatShow ();
IMPORT int uflashBuf;
IMPORT int uflow__9streambuf ();
IMPORT int uiomove ();
IMPORT int unbuffered__9streambuf ();
IMPORT int unbuffered__9streambufi ();
IMPORT int uncaught_exception__Fv ();
IMPORT int underflow__7filebuf ();
IMPORT int underflow__9streambuf ();
IMPORT int unexpected__Fv ();
IMPORT int unget__7istream ();
IMPORT int unget__7istreamc ();
IMPORT int ungetc ();
IMPORT int unique__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0 ();
IMPORT int unld ();
IMPORT int unldByGroup ();
IMPORT int unldByModuleId ();
IMPORT int unldByNameAndPath ();
IMPORT int unldTextSegmentCheck ();
IMPORT int unlink ();
IMPORT int unlock__FR7istream ();
IMPORT int unlock__FR7ostream ();
IMPORT int unsave_markers__9streambuf ();
IMPORT int unset__3ios11state_value ();
IMPORT int unsetf__3iosUl ();
IMPORT int useloopback;
IMPORT int userRsrvMemInit ();
IMPORT int userRsrvMemMalloc ();
IMPORT int userRsrvMemMallocAligned ();
IMPORT int usrAltivecInit ();
IMPORT int usrBootLineCrack ();
IMPORT int usrBootLineInit ();
IMPORT int usrBpInit ();
IMPORT int usrBreakpointSet ();
IMPORT int usrClock ();
IMPORT int usrDmaDone ();
IMPORT int usrDmaInit ();
IMPORT int usrExtraModules;
IMPORT int usrInit ();
IMPORT int usrKernelInit ();
IMPORT int usrMem2MemDmaStart ();
IMPORT int usrMem2PciDmaStart ();
IMPORT int usrMmuInit ();
IMPORT int usrNetIfAttach ();
IMPORT int usrNetIfConfig ();
IMPORT int usrNetIfTbl;
IMPORT int usrNetInit ();
IMPORT int usrNetProtoInit ();
IMPORT int usrPPPInit ();
IMPORT int usrPci2MemDmaStart ();
IMPORT int usrPci2PciDmaStart ();
IMPORT int usrReadGPR ();
IMPORT int usrRoot ();
IMPORT int usrSlipInit ();
IMPORT int usrStartupScript ();
IMPORT int usrWriteGPR ();
IMPORT int usrflashBuf;
IMPORT int uswab ();
IMPORT int utime ();
IMPORT int valloc ();
IMPORT int vecOffRelocMatch ();
IMPORT int vecOffRelocMatchRev ();
IMPORT int version ();
IMPORT int vfdprintf ();
IMPORT int vfprintf ();
IMPORT int vmBaseGlobalMapInit ();
IMPORT int vmBaseLibInit ();
IMPORT int vmBasePageSizeGet ();
IMPORT int vmBaseStateSet ();
IMPORT int vmLibInfo;
IMPORT int vmpxx ();
IMPORT int vprintf ();
IMPORT int vscan__7istreamPCcP13__va_list_tag ();
IMPORT int vscan__9streambufPCcP13__va_list_tagP3ios ();
IMPORT int vsprintf ();
IMPORT int vxAbsTicks;
IMPORT int vxCntmrIntConnect ();
IMPORT int vxCntmrIntCtrlInit ();
IMPORT int vxCntmrIntDisable ();
IMPORT int vxCntmrIntEnable ();
IMPORT int vxCunitIntClear ();
IMPORT int vxCunitIntConnect ();
IMPORT int vxCunitIntCtrlInit ();
IMPORT int vxCunitIntDisable ();
IMPORT int vxCunitIntEnable ();
IMPORT int vxDarGet ();
IMPORT int vxDarSet ();
IMPORT int vxDecGet ();
IMPORT int vxDecReload ();
IMPORT int vxDecSet ();
IMPORT int vxDsisrGet ();
IMPORT int vxDsisrSet ();
IMPORT int vxEieio ();
IMPORT int vxEthernetIntCntlInit ();
IMPORT int vxEthernetIntConnect ();
IMPORT int vxEthernetIntDisable ();
IMPORT int vxEthernetIntEnable ();
IMPORT int vxEventPendQ;
IMPORT int vxFirstBit ();
IMPORT int vxFpscrGet ();
IMPORT int vxFpscrSet ();
IMPORT int vxGhsFlagSet ();
IMPORT int vxHid0Get ();
IMPORT int vxHid0Set ();
IMPORT int vxHid1Get ();
IMPORT int vxHid1Set ();
IMPORT int vxImmrDevGet ();
IMPORT int vxImmrIsbGet ();
IMPORT int vxIntStackBase;
IMPORT int vxIntStackEnd;
IMPORT int vxMemArchProbe ();
IMPORT int vxMemProbe ();
IMPORT int vxMemProbeSup ();
IMPORT int vxMpscIntClear ();
IMPORT int vxMpscIntCntlInit ();
IMPORT int vxMpscIntConnect ();
IMPORT int vxMpscIntDisable ();
IMPORT int vxMpscIntEnable ();
IMPORT int vxMpscUartDevInit ();
IMPORT int vxMpscUartDevReset ();
IMPORT int vxMpscUartRxInt ();
IMPORT int vxMpscUartTxInt ();
IMPORT int vxMsrGet ();
IMPORT int vxMsrSet ();
IMPORT int vxPowMgtEnable;
IMPORT int vxPowerDown ();
IMPORT int vxPowerModeGet ();
IMPORT int vxPowerModeSet ();
IMPORT int vxPvrGet ();
IMPORT int vxSdmaIntClear ();
IMPORT int vxSdmaIntConnect ();
IMPORT int vxSdmaIntCtrlInit ();
IMPORT int vxSdmaIntDisable ();
IMPORT int vxSdmaIntEnable ();
IMPORT int vxSrr0Get ();
IMPORT int vxSrr0Set ();
IMPORT int vxSrr1Get ();
IMPORT int vxSrr1Set ();
IMPORT int vxTas ();
IMPORT int vxTaskEntry ();
IMPORT int vxTicks;
IMPORT int vxTimeBaseGet ();
IMPORT int vxTimeBaseSet ();
IMPORT int vxWorksVersion;
IMPORT int wakeup ();
IMPORT int watchDogApp ();
IMPORT int watchDogIsr ();
IMPORT int watchdogAppId;
IMPORT int watchdogFlag;
IMPORT int watchdogTest ();
IMPORT int wcstombs ();
IMPORT int wctomb ();
IMPORT int wdCancel ();
IMPORT int wdClassId;
IMPORT int wdCreate ();
IMPORT int wdDelete ();
IMPORT int wdDestroy ();
IMPORT int wdInit ();
IMPORT int wdInstClassId;
IMPORT int wdLibInit ();
IMPORT int wdShow ();
IMPORT int wdShowInit ();
IMPORT int wdStart ();
IMPORT int wdTerminate ();
IMPORT int wdTick ();
IMPORT int wdValueReg ();
IMPORT int wdbAltivecGet ();
IMPORT int wdbAltivecLibInit ();
IMPORT int wdbAltivecRestore ();
IMPORT int wdbAltivecSave ();
IMPORT int wdbAltivecSet ();
IMPORT int wdbBpInstall ();
IMPORT int wdbCksum ();
IMPORT int wdbCommMtu;
IMPORT int wdbConfig ();
IMPORT int wdbConnectLibInit ();
IMPORT int wdbCtxCreate ();
IMPORT int wdbCtxExitLibInit ();
IMPORT int wdbCtxExitNotifyHook ();
IMPORT int wdbCtxLibInit ();
IMPORT int wdbCtxResume ();
IMPORT int wdbCtxStartLibInit ();
IMPORT int wdbDbgArchInit ();
IMPORT int wdbDbgBpFind ();
IMPORT int wdbDbgBpGet ();
IMPORT int wdbDbgBpListInit ();
IMPORT int wdbDbgBpRemove ();
IMPORT int wdbDbgBpRemoveAll ();
IMPORT int wdbDbgBreakpoint ();
IMPORT int wdbDbgDabrGet ();
IMPORT int wdbDbgDabrSet ();
IMPORT int wdbDbgDarGet ();
IMPORT int wdbDbgDataAccessStub ();
IMPORT int wdbDbgHwAddrCheck ();
IMPORT int wdbDbgHwBpFind ();
IMPORT int wdbDbgHwBpHandle ();
IMPORT int wdbDbgHwBpSet ();
IMPORT int wdbDbgHwBpStub ();
IMPORT int wdbDbgIabrGet ();
IMPORT int wdbDbgIabrSet ();
IMPORT int wdbDbgRegsClear ();
IMPORT int wdbDbgRegsSet ();
IMPORT int wdbDbgTrace ();
IMPORT int wdbDbgTraceModeClear ();
IMPORT int wdbDbgTraceModeSet ();
IMPORT int wdbDbgTraceStub ();
IMPORT int wdbDirectCallLibInit ();
IMPORT int wdbE ();
IMPORT int wdbEndDebug;
IMPORT int wdbEndPktDevInit ();
IMPORT int wdbEventDeq ();
IMPORT int wdbEventLibInit ();
IMPORT int wdbEventNodeInit ();
IMPORT int wdbEventPost ();
IMPORT int wdbEvtptClassConnect ();
IMPORT int wdbEvtptLibInit ();
IMPORT int wdbExcLibInit ();
IMPORT int wdbExternEnterHook ();
IMPORT int wdbExternExitHook ();
IMPORT int wdbExternInit ();
IMPORT int wdbExternRegSetObjAdd ();
IMPORT int wdbExternRegsGet ();
IMPORT int wdbExternRegsSet ();
IMPORT int wdbExternSystemRegs;
IMPORT int wdbFpLibInit ();
IMPORT int wdbFppGet ();
IMPORT int wdbFppRestore ();
IMPORT int wdbFppSave ();
IMPORT int wdbFppSet ();
IMPORT int wdbFuncCallLibInit ();
IMPORT int wdbGopherLibInit ();
IMPORT int wdbGopherLock;
IMPORT int wdbInfoGet ();
IMPORT int wdbInstallCommIf ();
IMPORT int wdbInstallRtIf ();
IMPORT int wdbIsNowExternal ();
IMPORT int wdbIsNowTasking ();
IMPORT int wdbMbufAlloc ();
IMPORT int wdbMbufFree ();
IMPORT int wdbMemCoreLibInit ();
IMPORT int wdbMemLibInit ();
IMPORT int wdbMemTest ();
IMPORT int wdbModeSet ();
IMPORT int wdbNotifyHost ();
IMPORT int wdbNptInt ();
IMPORT int wdbNptShutdown ();
IMPORT int wdbNumMemRegions;
IMPORT int wdbOneShot;
IMPORT int wdbRegsLibInit ();
IMPORT int wdbResumeSystem ();
IMPORT int wdbRpcGetArgs ();
IMPORT int wdbRpcNotifyConnect ();
IMPORT int wdbRpcNotifyHost ();
IMPORT int wdbRpcRcv ();
IMPORT int wdbRpcReply ();
IMPORT int wdbRpcReplyErr ();
IMPORT int wdbRpcResendReply ();
IMPORT int wdbRpcXportInit ();
IMPORT int wdbRunsExternal ();
IMPORT int wdbRunsTasking ();
IMPORT int wdbSp ();
IMPORT int wdbSuspendSystem ();
IMPORT int wdbSuspendSystemHere ();
IMPORT int wdbSvcAdd ();
IMPORT int wdbSvcDispatch ();
IMPORT int wdbSvcDsaSvcRemove ();
IMPORT int wdbSvcHookAdd ();
IMPORT int wdbSvcLibInit ();
IMPORT int wdbSysBpLibInit ();
IMPORT int wdbSystemSuspend ();
IMPORT int wdbTargetIsConnected ();
IMPORT int wdbTask ();
IMPORT int wdbTaskBpLibInit ();
IMPORT int wdbTaskId;
IMPORT int wdbTaskInit ();
IMPORT int wdbToolName;
IMPORT int wdbTsfsDefaultDirPerm;
IMPORT int wdbTsfsDrv ();
IMPORT int wdbUserEvtLibInit ();
IMPORT int wdbUserEvtPost ();
IMPORT int wdbVioChannelRegister ();
IMPORT int wdbVioChannelUnregister ();
IMPORT int wdbVioDrv ();
IMPORT int wdbVioLibInit ();
IMPORT int wdgCnt;
IMPORT int wdgNMICnt;
IMPORT int wdgService;
IMPORT int what__C11logic_error ();
IMPORT int what__C13runtime_error ();
IMPORT int what__C9bad_alloc ();
IMPORT int what__C9exception ();
IMPORT int whoami ();
IMPORT int width__3iosi ();
IMPORT int width__C3ios ();
IMPORT int wildcard;
IMPORT int windDelay ();
IMPORT int windDelete ();
IMPORT int windExit ();
IMPORT int windIntStackSet ();
IMPORT int windLoadContext ();
IMPORT int windPendQFlush ();
IMPORT int windPendQGet ();
IMPORT int windPendQPut ();
IMPORT int windPendQRemove ();
IMPORT int windPendQTerminate ();
IMPORT int windPriNormalSet ();
IMPORT int windPrioritySet ();
IMPORT int windReadyQPut ();
IMPORT int windReadyQRemove ();
IMPORT int windResume ();
IMPORT int windSemDelete ();
IMPORT int windSpawn ();
IMPORT int windSuspend ();
IMPORT int windTickAnnounce ();
IMPORT int windUndelay ();
IMPORT int windWdCancel ();
IMPORT int windWdStart ();
IMPORT int windviewConfig ();
IMPORT int workQAdd0 ();
IMPORT int workQAdd1 ();
IMPORT int workQAdd2 ();
IMPORT int workQDoWork ();
IMPORT int workQInit ();
IMPORT int workQIsEmpty;
IMPORT int workQPanic ();
IMPORT int workQReadIx;
IMPORT int workQWriteIx;
IMPORT int writable__3ios ();
IMPORT int write ();
IMPORT int write__7ostreamPCSci ();
IMPORT int write__7ostreamPCUci ();
IMPORT int write__7ostreamPCci ();
IMPORT int write__7ostreamPCvi ();
IMPORT int writev ();
IMPORT int wrreg ();
IMPORT int ws__FR7istream ();
IMPORT int wvBufId;
IMPORT int wvDefaultBufMax;
IMPORT int wvDefaultBufMin;
IMPORT int wvDefaultBufOptions;
IMPORT int wvDefaultBufSize;
IMPORT int wvDefaultBufThresh;
IMPORT int wvDefaultRBuffParams;
IMPORT int wvEvent ();
IMPORT int wvEventInst ();
IMPORT int wvEvtBufferFullNotify;
IMPORT int wvEvtBufferGet ();
IMPORT int wvEvtClass;
IMPORT int wvEvtClassClear ();
IMPORT int wvEvtClassClearAll ();
IMPORT int wvEvtClassGet ();
IMPORT int wvEvtClassSet ();
IMPORT int wvEvtLogInit ();
IMPORT int wvEvtLogStart ();
IMPORT int wvEvtLogStop ();
IMPORT int wvInstIsOn;
IMPORT int wvLibInit ();
IMPORT int wvLibInit2 ();
IMPORT int wvLogHeader;
IMPORT int wvLogHeaderCreate ();
IMPORT int wvLogHeaderUpload ();
IMPORT int wvObjInst ();
IMPORT int wvObjInstModeSet ();
IMPORT int wvObjIsEnabled;
IMPORT int wvOff ();
IMPORT int wvOn ();
IMPORT int wvRBuffErrorHandler ();
IMPORT int wvRBuffMgr ();
IMPORT int wvRBuffMgrId;
IMPORT int wvRBuffMgrPrioritySet ();
IMPORT int wvSigInst ();
IMPORT int wvTaskNamesBufAdd ();
IMPORT int wvTaskNamesPreserve ();
IMPORT int wvTaskNamesUpload ();
IMPORT int wvTmrRegister ();
IMPORT int wvUpPathId;
IMPORT int wvUpTaskId;
IMPORT int wvUploadMaxAttempts;
IMPORT int wvUploadRetryBackoff;
IMPORT int wvUploadStart ();
IMPORT int wvUploadStop ();
IMPORT int wvUploadTaskConfig ();
IMPORT int wvUploadTaskOptions;
IMPORT int wvUploadTaskPriority;
IMPORT int wvUploadTaskStackSize;
IMPORT int xattrib ();
IMPORT int xchain__9streambuf ();
IMPORT int xcopy ();
IMPORT int xdelete ();
IMPORT int xdrCksum ();
IMPORT int xdr_ARRAY ();
IMPORT int xdr_CHECKSUM ();
IMPORT int xdr_TGT_ADDR_T ();
IMPORT int xdr_TGT_INT_T ();
IMPORT int xdr_UINT32 ();
IMPORT int xdr_WDB_AGENT_INFO ();
IMPORT int xdr_WDB_CALL_RETURN_INFO ();
IMPORT int xdr_WDB_CTX ();
IMPORT int xdr_WDB_CTX_CREATE_DESC ();
IMPORT int xdr_WDB_CTX_STEP_DESC ();
IMPORT int xdr_WDB_EVTPT_ADD_DESC ();
IMPORT int xdr_WDB_EVTPT_DEL_DESC ();
IMPORT int xdr_WDB_EVT_DATA ();
IMPORT int xdr_WDB_EVT_INFO ();
IMPORT int xdr_WDB_MEM_REGION ();
IMPORT int xdr_WDB_MEM_SCAN_DESC ();
IMPORT int xdr_WDB_MEM_XFER ();
IMPORT int xdr_WDB_OPQ_DATA_T ();
IMPORT int xdr_WDB_PARAM_WRAPPER ();
IMPORT int xdr_WDB_REG_READ_DESC ();
IMPORT int xdr_WDB_REG_WRITE_DESC ();
IMPORT int xdr_WDB_REPLY_WRAPPER ();
IMPORT int xdr_WDB_RT_INFO ();
IMPORT int xdr_WDB_STRING_T ();
IMPORT int xdr_WDB_TGT_INFO ();
IMPORT int xdr_accepted_reply ();
IMPORT int xdr_array ();
IMPORT int xdr_attrstat ();
IMPORT int xdr_authunix_parms ();
IMPORT int xdr_bool ();
IMPORT int xdr_bytes ();
IMPORT int xdr_callhdr ();
IMPORT int xdr_callmsg ();
IMPORT int xdr_char ();
IMPORT int xdr_createargs ();
IMPORT int xdr_des_block ();
IMPORT int xdr_deskey ();
IMPORT int xdr_dirlist ();
IMPORT int xdr_diropargs ();
IMPORT int xdr_diropokres ();
IMPORT int xdr_diropres ();
IMPORT int xdr_dirpath ();
IMPORT int xdr_double ();
IMPORT int xdr_entry ();
IMPORT int xdr_enum ();
IMPORT int xdr_exportnode ();
IMPORT int xdr_exports ();
IMPORT int xdr_fattr ();
IMPORT int xdr_fhandle ();
IMPORT int xdr_fhstatus ();
IMPORT int xdr_filename ();
IMPORT int xdr_float ();
IMPORT int xdr_floatInclude ();
IMPORT int xdr_free ();
IMPORT int xdr_ftype ();
IMPORT int xdr_groupnode ();
IMPORT int xdr_groups ();
IMPORT int xdr_int ();
IMPORT int xdr_linkargs ();
IMPORT int xdr_long ();
IMPORT int xdr_mountbody ();
IMPORT int xdr_mountlist ();
IMPORT int xdr_mountname ();
IMPORT int xdr_netobj ();
IMPORT int xdr_nfs_fh ();
IMPORT int xdr_nfscookie ();
IMPORT int xdr_nfspath ();
IMPORT int xdr_nfsstat ();
IMPORT int xdr_nfstime ();
IMPORT int xdr_opaque ();
IMPORT int xdr_opaque_auth ();
IMPORT int xdr_pmap ();
IMPORT int xdr_pmaplist ();
IMPORT int xdr_pointer ();
IMPORT int xdr_readargs ();
IMPORT int xdr_readdirargs ();
IMPORT int xdr_readdirres ();
IMPORT int xdr_readlinkres ();
IMPORT int xdr_readokres ();
IMPORT int xdr_readres ();
IMPORT int xdr_reference ();
IMPORT int xdr_rejected_reply ();
IMPORT int xdr_renameargs ();
IMPORT int xdr_replymsg ();
IMPORT int xdr_sattr ();
IMPORT int xdr_sattrargs ();
IMPORT int xdr_short ();
IMPORT int xdr_statfsokres ();
IMPORT int xdr_statfsres ();
IMPORT int xdr_string ();
IMPORT int xdr_symlinkargs ();
IMPORT int xdr_u_char ();
IMPORT int xdr_u_int ();
IMPORT int xdr_u_long ();
IMPORT int xdr_u_short ();
IMPORT int xdr_union ();
IMPORT int xdr_vector ();
IMPORT int xdr_void ();
IMPORT int xdr_wrapstring ();
IMPORT int xdr_writeargs ();
IMPORT int xdrmem_create ();
IMPORT int xdrrec_create ();
IMPORT int xdrrec_endofrecord ();
IMPORT int xdrrec_eof ();
IMPORT int xdrrec_skiprecord ();
IMPORT int xflags__9streambuf ();
IMPORT int xflags__9streambufi ();
IMPORT int xmalloc ();
IMPORT int xprt_register ();
IMPORT int xprt_unregister ();
IMPORT int xput_char__9streambufc ();
IMPORT int xrealloc ();
IMPORT int xsetflags__9streambufi ();
IMPORT int xsetflags__9streambufii ();
IMPORT int xsgetn__7filebufPci ();
IMPORT int xsgetn__9streambufPci ();
IMPORT int xsputn__7filebufPCci ();
IMPORT int xsputn__9streambufPCci ();
IMPORT int yearr;
IMPORT int yy_yys;
IMPORT int yy_yyv;
IMPORT int yychar;
IMPORT int yydebug;
IMPORT int yyerrflag;
IMPORT int yylval;
IMPORT int yynerrs;
IMPORT int yyparse ();
IMPORT int yyps;
IMPORT int yypv;
IMPORT int yys;
IMPORT int yystart ();
IMPORT int yystate;
IMPORT int yytmp;
IMPORT int yyv;
IMPORT int yyval;
IMPORT int zeroin_addr;

SYMBOL standTbl [4981] =
    {
        {{NULL}, "Bbase__9streambuf", (char*) Bbase__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "Bptr__9streambuf", (char*) Bptr__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "DCACHE_CTRL", (char*) &DCACHE_CTRL, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "DCD_regs_dump", (char*) DCD_regs_dump, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "DELAY", (char*) DELAY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "EEPROMDetect", (char*) EEPROMDetect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FlashCmdE", (char*) &FlashCmdE, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "FlashCmdS", (char*) &FlashCmdS, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "GET_THRM1", (char*) GET_THRM1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "GET_THRM3", (char*) GET_THRM3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "Gbase__9streambuf", (char*) Gbase__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "IntLatencyTotal", (char*) &IntLatencyTotal, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "IntTimeBaseEnd", (char*) &IntTimeBaseEnd, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "IntTimeBaseSt", (char*) &IntTimeBaseSt, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "MV64360_regs_dump", (char*) MV64360_regs_dump, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "Nbase__9streambuf", (char*) Nbase__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PCI_DCD_regs_dump", (char*) PCI_DCD_regs_dump, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PCI_REMAP_regs_dump", (char*) PCI_REMAP_regs_dump, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SDMA_RXBD_regs_dump", (char*) SDMA_RXBD_regs_dump, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SDMA_TXBD_regs_dump", (char*) SDMA_TXBD_regs_dump, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SET_THRM1", (char*) SET_THRM1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SET_THRM3", (char*) SET_THRM3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ShowCfdRing", (char*) ShowCfdRing, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "TestIntConnect", (char*) TestIntConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_10bad_typeid", (char*) _$_10bad_typeid, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_11logic_error", (char*) _$_11logic_error, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_11range_error", (char*) _$_11range_error, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_12domain_error", (char*) _$_12domain_error, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_12length_error", (char*) _$_12length_error, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_12out_of_range", (char*) _$_12out_of_range, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_12streammarker", (char*) _$_12streammarker, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_13bad_exception", (char*) _$_13bad_exception, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_13runtime_error", (char*) _$_13runtime_error, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_14__si_type_info", (char*) _$_14__si_type_info, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_14overflow_error", (char*) _$_14overflow_error, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_15underflow_error", (char*) _$_15underflow_error, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_16__user_type_info", (char*) _$_16__user_type_info, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_16invalid_argument", (char*) _$_16invalid_argument, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_17__class_type_info", (char*) _$_17__class_type_info, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_22_IO_istream_withassign", (char*) _$_22_IO_istream_withassign, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_22_IO_ostream_withassign", (char*) _$_22_IO_ostream_withassign, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_3ios", (char*) _$_3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_7filebuf", (char*) _$_7filebuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_7istream", (char*) _$_7istream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_7ostream", (char*) _$_7ostream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_8bad_cast", (char*) _$_8bad_cast, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_8iostream", (char*) _$_8iostream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_9bad_alloc", (char*) _$_9bad_alloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_9exception", (char*) _$_9exception, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_9streambuf", (char*) _$_9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_9type_info", (char*) _$_9type_info, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_Q2t24__default_alloc_template2b1i0_5_Lock", (char*) _$_Q2t24__default_alloc_template2b1i0_5_Lock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_$_t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) _$_t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_7filebuf$openprot", (char*) &_7filebuf$openprot, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_GLOBAL_$D$_IO_stdin_", (char*) _GLOBAL_$D$_IO_stdin_, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GLOBAL_$D$_t24__default_alloc_template2b1i0$_S_end_free", (char*) _GLOBAL_$D$_t24__default_alloc_template2b1i0$_S_end_free, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GLOBAL_$D$_un_link__9streambuf", (char*) _GLOBAL_$D$_un_link__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GLOBAL_$I$_IO_stdin_", (char*) _GLOBAL_$I$_IO_stdin_, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GLOBAL_$I$_t24__default_alloc_template2b1i0$_S_end_free", (char*) _GLOBAL_$I$_t24__default_alloc_template2b1i0$_S_end_free, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GLOBAL_$I$_un_link__9streambuf", (char*) _GLOBAL_$I$_un_link__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_adjust_column", (char*) _IO_adjust_column, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_cleanup", (char*) _IO_cleanup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_cleanup_registration_needed", (char*) &_IO_cleanup_registration_needed, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_IO_default_doallocate", (char*) _IO_default_doallocate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_default_finish", (char*) _IO_default_finish, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_default_pbackfail", (char*) _IO_default_pbackfail, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_default_read", (char*) _IO_default_read, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_default_seek", (char*) _IO_default_seek, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_default_seekoff", (char*) _IO_default_seekoff, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_default_seekpos", (char*) _IO_default_seekpos, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_default_setbuf", (char*) _IO_default_setbuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_default_stat", (char*) _IO_default_stat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_default_sync", (char*) _IO_default_sync, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_default_uflow", (char*) _IO_default_uflow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_default_underflow", (char*) _IO_default_underflow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_default_write", (char*) _IO_default_write, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_default_xsgetn", (char*) _IO_default_xsgetn, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_default_xsputn", (char*) _IO_default_xsputn, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_do_write", (char*) _IO_do_write, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_doallocbuf", (char*) _IO_doallocbuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_dtoa", (char*) _IO_dtoa, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_file_attach", (char*) _IO_file_attach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_file_close", (char*) _IO_file_close, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_file_close_it", (char*) _IO_file_close_it, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_file_doallocate", (char*) _IO_file_doallocate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_file_finish", (char*) _IO_file_finish, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_file_fopen", (char*) _IO_file_fopen, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_file_init", (char*) _IO_file_init, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_file_jumps", (char*) &_IO_file_jumps, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_IO_file_overflow", (char*) _IO_file_overflow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_file_read", (char*) _IO_file_read, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_file_seek", (char*) _IO_file_seek, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_file_seekoff", (char*) _IO_file_seekoff, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_file_setbuf", (char*) _IO_file_setbuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_file_stat", (char*) _IO_file_stat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_file_sync", (char*) _IO_file_sync, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_file_underflow", (char*) _IO_file_underflow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_file_write", (char*) _IO_file_write, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_file_xsputn", (char*) _IO_file_xsputn, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_flush_all", (char*) _IO_flush_all, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_flush_all_linebuffered", (char*) _IO_flush_all_linebuffered, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_free_backup_area", (char*) _IO_free_backup_area, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_getc", (char*) _IO_getc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_getline", (char*) _IO_getline, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_getline_info", (char*) _IO_getline_info, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_init", (char*) _IO_init, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_init_marker", (char*) _IO_init_marker, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_link_in", (char*) _IO_link_in, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_list_all", (char*) &_IO_list_all, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_IO_marker_delta", (char*) _IO_marker_delta, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_marker_difference", (char*) _IO_marker_difference, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_outfloat", (char*) _IO_outfloat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_padn", (char*) _IO_padn, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_peekc_locked", (char*) _IO_peekc_locked, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_putc", (char*) _IO_putc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_remove_marker", (char*) _IO_remove_marker, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_seekmark", (char*) _IO_seekmark, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_seekoff", (char*) _IO_seekoff, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_seekpos", (char*) _IO_seekpos, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_setb", (char*) _IO_setb, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_sgetn", (char*) _IO_sgetn, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_sputbackc", (char*) _IO_sputbackc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_stderr_", (char*) &_IO_stderr_, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_IO_stdin_", (char*) &_IO_stdin_, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_IO_stdout_", (char*) &_IO_stdout_, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_IO_strtod", (char*) _IO_strtod, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_sungetc", (char*) _IO_sungetc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_switch_to_backup_area", (char*) _IO_switch_to_backup_area, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_switch_to_get_mode", (char*) _IO_switch_to_get_mode, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_switch_to_main_get_area", (char*) _IO_switch_to_main_get_area, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_un_link", (char*) _IO_un_link, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_ungetc", (char*) _IO_ungetc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_unsave_markers", (char*) _IO_unsave_markers, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IO_vfscanf", (char*) _IO_vfscanf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_Randseed", (char*) &_Randseed, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_S_chunk_alloc__t24__default_alloc_template2b1i0UiRi", (char*) _S_chunk_alloc__t24__default_alloc_template2b1i0UiRi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_S_freelist_index__t24__default_alloc_template2b1i0Ui", (char*) _S_freelist_index__t24__default_alloc_template2b1i0Ui, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_S_oom_malloc__t23__malloc_alloc_template1i0Ui", (char*) _S_oom_malloc__t23__malloc_alloc_template1i0Ui, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_S_oom_realloc__t23__malloc_alloc_template1i0PvUi", (char*) _S_oom_realloc__t23__malloc_alloc_template1i0PvUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_S_refill__t24__default_alloc_template2b1i0Ui", (char*) _S_refill__t24__default_alloc_template2b1i0Ui, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_S_round_up__t24__default_alloc_template2b1i0Ui", (char*) _S_round_up__t24__default_alloc_template2b1i0Ui, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__10RBString_T", (char*) __10RBString_T, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__10RBString_TPCc", (char*) __10RBString_TPCc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__10RBString_TR10RBString_T", (char*) __10RBString_TR10RBString_T, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__10bad_typeid", (char*) __10bad_typeid, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__11logic_errorRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) __11logic_errorRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__11range_errorRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) __11range_errorRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__12domain_errorRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) __12domain_errorRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__12length_errorRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) __12length_errorRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__12out_of_rangeRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) __12out_of_rangeRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__12streammarkerP9streambuf", (char*) __12streammarkerP9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__13bad_exception", (char*) __13bad_exception, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__13runtime_error", (char*) __13runtime_error, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__13runtime_errorRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) __13runtime_errorRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__14__si_type_infoPCcRC16__user_type_info", (char*) __14__si_type_infoPCcRC16__user_type_info, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__14overflow_errorRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) __14overflow_errorRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__15underflow_errorRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) __15underflow_errorRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__16__user_type_infoPCc", (char*) __16__user_type_infoPCc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__16invalid_argumentRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) __16invalid_argumentRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__17__class_type_infoPCcPCQ217__class_type_info9base_infoUi", (char*) __17__class_type_infoPCcPCQ217__class_type_info9base_infoUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__18RBStringIterator_TRC10RBString_T", (char*) __18RBStringIterator_TRC10RBString_T, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__3iosP9streambufP7ostream", (char*) __3iosP9streambufP7ostream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__7filebuf", (char*) __7filebuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__7filebufi", (char*) __7filebufi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__7filebufiPci", (char*) __7filebufiPci, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__7istreami", (char*) __7istreami, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__7istreamiP9streambufP7ostream", (char*) __7istreamiP9streambufP7ostream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__7ostreami", (char*) __7ostreami, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__7ostreamiP9streambufP7ostream", (char*) __7ostreamiP9streambufP7ostream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__8bad_cast", (char*) __8bad_cast, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__8iostreami", (char*) __8iostreami, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__8iostreamiP9streambufP7ostream", (char*) __8iostreamiP9streambufP7ostream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__9exception", (char*) __9exception, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__9streambufi", (char*) __9streambufi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__9type_infoPCc", (char*) __9type_infoPCc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__AltiVec_runtime", (char*) &__AltiVec_runtime, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__DIAB_rd_pk_db", (char*) __DIAB_rd_pk_db, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__DIAB_rd_pk_fl", (char*) __DIAB_rd_pk_fl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__DIAB_rd_pk_ld", (char*) __DIAB_rd_pk_ld, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__DIAB_rd_sw_db", (char*) __DIAB_rd_sw_db, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__DIAB_rd_sw_fl", (char*) __DIAB_rd_sw_fl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__DIAB_rd_sw_ld", (char*) __DIAB_rd_sw_ld, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__DIAB_wr_pk_db", (char*) __DIAB_wr_pk_db, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__DIAB_wr_pk_fl", (char*) __DIAB_wr_pk_fl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__DIAB_wr_pk_ld", (char*) __DIAB_wr_pk_ld, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__DIAB_wr_sw_db", (char*) __DIAB_wr_sw_db, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__DIAB_wr_sw_fl", (char*) __DIAB_wr_sw_fl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__DIAB_wr_sw_ld", (char*) __DIAB_wr_sw_ld, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__Q23ios4Init", (char*) __Q23ios4Init, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__Q2t24__default_alloc_template2b1i0_5_Lock", (char*) __Q2t24__default_alloc_template2b1i0_5_Lock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__READ_FLASH16", (char*) __READ_FLASH16, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__READ_FLASH32", (char*) __READ_FLASH32, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__READ_FLASH64", (char*) __READ_FLASH64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__WRITE_FLASH16", (char*) __WRITE_FLASH16, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__WRITE_FLASH32", (char*) __WRITE_FLASH32, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__WRITE_FLASH64", (char*) __WRITE_FLASH64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "___x_diab_comljmp_o", (char*) &___x_diab_comljmp_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_hfpfitof_o", (char*) &___x_diab_hfpfitof_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_hfpfutof_o", (char*) &___x_diab_hfpfutof_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_libg_o", (char*) &___x_diab_libg_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_rd_pk_db_o", (char*) &___x_diab_rd_pk_db_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_rd_pk_fl_o", (char*) &___x_diab_rd_pk_fl_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_rd_pk_ld_o", (char*) &___x_diab_rd_pk_ld_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_rd_sw_db_o", (char*) &___x_diab_rd_sw_db_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_rd_sw_fl_o", (char*) &___x_diab_rd_sw_fl_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_rd_sw_ld_o", (char*) &___x_diab_rd_sw_ld_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_restfprs_o", (char*) &___x_diab_restfprs_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_restfprx_o", (char*) &___x_diab_restfprx_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_restfreg_o", (char*) &___x_diab_restfreg_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_restgprx_o", (char*) &___x_diab_restgprx_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_rtahooks_o", (char*) &___x_diab_rtahooks_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_savefprs_o", (char*) &___x_diab_savefprs_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_savefprx_o", (char*) &___x_diab_savefprx_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_savefreg_o", (char*) &___x_diab_savefreg_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_savegprx_o", (char*) &___x_diab_savegprx_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_sfpddtoll_o", (char*) &___x_diab_sfpddtoll_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_sfpddtoull_o", (char*) &___x_diab_sfpddtoull_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_sfpdlltod_o", (char*) &___x_diab_sfpdlltod_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_sfpdnorm_o", (char*) &___x_diab_sfpdnorm_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_sfpdulltod_o", (char*) &___x_diab_sfpdulltod_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_sfpfftoll_o", (char*) &___x_diab_sfpfftoll_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_sfpfftoull_o", (char*) &___x_diab_sfpfftoull_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_sfpflltof_o", (char*) &___x_diab_sfpflltof_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_sfpfnorm_o", (char*) &___x_diab_sfpfnorm_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_sfpfulltof_o", (char*) &___x_diab_sfpfulltof_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_svrs_vpr_o", (char*) &___x_diab_svrs_vpr_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_sxpadd64_o", (char*) &___x_diab_sxpadd64_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_sxpasr64_o", (char*) &___x_diab_sxpasr64_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_sxpcmp64_o", (char*) &___x_diab_sxpcmp64_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_sxpdiv32_o", (char*) &___x_diab_sxpdiv32_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_sxpdiv64_o", (char*) &___x_diab_sxpdiv64_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_sxplsl64_o", (char*) &___x_diab_sxplsl64_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_sxplsr64_o", (char*) &___x_diab_sxplsr64_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_sxpmul64_o", (char*) &___x_diab_sxpmul64_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_sxprem32_o", (char*) &___x_diab_sxprem32_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_sxprem64_o", (char*) &___x_diab_sxprem64_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_torn_syncatomic_o", (char*) &___x_diab_torn_syncatomic_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_torn_syncmutex_o", (char*) &___x_diab_torn_syncmutex_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_torn_syncsem_o", (char*) &___x_diab_torn_syncsem_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_va_arg_o", (char*) &___x_diab_va_arg_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_wr_pk_db_o", (char*) &___x_diab_wr_pk_db_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_wr_pk_fl_o", (char*) &___x_diab_wr_pk_fl_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_wr_pk_ld_o", (char*) &___x_diab_wr_pk_ld_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_wr_sw_db_o", (char*) &___x_diab_wr_sw_db_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_wr_sw_fl_o", (char*) &___x_diab_wr_sw_fl_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_diab_wr_sw_ld_o", (char*) &___x_diab_wr_sw_ld_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu___dummy_o", (char*) &___x_gnu___dummy_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu___gcc_bcmp_o", (char*) &___x_gnu___gcc_bcmp_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__ashldi3_o", (char*) &___x_gnu__ashldi3_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__ashrdi3_o", (char*) &___x_gnu__ashrdi3_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__bb_o", (char*) &___x_gnu__bb_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__clear_cache_o", (char*) &___x_gnu__clear_cache_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__cmpdi2_o", (char*) &___x_gnu__cmpdi2_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__divdi3_o", (char*) &___x_gnu__divdi3_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__eh_o", (char*) &___x_gnu__eh_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__eprintf_o", (char*) &___x_gnu__eprintf_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__ffsdi2_o", (char*) &___x_gnu__ffsdi2_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__fixdfdi_o", (char*) &___x_gnu__fixdfdi_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__fixsfdi_o", (char*) &___x_gnu__fixsfdi_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__fixtfdi_o", (char*) &___x_gnu__fixtfdi_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__fixunsdfdi_o", (char*) &___x_gnu__fixunsdfdi_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__fixunsdfsi_o", (char*) &___x_gnu__fixunsdfsi_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__fixunssfdi_o", (char*) &___x_gnu__fixunssfdi_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__fixunssfsi_o", (char*) &___x_gnu__fixunssfsi_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__fixunstfdi_o", (char*) &___x_gnu__fixunstfdi_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__fixunsxfdi_o", (char*) &___x_gnu__fixunsxfdi_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__fixunsxfsi_o", (char*) &___x_gnu__fixunsxfsi_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__fixxfdi_o", (char*) &___x_gnu__fixxfdi_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__floatdidf_o", (char*) &___x_gnu__floatdidf_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__floatdisf_o", (char*) &___x_gnu__floatdisf_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__floatditf_o", (char*) &___x_gnu__floatditf_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__floatdixf_o", (char*) &___x_gnu__floatdixf_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__lshrdi3_o", (char*) &___x_gnu__lshrdi3_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__moddi3_o", (char*) &___x_gnu__moddi3_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__muldi3_o", (char*) &___x_gnu__muldi3_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__negdi2_o", (char*) &___x_gnu__negdi2_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__shtab_o", (char*) &___x_gnu__shtab_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__trampoline_o", (char*) &___x_gnu__trampoline_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__ucmpdi2_o", (char*) &___x_gnu__ucmpdi2_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__udiv_w_sdiv_o", (char*) &___x_gnu__udiv_w_sdiv_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__udivdi3_o", (char*) &___x_gnu__udivdi3_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__udivmoddi4_o", (char*) &___x_gnu__udivmoddi4_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__umoddi3_o", (char*) &___x_gnu__umoddi3_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_cleanup_o", (char*) &___x_gnu_cleanup_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_cmathi_o", (char*) &___x_gnu_cmathi_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_cstdlibi_o", (char*) &___x_gnu_cstdlibi_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_cstrio_o", (char*) &___x_gnu_cstrio_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_cstrmain_o", (char*) &___x_gnu_cstrmain_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_dp_bit_o", (char*) &___x_gnu_dp_bit_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_exception_o", (char*) &___x_gnu_exception_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_filebuf_o", (char*) &___x_gnu_filebuf_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_filedoalloc_o", (char*) &___x_gnu_filedoalloc_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_fileops_o", (char*) &___x_gnu_fileops_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_floatconv_o", (char*) &___x_gnu_floatconv_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_fp_bit_o", (char*) &___x_gnu_fp_bit_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_frame_o", (char*) &___x_gnu_frame_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_genops_o", (char*) &___x_gnu_genops_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_ioassign_o", (char*) &___x_gnu_ioassign_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_iogetc_o", (char*) &___x_gnu_iogetc_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_iogetline_o", (char*) &___x_gnu_iogetline_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_iopadn_o", (char*) &___x_gnu_iopadn_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_ioputc_o", (char*) &___x_gnu_ioputc_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_ioseekoff_o", (char*) &___x_gnu_ioseekoff_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_ioseekpos_o", (char*) &___x_gnu_ioseekpos_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_iostream_o", (char*) &___x_gnu_iostream_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_ioungetc_o", (char*) &___x_gnu_ioungetc_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_iovfscanf_o", (char*) &___x_gnu_iovfscanf_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_isgetline_o", (char*) &___x_gnu_isgetline_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_isscan_o", (char*) &___x_gnu_isscan_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_new_o", (char*) &___x_gnu_new_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_opdel_o", (char*) &___x_gnu_opdel_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_opdelnt_o", (char*) &___x_gnu_opdelnt_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_opnew_o", (char*) &___x_gnu_opnew_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_opnewnt_o", (char*) &___x_gnu_opnewnt_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_opvdel_o", (char*) &___x_gnu_opvdel_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_opvdelnt_o", (char*) &___x_gnu_opvdelnt_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_opvnew_o", (char*) &___x_gnu_opvnew_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_opvnewnt_o", (char*) &___x_gnu_opvnewnt_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_outfloat_o", (char*) &___x_gnu_outfloat_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_peekc_o", (char*) &___x_gnu_peekc_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_sbscan_o", (char*) &___x_gnu_sbscan_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_stdexcepti_o", (char*) &___x_gnu_stdexcepti_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_stdstrbufs_o", (char*) &___x_gnu_stdstrbufs_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_stdstreams_o", (char*) &___x_gnu_stdstreams_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_stlinst_o", (char*) &___x_gnu_stlinst_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_streambuf_o", (char*) &___x_gnu_streambuf_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_tinfo2_o", (char*) &___x_gnu_tinfo2_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_tinfo_o", (char*) &___x_gnu_tinfo_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu_vrsave_o", (char*) &___x_gnu_vrsave_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__add64", (char*) __add64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__adddf3", (char*) __adddf3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__addsf3", (char*) __addsf3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__apl__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCc", (char*) __apl__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__apl__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) __apl__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__apl__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0c", (char*) __apl__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0c, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__as__10RBString_TR10RBString_T", (char*) __as__10RBString_TR10RBString_T, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__as__22_IO_istream_withassignR22_IO_istream_withassign", (char*) __as__22_IO_istream_withassignR22_IO_istream_withassign, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__as__22_IO_istream_withassignR7istream", (char*) __as__22_IO_istream_withassignR7istream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__as__22_IO_ostream_withassignR22_IO_ostream_withassign", (char*) __as__22_IO_ostream_withassignR22_IO_ostream_withassign, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__as__22_IO_ostream_withassignR7ostream", (char*) __as__22_IO_ostream_withassignR7ostream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__as__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCc", (char*) __as__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__as__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) __as__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__as__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0c", (char*) __as__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0c, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ashldi3", (char*) __ashldi3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ashrdi3", (char*) __ashrdi3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__asr64", (char*) __asr64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__assert", (char*) __assert, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__builtin_delete", (char*) __builtin_delete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__builtin_new", (char*) __builtin_new, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__builtin_vec_delete", (char*) __builtin_vec_delete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__builtin_vec_new", (char*) __builtin_vec_new, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__check_eh_spec", (char*) __check_eh_spec, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__clear_cache", (char*) __clear_cache, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__clocale", (char*) &__clocale, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__cmpdf2", (char*) __cmpdf2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__cmpdi2", (char*) __cmpdi2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__cmpsf2", (char*) __cmpsf2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__common_intrinsicsInit", (char*) __common_intrinsicsInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__common_long_jmp", (char*) __common_long_jmp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__costate", (char*) &__costate, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__cp_eh_info", (char*) __cp_eh_info, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__cp_exception_info", (char*) __cp_exception_info, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__cp_pop_exception", (char*) __cp_pop_exception, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__cp_push_exception", (char*) __cp_push_exception, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__cplusCore_o", (char*) &__cplusCore_o, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__cplusDem_o", (char*) &__cplusDem_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__cplusLibgccObjs", (char*) &__cplusLibgccObjs, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__cplusLoadObjFiles", (char*) &__cplusLoadObjFiles, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__cplusLoad_o", (char*) &__cplusLoad_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__cplusObjFiles", (char*) &__cplusObjFiles, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__cplusStlObjs", (char*) &__cplusStlObjs, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__cplusStr_o", (char*) &__cplusStr_o, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__cplusStringIoObjs", (char*) &__cplusStringIoObjs, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__cplusStringObjs", (char*) &__cplusStringObjs, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__cplusUsr_o", (char*) &__cplusUsr_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__cplusXtors_o", (char*) &__cplusXtors_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__cplus_type_matcher", (char*) __cplus_type_matcher, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ctype", (char*) &__ctype, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__daysSinceEpoch", (char*) __daysSinceEpoch, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__default_terminate", (char*) __default_terminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__default_unexpected__Fv", (char*) __default_unexpected__Fv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__deregister_frame", (char*) __deregister_frame, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__deregister_frame_info", (char*) __deregister_frame_info, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__diab_alloc_mutex", (char*) __diab_alloc_mutex, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__diab_alloc_semaphore", (char*) __diab_alloc_semaphore, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__diab_atomic_enter", (char*) __diab_atomic_enter, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__diab_atomic_restore", (char*) __diab_atomic_restore, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__diab_free_mutex", (char*) __diab_free_mutex, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__diab_free_semaphore", (char*) __diab_free_semaphore, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__diab_intrinsics_impfpInit", (char*) __diab_intrinsics_impfpInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__diab_intrinsics_implInit", (char*) __diab_intrinsics_implInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__diab_intrinsics_tornInit", (char*) __diab_intrinsics_tornInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__diab_lock_mutex", (char*) __diab_lock_mutex, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__diab_lock_semaphore", (char*) __diab_lock_semaphore, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__diab_trylock_mutex", (char*) __diab_trylock_mutex, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__diab_trylock_semaphore", (char*) __diab_trylock_semaphore, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__diab_unlock_mutex", (char*) __diab_unlock_mutex, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__diab_unlock_semaphore", (char*) __diab_unlock_semaphore, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__div32", (char*) __div32, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__div64", (char*) __div64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__divdf3", (char*) __divdf3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__divdi3", (char*) __divdi3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__divsf3", (char*) __divsf3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__dl__FPvRC9nothrow_t", (char*) __dl__FPvRC9nothrow_t, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__dl__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepPv", (char*) __dl__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepPv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__dtoll", (char*) __dtoll, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__dtoull", (char*) __dtoull, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__dummy", (char*) __dummy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__dynamic_cast", (char*) __dynamic_cast, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__eabi", (char*) __eabi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__eh_alloc", (char*) __eh_alloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__eh_free", (char*) __eh_free, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__eh_rtime_match", (char*) __eh_rtime_match, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__empty", (char*) __empty, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__eq64", (char*) __eq64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__eq__C10RBString_TR10RBString_T", (char*) __eq__C10RBString_TR10RBString_T, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__eq__C9type_infoRC9type_info", (char*) __eq__C9type_infoRC9type_info, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__eq__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_PCX01RCt12basic_string3ZX01ZX11ZX21_b", (char*) __eq__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_PCX01RCt12basic_string3ZX01ZX11ZX21_b, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__eq__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21PCX01_b", (char*) __eq__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21PCX01_b, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__eq__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21T0_b", (char*) __eq__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21T0_b, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__eqdf2", (char*) __eqdf2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__eqsf2", (char*) __eqsf2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__errno", (char*) __errno, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__exp10", (char*) __exp10, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__extendsfdf2", (char*) __extendsfdf2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ffsdi2", (char*) __ffsdi2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__fixdfdi", (char*) __fixdfdi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__fixdfsi", (char*) __fixdfsi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__fixsfdi", (char*) __fixsfdi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__fixsfsi", (char*) __fixsfsi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__fixunsdfdi", (char*) __fixunsdfdi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__fixunsdfsi", (char*) __fixunsdfsi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__fixunssfdi", (char*) __fixunssfdi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__fixunssfsi", (char*) __fixunssfsi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__floatdidf", (char*) __floatdidf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__floatdisf", (char*) __floatdisf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__floatsidf", (char*) __floatsidf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__floatsisf", (char*) __floatsisf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__fpcmp_parts_d", (char*) __fpcmp_parts_d, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__fpcmp_parts_f", (char*) __fpcmp_parts_f, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__frame_state_for", (char*) __frame_state_for, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ftoll", (char*) __ftoll, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ftoull", (char*) __ftoull, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__gcc_bcmp", (char*) __gcc_bcmp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__gcc_intrinsicsInit", (char*) __gcc_intrinsicsInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ge64", (char*) __ge64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ge__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_PCX01RCt12basic_string3ZX01ZX11ZX21_b", (char*) __ge__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_PCX01RCt12basic_string3ZX01ZX11ZX21_b, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ge__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21PCX01_b", (char*) __ge__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21PCX01_b, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ge__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21T0_b", (char*) __ge__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21T0_b, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__gedf2", (char*) __gedf2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__gesf2", (char*) __gesf2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__getDstInfo", (char*) __getDstInfo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__getTime", (char*) __getTime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__getZoneInfo", (char*) __getZoneInfo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__get_dynamic_handler_chain", (char*) __get_dynamic_handler_chain, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__get_eh_context", (char*) __get_eh_context, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__get_eh_info", (char*) __get_eh_info, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__get_eh_table_language", (char*) __get_eh_table_language, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__get_eh_table_version", (char*) __get_eh_table_version, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__gt64", (char*) __gt64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__gt__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_PCX01RCt12basic_string3ZX01ZX11ZX21_b", (char*) __gt__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_PCX01RCt12basic_string3ZX01ZX11ZX21_b, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__gt__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21PCX01_b", (char*) __gt__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21PCX01_b, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__gt__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21T0_b", (char*) __gt__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21T0_b, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__gtdf2", (char*) __gtdf2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__gtsf2", (char*) __gtsf2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__includeDiabIntrinsics", (char*) &__includeDiabIntrinsics, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__includeGnuIntrinsics", (char*) &__includeGnuIntrinsics, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__is_pointer__FPv", (char*) __is_pointer__FPv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__julday", (char*) __julday, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__le64", (char*) __le64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__le__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_PCX01RCt12basic_string3ZX01ZX11ZX21_b", (char*) __le__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_PCX01RCt12basic_string3ZX01ZX11ZX21_b, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__le__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21PCX01_b", (char*) __le__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21PCX01_b, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__le__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21T0_b", (char*) __le__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21T0_b, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ledf2", (char*) __ledf2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__length_error__FPCc", (char*) __length_error__FPCc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__lesf2", (char*) __lesf2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__lltod", (char*) __lltod, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__lltof", (char*) __lltof, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__locale", (char*) &__locale, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__loctime", (char*) &__loctime, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__ls__7ostreamP9streambuf", (char*) __ls__7ostreamP9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ls__7ostreamPCSc", (char*) __ls__7ostreamPCSc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ls__7ostreamPCUc", (char*) __ls__7ostreamPCUc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ls__7ostreamPCc", (char*) __ls__7ostreamPCc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ls__7ostreamPFR3ios_R3ios", (char*) __ls__7ostreamPFR3ios_R3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ls__7ostreamPFR7ostream_R7ostream", (char*) __ls__7ostreamPFR7ostream_R7ostream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ls__7ostreamSc", (char*) __ls__7ostreamSc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ls__7ostreamUc", (char*) __ls__7ostreamUc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ls__7ostreamUi", (char*) __ls__7ostreamUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ls__7ostreamUl", (char*) __ls__7ostreamUl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ls__7ostreamUs", (char*) __ls__7ostreamUs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ls__7ostreamUx", (char*) __ls__7ostreamUx, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ls__7ostreamb", (char*) __ls__7ostreamb, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ls__7ostreamc", (char*) __ls__7ostreamc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ls__7ostreamd", (char*) __ls__7ostreamd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ls__7ostreamf", (char*) __ls__7ostreamf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ls__7ostreami", (char*) __ls__7ostreami, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ls__7ostreaml", (char*) __ls__7ostreaml, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ls__7ostreamr", (char*) __ls__7ostreamr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ls__7ostreams", (char*) __ls__7ostreams, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ls__7ostreamx", (char*) __ls__7ostreamx, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ls__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_R7ostreamRCt12basic_string3ZX01ZX11ZX21_R7ostream", (char*) __ls__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_R7ostreamRCt12basic_string3ZX01ZX11ZX21_R7ostream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__lshrdi3", (char*) __lshrdi3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__lsl64", (char*) __lsl64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__lsr64", (char*) __lsr64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__lt64", (char*) __lt64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__lt__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_PCX01RCt12basic_string3ZX01ZX11ZX21_b", (char*) __lt__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_PCX01RCt12basic_string3ZX01ZX11ZX21_b, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__lt__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21PCX01_b", (char*) __lt__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21PCX01_b, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__lt__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21T0_b", (char*) __lt__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21T0_b, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ltdf2", (char*) __ltdf2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ltsf2", (char*) __ltsf2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__make_dp", (char*) __make_dp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__make_fp", (char*) __make_fp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__moddi3", (char*) __moddi3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__mul64", (char*) __mul64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__muldf3", (char*) __muldf3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__muldi3", (char*) __muldi3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__mulsf3", (char*) __mulsf3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ne64", (char*) __ne64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ne__C9type_infoRC9type_info", (char*) __ne__C9type_infoRC9type_info, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ne__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_PCX01RCt12basic_string3ZX01ZX11ZX21_b", (char*) __ne__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_PCX01RCt12basic_string3ZX01ZX11ZX21_b, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ne__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21PCX01_b", (char*) __ne__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21PCX01_b, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ne__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21T0_b", (char*) __ne__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21T0_b, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__nedf2", (char*) __nedf2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__negdf2", (char*) __negdf2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__negdi2", (char*) __negdi2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__negsf2", (char*) __negsf2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__nesf2", (char*) __nesf2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__new_handler", (char*) &__new_handler, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__nt__C3ios", (char*) __nt__C3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__nw__FUiPv", (char*) __nw__FUiPv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__nw__FUiRC9nothrow_t", (char*) __nw__FUiRC9nothrow_t, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__nw__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepUiUi", (char*) __nw__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepUiUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__opPv__C3ios", (char*) __opPv__C3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__out_of_range__FPCc", (char*) __out_of_range__FPCc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__overflow", (char*) __overflow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__overflow__FP9streambufi", (char*) __overflow__FP9streambufi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__pack_d", (char*) __pack_d, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__pack_f", (char*) __pack_f, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__pl__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_PCX01RCt12basic_string3ZX01ZX11ZX21_t12basic_string3ZX01ZX11ZX21", (char*) __pl__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_PCX01RCt12basic_string3ZX01ZX11ZX21_t12basic_string3ZX01ZX11ZX21, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__pl__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21PCX01_t12basic_string3ZX01ZX11ZX21", (char*) __pl__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21PCX01_t12basic_string3ZX01ZX11ZX21, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__pl__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21T0_t12basic_string3ZX01ZX11ZX21", (char*) __pl__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21T0_t12basic_string3ZX01ZX11ZX21, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__pl__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21X01_t12basic_string3ZX01ZX11ZX21", (char*) __pl__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_RCt12basic_string3ZX01ZX11ZX21X01_t12basic_string3ZX01ZX11ZX21, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__pl__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_X01RCt12basic_string3ZX01ZX11ZX21_t12basic_string3ZX01ZX11ZX21", (char*) __pl__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_X01RCt12basic_string3ZX01ZX11ZX21_t12basic_string3ZX01ZX11ZX21, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__pure_virtual", (char*) __pure_virtual, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__pure_virtual_called", (char*) __pure_virtual_called, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__register_frame", (char*) __register_frame, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__register_frame_info", (char*) __register_frame_info, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__register_frame_info_table", (char*) __register_frame_info_table, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__register_frame_table", (char*) __register_frame_table, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rem32", (char*) __rem32, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rem64", (char*) __rem64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rethrow", (char*) __rethrow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rs__7istreamP9streambuf", (char*) __rs__7istreamP9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rs__7istreamPFR3ios_R3ios", (char*) __rs__7istreamPFR3ios_R3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rs__7istreamPFR7istream_R7istream", (char*) __rs__7istreamPFR7istream_R7istream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rs__7istreamPSc", (char*) __rs__7istreamPSc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rs__7istreamPUc", (char*) __rs__7istreamPUc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rs__7istreamPc", (char*) __rs__7istreamPc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rs__7istreamRSc", (char*) __rs__7istreamRSc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rs__7istreamRUc", (char*) __rs__7istreamRUc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rs__7istreamRUi", (char*) __rs__7istreamRUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rs__7istreamRUl", (char*) __rs__7istreamRUl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rs__7istreamRUs", (char*) __rs__7istreamRUs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rs__7istreamRUx", (char*) __rs__7istreamRUx, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rs__7istreamRb", (char*) __rs__7istreamRb, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rs__7istreamRc", (char*) __rs__7istreamRc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rs__7istreamRd", (char*) __rs__7istreamRd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rs__7istreamRf", (char*) __rs__7istreamRf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rs__7istreamRi", (char*) __rs__7istreamRi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rs__7istreamRl", (char*) __rs__7istreamRl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rs__7istreamRr", (char*) __rs__7istreamRr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rs__7istreamRs", (char*) __rs__7istreamRs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rs__7istreamRx", (char*) __rs__7istreamRx, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rs__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_R7istreamRt12basic_string3ZX01ZX11ZX21_R7istream", (char*) __rs__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_R7istreamRt12basic_string3ZX01ZX11ZX21_R7istream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rta_longjmp", (char*) &__rta_longjmp, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__rtc_alloc_ptr", (char*) &__rtc_alloc_ptr, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__rtc_blkinfo_ptr", (char*) &__rtc_blkinfo_ptr, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__rtc_free_ptr", (char*) &__rtc_free_ptr, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__rtti_array", (char*) __rtti_array, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rtti_attr", (char*) __rtti_attr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rtti_class", (char*) __rtti_class, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rtti_func", (char*) __rtti_func, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rtti_ptmd", (char*) __rtti_ptmd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rtti_ptmf", (char*) __rtti_ptmf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rtti_ptr", (char*) __rtti_ptr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rtti_si", (char*) __rtti_si, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rtti_user", (char*) __rtti_user, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__saverest_o", (char*) &__saverest_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__sclose", (char*) __sclose, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__set_malloc_handler__t23__malloc_alloc_template1i0PFv_v", (char*) __set_malloc_handler__t23__malloc_alloc_template1i0PFv_v, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__sflags", (char*) __sflags, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__sflush", (char*) __sflush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__sfvwrite", (char*) __sfvwrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__shtab", (char*) &__shtab, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__sjpopnthrow", (char*) __sjpopnthrow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__sjthrow", (char*) __sjthrow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__smakebuf", (char*) __smakebuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__sread", (char*) __sread, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__srefill", (char*) __srefill, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__srget", (char*) __srget, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__sseek", (char*) __sseek, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__start_cp_handler", (char*) __start_cp_handler, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__stderr", (char*) __stderr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__stdin", (char*) __stdin, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__stdout", (char*) __stdout, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__strxfrm", (char*) __strxfrm, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__sub64", (char*) __sub64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__subdf3", (char*) __subdf3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__subsf3", (char*) __subsf3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__swbuf", (char*) __swbuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__swrite", (char*) __swrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__swsetup", (char*) __swsetup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) __t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCc", (char*) __t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi", (char*) __t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) __t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUi", (char*) __t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Uic", (char*) __t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Uic, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__terminate", (char*) __terminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__terminate_func", (char*) &__terminate_func, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__tf10bad_typeid", (char*) __tf10bad_typeid, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tf11logic_error", (char*) __tf11logic_error, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tf11range_error", (char*) __tf11range_error, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tf12domain_error", (char*) __tf12domain_error, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tf12length_error", (char*) __tf12length_error, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tf12out_of_range", (char*) __tf12out_of_range, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tf13bad_exception", (char*) __tf13bad_exception, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tf13runtime_error", (char*) __tf13runtime_error, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tf14__si_type_info", (char*) __tf14__si_type_info, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tf14overflow_error", (char*) __tf14overflow_error, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tf15underflow_error", (char*) __tf15underflow_error, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tf16__user_type_info", (char*) __tf16__user_type_info, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tf16invalid_argument", (char*) __tf16invalid_argument, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tf17__class_type_info", (char*) __tf17__class_type_info, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tf22_IO_istream_withassign", (char*) __tf22_IO_istream_withassign, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tf22_IO_ostream_withassign", (char*) __tf22_IO_ostream_withassign, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tf3ios", (char*) __tf3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tf7filebuf", (char*) __tf7filebuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tf7istream", (char*) __tf7istream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tf7ostream", (char*) __tf7ostream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tf8bad_cast", (char*) __tf8bad_cast, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tf8iostream", (char*) __tf8iostream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tf9bad_alloc", (char*) __tf9bad_alloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tf9exception", (char*) __tf9exception, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tf9streambuf", (char*) __tf9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tf9type_info", (char*) __tf9type_info, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tfSc", (char*) __tfSc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tfUc", (char*) __tfUc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tfUi", (char*) __tfUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tfUl", (char*) __tfUl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tfUs", (char*) __tfUs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tfUx", (char*) __tfUx, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tfb", (char*) __tfb, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tfc", (char*) __tfc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tfd", (char*) __tfd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tff", (char*) __tff, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tfi", (char*) __tfi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tfl", (char*) __tfl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tfr", (char*) __tfr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tfs", (char*) __tfs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tfv", (char*) __tfv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tfw", (char*) __tfw, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__tfx", (char*) __tfx, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__throw", (char*) __throw, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__throw_bad_cast", (char*) __throw_bad_cast, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__throw_bad_typeid", (char*) __throw_bad_typeid, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__throw_type_match", (char*) __throw_type_match, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__throw_type_match_rtti", (char*) __throw_type_match_rtti, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ti10bad_typeid", (char*) &__ti10bad_typeid, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti11_ios_fields", (char*) &__ti11_ios_fields, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti11logic_error", (char*) &__ti11logic_error, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti11range_error", (char*) &__ti11range_error, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti12domain_error", (char*) &__ti12domain_error, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti12length_error", (char*) &__ti12length_error, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti12out_of_range", (char*) &__ti12out_of_range, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti13bad_exception", (char*) &__ti13bad_exception, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti13runtime_error", (char*) &__ti13runtime_error, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti14__si_type_info", (char*) &__ti14__si_type_info, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti14overflow_error", (char*) &__ti14overflow_error, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti15underflow_error", (char*) &__ti15underflow_error, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti16__attr_type_info", (char*) &__ti16__attr_type_info, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti16__func_type_info", (char*) &__ti16__func_type_info, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti16__ptmd_type_info", (char*) &__ti16__ptmd_type_info, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti16__ptmf_type_info", (char*) &__ti16__ptmf_type_info, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti16__user_type_info", (char*) &__ti16__user_type_info, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti16invalid_argument", (char*) &__ti16invalid_argument, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti17__array_type_info", (char*) &__ti17__array_type_info, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti17__class_type_info", (char*) &__ti17__class_type_info, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti19__builtin_type_info", (char*) &__ti19__builtin_type_info, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti19__pointer_type_info", (char*) &__ti19__pointer_type_info, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti22_IO_istream_withassign", (char*) &__ti22_IO_istream_withassign, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti22_IO_ostream_withassign", (char*) &__ti22_IO_ostream_withassign, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti3ios", (char*) &__ti3ios, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti7filebuf", (char*) &__ti7filebuf, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti7istream", (char*) &__ti7istream, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti7ostream", (char*) &__ti7ostream, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti8_IO_FILE", (char*) &__ti8_IO_FILE, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti8bad_cast", (char*) &__ti8bad_cast, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti8iostream", (char*) &__ti8iostream, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti9bad_alloc", (char*) &__ti9bad_alloc, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti9exception", (char*) &__ti9exception, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti9streambuf", (char*) &__ti9streambuf, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__ti9type_info", (char*) &__ti9type_info, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__tiSc", (char*) &__tiSc, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__tiUc", (char*) &__tiUc, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__tiUi", (char*) &__tiUi, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__tiUl", (char*) &__tiUl, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__tiUs", (char*) &__tiUs, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__tiUx", (char*) &__tiUx, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__tib", (char*) &__tib, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__tic", (char*) &__tic, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__tid", (char*) &__tid, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__tif", (char*) &__tif, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__tii", (char*) &__tii, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__til", (char*) &__til, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__tir", (char*) &__tir, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__tis", (char*) &__tis, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__tiv", (char*) &__tiv, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__tiw", (char*) &__tiw, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__tix", (char*) &__tix, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__truncdfsf2", (char*) __truncdfsf2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ucmpdi2", (char*) __ucmpdi2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__udiv32", (char*) __udiv32, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__udiv64", (char*) __udiv64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__udiv_w_sdiv", (char*) __udiv_w_sdiv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__udivdi3", (char*) __udivdi3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__udivmoddi4", (char*) __udivmoddi4, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__uflow", (char*) __uflow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__uge64", (char*) __uge64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ugt64", (char*) __ugt64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ule64", (char*) __ule64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ulltod", (char*) __ulltod, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ulltof", (char*) __ulltof, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ult64", (char*) __ult64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__umoddi3", (char*) __umoddi3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__uncatch_exception", (char*) __uncatch_exception, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__underflow", (char*) __underflow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__unpack_d", (char*) __unpack_d, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__unpack_f", (char*) __unpack_f, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__unwinding_cleanup", (char*) __unwinding_cleanup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__urem32", (char*) __urem32, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__urem64", (char*) __urem64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__va_arg", (char*) __va_arg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__vc__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui", (char*) __vc__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__vc__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepUi", (char*) __vc__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__vc__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui", (char*) __vc__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__vd__FPvRC9nothrow_t", (char*) __vd__FPvRC9nothrow_t, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__vn__FUiPv", (char*) __vn__FUiPv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__vn__FUiRC9nothrow_t", (char*) __vn__FUiRC9nothrow_t, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__wdbEventListIsEmpty", (char*) &__wdbEventListIsEmpty, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__wdbEvtptDeleteAll", (char*) &__wdbEvtptDeleteAll, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_archHelp_msg", (char*) &_archHelp_msg, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_arpCmd", (char*) _arpCmd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_authenticate", (char*) _authenticate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_clockRealtime", (char*) &_clockRealtime, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_cplusDemangle", (char*) _cplusDemangle, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_d_dtoll", (char*) _d_dtoll, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_d_dtoull", (char*) _d_dtoull, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_d_lltod", (char*) _d_lltod, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_d_ulltod", (char*) _d_ulltod, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_db_show_arptab", (char*) _db_show_arptab, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_dbgArchInit", (char*) _dbgArchInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_dbgBrkDisplayHard", (char*) _dbgBrkDisplayHard, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_dbgDsmInstRtn", (char*) &_dbgDsmInstRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_dbgFuncCallCheck", (char*) _dbgFuncCallCheck, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_dbgInfoPCGet", (char*) _dbgInfoPCGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_dbgInstSizeGet", (char*) _dbgInstSizeGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_dbgRetAdrsGet", (char*) _dbgRetAdrsGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_dbgTaskPCGet", (char*) _dbgTaskPCGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_dbgTaskPCSet", (char*) _dbgTaskPCSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_f_ftoll", (char*) _f_ftoll, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_f_ftoull", (char*) _f_ftoull, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_f_lltof", (char*) _f_lltof, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_f_ulltof", (char*) _f_ulltof, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_find__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCccUiUi", (char*) _find__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCccUiUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_frcPci0ReadConfigReg", (char*) _frcPci0ReadConfigReg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_frcPci0WriteConfigReg", (char*) _frcPci0WriteConfigReg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_func_altivecProbe", (char*) &_func_altivecProbe, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_altivecProbeRtn", (char*) &_func_altivecProbeRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_func_altivecTaskRegsGet", (char*) &_func_altivecTaskRegsGet, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_altivecTaskRegsShow", (char*) &_func_altivecTaskRegsShow, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_bdall", (char*) &_func_bdall, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_breakpoint", (char*) &_func_breakpoint, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_func_dbgHostNotify", (char*) &_func_dbgHostNotify, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_func_dbgTargetNotify", (char*) &_func_dbgTargetNotify, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_func_dspMregsHook", (char*) &_func_dspMregsHook, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_dspRegsListHook", (char*) &_func_dspRegsListHook, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_dspTaskRegsShow", (char*) &_func_dspTaskRegsShow, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogM0", (char*) &_func_evtLogM0, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogM1", (char*) &_func_evtLogM1, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogM2", (char*) &_func_evtLogM2, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogM3", (char*) &_func_evtLogM3, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogO", (char*) &_func_evtLogO, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogOIntLock", (char*) &_func_evtLogOIntLock, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogPoint", (char*) &_func_evtLogPoint, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogReserveTaskName", (char*) &_func_evtLogReserveTaskName, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogString", (char*) &_func_evtLogString, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogT0", (char*) &_func_evtLogT0, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogT0_noInt", (char*) &_func_evtLogT0_noInt, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogT1", (char*) &_func_evtLogT1, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogT1_noTS", (char*) &_func_evtLogT1_noTS, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogTSched", (char*) &_func_evtLogTSched, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_excBaseHook", (char*) &_func_excBaseHook, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_excInfoShow", (char*) &_func_excInfoShow, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_excIntHook", (char*) &_func_excIntHook, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_excJobAdd", (char*) &_func_excJobAdd, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_excPanicHook", (char*) &_func_excPanicHook, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_excTrapRtn", (char*) &_func_excTrapRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_func_fastUdpErrorNotify", (char*) &_func_fastUdpErrorNotify, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_fastUdpInput", (char*) &_func_fastUdpInput, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_fastUdpPortTest", (char*) &_func_fastUdpPortTest, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_fclose", (char*) &_func_fclose, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_fileDoesNotExist", (char*) &_func_fileDoesNotExist, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_func_fppTaskRegsShow", (char*) &_func_fppTaskRegsShow, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_ftpLs", (char*) &_func_ftpLs, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_ftpTransientFatal", (char*) &_func_ftpTransientFatal, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_func_intConnectRtn", (char*) &_func_intConnectRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_func_intDisableRtn", (char*) &_func_intDisableRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_func_intEnableRtn", (char*) &_func_intEnableRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_func_intLevelSetRtn", (char*) &_func_intLevelSetRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_func_intVecBaseGetRtn", (char*) &_func_intVecBaseGetRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_func_intVecBaseSetRtn", (char*) &_func_intVecBaseSetRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_func_intVecGetRtn", (char*) &_func_intVecGetRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_func_intVecSetRtn", (char*) &_func_intVecSetRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_func_ioTaskStdSet", (char*) &_func_ioTaskStdSet, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_ipsecFilterHook", (char*) &_func_ipsecFilterHook, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_ipsecInput", (char*) &_func_ipsecInput, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_ipsecOutput", (char*) &_func_ipsecOutput, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_logMsg", (char*) &_func_logMsg, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_memalign", (char*) &_func_memalign, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_netLsByName", (char*) &_func_netLsByName, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_printErr", (char*) &_func_printErr, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_pthread_setcanceltype", (char*) &_func_pthread_setcanceltype, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_remCurIdGet", (char*) &_func_remCurIdGet, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_remCurIdSet", (char*) &_func_remCurIdSet, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_selPtyAdd", (char*) &_func_selPtyAdd, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_selPtyDelete", (char*) &_func_selPtyDelete, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_selTyAdd", (char*) &_func_selTyAdd, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_selTyDelete", (char*) &_func_selTyDelete, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_selWakeupAll", (char*) &_func_selWakeupAll, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_selWakeupListInit", (char*) &_func_selWakeupListInit, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_selWakeupListTerm", (char*) &_func_selWakeupListTerm, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_sigExcKill", (char*) &_func_sigExcKill, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_sigTimeoutRecalc", (char*) &_func_sigTimeoutRecalc, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_sigprocmask", (char*) &_func_sigprocmask, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_smObjObjShow", (char*) &_func_smObjObjShow, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_spy", (char*) &_func_spy, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_spyClkStart", (char*) &_func_spyClkStart, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_spyClkStop", (char*) &_func_spyClkStop, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_spyReport", (char*) &_func_spyReport, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_spyStop", (char*) &_func_spyStop, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_spyTask", (char*) &_func_spyTask, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_sseTaskRegsShow", (char*) &_func_sseTaskRegsShow, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_symFindByValue", (char*) &_func_symFindByValue, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_symFindByValueAndType", (char*) &_func_symFindByValueAndType, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_symFindSymbol", (char*) &_func_symFindSymbol, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_symNameGet", (char*) &_func_symNameGet, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_symTypeGet", (char*) &_func_symTypeGet, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_symValueGet", (char*) &_func_symValueGet, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_taskCreateHookAdd", (char*) &_func_taskCreateHookAdd, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_taskDeleteHookAdd", (char*) &_func_taskDeleteHookAdd, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_taskRegsShowRtn", (char*) &_func_taskRegsShowRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_tmrConnect", (char*) &_func_tmrConnect, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_tmrDisable", (char*) &_func_tmrDisable, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_tmrEnable", (char*) &_func_tmrEnable, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_tmrFreq", (char*) &_func_tmrFreq, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_tmrPeriod", (char*) &_func_tmrPeriod, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_tmrStamp", (char*) &_func_tmrStamp, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_tmrStampLock", (char*) &_func_tmrStampLock, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_trace", (char*) &_func_trace, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_func_trgCheck", (char*) &_func_trgCheck, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_valloc", (char*) &_func_valloc, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_vxMemProbeHook", (char*) &_func_vxMemProbeHook, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_func_wdbIsNowExternal", (char*) &_func_wdbIsNowExternal, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_func_wvNetAddressFilterTest", (char*) &_func_wvNetAddressFilterTest, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_wvNetPortFilterTest", (char*) &_func_wvNetPortFilterTest, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_icmpErrorHook", (char*) &_icmpErrorHook, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_igmpJoinAlertHook", (char*) &_igmpJoinAlertHook, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_igmpJoinGrpHook", (char*) &_igmpJoinGrpHook, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_igmpLeaveAlertHook", (char*) &_igmpLeaveAlertHook, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_igmpLeaveGrpHook", (char*) &_igmpLeaveGrpHook, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_igmpMessageHook", (char*) &_igmpMessageHook, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_igmpQuerierTimeUpdateHook", (char*) &_igmpQuerierTimeUpdateHook, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_initialize_dbg_uart", (char*) _initialize_dbg_uart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_insque", (char*) _insque, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_ipCfgFlags", (char*) &_ipCfgFlags, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_ipFilterHook", (char*) &_ipFilterHook, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_itof", (char*) _itof, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_landAttackSafe", (char*) &_landAttackSafe, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_link_in__9streambuf", (char*) _link_in__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_m2IfTableUpdate", (char*) &_m2IfTableUpdate, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_m2SetIfLastChange", (char*) &_m2SetIfLastChange, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_mCastRouteCmdHook", (char*) &_mCastRouteCmdHook, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_mCastRouteFwdHook", (char*) &_mCastRouteFwdHook, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_netFree", (char*) _netFree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_netMalloc", (char*) _netMalloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_null_auth", (char*) &_null_auth, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_pNetBufCollect", (char*) &_pNetBufCollect, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_pNetDpool", (char*) &_pNetDpool, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_pNetPoolFuncTbl", (char*) &_pNetPoolFuncTbl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_pNetSysPool", (char*) &_pNetSysPool, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_pSigQueueFreeHead", (char*) &_pSigQueueFreeHead, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_pSysBatInitFunc", (char*) &_pSysBatInitFunc, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_pSysL2CacheDisable", (char*) &_pSysL2CacheDisable, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_pSysL2CacheEnable", (char*) &_pSysL2CacheEnable, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_pSysL2CacheFlush", (char*) &_pSysL2CacheFlush, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_pSysL2CacheInvFunc", (char*) &_pSysL2CacheInvFunc, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_pSysL3CacheFlushDisableFunc", (char*) &_pSysL3CacheFlushDisableFunc, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_pSysL3CacheInvalEnableFunc", (char*) &_pSysL3CacheInvalEnableFunc, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_pTcpPcbHead", (char*) &_pTcpPcbHead, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_pTcpPcbPrint", (char*) &_pTcpPcbPrint, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_pUdpPcbHead", (char*) &_pUdpPcbHead, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_panicHook", (char*) &_panicHook, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_pingTxInterval", (char*) &_pingTxInterval, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_pingTxLen", (char*) &_pingTxLen, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_pingTxTmo", (char*) &_pingTxTmo, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_presolvHostLibGetByAddr", (char*) &_presolvHostLibGetByAddr, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_presolvHostLibGetByName", (char*) &_presolvHostLibGetByName, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_procNumWasSet", (char*) &_procNumWasSet, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_protoSwIndex", (char*) &_protoSwIndex, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_remque", (char*) _remque, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restf14", (char*) _restf14, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restf15", (char*) _restf15, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restf16", (char*) _restf16, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restf17", (char*) _restf17, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restf18", (char*) _restf18, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restf19", (char*) _restf19, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restf20", (char*) _restf20, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restf21", (char*) _restf21, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restf22", (char*) _restf22, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restf23", (char*) _restf23, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restf24", (char*) _restf24, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restf25", (char*) _restf25, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restf26", (char*) _restf26, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restf27", (char*) _restf27, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restf28", (char*) _restf28, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restf29", (char*) _restf29, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restf30", (char*) _restf30, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restf31", (char*) _restf31, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfpr_14_l", (char*) _restfpr_14_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfpr_15_l", (char*) _restfpr_15_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfpr_16_l", (char*) _restfpr_16_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfpr_17_l", (char*) _restfpr_17_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfpr_18_l", (char*) _restfpr_18_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfpr_19_l", (char*) _restfpr_19_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfpr_20_l", (char*) _restfpr_20_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfpr_21_l", (char*) _restfpr_21_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfpr_22_l", (char*) _restfpr_22_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfpr_23_l", (char*) _restfpr_23_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfpr_24_l", (char*) _restfpr_24_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfpr_25_l", (char*) _restfpr_25_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfpr_26_l", (char*) _restfpr_26_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfpr_27_l", (char*) _restfpr_27_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfpr_28_l", (char*) _restfpr_28_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfpr_29_l", (char*) _restfpr_29_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfpr_30_l", (char*) _restfpr_30_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfpr_31_l", (char*) _restfpr_31_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprs_14_l", (char*) _restfprs_14_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprs_15_l", (char*) _restfprs_15_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprs_16_l", (char*) _restfprs_16_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprs_17_l", (char*) _restfprs_17_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprs_18_l", (char*) _restfprs_18_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprs_19_l", (char*) _restfprs_19_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprs_20_l", (char*) _restfprs_20_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprs_21_l", (char*) _restfprs_21_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprs_22_l", (char*) _restfprs_22_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprs_23_l", (char*) _restfprs_23_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprs_24_l", (char*) _restfprs_24_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprs_25_l", (char*) _restfprs_25_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprs_26_l", (char*) _restfprs_26_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprs_27_l", (char*) _restfprs_27_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprs_28_l", (char*) _restfprs_28_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprs_29_l", (char*) _restfprs_29_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprs_30_l", (char*) _restfprs_30_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprs_31_l", (char*) _restfprs_31_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprx_14_l", (char*) _restfprx_14_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprx_15_l", (char*) _restfprx_15_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprx_16_l", (char*) _restfprx_16_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprx_17_l", (char*) _restfprx_17_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprx_18_l", (char*) _restfprx_18_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprx_19_l", (char*) _restfprx_19_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprx_20_l", (char*) _restfprx_20_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprx_21_l", (char*) _restfprx_21_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprx_22_l", (char*) _restfprx_22_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprx_23_l", (char*) _restfprx_23_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprx_24_l", (char*) _restfprx_24_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprx_25_l", (char*) _restfprx_25_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprx_26_l", (char*) _restfprx_26_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprx_27_l", (char*) _restfprx_27_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprx_28_l", (char*) _restfprx_28_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprx_29_l", (char*) _restfprx_29_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprx_30_l", (char*) _restfprx_30_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restfprx_31_l", (char*) _restfprx_31_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_14", (char*) _restgpr_14, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_14_l", (char*) _restgpr_14_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_15", (char*) _restgpr_15, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_15_l", (char*) _restgpr_15_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_16", (char*) _restgpr_16, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_16_l", (char*) _restgpr_16_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_17", (char*) _restgpr_17, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_17_l", (char*) _restgpr_17_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_18", (char*) _restgpr_18, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_18_l", (char*) _restgpr_18_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_19", (char*) _restgpr_19, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_19_l", (char*) _restgpr_19_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_20", (char*) _restgpr_20, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_20_l", (char*) _restgpr_20_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_21", (char*) _restgpr_21, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_21_l", (char*) _restgpr_21_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_22", (char*) _restgpr_22, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_22_l", (char*) _restgpr_22_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_23", (char*) _restgpr_23, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_23_l", (char*) _restgpr_23_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_24", (char*) _restgpr_24, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_24_l", (char*) _restgpr_24_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_25", (char*) _restgpr_25, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_25_l", (char*) _restgpr_25_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_26", (char*) _restgpr_26, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_26_l", (char*) _restgpr_26_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_27", (char*) _restgpr_27, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_27_l", (char*) _restgpr_27_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_28", (char*) _restgpr_28, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_28_l", (char*) _restgpr_28_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_29", (char*) _restgpr_29, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_29_l", (char*) _restgpr_29_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_30", (char*) _restgpr_30, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_30_l", (char*) _restgpr_30_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_31", (char*) _restgpr_31, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgpr_31_l", (char*) _restgpr_31_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgprx_14_l", (char*) _restgprx_14_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgprx_15_l", (char*) _restgprx_15_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgprx_16_l", (char*) _restgprx_16_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgprx_17_l", (char*) _restgprx_17_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgprx_18_l", (char*) _restgprx_18_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgprx_19_l", (char*) _restgprx_19_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgprx_20_l", (char*) _restgprx_20_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgprx_21_l", (char*) _restgprx_21_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgprx_22_l", (char*) _restgprx_22_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgprx_23_l", (char*) _restgprx_23_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgprx_24_l", (char*) _restgprx_24_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgprx_25_l", (char*) _restgprx_25_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgprx_26_l", (char*) _restgprx_26_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgprx_27_l", (char*) _restgprx_27_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgprx_28_l", (char*) _restgprx_28_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgprx_29_l", (char*) _restgprx_29_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgprx_30_l", (char*) _restgprx_30_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restgprx_31_l", (char*) _restgprx_31_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restv20", (char*) _restv20, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restv21", (char*) _restv21, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restv22", (char*) _restv22, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restv23", (char*) _restv23, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restv24", (char*) _restv24, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restv25", (char*) _restv25, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restv26", (char*) _restv26, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restv27", (char*) _restv27, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restv28", (char*) _restv28, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restv29", (char*) _restv29, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restv30", (char*) _restv30, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restv31", (char*) _restv31, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restvr20", (char*) _restvr20, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restvr21", (char*) _restvr21, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restvr22", (char*) _restvr22, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restvr23", (char*) _restvr23, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restvr24", (char*) _restvr24, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restvr25", (char*) _restvr25, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restvr26", (char*) _restvr26, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restvr27", (char*) _restvr27, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restvr28", (char*) _restvr28, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restvr29", (char*) _restvr29, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restvr30", (char*) _restvr30, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_restvr31", (char*) _restvr31, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savef14", (char*) _savef14, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savef15", (char*) _savef15, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savef16", (char*) _savef16, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savef17", (char*) _savef17, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savef18", (char*) _savef18, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savef19", (char*) _savef19, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savef20", (char*) _savef20, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savef21", (char*) _savef21, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savef22", (char*) _savef22, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savef23", (char*) _savef23, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savef24", (char*) _savef24, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savef25", (char*) _savef25, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savef26", (char*) _savef26, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savef27", (char*) _savef27, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savef28", (char*) _savef28, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savef29", (char*) _savef29, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savef30", (char*) _savef30, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savef31", (char*) _savef31, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefpr_14_l", (char*) _savefpr_14_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefpr_15_l", (char*) _savefpr_15_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefpr_16_l", (char*) _savefpr_16_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefpr_17_l", (char*) _savefpr_17_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefpr_18_l", (char*) _savefpr_18_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefpr_19_l", (char*) _savefpr_19_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefpr_20_l", (char*) _savefpr_20_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefpr_21_l", (char*) _savefpr_21_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefpr_22_l", (char*) _savefpr_22_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefpr_23_l", (char*) _savefpr_23_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefpr_24_l", (char*) _savefpr_24_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefpr_25_l", (char*) _savefpr_25_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefpr_26_l", (char*) _savefpr_26_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefpr_27_l", (char*) _savefpr_27_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefpr_28_l", (char*) _savefpr_28_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefpr_29_l", (char*) _savefpr_29_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefpr_30_l", (char*) _savefpr_30_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefpr_31_l", (char*) _savefpr_31_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprs_14_l", (char*) _savefprs_14_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprs_15_l", (char*) _savefprs_15_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprs_16_l", (char*) _savefprs_16_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprs_17_l", (char*) _savefprs_17_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprs_18_l", (char*) _savefprs_18_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprs_19_l", (char*) _savefprs_19_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprs_20_l", (char*) _savefprs_20_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprs_21_l", (char*) _savefprs_21_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprs_22_l", (char*) _savefprs_22_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprs_23_l", (char*) _savefprs_23_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprs_24_l", (char*) _savefprs_24_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprs_25_l", (char*) _savefprs_25_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprs_26_l", (char*) _savefprs_26_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprs_27_l", (char*) _savefprs_27_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprs_28_l", (char*) _savefprs_28_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprs_29_l", (char*) _savefprs_29_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprs_30_l", (char*) _savefprs_30_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprs_31_l", (char*) _savefprs_31_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprx_14_l", (char*) _savefprx_14_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprx_15_l", (char*) _savefprx_15_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprx_16_l", (char*) _savefprx_16_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprx_17_l", (char*) _savefprx_17_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprx_18_l", (char*) _savefprx_18_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprx_19_l", (char*) _savefprx_19_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprx_20_l", (char*) _savefprx_20_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprx_21_l", (char*) _savefprx_21_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprx_22_l", (char*) _savefprx_22_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprx_23_l", (char*) _savefprx_23_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprx_24_l", (char*) _savefprx_24_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprx_25_l", (char*) _savefprx_25_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprx_26_l", (char*) _savefprx_26_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprx_27_l", (char*) _savefprx_27_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprx_28_l", (char*) _savefprx_28_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprx_29_l", (char*) _savefprx_29_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprx_30_l", (char*) _savefprx_30_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savefprx_31_l", (char*) _savefprx_31_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_14", (char*) _savegpr_14, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_14_l", (char*) _savegpr_14_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_15", (char*) _savegpr_15, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_15_l", (char*) _savegpr_15_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_16", (char*) _savegpr_16, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_16_l", (char*) _savegpr_16_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_17", (char*) _savegpr_17, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_17_l", (char*) _savegpr_17_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_18", (char*) _savegpr_18, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_18_l", (char*) _savegpr_18_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_19", (char*) _savegpr_19, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_19_l", (char*) _savegpr_19_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_20", (char*) _savegpr_20, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_20_l", (char*) _savegpr_20_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_21", (char*) _savegpr_21, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_21_l", (char*) _savegpr_21_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_22", (char*) _savegpr_22, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_22_l", (char*) _savegpr_22_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_23", (char*) _savegpr_23, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_23_l", (char*) _savegpr_23_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_24", (char*) _savegpr_24, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_24_l", (char*) _savegpr_24_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_25", (char*) _savegpr_25, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_25_l", (char*) _savegpr_25_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_26", (char*) _savegpr_26, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_26_l", (char*) _savegpr_26_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_27", (char*) _savegpr_27, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_27_l", (char*) _savegpr_27_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_28", (char*) _savegpr_28, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_28_l", (char*) _savegpr_28_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_29", (char*) _savegpr_29, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_29_l", (char*) _savegpr_29_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_30", (char*) _savegpr_30, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_30_l", (char*) _savegpr_30_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_31", (char*) _savegpr_31, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegpr_31_l", (char*) _savegpr_31_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegprx_14_l", (char*) _savegprx_14_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegprx_15_l", (char*) _savegprx_15_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegprx_16_l", (char*) _savegprx_16_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegprx_17_l", (char*) _savegprx_17_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegprx_18_l", (char*) _savegprx_18_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegprx_19_l", (char*) _savegprx_19_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegprx_20_l", (char*) _savegprx_20_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegprx_21_l", (char*) _savegprx_21_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegprx_22_l", (char*) _savegprx_22_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegprx_23_l", (char*) _savegprx_23_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegprx_24_l", (char*) _savegprx_24_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegprx_25_l", (char*) _savegprx_25_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegprx_26_l", (char*) _savegprx_26_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegprx_27_l", (char*) _savegprx_27_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegprx_28_l", (char*) _savegprx_28_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegprx_29_l", (char*) _savegprx_29_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegprx_30_l", (char*) _savegprx_30_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savegprx_31_l", (char*) _savegprx_31_l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savev20", (char*) _savev20, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savev21", (char*) _savev21, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savev22", (char*) _savev22, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savev23", (char*) _savev23, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savev24", (char*) _savev24, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savev25", (char*) _savev25, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savev26", (char*) _savev26, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savev27", (char*) _savev27, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savev28", (char*) _savev28, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savev29", (char*) _savev29, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savev30", (char*) _savev30, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savev31", (char*) _savev31, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savevr20", (char*) _savevr20, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savevr21", (char*) _savevr21, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savevr22", (char*) _savevr22, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savevr23", (char*) _savevr23, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savevr24", (char*) _savevr24, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savevr25", (char*) _savevr25, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savevr26", (char*) _savevr26, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savevr27", (char*) _savevr27, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savevr28", (char*) _savevr28, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savevr29", (char*) _savevr29, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savevr30", (char*) _savevr30, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_savevr31", (char*) _savevr31, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_sb_readline__FP9streambufRlc", (char*) _sb_readline__FP9streambufRlc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_sch_istable", (char*) &_sch_istable, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_sch_tolower", (char*) &_sch_tolower, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_sch_toupper", (char*) &_sch_toupper, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_seterr_reply", (char*) _seterr_reply, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_setjmpSetup", (char*) _setjmpSetup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_sigCtxLoad", (char*) _sigCtxLoad, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_sigCtxRtnValSet", (char*) _sigCtxRtnValSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_sigCtxSave", (char*) _sigCtxSave, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_sigCtxSetup", (char*) _sigCtxSetup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_sigCtxStackEnd", (char*) _sigCtxStackEnd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_sigfaulttable", (char*) &_sigfaulttable, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_skip_ws__7istream", (char*) _skip_ws__7istream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_svcauth_null", (char*) _svcauth_null, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_svcauth_short", (char*) _svcauth_short, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_svcauth_unix", (char*) _svcauth_unix, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_sysInit", (char*) _sysInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0$nilRep", (char*) &_t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0$nilRep, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0$npos", (char*) &_t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0$npos, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_t23__malloc_alloc_template1i0$__malloc_alloc_oom_handler", (char*) &_t23__malloc_alloc_template1i0$__malloc_alloc_oom_handler, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_t24__default_alloc_template2b1i0$_S_end_free", (char*) &_t24__default_alloc_template2b1i0$_S_end_free, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_t24__default_alloc_template2b1i0$_S_free_list", (char*) &_t24__default_alloc_template2b1i0$_S_free_list, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_t24__default_alloc_template2b1i0$_S_heap_size", (char*) &_t24__default_alloc_template2b1i0$_S_heap_size, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_t24__default_alloc_template2b1i0$_S_node_allocator_lock", (char*) &_t24__default_alloc_template2b1i0$_S_node_allocator_lock, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_t24__default_alloc_template2b1i0$_S_start_free", (char*) &_t24__default_alloc_template2b1i0$_S_start_free, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_throw_failure__C3ios", (char*) _throw_failure__C3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_un_link__9streambuf", (char*) _un_link__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_utof", (char*) _utof, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_vt$10bad_typeid", (char*) &_vt$10bad_typeid, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vt$11logic_error", (char*) &_vt$11logic_error, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vt$11range_error", (char*) &_vt$11range_error, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vt$12domain_error", (char*) &_vt$12domain_error, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vt$12length_error", (char*) &_vt$12length_error, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vt$12out_of_range", (char*) &_vt$12out_of_range, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vt$13bad_exception", (char*) &_vt$13bad_exception, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vt$13runtime_error", (char*) &_vt$13runtime_error, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vt$14__si_type_info", (char*) &_vt$14__si_type_info, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vt$14overflow_error", (char*) &_vt$14overflow_error, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vt$15underflow_error", (char*) &_vt$15underflow_error, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vt$16__user_type_info", (char*) &_vt$16__user_type_info, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vt$16invalid_argument", (char*) &_vt$16invalid_argument, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vt$17__class_type_info", (char*) &_vt$17__class_type_info, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vt$22_IO_istream_withassign$3ios", (char*) &_vt$22_IO_istream_withassign$3ios, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vt$22_IO_ostream_withassign$3ios", (char*) &_vt$22_IO_ostream_withassign$3ios, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vt$3ios", (char*) &_vt$3ios, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vt$7filebuf", (char*) &_vt$7filebuf, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vt$7istream$3ios", (char*) &_vt$7istream$3ios, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vt$7ostream$3ios", (char*) &_vt$7ostream$3ios, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vt$8bad_cast", (char*) &_vt$8bad_cast, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vt$8iostream$3ios", (char*) &_vt$8iostream$3ios, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vt$9bad_alloc", (char*) &_vt$9bad_alloc, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vt$9exception", (char*) &_vt$9exception, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vt$9streambuf", (char*) &_vt$9streambuf, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vt$9type_info", (char*) &_vt$9type_info, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_vtable__9streambuf", (char*) _vtable__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_wdbTaskBpAdd", (char*) &_wdbTaskBpAdd, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_wdbTaskBpBreakpoint", (char*) &_wdbTaskBpBreakpoint, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_wdbTaskBpTrace", (char*) &_wdbTaskBpTrace, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_wdbTaskCont", (char*) &_wdbTaskCont, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_wdbTaskStep", (char*) &_wdbTaskStep, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "abort", (char*) abort, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "abs", (char*) abs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "abs__Fd", (char*) abs__Fd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "abs__Ff", (char*) abs__Ff, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "abs__Fl", (char*) abs__Fl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "abs__Fr", (char*) abs__Fr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "accept", (char*) accept, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "accepted", (char*) accepted, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "acos", (char*) acos, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "activeQHead", (char*) &activeQHead, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "addCacheControlBits", (char*) addCacheControlBits, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "addDomain", (char*) addDomain, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "addSegNames", (char*) addSegNames, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "alloc__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Uib", (char*) alloc__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Uib, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "allocate__9streambuf", (char*) allocate__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "allocate__t23__malloc_alloc_template1i0Ui", (char*) allocate__t23__malloc_alloc_template1i0Ui, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "allocate__t24__default_alloc_template2b1i0Ui", (char*) allocate__t24__default_alloc_template2b1i0Ui, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "allocbuf__9streambuf", (char*) allocbuf__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "altivecArchInit", (char*) altivecArchInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "altivecArchTaskCreateInit", (char*) altivecArchTaskCreateInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "altivecCreateHookRtn", (char*) &altivecCreateHookRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "altivecCtxToRegs", (char*) altivecCtxToRegs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "altivecDisplayHookRtn", (char*) &altivecDisplayHookRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "altivecInit", (char*) altivecInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "altivecProbe", (char*) altivecProbe, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "altivecRegsToCtx", (char*) altivecRegsToCtx, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "altivecRestore", (char*) altivecRestore, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "altivecSave", (char*) altivecSave, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "altivecShowInit", (char*) altivecShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "altivecTaskRegsGet", (char*) altivecTaskRegsGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "altivecTaskRegsSet", (char*) altivecTaskRegsSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "altivecTaskRegsShow", (char*) altivecTaskRegsShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "am29BotOffset", (char*) &am29BotOffset, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "am29TopOffset", (char*) &am29TopOffset, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "append__10RBString_TPCci", (char*) append__10RBString_TPCci, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "append__10RBString_TR10RBString_T", (char*) append__10RBString_TR10RBString_T, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "append__10RBString_Tc", (char*) append__10RBString_Tc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "append__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCc", (char*) append__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "append__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi", (char*) append__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "append__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUi", (char*) append__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "append__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Uic", (char*) append__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Uic, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "arpEntryDelete", (char*) arpEntryDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "arpMaxEntries", (char*) &arpMaxEntries, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "arpRxmitTicks", (char*) &arpRxmitTicks, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "arpShow", (char*) arpShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "arp_allocated", (char*) &arp_allocated, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "arp_intimer", (char*) &arp_intimer, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "arp_inuse", (char*) &arp_inuse, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "arp_maxtries", (char*) &arp_maxtries, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "arp_rtrequest", (char*) arp_rtrequest, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "arpinit_done", (char*) &arpinit_done, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "arpintr", (char*) arpintr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "arpintrq", (char*) &arpintrq, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "arpioctl", (char*) arpioctl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "arpresolve", (char*) arpresolve, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "arpt_down", (char*) &arpt_down, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "arpt_keep", (char*) &arpt_keep, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "arpt_prune", (char*) &arpt_prune, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "arptabShow", (char*) arptabShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "arptfree", (char*) arptfree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "arpwhohas", (char*) arpwhohas, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "asctime", (char*) asctime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "asctime_r", (char*) asctime_r, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "asin", (char*) asin, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "assertFiles", (char*) &assertFiles, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "assign__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCc", (char*) assign__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "assign__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi", (char*) assign__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "assign__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUi", (char*) assign__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "assign__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Uic", (char*) assign__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Uic, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "assign__t18string_char_traits1ZcRcRCc", (char*) assign__t18string_char_traits1ZcRcRCc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "at__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui", (char*) at__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "at__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui", (char*) at__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "atan", (char*) atan, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "atan2", (char*) atan2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "atexit", (char*) atexit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "atof", (char*) atof, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "atoi", (char*) atoi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "atol", (char*) atol, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "atox", (char*) atox, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "attach__7filebufi", (char*) attach__7filebufi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "attrib", (char*) attrib, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "authnone_create", (char*) authnone_create, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "authunix_create", (char*) authunix_create, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "b", (char*) b, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bad__C3ios", (char*) bad__C3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "base__C9streambuf", (char*) base__C9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bcmp", (char*) bcmp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bcopy", (char*) bcopy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bcopyBytes", (char*) bcopyBytes, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bcopyLongs", (char*) bcopyLongs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bcopyWords", (char*) bcopyWords, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bcopy_to_mbufs", (char*) bcopy_to_mbufs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bd", (char*) bd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bdall", (char*) bdall, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "before__C9type_infoRC9type_info", (char*) before__C9type_infoRC9type_info, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "begin__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) begin__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "begin__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) begin__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bfill", (char*) bfill, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bfillBytes", (char*) bfillBytes, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bh", (char*) bh, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bibFreeRtn", (char*) &bibFreeRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "bibInit", (char*) bibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bibMallocRtn", (char*) &bibMallocRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "bibRead", (char*) bibRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bibReadIntf", (char*) bibReadIntf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bibWrite", (char*) bibWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bibWriteIntf", (char*) bibWriteIntf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bibgIntf", (char*) &bibgIntf, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "binaryOptionsTsfsDrv", (char*) &binaryOptionsTsfsDrv, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "bind", (char*) bind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bindresvport", (char*) bindresvport, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "binvert", (char*) binvert, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "blen__C9streambuf", (char*) blen__C9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootBpAnchorExtract", (char*) bootBpAnchorExtract, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootChange", (char*) bootChange, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootEraseOrProgramOk", (char*) bootEraseOrProgramOk, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootFlashAutoSelect", (char*) bootFlashAutoSelect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootFlashErase", (char*) bootFlashErase, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootFlashGetProtect", (char*) bootFlashGetProtect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootFlashGetSectorOff", (char*) bootFlashGetSectorOff, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootFlashParamRead", (char*) bootFlashParamRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootFlashParamWrite", (char*) bootFlashParamWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootFlashParamsProgram", (char*) bootFlashParamsProgram, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootFlashProgram1byte", (char*) bootFlashProgram1byte, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootLeaseExtract", (char*) bootLeaseExtract, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootNetmaskExtract", (char*) bootNetmaskExtract, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootParamsErrorPrint", (char*) bootParamsErrorPrint, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootParamsPrompt", (char*) bootParamsPrompt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootParamsShow", (char*) bootParamsShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootScanNum", (char*) bootScanNum, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootStringToStruct", (char*) bootStringToStruct, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootStructToString", (char*) bootStructToString, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootrom_pciregfile", (char*) &bootrom_pciregfile, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "bootrom_sramregfile", (char*) &bootrom_sramregfile, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "bootrom_userflregfile", (char*) &bootrom_userflregfile, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "bp1", (char*) &bp1, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "bp2", (char*) &bp2, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "bpFreeList", (char*) &bpFreeList, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "bpList", (char*) &bpList, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "brgDbg", (char*) brgDbg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "brgInit", (char*) brgInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "brgSetCdv", (char*) brgSetCdv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "brgStart", (char*) brgStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "brg_regs_dump", (char*) brg_regs_dump, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bsdAccept", (char*) bsdAccept, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bsdBind", (char*) bsdBind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bsdConnect", (char*) bsdConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bsdConnectWithTimeout", (char*) bsdConnectWithTimeout, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bsdGetpeername", (char*) bsdGetpeername, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bsdGetsockname", (char*) bsdGetsockname, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bsdGetsockopt", (char*) bsdGetsockopt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bsdListen", (char*) bsdListen, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bsdRecv", (char*) bsdRecv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bsdRecvfrom", (char*) bsdRecvfrom, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bsdRecvmsg", (char*) bsdRecvmsg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bsdSend", (char*) bsdSend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bsdSendmsg", (char*) bsdSendmsg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bsdSendto", (char*) bsdSendto, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bsdSetsockopt", (char*) bsdSetsockopt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bsdShutdown", (char*) bsdShutdown, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bsdSock43ApiFlag", (char*) &bsdSock43ApiFlag, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "bsdSockFunc", (char*) &bsdSockFunc, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "bsdSockLibInit", (char*) bsdSockLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bsdSocket", (char*) bsdSocket, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bsdZbufSockRtn", (char*) bsdZbufSockRtn, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bsearch", (char*) bsearch, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bspEndIntConnect", (char*) &bspEndIntConnect, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "bspEndIntDisable", (char*) &bspEndIntDisable, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "bspEndIntDisconnect", (char*) &bspEndIntDisconnect, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "bspEndIntEnable", (char*) &bspEndIntEnable, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "bswap", (char*) bswap, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "buf", (char*) &buf, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "bufAlloc", (char*) bufAlloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bufFree", (char*) bufFree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bufPoolInit", (char*) bufPoolInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "buffer", (char*) &buffer, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "bufferedRtShow", (char*) &bufferedRtShow, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "build_cluster", (char*) build_cluster, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "busIoMemRanges", (char*) &busIoMemRanges, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "bzero", (char*) bzero, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "c", (char*) c, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "c_str__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) c_str__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheArchDisable", (char*) cacheArchDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheArchDisableFromMmu", (char*) cacheArchDisableFromMmu, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheArchDmaFree", (char*) cacheArchDmaFree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheArchDmaMalloc", (char*) cacheArchDmaMalloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheArchEnable", (char*) cacheArchEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheArchFlush", (char*) cacheArchFlush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheArchInvalidate", (char*) cacheArchInvalidate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheArchLibInit", (char*) cacheArchLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheArchPipeFlush", (char*) cacheArchPipeFlush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheArchTextUpdate", (char*) cacheArchTextUpdate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheClear", (char*) cacheClear, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheDToEnable", (char*) &cacheDToEnable, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cacheDataEnabled", (char*) &cacheDataEnabled, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cacheDataMode", (char*) &cacheDataMode, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cacheDisable", (char*) cacheDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheDmaFree", (char*) cacheDmaFree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheDmaFreeRtn", (char*) &cacheDmaFreeRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cacheDmaFuncs", (char*) &cacheDmaFuncs, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cacheDmaMalloc", (char*) cacheDmaMalloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheDmaMallocRtn", (char*) &cacheDmaMallocRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cacheDrvFlush", (char*) cacheDrvFlush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheDrvInvalidate", (char*) cacheDrvInvalidate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheDrvPhysToVirt", (char*) cacheDrvPhysToVirt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheDrvVirtToPhys", (char*) cacheDrvVirtToPhys, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheEnable", (char*) cacheEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheErrnoSet", (char*) cacheErrnoSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheFlush", (char*) cacheFlush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheFuncsSet", (char*) cacheFuncsSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheIToEnable", (char*) &cacheIToEnable, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cacheInvalidate", (char*) cacheInvalidate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheLib", (char*) &cacheLib, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cacheLibInit", (char*) cacheLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheLock", (char*) cacheLock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheMmuAvailable", (char*) &cacheMmuAvailable, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cacheNullFuncs", (char*) &cacheNullFuncs, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cachePipeFlush", (char*) cachePipeFlush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cachePpcDisable", (char*) cachePpcDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cachePpcEnable", (char*) cachePpcEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cachePpcReadOrigin", (char*) &cachePpcReadOrigin, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cacheTextUpdate", (char*) cacheTextUpdate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheUnlock", (char*) cacheUnlock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheUserFuncs", (char*) &cacheUserFuncs, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "calloc", (char*) calloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "callrpc", (char*) callrpc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "capacity__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) capacity__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cbioBlkCopy", (char*) cbioBlkCopy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cbioBlkRW", (char*) cbioBlkRW, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cbioBytesRW", (char*) cbioBytesRW, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cbioClass", (char*) &cbioClass, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "cbioClassId", (char*) &cbioClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cbioDevCreate", (char*) cbioDevCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cbioDevVerify", (char*) cbioDevVerify, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cbioIoctl", (char*) cbioIoctl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cbioLibInit", (char*) cbioLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cbioLock", (char*) cbioLock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cbioModeGet", (char*) cbioModeGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cbioModeSet", (char*) cbioModeSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cbioMutexSemOptions", (char*) &cbioMutexSemOptions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cbioParamsGet", (char*) cbioParamsGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cbioRdyChgdGet", (char*) cbioRdyChgdGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cbioRdyChgdSet", (char*) cbioRdyChgdSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cbioRdyChk", (char*) cbioRdyChk, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cbioShow", (char*) cbioShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cbioUnlock", (char*) cbioUnlock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cbioWrapBlkDev", (char*) cbioWrapBlkDev, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cd", (char*) cd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ceil", (char*) ceil, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cenr", (char*) &cenr, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "cerr", (char*) &cerr, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cfree", (char*) cfree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "changeReg", (char*) changeReg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "chdir", (char*) chdir, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "checkInetAddrField", (char*) checkInetAddrField, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "checkStack", (char*) checkStack, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "check_realloc__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui", (char*) check_realloc__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "check_trailer", (char*) check_trailer, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "checksum", (char*) checksum, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "chkdsk", (char*) chkdsk, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cin", (char*) &cin, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "clDescTbl", (char*) &clDescTbl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "clDescTblNumEnt", (char*) &clDescTblNumEnt, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "classClassId", (char*) &classClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "classCreate", (char*) classCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "classDestroy", (char*) classDestroy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "classInit", (char*) classInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "classInstConnect", (char*) classInstConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "classInstrument", (char*) classInstrument, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "classLibInit", (char*) classLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "classMemPartIdSet", (char*) classMemPartIdSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "classShow", (char*) classShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "classShowConnect", (char*) classShowConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "classShowInit", (char*) classShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clearIntFlag", (char*) clearIntFlag, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clear__10RBString_T", (char*) clear__10RBString_T, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clear__3iosi", (char*) clear__3iosi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clearerr", (char*) clearerr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clnt_create", (char*) clnt_create, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clnt_genericInclude", (char*) clnt_genericInclude, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clnt_pcreateerror", (char*) clnt_pcreateerror, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clnt_perrno", (char*) clnt_perrno, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clnt_perror", (char*) clnt_perror, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clnt_rawInclude", (char*) clnt_rawInclude, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clnt_simpleInclude", (char*) clnt_simpleInclude, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clnt_spcreateerror", (char*) clnt_spcreateerror, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clnt_sperrno", (char*) clnt_sperrno, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clnt_sperror", (char*) clnt_sperror, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clntraw_create", (char*) clntraw_create, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clnttcp_create", (char*) clnttcp_create, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clntudp_bufcreate", (char*) clntudp_bufcreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clntudp_create", (char*) clntudp_create, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clntudp_freeres", (char*) clntudp_freeres, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clock", (char*) clock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clockLibInit", (char*) clockLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clock_getres", (char*) clock_getres, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clock_gettime", (char*) clock_gettime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clock_setres", (char*) clock_setres, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clock_settime", (char*) clock_settime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clock_show", (char*) clock_show, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clog", (char*) &clog, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "clone__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3Rep", (char*) clone__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3Rep, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "close", (char*) close, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "close__3ios", (char*) close__3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "close__7filebuf", (char*) close__7filebuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "closedir", (char*) closedir, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "compare__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi", (char*) compare__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "compare__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUiUi", (char*) compare__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUiUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "compare__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUi", (char*) compare__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "compare__t18string_char_traits1ZcPCcT1Ui", (char*) compare__t18string_char_traits1ZcPCcT1Ui, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "compare_error", (char*) compare_error, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "connect", (char*) connect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "connectWithTimeout", (char*) connectWithTimeout, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "connectors", (char*) &connectors, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "consoleFd", (char*) &consoleFd, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "consoleName", (char*) &consoleName, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "copy", (char*) copy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "copyFromMbufs", (char*) copyFromMbufs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "copyStreams", (char*) copyStreams, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "copy__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PcUiUi", (char*) copy__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PcUiUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "copy__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepUiPCcUi", (char*) copy__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepUiPCcUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "copy__t18string_char_traits1ZcPcPCcUi", (char*) copy__t18string_char_traits1ZcPcPCcUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "copyright_wind_river", (char*) &copyright_wind_river, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "copysign", (char*) copysign, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "correctaddr", (char*) correctaddr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cos", (char*) cos, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cosh", (char*) cosh, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "count", (char*) &count, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "cout", (char*) &cout, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cp", (char*) cp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cpldGppDisablePin13", (char*) cpldGppDisablePin13, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusArraysInit", (char*) cplusArraysInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusCallCtors", (char*) cplusCallCtors, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusCallDtors", (char*) cplusCallDtors, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusCallNewHandler__Fv", (char*) cplusCallNewHandler__Fv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusCtors", (char*) cplusCtors, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusCtorsLink", (char*) cplusCtorsLink, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusDemangle", (char*) cplusDemangle, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusDemangleFunc", (char*) &cplusDemangleFunc, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cplusDemanglerInit", (char*) cplusDemanglerInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusDemanglerMode", (char*) &cplusDemanglerMode, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cplusDemanglerSet", (char*) cplusDemanglerSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusDemanglerStyle", (char*) &cplusDemanglerStyle, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "cplusDemanglerStyleSet", (char*) cplusDemanglerStyleSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusDtors", (char*) cplusDtors, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusDtorsLink", (char*) cplusDtorsLink, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusLibInit", (char*) cplusLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusLibMinInit", (char*) cplusLibMinInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusLoadFixup", (char*) cplusLoadFixup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusMatchMangled", (char*) cplusMatchMangled, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusNewHandlerExists__Fv", (char*) cplusNewHandlerExists__Fv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusNewHdlMutex", (char*) &cplusNewHdlMutex, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "cplusTerminate__Fv", (char*) cplusTerminate__Fv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusUnloadFixup", (char*) cplusUnloadFixup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusXtorSet", (char*) cplusXtorSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusXtorStrategy", (char*) &cplusXtorStrategy, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cplus_demangle", (char*) cplus_demangle, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplus_demangle_name_to_style", (char*) cplus_demangle_name_to_style, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplus_demangle_opname", (char*) cplus_demangle_opname, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplus_demangle_set_style", (char*) cplus_demangle_set_style, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplus_mangle_opname", (char*) cplus_mangle_opname, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "creat", (char*) creat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "create__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepUi", (char*) create__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "creationDate", (char*) &creationDate, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cret", (char*) cret, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ctime", (char*) ctime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ctime_r", (char*) ctime_r, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ctohex", (char*) ctohex, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ctohex1", (char*) ctohex1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ctypeFiles", (char*) &ctypeFiles, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cunitInit", (char*) cunitInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cunitSetAccessControl", (char*) cunitSetAccessControl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cur_ptr__7filebuf", (char*) cur_ptr__7filebuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "currentContext", (char*) &currentContext, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "current_demangling_style", (char*) &current_demangling_style, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "d", (char*) d, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "data__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) data__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "data__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3Rep", (char*) data__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3Rep, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dater", (char*) &dater, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "dayr", (char*) &dayr, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "db_print_sa", (char*) db_print_sa, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "db_show_arptab", (char*) db_show_arptab, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dbgHelp", (char*) dbgHelp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dbgInit", (char*) dbgInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dbgLockUnbreakable", (char*) &dbgLockUnbreakable, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "dbgPrintAltivec", (char*) &dbgPrintAltivec, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "dbgPrintCall", (char*) dbgPrintCall, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dbgPrintDsp", (char*) &dbgPrintDsp, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "dbgPrintFpp", (char*) &dbgPrintFpp, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "dbgPrintSimd", (char*) &dbgPrintSimd, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "dbgSafeUnbreakable", (char*) &dbgSafeUnbreakable, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "dbgTaskBpBreakpoint", (char*) dbgTaskBpBreakpoint, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dbgTaskBpHooksInstall", (char*) dbgTaskBpHooksInstall, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dbgTaskBpTrace", (char*) dbgTaskBpTrace, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dbgTaskCont", (char*) dbgTaskCont, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dbgTaskStep", (char*) dbgTaskStep, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dbgUnbreakableOld", (char*) &dbgUnbreakableOld, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "dcacheCtrl", (char*) &dcacheCtrl, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "dcacheDevCreate", (char*) dcacheDevCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dcacheDevDisable", (char*) dcacheDevDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dcacheDevEnable", (char*) dcacheDevEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dcacheDevMemResize", (char*) dcacheDevMemResize, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dcacheDevTune", (char*) dcacheDevTune, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dcacheShow", (char*) dcacheShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dcacheUpd", (char*) dcacheUpd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dcacheUpdTaskId", (char*) &dcacheUpdTaskId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "dcacheUpdTaskOptions", (char*) &dcacheUpdTaskOptions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "dcacheUpdTaskPriority", (char*) &dcacheUpdTaskPriority, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "dcacheUpdTaskStack", (char*) &dcacheUpdTaskStack, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "dcast__C14__si_type_infoRC9type_infoiPvPC9type_infoT3", (char*) dcast__C14__si_type_infoRC9type_infoiPvPC9type_infoT3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dcast__C16__user_type_infoRC9type_infoiPvPC9type_infoT3", (char*) dcast__C16__user_type_infoRC9type_infoiPvPC9type_infoT3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dcast__C17__class_type_infoRC9type_infoiPvPC9type_infoT3", (char*) dcast__C17__class_type_infoRC9type_infoiPvPC9type_infoT3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "deallocate__t23__malloc_alloc_template1i0PvUi", (char*) deallocate__t23__malloc_alloc_template1i0PvUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "deallocate__t24__default_alloc_template2b1i0PvUi", (char*) deallocate__t24__default_alloc_template2b1i0PvUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "debug_flash_width", (char*) &debug_flash_width, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "dec__FR3ios", (char*) dec__FR3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "decode_identifier", (char*) decode_identifier, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "decode_identifier_styled", (char*) decode_identifier_styled, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "delta__12streammarker", (char*) delta__12streammarker, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "delta__12streammarkerR12streammarker", (char*) delta__12streammarkerR12streammarker, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "detectRAMsize", (char*) detectRAMsize, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "detectUserFlash", (char*) detectUserFlash, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "devs", (char*) devs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "difftime", (char*) difftime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dirList", (char*) dirList, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "diskFormat", (char*) diskFormat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "diskInit", (char*) diskInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "div", (char*) div, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "div_r", (char*) div_r, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dllAdd", (char*) dllAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dllCount", (char*) dllCount, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dllCreate", (char*) dllCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dllDelete", (char*) dllDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dllEach", (char*) dllEach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dllGet", (char*) dllGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dllInit", (char*) dllInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dllInsert", (char*) dllInsert, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dllRemove", (char*) dllRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dllTerminate", (char*) dllTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "do_osfx__7ostream", (char*) do_osfx__7ostream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "do_protocol_with_type", (char*) do_protocol_with_type, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "doallocate__7filebuf", (char*) doallocate__7filebuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "doallocate__9streambuf", (char*) doallocate__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "doallocbuf__9streambuf", (char*) doallocbuf__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "domaininit", (char*) domaininit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "domains", (char*) &domains, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "dosChkDebug", (char*) &dosChkDebug, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "dosChkDsk", (char*) dosChkDsk, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosChkEntryMark", (char*) dosChkEntryMark, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosChkLibInit", (char*) dosChkLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosChkMinDate", (char*) &dosChkMinDate, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "dosDirHdlrsList", (char*) &dosDirHdlrsList, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "dosDirOldDebug", (char*) &dosDirOldDebug, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "dosDirOldLibInit", (char*) dosDirOldLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFatHdlrsList", (char*) &dosFatHdlrsList, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "dosFsCacheSizeDefault", (char*) &dosFsCacheSizeDefault, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "dosFsChkDsk", (char*) dosFsChkDsk, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFsChkRtn", (char*) &dosFsChkRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "dosFsChkTree", (char*) dosFsChkTree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFsConfigGet", (char*) dosFsConfigGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFsConfigInit", (char*) dosFsConfigInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFsConfigShow", (char*) dosFsConfigShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFsDateTimeInstall", (char*) dosFsDateTimeInstall, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFsDebug", (char*) &dosFsDebug, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "dosFsDevCreate", (char*) dosFsDevCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFsDevInit", (char*) dosFsDevInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFsDevInitOptionsSet", (char*) dosFsDevInitOptionsSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFsDrvNum", (char*) &dosFsDrvNum, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "dosFsFatInit", (char*) dosFsFatInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFsFmtLibInit", (char*) dosFsFmtLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFsHdlrInstall", (char*) dosFsHdlrInstall, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFsInit", (char*) dosFsInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFsLastAccessDateEnable", (char*) dosFsLastAccessDateEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFsLibInit", (char*) dosFsLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFsMkfs", (char*) dosFsMkfs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFsMkfsOptionsSet", (char*) dosFsMkfsOptionsSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFsModeChange", (char*) dosFsModeChange, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFsReadyChange", (char*) dosFsReadyChange, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFsShow", (char*) dosFsShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFsVolDescGet", (char*) dosFsVolDescGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFsVolFormat", (char*) dosFsVolFormat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFsVolFormatRtn", (char*) &dosFsVolFormatRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "dosFsVolIsFat12", (char*) dosFsVolIsFat12, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFsVolOptionsGet", (char*) dosFsVolOptionsGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFsVolOptionsSet", (char*) dosFsVolOptionsSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosFsVolUnmount", (char*) dosFsVolUnmount, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosSetVolCaseSens", (char*) dosSetVolCaseSens, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dosVDirDebug", (char*) &dosVDirDebug, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "dosVDirLibInit", (char*) dosVDirLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "drem", (char*) drem, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "drvCtrl", (char*) &drvCtrl, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "drvTable", (char*) &drvTable, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "dsmInst", (char*) dsmInst, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "e", (char*) e, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eBptr__9streambuf", (char*) eBptr__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eGptr__9streambuf", (char*) eGptr__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eNptr__9streambuf", (char*) eNptr__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eback__C9streambuf", (char*) eback__C9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ebuf__C9streambuf", (char*) ebuf__C9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "egptr__C9streambuf", (char*) egptr__C9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "elfRelocRelEntryRd", (char*) elfRelocRelEntryRd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "elfRelocRelaEntryRd", (char*) elfRelocRelaEntryRd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "empty__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) empty__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "end8023AddressForm", (char*) end8023AddressForm, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "endDevName", (char*) endDevName, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "endDevTbl", (char*) &endDevTbl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "endEtherAddressForm", (char*) endEtherAddressForm, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "endEtherPacketAddrGet", (char*) endEtherPacketAddrGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "endEtherPacketDataGet", (char*) endEtherPacketDataGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "endFindByName", (char*) endFindByName, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "endFlagsClr", (char*) endFlagsClr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "endFlagsGet", (char*) endFlagsGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "endFlagsSet", (char*) endFlagsSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "endMibIfInit", (char*) endMibIfInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "endMultiLstCnt", (char*) endMultiLstCnt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "endMultiLstFirst", (char*) endMultiLstFirst, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "endMultiLstNext", (char*) endMultiLstNext, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "endObjFlagSet", (char*) endObjFlagSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "endObjInit", (char*) endObjInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "endObjectUnload", (char*) endObjectUnload, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "endPollStatsInit", (char*) endPollStatsInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "endRcvRtnCall", (char*) endRcvRtnCall, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "endTxSemGive", (char*) endTxSemGive, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "endTxSemTake", (char*) endTxSemTake, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "end__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) end__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "end__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) end__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "end_addr", (char*) &end_addr, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "endl__FR7ostream", (char*) endl__FR7ostream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ends__FR7ostream", (char*) ends__FR7ostream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "envLibInit", (char*) envLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "envPrivateCreate", (char*) envPrivateCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "envPrivateDestroy", (char*) envPrivateDestroy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "envShow", (char*) envShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eof__C3ios", (char*) eof__C3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eos__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) eos__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eos__t18string_char_traits1Zc", (char*) eos__t18string_char_traits1Zc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "epptr__C9streambuf", (char*) epptr__C9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eq__t18string_char_traits1ZcRCcT1", (char*) eq__t18string_char_traits1ZcRCcT1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "erase__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Pc", (char*) erase__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Pc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "erase__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PcT1", (char*) erase__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PcT1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "erase__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUi", (char*) erase__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "errno", (char*) &errno, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "errnoGet", (char*) errnoGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "errnoOfTaskGet", (char*) errnoOfTaskGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "errnoOfTaskSet", (char*) errnoOfTaskSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "errnoSet", (char*) errnoSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ethBCopy", (char*) ethBCopy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ethClearMibCounters", (char*) ethClearMibCounters, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ethPortInit", (char*) ethPortInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ethPortMcAddr", (char*) ethPortMcAddr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ethPortReadSmiReg", (char*) ethPortReadSmiReg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ethPortReceive", (char*) ethPortReceive, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ethPortReset", (char*) ethPortReset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ethPortSend", (char*) ethPortSend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ethPortSetRxCoal", (char*) ethPortSetRxCoal, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ethPortSetTxCoal", (char*) ethPortSetTxCoal, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ethPortStart", (char*) ethPortStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ethPortUcAddrSet", (char*) ethPortUcAddrSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ethPortWriteSmiReg", (char*) ethPortWriteSmiReg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ethReadMibCounter", (char*) ethReadMibCounter, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ethRxReturnBuff", (char*) ethRxReturnBuff, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ethTxReturnDesc", (char*) ethTxReturnDesc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "etherInitRxDescRing", (char*) etherInitRxDescRing, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "etherInitTxDescRing", (char*) etherInitTxDescRing, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "etherMultiAdd", (char*) etherMultiAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "etherMultiDebug", (char*) &etherMultiDebug, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "etherMultiDel", (char*) etherMultiDel, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "etherMultiGet", (char*) etherMultiGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ether_addmulti", (char*) ether_addmulti, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ether_attach", (char*) ether_attach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ether_delmulti", (char*) ether_delmulti, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ether_ifattach", (char*) ether_ifattach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ether_input", (char*) ether_input, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ether_ipmulticast_max", (char*) &ether_ipmulticast_max, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ether_ipmulticast_min", (char*) &ether_ipmulticast_min, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ether_output", (char*) ether_output, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ether_sprintf", (char*) ether_sprintf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "etherbroadcastaddr", (char*) &etherbroadcastaddr, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ethernetGetConfigReg", (char*) ethernetGetConfigReg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ethernetPhyReset", (char*) ethernetPhyReset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ethernetResetConfigReg", (char*) ethernetResetConfigReg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ethernetSetConfigReg", (char*) ethernetSetConfigReg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eventClear", (char*) eventClear, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eventEvtRtn", (char*) &eventEvtRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "eventInit", (char*) eventInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eventLibInit", (char*) eventLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eventReceive", (char*) eventReceive, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eventRsrcSend", (char*) eventRsrcSend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eventRsrcShow", (char*) eventRsrcShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eventSend", (char*) eventSend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eventStart", (char*) eventStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eventTaskShow", (char*) eventTaskShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eventTerminate", (char*) eventTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtAction", (char*) &evtAction, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "evtBufferBind", (char*) evtBufferBind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtBufferId", (char*) &evtBufferId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "evtLogFuncBind", (char*) evtLogFuncBind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtLogM0", (char*) evtLogM0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtLogM1", (char*) evtLogM1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtLogM2", (char*) evtLogM2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtLogM3", (char*) evtLogM3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtLogO", (char*) evtLogO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtLogOInt", (char*) evtLogOInt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtLogPoint", (char*) evtLogPoint, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtLogString", (char*) evtLogString, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtLogT0", (char*) evtLogT0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtLogT0_noInt", (char*) evtLogT0_noInt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtLogT1", (char*) evtLogT1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtLogT1_noTS", (char*) evtLogT1_noTS, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtLogTasks", (char*) evtLogTasks, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtObjLogFuncBind", (char*) evtObjLogFuncBind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtTimeStamp", (char*) &evtTimeStamp, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "evtsched", (char*) evtsched, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excConnect", (char*) excConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excEPSet", (char*) excEPSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excEnt", (char*) excEnt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excExcHandle", (char*) excExcHandle, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excExcepHook", (char*) &excExcepHook, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "excExit", (char*) excExit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excExtendedVectors", (char*) &excExtendedVectors, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "excHookAdd", (char*) excHookAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excInit", (char*) excInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excIntConnect", (char*) excIntConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excIntHandle", (char*) excIntHandle, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excJobAdd", (char*) excJobAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excMsgQId", (char*) &excMsgQId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "excRelocConnect", (char*) excRelocConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excRelocIntConnect", (char*) excRelocIntConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excShowInit", (char*) excShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excTask", (char*) excTask, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excTaskId", (char*) &excTaskId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "excTaskOptions", (char*) &excTaskOptions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "excTaskPriority", (char*) &excTaskPriority, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "excTaskStackSize", (char*) &excTaskStackSize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "excVecBaseGet", (char*) excVecBaseGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excVecBaseSet", (char*) excVecBaseSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excVecGet", (char*) excVecGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excVecInit", (char*) excVecInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excVecSet", (char*) excVecSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "exceptions__3iosi", (char*) exceptions__3iosi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "exceptions__C3ios", (char*) exceptions__C3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excess_slop__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepUiUi", (char*) excess_slop__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepUiUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "execute", (char*) execute, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "exit", (char*) exit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "exp", (char*) exp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "exp__E", (char*) exp__E, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "expm1", (char*) expm1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "extendedTestError", (char*) extendedTestError, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "extractCString__10RBString_TPci", (char*) extractCString__10RBString_TPci, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fabs", (char*) fabs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fail__C3ios", (char*) fail__C3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fastUdpInitialized", (char*) &fastUdpInitialized, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "fat16ClustValueGet", (char*) fat16ClustValueGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fat16ClustValueSet", (char*) fat16ClustValueSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fat16Debug", (char*) &fat16Debug, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "fat16VolMount", (char*) fat16VolMount, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fatClugFac", (char*) &fatClugFac, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "fclose", (char*) fclose, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fd", (char*) &fd, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "fdTable", (char*) &fdTable, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "fd__C7filebuf", (char*) fd__C7filebuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fdopen", (char*) fdopen, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fdprintf", (char*) fdprintf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fei82557Int", (char*) fei82557Int, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fei82557MDIRead", (char*) fei82557MDIRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fei82557MDIWrite", (char*) fei82557MDIWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "feiDevices", (char*) &feiDevices, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "feiEndIntConnect", (char*) &feiEndIntConnect, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "feiEndIntDisconnect", (char*) &feiEndIntDisconnect, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "feiResources", (char*) &feiResources, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "feiShow", (char*) feiShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "feiTxStartup", (char*) feiTxStartup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "feidevices", (char*) &feidevices, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "feof", (char*) feof, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ferror", (char*) ferror, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fflush", (char*) fflush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ffsMsb", (char*) ffsMsb, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fgetc", (char*) fgetc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fgetpos", (char*) fgetpos, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fgets", (char*) fgets, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fieldSzIncludeSign", (char*) &fieldSzIncludeSign, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "fileUpPathDefaultPerm", (char*) &fileUpPathDefaultPerm, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "fileUploadPathClose", (char*) fileUploadPathClose, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fileUploadPathCreate", (char*) fileUploadPathCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fileUploadPathLibInit", (char*) fileUploadPathLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fileUploadPathWrite", (char*) fileUploadPathWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "file_ptr__7filebuf", (char*) file_ptr__7filebuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fileno", (char*) fileno, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fill__3ioss", (char*) fill__3ioss, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fill__C3ios", (char*) fill__C3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "find__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi", (char*) find__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "find__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUiUi", (char*) find__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUiUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "find__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui", (char*) find__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "find__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0cUi", (char*) find__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0cUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "find_first_not_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi", (char*) find_first_not_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "find_first_not_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUiUi", (char*) find_first_not_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUiUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "find_first_not_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui", (char*) find_first_not_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "find_first_not_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0cUi", (char*) find_first_not_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0cUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "find_first_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi", (char*) find_first_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "find_first_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUiUi", (char*) find_first_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUiUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "find_first_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui", (char*) find_first_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "find_first_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0cUi", (char*) find_first_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0cUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "find_last_not_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi", (char*) find_last_not_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "find_last_not_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUiUi", (char*) find_last_not_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUiUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "find_last_not_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui", (char*) find_last_not_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "find_last_not_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0cUi", (char*) find_last_not_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0cUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "find_last_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi", (char*) find_last_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "find_last_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUiUi", (char*) find_last_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUiUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "find_last_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui", (char*) find_last_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "find_last_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0cUi", (char*) find_last_of__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0cUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "finite", (char*) finite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fioFltInstall", (char*) fioFltInstall, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fioFormatV", (char*) fioFormatV, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fioLibInit", (char*) fioLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fioRdString", (char*) fioRdString, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fioRead", (char*) fioRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fioScanV", (char*) fioScanV, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "flags__3iosUl", (char*) flags__3iosUl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "flags__C3ios", (char*) flags__C3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "flashAutoDetect", (char*) flashAutoDetect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "flashBlocks", (char*) &flashBlocks, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "flashBootSectorInit", (char*) flashBootSectorInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "flashBuf", (char*) &flashBuf, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "flashBufNV", (char*) &flashBufNV, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "flash_index", (char*) &flash_index, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "flash_read", (char*) &flash_read, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "flash_width", (char*) &flash_width, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "flash_write", (char*) &flash_write, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "floatInit", (char*) floatInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "flocalBuf", (char*) &flocalBuf, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "floor", (char*) floor, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "flush__7ostream", (char*) flush__7ostream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "flush__FR7ostream", (char*) flush__FR7ostream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "flush_all__9streambuf", (char*) flush_all__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "flush_all_linebuffered__9streambuf", (char*) flush_all_linebuffered__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fmod", (char*) fmod, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fopen", (char*) fopen, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fpClassId", (char*) &fpClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "fpCtlRegName", (char*) &fpCtlRegName, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "fpExcMsgTbl", (char*) &fpExcMsgTbl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "fpRegName", (char*) &fpRegName, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "fpTypeGet", (char*) fpTypeGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fppArchInit", (char*) fppArchInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fppArchTaskCreateInit", (char*) fppArchTaskCreateInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fppCreateHookRtn", (char*) &fppCreateHookRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "fppCtxToRegs", (char*) fppCtxToRegs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fppDisplayHookRtn", (char*) &fppDisplayHookRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "fppInit", (char*) fppInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fppProbe", (char*) fppProbe, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fppRegsToCtx", (char*) fppRegsToCtx, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fppRestore", (char*) fppRestore, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fppSave", (char*) fppSave, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fppShowInit", (char*) fppShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fppTaskRegsCFmt", (char*) &fppTaskRegsCFmt, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "fppTaskRegsDFmt", (char*) &fppTaskRegsDFmt, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "fppTaskRegsGet", (char*) fppTaskRegsGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fppTaskRegsSet", (char*) fppTaskRegsSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fppTaskRegsShow", (char*) fppTaskRegsShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fprintf", (char*) fprintf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fputc", (char*) fputc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fputs", (char*) fputs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcAlignedMalloc", (char*) frcAlignedMalloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcBibAttach", (char*) frcBibAttach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcBibDataShow", (char*) frcBibDataShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcBibDataWrite", (char*) frcBibDataWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcBibDataWriteBin", (char*) frcBibDataWriteBin, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcBibDetach", (char*) frcBibDetach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcBibDrvShow", (char*) frcBibDrvShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcBibElemRead", (char*) frcBibElemRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcBibElemShow", (char*) frcBibElemShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcBibElemWrite", (char*) frcBibElemWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcBibErrnoGet", (char*) frcBibErrnoGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcBibRead", (char*) frcBibRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcBibShow", (char*) frcBibShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcBibShowRtn", (char*) &frcBibShowRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "frcBibWrite", (char*) frcBibWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcBoardIdInit", (char*) frcBoardIdInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcBootFlashFile", (char*) frcBootFlashFile, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcBootFlashFileChipErase", (char*) frcBootFlashFileChipErase, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcBootFlashFileV", (char*) frcBootFlashFileV, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcBootFlashProgram", (char*) frcBootFlashProgram, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcBootFlashSectorErase", (char*) frcBootFlashSectorErase, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcBootFlashVerify", (char*) frcBootFlashVerify, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcBootRegFileBurn", (char*) frcBootRegFileBurn, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcCPUClockGet", (char*) frcCPUClockGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcCheckSum", (char*) frcCheckSum, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcDispBoardInfo", (char*) frcDispBoardInfo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcDmaCompleteAppIntHandler0", (char*) frcDmaCompleteAppIntHandler0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcDmaCompleteAppIntHandler1", (char*) frcDmaCompleteAppIntHandler1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcDmaIntConnect", (char*) frcDmaIntConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcDmaIntCtrlInit", (char*) frcDmaIntCtrlInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcDmaIntDisable", (char*) frcDmaIntDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcDmaIntEnable", (char*) frcDmaIntEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcDmaTestStart0", (char*) frcDmaTestStart0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcDmaTestStart1", (char*) frcDmaTestStart1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcDmaTestTaskEntry0", (char*) frcDmaTestTaskEntry0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcDmaTestTaskEntry1", (char*) frcDmaTestTaskEntry1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcEEPROMRead16", (char*) frcEEPROMRead16, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcEEPROMRead8", (char*) frcEEPROMRead8, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcEEPROMTestRead", (char*) frcEEPROMTestRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcEEPROMTestWrite", (char*) frcEEPROMTestWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcEEPROMWrite16", (char*) frcEEPROMWrite16, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcEEPROMWrite8", (char*) frcEEPROMWrite8, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcFei82557EndLoad", (char*) frcFei82557EndLoad, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcFlashAutoSelect", (char*) frcFlashAutoSelect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcFlashBlockErase", (char*) frcFlashBlockErase, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcFlashBlockWrite", (char*) frcFlashBlockWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcFlashErase", (char*) frcFlashErase, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcFlashGetInfo", (char*) frcFlashGetInfo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcFlashLock", (char*) frcFlashLock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcFlashProgram", (char*) frcFlashProgram, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcFlashRead", (char*) frcFlashRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcFlashReadIdCommand", (char*) frcFlashReadIdCommand, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcFlashReadRst", (char*) frcFlashReadRst, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcFlashUnlock", (char*) frcFlashUnlock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcFlashUnlockComplete", (char*) frcFlashUnlockComplete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcGetBank0MemSize", (char*) frcGetBank0MemSize, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcGetBank1MemSize", (char*) frcGetBank1MemSize, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcGetExtSramSize", (char*) frcGetExtSramSize, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcGetFlashOrg", (char*) frcGetFlashOrg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcGetPcbRev", (char*) frcGetPcbRev, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcGetUserFlashSize", (char*) frcGetUserFlashSize, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcGetVariant", (char*) frcGetVariant, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcGppCPU1IntDisable", (char*) frcGppCPU1IntDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcGppCPU1IntEnable", (char*) frcGppCPU1IntEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcGppIntConnect", (char*) frcGppIntConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcGppIntCtrlInit", (char*) frcGppIntCtrlInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcGppIntDisable", (char*) frcGppIntDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcGppIntEnable", (char*) frcGppIntEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcI2CWrite", (char*) frcI2CWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcIdmaTestInit0", (char*) frcIdmaTestInit0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcIdmaTestInit1", (char*) frcIdmaTestInit1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcMACRead", (char*) frcMACRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcMACWrite", (char*) frcMACWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcMemTest", (char*) frcMemTest, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcModNumBase", (char*) &frcModNumBase, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "frcMv64360SiliconVersion", (char*) frcMv64360SiliconVersion, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcPCIDataWrite", (char*) frcPCIDataWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcPci0ReadConfigReg", (char*) frcPci0ReadConfigReg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcPci0WriteConfigReg", (char*) frcPci0WriteConfigReg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcPciConfigRead", (char*) &frcPciConfigRead, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "frcPciConfigReadVPD", (char*) frcPciConfigReadVPD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcPciConfigWrite", (char*) &frcPciConfigWrite, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "frcPciConfigWriteVPD", (char*) frcPciConfigWriteVPD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcPciSetActive", (char*) frcPciSetActive, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcPciShow", (char*) frcPciShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcPrintBank0MemSize", (char*) frcPrintBank0MemSize, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcPrintBank1MemSize", (char*) frcPrintBank1MemSize, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcPrintBoardInfo", (char*) frcPrintBoardInfo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcPrintBomRev", (char*) frcPrintBomRev, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcPrintMv64360Version", (char*) frcPrintMv64360Version, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcPrintPcbRev", (char*) frcPrintPcbRev, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcPrintUserFlashSize", (char*) frcPrintUserFlashSize, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcPrintVariant", (char*) frcPrintVariant, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcRTCDummyWrite1", (char*) frcRTCDummyWrite1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcRTCRead", (char*) frcRTCRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcRTCWrite", (char*) frcRTCWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcSys557Init", (char*) frcSys557Init, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcSysBoardId", (char*) &frcSysBoardId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "frcSysTest", (char*) frcSysTest, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestAllTests", (char*) frcTestAllTests, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestAltivec", (char*) frcTestAltivec, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestBIB", (char*) frcTestBIB, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestBaudRate", (char*) frcTestBaudRate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestBootFlash", (char*) frcTestBootFlash, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestChkECC", (char*) frcTestChkECC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestConfigEthPorts", (char*) frcTestConfigEthPorts, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestConnect", (char*) frcTestConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestDMA", (char*) frcTestDMA, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestDisable", (char*) frcTestDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestDispAllPCI0Reg", (char*) frcTestDispAllPCI0Reg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestDispFreq", (char*) frcTestDispFreq, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestDispMemMap", (char*) frcTestDispMemMap, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestDispReg", (char*) frcTestDispReg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestDualCPU", (char*) frcTestDualCPU, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestEthLinkDetails", (char*) frcTestEthLinkDetails, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestEthModeSet", (char*) frcTestEthModeSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestEthPHYTestMode", (char*) frcTestEthPHYTestMode, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestEthernet", (char*) frcTestEthernet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestGettimeUsec", (char*) frcTestGettimeUsec, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestI2C_EEPROM", (char*) frcTestI2C_EEPROM, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestIDMAScrubChk", (char*) frcTestIDMAScrubChk, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestIntLatHard", (char*) frcTestIntLatHard, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestIntLatSoft", (char*) frcTestIntLatSoft, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestIntLatency", (char*) frcTestIntLatency, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestIsMonarch", (char*) frcTestIsMonarch, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestLatInitialization", (char*) frcTestLatInitialization, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestMACAddr", (char*) frcTestMACAddr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestMPP18ISR_hard", (char*) frcTestMPP18ISR_hard, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestMPP18ISR_soft", (char*) frcTestMPP18ISR_soft, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestMV64360Semaphore", (char*) frcTestMV64360Semaphore, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestMemPerf", (char*) frcTestMemPerf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestMemTest", (char*) frcTestMemTest, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestPciScan", (char*) frcTestPciScan, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestRTC", (char*) frcTestRTC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestRTCRead", (char*) frcTestRTCRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestRTCWrite", (char*) frcTestRTCWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestSerial", (char*) frcTestSerial, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestSerialInfo", (char*) frcTestSerialInfo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestStressSerial", (char*) frcTestStressSerial, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestSubsysVendID", (char*) frcTestSubsysVendID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestSummary", (char*) frcTestSummary, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestSwitch", (char*) frcTestSwitch, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestTbReset", (char*) frcTestTbReset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestUserFlash", (char*) frcTestUserFlash, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestVPD", (char*) frcTestVPD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestWatchDogAppStart", (char*) frcTestWatchDogAppStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestWatchDogAppStop", (char*) frcTestWatchDogAppStop, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestWdInitConnect", (char*) &frcTestWdInitConnect, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "frcTestWdInitDisconnect", (char*) &frcTestWdInitDisconnect, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "frcTestWhoAmI", (char*) frcTestWhoAmI, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcTestgettimeofday", (char*) frcTestgettimeofday, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcVPDInit", (char*) frcVPDInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcVPDRead", (char*) frcVPDRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcVPDWrite", (char*) frcVPDWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcWatchdogDisable", (char*) frcWatchdogDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcWatchdogEnable", (char*) frcWatchdogEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcWatchdogInit", (char*) frcWatchdogInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcWatchdogLoad", (char*) frcWatchdogLoad, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcWatchdogNMILoad", (char*) frcWatchdogNMILoad, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frcWatchdogService", (char*) frcWatchdogService, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frc_end_of_vxworks", (char*) &frc_end_of_vxworks, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "frcbootFlashReadRst", (char*) frcbootFlashReadRst, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frccachePpcEnable", (char*) frccachePpcEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fread", (char*) fread, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "free", (char*) free, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "free_backup_area__9streambuf", (char*) free_backup_area__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "freopen", (char*) freopen, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frexp", (char*) frexp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frob_size__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepUi", (char*) frob_size__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fscanf", (char*) fscanf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fseek", (char*) fseek, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fsetpos", (char*) fsetpos, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fst", (char*) fst, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fstat", (char*) fstat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fstatfs", (char*) fstatfs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ftell", (char*) ftell, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ftpCommand", (char*) ftpCommand, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ftpCommandEnhanced", (char*) ftpCommandEnhanced, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ftpDataConnGet", (char*) ftpDataConnGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ftpDataConnInit", (char*) ftpDataConnInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ftpDataConnInitPassiveMode", (char*) ftpDataConnInitPassiveMode, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ftpHookup", (char*) ftpHookup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ftpLibDebugOptionsSet", (char*) ftpLibDebugOptionsSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ftpLogin", (char*) ftpLogin, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ftpLs", (char*) ftpLs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ftpReplyGet", (char*) ftpReplyGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ftpReplyGetEnhanced", (char*) ftpReplyGetEnhanced, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ftpTransientConfigGet", (char*) ftpTransientConfigGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ftpTransientConfigSet", (char*) ftpTransientConfigSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ftpTransientFatalInstall", (char*) ftpTransientFatalInstall, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ftpXfer", (char*) ftpXfer, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ftpdDebug", (char*) &ftpdDebug, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ftpdDelete", (char*) ftpdDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ftpdInit", (char*) ftpdInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ftpdTaskOptions", (char*) &ftpdTaskOptions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ftpdTaskPriority", (char*) &ftpdTaskPriority, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ftpdWindowSize", (char*) &ftpdWindowSize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ftpdWorkTaskOptions", (char*) &ftpdWorkTaskOptions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ftpdWorkTaskPriority", (char*) &ftpdWorkTaskPriority, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ftpdWorkTaskStackSize", (char*) &ftpdWorkTaskStackSize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ftplDebug", (char*) &ftplDebug, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ftplPasvModeDisable", (char*) &ftplPasvModeDisable, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ftplTransientMaxRetryCount", (char*) &ftplTransientMaxRetryCount, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ftplTransientRetryInterval", (char*) &ftplTransientRetryInterval, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ftpsCurrentClients", (char*) &ftpsCurrentClients, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "ftpsMaxClients", (char*) &ftpsMaxClients, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "fwrite", (char*) fwrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gbump__9streambufi", (char*) gbump__9streambufi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gcount__7istream", (char*) gcount__7istream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gei82543EndLoad", (char*) gei82543EndLoad, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "geiEndLoadStr", (char*) &geiEndLoadStr, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "geiResources", (char*) &geiResources, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "geidevices", (char*) &geidevices, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "getFlashBase", (char*) getFlashBase, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "getOption", (char*) getOption, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "getPLLCoreMultiplier", (char*) getPLLCoreMultiplier, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "getPMC280BoardInfo", (char*) getPMC280BoardInfo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "getParams", (char*) getParams, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "get__7istream", (char*) get__7istream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "get__7istreamPScic", (char*) get__7istreamPScic, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "get__7istreamPUcic", (char*) get__7istreamPUcic, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "get__7istreamPcic", (char*) get__7istreamPcic, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "get__7istreamRSc", (char*) get__7istreamRSc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "get__7istreamRUc", (char*) get__7istreamRUc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "get__7istreamRc", (char*) get__7istreamRc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "get_column__9streambuf", (char*) get_column__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "get_mem", (char*) get_mem, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "get_myaddress", (char*) get_myaddress, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "get_word", (char*) get_word, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "getc", (char*) getc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "getchar", (char*) getchar, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "getcwd", (char*) getcwd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "getenv", (char*) getenv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gethostname", (char*) gethostname, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "getline__7istreamPScic", (char*) getline__7istreamPScic, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "getline__7istreamPUcic", (char*) getline__7istreamPUcic, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "getline__7istreamPcic", (char*) getline__7istreamPcic, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "getline__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_R7istreamRt12basic_string3ZX01ZX11ZX21X01_R7istream", (char*) getline__H3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_R7istreamRt12basic_string3ZX01ZX11ZX21X01_R7istream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "getpeername", (char*) getpeername, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gets", (char*) gets, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gets__7istreamPPcc", (char*) gets__7istreamPPcc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "getsockname", (char*) getsockname, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "getsockopt", (char*) getsockopt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gettime", (char*) gettime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "getw", (char*) getw, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "getwd", (char*) getwd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gmtime", (char*) gmtime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gmtime_r", (char*) gmtime_r, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "good__C3ios", (char*) good__C3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gptr__C9streambuf", (char*) gptr__C9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "grab__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3Rep", (char*) grab__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3Rep, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtCntmrDisable", (char*) gtCntmrDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtCntmrEnable", (char*) gtCntmrEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtCntmrIsTerminate", (char*) gtCntmrIsTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtCntmrLoad", (char*) gtCntmrLoad, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtCntmrRead", (char*) gtCntmrRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtCntmrSetMode", (char*) gtCntmrSetMode, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtCntmrSetTCTclocks", (char*) gtCntmrSetTCTclocks, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtCntmrStart", (char*) gtCntmrStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtDmaCommand", (char*) gtDmaCommand, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtDmaEngineDisable", (char*) gtDmaEngineDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtDmaIsChannelActive", (char*) gtDmaIsChannelActive, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtDmaSetEngineAccessProtect", (char*) gtDmaSetEngineAccessProtect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtDmaSetMemorySpace", (char*) gtDmaSetMemorySpace, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtDmaSetMemorySpaceAttr", (char*) gtDmaSetMemorySpaceAttr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtDmaStart", (char*) gtDmaStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtDmaTransfer", (char*) gtDmaTransfer, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtDmaUpdateArbiter", (char*) gtDmaUpdateArbiter, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtInit", (char*) gtInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtInitSdramSize", (char*) gtInitSdramSize, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtInitWindows", (char*) gtInitWindows, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtIntConnect", (char*) gtIntConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtIntCtrlInit", (char*) gtIntCtrlInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtIntDisable", (char*) gtIntDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtIntDisconnect", (char*) gtIntDisconnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtIntEnable", (char*) gtIntEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtInternalRegBaseAddr", (char*) &gtInternalRegBaseAddr, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "gtMemoryDisableProtectRegion", (char*) gtMemoryDisableProtectRegion, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtMemoryDisableWindow", (char*) gtMemoryDisableWindow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtMemoryEnableWindow", (char*) gtMemoryEnableWindow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtMemoryGetBankBaseAddress", (char*) gtMemoryGetBankBaseAddress, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtMemoryGetBankSize", (char*) gtMemoryGetBankSize, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtMemoryGetDeviceBaseAddress", (char*) gtMemoryGetDeviceBaseAddress, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtMemoryGetDeviceParam", (char*) gtMemoryGetDeviceParam, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtMemoryGetDeviceSize", (char*) gtMemoryGetDeviceSize, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtMemoryGetDeviceWidth", (char*) gtMemoryGetDeviceWidth, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtMemoryGetInternalRegistersSpace", (char*) gtMemoryGetInternalRegistersSpace, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtMemoryGetInternalSramBaseAddr", (char*) gtMemoryGetInternalSramBaseAddr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtMemoryGetMemWindowStatus", (char*) gtMemoryGetMemWindowStatus, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtMemoryMapBank0", (char*) gtMemoryMapBank0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtMemoryMapBank1", (char*) gtMemoryMapBank1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtMemoryMapBank2", (char*) gtMemoryMapBank2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtMemoryMapBank3", (char*) gtMemoryMapBank3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtMemoryMapDevice0Space", (char*) gtMemoryMapDevice0Space, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtMemoryMapDevice1Space", (char*) gtMemoryMapDevice1Space, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtMemoryMapDevice2Space", (char*) gtMemoryMapDevice2Space, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtMemoryMapDevice3Space", (char*) gtMemoryMapDevice3Space, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtMemoryMapDeviceBootSpace", (char*) gtMemoryMapDeviceBootSpace, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtMemoryMapInternalRegistersSpace", (char*) gtMemoryMapInternalRegistersSpace, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtMemorySetDeviceParam", (char*) gtMemorySetDeviceParam, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtMemorySetInternalSramBaseAddr", (char*) gtMemorySetInternalSramBaseAddr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtMemorySetPciRemapValue", (char*) gtMemorySetPciRemapValue, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtMemorySetProtectRegion", (char*) gtMemorySetProtectRegion, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0ArbiterDisable", (char*) gtPci0ArbiterDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0ArbiterEnable", (char*) gtPci0ArbiterEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0DisableAccessRegion", (char*) gtPci0DisableAccessRegion, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0DisableBrokenAgentDetection", (char*) gtPci0DisableBrokenAgentDetection, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0EnableBrokenAgentDetection", (char*) gtPci0EnableBrokenAgentDetection, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0GetBusMode", (char*) gtPci0GetBusMode, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0GetIOspaceBase", (char*) gtPci0GetIOspaceBase, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0GetIOspaceSize", (char*) gtPci0GetIOspaceSize, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0GetMemory0Base", (char*) gtPci0GetMemory0Base, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0GetMemory0Size", (char*) gtPci0GetMemory0Size, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0GetMemory1Base", (char*) gtPci0GetMemory1Base, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0GetMemory1Size", (char*) gtPci0GetMemory1Size, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0GetMemory2Base", (char*) gtPci0GetMemory2Base, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0GetMemory2Size", (char*) gtPci0GetMemory2Size, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0GetMemory3Base", (char*) gtPci0GetMemory3Base, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0GetMemory3Size", (char*) gtPci0GetMemory3Size, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapBootDeviceMemorySpace", (char*) gtPci0MapBootDeviceMemorySpace, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapBootDeviceMemorySpaceDAC", (char*) gtPci0MapBootDeviceMemorySpaceDAC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapCPUspace", (char*) gtPci0MapCPUspace, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapCPUspaceDAC", (char*) gtPci0MapCPUspaceDAC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapDevice0MemorySpace", (char*) gtPci0MapDevice0MemorySpace, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapDevice0MemorySpaceDAC", (char*) gtPci0MapDevice0MemorySpaceDAC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapDevice1MemorySpace", (char*) gtPci0MapDevice1MemorySpace, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapDevice1MemorySpaceDAC", (char*) gtPci0MapDevice1MemorySpaceDAC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapDevice2MemorySpace", (char*) gtPci0MapDevice2MemorySpace, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapDevice2MemorySpaceDAC", (char*) gtPci0MapDevice2MemorySpaceDAC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapDevice3MemorySpace", (char*) gtPci0MapDevice3MemorySpace, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapDevice3MemorySpaceDAC", (char*) gtPci0MapDevice3MemorySpaceDAC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapIOspace", (char*) gtPci0MapIOspace, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapInternalRegIOSpace", (char*) gtPci0MapInternalRegIOSpace, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapInternalRegSpace", (char*) gtPci0MapInternalRegSpace, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapInternalRegSpaceDAC", (char*) gtPci0MapInternalRegSpaceDAC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapMemory0space", (char*) gtPci0MapMemory0space, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapMemory1space", (char*) gtPci0MapMemory1space, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapMemory2space", (char*) gtPci0MapMemory2space, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapMemory3space", (char*) gtPci0MapMemory3space, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapMemoryBank0", (char*) gtPci0MapMemoryBank0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapMemoryBank0DAC", (char*) gtPci0MapMemoryBank0DAC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapMemoryBank1", (char*) gtPci0MapMemoryBank1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapMemoryBank1DAC", (char*) gtPci0MapMemoryBank1DAC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapMemoryBank2", (char*) gtPci0MapMemoryBank2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapMemoryBank2DAC", (char*) gtPci0MapMemoryBank2DAC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapMemoryBank3", (char*) gtPci0MapMemoryBank3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapMemoryBank3DAC", (char*) gtPci0MapMemoryBank3DAC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapP2pIoSpace", (char*) gtPci0MapP2pIoSpace, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapP2pMem0Space", (char*) gtPci0MapP2pMem0Space, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapP2pMem0SpaceDAC", (char*) gtPci0MapP2pMem0SpaceDAC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapP2pMem1Space", (char*) gtPci0MapP2pMem1Space, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0MapP2pMem1SpaceDAC", (char*) gtPci0MapP2pMem1SpaceDAC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0OverBridgeReadConfigReg", (char*) gtPci0OverBridgeReadConfigReg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0OverBridgeWriteConfigReg", (char*) gtPci0OverBridgeWriteConfigReg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0P2PConfig", (char*) gtPci0P2PConfig, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0ParkingDisable", (char*) gtPci0ParkingDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0ScanSelfBars", (char*) gtPci0ScanSelfBars, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0SetRegionFeatures", (char*) gtPci0SetRegionFeatures, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPci0SetRegionFeaturesDAC", (char*) gtPci0SetRegionFeaturesDAC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPciCpuDevNum", (char*) gtPciCpuDevNum, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPciGetByte", (char*) gtPciGetByte, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPciGetMaxBusNo", (char*) gtPciGetMaxBusNo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPciGetWord", (char*) gtPciGetWord, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPciIntrToVecNum", (char*) gtPciIntrToVecNum, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPciSetByte", (char*) gtPciSetByte, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtPciSetWord", (char*) gtPciSetWord, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtRegReadBlock", (char*) gtRegReadBlock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtRegWriteBlock", (char*) gtRegWriteBlock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtSramInit", (char*) gtSramInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtSramMalloc", (char*) gtSramMalloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtSramMallocAligned", (char*) gtSramMallocAligned, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gtUartBaudRateChange", (char*) gtUartBaudRateChange, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "h", (char*) h, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashClassId", (char*) &hashClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "hashFuncIterScale", (char*) hashFuncIterScale, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashFuncModulo", (char*) hashFuncModulo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashFuncMultiply", (char*) hashFuncMultiply, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashKeyCmp", (char*) hashKeyCmp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashKeyStrCmp", (char*) hashKeyStrCmp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashLibInit", (char*) hashLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashTblCreate", (char*) hashTblCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashTblDelete", (char*) hashTblDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashTblDestroy", (char*) hashTblDestroy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashTblEach", (char*) hashTblEach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashTblFind", (char*) hashTblFind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashTblInit", (char*) hashTblInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashTblPut", (char*) hashTblPut, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashTblRemove", (char*) hashTblRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashTblTerminate", (char*) hashTblTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashinit", (char*) hashinit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "have_backup__9streambuf", (char*) have_backup__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "have_markers__9streambuf", (char*) have_markers__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "help", (char*) help, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hex__FR3ios", (char*) hex__FR3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hostAdd", (char*) hostAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hostDelete", (char*) hostDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hostGetByAddr", (char*) hostGetByAddr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hostGetByName", (char*) hostGetByName, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hostInitFlag", (char*) &hostInitFlag, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "hostList", (char*) &hostList, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "hostListSem", (char*) &hostListSem, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "hostName", (char*) hostName, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hostShow", (char*) hostShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hostTblInit", (char*) hostTblInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hostTblSearchByAddr", (char*) hostTblSearchByAddr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hostTblSearchByName", (char*) hostTblSearchByName, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hourr", (char*) &hourr, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "i", (char*) i, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iam", (char*) iam, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ibegin__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) ibegin__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "icmpCfgParams", (char*) &icmpCfgParams, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "icmpLibInit", (char*) icmpLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "icmpMaskGet", (char*) icmpMaskGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "icmpShowInit", (char*) icmpShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "icmp_ctloutput", (char*) icmp_ctloutput, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "icmp_init", (char*) icmp_init, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "icmp_input", (char*) icmp_input, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "icmpmask", (char*) &icmpmask, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "icmpmaskrepl", (char*) &icmpmaskrepl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "icmpstat", (char*) &icmpstat, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "icmpstatShow", (char*) icmpstatShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "id", (char*) &id, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "iend__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) iend__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifAddrAdd", (char*) ifAddrAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifAddrDelete", (char*) ifAddrDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifAddrGet", (char*) ifAddrGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifAddrSet", (char*) ifAddrSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifAllRoutesDelete", (char*) ifAllRoutesDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifBroadcastGet", (char*) ifBroadcastGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifBroadcastSet", (char*) ifBroadcastSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifDstAddrGet", (char*) ifDstAddrGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifDstAddrSet", (char*) ifDstAddrSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifFlagChange", (char*) ifFlagChange, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifFlagGet", (char*) ifFlagGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifFlagSet", (char*) ifFlagSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifIndexAlloc", (char*) ifIndexAlloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifIndexLibInit", (char*) ifIndexLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifIndexLibShutdown", (char*) ifIndexLibShutdown, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifIndexTest", (char*) ifIndexTest, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifIndexToIfName", (char*) ifIndexToIfName, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifIndexToIfp", (char*) ifIndexToIfp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifMaskGet", (char*) ifMaskGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifMaskSet", (char*) ifMaskSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifMetricGet", (char*) ifMetricGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifMetricSet", (char*) ifMetricSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifNameToIfIndex", (char*) ifNameToIfIndex, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifRouteCleanup", (char*) ifRouteCleanup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifRouteDelete", (char*) ifRouteDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifRouteDisable", (char*) ifRouteDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifRouteEnable", (char*) ifRouteEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifShow", (char*) ifShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "if_attach", (char*) if_attach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "if_dettach", (char*) if_dettach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "if_down", (char*) if_down, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "if_qflush", (char*) if_qflush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "if_slowtimo", (char*) if_slowtimo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "if_slowtimoRestart", (char*) if_slowtimoRestart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "if_up", (char*) if_up, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifa_ifwithaddr", (char*) ifa_ifwithaddr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifa_ifwithaf", (char*) ifa_ifwithaf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifa_ifwithdstaddr", (char*) ifa_ifwithdstaddr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifa_ifwithnet", (char*) ifa_ifwithnet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifa_ifwithroute", (char*) ifa_ifwithroute, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifafree", (char*) ifafree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifaof_ifpforaddr", (char*) ifaof_ifpforaddr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifconf", (char*) ifconf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifinit", (char*) ifinit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifioctl", (char*) ifioctl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifnet", (char*) &ifnet, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "ifpromisc", (char*) ifpromisc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifreset", (char*) ifreset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifreset2", (char*) ifreset2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifresetImmediate", (char*) ifresetImmediate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ifunit", (char*) ifunit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "igmpLibInit", (char*) igmpLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "igmpShowInit", (char*) igmpShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "igmp_fasttimo", (char*) igmp_fasttimo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "igmp_init", (char*) igmp_init, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "igmp_input", (char*) igmp_input, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "igmp_slowtimo", (char*) igmp_slowtimo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "igmpstat", (char*) &igmpstat, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "igmpstatShow", (char*) igmpstatShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ignore__7istreamii", (char*) ignore__7istreamii, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ignore__9streambufi", (char*) ignore__9streambufi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_addmulti", (char*) in_addmulti, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_avail__9streambuf", (char*) in_avail__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_backup__9streambuf", (char*) in_backup__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_broadcast", (char*) in_broadcast, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_canforward", (char*) in_canforward, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_cksum", (char*) in_cksum, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_control", (char*) in_control, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_delmulti", (char*) in_delmulti, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_iaonnetof", (char*) in_iaonnetof, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_ifaddr", (char*) &in_ifaddr, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "in_ifaddr_remove", (char*) in_ifaddr_remove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_ifinit", (char*) in_ifinit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_ifscrub", (char*) in_ifscrub, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_interfaces", (char*) &in_interfaces, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "in_lnaof", (char*) in_lnaof, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_localaddr", (char*) in_localaddr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_losing", (char*) in_losing, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_netof", (char*) in_netof, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_pcballoc", (char*) in_pcballoc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_pcbbind", (char*) in_pcbbind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_pcbconnect", (char*) in_pcbconnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_pcbdetach", (char*) in_pcbdetach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_pcbdisconnect", (char*) in_pcbdisconnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_pcbinshash", (char*) in_pcbinshash, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_pcbladdr", (char*) in_pcbladdr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_pcblookup", (char*) in_pcblookup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_pcblookuphash", (char*) in_pcblookuphash, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_pcbnotify", (char*) in_pcbnotify, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_pcbrehash", (char*) in_pcbrehash, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_rtchange", (char*) in_rtchange, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_setpeeraddr", (char*) in_setpeeraddr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_setsockaddr", (char*) in_setsockaddr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "in_socktrim", (char*) in_socktrim, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "index", (char*) index, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "inet_addr", (char*) inet_addr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "inet_aton", (char*) inet_aton, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "inet_lnaof", (char*) inet_lnaof, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "inet_makeaddr", (char*) inet_makeaddr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "inet_makeaddr_b", (char*) inet_makeaddr_b, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "inet_netmatch", (char*) inet_netmatch, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "inet_netof", (char*) inet_netof, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "inet_netof_string", (char*) inet_netof_string, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "inet_network", (char*) inet_network, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "inet_ntoa", (char*) inet_ntoa, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "inet_ntoa_b", (char*) inet_ntoa_b, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "inetctlerrmap", (char*) &inetctlerrmap, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "inetdomain", (char*) &inetdomain, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "inetstatShow", (char*) inetstatShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "inetsw", (char*) &inetsw, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "init__3iosP9streambufP7ostream", (char*) init__3iosP9streambufP7ostream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "init__7filebuf", (char*) init__7filebuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "initstate", (char*) initstate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "input_ipaddr", (char*) &input_ipaddr, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "insert__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PcUic", (char*) insert__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PcUic, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "insert__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Pcc", (char*) insert__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Pcc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "insert__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiPCc", (char*) insert__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiPCc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "insert__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiPCcUi", (char*) insert__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiPCcUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "insert__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUi", (char*) insert__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "insert__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUic", (char*) insert__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUic, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intCnt", (char*) &intCnt, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "intConnect", (char*) intConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intConnectHard", (char*) &intConnectHard, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "intConnectSoft", (char*) &intConnectSoft, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "intContext", (char*) intContext, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intCount", (char*) intCount, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intDisable", (char*) intDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intDisconnect", (char*) intDisconnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intEnable", (char*) intEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intEnt", (char*) intEnt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intEvent", (char*) &intEvent, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "intEventDbg", (char*) &intEventDbg, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "intEventIndx", (char*) &intEventIndx, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "intExit", (char*) intExit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intLevelSet", (char*) intLevelSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intLock", (char*) intLock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intLockLevelGet", (char*) intLockLevelGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intLockLevelSet", (char*) intLockLevelSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intRegsLock", (char*) intRegsLock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intRegsUnlock", (char*) intRegsUnlock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intRestrict", (char*) intRestrict, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intUnlock", (char*) intUnlock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intVecBaseGet", (char*) intVecBaseGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intVecBaseSet", (char*) intVecBaseSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intVecGet", (char*) intVecGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intVecSet", (char*) intVecSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "internalSramTopAddr", (char*) &internalSramTopAddr, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ioDefDevGet", (char*) ioDefDevGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ioDefDirGet", (char*) ioDefDirGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ioDefPath", (char*) &ioDefPath, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "ioDefPathCat", (char*) ioDefPathCat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ioDefPathGet", (char*) ioDefPathGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ioDefPathSet", (char*) ioDefPathSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ioFullFileNameGet", (char*) ioFullFileNameGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ioGlobalStdGet", (char*) ioGlobalStdGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ioGlobalStdSet", (char*) ioGlobalStdSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ioHelp", (char*) ioHelp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ioMaxLinkLevels", (char*) &ioMaxLinkLevels, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ioTaskStdGet", (char*) ioTaskStdGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ioTaskStdSet", (char*) ioTaskStdSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ioTemp", (char*) &ioTemp, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "io_defs__", (char*) &io_defs__, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "ioctl", (char*) ioctl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosClose", (char*) iosClose, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosCreate", (char*) iosCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosDelete", (char*) iosDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosDevAdd", (char*) iosDevAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosDevDelete", (char*) iosDevDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosDevFind", (char*) iosDevFind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosDevShow", (char*) iosDevShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosDrvInstall", (char*) iosDrvInstall, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosDrvRemove", (char*) iosDrvRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosDrvShow", (char*) iosDrvShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosDvList", (char*) &iosDvList, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "iosFdDevFind", (char*) iosFdDevFind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosFdFree", (char*) iosFdFree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosFdFreeHookRtn", (char*) &iosFdFreeHookRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "iosFdNew", (char*) iosFdNew, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosFdNewHookRtn", (char*) &iosFdNewHookRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "iosFdSet", (char*) iosFdSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosFdShow", (char*) iosFdShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosFdValue", (char*) iosFdValue, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosInit", (char*) iosInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosIoctl", (char*) iosIoctl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosLibInitialized", (char*) &iosLibInitialized, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "iosNextDevGet", (char*) iosNextDevGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosOpen", (char*) iosOpen, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosRead", (char*) iosRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosShowInit", (char*) iosShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosWrite", (char*) iosWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ipAttach", (char*) ipAttach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ipCfgParams", (char*) &ipCfgParams, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ipDetach", (char*) ipDetach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ipDrvCtrl", (char*) &ipDrvCtrl, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "ipEtherResolvRtn", (char*) ipEtherResolvRtn, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ipHeaderCreate", (char*) ipHeaderCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ipHeaderVerify", (char*) ipHeaderVerify, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ipLibInit", (char*) ipLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ipLibMultiInit", (char*) ipLibMultiInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ipMaxUnits", (char*) &ipMaxUnits, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ipMuxCookieGet", (char*) ipMuxCookieGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ipStrongEnded", (char*) &ipStrongEnded, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ipTkError", (char*) ipTkError, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ipToEtherMCastMap", (char*) ipToEtherMCastMap, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ip_ctloutput", (char*) ip_ctloutput, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ip_defttl", (char*) &ip_defttl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ip_dooptions", (char*) ip_dooptions, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ip_drain", (char*) ip_drain, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ip_forward", (char*) ip_forward, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ip_freef", (char*) ip_freef, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ip_freemoptions", (char*) ip_freemoptions, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ip_getmoptions", (char*) ip_getmoptions, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ip_id", (char*) &ip_id, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "ip_init", (char*) ip_init, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ip_mloopback", (char*) ip_mloopback, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ip_nhops", (char*) &ip_nhops, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ip_optcopy", (char*) ip_optcopy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ip_output", (char*) ip_output, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ip_pcbopts", (char*) ip_pcbopts, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ip_protox", (char*) &ip_protox, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "ip_rtaddr", (char*) ip_rtaddr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ip_slowtimo", (char*) ip_slowtimo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ip_stripoptions", (char*) ip_stripoptions, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ipforward_rt", (char*) &ipforward_rt, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "ipfragttl", (char*) &ipfragttl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ipfx0__7istream", (char*) ipfx0__7istream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ipfx1__7istream", (char*) ipfx1__7istream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ipfx__7istreami", (char*) ipfx__7istreami, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ipintr", (char*) ipintr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ipintrq", (char*) &ipintrq, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "ipq", (char*) &ipq, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "ipqmaxlen", (char*) &ipqmaxlen, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ipstat", (char*) &ipstat, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "ipstatShow", (char*) ipstatShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iptime", (char*) iptime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isIntFlagSet", (char*) isIntFlagSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isPMC280BootFlashPresent", (char*) isPMC280BootFlashPresent, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isPMC280DualCPU", (char*) isPMC280DualCPU, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isPMC280Monarch", (char*) isPMC280Monarch, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isPMC280TwoOnBoardEth", (char*) isPMC280TwoOnBoardEth, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isPMC280UsrFlashPresent", (char*) isPMC280UsrFlashPresent, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "is_del__t18string_char_traits1Zcc", (char*) is_del__t18string_char_traits1Zcc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "is_open__3ios", (char*) is_open__3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "is_open__C7filebuf", (char*) is_open__C7filebuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "is_reading__7filebuf", (char*) is_reading__7filebuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isalnum", (char*) isalnum, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isalpha", (char*) isalpha, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isatty", (char*) isatty, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iscntrl", (char*) iscntrl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isdate", (char*) isdate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isday", (char*) isday, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isdigit", (char*) isdigit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isfx__7istream", (char*) isfx__7istream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isgraph", (char*) isgraph, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ishour", (char*) ishour, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "islower", (char*) islower, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ismin", (char*) ismin, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ismon", (char*) ismon, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isprint", (char*) isprint, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ispunct", (char*) ispunct, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "issec", (char*) issec, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isspace", (char*) isspace, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "istreambuf__C7istream", (char*) istreambuf__C7istream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isupper", (char*) isupper, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isxdigit", (char*) isxdigit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isyear", (char*) isyear, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "kernelInit", (char*) kernelInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "kernelIsIdle", (char*) &kernelIsIdle, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "kernelState", (char*) &kernelState, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "kernelTimeSlice", (char*) kernelTimeSlice, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "kernelVersion", (char*) kernelVersion, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "kill", (char*) kill, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ksleep", (char*) ksleep, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "l", (char*) l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "labs", (char*) labs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lat_h", (char*) lat_h, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ld", (char*) ld, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ldCommonMatchAll", (char*) &ldCommonMatchAll, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ldexp", (char*) ldexp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ldiv", (char*) ldiv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ldiv_r", (char*) ldiv_r, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ledClose", (char*) ledClose, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ledControl", (char*) ledControl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ledId", (char*) &ledId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "ledOpen", (char*) ledOpen, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ledRead", (char*) ledRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "length__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) length__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "length__t18string_char_traits1ZcPCc", (char*) length__t18string_char_traits1ZcPCc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lexActions", (char*) lexActions, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lexClass", (char*) &lexClass, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "lexNclasses", (char*) &lexNclasses, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "lexStateTable", (char*) &lexStateTable, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "libiberty_demanglers", (char*) &libiberty_demanglers, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "linebuffered__9streambuf", (char*) linebuffered__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "linebuffered__9streambufi", (char*) linebuffered__9streambufi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "link_rtrequest", (char*) link_rtrequest, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "linkedCtorsInitialized", (char*) &linkedCtorsInitialized, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "listen", (char*) listen, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lkAddr", (char*) lkAddr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lkup", (char*) lkup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ll", (char*) ll, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "llinfo_arp", (char*) &llinfo_arp, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "llr", (char*) llr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "loadCommonManage", (char*) loadCommonManage, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "loadCommonMatch", (char*) loadCommonMatch, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "loadElfInit", (char*) loadElfInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "loadModule", (char*) loadModule, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "loadModuleAt", (char*) loadModuleAt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "loadModuleAtSym", (char*) loadModuleAtSym, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "loadModuleGet", (char*) loadModuleGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "loadRoutine", (char*) &loadRoutine, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "loadSegmentsAllocate", (char*) loadSegmentsAllocate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "loattach", (char*) loattach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "localToGlobalOffset", (char*) &localToGlobalOffset, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "localeFiles", (char*) &localeFiles, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "localeconv", (char*) localeconv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "localtime", (char*) localtime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "localtime_r", (char*) localtime_r, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lockKey", (char*) &lockKey, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "lock__FR7istream", (char*) lock__FR7istream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lock__FR7ostream", (char*) lock__FR7ostream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "log", (char*) log, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "log10", (char*) log10, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "logFdAdd", (char*) logFdAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "logFdDelete", (char*) logFdDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "logFdFromRlogin", (char*) &logFdFromRlogin, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "logFdSet", (char*) logFdSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "logInit", (char*) logInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "logMsg", (char*) logMsg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "logShow", (char*) logShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "logTask", (char*) logTask, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "logTaskId", (char*) &logTaskId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "logTaskOptions", (char*) &logTaskOptions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "logTaskPriority", (char*) &logTaskPriority, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "logTaskStackSize", (char*) &logTaskStackSize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "logb", (char*) logb, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "loginVerifyRtn", (char*) &loginVerifyRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "logout", (char*) logout, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "loif", (char*) &loif, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "longjmp", (char*) longjmp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "looutput", (char*) looutput, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ls", (char*) ls, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lseek", (char*) lseek, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lsr", (char*) lsr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstAdd", (char*) lstAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstConcat", (char*) lstConcat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstCount", (char*) lstCount, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstDelete", (char*) lstDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstExtract", (char*) lstExtract, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstFind", (char*) lstFind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstFirst", (char*) lstFirst, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstFree", (char*) lstFree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstGet", (char*) lstGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstInit", (char*) lstInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstInsert", (char*) lstInsert, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstLast", (char*) lstLast, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstLibInit", (char*) lstLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstNStep", (char*) lstNStep, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstNext", (char*) lstNext, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstNth", (char*) lstNth, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstPrevious", (char*) lstPrevious, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lt__t18string_char_traits1ZcRCcT1", (char*) lt__t18string_char_traits1ZcRCcT1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "m", (char*) m, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "m2If64BitCounters", (char*) &m2If64BitCounters, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mBufClGet", (char*) mBufClGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mCastHashInfo", (char*) &mCastHashInfo, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "mCastHashTblSize", (char*) &mCastHashTblSize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mClBlkConfig", (char*) &mClBlkConfig, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mHdrClGet", (char*) mHdrClGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mPrivRouteEntryAdd", (char*) mPrivRouteEntryAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mPrivRouteEntryDelete", (char*) mPrivRouteEntryDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mRegs", (char*) mRegs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mRouteAdd", (char*) mRouteAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mRouteDelete", (char*) mRouteDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mRouteEntryAdd", (char*) mRouteEntryAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mRouteEntryDelete", (char*) mRouteEntryDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mRouteShow", (char*) mRouteShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "m_adj", (char*) m_adj, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "m_cat", (char*) m_cat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "m_devget", (char*) m_devget, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "m_getclr", (char*) m_getclr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "m_prepend", (char*) m_prepend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "m_pullup", (char*) m_pullup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "malloc", (char*) malloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mask_rnhead", (char*) &mask_rnhead, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "mathFiles", (char*) &mathFiles, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mathHardInit", (char*) mathHardInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "maxDrivers", (char*) &maxDrivers, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "maxFiles", (char*) &maxFiles, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "max_hdr", (char*) &max_hdr, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "max_keylen", (char*) &max_keylen, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "max_linkhdr", (char*) &max_linkhdr, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "max_protohdr", (char*) &max_protohdr, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "max_size__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) max_size__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "maxruns", (char*) &maxruns, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mbinit", (char*) mbinit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mblen", (char*) mblen, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mbstowcs", (char*) mbstowcs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mbtowc", (char*) mbtowc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mbufShow", (char*) mbufShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mcastHashInit", (char*) mcastHashInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mcastHashTblDelete", (char*) mcastHashTblDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mcastHashTblInsert", (char*) mcastHashTblInsert, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mcastHashTblLookup", (char*) mcastHashTblLookup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memAddToPool", (char*) memAddToPool, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memDefaultAlignment", (char*) &memDefaultAlignment, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "memFindMax", (char*) memFindMax, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memInit", (char*) memInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memLibInit", (char*) memLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memOptionsSet", (char*) memOptionsSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartAddToPool", (char*) memPartAddToPool, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartAlignedAlloc", (char*) memPartAlignedAlloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartAlloc", (char*) memPartAlloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartAllocErrorRtn", (char*) &memPartAllocErrorRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "memPartBlockErrorRtn", (char*) &memPartBlockErrorRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "memPartBlockIsValid", (char*) memPartBlockIsValid, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartClassId", (char*) &memPartClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "memPartCreate", (char*) memPartCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartFindMax", (char*) memPartFindMax, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartFree", (char*) memPartFree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartInfoGet", (char*) memPartInfoGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartInit", (char*) memPartInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartInstClassId", (char*) &memPartInstClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "memPartLibInit", (char*) memPartLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartOptionsDefault", (char*) &memPartOptionsDefault, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "memPartOptionsSet", (char*) memPartOptionsSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartRealloc", (char*) memPartRealloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartSemInitRtn", (char*) &memPartSemInitRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "memPartShow", (char*) memPartShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memRead", (char*) memRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memShow", (char*) memShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memShowInit", (char*) memShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memSysPartId", (char*) &memSysPartId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "memTest_Basic", (char*) memTest_Basic, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memTest_Basic_Ever", (char*) memTest_Basic_Ever, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mem_perf", (char*) mem_perf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memalign", (char*) memalign, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memboundary_check", (char*) memboundary_check, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "membytes", (char*) &membytes, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "memchr", (char*) memchr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memcmp", (char*) memcmp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memcpy", (char*) memcpy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memmove", (char*) memmove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memoryRead", (char*) memoryRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memset", (char*) memset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memsplit", (char*) &memsplit, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "memtest_extended", (char*) memtest_extended, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memtest_usage", (char*) memtest_usage, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mgiEndLoad", (char*) mgiEndLoad, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mib2ErrorAdd", (char*) mib2ErrorAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mib2Init", (char*) mib2Init, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "miiAnCheck", (char*) miiAnCheck, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "miiLibInit", (char*) miiLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "miiLibUnInit", (char*) miiLibUnInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "miiPhyInit", (char*) miiPhyInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "miiPhyOptFuncMultiSet", (char*) miiPhyOptFuncMultiSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "miiPhyOptFuncSet", (char*) miiPhyOptFuncSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "miiPhyUnInit", (char*) miiPhyUnInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "minr", (char*) &minr, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "mkdir", (char*) mkdir, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mktime", (char*) mktime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mmuLibFuncs", (char*) &mmuLibFuncs, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "mmuPageBlockSize", (char*) &mmuPageBlockSize, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "mmuPhysAddrShift", (char*) &mmuPhysAddrShift, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mmuPpcADisable", (char*) mmuPpcADisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mmuPpcAEnable", (char*) mmuPpcAEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mmuPpcBatInit", (char*) mmuPpcBatInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mmuPpcBatInit750fx", (char*) mmuPpcBatInit750fx, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mmuPpcBatInitMPC74x5", (char*) mmuPpcBatInitMPC74x5, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mmuPpcBatInitMPC7x5", (char*) mmuPpcBatInitMPC7x5, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mmuPpcDEnabled", (char*) &mmuPpcDEnabled, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mmuPpcExtendedBlockEnableMPC74x5", (char*) mmuPpcExtendedBlockEnableMPC74x5, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mmuPpcExtraBatEnableMPC74x5", (char*) mmuPpcExtraBatEnableMPC74x5, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mmuPpcExtraBatEnableMPC7x5", (char*) mmuPpcExtraBatEnableMPC7x5, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mmuPpcExtraBatInit", (char*) mmuPpcExtraBatInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mmuPpcIEnabled", (char*) &mmuPpcIEnabled, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mmuPpcLibInit", (char*) mmuPpcLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mmuPpcSdr1Get", (char*) mmuPpcSdr1Get, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mmuPpcSdr1Set", (char*) mmuPpcSdr1Set, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mmuPpcSrGet", (char*) mmuPpcSrGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mmuPpcSrSet", (char*) mmuPpcSrSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mmuPpcTlbInvalidateAll", (char*) mmuPpcTlbInvalidateAll, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mmuPpcTlbie", (char*) mmuPpcTlbie, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mmuStateTransArray", (char*) &mmuStateTransArray, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "mmuStateTransArraySize", (char*) &mmuStateTransArraySize, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "modf", (char*) modf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleCheck", (char*) moduleCheck, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleClassId", (char*) &moduleClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "moduleCreate", (char*) moduleCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleCreateHookAdd", (char*) moduleCreateHookAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleCreateHookDelete", (char*) moduleCreateHookDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleDelete", (char*) moduleDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleEach", (char*) moduleEach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleFindByGroup", (char*) moduleFindByGroup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleFindByName", (char*) moduleFindByName, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleFindByNameAndPath", (char*) moduleFindByNameAndPath, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleFlagsGet", (char*) moduleFlagsGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleIdFigure", (char*) moduleIdFigure, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleIdListGet", (char*) moduleIdListGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleInfoGet", (char*) moduleInfoGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleInit", (char*) moduleInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleLibInit", (char*) moduleLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleNameGet", (char*) moduleNameGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleSegAdd", (char*) moduleSegAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleSegEach", (char*) moduleSegEach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleSegFirst", (char*) moduleSegFirst, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleSegGet", (char*) moduleSegGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleSegNext", (char*) moduleSegNext, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleShow", (char*) moduleShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleTerminate", (char*) moduleTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "monarchregfile", (char*) &monarchregfile, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "monr", (char*) &monr, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "move__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepUiPCcUi", (char*) move__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepUiPCcUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "move__t18string_char_traits1ZcPcPCcUi", (char*) move__t18string_char_traits1ZcPcPCcUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mpp18Initialized", (char*) &mpp18Initialized, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mpp24Initialized", (char*) &mpp24Initialized, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mpscChanInit", (char*) mpscChanInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mpscChanSetCdv", (char*) mpscChanSetCdv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mpscChanStart", (char*) mpscChanStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mpscChanStopRx", (char*) mpscChanStopRx, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mpscChanStopTx", (char*) mpscChanStopTx, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mpscChanStopTxRx", (char*) mpscChanStopTxRx, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mpscDbg", (char*) mpscDbg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mpsc_cause_regs_dump", (char*) mpsc_cause_regs_dump, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mpsc_regs_dump", (char*) mpsc_regs_dump, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQClassId", (char*) &msgQClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "msgQCreate", (char*) msgQCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQDelete", (char*) msgQDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQDistInfoGetRtn", (char*) &msgQDistInfoGetRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "msgQDistNumMsgsRtn", (char*) &msgQDistNumMsgsRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "msgQDistReceiveRtn", (char*) &msgQDistReceiveRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "msgQDistSendRtn", (char*) &msgQDistSendRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "msgQDistShowRtn", (char*) &msgQDistShowRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "msgQEvLibInit", (char*) msgQEvLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQEvStart", (char*) msgQEvStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQEvStop", (char*) msgQEvStop, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQInfoGet", (char*) msgQInfoGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQInit", (char*) msgQInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQInstClassId", (char*) &msgQInstClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "msgQLibInit", (char*) msgQLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQNumMsgs", (char*) msgQNumMsgs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQReceive", (char*) msgQReceive, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQSend", (char*) msgQSend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQShow", (char*) msgQShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQShowInit", (char*) msgQShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQSmInfoGetRtn", (char*) &msgQSmInfoGetRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "msgQSmNumMsgsRtn", (char*) &msgQSmNumMsgsRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "msgQSmReceiveRtn", (char*) &msgQSmReceiveRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "msgQSmSendRtn", (char*) &msgQSmSendRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "msgQSmShowRtn", (char*) &msgQSmShowRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "msgQTerminate", (char*) msgQTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mutexOptionsHostLib", (char*) &mutexOptionsHostLib, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mutexOptionsIosLib", (char*) &mutexOptionsIosLib, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mutexOptionsLogLib", (char*) &mutexOptionsLogLib, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mutexOptionsMemLib", (char*) &mutexOptionsMemLib, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mutexOptionsNetDrv", (char*) &mutexOptionsNetDrv, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mutexOptionsNfsDrv", (char*) &mutexOptionsNfsDrv, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mutexOptionsSelectLib", (char*) &mutexOptionsSelectLib, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mutexOptionsSymLib", (char*) &mutexOptionsSymLib, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mutexOptionsTsfsDrv", (char*) &mutexOptionsTsfsDrv, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mutexOptionsTyLib", (char*) &mutexOptionsTyLib, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mutexOptionsUnixLib", (char*) &mutexOptionsUnixLib, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mutexOptionsVmBaseLib", (char*) &mutexOptionsVmBaseLib, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "muxAddrResFuncAdd", (char*) muxAddrResFuncAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxAddrResFuncDel", (char*) muxAddrResFuncDel, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxAddrResFuncGet", (char*) muxAddrResFuncGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxAddressForm", (char*) muxAddressForm, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxBibLock", (char*) &muxBibLock, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "muxBind", (char*) muxBind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxDevExists", (char*) muxDevExists, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxDevLoad", (char*) muxDevLoad, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxDevStart", (char*) muxDevStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxDevStartAll", (char*) muxDevStartAll, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxDevStop", (char*) muxDevStop, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxDevStopAll", (char*) muxDevStopAll, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxDevUnload", (char*) muxDevUnload, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxError", (char*) muxError, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxIoctl", (char*) muxIoctl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxIterateByName", (char*) muxIterateByName, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxLibInit", (char*) muxLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxLinkHeaderCreate", (char*) muxLinkHeaderCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxMCastAddrAdd", (char*) muxMCastAddrAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxMCastAddrDel", (char*) muxMCastAddrDel, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxMCastAddrGet", (char*) muxMCastAddrGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxMaxBinds", (char*) &muxMaxBinds, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "muxPacketAddrGet", (char*) muxPacketAddrGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxPacketDataGet", (char*) muxPacketDataGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxPollDevAdd", (char*) muxPollDevAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxPollDevDel", (char*) muxPollDevDel, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxPollDevStat", (char*) muxPollDevStat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxPollEnd", (char*) muxPollEnd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxPollReceive", (char*) muxPollReceive, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxPollSend", (char*) muxPollSend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxPollStart", (char*) muxPollStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxPollTask", (char*) muxPollTask, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxReceive", (char*) muxReceive, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxSend", (char*) muxSend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxShow", (char*) muxShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxTaskDelayGet", (char*) muxTaskDelayGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxTaskDelaySet", (char*) muxTaskDelaySet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxTaskPriorityGet", (char*) muxTaskPriorityGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxTaskPrioritySet", (char*) muxTaskPrioritySet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxTkBibInit", (char*) muxTkBibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxTkBibShow", (char*) muxTkBibShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxTkBind", (char*) muxTkBind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxTkBindUpdate", (char*) muxTkBindUpdate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxTkCookieGet", (char*) muxTkCookieGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxTkDebug", (char*) &muxTkDebug, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "muxTkDevLoadUpdate", (char*) muxTkDevLoadUpdate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxTkDrvCheck", (char*) muxTkDrvCheck, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxTkPollReceive", (char*) muxTkPollReceive, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxTkPollReceive2", (char*) muxTkPollReceive2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxTkPollSend", (char*) muxTkPollSend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxTkReceive", (char*) muxTkReceive, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxTkSend", (char*) muxTkSend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxTkUnbindUpdate", (char*) muxTkUnbindUpdate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxTkUnloadUpdate", (char*) muxTkUnloadUpdate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxTxRestart", (char*) muxTxRestart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "muxUnbind", (char*) muxUnbind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mv", (char*) mv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mvEEPROMRTCWrite", (char*) mvEEPROMRTCWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mvI2cClearIntFlag", (char*) mvI2cClearIntFlag, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mvI2cDelay", (char*) mvI2cDelay, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mvI2cGenerateStartBit", (char*) mvI2cGenerateStartBit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mvI2cGenerateStopBit", (char*) mvI2cGenerateStopBit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mvI2cIntDisable", (char*) mvI2cIntDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mvI2cIntEnable", (char*) mvI2cIntEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mvI2cMasterEEPROMBurstread", (char*) mvI2cMasterEEPROMBurstread, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mvI2cMasterEEPROMread", (char*) mvI2cMasterEEPROMread, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mvI2cMasterEEPROMreadWord", (char*) mvI2cMasterEEPROMreadWord, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mvI2cMasterEEPROMtransmitAddress", (char*) mvI2cMasterEEPROMtransmitAddress, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mvI2cMasterEEPROMwrite", (char*) mvI2cMasterEEPROMwrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mvI2cMasterEEPROMwriteWord", (char*) mvI2cMasterEEPROMwriteWord, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mvI2cMasterInit", (char*) mvI2cMasterInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mvI2cMasterRead", (char*) mvI2cMasterRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mvI2cMasterReadBlock", (char*) mvI2cMasterReadBlock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mvI2cMasterWrite", (char*) mvI2cMasterWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mvI2cMasterWriteBlock", (char*) mvI2cMasterWriteBlock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mvI2cReadIntFlag", (char*) mvI2cReadIntFlag, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mvI2cReadStatus", (char*) mvI2cReadStatus, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mvI2cReset", (char*) mvI2cReset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mvI2cSetAckBit", (char*) mvI2cSetAckBit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mvI2cSlaveInit", (char*) mvI2cSlaveInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mvI2cTclock", (char*) &mvI2cTclock, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "mvRTCRead", (char*) mvRTCRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mvRTCWrite", (char*) mvRTCWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "myfill", (char*) myfill, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "name__C9type_info", (char*) name__C9type_info, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "namelessPrefix", (char*) &namelessPrefix, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ne__t18string_char_traits1ZcRCcT1", (char*) ne__t18string_char_traits1ZcRCcT1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netBufLibInit", (char*) netBufLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netClBlkFree", (char*) netClBlkFree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netClBlkGet", (char*) netClBlkGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netClBlkJoin", (char*) netClBlkJoin, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netClFree", (char*) netClFree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netClPoolIdGet", (char*) netClPoolIdGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netClusterGet", (char*) netClusterGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netDevCreate", (char*) netDevCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netDevCreate2", (char*) netDevCreate2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netDrv", (char*) netDrv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netDrvDebugLevelSet", (char*) netDrvDebugLevelSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netDrvFileDoesNotExist", (char*) netDrvFileDoesNotExist, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netDrvFileDoesNotExistInstall", (char*) netDrvFileDoesNotExistInstall, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netErrnoSet", (char*) netErrnoSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netHelp", (char*) netHelp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netJobAdd", (char*) netJobAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netLibGenInitialized", (char*) &netLibGenInitialized, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "netLibGeneralInit", (char*) netLibGeneralInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netLibInit", (char*) netLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netLibInitialized", (char*) &netLibInitialized, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "netLsByName", (char*) netLsByName, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netMblkChainDup", (char*) netMblkChainDup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netMblkClChainFree", (char*) netMblkClChainFree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netMblkClFree", (char*) netMblkClFree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netMblkClGet", (char*) netMblkClGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netMblkClJoin", (char*) netMblkClJoin, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netMblkDup", (char*) netMblkDup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netMblkFree", (char*) netMblkFree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netMblkGet", (char*) netMblkGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netMblkOffsetToBufCopy", (char*) netMblkOffsetToBufCopy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netMblkToBufCopy", (char*) netMblkToBufCopy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netPoolDelete", (char*) netPoolDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netPoolInit", (char*) netPoolInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netPoolKheapInit", (char*) netPoolKheapInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netPoolShow", (char*) netPoolShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netShowInit", (char*) netShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netStackDataPoolShow", (char*) netStackDataPoolShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netStackSysPoolShow", (char*) netStackSysPoolShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netTask", (char*) netTask, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netTaskId", (char*) &netTaskId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "netTaskOptions", (char*) &netTaskOptions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "netTaskPriority", (char*) &netTaskPriority, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "netTaskSemId", (char*) &netTaskSemId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "netTaskStackSize", (char*) &netTaskStackSize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "netTupleGet", (char*) netTupleGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netTupleGet2", (char*) netTupleGet2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netTypeAdd", (char*) netTypeAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netTypeDelete", (char*) netTypeDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netTypeInit", (char*) netTypeInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "netWorkInitStatus", (char*) &netWorkInitStatus, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "net_sysctl", (char*) net_sysctl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nextChar__18RBStringIterator_T", (char*) nextChar__18RBStringIterator_T, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nextFreeBlkPtr", (char*) &nextFreeBlkPtr, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "nfsAuthUnixGet", (char*) nfsAuthUnixGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsAuthUnixPrompt", (char*) nfsAuthUnixPrompt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsAuthUnixSet", (char*) nfsAuthUnixSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsAuthUnixShow", (char*) nfsAuthUnixShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsAutoClose", (char*) &nfsAutoClose, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "nfsCacheSize", (char*) &nfsCacheSize, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "nfsClientClose", (char*) nfsClientClose, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsDevInfoGet", (char*) nfsDevInfoGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsDevListGet", (char*) nfsDevListGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsDevShow", (char*) nfsDevShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsDirMount", (char*) nfsDirMount, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsDirReadOne", (char*) nfsDirReadOne, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsDirUnmount", (char*) nfsDirUnmount, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsDrv", (char*) nfsDrv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsDrvNumGet", (char*) nfsDrvNumGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsExportFree", (char*) nfsExportFree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsExportRead", (char*) nfsExportRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsExportShow", (char*) nfsExportShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsFileAttrGet", (char*) nfsFileAttrGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsFileRead", (char*) nfsFileRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsFileRemove", (char*) nfsFileRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsFileWrite", (char*) nfsFileWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsFsAttrGet", (char*) nfsFsAttrGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsHelp", (char*) nfsHelp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsIdSet", (char*) nfsIdSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsLookUpByName", (char*) nfsLookUpByName, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsMaxMsgLen", (char*) &nfsMaxMsgLen, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "nfsMaxPath", (char*) &nfsMaxPath, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "nfsMntDump", (char*) nfsMntDump, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsMntUnmountAll", (char*) nfsMntUnmountAll, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsMount", (char*) nfsMount, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsMountAll", (char*) nfsMountAll, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsReXmitSec", (char*) &nfsReXmitSec, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "nfsReXmitUSec", (char*) &nfsReXmitUSec, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "nfsRename", (char*) nfsRename, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsSockBufSize", (char*) &nfsSockBufSize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "nfsThingCreate", (char*) nfsThingCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "nfsTimeoutSec", (char*) &nfsTimeoutSec, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "nfsTimeoutUSec", (char*) &nfsTimeoutUSec, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "nfsUnmount", (char*) nfsUnmount, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "non_monarchregfile", (char*) &non_monarchregfile, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "nooffeidevices", (char*) &nooffeidevices, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "noofgeidevices", (char*) &noofgeidevices, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "nothrow", (char*) &nothrow, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "nullObject", (char*) &nullObject, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "num_flash_dev", (char*) &num_flash_dev, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "objAlloc", (char*) objAlloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "objAllocExtra", (char*) objAllocExtra, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "objCoreInit", (char*) objCoreInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "objCoreTerminate", (char*) objCoreTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "objFree", (char*) objFree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "objShow", (char*) objShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "oct__FR3ios", (char*) oct__FR3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "open", (char*) open, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "open__7filebufPCcT1", (char*) open__7filebufPCcT1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "open__7filebufPCcii", (char*) open__7filebufPCcii, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "opendir", (char*) opendir, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "operation_complete", (char*) operation_complete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "operation_fail", (char*) operation_fail, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "opfx__7ostream", (char*) opfx__7ostream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "option", (char*) option, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "option1", (char*) option1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "osfx__7ostream", (char*) osfx__7ostream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ostreambuf__C7ostream", (char*) ostreambuf__C7ostream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "out_waiting__9streambuf", (char*) out_waiting__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "overflow__7filebufi", (char*) overflow__7filebufi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "overflow__9streambufi", (char*) overflow__9streambufi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pAltivecRegSetObj", (char*) &pAltivecRegSetObj, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "pAltivecTaskIdPrevious", (char*) &pAltivecTaskIdPrevious, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "pEndPktDev", (char*) &pEndPktDev, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "pFppTaskIdPrevious", (char*) &pFppTaskIdPrevious, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "pInPkt", (char*) &pInPkt, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "pJobPool", (char*) &pJobPool, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "pMibRtn", (char*) &pMibRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "pRootMemStart", (char*) &pRootMemStart, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "pSockFdMap", (char*) &pSockFdMap, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "pTaskLastAltivecTcb", (char*) &pTaskLastAltivecTcb, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "pTaskLastDspTcb", (char*) &pTaskLastDspTcb, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "pTaskLastFpTcb", (char*) &pTaskLastFpTcb, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "pTcpRandHook", (char*) &pTcpRandHook, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "pTcpstates", (char*) &pTcpstates, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "pWdbMemRegions", (char*) &pWdbMemRegions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "pWdbRtIf", (char*) &pWdbRtIf, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "pWvNetEventMap", (char*) &pWvNetEventMap, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "padn__9streambufci", (char*) padn__9streambufci, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "panic", (char*) panic, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "panicSuspend", (char*) &panicSuspend, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "parse16", (char*) parse16, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "parse8", (char*) parse8, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "parse_opts", (char*) parse_opts, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pathBuild", (char*) pathBuild, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pathCat", (char*) pathCat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pathCondense", (char*) pathCondense, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pathLastName", (char*) pathLastName, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pathLastNamePtr", (char*) pathLastNamePtr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pathParse", (char*) pathParse, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pathSplit", (char*) pathSplit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pause", (char*) pause, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pbackfail__9streambufi", (char*) pbackfail__9streambufi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pbase__C9streambuf", (char*) pbase__C9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pbump__9streambufi", (char*) pbump__9streambufi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pc", (char*) pc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pciActive", (char*) &pciActive, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "pciAutoConfig", (char*) pciAutoConfig, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pciAutoConfigNonMonarch", (char*) pciAutoConfigNonMonarch, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pciBusProbe", (char*) pciBusProbe, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pciConfigAddr0", (char*) &pciConfigAddr0, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "pciConfigAddr1", (char*) &pciConfigAddr1, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "pciConfigAddr2", (char*) &pciConfigAddr2, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "pciConfigBdfPack", (char*) pciConfigBdfPack, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pciConfigInByte", (char*) pciConfigInByte, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pciConfigInLong", (char*) pciConfigInLong, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pciConfigInWord", (char*) pciConfigInWord, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pciConfigLibInit", (char*) pciConfigLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pciConfigMech", (char*) &pciConfigMech, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "pciConfigModifyByte", (char*) pciConfigModifyByte, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pciConfigModifyLong", (char*) pciConfigModifyLong, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pciConfigModifyWord", (char*) pciConfigModifyWord, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pciConfigOutByte", (char*) pciConfigOutByte, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pciConfigOutLong", (char*) pciConfigOutLong, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pciConfigOutWord", (char*) pciConfigOutWord, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pciConfigSpcl", (char*) &pciConfigSpcl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "pciDebug", (char*) &pciDebug, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "pciDevConfig", (char*) pciDevConfig, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pciFindClass", (char*) pciFindClass, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pciFindDevice", (char*) pciFindDevice, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pciIsr", (char*) pciIsr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pciLibInitStatus", (char*) &pciLibInitStatus, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "pciMaxBus", (char*) &pciMaxBus, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "pciScan", (char*) pciScan, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pciSpecialCycle", (char*) pciSpecialCycle, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pcicfg0_regs_dump", (char*) pcicfg0_regs_dump, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "peek__7istream", (char*) peek__7istream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "period", (char*) period, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "periodRun", (char*) periodRun, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "perror", (char*) perror, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pfctlinput", (char*) pfctlinput, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pffindproto", (char*) pffindproto, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pffindtype", (char*) pffindtype, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "phys", (char*) &phys, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ping", (char*) ping, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pingLibInit", (char*) pingLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pipeDevCreate", (char*) pipeDevCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pipeDevDelete", (char*) pipeDevDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pipeDrv", (char*) pipeDrv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pipeMsgQOptions", (char*) &pipeMsgQOptions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "pmPartId", (char*) &pmPartId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "pmap_getmaps", (char*) pmap_getmaps, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pmap_getmapsInclude", (char*) pmap_getmapsInclude, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pmap_getport", (char*) pmap_getport, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pmap_set", (char*) pmap_set, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pmap_unset", (char*) pmap_unset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pmaplist", (char*) &pmaplist, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "portmapd", (char*) portmapd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "portmapdId", (char*) &portmapdId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "portmapdOptions", (char*) &portmapdOptions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "portmapdPriority", (char*) &portmapdPriority, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "portmapdStackSize", (char*) &portmapdStackSize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "pow", (char*) pow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ppGlobalEnviron", (char*) &ppGlobalEnviron, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "pptr__C9streambuf", (char*) pptr__C9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "precision__3iosi", (char*) precision__3iosi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "precision__C3ios", (char*) precision__C3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "prepend__10RBString_TPCci", (char*) prepend__10RBString_TPCci, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "prepend__10RBString_TR10RBString_T", (char*) prepend__10RBString_TR10RBString_T, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "prepend__10RBString_Tc", (char*) prepend__10RBString_Tc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "preset_val", (char*) &preset_val, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "print64", (char*) print64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "print64Fine", (char*) print64Fine, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "print64Mult", (char*) print64Mult, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "print64Row", (char*) print64Row, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "printDramErr", (char*) printDramErr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "printErr", (char*) printErr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "printErrno", (char*) printErrno, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "printExc", (char*) printExc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "printLogo", (char*) printLogo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "print_status", (char*) print_status, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "print_summary", (char*) print_summary, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "print_test_name", (char*) print_test_name, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "printf", (char*) printf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "protectBIB", (char*) &protectBIB, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "protectMAC", (char*) &protectMAC, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "protectPCI", (char*) &protectPCI, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "proxyArpHook", (char*) &proxyArpHook, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "proxyBroadcastHook", (char*) &proxyBroadcastHook, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "prtMVReg", (char*) prtMVReg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ptyDevCreate", (char*) ptyDevCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ptyDevRemove", (char*) ptyDevRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ptyDrv", (char*) ptyDrv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pubseekoff__9streambuflQ23ios8seek_diri", (char*) pubseekoff__9streambuflQ23ios8seek_diri, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pubseekpos__9streambufli", (char*) pubseekpos__9streambufli, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "push_back__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0c", (char*) push_back__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0c, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "put__7ostreamSc", (char*) put__7ostreamSc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "put__7ostreamUc", (char*) put__7ostreamUc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "put__7ostreamc", (char*) put__7ostreamc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "put_mode__9streambuf", (char*) put_mode__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "putback__7istreamc", (char*) putback__7istreamc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "putc", (char*) putc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "putchar", (char*) putchar, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "putenv", (char*) putenv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "puts", (char*) puts, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "putw", (char*) putw, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pwd", (char*) pwd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qAdvance", (char*) qAdvance, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qCalibrate", (char*) qCalibrate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qCreate", (char*) qCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qDelete", (char*) qDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qEach", (char*) qEach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qFifoClassId", (char*) &qFifoClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "qFifoCreate", (char*) qFifoCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qFifoDelete", (char*) qFifoDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qFifoEach", (char*) qFifoEach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qFifoGet", (char*) qFifoGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qFifoInfo", (char*) qFifoInfo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qFifoInit", (char*) qFifoInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qFifoPut", (char*) qFifoPut, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qFifoRemove", (char*) qFifoRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qFirst", (char*) qFirst, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qGet", (char*) qGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qGetExpired", (char*) qGetExpired, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qInfo", (char*) qInfo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qInit", (char*) qInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qJobClassId", (char*) &qJobClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "qJobCreate", (char*) qJobCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qJobDelete", (char*) qJobDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qJobEach", (char*) qJobEach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qJobGet", (char*) qJobGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qJobInfo", (char*) qJobInfo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qJobInit", (char*) qJobInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qJobPut", (char*) qJobPut, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qJobTerminate", (char*) qJobTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qKey", (char*) qKey, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriBMapClassId", (char*) &qPriBMapClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "qPriBMapCreate", (char*) qPriBMapCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriBMapDelete", (char*) qPriBMapDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriBMapEach", (char*) qPriBMapEach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriBMapGet", (char*) qPriBMapGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriBMapInfo", (char*) qPriBMapInfo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriBMapInit", (char*) qPriBMapInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriBMapKey", (char*) qPriBMapKey, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriBMapListCreate", (char*) qPriBMapListCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriBMapListDelete", (char*) qPriBMapListDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriBMapPut", (char*) qPriBMapPut, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriBMapRemove", (char*) qPriBMapRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriBMapResort", (char*) qPriBMapResort, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListAdvance", (char*) qPriListAdvance, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListCalibrate", (char*) qPriListCalibrate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListClassId", (char*) &qPriListClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "qPriListCreate", (char*) qPriListCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListDelete", (char*) qPriListDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListEach", (char*) qPriListEach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListFromTailClassId", (char*) &qPriListFromTailClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "qPriListGet", (char*) qPriListGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListGetExpired", (char*) qPriListGetExpired, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListInfo", (char*) qPriListInfo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListInit", (char*) qPriListInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListKey", (char*) qPriListKey, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListPut", (char*) qPriListPut, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListPutFromTail", (char*) qPriListPutFromTail, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListRemove", (char*) qPriListRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListResort", (char*) qPriListResort, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListTerminate", (char*) qPriListTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPut", (char*) qPut, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qRemove", (char*) qRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qResort", (char*) qResort, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qTerminate", (char*) qTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qsort", (char*) qsort, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffClass", (char*) &rBuffClass, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "rBuffClassId", (char*) &rBuffClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "rBuffCreate", (char*) rBuffCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffDestroy", (char*) rBuffDestroy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffFlush", (char*) rBuffFlush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffInfoGet", (char*) rBuffInfoGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffLibInit", (char*) rBuffLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffNBytes", (char*) rBuffNBytes, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffRead", (char*) rBuffRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffReadCommit", (char*) rBuffReadCommit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffReadReserve", (char*) rBuffReadReserve, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffReset", (char*) rBuffReset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffSetFd", (char*) rBuffSetFd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffShow", (char*) rBuffShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffShowInit", (char*) rBuffShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffShowRtn", (char*) &rBuffShowRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "rBuffUpload", (char*) rBuffUpload, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffVerify", (char*) rBuffVerify, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffWrite", (char*) rBuffWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "raise", (char*) raise, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ramDevCreate", (char*) ramDevCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ramDrv", (char*) ramDrv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ramTest16bit", (char*) ramTest16bit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ramTest32bit", (char*) ramTest32bit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ramTest8bit", (char*) ramTest8bit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ramTestPrintDot", (char*) ramTestPrintDot, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rand", (char*) rand, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "random", (char*) random, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rawIpLibInit", (char*) rawIpLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rawLibInit", (char*) rawLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rbegin__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) rbegin__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rbegin__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) rbegin__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rcmd", (char*) rcmd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rdbuf__3iosP9streambuf", (char*) rdbuf__3iosP9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rdbuf__C3ios", (char*) rdbuf__C3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rdreg", (char*) rdreg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rdstate__C3ios", (char*) rdstate__C3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "read", (char*) read, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "read__7istreamPSci", (char*) read__7istreamPSci, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "read__7istreamPUci", (char*) read__7istreamPUci, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "read__7istreamPci", (char*) read__7istreamPci, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "read__7istreamPvi", (char*) read__7istreamPvi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "readable__3ios", (char*) readable__3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "readdir", (char*) readdir, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "readreg", (char*) readreg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "readv", (char*) readv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "readyQBMap", (char*) &readyQBMap, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "readyQHead", (char*) &readyQHead, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "realloc", (char*) realloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "reallocate__t23__malloc_alloc_template1i0PvUiUi", (char*) reallocate__t23__malloc_alloc_template1i0PvUiUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "reallocate__t24__default_alloc_template2b1i0PvUiUi", (char*) reallocate__t24__default_alloc_template2b1i0PvUiUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "reboot", (char*) reboot, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rebootHookAdd", (char*) rebootHookAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "recv", (char*) recv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "recvfrom", (char*) recvfrom, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "recvmsg", (char*) recvmsg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "redirInFd", (char*) &redirInFd, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "redirOutFd", (char*) &redirOutFd, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "reg_service", (char*) reg_service, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "reginittab", (char*) &reginittab, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "registerrpc", (char*) registerrpc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rejected", (char*) rejected, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "reld", (char*) reld, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "release__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3Rep", (char*) release__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3Rep, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "release_mem", (char*) release_mem, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "remCacheControlBits", (char*) remCacheControlBits, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "remCurIdGet", (char*) remCurIdGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "remCurIdSet", (char*) remCurIdSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "remLastResvPort", (char*) &remLastResvPort, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "remLibInit", (char*) remLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "remStdErrSetupTimeout", (char*) &remStdErrSetupTimeout, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "remove", (char*) remove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rename", (char*) rename, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rend__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) rend__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rend__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) rend__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rep__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) rep__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "repeat", (char*) repeat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "repeatRun", (char*) repeatRun, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PcT1PCc", (char*) replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PcT1PCc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PcT1PCcUi", (char*) replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PcT1PCcUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PcT1RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PcT1RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PcT1Uic", (char*) replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PcT1Uic, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUiPCc", (char*) replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUiPCc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUiPCcUi", (char*) replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUiPCcUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUiRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUi", (char*) replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUiRCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUiUic", (char*) replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUiUic, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUic", (char*) replace__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUic, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "repup__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PQ2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3Rep", (char*) repup__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PQ2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3Rep, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "reschedule", (char*) reschedule, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "reserve__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui", (char*) reserve__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "resize__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui", (char*) resize__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "resize__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Uic", (char*) resize__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Uic, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "restartTaskName", (char*) &restartTaskName, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "restartTaskOptions", (char*) &restartTaskOptions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "restartTaskPriority", (char*) &restartTaskPriority, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "restartTaskStackSize", (char*) &restartTaskStackSize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "rewind", (char*) rewind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rewinddir", (char*) rewinddir, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rfind__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi", (char*) rfind__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rfind__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUiUi", (char*) rfind__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0PCcUiUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rfind__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui", (char*) rfind__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0RCt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Ui, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rfind__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0cUi", (char*) rfind__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0cUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rindex", (char*) rindex, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rip_ctloutput", (char*) rip_ctloutput, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rip_init", (char*) rip_init, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rip_input", (char*) rip_input, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rip_output", (char*) rip_output, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rip_recvspace", (char*) &rip_recvspace, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "rip_sendspace", (char*) &rip_sendspace, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "rip_usrreq", (char*) rip_usrreq, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rlogChildTask", (char*) rlogChildTask, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rlogChildTaskId", (char*) &rlogChildTaskId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "rlogInTask", (char*) rlogInTask, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rlogInTaskId", (char*) &rlogInTaskId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "rlogInit", (char*) rlogInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rlogOutTask", (char*) rlogOutTask, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rlogOutTaskId", (char*) &rlogOutTaskId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "rlogShellName", (char*) &rlogShellName, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "rlogTaskOptions", (char*) &rlogTaskOptions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "rlogTaskPriority", (char*) &rlogTaskPriority, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "rlogTaskStackSize", (char*) &rlogTaskStackSize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "rlogTermType", (char*) &rlogTermType, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "rlogin", (char*) rlogin, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rlogind", (char*) rlogind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rlogindId", (char*) &rlogindId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "rlogindSocket", (char*) &rlogindSocket, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "rm", (char*) rm, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rmdir", (char*) rmdir, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rn_addmask", (char*) rn_addmask, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rn_addroute", (char*) rn_addroute, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rn_delete", (char*) rn_delete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rn_destroyhead", (char*) rn_destroyhead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rn_init", (char*) rn_init, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rn_inithead", (char*) rn_inithead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rn_insert", (char*) rn_insert, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rn_lookup", (char*) rn_lookup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rn_match", (char*) rn_match, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rn_mkfreelist", (char*) &rn_mkfreelist, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "rn_newpair", (char*) rn_newpair, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rn_refines", (char*) rn_refines, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rn_search", (char*) rn_search, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rn_search_m", (char*) rn_search_m, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rn_walksubtree", (char*) rn_walksubtree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rn_walktree", (char*) rn_walktree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rngBufGet", (char*) rngBufGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rngBufPut", (char*) rngBufPut, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rngCreate", (char*) rngCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rngDelete", (char*) rngDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rngFlush", (char*) rngFlush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rngFreeBytes", (char*) rngFreeBytes, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rngIsEmpty", (char*) rngIsEmpty, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rngIsFull", (char*) rngIsFull, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rngMoveAhead", (char*) rngMoveAhead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rngNBytes", (char*) rngNBytes, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rngPutAhead", (char*) rngPutAhead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rootMemNBytes", (char*) &rootMemNBytes, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "rootTaskId", (char*) &rootTaskId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "roundRobinOn", (char*) &roundRobinOn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "roundRobinSlice", (char*) &roundRobinSlice, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "routeAdd", (char*) routeAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "routeAgeSet", (char*) routeAgeSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "routeCmd", (char*) routeCmd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "routeDelete", (char*) routeDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "routeDrain", (char*) routeDrain, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "routeEntryFill", (char*) routeEntryFill, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "routeMetricSet", (char*) routeMetricSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "routeNetAdd", (char*) routeNetAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "routeShow", (char*) routeShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "routeSwap", (char*) routeSwap, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "route_init", (char*) route_init, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "routestatShow", (char*) routestatShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rpcClntErrnoSet", (char*) rpcClntErrnoSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rpcErrnoGet", (char*) rpcErrnoGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rpcInit", (char*) rpcInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rpcTaskInit", (char*) rpcTaskInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rresvport", (char*) rresvport, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rtIfaceMsgHook", (char*) &rtIfaceMsgHook, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "rtMem", (char*) &rtMem, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "rtMissMsgHook", (char*) &rtMissMsgHook, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "rtNewAddrMsgHook", (char*) &rtNewAddrMsgHook, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "rt_fixdelete", (char*) rt_fixdelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rt_maskedcopy", (char*) rt_maskedcopy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rt_setgate", (char*) rt_setgate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rt_tables", (char*) &rt_tables, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "rtable_init", (char*) rtable_init, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rtalloc", (char*) rtalloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rtalloc1", (char*) rtalloc1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rtccall", (char*) rtccall, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rtfree", (char*) rtfree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rtinit", (char*) rtinit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rtioctl", (char*) rtioctl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rtmodified", (char*) &rtmodified, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "rtredirect", (char*) rtredirect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rtrequest", (char*) rtrequest, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rtrequestAddEqui", (char*) rtrequestAddEqui, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rtrequestDelEqui", (char*) rtrequestDelEqui, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rtstat", (char*) &rtstat, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "rttrash", (char*) &rttrash, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "run", (char*) &run, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "runerrors", (char*) &runerrors, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "runtimeName", (char*) &runtimeName, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "runtimeVersion", (char*) &runtimeVersion, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "rxfei0counter", (char*) &rxfei0counter, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "rxfei1counter", (char*) &rxfei1counter, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "s", (char*) s, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "s1", (char*) &s1, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "s2", (char*) &s2, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "s29ALBotOffset", (char*) &s29ALBotOffset, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "s29ALTopOffset", (char*) &s29ALTopOffset, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "saving__12streammarker", (char*) saving__12streammarker, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sb_lock", (char*) sb_lock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sb_max", (char*) &sb_max, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sbappend", (char*) sbappend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sbappendaddr", (char*) sbappendaddr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sbappendcontrol", (char*) sbappendcontrol, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sbappendrecord", (char*) sbappendrecord, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sbcompress", (char*) sbcompress, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sbdrop", (char*) sbdrop, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sbdroprecord", (char*) sbdroprecord, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sbflush", (char*) sbflush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sbinsertoob", (char*) sbinsertoob, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sbrelease", (char*) sbrelease, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sbreserve", (char*) sbreserve, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sbseldequeue", (char*) sbseldequeue, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sbselqueue", (char*) sbselqueue, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sbumpc__9streambuf", (char*) sbumpc__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sbwait", (char*) sbwait, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sbwakeup", (char*) sbwakeup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "scalb", (char*) scalb, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "scanCharSet", (char*) scanCharSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "scanField", (char*) scanField, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "scan__7istreamPCce", (char*) scan__7istreamPCce, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "scan__9streambufPCce", (char*) scan__9streambufPCce, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "scanf", (char*) scanf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sdmaChanInit", (char*) sdmaChanInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sdmaChanReceive", (char*) sdmaChanReceive, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sdmaChanSend", (char*) sdmaChanSend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sdmaChanStart", (char*) sdmaChanStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sdmaChanStopRx", (char*) sdmaChanStopRx, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sdmaChanStopTx", (char*) sdmaChanStopTx, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sdmaChanStopTxRx", (char*) sdmaChanStopTxRx, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sdmaDbg", (char*) sdmaDbg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sdmaRxReturnBuff", (char*) sdmaRxReturnBuff, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sdmaTxReturnDesc", (char*) sdmaTxReturnDesc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sdramSize", (char*) &sdramSize, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "secr", (char*) &secr, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "seekg__7istreaml", (char*) seekg__7istreaml, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "seekg__7istreamlQ23ios8seek_dir", (char*) seekg__7istreamlQ23ios8seek_dir, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "seekmark__9streambufR12streammarkeri", (char*) seekmark__9streambufR12streammarkeri, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "seekoff__7filebuflQ23ios8seek_diri", (char*) seekoff__7filebuflQ23ios8seek_diri, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "seekoff__9streambuflQ23ios8seek_diri", (char*) seekoff__9streambuflQ23ios8seek_diri, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "seekp__7ostreaml", (char*) seekp__7ostreaml, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "seekp__7ostreamlQ23ios8seek_dir", (char*) seekp__7ostreamlQ23ios8seek_dir, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "seekpos__9streambufli", (char*) seekpos__9streambufli, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "selNodeAdd", (char*) selNodeAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "selNodeDelete", (char*) selNodeDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "selTaskDeleteHookAdd", (char*) selTaskDeleteHookAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "selWakeup", (char*) selWakeup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "selWakeupAll", (char*) selWakeupAll, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "selWakeupListInit", (char*) selWakeupListInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "selWakeupListLen", (char*) selWakeupListLen, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "selWakeupListTerm", (char*) selWakeupListTerm, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "selWakeupType", (char*) selWakeupType, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "select", (char*) select, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "selectInit", (char*) selectInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "selfish__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) selfish__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semBCoreInit", (char*) semBCoreInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semBCreate", (char*) semBCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semBGive", (char*) semBGive, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semBGiveDefer", (char*) semBGiveDefer, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semBInit", (char*) semBInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semBLibInit", (char*) semBLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semBTake", (char*) semBTake, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semCCoreInit", (char*) semCCoreInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semCCreate", (char*) semCCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semCGive", (char*) semCGive, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semCGiveDefer", (char*) semCGiveDefer, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semCInit", (char*) semCInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semCLibInit", (char*) semCLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semCTake", (char*) semCTake, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semClass", (char*) &semClass, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "semClassId", (char*) &semClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "semDelete", (char*) semDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semDestroy", (char*) semDestroy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semEvIsFreeTbl", (char*) &semEvIsFreeTbl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "semEvLibInit", (char*) semEvLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semEvStart", (char*) semEvStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semEvStop", (char*) semEvStop, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semFlush", (char*) semFlush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semFlushDefer", (char*) semFlushDefer, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semFlushDeferTbl", (char*) &semFlushDeferTbl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "semFlushTbl", (char*) &semFlushTbl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "semGive", (char*) semGive, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semGiveDefer", (char*) semGiveDefer, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semGiveDeferTbl", (char*) &semGiveDeferTbl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "semGiveTbl", (char*) &semGiveTbl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "semInfo", (char*) semInfo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semInstClass", (char*) &semInstClass, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "semInstClassId", (char*) &semInstClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "semIntRestrict", (char*) semIntRestrict, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semInvalid", (char*) semInvalid, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semLibInit", (char*) semLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semMCoreInit", (char*) semMCoreInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semMCreate", (char*) semMCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semMGive", (char*) semMGive, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semMGiveForce", (char*) semMGiveForce, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semMGiveKern", (char*) semMGiveKern, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semMGiveKernWork", (char*) &semMGiveKernWork, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "semMInit", (char*) semMInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semMLibInit", (char*) semMLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semMPendQPut", (char*) semMPendQPut, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semMTake", (char*) semMTake, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semQFlush", (char*) semQFlush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semQFlushDefer", (char*) semQFlushDefer, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semQInit", (char*) semQInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semShow", (char*) semShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semShowInit", (char*) semShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semSmInfoRtn", (char*) &semSmInfoRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "semSmShowRtn", (char*) &semSmShowRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "semTake", (char*) semTake, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semTakeTbl", (char*) &semTakeTbl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "semTerminate", (char*) semTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "send", (char*) send, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sendmsg", (char*) sendmsg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sendto", (char*) sendto, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "serialChanInit", (char*) serialChanInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "serialChanSetBaudRate", (char*) serialChanSetBaudRate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "serialChanStart", (char*) serialChanStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "serialChanStop", (char*) serialChanStop, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "setI2cBusy", (char*) setI2cBusy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "setI2cReady", (char*) setI2cReady, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "set__3iosi", (char*) set__3iosi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "set__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepUicUi", (char*) set__Q2t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0_3RepUicUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "set__t18string_char_traits1ZcPcRCcUi", (char*) set__t18string_char_traits1ZcPcRCcUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "set_column__9streambufi", (char*) set_column__9streambufi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "set_cplus_marker_for_demangling", (char*) set_cplus_marker_for_demangling, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "set_if_addr", (char*) set_if_addr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "set_new_handler__FPFv_v", (char*) set_new_handler__FPFv_v, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "set_offset__12streammarkeri", (char*) set_offset__12streammarkeri, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "set_terminate__FPFv_v", (char*) set_terminate__FPFv_v, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "set_unexpected__FPFv_v", (char*) set_unexpected__FPFv_v, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "setb__9streambufPcT1i", (char*) setb__9streambufPcT1i, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "setbuf", (char*) setbuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "setbuf__7filebufPci", (char*) setbuf__7filebufPci, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "setbuf__9streambufPci", (char*) setbuf__9streambufPci, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "setbuffer", (char*) setbuffer, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "setf__3iosUl", (char*) setf__3iosUl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "setf__3iosUlUl", (char*) setf__3iosUlUl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "setg__9streambufPcN21", (char*) setg__9streambufPcN21, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sethostname", (char*) sethostname, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "setjmp", (char*) setjmp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "setlinebuf", (char*) setlinebuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "setlocale", (char*) setlocale, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "setp__9streambufPcT1", (char*) setp__9streambufPcT1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "setsockopt", (char*) setsockopt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "setstate", (char*) setstate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "setstate__3iosi", (char*) setstate__3iosi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "settime", (char*) settime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "setvbuf", (char*) setvbuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sfpDoubleNormalize", (char*) sfpDoubleNormalize, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sfpDoubleNormalize2", (char*) sfpDoubleNormalize2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sfpFloatNormalize", (char*) sfpFloatNormalize, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sgetc__9streambuf", (char*) sgetc__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sgetn__9streambufPci", (char*) sgetn__9streambufPci, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shell", (char*) shell, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellHistSize", (char*) &shellHistSize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "shellHistory", (char*) shellHistory, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellInit", (char*) shellInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellIsRemoteConnectedGet", (char*) shellIsRemoteConnectedGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellIsRemoteConnectedSet", (char*) shellIsRemoteConnectedSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellLock", (char*) shellLock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellLogin", (char*) shellLogin, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellLoginInstall", (char*) shellLoginInstall, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellLogout", (char*) shellLogout, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellLogoutInstall", (char*) shellLogoutInstall, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellOrigStdSet", (char*) shellOrigStdSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellPromptSet", (char*) shellPromptSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellRestart", (char*) shellRestart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellScriptAbort", (char*) shellScriptAbort, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellTaskId", (char*) &shellTaskId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "shellTaskName", (char*) &shellTaskName, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "shellTaskOptions", (char*) &shellTaskOptions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "shellTaskPriority", (char*) &shellTaskPriority, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "shellTaskStackSize", (char*) &shellTaskStackSize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "shortbuf__9streambuf", (char*) shortbuf__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "show", (char*) show, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "showMap", (char*) showMap, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shutdown", (char*) shutdown, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigEvtRtn", (char*) &sigEvtRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "sigInit", (char*) sigInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigPendDestroy", (char*) sigPendDestroy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigPendInit", (char*) sigPendInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigPendKill", (char*) sigPendKill, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigaction", (char*) sigaction, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigaddset", (char*) sigaddset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigblock", (char*) sigblock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigdelset", (char*) sigdelset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigemptyset", (char*) sigemptyset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigfillset", (char*) sigfillset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigismember", (char*) sigismember, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "signal", (char*) signal, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigpending", (char*) sigpending, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigprocmask", (char*) sigprocmask, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigqueue", (char*) sigqueue, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigqueueInit", (char*) sigqueueInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigreturn", (char*) sigreturn, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigsetjmp", (char*) sigsetjmp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigsetmask", (char*) sigsetmask, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigsuspend", (char*) sigsuspend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigtimedwait", (char*) sigtimedwait, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigvec", (char*) sigvec, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigwait", (char*) sigwait, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigwaitinfo", (char*) sigwaitinfo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "silent_test", (char*) &silent_test, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "sin", (char*) sin, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sinh", (char*) sinh, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "size__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) size__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "skip__7istreami", (char*) skip__7istreami, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "skip_ws__FP9streambuf", (char*) skip_ws__FP9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sllCount", (char*) sllCount, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sllCreate", (char*) sllCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sllDelete", (char*) sllDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sllEach", (char*) sllEach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sllGet", (char*) sllGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sllInit", (char*) sllInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sllPrevious", (char*) sllPrevious, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sllPutAtHead", (char*) sllPutAtHead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sllPutAtTail", (char*) sllPutAtTail, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sllRemove", (char*) sllRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sllTerminate", (char*) sllTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "smMemPartAddToPoolRtn", (char*) &smMemPartAddToPoolRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "smMemPartAllocRtn", (char*) &smMemPartAllocRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "smMemPartFindMaxRtn", (char*) &smMemPartFindMaxRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "smMemPartFreeRtn", (char*) &smMemPartFreeRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "smMemPartOptionsSetRtn", (char*) &smMemPartOptionsSetRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "smMemPartReallocRtn", (char*) &smMemPartReallocRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "smMemPartShowRtn", (char*) &smMemPartShowRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "smObjPoolMinusOne", (char*) &smObjPoolMinusOne, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "smObjTaskDeleteFailRtn", (char*) &smObjTaskDeleteFailRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "smObjTcbFreeFailRtn", (char*) &smObjTcbFreeFailRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "smObjTcbFreeRtn", (char*) &smObjTcbFreeRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "snextc__9streambuf", (char*) snextc__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "snoopEnable", (char*) &snoopEnable, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sntpcInit", (char*) sntpcInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sntpcPort", (char*) &sntpcPort, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "sntpcTimeGet", (char*) sntpcTimeGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "so", (char*) so, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "soabort", (char*) soabort, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "soaccept", (char*) soaccept, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sobind", (char*) sobind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "socantrcvmore", (char*) socantrcvmore, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "socantsendmore", (char*) socantsendmore, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sockFdtosockFunc", (char*) sockFdtosockFunc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sockLibAdd", (char*) sockLibAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sockLibInit", (char*) sockLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "socket", (char*) socket, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "soclose", (char*) soclose, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "soconnect", (char*) soconnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "soconnect2", (char*) soconnect2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "socreate", (char*) socreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sodisconnect", (char*) sodisconnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sofree", (char*) sofree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sogetopt", (char*) sogetopt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sohasoutofband", (char*) sohasoutofband, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "soisconnected", (char*) soisconnected, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "soisconnecting", (char*) soisconnecting, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "soisdisconnected", (char*) soisdisconnected, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "soisdisconnecting", (char*) soisdisconnecting, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "solisten", (char*) solisten, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "somaxconn", (char*) &somaxconn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "sonewconn1", (char*) sonewconn1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "soo_ioctl", (char*) soo_ioctl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "soo_select", (char*) soo_select, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "soo_unselect", (char*) soo_unselect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "soqinsque", (char*) soqinsque, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "soqremque", (char*) soqremque, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "soreceive", (char*) soreceive, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "soreserve", (char*) soreserve, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sorflush", (char*) sorflush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sosend", (char*) sosend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sosetopt", (char*) sosetopt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "soshutdown", (char*) soshutdown, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sowakeup", (char*) sowakeup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sowakeupHook", (char*) &sowakeupHook, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sp", (char*) sp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "spTaskOptions", (char*) &spTaskOptions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "spTaskPriority", (char*) &spTaskPriority, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "spTaskStackSize", (char*) &spTaskStackSize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "splSemId", (char*) &splSemId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "splSemInit", (char*) splSemInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "splTid", (char*) &splTid, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "splimp", (char*) splimp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "splnet", (char*) splnet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "splnet2", (char*) splnet2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "splx", (char*) splx, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sprintf", (char*) sprintf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sputbackc__9streambufc", (char*) sputbackc__9streambufc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sputc__9streambufi", (char*) sputc__9streambufi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sputn__9streambufPCci", (char*) sputn__9streambufPCci, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "spy", (char*) spy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "spyClkStart", (char*) spyClkStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "spyClkStop", (char*) spyClkStop, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "spyHelp", (char*) spyHelp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "spyReport", (char*) spyReport, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "spyStop", (char*) spyStop, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "spyTask", (char*) spyTask, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sqrt", (char*) sqrt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "srand", (char*) srand, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "srandom", (char*) srandom, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sscanf", (char*) sscanf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sseekoff__9streambuflQ23ios8seek_diri", (char*) sseekoff__9streambuflQ23ios8seek_diri, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sseekpos__9streambufli", (char*) sseekpos__9streambufli, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "standAloneSymTbl", (char*) &standAloneSymTbl, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "start_addr", (char*) &start_addr, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "stat", (char*) stat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "statSymTbl", (char*) &statSymTbl, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "statTbl", (char*) &statTbl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "statTblSize", (char*) &statTblSize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "statfs", (char*) statfs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "stdioFiles", (char*) &stdioFiles, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "stdioFp", (char*) stdioFp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "stdioFpCreate", (char*) stdioFpCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "stdioFpDestroy", (char*) stdioFpDestroy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "stdioInit", (char*) stdioInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "stdioShow", (char*) stdioShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "stdioShowInit", (char*) stdioShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "stdlibFiles", (char*) &stdlibFiles, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "stossc__9streambuf", (char*) stossc__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strcat", (char*) strcat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strchr", (char*) strchr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strcmp", (char*) strcmp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strcoll", (char*) strcoll, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strcpy", (char*) strcpy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strcspn", (char*) strcspn, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strerror", (char*) strerror, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strerror_r", (char*) strerror_r, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strftime", (char*) strftime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "stringFiles", (char*) &stringFiles, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "strlen", (char*) strlen, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strncat", (char*) strncat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strncmp", (char*) strncmp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strncpy", (char*) strncpy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strpbrk", (char*) strpbrk, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strrchr", (char*) strrchr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strspn", (char*) strspn, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strstr", (char*) strstr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strtod", (char*) strtod, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strtok", (char*) strtok, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strtok_r", (char*) strtok_r, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strtol", (char*) strtol, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strtoul", (char*) strtoul, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strxfrm", (char*) strxfrm, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "subnetsarelocal", (char*) &subnetsarelocal, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "substr__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUi", (char*) substr__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0UiUi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "substrcmp", (char*) substrcmp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sungetc__9streambuf", (char*) sungetc__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "svc_find", (char*) svc_find, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "svc_getreq", (char*) svc_getreq, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "svc_getreqset", (char*) svc_getreqset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "svc_rawInclude", (char*) svc_rawInclude, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "svc_register", (char*) svc_register, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "svc_run", (char*) svc_run, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "svc_sendreply", (char*) svc_sendreply, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "svc_simpleInclude", (char*) svc_simpleInclude, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "svc_unregister", (char*) svc_unregister, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "svcerr_auth", (char*) svcerr_auth, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "svcerr_decode", (char*) svcerr_decode, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "svcerr_noproc", (char*) svcerr_noproc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "svcerr_noprog", (char*) svcerr_noprog, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "svcerr_progvers", (char*) svcerr_progvers, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "svcerr_systemerr", (char*) svcerr_systemerr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "svcerr_weakauth", (char*) svcerr_weakauth, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "svcfd_create", (char*) svcfd_create, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "svcraw_create", (char*) svcraw_create, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "svctcp_create", (char*) svctcp_create, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "svcudp_bufcreate", (char*) svcudp_bufcreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "svcudp_create", (char*) svcudp_create, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "svcudp_enablecache", (char*) svcudp_enablecache, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "swab", (char*) swab, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "swap__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Rt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) swap__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0Rt12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "switch_to_get_mode__9streambuf", (char*) switch_to_get_mode__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symAdd", (char*) symAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symAlloc", (char*) symAlloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symByCNameFind", (char*) symByCNameFind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symByValueAndTypeFind", (char*) symByValueAndTypeFind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symByValueFind", (char*) symByValueFind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symEach", (char*) symEach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symFindByCName", (char*) symFindByCName, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symFindByName", (char*) symFindByName, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symFindByNameAndType", (char*) symFindByNameAndType, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symFindByValue", (char*) symFindByValue, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symFindByValueAndType", (char*) symFindByValueAndType, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symFindSymbol", (char*) symFindSymbol, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symFree", (char*) symFree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symGroupDefault", (char*) &symGroupDefault, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "symInit", (char*) symInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symLibInit", (char*) symLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symLkupPgSz", (char*) &symLkupPgSz, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "symName", (char*) symName, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symNameGet", (char*) symNameGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symRemove", (char*) symRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symSAdd", (char*) symSAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symShow", (char*) symShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symShowInit", (char*) symShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symTblAdd", (char*) symTblAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symTblClassId", (char*) &symTblClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "symTblCreate", (char*) symTblCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symTblDelete", (char*) symTblDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symTblDestroy", (char*) symTblDestroy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symTblInit", (char*) symTblInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symTblRemove", (char*) symTblRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symTblTerminate", (char*) symTblTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symTypeGet", (char*) symTypeGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symValueGet", (char*) symValueGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "syncLoadRtn", (char*) &syncLoadRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "syncSymAddRtn", (char*) &syncSymAddRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "syncSymRemoveRtn", (char*) &syncSymRemoveRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "syncUnldRtn", (char*) &syncUnldRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sync__7filebuf", (char*) sync__7filebuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sync__7istream", (char*) sync__7istream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sync__9streambuf", (char*) sync__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sync_with_stdio__3ios", (char*) sync_with_stdio__3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sync_with_stdio__3iosi", (char*) sync_with_stdio__3iosi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sys543PciInit", (char*) sys543PciInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sys543Show", (char*) sys543Show, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sys557IntAck", (char*) sys557IntAck, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sys557IntDisable", (char*) sys557IntDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sys557IntEnable", (char*) sys557IntEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sys557PciInit", (char*) sys557PciInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sys557Show", (char*) sys557Show, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sys557eepromRead", (char*) sys557eepromRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sys557mdioRead", (char*) sys557mdioRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sys557mdioWrite", (char*) sys557mdioWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sys82543BoardInit", (char*) sys82543BoardInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysAdaEnable", (char*) &sysAdaEnable, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sysAltivecProbe", (char*) sysAltivecProbe, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysBatDesc", (char*) &sysBatDesc, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sysBootFile", (char*) &sysBootFile, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "sysBootHost", (char*) &sysBootHost, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "sysBootLine", (char*) &sysBootLine, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sysBootParams", (char*) &sysBootParams, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "sysBspRev", (char*) sysBspRev, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysBus", (char*) &sysBus, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sysBusToLocalAdrs", (char*) sysBusToLocalAdrs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysClDescTbl", (char*) &sysClDescTbl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sysClDescTblNumEnt", (char*) &sysClDescTblNumEnt, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sysClkConnect", (char*) sysClkConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysClkDisable", (char*) sysClkDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysClkEnable", (char*) sysClkEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysClkRateGet", (char*) sysClkRateGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysClkRateSet", (char*) sysClkRateSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysCplusEnable", (char*) &sysCplusEnable, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sysCpu", (char*) &sysCpu, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sysCpuCheck", (char*) sysCpuCheck, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysDelay", (char*) sysDelay, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysExcMsg", (char*) &sysExcMsg, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sysFlags", (char*) &sysFlags, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "sysGetBusSpd", (char*) sysGetBusSpd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysGetCpuRev", (char*) sysGetCpuRev, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysGetCpuVer", (char*) sysGetCpuVer, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysGetL2CR", (char*) sysGetL2CR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysGetL3CR", (char*) sysGetL3CR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysGetPrid", (char*) sysGetPrid, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysGetSDR1", (char*) sysGetSDR1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysHardReset", (char*) sysHardReset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysHwInit", (char*) sysHwInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysHwInit2", (char*) sysHwInit2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysInByte", (char*) sysInByte, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysInLong", (char*) sysInLong, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysInWord", (char*) sysInWord, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysIntCtrlInit", (char*) sysIntCtrlInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysIntEnable", (char*) sysIntEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysL1DcacheDisable", (char*) sysL1DcacheDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysL1DcacheEnable", (char*) sysL1DcacheEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysL2BackDisable", (char*) sysL2BackDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysL2BackEnable", (char*) sysL2BackEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysL2BackGlobalInv", (char*) sysL2BackGlobalInv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysL2BackHWFlush", (char*) sysL2BackHWFlush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysL2BackInit", (char*) sysL2BackInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysL2BackSWFlush", (char*) sysL2BackSWFlush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysL2BackSetWT", (char*) sysL2BackSetWT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysL2crGet", (char*) sysL2crGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysL2crPut", (char*) sysL2crPut, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysMachChkExcpHand", (char*) sysMachChkExcpHand, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysMachChkExcpInit", (char*) sysMachChkExcpInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysMclBlkConfig", (char*) &sysMclBlkConfig, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sysMemProbeSup", (char*) sysMemProbeSup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysMemTop", (char*) sysMemTop, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysMmuMapAdd", (char*) sysMmuMapAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysMmuPteShow", (char*) sysMmuPteShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysModel", (char*) sysModel, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysMsDelay", (char*) sysMsDelay, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysNvRamGet", (char*) sysNvRamGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysNvRamSet", (char*) sysNvRamSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysOutByte", (char*) sysOutByte, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysOutLong", (char*) sysOutLong, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysOutWord", (char*) sysOutWord, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysPci16Swap", (char*) sysPci16Swap, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysPci32Swap", (char*) sysPci32Swap, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysPciConfigInsertLong", (char*) sysPciConfigInsertLong, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysPhysMemDesc", (char*) &sysPhysMemDesc, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sysPhysMemDescNumEnt", (char*) &sysPhysMemDescNumEnt, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "sysPhysMemTop", (char*) sysPhysMemTop, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysPhysToVirt", (char*) sysPhysToVirt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysProcNum", (char*) &sysProcNum, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sysProcNumGet", (char*) sysProcNumGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysProcNumSet", (char*) sysProcNumSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysRamDrvConfig", (char*) sysRamDrvConfig, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysRdCpuReg", (char*) sysRdCpuReg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysSerialChanGet", (char*) sysSerialChanGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysSerialHwInit", (char*) sysSerialHwInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysSerialHwInit2", (char*) sysSerialHwInit2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysSioRead", (char*) sysSioRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysSioWrite", (char*) sysSioWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysStartType", (char*) &sysStartType, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "sysSymTbl", (char*) &sysSymTbl, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "sysTimeBaseLGet", (char*) sysTimeBaseLGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysTimeBaseLPut", (char*) sysTimeBaseLPut, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysTimestamp", (char*) sysTimestamp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysTimestampConnect", (char*) sysTimestampConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysTimestampDisable", (char*) sysTimestampDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysTimestampEnable", (char*) sysTimestampEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysTimestampFreq", (char*) sysTimestampFreq, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysTimestampLock", (char*) sysTimestampLock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysTimestampPeriod", (char*) sysTimestampPeriod, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysToMonitor", (char*) sysToMonitor, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysUsDelay", (char*) sysUsDelay, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysUserLEDSet", (char*) sysUserLEDSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysVirtToPhys", (char*) sysVirtToPhys, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sys_close__7filebuf", (char*) sys_close__7filebuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sys_close__9streambuf", (char*) sys_close__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sys_read__7filebufPci", (char*) sys_read__7filebufPci, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sys_read__9streambufPci", (char*) sys_read__9streambufPci, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sys_seek__7filebuflQ23ios8seek_dir", (char*) sys_seek__7filebuflQ23ios8seek_dir, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sys_seek__9streambuflQ23ios8seek_dir", (char*) sys_seek__9streambuflQ23ios8seek_dir, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sys_stat__7filebufPv", (char*) sys_stat__7filebufPv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sys_stat__9streambufPv", (char*) sys_stat__9streambufPv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sys_write__7filebufPCci", (char*) sys_write__7filebufPCci, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sys_write__9streambufPCci", (char*) sys_write__9streambufPCci, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "system", (char*) system, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tan", (char*) tan, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tanh", (char*) tanh, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "targetName", (char*) targetName, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskActivate", (char*) taskActivate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskArgsGet", (char*) taskArgsGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskArgsSet", (char*) taskArgsSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskBpHook", (char*) &taskBpHook, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "taskBpHookSet", (char*) taskBpHookSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskClassId", (char*) &taskClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "taskCreat", (char*) taskCreat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskCreateHookAdd", (char*) taskCreateHookAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskCreateHookDelete", (char*) taskCreateHookDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskCreateHookShow", (char*) taskCreateHookShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskCreateTable", (char*) &taskCreateTable, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "taskDelay", (char*) taskDelay, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskDelete", (char*) taskDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskDeleteForce", (char*) taskDeleteForce, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskDeleteHookAdd", (char*) taskDeleteHookAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskDeleteHookDelete", (char*) taskDeleteHookDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskDeleteHookShow", (char*) taskDeleteHookShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskDeleteTable", (char*) &taskDeleteTable, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "taskDestroy", (char*) taskDestroy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskHookInit", (char*) taskHookInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskHookShowInit", (char*) taskHookShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskIdCurrent", (char*) &taskIdCurrent, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "taskIdDefault", (char*) taskIdDefault, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskIdFigure", (char*) taskIdFigure, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskIdListGet", (char*) taskIdListGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskIdListSort", (char*) taskIdListSort, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskIdSelf", (char*) taskIdSelf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskIdVerify", (char*) taskIdVerify, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskInfoGet", (char*) taskInfoGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskInit", (char*) taskInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskInstClassId", (char*) &taskInstClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "taskIsReady", (char*) taskIsReady, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskIsSuspended", (char*) taskIsSuspended, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskLibInit", (char*) taskLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskLock", (char*) taskLock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskMsrDefault", (char*) &taskMsrDefault, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "taskMsrSet", (char*) taskMsrSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskName", (char*) taskName, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskNameToId", (char*) taskNameToId, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskOptionsGet", (char*) taskOptionsGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskOptionsSet", (char*) taskOptionsSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskOptionsString", (char*) taskOptionsString, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskPriRangeCheck", (char*) &taskPriRangeCheck, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "taskPriorityGet", (char*) taskPriorityGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskPrioritySet", (char*) taskPrioritySet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskRegName", (char*) &taskRegName, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "taskRegsFmt", (char*) &taskRegsFmt, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "taskRegsGet", (char*) taskRegsGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskRegsInit", (char*) taskRegsInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskRegsSet", (char*) taskRegsSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskRegsShow", (char*) taskRegsShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskRestart", (char*) taskRestart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskResume", (char*) taskResume, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskRtnValueSet", (char*) taskRtnValueSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskSafe", (char*) taskSafe, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskShow", (char*) taskShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskShowInit", (char*) taskShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskSpawn", (char*) taskSpawn, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskStackAllot", (char*) taskStackAllot, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskStatusString", (char*) taskStatusString, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskSuspend", (char*) taskSuspend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskSwapHookAdd", (char*) taskSwapHookAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskSwapHookAttach", (char*) taskSwapHookAttach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskSwapHookDelete", (char*) taskSwapHookDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskSwapHookDetach", (char*) taskSwapHookDetach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskSwapHookShow", (char*) taskSwapHookShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskSwapReference", (char*) &taskSwapReference, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "taskSwapTable", (char*) &taskSwapTable, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "taskSwitchHookAdd", (char*) taskSwitchHookAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskSwitchHookDelete", (char*) taskSwitchHookDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskSwitchHookShow", (char*) taskSwitchHookShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskSwitchTable", (char*) &taskSwitchTable, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "taskTcb", (char*) taskTcb, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskTerminate", (char*) taskTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskUndelay", (char*) taskUndelay, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskUnlock", (char*) taskUnlock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskUnsafe", (char*) taskUnsafe, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskVarAdd", (char*) taskVarAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskVarDelete", (char*) taskVarDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskVarGet", (char*) taskVarGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskVarInfo", (char*) taskVarInfo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskVarInit", (char*) taskVarInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskVarSet", (char*) taskVarSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tbTempHi", (char*) &tbTempHi, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tbTempLo", (char*) &tbTempLo, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tcbinfo", (char*) &tcbinfo, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "tcpCfgParams", (char*) &tcpCfgParams, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tcpDebugShow", (char*) tcpDebugShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcpLibInit", (char*) tcpLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcpOutRsts", (char*) &tcpOutRsts, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "tcpRandHookAdd", (char*) tcpRandHookAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcpRandHookDelete", (char*) tcpRandHookDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcpReportRtn", (char*) &tcpReportRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tcpShowInit", (char*) tcpShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcpTraceRtn", (char*) &tcpTraceRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tcp_alpha", (char*) &tcp_alpha, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "tcp_attach", (char*) tcp_attach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_backoff", (char*) &tcp_backoff, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tcp_beta", (char*) &tcp_beta, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "tcp_canceltimers", (char*) tcp_canceltimers, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_close", (char*) tcp_close, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_ctlinput", (char*) tcp_ctlinput, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_ctloutput", (char*) tcp_ctloutput, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_disconnect", (char*) tcp_disconnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_do_rfc1323", (char*) &tcp_do_rfc1323, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tcp_dooptions", (char*) tcp_dooptions, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_drain", (char*) tcp_drain, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_drop", (char*) tcp_drop, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_fasttimo", (char*) tcp_fasttimo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_init", (char*) tcp_init, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_input", (char*) tcp_input, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_iss", (char*) &tcp_iss, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "tcp_keepcnt", (char*) &tcp_keepcnt, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tcp_keepidle", (char*) &tcp_keepidle, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tcp_keepinit", (char*) &tcp_keepinit, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tcp_keepintvl", (char*) &tcp_keepintvl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tcp_last_inpcb", (char*) &tcp_last_inpcb, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tcp_maxidle", (char*) &tcp_maxidle, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "tcp_maxpersistidle", (char*) &tcp_maxpersistidle, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tcp_msl", (char*) &tcp_msl, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "tcp_mss", (char*) tcp_mss, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_mssdflt", (char*) &tcp_mssdflt, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tcp_newtcpcb", (char*) tcp_newtcpcb, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_notify", (char*) tcp_notify, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_now", (char*) &tcp_now, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "tcp_outflags", (char*) &tcp_outflags, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tcp_output", (char*) tcp_output, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_pcbhashsize", (char*) &tcp_pcbhashsize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tcp_pulloutofband", (char*) tcp_pulloutofband, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_quench", (char*) tcp_quench, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_reass", (char*) tcp_reass, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_recvspace", (char*) &tcp_recvspace, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tcp_respond", (char*) tcp_respond, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_rtlookup", (char*) tcp_rtlookup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_rttdflt", (char*) &tcp_rttdflt, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tcp_saveti", (char*) &tcp_saveti, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "tcp_sendspace", (char*) &tcp_sendspace, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tcp_setpersist", (char*) tcp_setpersist, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_slowtimo", (char*) tcp_slowtimo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_template", (char*) tcp_template, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_timers", (char*) tcp_timers, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_totbackoff", (char*) &tcp_totbackoff, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tcp_updatemtu", (char*) tcp_updatemtu, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_usrclosed", (char*) tcp_usrclosed, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_usrreq", (char*) tcp_usrreq, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcp_xmit_timer", (char*) tcp_xmit_timer, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcpcb", (char*) &tcpcb, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "tcprexmtthresh", (char*) &tcprexmtthresh, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tcpstat", (char*) &tcpstat, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "tcpstatShow", (char*) tcpstatShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tcpstates", (char*) &tcpstates, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "td", (char*) td, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tellg__7istream", (char*) tellg__7istream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tellp__7ostream", (char*) tellp__7ostream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "terminate__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) terminate__Ct12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "terminate__Fv", (char*) terminate__Fv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "testInfo", (char*) &testInfo, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "testIsr", (char*) testIsr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "testRdReg", (char*) testRdReg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "test_and_comparison", (char*) test_and_comparison, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "test_bitflip_comparison", (char*) test_bitflip_comparison, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "test_bitspread_comparison", (char*) test_bitspread_comparison, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "test_blockseq_comparison", (char*) test_blockseq_comparison, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "test_checkerboard_comparison", (char*) test_checkerboard_comparison, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "test_div_comparison", (char*) test_div_comparison, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "test_moving_inv32", (char*) test_moving_inv32, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "test_mul_comparison", (char*) test_mul_comparison, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "test_or_comparison", (char*) test_or_comparison, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "test_seqinc_comparison", (char*) test_seqinc_comparison, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "test_solidbits_comparison", (char*) test_solidbits_comparison, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "test_stuck_address", (char*) test_stuck_address, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "test_sub_comparison", (char*) test_sub_comparison, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "test_verify_success", (char*) test_verify_success, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "test_walkbits_comparison", (char*) test_walkbits_comparison, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "test_xor_comparison", (char*) test_xor_comparison, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "testmem_perf", (char*) testmem_perf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tests_1arg", (char*) &tests_1arg, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tests_noarg", (char*) &tests_noarg, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tftpCopy", (char*) tftpCopy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tftpErrorCreate", (char*) tftpErrorCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tftpGet", (char*) tftpGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tftpInfoShow", (char*) tftpInfoShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tftpInit", (char*) tftpInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tftpModeSet", (char*) tftpModeSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tftpPeerSet", (char*) tftpPeerSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tftpPut", (char*) tftpPut, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tftpQuit", (char*) tftpQuit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tftpReXmit", (char*) &tftpReXmit, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tftpSend", (char*) tftpSend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tftpTask", (char*) tftpTask, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tftpTaskOptions", (char*) &tftpTaskOptions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tftpTaskPriority", (char*) &tftpTaskPriority, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tftpTaskStackSize", (char*) &tftpTaskStackSize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tftpTimeout", (char*) &tftpTimeout, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tftpTrace", (char*) &tftpTrace, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tftpVerbose", (char*) &tftpVerbose, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tftpXfer", (char*) tftpXfer, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ti", (char*) ti, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tick64Get", (char*) tick64Get, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tick64Set", (char*) tick64Set, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tickAnnounce", (char*) tickAnnounce, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tickGet", (char*) tickGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tickQHead", (char*) &tickQHead, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "tickSet", (char*) tickSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tie__3iosP7ostream", (char*) tie__3iosP7ostream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tie__C3ios", (char*) tie__C3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "time", (char*) time, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "timeFiles", (char*) &timeFiles, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "timex", (char*) timex, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "timexClear", (char*) timexClear, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "timexFunc", (char*) timexFunc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "timexHelp", (char*) timexHelp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "timexInit", (char*) timexInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "timexN", (char*) timexN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "timexPost", (char*) timexPost, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "timexPre", (char*) timexPre, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "timexShow", (char*) timexShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tkRcvRtnCall", (char*) tkRcvRtnCall, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tmpfile", (char*) tmpfile, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tmpnam", (char*) tmpnam, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tolower", (char*) tolower, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "totalerrors", (char*) &totalerrors, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "toupper", (char*) toupper, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tr", (char*) tr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "trcDefaultArgs", (char*) &trcDefaultArgs, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "trcScanDepth", (char*) &trcScanDepth, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "trcStack", (char*) trcStack, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "trgCnt", (char*) &trgCnt, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "trgEvtClass", (char*) &trgEvtClass, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "ts", (char*) ts, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tsfsUploadPathClose", (char*) tsfsUploadPathClose, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tsfsUploadPathCreate", (char*) tsfsUploadPathCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tsfsUploadPathLibInit", (char*) tsfsUploadPathLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tsfsUploadPathWrite", (char*) tsfsUploadPathWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tt", (char*) tt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ttyDevCreate", (char*) ttyDevCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ttyDrv", (char*) ttyDrv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "txfei0counter", (char*) &txfei0counter, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "txfei1counter", (char*) &txfei1counter, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tyAbortFuncSet", (char*) tyAbortFuncSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tyAbortSet", (char*) tyAbortSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tyBackspaceChar", (char*) &tyBackspaceChar, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tyBackspaceSet", (char*) tyBackspaceSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tyDeleteLineChar", (char*) &tyDeleteLineChar, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tyDeleteLineSet", (char*) tyDeleteLineSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tyDevInit", (char*) tyDevInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tyDevRemove", (char*) tyDevRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tyEOFSet", (char*) tyEOFSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tyEofChar", (char*) &tyEofChar, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tyIRd", (char*) tyIRd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tyITx", (char*) tyITx, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tyIoctl", (char*) tyIoctl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tyMonitorTrapSet", (char*) tyMonitorTrapSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tyRead", (char*) tyRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tyWrite", (char*) tyWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "udb", (char*) &udb, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "udbinfo", (char*) &udbinfo, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "udpCfgParams", (char*) &udpCfgParams, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "udpCommIfInit", (char*) udpCommIfInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "udpDoCkSumRcv", (char*) &udpDoCkSumRcv, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "udpLibInit", (char*) udpLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "udpRcv", (char*) udpRcv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "udpShowInit", (char*) udpShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "udp_ctlinput", (char*) udp_ctlinput, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "udp_in", (char*) &udp_in, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "udp_init", (char*) udp_init, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "udp_input", (char*) udp_input, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "udp_last_inpcb", (char*) &udp_last_inpcb, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "udp_output", (char*) udp_output, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "udp_pcbhashsize", (char*) &udp_pcbhashsize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "udp_recvspace", (char*) &udp_recvspace, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "udp_sendspace", (char*) &udp_sendspace, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "udp_ttl", (char*) &udp_ttl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "udp_usrreq", (char*) udp_usrreq, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "udpcksum", (char*) &udpcksum, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "udpstat", (char*) &udpstat, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "udpstatShow", (char*) udpstatShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "uflashBuf", (char*) &uflashBuf, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "uflow__9streambuf", (char*) uflow__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "uiomove", (char*) uiomove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "unbuffered__9streambuf", (char*) unbuffered__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "unbuffered__9streambufi", (char*) unbuffered__9streambufi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "uncaught_exception__Fv", (char*) uncaught_exception__Fv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "underflow__7filebuf", (char*) underflow__7filebuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "underflow__9streambuf", (char*) underflow__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "unexpected__Fv", (char*) unexpected__Fv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "unget__7istream", (char*) unget__7istream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "unget__7istreamc", (char*) unget__7istreamc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ungetc", (char*) ungetc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "unique__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0", (char*) unique__t12basic_string3ZcZt18string_char_traits1ZcZt24__default_alloc_template2b1i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "unld", (char*) unld, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "unldByGroup", (char*) unldByGroup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "unldByModuleId", (char*) unldByModuleId, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "unldByNameAndPath", (char*) unldByNameAndPath, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "unldTextSegmentCheck", (char*) unldTextSegmentCheck, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "unlink", (char*) unlink, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "unlock__FR7istream", (char*) unlock__FR7istream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "unlock__FR7ostream", (char*) unlock__FR7ostream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "unsave_markers__9streambuf", (char*) unsave_markers__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "unset__3ios11state_value", (char*) unset__3ios11state_value, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "unsetf__3iosUl", (char*) unsetf__3iosUl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "useloopback", (char*) &useloopback, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "userRsrvMemInit", (char*) userRsrvMemInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "userRsrvMemMalloc", (char*) userRsrvMemMalloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "userRsrvMemMallocAligned", (char*) userRsrvMemMallocAligned, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrAltivecInit", (char*) usrAltivecInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrBootLineCrack", (char*) usrBootLineCrack, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrBootLineInit", (char*) usrBootLineInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrBpInit", (char*) usrBpInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrBreakpointSet", (char*) usrBreakpointSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrClock", (char*) usrClock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrDmaDone", (char*) usrDmaDone, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrDmaInit", (char*) usrDmaInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrExtraModules", (char*) &usrExtraModules, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "usrInit", (char*) usrInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrKernelInit", (char*) usrKernelInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrMem2MemDmaStart", (char*) usrMem2MemDmaStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrMem2PciDmaStart", (char*) usrMem2PciDmaStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrMmuInit", (char*) usrMmuInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrNetIfAttach", (char*) usrNetIfAttach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrNetIfConfig", (char*) usrNetIfConfig, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrNetIfTbl", (char*) &usrNetIfTbl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "usrNetInit", (char*) usrNetInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrNetProtoInit", (char*) usrNetProtoInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrPPPInit", (char*) usrPPPInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrPci2MemDmaStart", (char*) usrPci2MemDmaStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrPci2PciDmaStart", (char*) usrPci2PciDmaStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrReadGPR", (char*) usrReadGPR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrRoot", (char*) usrRoot, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrSlipInit", (char*) usrSlipInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrStartupScript", (char*) usrStartupScript, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrWriteGPR", (char*) usrWriteGPR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrflashBuf", (char*) &usrflashBuf, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "uswab", (char*) uswab, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "utime", (char*) utime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "valloc", (char*) valloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vecOffRelocMatch", (char*) vecOffRelocMatch, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vecOffRelocMatchRev", (char*) vecOffRelocMatchRev, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "version", (char*) version, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vfdprintf", (char*) vfdprintf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vfprintf", (char*) vfprintf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vmBaseGlobalMapInit", (char*) vmBaseGlobalMapInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vmBaseLibInit", (char*) vmBaseLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vmBasePageSizeGet", (char*) vmBasePageSizeGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vmBaseStateSet", (char*) vmBaseStateSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vmLibInfo", (char*) &vmLibInfo, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "vmpxx", (char*) vmpxx, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vprintf", (char*) vprintf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vscan__7istreamPCcP13__va_list_tag", (char*) vscan__7istreamPCcP13__va_list_tag, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vscan__9streambufPCcP13__va_list_tagP3ios", (char*) vscan__9streambufPCcP13__va_list_tagP3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vsprintf", (char*) vsprintf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxAbsTicks", (char*) &vxAbsTicks, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "vxCntmrIntConnect", (char*) vxCntmrIntConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxCntmrIntCtrlInit", (char*) vxCntmrIntCtrlInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxCntmrIntDisable", (char*) vxCntmrIntDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxCntmrIntEnable", (char*) vxCntmrIntEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxCunitIntClear", (char*) vxCunitIntClear, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxCunitIntConnect", (char*) vxCunitIntConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxCunitIntCtrlInit", (char*) vxCunitIntCtrlInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxCunitIntDisable", (char*) vxCunitIntDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxCunitIntEnable", (char*) vxCunitIntEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxDarGet", (char*) vxDarGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxDarSet", (char*) vxDarSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxDecGet", (char*) vxDecGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxDecReload", (char*) vxDecReload, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxDecSet", (char*) vxDecSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxDsisrGet", (char*) vxDsisrGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxDsisrSet", (char*) vxDsisrSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxEieio", (char*) vxEieio, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxEthernetIntCntlInit", (char*) vxEthernetIntCntlInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxEthernetIntConnect", (char*) vxEthernetIntConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxEthernetIntDisable", (char*) vxEthernetIntDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxEthernetIntEnable", (char*) vxEthernetIntEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxEventPendQ", (char*) &vxEventPendQ, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "vxFirstBit", (char*) vxFirstBit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxFpscrGet", (char*) vxFpscrGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxFpscrSet", (char*) vxFpscrSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxGhsFlagSet", (char*) vxGhsFlagSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxHid0Get", (char*) vxHid0Get, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxHid0Set", (char*) vxHid0Set, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxHid1Get", (char*) vxHid1Get, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxHid1Set", (char*) vxHid1Set, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxImmrDevGet", (char*) vxImmrDevGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxImmrIsbGet", (char*) vxImmrIsbGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxIntStackBase", (char*) &vxIntStackBase, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "vxIntStackEnd", (char*) &vxIntStackEnd, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "vxMemArchProbe", (char*) vxMemArchProbe, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxMemProbe", (char*) vxMemProbe, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxMemProbeSup", (char*) vxMemProbeSup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxMpscIntClear", (char*) vxMpscIntClear, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxMpscIntCntlInit", (char*) vxMpscIntCntlInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxMpscIntConnect", (char*) vxMpscIntConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxMpscIntDisable", (char*) vxMpscIntDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxMpscIntEnable", (char*) vxMpscIntEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxMpscUartDevInit", (char*) vxMpscUartDevInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxMpscUartDevReset", (char*) vxMpscUartDevReset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxMpscUartRxInt", (char*) vxMpscUartRxInt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxMpscUartTxInt", (char*) vxMpscUartTxInt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxMsrGet", (char*) vxMsrGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxMsrSet", (char*) vxMsrSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxPowMgtEnable", (char*) &vxPowMgtEnable, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "vxPowerDown", (char*) vxPowerDown, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxPowerModeGet", (char*) vxPowerModeGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxPowerModeSet", (char*) vxPowerModeSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxPvrGet", (char*) vxPvrGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxSdmaIntClear", (char*) vxSdmaIntClear, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxSdmaIntConnect", (char*) vxSdmaIntConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxSdmaIntCtrlInit", (char*) vxSdmaIntCtrlInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxSdmaIntDisable", (char*) vxSdmaIntDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxSdmaIntEnable", (char*) vxSdmaIntEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxSrr0Get", (char*) vxSrr0Get, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxSrr0Set", (char*) vxSrr0Set, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxSrr1Get", (char*) vxSrr1Get, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxSrr1Set", (char*) vxSrr1Set, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxTas", (char*) vxTas, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxTaskEntry", (char*) vxTaskEntry, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxTicks", (char*) &vxTicks, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "vxTimeBaseGet", (char*) vxTimeBaseGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxTimeBaseSet", (char*) vxTimeBaseSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxWorksVersion", (char*) &vxWorksVersion, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wakeup", (char*) wakeup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "watchDogApp", (char*) watchDogApp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "watchDogIsr", (char*) watchDogIsr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "watchdogAppId", (char*) &watchdogAppId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "watchdogFlag", (char*) &watchdogFlag, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "watchdogTest", (char*) watchdogTest, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wcstombs", (char*) wcstombs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wctomb", (char*) wctomb, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdCancel", (char*) wdCancel, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdClassId", (char*) &wdClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wdCreate", (char*) wdCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdDelete", (char*) wdDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdDestroy", (char*) wdDestroy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdInit", (char*) wdInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdInstClassId", (char*) &wdInstClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wdLibInit", (char*) wdLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdShow", (char*) wdShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdShowInit", (char*) wdShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdStart", (char*) wdStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdTerminate", (char*) wdTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdTick", (char*) wdTick, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdValueReg", (char*) wdValueReg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbAltivecGet", (char*) wdbAltivecGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbAltivecLibInit", (char*) wdbAltivecLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbAltivecRestore", (char*) wdbAltivecRestore, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbAltivecSave", (char*) wdbAltivecSave, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbAltivecSet", (char*) wdbAltivecSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbBpInstall", (char*) wdbBpInstall, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbCksum", (char*) wdbCksum, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbCommMtu", (char*) &wdbCommMtu, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wdbConfig", (char*) wdbConfig, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbConnectLibInit", (char*) wdbConnectLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbCtxCreate", (char*) wdbCtxCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbCtxExitLibInit", (char*) wdbCtxExitLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbCtxExitNotifyHook", (char*) wdbCtxExitNotifyHook, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbCtxLibInit", (char*) wdbCtxLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbCtxResume", (char*) wdbCtxResume, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbCtxStartLibInit", (char*) wdbCtxStartLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgArchInit", (char*) wdbDbgArchInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgBpFind", (char*) wdbDbgBpFind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgBpGet", (char*) wdbDbgBpGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgBpListInit", (char*) wdbDbgBpListInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgBpRemove", (char*) wdbDbgBpRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgBpRemoveAll", (char*) wdbDbgBpRemoveAll, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgBreakpoint", (char*) wdbDbgBreakpoint, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgDabrGet", (char*) wdbDbgDabrGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgDabrSet", (char*) wdbDbgDabrSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgDarGet", (char*) wdbDbgDarGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgDataAccessStub", (char*) wdbDbgDataAccessStub, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgHwAddrCheck", (char*) wdbDbgHwAddrCheck, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgHwBpFind", (char*) wdbDbgHwBpFind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgHwBpHandle", (char*) wdbDbgHwBpHandle, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgHwBpSet", (char*) wdbDbgHwBpSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgHwBpStub", (char*) wdbDbgHwBpStub, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgIabrGet", (char*) wdbDbgIabrGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgIabrSet", (char*) wdbDbgIabrSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgRegsClear", (char*) wdbDbgRegsClear, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgRegsSet", (char*) wdbDbgRegsSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgTrace", (char*) wdbDbgTrace, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgTraceModeClear", (char*) wdbDbgTraceModeClear, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgTraceModeSet", (char*) wdbDbgTraceModeSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgTraceStub", (char*) wdbDbgTraceStub, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDirectCallLibInit", (char*) wdbDirectCallLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbE", (char*) wdbE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbEndDebug", (char*) &wdbEndDebug, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wdbEndPktDevInit", (char*) wdbEndPktDevInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbEventDeq", (char*) wdbEventDeq, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbEventLibInit", (char*) wdbEventLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbEventNodeInit", (char*) wdbEventNodeInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbEventPost", (char*) wdbEventPost, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbEvtptClassConnect", (char*) wdbEvtptClassConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbEvtptLibInit", (char*) wdbEvtptLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbExcLibInit", (char*) wdbExcLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbExternEnterHook", (char*) wdbExternEnterHook, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbExternExitHook", (char*) wdbExternExitHook, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbExternInit", (char*) wdbExternInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbExternRegSetObjAdd", (char*) wdbExternRegSetObjAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbExternRegsGet", (char*) wdbExternRegsGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbExternRegsSet", (char*) wdbExternRegsSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbExternSystemRegs", (char*) &wdbExternSystemRegs, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wdbFpLibInit", (char*) wdbFpLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbFppGet", (char*) wdbFppGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbFppRestore", (char*) wdbFppRestore, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbFppSave", (char*) wdbFppSave, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbFppSet", (char*) wdbFppSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbFuncCallLibInit", (char*) wdbFuncCallLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbGopherLibInit", (char*) wdbGopherLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbGopherLock", (char*) &wdbGopherLock, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wdbInfoGet", (char*) wdbInfoGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbInstallCommIf", (char*) wdbInstallCommIf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbInstallRtIf", (char*) wdbInstallRtIf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbIsNowExternal", (char*) wdbIsNowExternal, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbIsNowTasking", (char*) wdbIsNowTasking, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbMbufAlloc", (char*) wdbMbufAlloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbMbufFree", (char*) wdbMbufFree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbMemCoreLibInit", (char*) wdbMemCoreLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbMemLibInit", (char*) wdbMemLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbMemTest", (char*) wdbMemTest, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbModeSet", (char*) wdbModeSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbNotifyHost", (char*) wdbNotifyHost, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbNptInt", (char*) wdbNptInt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbNptShutdown", (char*) wdbNptShutdown, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbNumMemRegions", (char*) &wdbNumMemRegions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wdbOneShot", (char*) &wdbOneShot, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wdbRegsLibInit", (char*) wdbRegsLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbResumeSystem", (char*) wdbResumeSystem, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbRpcGetArgs", (char*) wdbRpcGetArgs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbRpcNotifyConnect", (char*) wdbRpcNotifyConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbRpcNotifyHost", (char*) wdbRpcNotifyHost, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbRpcRcv", (char*) wdbRpcRcv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbRpcReply", (char*) wdbRpcReply, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbRpcReplyErr", (char*) wdbRpcReplyErr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbRpcResendReply", (char*) wdbRpcResendReply, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbRpcXportInit", (char*) wdbRpcXportInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbRunsExternal", (char*) wdbRunsExternal, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbRunsTasking", (char*) wdbRunsTasking, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbSp", (char*) wdbSp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbSuspendSystem", (char*) wdbSuspendSystem, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbSuspendSystemHere", (char*) wdbSuspendSystemHere, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbSvcAdd", (char*) wdbSvcAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbSvcDispatch", (char*) wdbSvcDispatch, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbSvcDsaSvcRemove", (char*) wdbSvcDsaSvcRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbSvcHookAdd", (char*) wdbSvcHookAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbSvcLibInit", (char*) wdbSvcLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbSysBpLibInit", (char*) wdbSysBpLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbSystemSuspend", (char*) wdbSystemSuspend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbTargetIsConnected", (char*) wdbTargetIsConnected, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbTask", (char*) wdbTask, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbTaskBpLibInit", (char*) wdbTaskBpLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbTaskId", (char*) &wdbTaskId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wdbTaskInit", (char*) wdbTaskInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbToolName", (char*) &wdbToolName, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wdbTsfsDefaultDirPerm", (char*) &wdbTsfsDefaultDirPerm, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wdbTsfsDrv", (char*) wdbTsfsDrv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbUserEvtLibInit", (char*) wdbUserEvtLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbUserEvtPost", (char*) wdbUserEvtPost, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbVioChannelRegister", (char*) wdbVioChannelRegister, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbVioChannelUnregister", (char*) wdbVioChannelUnregister, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbVioDrv", (char*) wdbVioDrv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbVioLibInit", (char*) wdbVioLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdgCnt", (char*) &wdgCnt, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wdgNMICnt", (char*) &wdgNMICnt, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wdgService", (char*) &wdgService, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "what__C11logic_error", (char*) what__C11logic_error, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "what__C13runtime_error", (char*) what__C13runtime_error, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "what__C9bad_alloc", (char*) what__C9bad_alloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "what__C9exception", (char*) what__C9exception, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "whoami", (char*) whoami, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "width__3iosi", (char*) width__3iosi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "width__C3ios", (char*) width__C3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wildcard", (char*) &wildcard, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "windDelay", (char*) windDelay, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windDelete", (char*) windDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windExit", (char*) windExit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windIntStackSet", (char*) windIntStackSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windLoadContext", (char*) windLoadContext, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windPendQFlush", (char*) windPendQFlush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windPendQGet", (char*) windPendQGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windPendQPut", (char*) windPendQPut, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windPendQRemove", (char*) windPendQRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windPendQTerminate", (char*) windPendQTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windPriNormalSet", (char*) windPriNormalSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windPrioritySet", (char*) windPrioritySet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windReadyQPut", (char*) windReadyQPut, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windReadyQRemove", (char*) windReadyQRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windResume", (char*) windResume, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windSemDelete", (char*) windSemDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windSpawn", (char*) windSpawn, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windSuspend", (char*) windSuspend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windTickAnnounce", (char*) windTickAnnounce, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windUndelay", (char*) windUndelay, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windWdCancel", (char*) windWdCancel, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windWdStart", (char*) windWdStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windviewConfig", (char*) windviewConfig, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "workQAdd0", (char*) workQAdd0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "workQAdd1", (char*) workQAdd1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "workQAdd2", (char*) workQAdd2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "workQDoWork", (char*) workQDoWork, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "workQInit", (char*) workQInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "workQIsEmpty", (char*) &workQIsEmpty, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "workQPanic", (char*) workQPanic, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "workQReadIx", (char*) &workQReadIx, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "workQWriteIx", (char*) &workQWriteIx, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "writable__3ios", (char*) writable__3ios, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "write", (char*) write, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "write__7ostreamPCSci", (char*) write__7ostreamPCSci, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "write__7ostreamPCUci", (char*) write__7ostreamPCUci, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "write__7ostreamPCci", (char*) write__7ostreamPCci, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "write__7ostreamPCvi", (char*) write__7ostreamPCvi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "writev", (char*) writev, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wrreg", (char*) wrreg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ws__FR7istream", (char*) ws__FR7istream, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvBufId", (char*) &wvBufId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvDefaultBufMax", (char*) &wvDefaultBufMax, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvDefaultBufMin", (char*) &wvDefaultBufMin, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvDefaultBufOptions", (char*) &wvDefaultBufOptions, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvDefaultBufSize", (char*) &wvDefaultBufSize, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvDefaultBufThresh", (char*) &wvDefaultBufThresh, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvDefaultRBuffParams", (char*) &wvDefaultRBuffParams, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvEvent", (char*) wvEvent, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvEventInst", (char*) wvEventInst, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvEvtBufferFullNotify", (char*) &wvEvtBufferFullNotify, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wvEvtBufferGet", (char*) wvEvtBufferGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvEvtClass", (char*) &wvEvtClass, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvEvtClassClear", (char*) wvEvtClassClear, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvEvtClassClearAll", (char*) wvEvtClassClearAll, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvEvtClassGet", (char*) wvEvtClassGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvEvtClassSet", (char*) wvEvtClassSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvEvtLogInit", (char*) wvEvtLogInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvEvtLogStart", (char*) wvEvtLogStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvEvtLogStop", (char*) wvEvtLogStop, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvInstIsOn", (char*) &wvInstIsOn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvLibInit", (char*) wvLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvLibInit2", (char*) wvLibInit2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvLogHeader", (char*) &wvLogHeader, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvLogHeaderCreate", (char*) wvLogHeaderCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvLogHeaderUpload", (char*) wvLogHeaderUpload, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvObjInst", (char*) wvObjInst, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvObjInstModeSet", (char*) wvObjInstModeSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvObjIsEnabled", (char*) &wvObjIsEnabled, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvOff", (char*) wvOff, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvOn", (char*) wvOn, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvRBuffErrorHandler", (char*) wvRBuffErrorHandler, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvRBuffMgr", (char*) wvRBuffMgr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvRBuffMgrId", (char*) &wvRBuffMgrId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wvRBuffMgrPrioritySet", (char*) wvRBuffMgrPrioritySet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvSigInst", (char*) wvSigInst, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvTaskNamesBufAdd", (char*) wvTaskNamesBufAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvTaskNamesPreserve", (char*) wvTaskNamesPreserve, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvTaskNamesUpload", (char*) wvTaskNamesUpload, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvTmrRegister", (char*) wvTmrRegister, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvUpPathId", (char*) &wvUpPathId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvUpTaskId", (char*) &wvUpTaskId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvUploadMaxAttempts", (char*) &wvUploadMaxAttempts, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wvUploadRetryBackoff", (char*) &wvUploadRetryBackoff, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wvUploadStart", (char*) wvUploadStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvUploadStop", (char*) wvUploadStop, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvUploadTaskConfig", (char*) wvUploadTaskConfig, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvUploadTaskOptions", (char*) &wvUploadTaskOptions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wvUploadTaskPriority", (char*) &wvUploadTaskPriority, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wvUploadTaskStackSize", (char*) &wvUploadTaskStackSize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "xattrib", (char*) xattrib, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xchain__9streambuf", (char*) xchain__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xcopy", (char*) xcopy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdelete", (char*) xdelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdrCksum", (char*) xdrCksum, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_ARRAY", (char*) xdr_ARRAY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_CHECKSUM", (char*) xdr_CHECKSUM, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_TGT_ADDR_T", (char*) xdr_TGT_ADDR_T, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_TGT_INT_T", (char*) xdr_TGT_INT_T, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_UINT32", (char*) xdr_UINT32, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_AGENT_INFO", (char*) xdr_WDB_AGENT_INFO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_CALL_RETURN_INFO", (char*) xdr_WDB_CALL_RETURN_INFO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_CTX", (char*) xdr_WDB_CTX, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_CTX_CREATE_DESC", (char*) xdr_WDB_CTX_CREATE_DESC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_CTX_STEP_DESC", (char*) xdr_WDB_CTX_STEP_DESC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_EVTPT_ADD_DESC", (char*) xdr_WDB_EVTPT_ADD_DESC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_EVTPT_DEL_DESC", (char*) xdr_WDB_EVTPT_DEL_DESC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_EVT_DATA", (char*) xdr_WDB_EVT_DATA, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_EVT_INFO", (char*) xdr_WDB_EVT_INFO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_MEM_REGION", (char*) xdr_WDB_MEM_REGION, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_MEM_SCAN_DESC", (char*) xdr_WDB_MEM_SCAN_DESC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_MEM_XFER", (char*) xdr_WDB_MEM_XFER, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_OPQ_DATA_T", (char*) xdr_WDB_OPQ_DATA_T, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_PARAM_WRAPPER", (char*) xdr_WDB_PARAM_WRAPPER, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_REG_READ_DESC", (char*) xdr_WDB_REG_READ_DESC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_REG_WRITE_DESC", (char*) xdr_WDB_REG_WRITE_DESC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_REPLY_WRAPPER", (char*) xdr_WDB_REPLY_WRAPPER, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_RT_INFO", (char*) xdr_WDB_RT_INFO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_STRING_T", (char*) xdr_WDB_STRING_T, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_TGT_INFO", (char*) xdr_WDB_TGT_INFO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_accepted_reply", (char*) xdr_accepted_reply, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_array", (char*) xdr_array, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_attrstat", (char*) xdr_attrstat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_authunix_parms", (char*) xdr_authunix_parms, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_bool", (char*) xdr_bool, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_bytes", (char*) xdr_bytes, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_callhdr", (char*) xdr_callhdr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_callmsg", (char*) xdr_callmsg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_char", (char*) xdr_char, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_createargs", (char*) xdr_createargs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_des_block", (char*) xdr_des_block, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_deskey", (char*) xdr_deskey, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_dirlist", (char*) xdr_dirlist, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_diropargs", (char*) xdr_diropargs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_diropokres", (char*) xdr_diropokres, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_diropres", (char*) xdr_diropres, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_dirpath", (char*) xdr_dirpath, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_double", (char*) xdr_double, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_entry", (char*) xdr_entry, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_enum", (char*) xdr_enum, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_exportnode", (char*) xdr_exportnode, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_exports", (char*) xdr_exports, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_fattr", (char*) xdr_fattr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_fhandle", (char*) xdr_fhandle, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_fhstatus", (char*) xdr_fhstatus, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_filename", (char*) xdr_filename, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_float", (char*) xdr_float, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_floatInclude", (char*) xdr_floatInclude, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_free", (char*) xdr_free, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_ftype", (char*) xdr_ftype, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_groupnode", (char*) xdr_groupnode, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_groups", (char*) xdr_groups, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_int", (char*) xdr_int, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_linkargs", (char*) xdr_linkargs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_long", (char*) xdr_long, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_mountbody", (char*) xdr_mountbody, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_mountlist", (char*) xdr_mountlist, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_mountname", (char*) xdr_mountname, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_netobj", (char*) xdr_netobj, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_nfs_fh", (char*) xdr_nfs_fh, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_nfscookie", (char*) xdr_nfscookie, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_nfspath", (char*) xdr_nfspath, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_nfsstat", (char*) xdr_nfsstat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_nfstime", (char*) xdr_nfstime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_opaque", (char*) xdr_opaque, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_opaque_auth", (char*) xdr_opaque_auth, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_pmap", (char*) xdr_pmap, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_pmaplist", (char*) xdr_pmaplist, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_pointer", (char*) xdr_pointer, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_readargs", (char*) xdr_readargs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_readdirargs", (char*) xdr_readdirargs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_readdirres", (char*) xdr_readdirres, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_readlinkres", (char*) xdr_readlinkres, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_readokres", (char*) xdr_readokres, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_readres", (char*) xdr_readres, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_reference", (char*) xdr_reference, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_rejected_reply", (char*) xdr_rejected_reply, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_renameargs", (char*) xdr_renameargs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_replymsg", (char*) xdr_replymsg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_sattr", (char*) xdr_sattr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_sattrargs", (char*) xdr_sattrargs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_short", (char*) xdr_short, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_statfsokres", (char*) xdr_statfsokres, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_statfsres", (char*) xdr_statfsres, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_string", (char*) xdr_string, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_symlinkargs", (char*) xdr_symlinkargs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_u_char", (char*) xdr_u_char, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_u_int", (char*) xdr_u_int, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_u_long", (char*) xdr_u_long, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_u_short", (char*) xdr_u_short, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_union", (char*) xdr_union, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_vector", (char*) xdr_vector, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_void", (char*) xdr_void, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_wrapstring", (char*) xdr_wrapstring, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_writeargs", (char*) xdr_writeargs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdrmem_create", (char*) xdrmem_create, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdrrec_create", (char*) xdrrec_create, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdrrec_endofrecord", (char*) xdrrec_endofrecord, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdrrec_eof", (char*) xdrrec_eof, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdrrec_skiprecord", (char*) xdrrec_skiprecord, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xflags__9streambuf", (char*) xflags__9streambuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xflags__9streambufi", (char*) xflags__9streambufi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xmalloc", (char*) xmalloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xprt_register", (char*) xprt_register, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xprt_unregister", (char*) xprt_unregister, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xput_char__9streambufc", (char*) xput_char__9streambufc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xrealloc", (char*) xrealloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xsetflags__9streambufi", (char*) xsetflags__9streambufi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xsetflags__9streambufii", (char*) xsetflags__9streambufii, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xsgetn__7filebufPci", (char*) xsgetn__7filebufPci, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xsgetn__9streambufPci", (char*) xsgetn__9streambufPci, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xsputn__7filebufPCci", (char*) xsputn__7filebufPCci, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xsputn__9streambufPCci", (char*) xsputn__9streambufPCci, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "yearr", (char*) &yearr, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "yy_yys", (char*) &yy_yys, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "yy_yyv", (char*) &yy_yyv, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "yychar", (char*) &yychar, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "yydebug", (char*) &yydebug, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "yyerrflag", (char*) &yyerrflag, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "yylval", (char*) &yylval, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "yynerrs", (char*) &yynerrs, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "yyparse", (char*) yyparse, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "yyps", (char*) &yyps, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "yypv", (char*) &yypv, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "yys", (char*) &yys, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "yystart", (char*) yystart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "yystate", (char*) &yystate, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "yytmp", (char*) &yytmp, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "yyv", (char*) &yyv, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "yyval", (char*) &yyval, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "zeroin_addr", (char*) &zeroin_addr, 0, SYM_GLOBAL | SYM_BSS},
    };


ULONG standTblSize = 4981;
