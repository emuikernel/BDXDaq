h04622
s 00000/00000/00712
d D 1.3 07/11/12 16:41:06 heddle 4 3
c new start counter
e
s 00001/00001/00711
d D 1.2 03/04/17 16:42:30 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 02/09/09 16:27:10 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 ced/field.c
e
s 00712/00000/00000
d D 1.1 02/09/09 16:27:09 boiarino 1 0
c date and time created 02/09/09 16:27:09 by boiarino
e
u
U
f e 0
t
T
I 1
/*
----------------------------------------------------
-							
-  File:    field.c	
-							
-  Summary:						
-           controls fieldmap drawing
-					       
-                     				
-  Revision history:				
-                     				
-  Date       Programmer     Comments		
----------------------------------------------------
-  10/19/94	DPH		Initial Version	
-                                              	
------------------------------------------------------
*/

#include "ced.h"

#define MAXPNTSINREQ   10000   /*max number of points allowed to accumulate into a request */

/*-------- local prototypes --------*/

static void DrawFieldmapItem(Hv_Item   Item,
			     Hv_Region  region);

static int ReadBGridHeader(FILE      *fd,
			   FieldMap    *fm,
			   unsigned long fsize);

static int ReadBGrid(char   *fname,
		     FieldMap *fm);

static unsigned long FileSize(char   *fname);

static Boolean  snake(fvect     R,
		      FieldMap    *fm,
		      fvect     B);

static void DrawFieldmap(Hv_Item    Item ,
			 Hv_Region   region);


#define SIN60  0.86602540

/* -------- BField ------------- */

Boolean BField(float   x,
	       float   y,
	       float   z,
	       fvect   B,
	       float  *Bmag)

/* returns the magnitude of the field and its
   components in B. Point is rotated to sector 1
   and the the field is unrotated back.*/

{
  Boolean   inbounds;
  fvect     R;
  static    float    xf[6], yf[6];
  static    Boolean  veryfirst = True;
  float     xo, yo, bxo, byo;
  int       sector;

/* if first time, set up the rotation factors */

  if (veryfirst) {
    xf[0] =  1.0;    yf[0] = 0.0;
    xf[1] =  0.5;    yf[1] = SIN60;
    xf[2] = -0.5;    yf[2] = SIN60;
    xf[3] = -1.0;    yf[3] = 0.0;
    xf[4] = -0.5;    yf[4] = -SIN60;
    xf[5] =  0.5;    yf[5] = -SIN60;
    veryfirst = False;
  }

/* rotate to sector 1 */

  sector = XYtoSector(x, y);
  if (sector > 0) {
    xo = x;
    yo = y;
    x =  xf[sector]*xo + yf[sector]*yo;
    y = -yf[sector]*xo + xf[sector]*yo;
  }

  R[Xcomp] = x;
  R[Ycomp] = y;
  R[Zcomp] = z;
  inbounds = snake(R, &fmap, B);

  if (inbounds) {

/* unrotate  if sect != 0 */

    if (sector > 0) {
      bxo = B[Xcomp];
      byo = B[Ycomp];
      B[Xcomp] = xf[sector]*bxo - yf[sector]*byo;
      B[Ycomp] = yf[sector]*bxo + xf[sector]*byo;
    }

/* scale field */

    if ((fabs(fmap.scalefact) - 1.0) > 1.0e-4) {
      B[Xcomp] *= fmap.scalefact;
      B[Ycomp] *= fmap.scalefact;
      B[Zcomp] *= fmap.scalefact;
    }
    *Bmag = fvectmag(B);
  }
  else
    *Bmag = 0.0;
  
  return  inbounds;
} 

#undef SIN60

/*--------- InitField ----------*/

void InitField(void)

/* once only initialization of FMap structure.
   (no field is read in, the ptr to the actual
   data is set to NULL */


{
  fmap.fmapptr = NULL; 
}


/*----------- ReadField -----------*/

void ReadField(void)

{
  char       *fmapfname = NULL;
  ViewData   viewdata;
  Hv_View    temp;

  fmapfname = Hv_FileSelect("Select a Fieldmap File", "*.fmap", " ");

  if (fmapfname == NULL)
    return;

/* fmap is the global CLAS fieldmap variable.
   Free it if it presently holds data */

  Hv_Free(fmap.fmapptr);
  fmap.fmapptr = NULL;

/* now try to load it */

  ReadBGrid(fmapfname, &fmap);
  Hv_Free(fmapfname);

/* redraw all views displaying a fieldmap */
  
  for (temp = Hv_views.first;  temp != NULL;  temp = temp->next) {
    viewdata = GetViewData(temp);
    if (viewdata->displayField)
      Hv_DrawViewHotRect(temp);
  }

}

/*----------- MallocFieldmapItem -----------*/

Hv_Item  MallocFieldmapItem(Hv_View View)

{
  ViewData      viewdata;
  Hv_Item       Item;

  viewdata = GetViewData(View);

/* only one field map item per view */

  if (viewdata->FieldmapItem != NULL)
    return NULL;

  Item = Hv_VaCreateItem(View,
			 Hv_TAG,         FIELDMAP,
			 Hv_USERDRAW,    DrawFieldmapItem,
			 Hv_DRAWCONTROL, Hv_ALWAYSREDRAW + Hv_INBACKGROUND,
			 NULL);

  viewdata->FieldmapItem = Item;
  Hv_SetViewBrickWallItem(Item);

  return Item;
}


/*---------- DrawFieldmapItem --------------*/

static void DrawFieldmapItem(Hv_Item   Item,
			     Hv_Region  region)
{
  Hv_View        View = Item->view;
  ViewData       viewdata;

/* return if no field map to draw */

  if (fmap.fmapptr == NULL)
    return;

  viewdata = GetViewData(View);

/* also return if display is off */

  if (!(viewdata->displayField))
    return;

  StartWait();
  fprintf(stderr, "drawing fieldmap\n");
  DrawFieldmap(Item, region);
  StopWait();
}


/*-------- ReadBGridHeader -------*/

static int ReadBGridHeader(FILE      *fd,
			   FieldMap    *fm,
			   unsigned long fsize)

/* reads the header infor on the magnetic field file and does
   some preliminary calculations

 *fd     input stream
 *fm     pointer to fieldmap structure
  fsize  file size size used to check FORTRAN vs C format
*/


{
  long            firstword;              /*first word in file -- used to see if its a fortran file*/
  int             nw;                     /* number of four byte words in file (exc) */
  Vcomp           comp;                   /* xyz component looper*/


/* check to see if its a FORTRAN file. The FORTRAN file
   will have an extra word at top (&bottom) equal to the size
   of the file - 8 . If it is not FORTRAN, then that first
   word is actually data (nw) */

  fread(&firstword, sizeof(long), 1, fd);

  if (fsize == (firstword + 8)) {
    printf("Field file has FORTRAN format.\n");
    fread(&nw, sizeof(long), 1, fd);
  }
  else {
    printf("Field file has C format.\n");
    nw = firstword;
  }
  
/* the structure of Bogdan/s fieldmap is very simple. */

  fread(fm->Nxyz, sizeof(ivect), 1, fd);
  fread(fm->Rmin, sizeof(fvect), 1, fd);
  fread(fm->dR,   sizeof(fvect), 1, fd);

  fprintf(stdout, "[Nx, Ny, Nz] = [%d, %d, %d]\n",
	  (int)(fm->Nxyz)[0],
	  (int)(fm->Nxyz)[1],
	  (int)(fm->Nxyz)[2]);
  fprintf(stdout, "[RminX, RminY, RminZ] = [%f, %f, %f]\n", (fm->Rmin)[0], (fm->Rmin)[1], (fm->Rmin)[2]);
  fprintf(stdout, "[dX, dY, dZ] = [%f, %f, %f]\n", (fm->dR)[0], (fm->dR)[1], (fm->dR)[2]);

/*  scalevect(fm->Rmin, 0.01); */                    /* to convert cm to meters */
/*  scalevect(fm->dR, 0.01);   */                      /* to convert cm to meters */

  fread(&(fm->ngr), sizeof(long), 1, fd);  /* ngr should equal nx*ny*nz*/

  if (fm->ngr != (3 * (fm->Nxyz[Xcomp]) * (fm->Nxyz[Ycomp]) * (fm->Nxyz[Zcomp]))) {
    printf("could not understand fieldmap file\n");
    return(1);
  }

/* some derived quantities */

  fm->scalefact = 1.0;
  fm->NxNy = (fm->Nxyz[Xcomp]) * (fm->Nxyz[Ycomp]);

  for (comp = Xcomp; comp <= Zcomp; comp++) {
    fm->Rmax[comp] = fm->Rmin[comp] + (fm->Nxyz[comp] - 1)*fm->dR[comp];
    fm->RdR[comp] = 1.0/(fm->dR[comp]);
  }

  fprintf(stdout, "[RmaxX, RmaxY, RmaxZ] = [%f, %f, %f]\n", 100.0*((fm->Rmax)[0]),
	  100.0*((fm->Rmax)[1]), 100.0*((fm->Rmax)[2]));
  
  return(0);
}

/*-------- ReadBGrid--------*/

static int ReadBGrid(char   *fname,
		     FieldMap *fm)

/*
  *fname     --> name of unformatted fieldmap file
  *fm       <-- field map struct that gets loaded
*/

/* Read the 3D field map in Bogdan's format */

{
  int             numread;      /* returns # bytes read */ 
  int             fieldsize;    /* 4*ngr -> storage requirement of field */

  unsigned long   fsize;        /*size of file*/

  float           *compptr;     /*generic pointer to a B-field component (float)*/
  float           *eod;         /*pointer to end of data*/
  FILE            *magfile;     /*file containing magnetic field*/

/* NOTE: Bogdan uses the "standard CLAS" c.s.
   for his field map -- not the SDA c.s. */

  printf("Attempting to read SDA field map\n");
  magfile = fopen(fname, "r");

/* open was not successful, return error */

  if (magfile == NULL)
    return(1);

/* read the header */

  fsize = FileSize(fname);
  ReadBGridHeader(magfile, fm, fsize);


/* ask for the memory for the field map array.
   Note there are ngr B-field components -> ngr*4 bytes */

  fieldsize = fm->ngr*4;
  printf("storage requirement for field: %i bytes\n", fieldsize);
  fm->fmapptr = (fvect *)Hv_Malloc(fieldsize);
  
/* read the field map array */

  numread = fread(fm->fmapptr, sizeof(float), fm->ngr, magfile);
  numread *= sizeof(float);
  printf("number of bytes read: %i [bytes]\n", numread); 

  fclose(magfile);             /* close the fieldmap file */

  if (fieldsize != numread) {
    printf("error in reading magnetic field\n");
    Hv_Free(fm->fmapptr);
    fm->fmapptr = NULL;
    return(1);
  }

/* scale the field to convert to tesla from kG. Note that the
   number of magnetic field components is 3*ngr*/

  printf("Converting field from kG to Tesla ...");

  eod = ((float *)(fm->fmapptr)) + fm->ngr;

  for (compptr = (float *)(fm->fmapptr); compptr < eod; compptr++)
      *compptr *= 0.1;

  printf(" done\n");
  return 0;
}



/* ------- FileSize --------- */

static unsigned long  FileSize(char *fname)

/* returns size of named file in bytes */

{
  struct stat  buf;

  stat(fname, &buf);
  return((unsigned long)buf.st_size);
}


/* ------ snake ------- */

static Boolean  snake(fvect       R,
                      FieldMap   *fm,
                      fvect       B)

/* 
       R     --> where field is to be evaluated --
      *fm    --> pointer to field map struct
       B     <-- interpolated value of field at R
*/

/* returns True if in bounds, False if out of bounds */

/* Interpolates the 3D field. Note:
   the field map is organized as if written by this piece
   of PSEUDO-FORTRAN code:

                 DO I = 1, Nz
		   DO J = 1, Ny
		     DO K = 1, Nx
                       WRITE  Bx(Xk, Yj, Zi)
                       WRITE  By(Xk, Yj, Zi)
                       WRITE  Bz(Xk, Yj, Zi)
		     END DO
		   END DO
		 END DO
*/

{

  float    Bfact = 1.0;
  Boolean  OutOfBounds = False;
  ivect    indvect;
  fvect    findvect;
  Vcomp    comp;

  float    F000,F00P,F00M,F0P0,F0M0,FP00,FM00,F0PP,FP0P,FPP0,FPPP;
  int      I000,I00P,I00M,I0P0,I0M0,IP00,IM00,I0PP,IP0P,IPP0,IPPP;
  float    CP,CQ,CR,DP,DQ,DR,E,PP,QQ,RR;

/* use the symmetry of the field, the fact that a) only sector
   one is stored and b) in fact only half of sector 1 with
   y > 0 is stored */


  if (R[Ycomp] < 0.0) {
    Bfact = -1.0;
    R[Ycomp] = -R[Ycomp];
  }

  for (comp = Xcomp; comp <= Zcomp; comp++) {
    findvect[comp] = (R[comp] - (fm->Rmin[comp]))*(fm->RdR[comp]);
    indvect[comp] = (long)findvect[comp];
    OutOfBounds = ((OutOfBounds) || ((indvect[comp] < 1) || ((indvect[comp]+2) > fm->Nxyz[comp])));
  }

/* offest to get proper place in array */

  if (!OutOfBounds) {
    
    PP = findvect[Xcomp] - indvect[Xcomp];
    QQ = findvect[Ycomp] - indvect[Ycomp];
    RR = findvect[Zcomp] - indvect[Zcomp];

/*  3 dim. 2nd order interpolation inside the TORUS for each
    component of the field. Use 11 values of the field:    */

    I000 = (indvect[Xcomp]  ) + (fm->Nxyz[Xcomp])*(indvect[Ycomp]  ) + fm->NxNy*(indvect[Zcomp]  );  
    I00P = (indvect[Xcomp]  ) + (fm->Nxyz[Xcomp])*(indvect[Ycomp]  ) + fm->NxNy*(indvect[Zcomp]+1);  
    I00M = (indvect[Xcomp]  ) + (fm->Nxyz[Xcomp])*(indvect[Ycomp]  ) + fm->NxNy*(indvect[Zcomp]-1);  
    I0P0 = (indvect[Xcomp]  ) + (fm->Nxyz[Xcomp])*(indvect[Ycomp]+1) + fm->NxNy*(indvect[Zcomp]  );  
    I0M0 = (indvect[Xcomp]  ) + (fm->Nxyz[Xcomp])*(indvect[Ycomp]-1) + fm->NxNy*(indvect[Zcomp]  );  
    IP00 = (indvect[Xcomp]+1) + (fm->Nxyz[Xcomp])*(indvect[Ycomp]  ) + fm->NxNy*(indvect[Zcomp]  );  
    IM00 = (indvect[Xcomp]-1) + (fm->Nxyz[Xcomp])*(indvect[Ycomp]  ) + fm->NxNy*(indvect[Zcomp]  );  
    I0PP = (indvect[Xcomp]  ) + (fm->Nxyz[Xcomp])*(indvect[Ycomp]+1) + fm->NxNy*(indvect[Zcomp]+1);  
    IP0P = (indvect[Xcomp]+1) + (fm->Nxyz[Xcomp])*(indvect[Ycomp]  ) + fm->NxNy*(indvect[Zcomp]+1);  
    IPP0 = (indvect[Xcomp]+1) + (fm->Nxyz[Xcomp])*(indvect[Ycomp]+1) + fm->NxNy*(indvect[Zcomp]  );  
    IPPP = (indvect[Xcomp]+1) + (fm->Nxyz[Xcomp])*(indvect[Ycomp]+1) + fm->NxNy*(indvect[Zcomp]+1);  

    for (comp = Xcomp; comp <= Zcomp; comp++) {

      F000 = (*(fm->fmapptr + I000))[comp];  
      F00P = (*(fm->fmapptr + I00P))[comp];  
      F00M = (*(fm->fmapptr + I00M))[comp];  
      F0P0 = (*(fm->fmapptr + I0P0))[comp];  
      F0M0 = (*(fm->fmapptr + I0M0))[comp];  
      FP00 = (*(fm->fmapptr + IP00))[comp];  
      FM00 = (*(fm->fmapptr + IM00))[comp];  
      F0PP = (*(fm->fmapptr + I0PP))[comp];  
      FP0P = (*(fm->fmapptr + IP0P))[comp];  
      FPP0 = (*(fm->fmapptr + IPP0))[comp];  
      FPPP = (*(fm->fmapptr + IPPP))[comp];  
      
      CP = FP00+FM00-2.*F000;
      CQ = F0P0+F0M0-2.*F000;
      CR = F00P+F00M-2.*F000;
      DP = F000-FP00+FPP0-F0P0;
      DQ = F000-F0P0+F0PP-F00P;
      DR = F000-F00P+FP0P-FP00;
      E  = -DP-F0PP+F00P-FP0P+FPPP;
	  
/*  Compute the Taylor's series:   */
	  
      B[comp]= F000+0.5*(PP*(FP00-FM00)+PP*PP*CP
			 +QQ*(F0P0-F0M0)+QQ*QQ*CQ
			 +RR*(F00P-F00M)+RR*RR*CR)
	+PP*QQ*DP+QQ*RR*DQ+PP*RR*DR+PP*QQ*RR*E;
      
    }
  }
    else {  /*OutOfBounds*/
      for (comp = Xcomp; comp <= Zcomp; comp++)
	B[comp] = 0.0;
      return False;
    }
  
  if (Bfact < 0.0) {      /* switch back Ry, switch signs of Bx & Bz */
    R[Ycomp] = -R[Ycomp];
    B[Xcomp] = -B[Xcomp];
    B[Zcomp] = -B[Zcomp];
  }
  return True;
}




/* --------- DrawFieldmap ----------- */

static void  DrawFieldmap(Hv_Item   Item,
			  Hv_Region region)
   
{

  Hv_View                View = Item->view;
  int		         i,j;			  /* Loopers */
  short		         zt, xt;		  /* temporary pixel coordinates */
  float		         fzt, fxt;		  /* temporary world coordinates */
  float		         dz, dx;		  /* in CLAS sys, horiz & vert WORLD coord sampling spacing*/
  int	  	         zstep, xstep;		  /* steps */	
  float		         field;
  int                    cindex;                  /*color index*/
  float		         scale;
  Hv_FRect               frect;                   /*temporary world system*/
  fvect                  Bvect;

  Hv_Point               *cPtrs[Hv_COLORS];    /*color point for each color (for each color)*/
  short                  occurs[Hv_COLORS];    /*how many times each color occurs*/
  Hv_Rect	         rect;	                  /*bounding rect of current clip region */

  XRectangle             *rectangles;

  short                  dpx, dpy;
  float                  x, y, z, phi;            /*true CLAS coordinates*/
  short                  mincolor, maxcolor;      /*range of colors in the rainbow*/
  Boolean                inbounds;
  ViewData               viewdata;
  Hv_RainbowData        *data;
  float                  vmax, vmin;
  Boolean                backwards;

  viewdata = GetViewData(View);
  data = (Hv_RainbowData *) (viewdata->FieldRainbowItem->data);

  mincolor = data->mincolor;
  maxcolor = data->maxcolor;

  if (mincolor == maxcolor)
    return;

/* note the possibility (likelyhood) that the color
   we want for the MAX field corresponds to the smallest
   color index -- i.e. Hv_red */

  
  backwards = (mincolor > maxcolor);

/* find the smallest rect that needs to be redrawn */

  if (Hv_inPostscriptMode) {
      Hv_CopyRect(&rect, View->hotrect);
  }
  else {
      if (region != NULL)
	  Hv_ClipBox(region, &rect);
  }

  Hv_ShrinkRect(&rect, -1, -1);
  Hv_LocalRectToWorldRect(View, &rect, &frect);

/* get pixels steps from the raimbow data */

  dpx = data->dx;
  dpy = data->dy;
  vmax = (data->scalefactor)*(data->valmax);
  vmin = (data->scalefactor)*(data->valmin);

/* be wary of the CLAS system: x vertical and z: horizontal */

  dz = (frect.width*dpx)/rect.width;
  dx = (frect.height*dpy)/rect.height;

  if ((dz <= 0.0) || (dx <= 0.0)) {
    fprintf(stderr, "illegal step size [%f, %f] in DrawFieldmap\n", dz, dx);
    return;
  }

/*increment dpx & dpy so the color fills work properly */

  dpx++;
  dpy++;

/* we will loop over screen world coordinates z & x, converting
   to CLAS sector 1 coordinates to get the field */
  
  XFlush(Hv_display);                                   /*so layout is drawn and cursor*/
 
/* now get the number of steps */
      
  zstep = 1 + (frect.xmax - frect.xmin)/dz;
  xstep = 1 + (frect.ymax - frect.ymin)/dx;

/* using the backwards boolean, scale is always positive */

  if (backwards)
    scale = (mincolor - maxcolor)/(vmax - vmin);
  else
    scale = (maxcolor - mincolor)/(vmax - vmin);

/* init occurs array, which will keep track of how many times each color appears.
   also init the point block for each color in the rainbow*/

  for (i = Hv_sMin(mincolor, maxcolor); i <= Hv_sMax(mincolor, maxcolor); i++) {
    occurs[i] = 0;
    cPtrs[i] = (Hv_Point *)Hv_Malloc(MAXPNTSINREQ*sizeof(Hv_Point));
  }

/* use gray8 for identically zero, which should indicate out of bounds */ 

  occurs[Hv_gray8] = 0;
  cPtrs[Hv_gray8] = (Hv_Point *) Hv_Malloc(MAXPNTSINREQ*sizeof(Hv_Point));

/* also use black for over the max*/

  occurs[Hv_black] = 0;
  cPtrs[Hv_black] = (Hv_Point *) Hv_Malloc(MAXPNTSINREQ*sizeof(Hv_Point));

D 3
/* malloc the color rectangles. Set the width  & height for all of 'em */
E 3
I 3
/* alloc the color rectangles. Set the width  & height for all of 'em */
E 3

  rectangles = (XRectangle *) Hv_Malloc(MAXPNTSINREQ * sizeof(XRectangle));

  for (i = 0; i < MAXPNTSINREQ; i++) {
    rectangles[i].width  = dpx;
    rectangles[i].height = dpy;
  }

/*loop over rows and columns, obtaining position and color for each corresponding rect*/

  for (i = 0; i < zstep; i++) {
    fzt = frect.xmin + i*dz;
    
    for (j = 0; j < xstep; j++) {
      fxt = frect.ymax - j*dx;

/* convert the screen world coordinates to true CLAS coordinates */

      ScreenZXtoCLASXYZ(View, fzt, fxt, &phi, &x, &y, &z);  
      
      inbounds = BField(x, y, z, Bvect, &field);   /*absolute value of field*/
      
      if (!inbounds)
	cindex = Hv_gray8;
      else if (field < vmin)
	cindex = Hv_white;
      else if (field > vmax)
	cindex = Hv_black;
      else {
	if (backwards) {
	  cindex = maxcolor + (int)((vmax - field) * scale);  /*corresponding color index*/
	  cindex = Hv_sMax(maxcolor, Hv_sMin(mincolor, cindex));
	}
	else {
	  cindex = mincolor + (int)((field - vmin) * scale);  /*corresponding color index*/
	  cindex = Hv_sMax(mincolor, Hv_sMin(maxcolor, cindex));
	}
      }

      Hv_WorldToLocal(View, fzt, fxt, &zt, &xt);
      (cPtrs[cindex] + occurs[cindex])->x = zt;
      (cPtrs[cindex] + occurs[cindex])->y = xt;
      occurs[cindex]++;
	
/* dump it if we've reached the limit */
	
      if (occurs[cindex] == MAXPNTSINREQ)
	DumpCRects(cindex, cPtrs[cindex], occurs, rectangles);
    }
  }
      
/* now a final dump to get any stragglers */

  for(i = Hv_sMin(mincolor, maxcolor); i <= Hv_sMax(mincolor, maxcolor); i++)
      if (occurs[i] > 0)
	DumpCRects(i, cPtrs[i], occurs, rectangles); 
  DumpCRects(Hv_black, cPtrs[Hv_black], occurs, rectangles); 
  DumpCRects(Hv_gray8, cPtrs[Hv_gray8], occurs, rectangles); 
  
  Hv_Free(rectangles);
  
  for(i = Hv_sMin(mincolor, maxcolor); i <= Hv_sMax(mincolor, maxcolor); i++)
      Hv_Free(cPtrs[i]);
  Hv_Free(cPtrs[Hv_black]);
  Hv_Free(cPtrs[Hv_gray8]);

}

#undef MAXPNTSINREQ


E 1
