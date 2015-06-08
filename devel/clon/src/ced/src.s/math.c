/*
----------------------------------------------------
-							
-  File:    math.c				
-							
-  Summary:						
-            athematical utility functions
-					       
-                     				
-  Revision history:				
-                     				
-  Date       Programmer     Comments		
------------------------------------------------------
-  10/19/94	DPH		Initial Version	
-                                              	
------------------------------------------------------
*/

#include "ced.h"


/*---------- StartWait --------*/

void          StartWait(void)

{

  Hv_SetCursor(Hv_waitCursor);
  Hv_Flush();
}


/*---------- StopWait ---------*/

void          StopWait(void)

{
  Hv_SetCursor(Hv_defaultCursor);
  Hv_Flush();
}

/* -------- scalevect ------ */

void scalevect(fvect   R,
	       float   sf)

/* multiplies each component of the real
   vector R by the scalar sf

fvect     R   <--> vector to be scaled
float     sf   --> sacling factor

*/

{
  Vcomp     comp;
  for (comp = Xcomp; comp <= Zcomp; comp++)
    R[comp] *= sf;
}



/*----- fvectmag ------*/

float  fvectmag(fvect fv)

/* simply returns the magnitude of a real vector

 fv  --> input 3D Cartesian vector
*/

{
  return(sqrt(fv[Xcomp]*fv[Xcomp] + fv[Ycomp]*fv[Ycomp] + fv[Zcomp]*fv[Zcomp]));
}
 
/* ------ Cart3DLength ------- */

float    Cart3DLength(Cart3D *R)

/* usual vector magnitude function */
     
{
  return(sqrt(R->x*R->x + R->y*R->y + R->z*R->z));
}

/* ------- PixelToPixelWorldDistance -------*/

float PixelToPixelWorldDistance(Hv_View   View,
				short    x1,
				short    y1,
				short    x2,
				short    y2)
{
  float dx, dy, fx1, fx2, fy1, fy2;

  Hv_LocalToWorld(View, &fx1, &fy1, x1, y1);
  Hv_LocalToWorld(View, &fx2, &fy2, x2, y2);
  dx = fx1 - fx2;
  dy = fy1 - fy2;

  return (float)sqrt(dx*dx + dy*dy);
}

/*--------- Distance3D ---------- */

float Distance3D(float  x1,
		 float  y1,
		 float  z1,
		 float  x2,
		 float  y2,
		 float  z2)
{
  float    x, y, z;

  x = x1 - x2;
  y = y1 - y2;
  z = z1 - z2;

  return sqrt(x*x + y*y + z*z);
}


/*--------- ScreenZXtoCLASXYZ ------------*/

void  ScreenZXtoCLASXYZ(Hv_View  View,
			float   fz,
			float   fx,
			float   *phi,
			float   *x,
			float   *y,
			float   *z)

/* converts the screen cordinates in the
   world system to CLAS XYZ

   fz, fx;         screen (world) coordinates
  *phi;            phi in radians
   *x, *y, *z;     CLAS catresian coordinates
*/

{
  float      fabsx;
  ViewData   viewdata;

  viewdata = GetViewData(View);

  *phi = viewdata->phi;
  if (fx < 0.0)
    *phi += Hv_PI;

  *z = fz;

  fabsx = fabs(fx);
  *x = fabsx*cos(*phi);
  *y = fabsx*sin(*phi);

/* note our units for distance are in cm */

  CheckZeroTolerance(x, 1.0e-3);
  CheckZeroTolerance(y, 1.0e-3);
  CheckZeroTolerance(z, 1.0e-3);
}

/*------ MidplanePolyToProjectedPoly --------*/

void  MidplanePolyToProjectedPoly(Hv_View   View,
				  Hv_FPoint *fpts,
				  short     np)

/* INPLACE conversion from midplane poly to
   projected poly */

{
  short  i;
  ViewData    viewdata;

  viewdata = GetViewData(View);

  if (viewdata->dphi == 0)
    return;

  for (i = 0; i < np; i++)
    fpts[i].v = MidplaneXtoProjectedX(View, fpts[i].v);
}



/*--------- MidplaneXtoProjectedX ------*/

float MidplaneXtoProjectedX(Hv_View   View,
			    float    x)

{
  ViewData    viewdata;

  viewdata = GetViewData(View);

  if (viewdata->dphi == 0)
    return x;
  else
    return x/cos(Hv_DEGTORAD*(viewdata->dphi));
}


/*------- XYZtoSector ------*/

int XYtoSector(float x,
	       float y)

/* coverts the clas coordinates x y (
   the third corrdinate, z, is not needed) to a
   C INDEX sector number [0..5] */

{
  float  phi;
  int    sector;

  if ((x == 0.0) && (y == 0.0))
    phi = 0.0;
  else {
    phi = atan2(y, x);
    if (phi < 0.0)
      phi += Hv_TWOPI;
    phi *= Hv_RADTODEG;
  }

  sector = (int)phi;
  sector += 30;
  sector = sector/60;

  return sector % 6;
}

/*------------ CLASXYZtoSectorXYZ ---------*/

extern void  CLASXYZtoSectorXYZ(int   sect,
				float x,
				float y,
				float z,
				float *sx,
				float *sy,
				float *sz)

/* rotates clas xyz to sector system for whatever
   sector. This has +x axis verticle
   in that sector

   sect is a C INDEX */

{
    int sector;
    float  theta;
    Hv_FPoint  fp;

    *sz = z;

    if (sect == 0) {
	*sx = x;
	*sy = y;
	return;
    }

    theta = (-60.0*sect)*Hv_DEGTORAD;
    fp.h = x;
    fp.v = y;
    Hv_RotateFPoint(theta, &fp);
    *sx = fp.h;
    *sy = fp.v;
/*fprintf(stderr, "[%d] CLAS  [%f, %f] sector  [%f, %f] \n", sect + 1, x, y, *sx, *sy); */
}



/*--------- ScreenZXtoCLASCoord ------------*/


void  ScreenZXtoCLASCoord(Hv_View  View,
			  float   fz,
			  float   fx,
			  float   *phi,
			  float   *x,
			  float   *y,
			  float   *z,
			  float   *r,
			  float   *theta)

/* converts the screen cordinates in the world
   system to actual CLAS coordinates

   fz, fx            screen (world) coordinates
   *phi              phi in radians
   *x, *y, *z        CLAS catresian coordinates
   *r, *theta        remaining CLAS polar coordinates
*/

{
  ScreenZXtoCLASXYZ(View, fz, fx, phi, x, y, z);

  *r = sqrt((*x * *x) + (*y * *y) + (*z * *z));
  
  if (fabs(*r) < 1.0e-10)
    *theta = 0.0;
  else
    *theta = acos(*z / *r);

/* note our units for distance are in cm */

  CheckZeroTolerance(r, 1.0e-3);
  CheckZeroTolerance(theta, 1.0e-3);
}


/*--------- LocaltoCLASXYZ ------------*/

void  LocaltoCLASXYZ(Hv_View  View,
		     short   pixx,
		     short   pixy,
		     float   *phi,
		     float   *x,
		     float   *y,
		     float   *z)

/* converts the pixel cordinates in the
   local system to CLAS XYZ 

     pixx, pixy;     pixel coordinates
    *phi;            phi in radians
     *x, *y, *z;     CLAS catresian coordinates

*/

{
  float   fz, fx;

  Hv_LocalToWorld(View, &fz, &fx, pixx, pixy);
  ScreenZXtoCLASXYZ(View, fz, fx, phi, x, y, z);
}

/*--------  CEDxyToCLASxy  ---------*/

void  CEDxyToCLASxy(float  xced,
		    float  yced,
		    float  *x,
		    float  *y,
		    int   sect)

/*****************************************
Converts the x and y coordinates of a local
sector system (the 1-4, 2-5, or 3-6 with
+x going through middle of 1, 2 & 3) to
real CLAS xy with x through middle of
sector 1.

sect is a C Index

******************************************/

{

  sect++;   /* make it easier to read */

  switch (sect) {
  case 1:
    *x = xced;
    *y = yced;
    break;
    
  case 2:
    *x = 0.5*xced - ROOT3OVER2*yced;
    *y = ROOT3OVER2*xced + 0.5*yced;
    break;
    
  case 3:
    *x = -0.5*xced - ROOT3OVER2*yced;
    *y = ROOT3OVER2*xced - 0.5*yced;
    break;
    
  case 4:
    *x = -xced;
    *y = yced;
    break;
    
  case 5:
    *x = -0.5*xced + ROOT3OVER2*yced;
    *y = -ROOT3OVER2*xced - 0.5*yced;
    break;
    
  case 6:
    *x =  0.5*xced + ROOT3OVER2*yced;
    *y = -ROOT3OVER2*xced + 0.5*yced;
    break;

  default:
    *x = xced;
    *y = yced;
    fprintf(stderr, "BAD SECTOR PASSED TO CEDxyToCLASxy\n");
  }
}


/*--------- LocaltoCLASCoord ------------*/

void  LocaltoCLASCoord(Hv_View  View,
		       short   pixx,
		       short   pixy,
		       float   *phi,
		       float   *x,
		       float   *y,
		       float   *z,
		       float   *r,
		       float   *theta)

/* converts the screen cordinates in the world system to
   actual CLAS coordinates

      pixx, pixy;    pixel coordinates
     *phi;           phi in radians
     *x, *y, *z;     CLAS catresian coordinates
      *r, *theta;    remaining CLAS polar coordinates

*/

{
  float   fz, fx;

  
  Hv_LocalToWorld(View, &fz, &fx, pixx, pixy);

  ScreenZXtoCLASCoord(View, fz, fx, phi, x, y, z, r, theta);
}

/* -------- CheckZeroTolerance ------*/

void CheckZeroTolerance(float *v,
			float eps)

/* allows us to set to zero numbers that are close enough*/

{
  if (fabs(*v) < eps)
    *v = 0.0;
}
