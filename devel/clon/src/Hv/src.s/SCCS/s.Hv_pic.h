h20525
s 00000/00000/00000
d R 1.2 02/09/09 11:30:44 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 Hv/s/Hv_pic.h
e
s 00055/00000/00000
d D 1.1 02/09/09 11:30:43 boiarino 1 0
c date and time created 02/09/09 11:30:43 by boiarino
e
u
U
f e 0
t
T
I 1
/*
 *  much code borrowed  from John Bradley's  (bradley@cis.upenn.edu)
 *  Xv packages and the Independent JPEG Group's jpeg library.
 */


#ifndef __HVIMAGEH__
#define __HVIMAGEH__


#define PIC8   0
#define PIC24  1

#define F_GIF         0
#define F_JPEG        1


#define F_FULLCOLOR 0
#define F_GREYSCALE 1

#define RFT_ERROR    -1    /* couldn't open file, or whatever... */
#define RFT_UNKNOWN   0
#define RFT_GIF       1
#define RFT_JFIF      2

extern  void        Hv_ProcessPic(Hv_PicInfo *pinfo);

extern  void        Hv_DestroyPic(Hv_PicInfo *);

extern  void        Hv_PicFromFile(char *fname,
				   Hv_PicInfo **pinfo);

extern  void        Hv_ResizePic(Hv_PicInfo *pinfo,
				 int    neww,
				 int    newh);

extern  int         Hv_ScaleAndCropPic(Hv_PicInfo *pinfo,
				       int  x,
				       int  y,
				       int  cropw,
				       int  croph,
				       int  scalew,
				       int  scaleh);


#endif









E 1
