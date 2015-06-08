
#ifndef VXWORKS

/* scintbuts.c */

#include <stdio.h>
#include "Hv.h"
#include "ced.h"
#include "tigris.h"

int SDbits4sgl[6][2][96]; /* global */
int SPbits4sgl[6][2][64]; /* global */
int EDbits4sgl[6][2][96]; /* global */

extern int bits4FB[6][2][96]; /* defined in feedback.c */
extern int scintColor[6]; /* defined in scintillator.c */


/* local prototypes */

static void drawJamEDs(Hv_Point _p[4], ViewData vdata, BitBoxStruct *b,
                       int dx, int dy);
static void drawJamSPs(Hv_Point _p[4], ViewData vdata, BitBoxStruct *b,
                       int dx, int dy);
static void drawJamSDs(Hv_Point _p[4], ViewData vdata, BitBoxStruct *b,
                       int dx, int dy);

/* -===============================================================- */

void
bbSDclick(Hv_Event hevent)
{
  Hv_Point pt[4];

  int   dx = (hevent->item->area->right - hevent->item->area->left ) /16;
  int   dy = (hevent->item->area->bottom - hevent->item->area->top ) / 3;
  int   wx;
  int   wy;
  BitBoxStruct *z = hevent->item->data ;
  ViewData     vdata=GetViewData (hevent->item->view);

  wx = (hevent->where.x - hevent->item->area->left) / dx;
  wy = (hevent->where.y - hevent->item->area->top) / dy;

  if (z->rbase == 0)
    LightScintillators (hevent->view, wx, wy, pt);
  else
    LightScintillators (hevent->view, (((wx*2)+z->rbase)-1), wy, pt);

  DrawSDbits (hevent->item, NULL);

  printf ("x click = %d\ny click = %d\n", wx, wy);
}

/* -===============================================================- */

void
bbEDclick(Hv_Event hevent)
{
  int   dx = (hevent->item->area->right - hevent->item->area->left ) /16;
  int   dy = (hevent->item->area->bottom - hevent->item->area->top ) / 6;
  int   wx;
  int   wy;
  BitBoxStruct *z = hevent->item->data ;
  ViewData     vdata=GetViewData (hevent->item->view);

  wx = (hevent->where.x - hevent->item->area->left) / dx;
  wy = (hevent->where.y - hevent->item->area->top) / dy;

  printf ("wx[%d] wy[%d] b[%d]\n", wx, wy, ((wx+(wy*16))+1) );

  EDbits4sgl[vdata->sectorNumber][vdata->sglphase][(wx+(wy*16))] ^= 1;

  DrawEDbits (vdata->ExtraDataBits, NULL);

}

/* -===============================================================- */

void
bbSPclick(Hv_Event hevent)
{
  Hv_Point pt[4];

  int   dx = (hevent->item->area->right - hevent->item->area->left ) /16;
  int   dy = (hevent->item->area->bottom - hevent->item->area->top ) / 4;
  int   wx;
  int   wy;
  BitBoxStruct *z = hevent->item->data ;
  ViewData     vdata=GetViewData (hevent->item->view);

  wx = (hevent->where.x - hevent->item->area->left) / dx;
  wy = (hevent->where.y - hevent->item->area->top) / dy;

  printf ("wx[%d] wy[%d]\n", wx, wy);

  SPbits4sgl[vdata->sectorNumber][vdata->sglphase][(wx+(wy*16))] ^= 1;

  DrawSPbits (vdata->SpareDataBits, NULL);

}

/* ================================================================ */

void
drawJamEDs(Hv_Point _p[4], ViewData vdata, BitBoxStruct *b, int dx, int dy) 
{
  Hv_Point _cp[4];
  int wx, owx, wy, owy, i, j, row;

  for(j=0, row=-1, i=b->rangel; i<b->rangeh; i++, j++)
  {
    if ((i%16)==0) {
      row+=1;
      j=0;
    }

    if (EDbits4sgl[vdata->sectorNumber][vdata->sglphase][i] == 1) {
      owx = dx*j;
      wx  = dx*(j+1)+1;
      owy = dy*row;
      wy  = dy*(row+1);
      _cp[0].x = _p[0].x+owx;
      _cp[0].y = _p[0].y+owy;
      _cp[1].x = _p[0].x+wx;
      _cp[1].y = _p[0].y+owy;
      _cp[2].x = _p[0].x+wx;
      _cp[2].y = _p[0].y+wy;
      _cp[3].x = _p[0].x+owx;
      _cp[3].y = _p[0].y+wy;
      Hv_FillPolygon (_cp, 4, False, scintColor[row+b->vbase], Hv_gray10);
    }
  }
}

/* ================================================================ */

void
drawJamSPs(Hv_Point _p[4], ViewData vdata, BitBoxStruct *b, int dx, int dy)
{
  Hv_Point   _cp[4];
  int        wx, owx, wy, owy, i, j, row;
  
  for (j=0, row=-1, i=b->rangel; i<b->rangeh; i++, j++) {
    if ((i%16)==0) {
      row+=1;
      j=0;
    }

    if (SPbits4sgl[vdata->sectorNumber][vdata->sglphase][i] == 1) {
      owx = dx*j;
      wx  = dx*(j+1)+1;
      owy = dy*row;
      wy  = dy*(row+1);
      _cp[0].x = _p[0].x+owx;
      _cp[0].y = _p[0].y+owy;
      _cp[1].x = _p[0].x+wx;
      _cp[1].y = _p[0].y+owy;
      _cp[2].x = _p[0].x+wx;
      _cp[2].y = _p[0].y+wy;
      _cp[3].x = _p[0].x+owx;
      _cp[3].y = _p[0].y+wy;
      Hv_FillPolygon (_cp, 4, False, scintColor[row+b->vbase], Hv_gray10);
    }
  }
  return;
}

/* ================================================================ */

void
drawJamSDs(Hv_Point _p[4], ViewData vdata, BitBoxStruct *b, int dx, int dy)
{
  Hv_Point     _cp[4];
  int          wx, owx, wy, owy, i, j, row;

/* 
 * 1st: draw in any color that is needed 
 */
  
  for (j=0, row=-1, i=b->rangel; i<b->rangeh; i++, j++) {
    if ((i%16)==0) {
      row+=1;
      j=0;
    }

    if(SDbits4sgl[vdata->sectorNumber][vdata->sglphase][i] == 1)
    {
      owx = dx*j;
      wx  = dx*(j+1)+1;
      owy = dy*row;
      wy  = dy*(row+1);
      _cp[0].x = _p[0].x+owx;
      _cp[0].y = _p[0].y+owy;
      _cp[1].x = _p[0].x+wx;
      _cp[1].y = _p[0].y+owy;
      _cp[2].x = _p[0].x+wx;
      _cp[2].y = _p[0].y+wy;
      _cp[3].x = _p[0].x+owx;
      _cp[3].y = _p[0].y+wy;
      Hv_FillPolygon (_cp, 4, False, scintColor[row+b->vbase], Hv_gray10);
    }

  }
  for (j=0, row=-1, i=b->rangel; i<b->rangeh; i++, j++) {
    if ((i%16)==0) {
      row+=1;
      j=0;
    }

    if (bits4FB[vdata->sectorNumber][vdata->sglphase][i] == 1) {
      owx = dx*j;
      wx  = dx*(j+1);
      owy = dy*row;
      wy  = dy*(row+1);
      _cp[0].x = _p[0].x+owx;
      _cp[0].y = _p[0].y+owy;
      _cp[1].x = _p[0].x+wx;
      _cp[1].y = _p[0].y+owy;
      _cp[2].x = _p[0].x+wx;
      _cp[2].y = _p[0].y+wy;
      _cp[3].x = _p[0].x+owx;
      _cp[3].y = _p[0].y+wy;
      Hv_DrawLine (_cp[0].x, _cp[0].y, _cp[2].x, _cp[2].y, Hv_black);
      Hv_DrawLine (_cp[1].x, _cp[1].y, _cp[3].x, _cp[3].y, Hv_black);
    }

  }
}

/* -==============================================================- */

void
DrawSDbits(Hv_Item Item, Hv_Region region)
{
  Hv_Point     _p[4], _cp[4];
  BitBoxStruct *b=Item->data;
  ViewData     vdata=GetViewData (Item->view);
  int          dx = (Item->area->width) / (b->nhordivs+1);
  int          dy = (Item->area->bottom - Item->area->top) / (b->nvertdivs+1);
  int          wx, owx, wy, owy, i, j, row;

  _p[0].x = Item->area->left;
  _p[0].y = Item->area->top;

  _p[1].x = Item->area->right;
  _p[1].y = Item->area->top;

  _p[2].x = Item->area->right;
  _p[2].y = Item->area->bottom;

  _p[3].x = Item->area->left;
  _p[3].y = Item->area->bottom;

  Hv_FillPolygon (_p, 4, False, b->color, Hv_gray10);

  drawJamSDs (_p, vdata, b, dx, dy);
  
  for (i=0, wx=0 ; i<b->nhordivs; i++) {
    wx = dx*(i+1);
    Hv_DrawLine ((_p[0].x+wx), _p[0].y, (_p[3].x+wx), _p[3].y, Hv_black);
  }

  for (i=0, wy=0; i<b->nvertdivs; i++) {
    wy = dy*(i+1);
    Hv_DrawLine (_p[0].x, (_p[0].y+wy), _p[1].x, (_p[1].y+wy), Hv_black);
  }

  Hv_FramePolygon (_p, 4, Hv_black);
}

/* -==============================================================- */

void
DrawEDbits(Hv_Item Item, Hv_Region region)
{
  Hv_Point     _p[4], _cp[4];
  BitBoxStruct *b=Item->data;
  ViewData     vdata=GetViewData (Item->view);
  int          dx = (Item->area->width) / (b->nhordivs+1);
  int          dy = (Item->area->bottom - Item->area->top) / (b->nvertdivs+1);
  int          wx, owx, wy, owy, i, j, row;

  _p[0].x = Item->area->left;
  _p[0].y = Item->area->top;

  _p[1].x = Item->area->right;
  _p[1].y = Item->area->top;

  _p[2].x = Item->area->right;
  _p[2].y = Item->area->bottom;

  _p[3].x = Item->area->left;
  _p[3].y = Item->area->bottom;

  Hv_FillPolygon (_p, 4, False, b->color, Hv_gray10);

  drawJamEDs (_p, vdata, b, dx, dy);
  
  for (i=0, wx=0 ; i<b->nhordivs; i++) {
    wx = dx*(i+1);
    Hv_DrawLine ((_p[0].x+wx), _p[0].y, (_p[3].x+wx), _p[3].y, Hv_black);
  }

  for (i=0, wy=0; i<b->nvertdivs; i++) {
    wy = dy*(i+1);
    Hv_DrawLine (_p[0].x, (_p[0].y+wy), _p[1].x, (_p[1].y+wy), Hv_black);
  }

  Hv_FramePolygon (_p, 4, Hv_black);
}

/* -==============================================================- */

void
DrawSPbits(Hv_Item Item, Hv_Region region)
{
  Hv_Point     _p[4], _cp[4];
  BitBoxStruct *b=Item->data;
  ViewData     vdata=GetViewData (Item->view);
  int          dx = (Item->area->width) / (b->nhordivs+1);
  int          dy = (Item->area->bottom - Item->area->top) / (b->nvertdivs+1);
  int          wx, owx, wy, owy, i, j, row;

  _p[0].x = Item->area->left;
  _p[0].y = Item->area->top;

  _p[1].x = Item->area->right;
  _p[1].y = Item->area->top;

  _p[2].x = Item->area->right;
  _p[2].y = Item->area->bottom;

  _p[3].x = Item->area->left;
  _p[3].y = Item->area->bottom;

  Hv_FillPolygon (_p, 4, False, b->color, Hv_gray10);

  drawJamSPs (_p, vdata, b, dx, dy);
  
  for (i=0, wx=0 ; i<b->nhordivs; i++) {
    wx = dx*(i+1);
    Hv_DrawLine ((_p[0].x+wx), _p[0].y, (_p[3].x+wx), _p[3].y, Hv_black);
  }

  for (i=0, wy=0; i<b->nvertdivs; i++) {
    wy = dy*(i+1);
    Hv_DrawLine (_p[0].x, (_p[0].y+wy), _p[1].x, (_p[1].y+wy), Hv_black);
  }

  Hv_FramePolygon (_p, 4, Hv_black);
}

#else /* UNIX only */

void
scintbuts_dummy()
{
}

#endif
