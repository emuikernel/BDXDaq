/* cplusgnuIos.c - VxWorks iostreams class library initialization */

/* Copyright 1995 Wind River Systems, Inc. */

/*
modification history
--------------------
03e,06nov01,sn   use new prefixed names
03d,24jan01,sn   use wrapper symbols
03c,10oct00,sn   regenerated
03b,03jun98,sn   moved the iostdio files to cplusIosTest.c
                 unless INCLUDE_CPLUS_IOSTREAMS_FULL is defined only
		 pull in what's required by iostream.o
02b,05may98,sn   added the "iostdio" object files
                 fixed a typo in the modification history
02a,15jan98,sn   rewrote to force-link GNU iostreams
01c,22jul96,jmb  fix comment attached to final endif
01b,07jun95,srh  correct duplicate naming problem (SPR 5100)
01a,14jun95,srh  written.
*/

/*
DESCRIPTION
This file is used to include the iostreams classes in the 
VxWorks build. The routines are only included when this file is 
included by usrConfig.c.

NOMANUAL
*/

#ifndef __INCcplusIosc
#define __INCcplusIosc

#ifdef INCLUDE_CPLUS_IOSTREAMS
extern char ___x_gnu_cleanup_o;
extern char ___x_gnu_filebuf_o;
extern char ___x_gnu_filedoalloc_o;
extern char ___x_gnu_fileops_o;
extern char ___x_gnu_floatconv_o;
extern char ___x_gnu_genops_o;
extern char ___x_gnu_ioassign_o;
extern char ___x_gnu_iogetc_o;
extern char ___x_gnu_iogetline_o;
extern char ___x_gnu_iopadn_o;
extern char ___x_gnu_ioputc_o;
extern char ___x_gnu_ioseekoff_o;
extern char ___x_gnu_ioseekpos_o;
extern char ___x_gnu_iostream_o;
extern char ___x_gnu_ioungetc_o;
extern char ___x_gnu_iovfscanf_o;
extern char ___x_gnu_isgetline_o;
extern char ___x_gnu_isscan_o;
extern char ___x_gnu_outfloat_o;
extern char ___x_gnu_peekc_o;
extern char ___x_gnu_sbscan_o;
extern char ___x_gnu_stdstrbufs_o;
extern char ___x_gnu_stdstreams_o;
extern char ___x_gnu_streambuf_o;

char * __cplusIosObjs [] =
{
    &___x_gnu_cleanup_o,
    &___x_gnu_filebuf_o,
    &___x_gnu_filedoalloc_o,
    &___x_gnu_fileops_o,
    &___x_gnu_floatconv_o,
    &___x_gnu_genops_o,
    &___x_gnu_ioassign_o,
    &___x_gnu_iogetc_o,
    &___x_gnu_iogetline_o,
    &___x_gnu_iopadn_o,
    &___x_gnu_ioputc_o,
    &___x_gnu_ioseekoff_o,
    &___x_gnu_ioseekpos_o,
    &___x_gnu_iostream_o,
    &___x_gnu_ioungetc_o,
    &___x_gnu_iovfscanf_o,
    &___x_gnu_isgetline_o,
    &___x_gnu_isscan_o,
    &___x_gnu_outfloat_o,
    &___x_gnu_peekc_o,
    &___x_gnu_sbscan_o,
    &___x_gnu_stdstrbufs_o,
    &___x_gnu_stdstreams_o,
    &___x_gnu_streambuf_o,
    0
};
#endif

#ifdef INCLUDE_CPLUS_IOSTREAMS_FULL
extern char ___x_gnu_PlotFile_o;
extern char ___x_gnu_SFile_o;
extern char ___x_gnu_cleanup_o;
extern char ___x_gnu_editbuf_o;
extern char ___x_gnu_filebuf_o;
extern char ___x_gnu_filedoalloc_o;
extern char ___x_gnu_fileops_o;
extern char ___x_gnu_floatconv_o;
extern char ___x_gnu_fstream_o;
extern char ___x_gnu_genops_o;
extern char ___x_gnu_indstream_o;
extern char ___x_gnu_ioassign_o;
extern char ___x_gnu_ioextend_o;
extern char ___x_gnu_iofclose_o;
extern char ___x_gnu_iofeof_o;
extern char ___x_gnu_ioferror_o;
extern char ___x_gnu_iofgetpos_o;
extern char ___x_gnu_iofread_o;
extern char ___x_gnu_iofscanf_o;
extern char ___x_gnu_iofsetpos_o;
extern char ___x_gnu_iogetc_o;
extern char ___x_gnu_iogetdelim_o;
extern char ___x_gnu_iogetline_o;
extern char ___x_gnu_ioignore_o;
extern char ___x_gnu_iomanip_o;
extern char ___x_gnu_iopadn_o;
extern char ___x_gnu_ioprims_o;
extern char ___x_gnu_ioprintf_o;
extern char ___x_gnu_ioputc_o;
extern char ___x_gnu_ioseekoff_o;
extern char ___x_gnu_ioseekpos_o;
extern char ___x_gnu_iostream_o;
extern char ___x_gnu_iostrerror_o;
extern char ___x_gnu_ioungetc_o;
extern char ___x_gnu_iovfprintf_o;
extern char ___x_gnu_iovfscanf_o;
extern char ___x_gnu_isgetline_o;
extern char ___x_gnu_isgetsb_o;
extern char ___x_gnu_isscan_o;
extern char ___x_gnu_osform_o;
extern char ___x_gnu_outfloat_o;
extern char ___x_gnu_parsestream_o;
extern char ___x_gnu_peekc_o;
extern char ___x_gnu_sbform_o;
extern char ___x_gnu_sbgetline_o;
extern char ___x_gnu_sbscan_o;
extern char ___x_gnu_stdiostream_o;
extern char ___x_gnu_stdstrbufs_o;
extern char ___x_gnu_stdstreams_o;
extern char ___x_gnu_stream_o;
extern char ___x_gnu_streambuf_o;
extern char ___x_gnu_strops_o;
extern char ___x_gnu_strstream_o;

char * __cplusIosFullObjs [] =
{
    &___x_gnu_PlotFile_o,
    &___x_gnu_SFile_o,
    &___x_gnu_cleanup_o,
    &___x_gnu_editbuf_o,
    &___x_gnu_filebuf_o,
    &___x_gnu_filedoalloc_o,
    &___x_gnu_fileops_o,
    &___x_gnu_floatconv_o,
    &___x_gnu_fstream_o,
    &___x_gnu_genops_o,
    &___x_gnu_indstream_o,
    &___x_gnu_ioassign_o,
    &___x_gnu_ioextend_o,
    &___x_gnu_iofclose_o,
    &___x_gnu_iofeof_o,
    &___x_gnu_ioferror_o,
    &___x_gnu_iofgetpos_o,
    &___x_gnu_iofread_o,
    &___x_gnu_iofscanf_o,
    &___x_gnu_iofsetpos_o,
    &___x_gnu_iogetc_o,
    &___x_gnu_iogetdelim_o,
    &___x_gnu_iogetline_o,
    &___x_gnu_ioignore_o,
    &___x_gnu_iomanip_o,
    &___x_gnu_iopadn_o,
    &___x_gnu_ioprims_o,
    &___x_gnu_ioprintf_o,
    &___x_gnu_ioputc_o,
    &___x_gnu_ioseekoff_o,
    &___x_gnu_ioseekpos_o,
    &___x_gnu_iostream_o,
    &___x_gnu_iostrerror_o,
    &___x_gnu_ioungetc_o,
    &___x_gnu_iovfprintf_o,
    &___x_gnu_iovfscanf_o,
    &___x_gnu_isgetline_o,
    &___x_gnu_isgetsb_o,
    &___x_gnu_isscan_o,
    &___x_gnu_osform_o,
    &___x_gnu_outfloat_o,
    &___x_gnu_parsestream_o,
    &___x_gnu_peekc_o,
    &___x_gnu_sbform_o,
    &___x_gnu_sbgetline_o,
    &___x_gnu_sbscan_o,
    &___x_gnu_stdiostream_o,
    &___x_gnu_stdstrbufs_o,
    &___x_gnu_stdstreams_o,
    &___x_gnu_stream_o,
    &___x_gnu_streambuf_o,
    &___x_gnu_strops_o,
    &___x_gnu_strstream_o,
    0
};
#endif

#endif /* __INCcplusIosc */
