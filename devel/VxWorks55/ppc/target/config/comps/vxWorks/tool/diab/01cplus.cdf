/*
Copyright 1998-2001 Wind River Systems, Inc.

modification history
--------------------
01c,20mar02,sn   SPR 74555 - removed empty components
01b,13mar02,sn   SPR 73984, 73342 - added MODULES info
01a,06nov01,sn   wrote

DESCRIPTION

This file contains descriptions for the Diab toolchain specific C++
compiler support and standard library components.
*/

Component INCLUDE_CPLUS_LANG {
	CONFIGLETTES	cplusdiabLang.c
	MODULES		_x_diab_gthrow.o \
         		_x_diab_array_del.o \
		        _x_diab_array_new.o \
	         	_x_diab_array_nonew.o \
		        _x_diab_array_pdel.o \
         		_x_diab_cplustls.o \
         		_x_diab_cppalloc.o \
         		_x_diab_cxxcatch.o \
         		_x_diab_delete.o \
         		_x_diab_dtor_list.o \
         		_x_diab_error.o \
         		_x_diab_memzero.o \
         		_x_diab_new.o \
         		_x_diab_newhandler.o \
         		_x_diab_newnothrow.o \
         		_x_diab_nothrow.o \
         		_x_diab_placedel.o \
         		_x_diab_rtti.o \
         		_x_diab_setterm.o \
         		_x_diab_setunexp.o \
         		_x_diab_terminate.o \
         		_x_diab_typeinfo.o \
         		_x_diab_unexpected.o \
         		_x_diab_vec_newdel.o


}

Component INCLUDE_CPLUS_IOSTREAMS {
	NAME		C++ iostreams and other standard library facilities
	SYNOPSIS	C++ standard library
	REQUIRES        INCLUDE_CPLUS
	CONFIGLETTES    cplusdiabIos.c
	MODULES         _x_diab_fiopen.o \
			_x_diab_iomanip.o \
			_x_diab_ios.o \
			_x_diab_iostream.o \
			_x_diab_limits.o \
			_x_diab_locale0.o \
			_x_diab_locale.o \
			_x_diab_nomemory.o \
			_x_diab_string.o \
         		_x_diab_strstrea.o \
         		_x_diab_strtoll.o \
         		_x_diab_strtoull.o \
         		_x_diab_throw.o \
         		_x_diab_wcslen.o \
         		_x_diab_wiostrea.o \
         		_x_diab_wlocale.o \
         		_x_diab_wmemchr.o \
         		_x_diab_wmemcmp.o \
         		_x_diab_wmemcpy.o \
         		_x_diab_wmemmove.o \
         		_x_diab_wmemset.o \
         		_x_diab_xfvalues.o \
         		_x_diab_xgetglob.o \
         		_x_diab_xgetptimes.o \
         		_x_diab_xlocale.o \
         		_x_diab_xlvalues.o \
         		_x_diab_xstart.o \
         		_x_diab_xsyslock.o \
         		_x_diab_xvalues.o

}
