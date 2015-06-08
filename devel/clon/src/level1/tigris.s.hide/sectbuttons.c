
#ifndef VXWORKS

/* sectbuttons.c */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Hv.h"
#include "ced.h"
#include "tigris.h"

extern struct Look look; /* defined in cradle.c */
extern int Compile_Done; /* defined in cradle.c */
extern TriggerInfo Jti;  /* defined in setup.c */
extern int trigColor[6]; /* defined in init.c */
extern char *fname;      /* defined in init.c */
extern struct phase3_def JAMphase3; /* defined in fileio.c */
extern struct phase3_def phase3; /* defined in fileio.c */


/* local prototypes */

static void triggerButton(Hv_Event hevent);
static void PutTextIntoHv(int offs, int but, char *buf);
static void trigSEButton(Hv_Event hevent);
static void flush_trigr();
static void change_triglabels(int n);







/* TEMPORARY */
/* see Hv_data.h for 'Hv_Item' and 'Hv_String' structure */

static void
my_UpdateTextItem(Hv_Item Item)
{
  Hv_Region   viewreg, areareg, totreg;
  short       extra;
  Hv_Rect     area;

  if(Item == NULL) return;
  viewreg = Hv_GetViewRegion(Item->view);
  extra = Item->scheme->framewidth + Item->scheme->borderwidth;
  Hv_ShrinkRect(&area, extra, extra);
  Hv_ResizeRect(&area, -1, -1);
  Hv_CopyRect(&area, Item->area);
  areareg = Hv_RectRegion(&area);
  totreg = Hv_IntersectRegion(viewreg, areareg);
  Hv_DrawItem(Item, totreg);
  Hv_DestroyRegion(viewreg);
  Hv_DestroyRegion(areareg);
  Hv_DestroyRegion(totreg);
}

char *
my_GetItemText(Hv_Item Item)    
{
printf("my_GetItemText reached\n");

  if((Item == NULL) || (Item->str == NULL)) return NULL;
  return(Item->str->text);
}

void
my_SetItemText(Hv_Item Item, char *text)    
{
  Hv_String *str;
printf("my_SetItemText reached\n");

  if((Item == NULL) || ((str = Item->str) == NULL)) return;

  Hv_Free(str->text);
  Hv_InitCharStr(&(str->text), text);
  str->offset = 0;
}

void
my_ConcatTextToTextItem(Hv_Item Item, char *concattext)
/****************************************
  Add the text to the text item and see if
  it needs to be scrolled.
  ****************************************/
{
  char *text, *newtext;
  int extra, len1, len2;
  short room;
  short w, h;
printf("my_ConcatTextToTextItem reached\n");

  if((Item == NULL) || (Item->str == NULL) || (concattext == NULL)) return;

  text = my_GetItemText(Item);

  /* next check fixes bug found by James Muguira */

  if(text == NULL)
  {
    Hv_InitCharStr(&text, concattext);
    return;
  }

  len1 = strlen(text);
  len2 = strlen(concattext);

  newtext = (char *)Hv_Malloc(len1+len2+1);
  strcpy(newtext, text);
  strcat(newtext, concattext);
  my_SetItemText(Item, newtext);

  /* see if offset is required */

  Hv_CompoundStringDimension(Item->str, &w, &h);
  extra = 2 + Item->scheme->framewidth + Item->scheme->borderwidth;
  extra *= 2;
  room = Item->area->width - extra;

  Item->str->offset = Hv_sMax(0, w - room); /* 'offset' is scrolling param */
printf("Item->str->offset=%d\n",Item->str->offset);

  my_UpdateTextItem(Item);
}

/* TEMPORARY */








/* ----------------------- triggerButton ------------------- */

static void
triggerButton(Hv_Event hevent)
{

  /* User pressed either and || or || not || clr */
  printf ("trigger button push!\n") ;
}

/* ------------------ trigAndButton --------------------- */

void
trigAndButClick(Hv_Event hevent)
{
  if (hevent->item->user1 == 1) {
    switch (Jti.operator[ (hevent->item->user1-1) + Jti.butSet1] ) {
    case '+' : 
      Hv_InvertColorScheme (Jti.bOr1->scheme) ;
      Hv_DrawItem (Jti.bOr1, NULL);
      break;
    case '/':
      Hv_InvertColorScheme (Jti.bNot1->scheme) ;
      Hv_DrawItem (Jti.bNot1, NULL);
      break;
    case 'C' :
      Hv_InvertColorScheme (Jti.bClr1->scheme) ;
      Hv_DrawItem (Jti.bClr1, NULL);
      break;
    }
    Hv_InvertColorScheme (Jti.bAnd1->scheme);
    Hv_DrawItem (Jti.bAnd1, NULL);
    Jti.operator[ (hevent->item->user1-1) + Jti.butSet1] = '*' ;
  } else {
    switch (Jti.operator[ (hevent->item->user1-1) + Jti.butSet1] ) {
    case '+' : 
      Hv_ToggleButton (Jti.bOr2);
      Hv_InvertColorScheme (Jti.bOr2->scheme) ;
      Hv_DrawItem (Jti.bOr2, NULL);
      break;
    case '/':
      Hv_ToggleButton (Jti.bClr1);
      Hv_InvertColorScheme (Jti.bNot2->scheme) ;
      Hv_DrawItem (Jti.bNot2, NULL);
      break;
    case 'C' :
      Hv_ToggleButton (Jti.bClr1);
      Hv_InvertColorScheme (Jti.bClr2->scheme) ;
      Hv_DrawItem (Jti.bClr2, NULL);
      break;
    }
    Hv_InvertColorScheme (Jti.bAnd2->scheme);
    Hv_DrawItem (Jti.bAnd2, NULL);
    Jti.operator[ (hevent->item->user1-1) + Jti.butSet1] = '*' ;
  }
}

/* ------------------ trigOrButton --------------------- */

void
trigOrButClick(Hv_Event hevent)
{
  char *t, buf[1024];
  int len, index;
  int i, j;
  
  if (hevent->item->user1 == 1) {
    index = 0 + Jti.butSet1 ;
    j=0;
  } else {
    index = 2 + Jti.butSet2;
    j=1;
  }

  for (i=0; i<6; i++)
    Jti.can_not_do[index][j][i] = 0;

  t = my_GetItemText(Jti.titem[index]);
  len = strlen(t);
  strncpy(buf,t,len);
  buf[len-1] = '+';
  buf[len] = 0;

  my_SetItemText(Jti.titem[index],buf);
  Hv_DrawTextItem(Jti.titem[index],NULL);

  if (hevent->item->user1 == 1) {
    switch (Jti.operator[ index ] ) {
    case '*' : 
      Hv_InvertColorScheme (Jti.bAnd1->scheme) ;
      Hv_DrawItem (Jti.bAnd1, NULL);
      break;
    case '/':
      Hv_InvertColorScheme (Jti.bNot1->scheme) ;
      Hv_DrawItem (Jti.bNot1, NULL);
      break;
    case 'C' :
      Hv_InvertColorScheme (Jti.bClr1->scheme) ;
      Hv_DrawItem (Jti.bClr1, NULL);
      break;
    }
    Hv_InvertColorScheme (Jti.bOr1->scheme);
    Hv_DrawItem (Jti.bOr1, NULL);
    Jti.operator[ index ] = '+' ;

  } else {

    switch (Jti.operator[ index ] ) {
    case '*' : 
      Hv_InvertColorScheme (Jti.bAnd2->scheme) ;
      Hv_DrawItem (Jti.bAnd2, NULL);
      break;
    case '/':
      Hv_InvertColorScheme (Jti.bNot2->scheme) ;
      Hv_DrawItem (Jti.bNot2, NULL);
      break;
    case 'C' :
      Hv_InvertColorScheme (Jti.bClr2->scheme) ;
      Hv_DrawItem (Jti.bClr2, NULL);
      break;
    }
    Hv_InvertColorScheme (Jti.bOr2->scheme);
    Hv_DrawItem (Jti.bOr2, NULL);
    Jti.operator[ index ] = '+' ;

  }
  

}

/* ------------------ Trignotbutclick --------------------- */

void
trigNotButClick(Hv_Event hevent)
{
  if (hevent->item->user1 == 1) {
    switch (Jti.operator[ (hevent->item->user1-1) + Jti.butSet1] ) {
    case '*' : 
      Hv_InvertColorScheme (Jti.bAnd1->scheme) ;
      Hv_DrawItem (Jti.bAnd1, NULL);
      break;
    case '+':
      Hv_InvertColorScheme (Jti.bOr1->scheme) ;
      Hv_DrawItem (Jti.bOr1, NULL);
      break;
    case 'C' :
      Hv_InvertColorScheme (Jti.bClr1->scheme) ;
      Hv_DrawItem (Jti.bClr1, NULL);
      break;
    }
    Hv_InvertColorScheme (Jti.bNot1->scheme);
    Hv_DrawItem (Jti.bNot1, NULL);
    Jti.operator[ (hevent->item->user1-1) + Jti.butSet1] = '/' ;
  } else {
    switch (Jti.operator[ (hevent->item->user1-1) + Jti.butSet1] ) {
    case '*' : 
      Hv_InvertColorScheme (Jti.bAnd2->scheme) ;
      Hv_DrawItem (Jti.bAnd2, NULL);
      break;
    case '+':
      Hv_InvertColorScheme (Jti.bOr2->scheme) ;
      Hv_DrawItem (Jti.bOr2, NULL);
      break;
    case 'C' :
      Hv_InvertColorScheme (Jti.bClr2->scheme) ;
      Hv_DrawItem (Jti.bClr2, NULL);
      break;
    }
    Hv_InvertColorScheme (Jti.bNot2->scheme);
    Hv_DrawItem (Jti.bNot2, NULL);
    Jti.operator[ (hevent->item->user1-1) + Jti.butSet1] = '/' ;
  }
}

/* ------------------ trigClrButClick --------------------- */

void
trigClrButClick(Hv_Event hevent)
{
  int index;
  char buf[1024];
  int i, j;
  
  if(hevent->item->user1 == 1)
  {
    index = 0 + Jti.butSet1 ;
    j=0;
  }
  else
  {
    index = 2 + Jti.butSet2;
    j=1;
  }

  buf[0] = ' ';
  buf[1] = 0;

  for(i=0; i<6; i++) Jti.can_not_do[index][j][i] = 0;

  my_SetItemText(Jti.titem[index],buf);
  Hv_DrawTextItem(Jti.titem[index],NULL);

}

/* ----------------- a utility ------------------------ */
/* offs=0 or 2, but=0 or 1, they are all window number from 0 to 3 */
/* 'buf' contains string to be drawn */ 
static void
PutTextIntoHv(int offs, int but, char *buf)
{
  char *t;

  t = my_GetItemText(Jti.titem[offs+but]);
  if(t == 0)
    my_SetItemText(Jti.titem[offs+but],buf);
  else
    my_ConcatTextToTextItem(Jti.titem[offs+but],buf);

  Jti.operator[offs+but] = '*';
printf("PISHEM TEXT ...\n");
  Hv_DrawTextItem(Jti.titem[offs+but],NULL);
}

/* ------------------ trigSeButton --------------------- */

static void
trigSEButton(Hv_Event hevent)
{
  int offs;
  char *t, buf[80];

  printf("button %d   sector %d\n", hevent->item->user2, hevent->item->user1);

  if (Jti.rstate == 0)  /* == 0 iff look @ 1st 4 triggers */
    offs = 0; /* use the bottom 4 trig can_not_do bits */
  else
    offs = 4; /* use the top 4 trig can_not_do bits */

  if (hevent->item->user2 == 3) {
    if (Jti.can_not_do[ (offs+Jti.butSet1) ][0][(hevent->item->user1-1)]) {
      Hv_Warning ("Some kind of complaint #1\n");
      return ;
    } else {
      if ((hevent->item->user1-1) < 3) {
	Jti.can_not_do [ (offs+Jti.butSet1) ][0][3] = 1;
	Jti.can_not_do [ (offs+Jti.butSet1) ][0][4] = 1;
	Jti.can_not_do [ (offs+Jti.butSet1) ][0][5] = 1;
      } else {
	Jti.can_not_do [ (offs+Jti.butSet1) ][0][0] = 1;
	Jti.can_not_do [ (offs+Jti.butSet1) ][0][1] = 1;
	Jti.can_not_do [ (offs+Jti.butSet1) ][0][2] = 1;
      }
    }
  }
  if (hevent->item->user2 == 6) {
    if (Jti.can_not_do[ (offs+(Jti.butSet2+2)) ][1][(hevent->item->user1-1)]) {
      Hv_Warning ("Some kind of complaint #2\n");
      return ;
    } else {
      if ((hevent->item->user1-1) < 3) {
	Jti.can_not_do [ (offs+(Jti.butSet2+2)) ][1][3] = 1;
	Jti.can_not_do [ (offs+(Jti.butSet2+2)) ][1][4] = 1;
	Jti.can_not_do [ (offs+(Jti.butSet2+2)) ][1][5] = 1;
      } else {
	Jti.can_not_do [ (offs+(Jti.butSet2+2)) ][1][0] = 1;
	Jti.can_not_do [ (offs+(Jti.butSet2+2)) ][1][1] = 1;
	Jti.can_not_do [ (offs+(Jti.butSet2+2)) ][1][2] = 1;
      }
    }
  }

  if (hevent->item->user2 < 4) {
    if (Jti.operator[0+Jti.butSet1] == '/') {
      sprintf (buf, " %cS%d.SE%d %c", Jti.operator[0+Jti.butSet1], hevent->item->user1, (hevent->item->user2+Jti.which6), '*');
      Hv_InvertColorScheme (Jti.bAnd1->scheme) ;
      Hv_DrawItem (Jti.bAnd1, NULL);
      PutTextIntoHv(0,Jti.butSet1,buf);
      Hv_InvertColorScheme (Jti.bNot1->scheme) ;
      Hv_DrawItem (Jti.bNot1, NULL);

    } else {
      if (Jti.operator [ 0 + Jti.butSet1 ] == '+') {
	sprintf (buf, " S%d.SE%d %c", hevent->item->user1, (hevent->item->user2+Jti.which6), '*');
	Hv_InvertColorScheme (Jti.bOr1->scheme);
	Hv_DrawItem (Jti.bOr1, NULL);

	Hv_InvertColorScheme (Jti.bAnd1->scheme);
	Hv_DrawItem (Jti.bAnd1, NULL);
	PutTextIntoHv(0,Jti.butSet1,buf);

      } else {
	sprintf(buf," S%d.SE%d %c",
      hevent->item->user1,(hevent->item->user2+Jti.which6), '*');
	PutTextIntoHv(0,Jti.butSet1,buf);
      }
    } 
  } else {
    t = my_GetItemText(Jti.titem[ 2 + Jti.butSet2]);
    if (Jti.operator[2+Jti.butSet2] == '/') {
      sprintf (buf, " %cS%d.SE%d %c", Jti.operator[2+Jti.butSet2], hevent->item->user1, (hevent->item->user2+Jti.which6), '*');
      Hv_InvertColorScheme (Jti.bAnd2->scheme) ;
      Hv_DrawItem (Jti.bAnd2, NULL);

      Hv_InvertColorScheme (Jti.bNot2->scheme) ;
      Hv_DrawItem (Jti.bNot2, NULL);

      PutTextIntoHv(2,Jti.butSet2,buf);
      /* my_ConcatTextToTextItem (Jti.titem[ 2 + Jti.butSet2 ], buf) ; */
      Jti.operator [2+Jti.butSet2] = '*';
    } else
      if (Jti.operator [ 2 + Jti.butSet2 ] == '+') {
	sprintf (buf, " S%d.SE%d %c", hevent->item->user1, (hevent->item->user2+Jti.which6), '*');
	Hv_InvertColorScheme (Jti.bOr2->scheme);
	Hv_DrawItem (Jti.bOr2, NULL);

	Hv_InvertColorScheme (Jti.bAnd2->scheme);
	Hv_DrawItem (Jti.bAnd2, NULL);

	PutTextIntoHv(2,Jti.butSet2,buf);
	/* my_ConcatTextToTextItem (Jti.titem[ 2 + Jti.butSet2 ], buf) ;     */
	Jti.operator [2+Jti.butSet2] = '*';
      } else {
	sprintf (buf, " S%d.SE%d %c", hevent->item->user1, (hevent->item->user2+Jti.which6), '*');
	PutTextIntoHv(2,Jti.butSet2,buf);
	/* my_ConcatTextToTextItem (Jti.titem[ 2 + Jti.butSet2 ], buf) ; */
	Jti.operator [2+Jti.butSet2] = '*';
      }
  }


}

/* ------------------ flush trigger text items --------------------- */

static void
flush_trigr()
{
  int itrig;
  char *t;

  for(itrig=0; itrig<4; itrig++)
  {
    t = my_GetItemText(Jti.titem[itrig]);
    if(t == NULL) continue;

    if(Jti.trigString[Jti.rstate][itrig])
      free(Jti.trigString[Jti.rstate][itrig]);

    Jti.trigString[Jti.rstate][itrig] = strdup(t);
    if(Jti.trigString[Jti.rstate][itrig] == NULL)
      printf("flush_trigr: ERROR: strdup returns 0\n");
  }
}

/* ---------------- put new text into trigger text items ------------------ */
/* n is either 0 or 6 */
static void
change_triglabels(int n)
{
  int i, j, k;
  char str[2048];

  for(k=1, j=0; j<18; j++)
  {
    sprintf(str,"%d",(k+n));
    my_SetItemText(Jti.sb[j],str);
    Hv_DrawItem (Jti.sb[j],NULL);
    if(++k > 3) k = 1;
  }

  for(k=4, j=18; j<36; j++)
  {
    sprintf(str,"%d",(k+n));
    my_SetItemText(Jti.sb[j],str);
    Hv_DrawItem(Jti.sb[j],NULL);
    if(++k > 6) k=4;
  }
} 

/* ---------------- put new text into trigger text items ------------------ */

void
refresh_trigr(int zzz)
{
  int it;

  for(it=0; it<4; it++)
  {
    my_SetItemText(Jti.titem[it], Jti.trigString[zzz][it]);
    Hv_DrawTextItem(Jti.titem[it], NULL);
  }
}

/* ------------------ SERadioClick --------------------- */

void
SERadioClick(Hv_Event hevent)
{
  int z;

  z = Hv_HandleRadioButton (hevent->item) ;
  if(hevent->item->user1 == 2)
  {
    flush_trigr();
    Jti.rstate = 1;
    Jti.which6 = 6;
    change_triglabels(6);
    refresh_trigr(Jti.rstate);
  }
  else
  {
    flush_trigr();
    Jti.rstate = 0;
    Jti.which6 = 0;
    change_triglabels(0);
    refresh_trigr(Jti.rstate);
  }
  printf("SERadio Click: user1 = %d\n", hevent->item->user1);
}

/* ------------------ DrawTrigButtons ------------------ */
/* n is index for 1-3 or 4-6 */
void
DrawTrigButtons(Hv_View View, Hv_Item Item, int n)
{
  int i = 1, z;
  char lb[10] ;
  short tcolor ;

  if (n > 0) { 
    z = 18;
    tcolor = Hv_black ;
  } else {
    z = 0;
    tcolor = Hv_skyBlue ;
  }
/* Assumes the last item created was a hex sector */
/* sector - 1 */
  sprintf (lb, "%d", i+n+Jti.which6) ;
  Jti.sb[0+z] = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_DOMAIN,            Hv_INSIDEHOTRECT,
                         Hv_FILLCOLOR,         trigColor[(i-1)+n],
                         Hv_LABEL,             lb,
			 Hv_USER1,             1,    /* sector */
			 Hv_USER2,             1+n,    /* se */
                         Hv_TEXTCOLOR,         tcolor,
                         Hv_LEFT,              Item->area->left+7,
                         Hv_TOP,               Item->area->top+145,
                         Hv_HEIGHT,            20,
                         Hv_WIDTH,             20,
                         Hv_SINGLECLICK,       trigSEButton,
                         NULL) ;
  i++ ;
  sprintf (lb, "%d", i+n+Jti.which6) ;
  Jti.sb[1+z] = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_LABEL,             lb,

			 Hv_USER1,             1,
			 Hv_USER2,             2+n,

                         Hv_DOMAIN,            Hv_INSIDEHOTRECT,
                         Hv_TEXTCOLOR,         tcolor,
                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_FILLCOLOR,         trigColor[(i-1)+n],
                         Hv_HEIGHT,            20,
                         Hv_WIDTH,             20,
                         Hv_SINGLECLICK,       trigSEButton,
                         NULL) ;

  i++ ;
  sprintf (lb, "%d", i+n+Jti.which6) ;
  Jti.sb[2+z] = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_LABEL,             lb,
			 Hv_USER1,             1,
			 Hv_USER2,             3+n,

                         Hv_DOMAIN,            Hv_INSIDEHOTRECT,
                         Hv_TEXTCOLOR,         tcolor,
                         Hv_FILLCOLOR,         trigColor[(i-1)+n],
                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_HEIGHT,            20,
                         Hv_WIDTH,             20,
                         Hv_SINGLECLICK,       trigSEButton,
                         NULL) ;

  i = 1 ;

/* sector - 2 */
  sprintf (lb, "%d", i+n+Jti.which6) ;
  Jti.sb[3+z] = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_DOMAIN,            Hv_INSIDEHOTRECT,
                         Hv_FILLCOLOR,         trigColor[(i-1)+n],
                         Hv_TEXTCOLOR,         tcolor,
                         Hv_LABEL,             lb,

			 Hv_USER1,             2,
			 Hv_USER2,             1+n,

                         Hv_LEFT,              Item->area->left+60,
                         Hv_TOP,               Item->area->top+50,
                         Hv_HEIGHT,            20,
                         Hv_WIDTH,             20,
                         Hv_SINGLECLICK,       trigSEButton,
                         NULL) ;

  i++;
  sprintf (lb, "%d", i+n+Jti.which6) ;
  Jti.sb[4+z] = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_DOMAIN,            Hv_INSIDEHOTRECT,
                         Hv_TEXTCOLOR,         tcolor,
                         Hv_FILLCOLOR,         trigColor[(i-1)+n],
                         Hv_LABEL,             lb,

			 Hv_USER1,             2,
			 Hv_USER2,             2+n,

                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_HEIGHT,            20,
                         Hv_WIDTH,             20,
                         Hv_SINGLECLICK,       trigSEButton,
                         NULL) ;

  i++;
  sprintf (lb, "%d", i+n+Jti.which6) ;
  Jti.sb[5+z] = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_DOMAIN,            Hv_INSIDEHOTRECT,
                         Hv_TEXTCOLOR,         tcolor,
                         Hv_FILLCOLOR,         trigColor[(i-1)+n],
                         Hv_LABEL,             lb,

			 Hv_USER1,             2,
			 Hv_USER2,             3+n,

                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_HEIGHT,            20,
                         Hv_WIDTH,             20,
                         Hv_SINGLECLICK,       trigSEButton,
                         NULL) ;

  i=1;

/* sector - 3 */
  sprintf (lb, "%d", i+n+Jti.which6) ;
  Jti.sb[6+z] = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_DOMAIN,            Hv_INSIDEHOTRECT,
                         Hv_TEXTCOLOR,         tcolor,
                         Hv_FILLCOLOR,         trigColor[(i-1)+n],
                         Hv_LABEL,             lb,

			 Hv_USER1,             3,
			 Hv_USER2,             1+n,

                         Hv_LEFT,              Item->area->left+188,
                         Hv_TOP,               Item->area->top+50,
                         Hv_HEIGHT,            20,
                         Hv_WIDTH,             20,
                         Hv_SINGLECLICK,       trigSEButton,
                         NULL) ;

  i++ ;
  sprintf (lb, "%d", i+n+Jti.which6) ;
  Jti.sb[7+z] = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_DOMAIN,            Hv_INSIDEHOTRECT,
                         Hv_TEXTCOLOR,         tcolor,
                         Hv_FILLCOLOR,         trigColor[(i-1)+n],
                         Hv_LABEL,             lb,

			 Hv_USER1,             3,
			 Hv_USER2,             2+n,

                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_HEIGHT,            20,
                         Hv_WIDTH,             20,
                         Hv_SINGLECLICK,       trigSEButton,
                         NULL) ;

  i++ ;
  sprintf (lb, "%d", i+n+Jti.which6) ;
  Jti.sb[8+z] = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_DOMAIN,            Hv_INSIDEHOTRECT,
                         Hv_TEXTCOLOR,         tcolor,
                         Hv_FILLCOLOR,         trigColor[(i-1)+n],
                         Hv_LABEL,             lb,

			 Hv_USER1,             3,
			 Hv_USER2,             3+n,

                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_HEIGHT,            20,
                         Hv_WIDTH,             20,
                         Hv_SINGLECLICK,       trigSEButton,
                         NULL) ;


  i=1 ;

/* sector - 4 */
  sprintf (lb, "%d", i+n+Jti.which6) ;
  Jti.sb[9+z] = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_TEXTCOLOR,         tcolor,
                         Hv_FILLCOLOR,         trigColor[(i-1)+n],
                         Hv_DOMAIN,            Hv_INSIDEHOTRECT,
                         Hv_LABEL,             lb,

			 Hv_USER1,             4,
			 Hv_USER2,             1+n,

                         Hv_LEFT,              Item->area->left+240,
                         Hv_TOP,               Item->area->top+145,
                         Hv_HEIGHT,            20,
                         Hv_WIDTH,             20,
                         Hv_SINGLECLICK,       trigSEButton,
                         NULL) ;

  i++ ;
  sprintf (lb, "%d", i+n+Jti.which6) ;
  Jti.sb[10+z] = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_TEXTCOLOR,         tcolor,
                         Hv_DOMAIN,            Hv_INSIDEHOTRECT,
                         Hv_FILLCOLOR,         trigColor[(i-1)+n],
                         Hv_LABEL,             lb,

			 Hv_USER1,             4,
			 Hv_USER2,             2+n,

                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_HEIGHT,            20,
                         Hv_WIDTH,             20,
                         Hv_SINGLECLICK,       trigSEButton,
                         NULL) ;

  i++ ;
  sprintf (lb, "%d", i+n+Jti.which6) ;
  Jti.sb[11+z] = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_TEXTCOLOR,         tcolor,
                         Hv_FILLCOLOR,         trigColor[(i-1)+n],
                         Hv_DOMAIN,            Hv_INSIDEHOTRECT,
                         Hv_LABEL,             lb,

			 Hv_USER1,             4,
			 Hv_USER2,             3+n,

                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_HEIGHT,            20,
                         Hv_WIDTH,             20,
                         Hv_SINGLECLICK,       trigSEButton,
                         NULL) ;

  i=1;
/* sector - 5 */
  sprintf (lb, "%d", i+n+Jti.which6) ;
  Jti.sb[12+z] = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_TEXTCOLOR,         tcolor,
                         Hv_FILLCOLOR,         trigColor[(i-1)+n],
                         Hv_DOMAIN,            Hv_INSIDEHOTRECT,
                         Hv_LABEL,             lb,

			 Hv_USER1,             5,
			 Hv_USER2,             1+n,

                         Hv_LEFT,              Item->area->left+188,
                         Hv_TOP,               Item->area->top+262,
                         Hv_HEIGHT,            20,
                         Hv_WIDTH,             20,
                         Hv_SINGLECLICK,       trigSEButton,
                         NULL) ;

  i++ ;
  sprintf (lb, "%d", i+n+Jti.which6) ;
  Jti.sb[13+z] = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_TEXTCOLOR,         tcolor,
                         Hv_DOMAIN,            Hv_INSIDEHOTRECT,
                         Hv_FILLCOLOR,         trigColor[(i-1)+n],
                         Hv_LABEL,             lb,

			 Hv_USER1,             5,
			 Hv_USER2,             2+n,

                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_HEIGHT,            20,
                         Hv_WIDTH,             20,
                         Hv_SINGLECLICK,       trigSEButton,
                         NULL) ;

  i++ ;
  sprintf (lb, "%d", i+n+Jti.which6) ;
  Jti.sb[14+z] = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_TEXTCOLOR,         tcolor,
                         Hv_DOMAIN,            Hv_INSIDEHOTRECT,
                         Hv_FILLCOLOR,         trigColor[(i-1)+n],
                         Hv_LABEL,             lb,

			 Hv_USER1,             5,
			 Hv_USER2,             3+n,

                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_HEIGHT,            20,
                         Hv_WIDTH,             20,
                         Hv_SINGLECLICK,       trigSEButton,
                         NULL) ;

/* sector - 6 */
  i=1; 
  sprintf (lb, "%d", i+n+Jti.which6) ;
  Jti.sb[15+z] = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_TEXTCOLOR,         tcolor,
                         Hv_FILLCOLOR,         trigColor[(i-1)+n],
                         Hv_DOMAIN,            Hv_INSIDEHOTRECT,
                         Hv_LABEL,             lb,

			 Hv_USER1,             6,
			 Hv_USER2,             1+n,

                         Hv_LEFT,              Item->area->left+58,
                         Hv_TOP,               Item->area->top+262,
                         Hv_HEIGHT,            20,
                         Hv_WIDTH,             20,
                         Hv_SINGLECLICK,       trigSEButton,
                         NULL) ;


  i++ ;
  sprintf (lb, "%d", i+n+Jti.which6) ;
  Jti.sb[16+z] = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_TEXTCOLOR,         tcolor,
                         Hv_FILLCOLOR,         trigColor[(i-1)+n],
                         Hv_DOMAIN,            Hv_INSIDEHOTRECT,
                         Hv_LABEL,             lb,

			 Hv_USER1,             6,
			 Hv_USER2,             2+n,

                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_HEIGHT,            20,
                         Hv_WIDTH,             20,
                         Hv_SINGLECLICK,       trigSEButton,
                         NULL) ;

  i++ ;
  sprintf (lb, "%d", i+n+Jti.which6) ;
  Jti.sb[17+z] = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_TEXTCOLOR,         tcolor,
                         Hv_FILLCOLOR,         trigColor[(i-1)+n],
                         Hv_DOMAIN,            Hv_INSIDEHOTRECT,
                         Hv_LABEL,             lb,

			 Hv_USER1,             6,
			 Hv_USER2,             3+n,

                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_HEIGHT,            20,
                         Hv_WIDTH,             20,
                         Hv_SINGLECLICK,       trigSEButton,
                         NULL) ;


}

/* ------------------------------ TrigDownLoader ------------------------------ */

void
TrigParser()
{
  char bfr[2048];
  char *t, erbuf[80];
  int i, j, k, len, jtrig, jp;
  int sectorNum, TrigErrors=0, SEN, product=0;
  int itrig = 0;
  int value = 1;

  /*printf("TrigParser reached\n");fflush(stdout);*/
  /* Phase3 data variables set to 2 (X)  */
  for(i=0; i<MaxTrig; i++)
  {
    for(j=0; j<MaxTrigProducts; j++)
    {
      /*for(k=0; k<19; k++)Sergey:do not touch EXT, it will go from 'menus.c'*/
      for(k=0; k<18; k++)
      {
        phase3.Trig[i][j][k] = 2;
        JAMphase3.Trig[i][j][k] = 2;
      }
	}

    /* comment out until the widgets are ready
    phase3.Trig_Delay[i] = 1;
    phase3.Enable_Async[i] = 0;*/
  }
  /*printf("1\n");fflush(stdout);*/
  flush_trigr();
  /*printf("2\n");fflush(stdout);*/
  for(jp=0, jtrig=0; jtrig<8; jp++, jtrig+=4)
  {
    for(itrig=0, value=1; itrig<4; itrig++)
    {
      if(Jti.trigString[jp][itrig] == 0) continue;
      init(Jti.trigString[jp][itrig]);
      lex(bfr);
      while(!Compile_Done)
      {
        if(look.ch == '/')
        {
          value = 0;
          skip_op();
          lex(bfr);
        }
        else
	      value = 1;
        lex(bfr); /* lex to find sector number */
        sectorNum = look.num;
printf("TrigParser: sector # %d\n",sectorNum);
        if(sectorNum<1 || sectorNum>6)
        {
          sprintf(erbuf, "error: there are only 1-6 sectors: %d", SEN);
          Hv_Warning(erbuf);
          TrigErrors += 1;
        }

        skip_op(); /* skip the period */
        lex(bfr);
        if(Is_SE ())
        {
          lex (bfr);
          SEN = look.num;
          if(jtrig == 0)
          {
            if(SEN<1 || SEN>6)
            {
              sprintf(erbuf, "error: scint events are: 1-3 : %d", SEN);
              Hv_Warning(erbuf);
              TrigErrors += 1;
            }
            if(SEN>=4) SEN-=3; /* map SE back to range 1-3 */
          }
          else
          {
            if(SEN<7 || SEN>12)
            {
              sprintf (erbuf, "error: scint events are: 6-12 : %d", SEN);
              Hv_Warning (erbuf);
              TrigErrors += 1;
            }
            if(SEN<=9)
              SEN -= 6;
            else /* set this back to range 1-3 */
              SEN -= 9;
          }

          JAMphase3.Trig[((itrig+1)+jtrig)][product][((sectorNum-1)*3)+(SEN-1)]
            = value;
          printf("itrig+1 = %d, prod=%d, sectorNum=%d, sectorNum-1*3=%d, SEN-1=%d\n", ((itrig+1)+jtrig), (product), sectorNum, ((sectorNum-1)*3), (SEN-1));
          if(Compile_Done) break;
          lex(bfr); /* got the operator (*, +) */
          if(Is_or_op())
          {
            product += 1;
            skip_op();
          }
          else 
            if(Is_and_op())
              skip_op();
            else
              TrigErrors += 1;

          lex(bfr) ;
        }
        else
        {
          sprintf(erbuf, "unknown character found in trigger def");
          Hv_Warning(erbuf);
          TrigErrors += 1;
        }
      }
    }
  }

  printf("... done init'ing now copy.\n");fflush(stdout);

  /* Now, set my data in and send to the board. */
  for(itrig = 1; itrig < 9; itrig++)
  {
    for(j=0; j<MaxTrigProducts; j++)
      /*for(k=0; k<19; k++)Sergey:do not touch EXT, it will go from 'menus.c'*/
      for(k=0; k<18; k++)
      {
        phase3.Trig[itrig][j][k] = JAMphase3.Trig[itrig][j][k];
      }
      /*phase3.Trig_Delay[i] = 1;*/

    /* comment out next line until the widget is actually used
    phase3.Enable_Async[i] = JAMphase3.Enable_Async[itrig];*/
  }

  printf ("... done, now you can download\n");fflush(stdout);
}

/* ------------------- Download Trigger --------------- */

void
downloadTrig()
{
  FILE *fp; 
  char *outname = "/usr/local/clas/parms/ts/Current.Config"; 
  
  download_phase3();

  printf("file name = %s",fname);
  if((fp=fopen (outname, "w"))!=0)
  {
    fprintf (fp, "%s\n", fname);
    fprintf(fp, "(only ~trigger/tigris and ~trigger/tigrisLite update this info)\n");
    fclose (fp);
  }
  else
  {
    printf("Error opening file /usr/local/clas/parms/ts/Current.Config");
  }

  printf("... done download\n");
}

#else /* UNIX only */

void
sectbuttons_dummy()
{
}

#endif
