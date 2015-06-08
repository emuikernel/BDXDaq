/*
Copyright 1998-2001 Wind River Systems, Inc.

modification history
--------------------
01d,20mar02,sn   SPR 71699 - Name and synopsis of INCLUDE_CPLUS_LANG now
                 defined for all toolchains in 00vxWorks.cdf
01c,07nov01,rbl  remove unnecessary dependency on INCLUDE_GNU_64BIT
01b,06nov01,sn   use new prefixed names
01a,06nov01,sn   wrote

DESCRIPTION

This file contains descriptions for the GNU toolchain specific C++
compiler support and standard library components.
*/

Component INCLUDE_CPLUS_LANG {
	MODULES		_x_gnu___dummy.o \
			_x_gnu__eh.o \
			_x_gnu_cmathi.o \
			_x_gnu_cstdlibi.o \
			_x_gnu_tinfo.o \
			_x_gnu_tinfo2.o \
			_x_gnu_opnew.o \
			_x_gnu_opvnew.o \
			_x_gnu_opnewnt.o \
			_x_gnu_opvnewnt.o \
			_x_gnu_opdel.o \
			_x_gnu_opvdel.o \
			_x_gnu_opdelnt.o \
			_x_gnu_opvdelnt.o \
			_x_gnu_new.o \
			_x_gnu_exception.o
	CONFIGLETTES	cplusgnuLang.c
}

Component INCLUDE_CPLUS_STRING {
	NAME		C++ string type
	MODULES		_x_gnu_cstrmain.o \
			_x_gnu_stdexcepti.o 
	REQUIRES	INCLUDE_CPLUS
	CONFIGLETTES	cplusgnuString.c
}


Component INCLUDE_CPLUS_STRING_IO {
	NAME		C++ string i/o
	MODULES		_x_gnu_cstrio.o 
	REQUIRES	INCLUDE_CPLUS_STRING 
	CONFIGLETTES	cplusgnuStringIo.c
}

Component INCLUDE_CPLUS_COMPLEX {
	NAME		C++ complex number type
	MODULES		_x_gnu_dcomplex.o \
			_x_gnu_ldcomplex.o \
			_x_gnu_fcomplex.o
	REQUIRES	INCLUDE_CPLUS
	CONFIGLETTES	cplusgnuComplex.c 
}

Component INCLUDE_CPLUS_COMPLEX_IO {
	NAME		C++ complex number i/o
	MODULES		_x_gnu_dcomio.o \
			_x_gnu_fcomio.o \
			_x_gnu_ldcomio.o 
	REQUIRES	INCLUDE_CPLUS_COMPLEX 
	CONFIGLETTES	cplusgnuComplexIo.c
}


Component INCLUDE_CPLUS_STL {
	NAME	    	C++ standard template library
	MODULES		_x_gnu_stlinst.o 
	REQUIRES	INCLUDE_CPLUS
	CONFIGLETTES	cplusgnuStl.c
}

Component INCLUDE_CPLUS_IOSTREAMS {
	NAME		core C++ iostreams
	SYNOPSIS	C++ iostreams (core functionality needed for "hello world")
	MODULES		_x_gnu_cleanup.o \
			_x_gnu_filebuf.o \
			_x_gnu_filedoalloc.o \
			_x_gnu_fileops.o \
			_x_gnu_floatconv.o \
			_x_gnu_genops.o \
			_x_gnu_ioassign.o \
			_x_gnu_iogetc.o \
			_x_gnu_iogetline.o \
			_x_gnu_iopadn.o \
			_x_gnu_ioputc.o \
			_x_gnu_ioseekoff.o \
			_x_gnu_ioseekpos.o \
			_x_gnu_iostream.o \
			_x_gnu_ioungetc.o \
			_x_gnu_iovfscanf.o \
			_x_gnu_isgetline.o \
			_x_gnu_isscan.o \
			_x_gnu_outfloat.o \
			_x_gnu_peekc.o \
			_x_gnu_sbscan.o \
			_x_gnu_stdstrbufs.o \
			_x_gnu_stdstreams.o \
			_x_gnu_streambuf.o
	CONFIGLETTES	cplusgnuIos.c
}

Component INCLUDE_CPLUS_IOSTREAMS_FULL {
	NAME	    	full C++ iostreams
	MODULES		_x_gnu_PlotFile.o\
			_x_gnu_SFile.o\
			_x_gnu_editbuf.o\
			_x_gnu_fstream.o\
			_x_gnu_indstream.o\
			_x_gnu_ioextend.o\
			_x_gnu_iofclose.o\
			_x_gnu_iofeof.o\
			_x_gnu_ioferror.o\
			_x_gnu_iofgetpos.o\
			_x_gnu_iofread.o\
			_x_gnu_iofscanf.o\
			_x_gnu_iofsetpos.o\
			_x_gnu_iogetdelim.o\
			_x_gnu_ioignore.o\
			_x_gnu_iomanip.o\
			_x_gnu_ioprims.o\
			_x_gnu_ioprintf.o\
			_x_gnu_iostrerror.o\
			_x_gnu_iovfprintf.o\
			_x_gnu_isgetsb.o\
			_x_gnu_osform.o\
			_x_gnu_parsestream.o\
			_x_gnu_sbform.o\
			_x_gnu_sbgetline.o\
			_x_gnu_stdiostream.o\
			_x_gnu_stream.o\
			_x_gnu_strops.o\
			_x_gnu_strstream.o
	REQUIRES	INCLUDE_CPLUS INCLUDE_CPLUS_IOSTREAMS
	/* This configlette pulls in either minimal or full
         * iostreams depending on whether INCLUDE_CPLUS_IOSTREAMS_FULL
         * is defined */
	CONFIGLETTES	cplusgnuIos.c
}
