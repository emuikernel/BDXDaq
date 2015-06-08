
/**
 * <EM>This is the code, borrowed from public releases of 
 * the Xv super utility, for converting a jpeg into an image. </EM>
 * <P>
 * Much code was borrowed from John Bradley's  (bradley@cis.upenn.edu)
 * Xv packages and the Independent JPEG Group's jpeg library.
 * <P>
 * The Hv library was developed at The Thomas Jefferson National
 * Accelerator Facility under contract to the
 * Department of Energy and is the property of they U.S. Government.
 * Partial support came from the National Science Foundation.
 * <P>
 * It may be used to develop commercial software, but the Hv
 * source code and/or compiled modules/libraries may not be sold.
 * <P>
 * Questions or comments should be directed
 * to <a href="mailto:heddle@cebaf.gov">heddle@cebaf.gov</a> <HR>
 */


#include "Hv.h"
#include "Hv_pic.h"



/* includes from the jpeg library */

#ifndef Hv_NOJPEG

#include <setjmp.h>

#include "jpeg/jpeglib.h"
#include "jpeg/jerror.h"
/*
#include "jpeg/jpeglib.h"
#include "jpeg/jerror.h"
*/

struct my_error_mgr {
  struct jpeg_error_mgr pub;
  jmp_buf               setjmp_buffer;
};

typedef struct my_error_mgr *my_error_ptr;

static void      xv_error_exit(j_common_ptr);

static void      xv_error_output(j_common_ptr);
#endif  /* end has jpeg */

static void Hv_CreateImageFromPicInfo(Hv_PicInfo *pinfo);

static byte *Hv_ScaleCropPic(byte *pic,
			     int   normalw,
			     int   normalh,
			     int   cx,
			     int   cy,
			     int   cw,
			     int   ch,
			     int   sw,
			     int   sh);

static byte *Hv_ScalePic(byte *pic,
		      int   normalw,
		      int   normalh,
		      int   neww,
		      int   newh);

static int Hv_LoadJPEG(char *fname,
		       Hv_PicInfo *pinfo);



static int ReadFileType(char *fname);


static void xbzero(char *s,
		  size_t len);

static int ReadPicFile(char       *fname,
		       int         ftype,
		       Hv_PicInfo *pinfo);

static Hv_PicInfo *Hv_CreatePicInfo();

static byte         *pic;                    /* ptr to loaded picture */
static byte          rMap[256],gMap[256],bMap[256];  /* colormap */

static Hv_Image        *theImage;     /* X version of epic */

/**
 * Hv_CreateImageFromPicInfo
 * @purpose  Creates an image from a picinfo and puts it into
 * the pic info's image field.
 * @param  pinfo   The pic info
 * @local
 */

static void Hv_CreateImageFromPicInfo(Hv_PicInfo *pinfo) {

  int w, h, nw, nh;
  byte *temppic;

  if (pinfo == NULL)
    return;


/* compare "normal" and actual sizes to see if resize is necessary*/

  w  = pinfo->w;
  h  = pinfo->h;
  nw = pinfo->normw;
  nh = pinfo->normh;



  if ((w == nw) && (h == nh)) {
    temppic = pinfo->pic;
  }
  else {
    temppic = Hv_ScalePic(pinfo->pic, nw, nh, w, h);
  }

/* get the image */

  theImage = Hv_Pic8ToImage(temppic,
			    w, h, 
			    &(pinfo->mycolors),
			    rMap,
			    gMap,
			    bMap);

/* if temppic was merely allocated to scale the image,
   now we can free it */

  if ((temppic != pinfo->pic) && (temppic != NULL))
    free(temppic);

/* clear previous image */

  Hv_DestroyImage(pinfo->image);

  pinfo->image = theImage;
}

/**
 * Hv_PicFromFile
 * @purpose  Load a file into an image. At present, only
 * jpeg is supported (and that experimentally)
 * @param    fname   full file name
 * @param    pinfo   The Hv_PicInfo will be malloced and returned
 */

void Hv_PicFromFile(char     *fname,
		    Hv_PicInfo **pinfo) {
  
  int       ftype;
  int       status;
  byte     *temp;
  int       neww, newh;
  long      lw, lh;

  if (fname == NULL)
    return;
  
  ftype = ReadFileType(fname);
  
  if (ftype != RFT_JFIF) {
    Hv_Println("Couldn't load %s of type %d. Currently only handles JPEGs.",
	       fname,  ftype);
    return;
  }

/* allocate the picinfo */   

  *pinfo = Hv_CreatePicInfo();
  
/* read the picfile */
  
  status = ReadPicFile(fname, ftype, *pinfo);
 
  
/* shrink pics that are too big (Win32 problem) */

  if ((*pinfo)->normw > 2000) {

Hv_Println("Shrink pic [%s] ", fname);
Hv_Println("normw: %d", (*pinfo)->normw);

	  neww = 1600;
	  lw = (long)((*pinfo)->normw);
	  lh = (long)((*pinfo)->normh);

	  newh = (int)((((long)neww)*lh)/lw);
	  temp = Hv_ScalePic((*pinfo)->pic,
		  (*pinfo)->normw,
		  (*pinfo)->normh,
		  neww,
		  newh);

	  free((*pinfo)->pic);
	  (*pinfo)->pic = temp;
	  (*pinfo)->w = neww;
	  (*pinfo)->h = newh;
	  (*pinfo)->normw = neww;
	  (*pinfo)->normh = newh;
  }


  if (status == 0) {
    Hv_Println("File %s was not read.", fname);
    Hv_DestroyPic(*pinfo);
    *pinfo = NULL;
    return;
  }
  else
    Hv_Println("File %s was read.", fname);

/* process the image */
  
  Hv_ProcessPic(*pinfo);
  
  if ((*pinfo)->image == NULL) 
    Hv_Println("Image was NOT created.");
  else
    Hv_Println("Image was created.");
}


/**
 * Hv_DestroyPic
 * @purpose Destroys an Hv_PicInfo
 * @param pinfo   The pic info to destroy
 * @local
 */

void Hv_DestroyPic(Hv_PicInfo *pinfo) {
    if (pinfo == NULL)
	return;

    if (pinfo->pic != NULL)
	free(pinfo->pic);

    if (pinfo->mycolors != NULL)
	free(pinfo->mycolors);
    Hv_DestroyImage(pinfo->image);

    free(pinfo);
}


/**
 * Hv_CreatePicInfo
 * @purpose  Allocate an "empty" Hv_PicInfo record.
 *@local  
 */


static Hv_PicInfo *Hv_CreatePicInfo() {
  Hv_PicInfo *pinfo;
  
  pinfo = (Hv_PicInfo *)malloc(sizeof(Hv_PicInfo));
  xbzero((char *) pinfo, sizeof(Hv_PicInfo));
  
/* init important fields of pinfo */

  pinfo->pic = (byte *) NULL;
  pinfo->mycolors = NULL;
  pinfo->image = (Hv_Image *)NULL;
  pinfo->x = 0;
  pinfo->y = 0;
  pinfo->cw = -1;
  pinfo->ch = -1;
  pinfo->sw = -1;
  pinfo->sh = -1;
  return pinfo;
}



static void xbzero(char *s,
		  size_t len) {
  for ( ; len>0; len--)
    *s++ = 0;
}


/********************************/
static int ReadPicFile(char       *fname,
		       int         ftype,
		       Hv_PicInfo *pinfo) {


  int rv = 0;

  /* by default, most formats aren't multi-page */
  
  switch (ftype) {
  case RFT_JFIF:
    rv = Hv_LoadJPEG(fname, pinfo);
    break;
    
  }
  return rv;
}


/********************************/
static int ReadFileType(char *fname) {

  /* opens fname (which *better* be an actual file by this point!) and
     reads the first couple o' bytes.  Figures out what the file's likely
     to be, and returns the appropriate RFT_*** code */


  FILE *fp;
  byte  magicno[30];    /* first 30 bytes of file */
  int   rv, n;
  
  if (!fname) {
    Hv_Println("Null file name.");
    return RFT_ERROR;   /* shouldn't happen */
  }
  
  fp = fopen(fname, "rb");
  if (!fp) {
    Hv_Println("File %s was not opened.", fname);
    return RFT_ERROR;
  }

  n = fread(magicno, (size_t) 1, (size_t) 30, fp);  
  fclose(fp);

  if (n < 30)
    return RFT_UNKNOWN;    /* files less than 30 bytes long... */

  rv = RFT_UNKNOWN;

  if (magicno[0]==0xff && magicno[1]==0xd8 && 
      magicno[2]==0xff) {
    rv = RFT_JFIF;
    Hv_Println("File %s is a JPEG file.", fname);
  }

  
  if (rv == RFT_UNKNOWN)
    Hv_Println("File %s is of unknown type.", fname);
  return rv;
}

/**
 * Hv_ResizePic
 * @purpose Scale a picture and generate a new image. This does
 * NOT affect the raw picture data in the pic field.
 * @param pinfo  The pic to resize
 * @param neww   The new width
 * @param newh   The new height
 */

void  Hv_ResizePic(Hv_PicInfo *pinfo,
		   int         neww,
		   int         newh) {

  if (pinfo == NULL)
    return;

/* perhaps no need */

  if ((pinfo->w == neww) && (pinfo->h == newh) && (pinfo->image != NULL)) { 
    Hv_Println("No need to resize.");
    return;
  }

  Hv_Println("resizing");

  pinfo->w = neww;
  pinfo->h = newh;

  Hv_CreateImageFromPicInfo(pinfo);
}


/**
 * Hv_ScaleAndCropPic
 * @purpose Scale Crop a picture and generate a new image. This does
 * NOT affect the raw picture data in the pic field.
 * @param pinfo   The pic to crop
 * @param x       The left of the crop rectangle
 * @param y       The top of the crop rectangle
 * @param cropw   The width of the crop rectangle
 * @param croph   The height of the crop rectangle
 * @param scalew  The overall (scaled) width of the image
 * @param scaleh  The overall (scaled) height of the image
 * @return  0 if rescaled/cropped, 1 if did nothing, -1 on error
 */

int Hv_ScaleAndCropPic(Hv_PicInfo *pinfo,
		       int  x,
		       int  y,
		       int  cropw,
		       int  croph,
		       int  scalew,
		       int  scaleh) {


  byte     *temppic = NULL;
  Hv_Image *image;

/* look for pathologies */

  if ((x < 0) || 
      (y < 0) || 
      (cropw <= 0) || 
      (croph <= 0) || 
      (scalew <= 0) || 
      (scaleh <= 0) || 
      (pinfo == NULL)) {
    Hv_Println("Bad data in Hv_CropPic");
    return -1;
  }

/* may not need to change anything */

  if ((x == pinfo->x) &&
      (y == pinfo->y) &&
      (cropw == pinfo->cw) &&
      (croph == pinfo->ch) &&
      (scalew == pinfo->sw) &&
      (scaleh == pinfo->sh) &&
      (pinfo->image != NULL)) {

    Hv_Println("No need to do anything"); 
    return 1;
  }

  pinfo->sw = scalew;
  pinfo->sh = scaleh;
  pinfo->cw = cropw;
  pinfo->ch = croph;
  pinfo->x = x;
  pinfo->y = y;

/* the new pic will be the size of the crop rect */

  pinfo->w = cropw;
  pinfo->h = croph;

  temppic = Hv_ScaleCropPic(pinfo->pic,
			    pinfo->normw,
			    pinfo->normh,
			    x,
			    y,
			    cropw, 
			    croph,
			    scalew,
			    scaleh);

/* get the image */

  image = Hv_Pic8ToImage(temppic,
			 cropw,
			 croph, 
			 &(pinfo->mycolors),
			 rMap,
			 gMap,
			 bMap);


  free(temppic);

/* clear previous image */

  Hv_DestroyImage(pinfo->image);

  pinfo->image = image;
  return 0;
}


/**
 * Hv_ScaleCropPic
 * @purpose  Scale and crop the pic simultaneously
 * @param pic       The raw pic data
 * @param normalw   The normal width
 * @param normalh   The normal height
 * @param cx      The left of the crop rect
 * @param cy      The top of the crop rect
 * @param cw      The width of the crop rect
 * @param ch      The height of the crop rect
 * @param sw      The width of the scale rect
 * @param sh      The height of the scale rect
 * @local
 */

  
static byte *Hv_ScaleCropPic(byte *pic,
			     int   normalw,
			     int   normalh,
			     int   cx,
			     int   cy,
			     int   cw,
			     int   ch,
			     int   sw,
			     int   sh) {

  byte        *newpic = NULL;
  int          ty, i, j, *xoff;
  byte        *rawptr = NULL;
  byte        *newptr = NULL;
  size_t       size;

  size = (size_t) (cw*ch);
  newpic = (byte *) malloc(size);
  xbzero((char *)newpic, size);

/* the x array of offsets, which will work for 
   each row */
   
  xoff = (int *) malloc(cw * sizeof(int));

  for (i = 0; i < cw; i++) 
    xoff[i] = ((normalw * (i+cx)) / sw);

  for (j = 0;  j < ch;  j++) {

/* get the correct row in the raw pic */

    ty = ((normalh * (j+cy)) / sh);
    rawptr = pic + (ty * normalw);

/* get the correct row in the new pic */

    newptr = newpic + (j * cw);

    for (i = 0; i < cw; i++) {
      newptr[i] = rawptr[xoff[i]];
    }
  }
  free(xoff);

  return newpic;
}


/**
 * Hv_ScalePic
 * @purpose  Scale the pic
 * @param pic       The raw pic data
 * @param normalw   The normal width
 * @param normalh   The normal height
 * @param neww      The new width
 * @param newh      The new height
 * @local
 */

  
static byte *Hv_ScalePic(byte *pic,
			 int   normalw,
			 int   normalh,
			 int   neww,
			 int   newh) {

  byte *newpic;
  int          cy,ex,ey,*cxarr, *cxarrp;
  byte        *clptr,*elptr,*epptr;
  size_t       size;

  clptr = NULL; 
  cxarrp = NULL;
  cy = 0;  /* shut up compiler */


  size = (size_t) (neww*newh);
  newpic = (byte *) malloc(size);
  xbzero((char *)newpic, size);

/* the x array of offsets */

  cxarr = (int *) malloc(neww * sizeof(int));

  for (ex=0; ex < neww; ex++) 
    cxarr[ex] = (normalw * ex) / neww;

  elptr = epptr = newpic;

  for (ey=0;  ey < newh;  ey++, elptr+= neww) {
    
    cy = (normalh * ey) / newh;
    epptr = elptr;
    clptr = pic + (cy * normalw);
    
    for (ex=0, cxarrp = cxarr;  ex < neww;  ex++, epptr++) 
      *epptr = clptr[*cxarrp++];
  }
  free(cxarr);

  return newpic;
}

/**
 * Hv_ProcessPic
 * @purpose Process the raw pic data stored in the pic field
 * into an image (stored in the image field)
 * @param pinfo   The Hv_PicInfo record to process
 */


  void Hv_ProcessPic(Hv_PicInfo *pinfo) {
    int i;


    pic   = pinfo->pic;

    for (i=0; i<256; i++) {
	rMap[i] = pinfo->r[i];
	gMap[i] = pinfo->g[i];
	bMap[i] = pinfo->b[i];
    }
  
    Hv_CreateImageFromPicInfo(pinfo);
}


/**
 * Hv_LoadJPEG
 * @purpose  Load a jpeg file
 * @param  fname    The file to load
 * @param  pinfo    The pic info rec to put it in
 * @return '1' on success, '0' on failure 
 * @local
 */


static int Hv_LoadJPEG(char *fname,
		       Hv_PicInfo *pinfo) {

#ifndef Hv_NOJPEG
  struct jpeg_decompress_struct    cinfo;
  struct my_error_mgr              jerr;
  JSAMPROW                         rowptr[1];
  FILE                            *fp;
  static byte                     *pic;
  long                             filesize;
  int                              i,w,h,bperpix;


  Hv_Println("Trying to load a JPEG file: %s", fname);

  pic       = (byte *) NULL;

  pinfo->type  = PIC8;

  if ((fp = fopen(fname, "rb")) == NULL)
    return 0;

  fseek(fp, 0L, 2);
  filesize = ftell(fp);
  fseek(fp, 0L, 0);


  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit     = xv_error_exit;
  jerr.pub.output_message = xv_error_output;

  if (setjmp(jerr.setjmp_buffer)) {
    jpeg_destroy_decompress(&cinfo);
    fclose(fp);
    if (pic)     free(pic);

    pinfo->pic = (byte *) NULL;

    return 0;
  }

  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, fp);
  (void) jpeg_read_header(&cinfo, TRUE);

  /* do various cleverness regarding decompression parameters & such... */


  jpeg_calc_output_dimensions(&cinfo);
  w = cinfo.output_width;
  h = cinfo.output_height;
  pinfo->normw = w;  pinfo->normh = h;


  if (cinfo.jpeg_color_space == JCS_GRAYSCALE) {
    cinfo.out_color_space = JCS_GRAYSCALE;
    cinfo.quantize_colors = FALSE;
    
    
    for (i=0; i<256; i++) pinfo->r[i] = pinfo->g[i] = pinfo->b[i] = i;
  }
  else {
    cinfo.out_color_space = JCS_RGB;
    cinfo.quantize_colors = TRUE;     /* default: give 24-bit image to XV */
  }
  
  jpeg_calc_output_dimensions(&cinfo);   /* note colorspace changes... */
    

  if (cinfo.output_components != 1 && cinfo.output_components != 3) {

    jpeg_destroy_decompress(&cinfo);
    fclose(fp);
    return 0;
  }


  bperpix = cinfo.output_components;
  pinfo->type = (bperpix == 1) ? PIC8 : PIC24;


  pic = (byte *) malloc((size_t) (w * h * bperpix));
  if (!pic) {

    jpeg_destroy_decompress(&cinfo);
    fclose(fp);
    return 0;
  }
  
  jpeg_start_decompress(&cinfo);

  while (cinfo.output_scanline < cinfo.output_height) {
    rowptr[0] = (JSAMPROW) &pic[cinfo.output_scanline * w * bperpix];
    (void) jpeg_read_scanlines(&cinfo, rowptr, (JDIMENSION) 1);
  }

/* return 'Hv_PicInfo' structure */

  pinfo->pic = pic;
  pinfo->w = w;
  pinfo->h = h;

  if (pinfo->type == PIC8) {
    Hv_Println("Pic type: PIC8");
  }
  else
    Hv_Println("Pic type: PIC24");



  if (cinfo.out_color_space == JCS_GRAYSCALE) {

    for (i=0; i<256; i++) pinfo->r[i] = pinfo->g[i] = pinfo->b[i] = i;
  }
  else {

    if (cinfo.quantize_colors) {
      for (i=0; i<cinfo.actual_number_of_colors; i++) {
	pinfo->r[i] = cinfo.colormap[0][i];
	pinfo->g[i] = cinfo.colormap[1][i];
	pinfo->b[i] = cinfo.colormap[2][i];
      }
    }
  }
  
  
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  fclose(fp);

  return 1;
#else
  return 0;
#endif /* has jpeg */
}

#ifndef Hv_NOJPEG  
static void xv_error_exit(j_common_ptr cinfo) { 
  my_error_ptr myerr;

  myerr = (my_error_ptr) cinfo->err;
  (*cinfo->err->output_message)(cinfo);
  longjmp(myerr->setjmp_buffer, 1);
}

static void xv_error_output(j_common_ptr cinfo) {
  my_error_ptr myerr;
  char         buffer[JMSG_LENGTH_MAX];

  myerr = (my_error_ptr) cinfo->err;
  (*cinfo->err->format_message)(cinfo, buffer);

}
#endif   /* end has jpeg */



