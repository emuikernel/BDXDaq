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









