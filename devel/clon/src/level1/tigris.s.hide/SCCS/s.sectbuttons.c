h64235
s 00011/00000/01072
d D 1.3 06/09/01 00:33:51 boiarino 4 3
c dummy VXWORKS
c 
e
s 00370/00261/00702
d D 1.2 02/08/25 19:48:19 boiarino 3 1
c big cleanup (globals, locals, prototypes)
c some change(s) to make async stuff works
c BUG fix:
c   lex() -> lex(string)
e
s 00000/00000/00000
d R 1.2 02/03/29 12:13:03 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/level1/tigris/sectbuttons.c
e
s 00963/00000/00000
d D 1.1 02/03/29 12:13:02 boiarino 1 0
c date and time created 02/03/29 12:13:02 by boiarino
e
u
U
f e 0
t
T
I 3

I 4
#ifndef VXWORKS

E 4
/* sectbuttons.c */

#include <stdio.h>
E 3
I 1
#include <string.h>
I 3
#include <stdlib.h>
E 3
#include "Hv.h"
#include "ced.h"
D 3
#include "simple.h"
#include "def.h"
#include "sgl.h"
/*****************  commented out  **********************
#include <rpc/rpc.h>
#include "/usr/user3/dputnam/trig_test/rpc/hptest.h" 
******************  commented out  **********************/
extern void             init ();
extern int              lex ();
extern void             skip_op ();
extern int              Is_SE ();
extern int              Is_or_op ();
extern int              Is_and_op ();
extern void             download_phase3() ;
E 3
I 3
#include "tigris.h"
E 3

D 3
extern struct phase3_def JAMphase3;
extern struct phase3_def phase3;
E 3
I 3
extern struct Look look; /* defined in cradle.c */
extern int Compile_Done; /* defined in cradle.c */
extern TriggerInfo Jti;  /* defined in setup.c */
extern int trigColor[6]; /* defined in init.c */
extern char *fname;      /* defined in init.c */
extern struct phase3_def JAMphase3; /* defined in fileio.c */
extern struct phase3_def phase3; /* defined in fileio.c */
E 3

D 3
extern TriggerInfo Jti;
E 3

D 3
extern struct Look look;
extern int         Compile_Done;
E 3
I 3
/* local prototypes */
E 3

D 3
extern int scintColor [6];
extern int trigColor [6];
E 3
I 3
static void triggerButton(Hv_Event hevent);
static void PutTextIntoHv(int offs, int but, char *buf);
static void trigSEButton(Hv_Event hevent);
static void flush_trigr();
static void change_triglabels(int n);
E 3

D 3
/********************  commented out  ************************ 
CLIENT *cl;
*********************  commneted out  ***********************/
E 3

D 3
extern char *server;
E 3

D 3
/* ----------------------- triggerButton ------------------- */
E 3

D 3
void triggerButton (hevent)
E 3

D 3
Hv_Event hevent ;
E 3
I 3


/* TEMPORARY */
/* see Hv_data.h for 'Hv_Item' and 'Hv_String' structure */

static void
my_UpdateTextItem(Hv_Item Item)
E 3
{
I 3
  Hv_Region   viewreg, areareg, totreg;
  short       extra;
  Hv_Rect     area;
E 3

D 3
 /* User pressed either and || or || not || clr */
  printf ("trigger button push!\n") ;
  
E 3
I 3
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
E 3

I 3
char *
my_GetItemText(Hv_Item Item)    
{
printf("my_GetItemText reached\n");

  if((Item == NULL) || (Item->str == NULL)) return NULL;
  return(Item->str->text);
E 3
}

I 3
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

E 3
/* ------------------ trigAndButton --------------------- */

D 3
void trigAndButClick (hevent)
Hv_Event hevent;
E 3
I 3
void
trigAndButClick(Hv_Event hevent)
E 3
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
I 3

E 3
/* ------------------ trigOrButton --------------------- */

D 3
void trigOrButClick (hevent)
Hv_Event hevent;
E 3
I 3
void
trigOrButClick(Hv_Event hevent)
E 3
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

D 3
  t = Hv_GetItemText (Jti.titem[index]) ;
  len = strlen (t);
  strncpy (buf, t, len);
  buf[len-1] = '+' ;
E 3
I 3
  t = my_GetItemText(Jti.titem[index]);
  len = strlen(t);
  strncpy(buf,t,len);
  buf[len-1] = '+';
  buf[len] = 0;
E 3

D 3
  buf[len] = 0x00;
E 3
I 3
  my_SetItemText(Jti.titem[index],buf);
  Hv_DrawTextItem(Jti.titem[index],NULL);
E 3

D 3
  Hv_SetItemText (Jti.titem[index], buf) ;
  Hv_DrawItem (Jti.titem[index], NULL);

E 3
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

D 3
void trigNotButClick (hevent)
Hv_Event hevent;
E 3
I 3
void
trigNotButClick(Hv_Event hevent)
E 3
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

D 3
void trigClrButClick (hevent)
Hv_Event hevent;
E 3
I 3
void
trigClrButClick(Hv_Event hevent)
E 3
{
  int index;
  char buf[1024];
D 3
  int i,j;
E 3
I 3
  int i, j;
E 3
  
D 3
  if (hevent->item->user1 == 1) {
E 3
I 3
  if(hevent->item->user1 == 1)
  {
E 3
    index = 0 + Jti.butSet1 ;
    j=0;
D 3
  } else {
E 3
I 3
  }
  else
  {
E 3
    index = 2 + Jti.butSet2;
    j=1;
  }

  buf[0] = ' ';
D 3
  buf[1] = 0x00;
E 3
I 3
  buf[1] = 0;
E 3

D 3
  for (i=0; i<6; i++)
    Jti.can_not_do[index][j][i] = 0;
E 3
I 3
  for(i=0; i<6; i++) Jti.can_not_do[index][j][i] = 0;
E 3

D 3
  Hv_SetItemText (Jti.titem[index], buf) ;
  Hv_DrawItem (Jti.titem[index], NULL);
E 3
I 3
  my_SetItemText(Jti.titem[index],buf);
  Hv_DrawTextItem(Jti.titem[index],NULL);
E 3

}
I 3

E 3
/* ----------------- a utility ------------------------ */
D 3
void PutTextIntoHv (int offs, int but, char *buf)
E 3
I 3
/* offs=0 or 2, but=0 or 1, they are all window number from 0 to 3 */
/* 'buf' contains string to be drawn */ 
static void
PutTextIntoHv(int offs, int but, char *buf)
E 3
{
  char *t;

D 3
  t = Hv_GetItemText(Jti.titem[ offs + but ]);
  if (t == 0) 
    Hv_SetItemText (Jti.titem[ offs + but ], buf);
E 3
I 3
  t = my_GetItemText(Jti.titem[offs+but]);
  if(t == 0)
    my_SetItemText(Jti.titem[offs+but],buf);
E 3
  else
D 3
    Hv_ConcatTextToTextItem (Jti.titem[ offs + but ], buf) ;
  Jti.operator [offs + but] = '*';
  Hv_DrawItem (Jti.titem[ offs + but ], NULL);
E 3
I 3
    my_ConcatTextToTextItem(Jti.titem[offs+but],buf);

  Jti.operator[offs+but] = '*';
printf("PISHEM TEXT ...\n");
  Hv_DrawTextItem(Jti.titem[offs+but],NULL);
E 3
}
I 3

E 3
/* ------------------ trigSeButton --------------------- */

D 3
void trigSEButton (hevent)
Hv_Event    hevent ;
E 3
I 3
static void
trigSEButton(Hv_Event hevent)
E 3
{
  int offs;
D 3
  char *t, buf[80] ;
E 3
I 3
  char *t, buf[80];
E 3

D 3
  printf ("button %d   sector %d\n", hevent->item->user2, hevent->item->user1) ;
E 3
I 3
  printf("button %d   sector %d\n", hevent->item->user2, hevent->item->user1);
E 3

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
D 3
      PutTextIntoHv (0, Jti.butSet1, buf);
E 3
I 3
      PutTextIntoHv(0,Jti.butSet1,buf);
E 3
      Hv_InvertColorScheme (Jti.bNot1->scheme) ;
      Hv_DrawItem (Jti.bNot1, NULL);

    } else {
      if (Jti.operator [ 0 + Jti.butSet1 ] == '+') {
	sprintf (buf, " S%d.SE%d %c", hevent->item->user1, (hevent->item->user2+Jti.which6), '*');
	Hv_InvertColorScheme (Jti.bOr1->scheme);
	Hv_DrawItem (Jti.bOr1, NULL);

	Hv_InvertColorScheme (Jti.bAnd1->scheme);
	Hv_DrawItem (Jti.bAnd1, NULL);
D 3
	PutTextIntoHv (0,  Jti.butSet1, buf);
E 3
I 3
	PutTextIntoHv(0,Jti.butSet1,buf);
E 3

      } else {
D 3
	sprintf (buf, " S%d.SE%d %c", hevent->item->user1, (hevent->item->user2+Jti.which6), '*');
	PutTextIntoHv (0, Jti.butSet1, buf);

E 3
I 3
	sprintf(buf," S%d.SE%d %c",
      hevent->item->user1,(hevent->item->user2+Jti.which6), '*');
	PutTextIntoHv(0,Jti.butSet1,buf);
E 3
      }
D 3

E 3
    } 
D 3

E 3
  } else {
D 3
    t = Hv_GetItemText (Jti.titem[ 2 + Jti.butSet2]);
E 3
I 3
    t = my_GetItemText(Jti.titem[ 2 + Jti.butSet2]);
E 3
    if (Jti.operator[2+Jti.butSet2] == '/') {
      sprintf (buf, " %cS%d.SE%d %c", Jti.operator[2+Jti.butSet2], hevent->item->user1, (hevent->item->user2+Jti.which6), '*');
      Hv_InvertColorScheme (Jti.bAnd2->scheme) ;
      Hv_DrawItem (Jti.bAnd2, NULL);

      Hv_InvertColorScheme (Jti.bNot2->scheme) ;
      Hv_DrawItem (Jti.bNot2, NULL);

D 3
      PutTextIntoHv (2, Jti.butSet2, buf);
      /* Hv_ConcatTextToTextItem (Jti.titem[ 2 + Jti.butSet2 ], buf) ; */
E 3
I 3
      PutTextIntoHv(2,Jti.butSet2,buf);
      /* my_ConcatTextToTextItem (Jti.titem[ 2 + Jti.butSet2 ], buf) ; */
E 3
      Jti.operator [2+Jti.butSet2] = '*';
    } else
      if (Jti.operator [ 2 + Jti.butSet2 ] == '+') {
	sprintf (buf, " S%d.SE%d %c", hevent->item->user1, (hevent->item->user2+Jti.which6), '*');
	Hv_InvertColorScheme (Jti.bOr2->scheme);
	Hv_DrawItem (Jti.bOr2, NULL);

	Hv_InvertColorScheme (Jti.bAnd2->scheme);
	Hv_DrawItem (Jti.bAnd2, NULL);

D 3
	PutTextIntoHv (2, Jti.butSet2, buf);
	/* Hv_ConcatTextToTextItem (Jti.titem[ 2 + Jti.butSet2 ], buf) ;     */
E 3
I 3
	PutTextIntoHv(2,Jti.butSet2,buf);
	/* my_ConcatTextToTextItem (Jti.titem[ 2 + Jti.butSet2 ], buf) ;     */
E 3
	Jti.operator [2+Jti.butSet2] = '*';
      } else {
	sprintf (buf, " S%d.SE%d %c", hevent->item->user1, (hevent->item->user2+Jti.which6), '*');
D 3
	PutTextIntoHv (2, Jti.butSet2, buf);
	/* Hv_ConcatTextToTextItem (Jti.titem[ 2 + Jti.butSet2 ], buf) ; */
E 3
I 3
	PutTextIntoHv(2,Jti.butSet2,buf);
	/* my_ConcatTextToTextItem (Jti.titem[ 2 + Jti.butSet2 ], buf) ; */
E 3
	Jti.operator [2+Jti.butSet2] = '*';
      }
  }


}
I 3

E 3
/* ------------------ flush trigger text items --------------------- */
D 3
void flush_trigr()
E 3
I 3

static void
flush_trigr()
E 3
{
  int itrig;
  char *t;

D 3
  for (itrig=0; itrig < 4; itrig++) {
    t = Hv_GetItemText (Jti.titem[itrig]) ;
    if (t == NULL) continue;
E 3
I 3
  for(itrig=0; itrig<4; itrig++)
  {
    t = my_GetItemText(Jti.titem[itrig]);
    if(t == NULL) continue;
E 3

D 3
    if (Jti.trigString[Jti.rstate][itrig])
      free (Jti.trigString[Jti.rstate][itrig]);
E 3
I 3
    if(Jti.trigString[Jti.rstate][itrig])
      free(Jti.trigString[Jti.rstate][itrig]);
E 3

D 3
    Jti.trigString[Jti.rstate][itrig] = strdup (t);
E 3
I 3
    Jti.trigString[Jti.rstate][itrig] = strdup(t);
    if(Jti.trigString[Jti.rstate][itrig] == NULL)
      printf("flush_trigr: ERROR: strdup returns 0\n");
E 3
  }
}
I 3

E 3
/* ---------------- put new text into trigger text items ------------------ */
D 3
void change_triglabels (n)
int n;  /* n is either 0 or 6 */
E 3
I 3
/* n is either 0 or 6 */
static void
change_triglabels(int n)
E 3
{
  int i, j, k;
  char str[2048];

D 3
  for (k=1, j=0; j<18; j++) {
    sprintf (str, "%d", (k+n));
    Hv_SetItemText (Jti.sb[j], str);
    Hv_DrawItem (Jti.sb[j], NULL);
    if (++k > 3) k = 1;
E 3
I 3
  for(k=1, j=0; j<18; j++)
  {
    sprintf(str,"%d",(k+n));
    my_SetItemText(Jti.sb[j],str);
    Hv_DrawItem (Jti.sb[j],NULL);
    if(++k > 3) k = 1;
E 3
  }

D 3

  for (k=4, j=18; j<36; j++) {
    sprintf (str, "%d", (k+n));
    Hv_SetItemText (Jti.sb[j], str);
    Hv_DrawItem (Jti.sb[j], NULL);
    if (++k > 6) k=4;
E 3
I 3
  for(k=4, j=18; j<36; j++)
  {
    sprintf(str,"%d",(k+n));
    my_SetItemText(Jti.sb[j],str);
    Hv_DrawItem(Jti.sb[j],NULL);
    if(++k > 6) k=4;
E 3
  }
D 3

E 3
} 
I 3

E 3
/* ---------------- put new text into trigger text items ------------------ */
D 3
void refresh_trigr (zzz)
int zzz;
E 3
I 3

void
refresh_trigr(int zzz)
E 3
{
  int it;

D 3
  for (it=0; it < 4; it++) {
    Hv_SetItemText (Jti.titem[it], Jti.trigString [zzz][it]) ;
    Hv_DrawItem (Jti.titem[it], NULL);
E 3
I 3
  for(it=0; it<4; it++)
  {
    my_SetItemText(Jti.titem[it], Jti.trigString[zzz][it]);
    Hv_DrawTextItem(Jti.titem[it], NULL);
E 3
  }
D 3


E 3
}
I 3

E 3
/* ------------------ SERadioClick --------------------- */

D 3
void SERadioClick (hevent)
Hv_Event     hevent ;
E 3
I 3
void
SERadioClick(Hv_Event hevent)
E 3
{
  int z;

  z = Hv_HandleRadioButton (hevent->item) ;
D 3
  if (hevent->item->user1 == 2) {
    flush_trigr () ;
E 3
I 3
  if(hevent->item->user1 == 2)
  {
    flush_trigr();
E 3
    Jti.rstate = 1;
D 3
    Jti.which6 = 6 ;
    change_triglabels (6);
    refresh_trigr (Jti.rstate);
  } else {
    flush_trigr ();
E 3
I 3
    Jti.which6 = 6;
    change_triglabels(6);
    refresh_trigr(Jti.rstate);
  }
  else
  {
    flush_trigr();
E 3
    Jti.rstate = 0;
    Jti.which6 = 0;
D 3
    change_triglabels (0);
    refresh_trigr (Jti.rstate);
E 3
I 3
    change_triglabels(0);
    refresh_trigr(Jti.rstate);
E 3
  }
D 3
  printf ("SERadio Click: user1 = %d\n", hevent->item->user1);
E 3
I 3
  printf("SERadio Click: user1 = %d\n", hevent->item->user1);
E 3
}

/* ------------------ DrawTrigButtons ------------------ */
D 3

void DrawTrigButtons (View, Item, n)
Hv_View     View ;
Hv_Item     Item ;
int         n;        /* index for 1-3 or 4-6 */
E 3
I 3
/* n is index for 1-3 or 4-6 */
void
DrawTrigButtons(Hv_View View, Hv_Item Item, int n)
E 3
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

D 3
void TrigParser ()
E 3
I 3
void
TrigParser()
E 3
{
D 3
char *t, erbuf[80];
int  i,j, k, len, jtrig, jp;
int sectorNum, TrigErrors=0, SEN, product=0 ;
int itrig = 0;
int value = 1;
E 3
I 3
  char bfr[2048];
  char *t, erbuf[80];
  int i, j, k, len, jtrig, jp;
  int sectorNum, TrigErrors=0, SEN, product=0;
  int itrig = 0;
  int value = 1;
E 3

I 3
  /*printf("TrigParser reached\n");fflush(stdout);*/
E 3
  /* Phase3 data variables set to 2 (X)  */
D 3
  for (i=0;i<MaxTrig;i++)
E 3
I 3
  for(i=0; i<MaxTrig; i++)
E 3
  {
D 3
    for (j=0;j<MaxTrigProducts;j++)
      for (k=0;k<19;k++) {
	phase3.Trig[i][j][k] = 2;
	JAMphase3.Trig[i][j][k] = 2;
E 3
I 3
    for(j=0; j<MaxTrigProducts; j++)
    {
      /*for(k=0; k<19; k++)Sergey:do not touch EXT, it will go from 'menus.c'*/
      for(k=0; k<18; k++)
      {
        phase3.Trig[i][j][k] = 2;
        JAMphase3.Trig[i][j][k] = 2;
E 3
      }
I 3
	}
E 3

D 3
/* comment out until the widgets are ready */

/*    phase3.Trig_Delay[i] = 1;
E 3
I 3
    /* comment out until the widgets are ready
    phase3.Trig_Delay[i] = 1;
E 3
    phase3.Enable_Async[i] = 0;*/
  }
D 3
  flush_trigr () ;
  for (jp=0, jtrig=0; jtrig<8; jp++, jtrig+=4) {
    for (itrig=0, value = 1; itrig < 4; itrig++) {
      if (Jti.trigString[jp][itrig] == 0)
	continue;
      init (Jti.trigString[jp][itrig]) ;
      
      lex () ;
      while (!Compile_Done) {
	if (look.ch == '/') {
	  value = 0;
	  skip_op ();
	  lex ();
	} else
	  value = 1;
	lex () ; /* lex to find sector number */
	sectorNum = look.num;
	
	if (sectorNum < 1 || sectorNum > 6){
	  sprintf (erbuf, "error: there are only 1-6 sectors: %d", SEN);
	  Hv_Warning (erbuf);
	  TrigErrors += 1;
	}
	skip_op ();  /* skip the period */
	lex ();
	if (Is_SE ()) {
	  lex ();
	  SEN = look.num;
	  if (jtrig == 0) {
	    if (SEN < 1 || SEN > 6) {
	      sprintf (erbuf, "error: scint events are: 1-3 : %d", SEN);
	      Hv_Warning (erbuf);
	      TrigErrors += 1;
	    }
	    if (SEN >= 4) SEN -= 3;   /* map SE back to range 1-3 */
	  } else {
	    if (SEN < 7 || SEN > 12) {
	      sprintf (erbuf, "error: scint events are: 6-12 : %d", SEN);
	      Hv_Warning (erbuf);
	      TrigErrors += 1;
	    }
	    if (SEN <= 9)
	      SEN -= 6;
	    else        /* set this back to range 1-3 */
	      SEN -= 9;
	  }
	  JAMphase3.Trig[((itrig+1)+jtrig)][product][ ((sectorNum-1)*3) + (SEN-1) ] = value;
	  printf ("itrig+1 = %d, prod=%d, sectorNum=%d, sectorNum-1*3=%d, SEN-1=%d\n", ((itrig+1)+jtrig), (product), sectorNum, ((sectorNum-1)*3), (SEN-1));
	  if (Compile_Done) break;
	  lex ();     /* got the operator (*, +) */
	  if (Is_or_op ()) {
	    product += 1;
	    skip_op ();
	  } else 
	    if (Is_and_op ())
	      skip_op ();
	    else
	      TrigErrors += 1;
	  
	  lex () ;
	} else {
	  sprintf (erbuf, "unknown character found in trigger def");
	  Hv_Warning (erbuf);
	  TrigErrors += 1;
	}
E 3
I 3
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
E 3
      }
    }
  }

D 3
  printf ("... done init'ing now copy.\n");
  /* Now, set my data in and send to the board. */
E 3
I 3
  printf("... done init'ing now copy.\n");fflush(stdout);
E 3

D 3

  for (itrig = 1; itrig < 9; itrig++) {
    for (j=0; j<MaxTrigProducts; j++)
      for (k=0; k<19; k++) {
	phase3.Trig[itrig][j][k] = JAMphase3.Trig[itrig][j][k];
E 3
I 3
  /* Now, set my data in and send to the board. */
  for(itrig = 1; itrig < 9; itrig++)
  {
    for(j=0; j<MaxTrigProducts; j++)
      /*for(k=0; k<19; k++)Sergey:do not touch EXT, it will go from 'menus.c'*/
      for(k=0; k<18; k++)
      {
        phase3.Trig[itrig][j][k] = JAMphase3.Trig[itrig][j][k];
E 3
      }
D 3
    /*  phase3.Trig_Delay[i] = 1;   */
E 3
I 3
      /*phase3.Trig_Delay[i] = 1;*/
E 3

D 3
/* comment out next line until the widget is actually used */
/*    phase3.Enable_Async[i] = JAMphase3.Enable_Async[itrig];*/
E 3
I 3
    /* comment out next line until the widget is actually used
    phase3.Enable_Async[i] = JAMphase3.Enable_Async[itrig];*/
E 3
  }

D 3
  
  printf("SPB SPB SPB SPB SPB SPB");
  /*  fp=fopen ("/usr/local/clas/parms/ts/Current.Config", "w");
  fprintf (fp, "%s", fname1);
  fclose (fp); */


  printf ("... done, now download\n");
E 3
I 3
  printf ("... done, now you can download\n");fflush(stdout);
E 3
}

/* ------------------- Download Trigger --------------- */

D 3
void downloadTrig ()
E 3
I 3
void
downloadTrig()
E 3
{
D 3

/*  char                   *outname = "test.config"; */
   char                   *outname = "/usr/local/clas/parms/ts/Current.Config"; 
  
  extern char * fname;
E 3
  FILE *fp; 
I 3
  char *outname = "/usr/local/clas/parms/ts/Current.Config"; 
  
  download_phase3();
E 3

D 3
/*
  if ((cl = clnt_create (server, HPTESTPROG, HPTESTVERS, "tcp")) == NULL) {
    clnt_pcreateerror(server);
    fprintf(stderr,"Can't contact server on %s.\n",server);
    exit(1);
  }
  clnt_destroy (cl) ;
*/

/*
  printf ("########################################\n");
  printf ("########################################\n");
  printf ("########################################\n");
  printf ("########################################\n");
  for (i=0; i<MaxTrigProducts; i++) {
    for (j=0; j<19; j++) {
      printf (" %d ", JAMphase3.Trig[1][i][j]);
    }
    printf ("\n");
  }
*/
  download_phase3 () ;

  /*  printf("SPB SPB SPB SPB SPB SPB\n"); */
E 3
  printf("file name = %s",fname);
D 3

  if ((fp=fopen (outname, "w"))!=0) {
      fprintf (fp, "%s\n", fname);
      fprintf(fp, "(only ~trigger/tigris and ~trigger/tigrisLite update this info)\n");
      fclose (fp);
E 3
I 3
  if((fp=fopen (outname, "w"))!=0)
  {
    fprintf (fp, "%s\n", fname);
    fprintf(fp, "(only ~trigger/tigris and ~trigger/tigrisLite update this info)\n");
    fclose (fp);
E 3
  }
D 3
  else {
      printf("Error opening file /usr/local/clas/parms/ts/Current.Config");
E 3
I 3
  else
  {
    printf("Error opening file /usr/local/clas/parms/ts/Current.Config");
E 3
  }

D 3
  printf ("... done download\n");

E 3
I 3
  printf("... done download\n");
E 3
}
I 4

#else /* UNIX only */

void
sectbuttons_dummy()
{
}

#endif
E 4
E 1
