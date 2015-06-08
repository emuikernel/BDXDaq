
#ifndef VXWORKS

/* evbutton.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "Hv.h"
#include "ced.h"
#include "tigris.h"

char buf[SIZEOBUF]; /* global */

extern char *JJ_SEtextItem[6][12]; /* defined in setup.c */
extern char *SECommentText[6][12]; /* defined in setup.c */

extern struct Look look; /* defined in cradle.c */
extern int Compile_Done; /* defined in cradle.c */
extern VisSectorDef secdef[6]; /* defined in init.c */
extern int SDbits4sgl[6][2][96]; /* defined in scintbuts.c */
extern int SPbits4sgl[6][2][64]; /* defined in scintbuts.c */
extern int EDbits4sgl[6][2][96]; /* defined in scintbuts.c */
extern int scintColor[6]; /* defined in scintillator.c */
extern int  trigColor[6]; /* defined in init.c */
extern struct SLsecdef Sector[7]; /* defined in fileio.c */
extern Hv_View view[7]; /* defined in init.c */


/* local variables */

static char operator = '*';
static char *erbuf[100];
static int product1 = 0;
static Hv_Item evbox;
static Hv_Widget SEPDialog = NULL;
static Hv_Widget SEPtext = NULL;
static Hv_TextPosition tpos = 0;
static int FileOnlyMode = 0;

/* local prototypes */

static void EV_ButtonCCClick(Hv_Event hevent);
static void EV_ButtonFCClick(Hv_Event hevent);
static void EV_ButtonSPClick(Hv_Event hevent);
static void EV_ButtonSDClick(Hv_Event hevent);
static void EV_ButtonAndClick(Hv_Event hevent);
static void EV_ButtonOrClick(Hv_Event hevent);
static void EV_ButtonNotClick(Hv_Event hevent);
static void EV_ButtonClrClick(Hv_Event hevent);
static void EV_ButtonChkClick(Hv_Event hevent);
static void add2error(int se, char *s);
static void StuffSEPtext(char *line);
static void InitSEParseDlog(void);
static int  SEparse(int jb, int ja, int product, char *t);
static void setuptheSElabels(Hv_View View, int offset);
static void label_the_se(Hv_View View, int offset);


/* --------------------------------------------------------- */

int
EV_case(int z, int j, char *flag)
{
  char *letters;
  int r=0;

  switch (j) {
  case 0:
  case 1:
  case 2:
  case 3:
  case 4:
  case 5: { letters = "SD"; break; }

  case 6:
  case 7:
  case 8:
  case 9: { letters = "SP" ; j -= 6; break ; }

  case 10:
  case 11:
  case 12: { letters = "FC"; j-= 10; break ; }

  case 13:
  case 14:
  case 15: { letters = "CC"; j -= 13; break ; }
  default:
    {letters = "IDONTKNOW" ; break ; }
  }

  /*printf("letters = %s\n",letters);*/
  switch (z) {
  case 0:
    if (*flag == ' ')
      sprintf (buf, "%s%c%s%d", buf, '/',  letters, (j+1)) ;
    else
    if (*flag == '*')
      sprintf (buf, "%s * %c%s%d", buf, '/',  letters, (j+1)) ;
    else
    if (*flag == '+')
      sprintf (buf, "%s + %c%s%d", buf, '/',  letters, (j+1)) ;
    else
      sprintf (buf, "%s%c%s%d", buf, '/',  letters, (j+1)) ;
    r=1;
    break;
    
  case 1:
    if (*flag == ' ')
      sprintf (buf, "%s%s%d", buf,  letters, (j+1)) ;
    else
    if (*flag == '*')
      sprintf (buf, "%s * %s%d", buf,  letters, (j+1)) ;
    else
    if (*flag == '+')
      sprintf (buf, "%s + %s%d", buf,  letters, (j+1)) ;
    else
      sprintf (buf, "%s%s%d", buf,  letters, (j+1)) ;
    r=1;
    break;
    
  case 2:
  default:
    break;
  }
  return r;
}

/* ------------ EV_ButtonCCClick --------------- */

void
EV_ButtonCCClick(Hv_Event hevent)
{
  ViewData  vdata = (ViewData)hevent->view->data ;
  char *t;

  if(vdata->activeTextItem == NULL)
  {
    printf("application error: no activeTextItem\n");
    exit(0);
  }
  if (operator == '/') {
    sprintf (buf, "%cCC%d %c ", '/', (int)hevent->item->data, '*') ;
    Hv_ToggleButton (vdata->butNot) ;
    Hv_ToggleButton (vdata->butAnd) ;
    operator = '*' ;
  } else if (operator == '+') {
    Hv_ToggleButton (vdata->butAnd) ;
    operator = '*' ;
    sprintf (buf, "CC%d %c ", (int)hevent->item->data, operator) ;
  } else
    sprintf (buf, "CC%d %c ", (int)hevent->item->data, operator) ;

  printf("%s\n",buf);

  t = Hv_GetItemText (vdata->activeTextItem) ;
  if (t == NULL) 
    Hv_SetItemText (vdata->activeTextItem, buf);
  else
    Hv_ConcatTextToTextItem (vdata->activeTextItem, buf) ;

  Hv_DrawItem (vdata->activeTextItem, NULL);

}

/* ------------ EV_ButtonEDClick --------------- */

void
EV_ButtonFCClick(Hv_Event hevent)
{
  ViewData  vdata = (ViewData)hevent->view->data ;
  char *t;

  if(vdata->activeTextItem == NULL)
  {
    printf ("application error: no activeTextItem\n");
    exit(0);
  }
  if (operator == '/') {
    sprintf (buf, "%cFC%d %c ", '/', (int)hevent->item->data, '*') ;
    Hv_ToggleButton (vdata->butNot) ;
    Hv_ToggleButton (vdata->butAnd) ;
    operator = '*' ;
  } else if (operator == '+') {
    Hv_ToggleButton (vdata->butAnd) ;
    operator = '*' ;
    sprintf (buf, "FC%d %c ", (int)hevent->item->data, operator) ;
  } else
    sprintf (buf, "FC%d %c ", (int)hevent->item->data, operator) ;

  printf ("%s\n", buf);

  t = Hv_GetItemText (vdata->activeTextItem) ;
  if (t == NULL) 
    Hv_SetItemText (vdata->activeTextItem, buf);
  else
    Hv_ConcatTextToTextItem (vdata->activeTextItem, buf) ;

  Hv_DrawItem (vdata->activeTextItem, NULL);

}

/* ------------ EV_ButtonSPClick --------------- */

void
EV_ButtonSPClick(Hv_Event hevent)
{
  ViewData  vdata = (ViewData)hevent->view->data ;
  char *t;

  if (vdata->activeTextItem == NULL) {
    printf ("application error: no activeTextItem\n");
    exit (00);
  }
  if (operator == '/') {
    sprintf (buf, "%cSP%d %c ", '/', (int)hevent->item->data, '*') ;
    Hv_ToggleButton (vdata->butNot) ;
    Hv_ToggleButton (vdata->butAnd) ;
    operator = '*' ;
  } else if (operator == '+') {
    Hv_ToggleButton (vdata->butAnd) ;
    operator = '*' ;
    sprintf (buf, "SP%d %c ", (int)hevent->item->data, operator) ;
  } else
    sprintf (buf, "SP%d %c ", (int)hevent->item->data, operator) ;

  printf ("%s\n", buf);

  t = Hv_GetItemText (vdata->activeTextItem) ;
  if (t == NULL) 
    Hv_SetItemText (vdata->activeTextItem, buf);
  else
    Hv_ConcatTextToTextItem (vdata->activeTextItem, buf) ;

  Hv_DrawItem (vdata->activeTextItem, NULL);

}

/* ------------ EV_ButtonSDClick --------------- */

void
EV_ButtonSDClick(Hv_Event hevent)
{
  ViewData  vdata = (ViewData)hevent->view->data ;
  char *t;

  if (vdata->activeTextItem == NULL) {
    printf ("application error: no activeTextItem\n");
    exit (00);
  }
  if (operator == '/') {
    sprintf (buf, "%cSD%d %c ", '/', (int)hevent->item->data, '*') ;
    Hv_ToggleButton (vdata->butNot) ;
    Hv_ToggleButton (vdata->butAnd) ;
    operator = '*' ;
  } else if (operator == '+') {
    Hv_ToggleButton (vdata->butAnd) ;
    operator = '*' ;
    sprintf (buf, "SD%d %c ", (int)hevent->item->data, operator) ;
  } else
    sprintf (buf, "SD%d %c ", (int)hevent->item->data, operator) ;

  printf ("%s\n", buf);

  t = Hv_GetItemText (vdata->activeTextItem) ;
  if (t == NULL) 
    Hv_SetItemText (vdata->activeTextItem, buf);
  else
    Hv_ConcatTextToTextItem (vdata->activeTextItem, buf) ;

  Hv_DrawItem (vdata->activeTextItem, NULL);
}

/* --------------- EV_ButtonAndClick --------------- */

void
EV_ButtonAndClick(Hv_Event hevent)
{
  ViewData  vdata = (ViewData)hevent->view->data ;

  switch (operator) {
    case '/' : Hv_ToggleButton (vdata->butNot) ; break ;
  }
  Hv_ToggleButton (vdata->butAnd) ;
  operator = '*' ;
}


/* -------------- EV_ButtonOrClick ------------ */

void
EV_ButtonOrClick(Hv_Event hevent)
{
  char *t ;
  int l, j;
  ViewData  vdata = (ViewData)hevent->view->data ;

  t = Hv_GetItemText (vdata->activeTextItem) ;

  if (t == 0) return ;
  l = strlen (t) ;
  strncpy (buf, t, l) ;
  buf[l+1] = 0x00 ;
  for (j=l; j>0; j--) 
    if (buf[j] == '*')
      break;
  if (j == 0) {
    sprintf (buf, "[%s] badly formed equation?", buf);
    Hv_Warning (buf);
  }
  else
    buf[j] = '+' ;

  Hv_ToggleButton (vdata->butOr) ;

  Hv_SetItemText (vdata->activeTextItem, buf) ;
  Hv_DrawItem (vdata->activeTextItem, NULL);

  printf ("or button -- %s \n", buf) ;
  switch (operator) {
    case '*' : { Hv_ToggleButton (vdata->butAnd) ; break ; }
    case '/' : { Hv_ToggleButton (vdata->butNot) ; break ; }
  }
  Hv_ToggleButton (vdata->butOr) ;

  operator = '+' ;
}

/* ---------------- EV_ButtonNotClick ----------------- */

void
EV_ButtonNotClick(Hv_Event hevent)
{
  ViewData  vdata = (ViewData)hevent->view->data ;

  switch (operator) {
    case '*' : 
    case '+':
      { Hv_ToggleButton (vdata->butAnd) ; break ; }
  }
  Hv_ToggleButton (vdata->butNot) ;
  operator = '/' ;
}

/* --------------- Ev_ButtonClrClick ------------ */

void
EV_ButtonClrClick(Hv_Event hevent)
{
  char *t;
  ViewData  vdata = (ViewData)hevent->view->data ;

  Hv_ToggleButton (vdata->butClr) ;

  t = Hv_GetItemText (vdata->activeTextItem) ;
  if (t != NULL) {
    Hv_SetItemText (vdata->activeTextItem, NULL) ;
    Hv_DrawItem (vdata->activeTextItem, NULL) ;
  }
  else
    Hv_DrawItem (vdata->activeTextItem, NULL) ;

  Hv_ToggleButton (vdata->butClr) ;

  operator = '*' ;
}

/* --------------- Ev_ButtonChkClick ------------ */

void
EV_ButtonChkClick(Hv_Event hevent)
{
  ViewData  vdata = (ViewData)hevent->view->data ;

  Hv_ToggleButton (vdata->butChk) ;

  movSE2sgl();

  Hv_ToggleButton (vdata->butChk) ;

  operator = '*' ;
}

/* -------------------------- flush_those_textItems -------------------- */

void
flush_those_textItems(ViewData vdata)
{
  int j;
  char *t;

  for (j=0; j<6; j++) {
    t = Hv_GetItemText (vdata->eitem[j]);
    if (t != 0) {
      if (JJ_SEtextItem[vdata->sectorNumber][vdata->ButState6_12+j] != 0)
	free (JJ_SEtextItem[vdata->sectorNumber][vdata->ButState6_12+j]);
      
      JJ_SEtextItem[vdata->sectorNumber][vdata->ButState6_12+j] = strdup (t);
      /*printf ("butState[%d] : JJ_text[%s]\n", vdata->ButState6_12, 
	      JJ_SEtextItem[vdata->sectorNumber][vdata->ButState6_12+j]); */
    }
    else {
      if (JJ_SEtextItem[vdata->sectorNumber][vdata->ButState6_12+j])
	free (JJ_SEtextItem[vdata->sectorNumber][vdata->ButState6_12+j]) ;
      JJ_SEtextItem[vdata->sectorNumber][vdata->ButState6_12+j] = 0;
    }
  }
}

/* ------------------ SERadioClick --------------------- */
/*
 * this does a lot. 
 *
 * for each click of the radio button ...
 *   change the phase if needed
 *   relabel the SE labels
 *   flush the text items out to the underlying data struct
 *   Draw the text item contents from the possibly new pointer.
 *   Draw DaveBits for scintillators...
 *   Draw DaveBits for spares...
 *   Draw DaveBits for extra data...
 */

void
EVSERadioClick(Hv_Event hevent)
{
  int j, z;
  static int k=0;  /* k was ment to toggle, 
		    * I start at zero (0). 
		    */
  ViewData  vdata = (ViewData)hevent->view->data ;
  char *t;

  z = (short)Hv_HandleRadioButton (hevent->item) ;
  if (z == 1) {
    if (k != 0) 
      if (++vdata->sglphase > 1) vdata->sglphase=0;
    k = 0;
  }
  else
    if (z == 2) {
      if (k != 6) 
	if (++vdata->sglphase > 1) vdata->sglphase=0;
      k = 6;
    }

  printf ("EVSE: %d\n", hevent->item->user1);

  label_the_se ( (Hv_View)hevent->view, k+1);
  flush_those_textItems (vdata);
  vdata->ButState6_12 = k;

  for (j=0; j<6; j++) {
    Hv_SetItemText (vdata->eitem[j], JJ_SEtextItem[vdata->sectorNumber][vdata->ButState6_12+j]);
    Hv_DrawItem (vdata->eitem[j], NULL);
  }
  DrawAllScintItems (hevent->view);
  Hv_DrawItem (vdata->ScintDFront16bits, NULL);
  Hv_DrawItem (vdata->ScintDBack32bits, NULL);

  Hv_activeTextItem = vdata->eitem[0];

  vdata->OldCommentIndex = k ;  
  Hv_SetItemText (vdata->evcommentbox, 
		  SECommentText[vdata->sectorNumber][k]) ;

  Hv_DrawItem (vdata->evcommentbox, NULL);
  
  Hv_DrawItem (vdata->SpareDataBits, NULL);
  Hv_DrawItem (vdata->ExtraDataBits, NULL);

  printf ("EVSE: %d\n", hevent->item->user1);
  printf ("BS6_12: %d\n", vdata->ButState6_12);
  printf ("sglph: %d\n", vdata->sglphase) ;
}

/* -------------- mov2sgl  ---------------*/

void
mov2sgl(int nphase)
{
  int jamBit, j, k, sector;

  /* 1st init sgl struct */
  for (sector=0; sector<7; sector++) {
    for (k=0; k<6; k++) {
      for (jamBit=0; jamBit<33; jamBit++) {
	Sector[sector].SD[nphase][k][jamBit] = 0;
	Sector[sector].ED[nphase][k][jamBit] = 0;
      }
    }
    for (k=0; k<4; k++)
      for (jamBit=0; jamBit<17; jamBit++)
	Sector[sector].SP[nphase][k][jamBit] = 0;
  }
  /*
   * After sgl has been init'ed copy my bits into sgl.
   * for the front scintillators (the first 16) copy them in
   * to sgl. sgl is 32 bits wide by 6 rows. for each of the 1st 3 rows fill up
   * the 1st half (16 bits). Then, continue, this time, reset the pointer to zero
   * then add 17 filling the last 16 bits.
   *
   * The sgl data structure is a direct mapping of what was seen on the screen
   * if you looked at sgl's proto-type. He had 16 bits stacked on top of 16 bits 
   * (or 32 bits)
   *
   * This is MUCH more clear if you uncomment the 2 print statements and run a "download"!
   *
   * I map sgl's 6 x 33 a little differently: 
   * I map it as ( (1 thru 48 bits) x 3 rows ).
   *
   * The Tigris interface makes the mapping automatically.
   */

  for (sector=0; sector<6; sector++) {
    for (jamBit=0, j=1, k=-1; jamBit<48; jamBit++, j++) {
      if ( (jamBit % 16) == 0) {
	k++;
	j=1;
      }
      /* printf ("++++++++++++++++++++++++++++++  1 ++++++++++++ i[%d] j[%d] k[%d]\n", jamBit,j,k); */
      if (SDbits4sgl [sector][nphase][jamBit] > 0)
	Sector[sector+1].SD[nphase][k][j] = 1;
      if (EDbits4sgl [sector][nphase][jamBit] > 0)
	Sector[sector+1].ED[nphase][k][j] = 1;
    }
    for (j=0; jamBit<96; jamBit++, j++) {
      if ( (jamBit % 16) == 0) {
	k++;
	j=0;
      }
      /* printf ("+++++++++++++++++++++++++++++++ 2  +++++++++++ i[%2d] j[%2d] k[%d]\n", jamBit,(j+17),k); */
      if (SDbits4sgl[sector][nphase][jamBit] > 0)
	Sector[sector+1].SD[nphase][k][j+17] = 1;
      if (EDbits4sgl [sector][nphase][jamBit] > 0)
	Sector[sector+1].ED[nphase][k][j+17] = 1;

    }
  }
  for (sector=0; sector<6; sector++) {
    for (k=0; k<4; k++) {
      for (j=0; j<16; j++) {
	if (SPbits4sgl[sector][nphase][ j+(k*16) ] == 1)
	  Sector[sector+1].SP[nphase][k][j+1] = 1;
      }
    }
  }
}

/* -------------- mov1a2sgl  ---------------*/

void
movPhase12sgl(int phase)
{
  int i;
  
  printf ("send the data to a file - need it in the board to?\n");
  mov2sgl(phase);
  FileOnlyMode = 0;

  for (i=1; i<7; i++) {
    download_phase1(i, phase);
    printf ("done downloading sector %d\n", i);
  }
/*  clnt_destroy (cl) ;  */
}

/* ------------------------ */

void
add2error(int se, char *s)
{
  if (s == 0) {
    erbuf[se] = strdup ("no string passed to add2error");
    return ;
  }
  if (erbuf[se] != 0)
    free (erbuf[se]);
  
  erbuf[se] = strdup (s) ;
}

/* -------------------------------------------------------------------------- */

void
StuffSEPtext(char *line)
{
  /* if there is no RS Dialog, init it so that
  we can write into it */

  if (!SEPDialog)
    InitSEParseDlog();

  Hv_AddLineToScrolledText(SEPtext, &tpos, line);
}

/*-------- InitRunDiagnosticsDlog --------*/

void
InitSEParseDlog(void)
{
  Hv_Widget  rowcol, dummy;

  Hv_VaCreateDialog(&SEPDialog,
		    Hv_TITLE,  " SE Compilation Diagnostics ", 
		    Hv_TYPE,   Hv_MODELESS,
		    NULL);

  rowcol = Hv_DialogColumn(SEPDialog, 3);

  SEPtext = Hv_VaCreateDialogItem(rowcol,
				  Hv_TYPE,        Hv_SCROLLEDTEXTDIALOGITEM,
				  Hv_NUMROWS,     24,
				  Hv_NUMCOLUMNS,  100,
				  Hv_BACKGROUND,  Hv_white,
				  Hv_TEXTCOLOR,   Hv_black,
				  NULL);


/* close out with just an ok button */

  dummy = Hv_StandardActionButtons(rowcol, 100, Hv_OKBUTTON + Hv_CANCELBUTTON);

}
 
/* ------------------------ */

int
SEparse(int jb, int ja, int product, char *t)
{
  int value=1, len, SEerrors = 0, ofs;
  char bfr[2048];

  len = strlen(t) ;
  if(len == 0) return 0;

  init(t);  
  lex(bfr);
  if(Compile_Done)
  {
    sprintf (bfr, "sector[%d] SE[%d] Whats this [%s]\n", (jb+1), (ja+1), look.s);
    StuffSEPtext (bfr);
    return(1);
  }

  while(!Compile_Done)
  {
    if(look.ch == '/')
    {
      value = 0;    /* zero = the not operator ! */
      skip_op ();
      lex(bfr);
    }
    else
      value = 1;    /* one = the and operator */

    if(Is_SD())
    {
      lex(bfr);
      if(look.num < 1 || look.num > 6)
      {
        sprintf(bfr, 
                "sector[%d] SE[%d] error: scint events are: 1-6 ONLY: %d\n",
                (jb+1),(ja+1),look.num);
        add2error(SEerrors, bfr);
        StuffSEPtext (bfr);
        Compile_Done = 1;
        SEerrors ++;
        skip_op();
      }
      secdef[jb].se.products[ja][product][look.num-1] = value;
      lex(bfr);
      if(Is_or_op ())
      {
        product += 1;
        printf("inc the product (found an or)\n");
        skip_op();
      }
      else if(Is_and_op())
        skip_op();
      else
      {
        sprintf(bfr, 
"sector[%d] SE[%d] You need an operator - and[*] or[+] not[/], found: [%c]\n", 
                (jb+1),(ja+1),look.ch);
        add2error(SEerrors, bfr);
        StuffSEPtext (bfr);
        Compile_Done = 1;
        SEerrors ++;
        skip_op();
      }
    }
    else if(Is_SP())
    {
      ofs = 6;
      lex(bfr);
      if(look.num < 1 || look.num > 4)
      {
        sprintf(bfr,"sector[%d] SE[%d] error: spares are: 1-4 found: %d\n",
                (jb+1),(ja+1),look.num);
        add2error(SEerrors, bfr);
        StuffSEPtext (bfr);
        Compile_Done = 1;
        SEerrors ++;
        skip_op();
      }
      secdef[jb].se.products[ja][product][ofs+look.num-1] = value;
      lex(bfr);
      if(Is_or_op())
      {
        product ++;
        printf("inc the product (found an or)\n");
        skip_op();
      }
      else if(Is_and_op ()) 
        skip_op();
      else
      {
        sprintf(bfr, 
"sector[%d] SE[%d] You need an operator - and[*] or[+] not[/], found: [%c]\n", 
                (jb+1),(ja+1),look.ch);
        add2error(SEerrors, bfr);
        StuffSEPtext(bfr);
        Compile_Done = 1;
        SEerrors ++;
        skip_op();
      }
    }
    else if(Is_FC())
    {
      ofs = 10;
      lex(bfr);
      if(look.num < 1 || look.num > 3)
      {
        sprintf(bfr,"sector[%d] SE[%d] error: FC's are: 1-3 found: %d\n",
                (jb+1),(ja+1),look.num);
        add2error(SEerrors, bfr);
        Hv_Warning (erbuf[SEerrors]);
        Compile_Done = 1;
        SEerrors ++;
        skip_op();
      }
      secdef[jb].se.products[ja][product][ofs+look.num-1] = value;
      lex(bfr);
      if(Is_or_op())
      {
        product ++;
        printf("inc the product (found an or)\n");
        skip_op();
      }
      else if(Is_and_op ()) 
        skip_op();
      else
      {
        sprintf(bfr, 
"sector[%d] SE[%d] You need an operator - and[*] or[+] not[/], found: [%c]\n", 
                (jb+1),(ja+1),look.ch);
        add2error(SEerrors, bfr);
        StuffSEPtext(bfr);
        Compile_Done = 1;
        SEerrors ++;
        skip_op();
      }
    }
    else if(Is_CC())
    {
      ofs = 13;
      lex(bfr);
      if(look.num < 1 || look.num > 3)
      {
        sprintf(bfr,"sector[%d] SE[%d] error: CC's are: 1-3 found: %d\n",
                (jb+1),(ja+1),look.num);
        add2error(SEerrors, bfr);
        StuffSEPtext(bfr);
        Compile_Done = 1;
        SEerrors ++;
        skip_op();
      }
      secdef[jb].se.products[ja][product][ofs+look.num-1] = value;
      lex(bfr);
      if(Is_or_op())
      {
        product ++;
        printf("inc the product (found an or)\n");
        skip_op();
      }
      else if(Is_and_op ()) 
        skip_op();
      else
      {
        sprintf(bfr, 
"sector[%d] SE[%d] You need an operator - and[*] or[+] not[/], found: [%c]\n", 
                (jb+1),(ja+1),look.ch);
        add2error(SEerrors, bfr);
        StuffSEPtext(bfr);
        Compile_Done = 1;
        SEerrors ++;
        skip_op();
      }
    }
    else
    {
      sprintf(bfr, 
              "sector[%d] SE[%d] don't know what this is... found: [%s]\n", 
              (jb+1),(ja+1),look.s);
      add2error(SEerrors, bfr);
      StuffSEPtext(bfr);
      Compile_Done = 1;
      SEerrors ++;
      skip_op();
    }

    lex(bfr);
  }

  if(SEerrors>0) printf("Number of SEerrors[%d]\n", SEerrors);

  return(SEerrors);
}
/* ------------------------ */

void
movSE2sgl()
{
  char *t;
  int i, mxse, sctr, j, r, sector;

  r = 0;
  printf("Move the scintillator events into place\n");

  for(sector=0; sector<7; sector++) 
    for(mxse=0; mxse<12; mxse++) 
      for(i=0; i<32; i++) /* init Stephan's struct - fill it later in this routine */
        for(j=0; j<16; j++) 
          Sector[sector].SE[mxse][i][j] = 2;

/*Sergey: BUG, and very bad one: secdef[6], but loop over 7 elements !!! */
/*for(sector=0; sector<7; sector++) */
  for(sector=0; sector<6; sector++) 
    for(mxse=0; mxse<12; mxse++)
      for(i=0; i<32; i++) /* init my struct and fill it with SEparse */
        for(j=0; j<16; j++) 
          secdef[sector].se.products[mxse][i][j] = 2;

  for(sctr=0; sctr<6; sctr++)
  {
    flush_those_textItems(view[sctr]->data);
    for(mxse=0; mxse<12; mxse++)
    {
      t = JJ_SEtextItem[sctr][mxse];
      if(t == NULL) continue;

      if((r = SEparse(sctr, mxse, product1, t)) == 0)
      {
        FileOnlyMode = 1;
        /* I've prepared my data, now move it to sgl */
        for(i=0; i<32; i++)
        {
          for(j=0; j<16; j++)
          {
            /*printf ("==== [%d]", secdef[sctr].se.products[mxse][i][j]);*/
            Sector[sctr+1].SE[mxse][i][j] =
                                         secdef[sctr].se.products[mxse][i][j]; 
          }
          /*printf("\n");*/
        }
      }
    }   
  }

  if(r != 0)
  {
    printf ("SEparse returned: %d\n", r);
    if(SEPDialog == 0) InitSEParseDlog();
    Hv_DoDialog(SEPDialog, NULL);
    StuffSEPtext ("-------------------------------------------------------\n");
  }

}

/* ------------ downSE ----------------- */

void
downloadSE()
{
  int i;

  for(i=1; i<7; i++)
  {
    download_phase2(i);
    printf("phase 2 sector %d\n", i);
  }
}

/* ----- Build the event definition area -------*/

void
buildEventArea(Hv_View View)
{
  Hv_Item     evcomboundbox;
  Hv_Item     button, evTitle, box1, item ;
  Hv_Rect    *hotrect = View->hotrect ;
  ViewData    vdata;
  int         i, base ;
  char        b[80] ;

  vdata = GetViewData (View) ;

  evbox = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BOXITEM,
                         Hv_LEFT,              hotrect->left-5,
                         Hv_TOP,               hotrect->bottom+10,
                         Hv_WIDTH,             hotrect->width+10,
                         Hv_HEIGHT,            ( (View->local->bottom-20) - hotrect->bottom),
                         NULL) ;

  evTitle = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            evbox,
                         Hv_LEFT,              evbox->area->left+250,
                         Hv_TOP,               evbox->area->top+10,
                         Hv_TEXT,              "Tigris Event Builder",
                         NULL) ;

  evcomboundbox = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BOXITEM,
                         Hv_RELATIVEPLACEMENT,  Hv_POSITIONRIGHT, 
                         Hv_WIDTH,              View->local->right - hotrect->right - 15,
                         Hv_HEIGHT,            ( (View->local->bottom-20) - hotrect->bottom),
                         NULL) ;

  setuptheSElabels (View, 1) ;

  vdata->eitem[0] = Hv_VaCreateItem (View,
                         Hv_TYPE,          Hv_TEXTENTRYITEM,
                         Hv_DOMAIN,        Hv_OUTSIDEHOTRECT, 
                         Hv_LEFT,          evbox->area->left+35,
                         Hv_TOP,           evbox->area->top+30,
                         Hv_PARENT,        evbox,
                         Hv_WIDTH,         evbox->area->width-40,
                         Hv_HEIGHT,        25,
			 Hv_USER1,         0, /* for the comment box */
                         Hv_DRAWCONTROL,   Hv_FRAMEAREA + Hv_DONTAUTOSIZE,
				     /* Hv_SINGLECLICK,   JAM_FieldSingleClick, */
                         NULL) ; 

  vdata->eitem[0]->singleclick = JAM_FieldSingleClick;
  vdata->activeTextItem = vdata->eitem[0] ;

  printf ("sector number = %d\n", vdata->sectorNumber);
  initScintEventStructs (vdata->sectorNumber);

  /* Hv_activeTextItem */

  /* user1 = id of the comment to show in the textbox. */
  for (i=1; i<6; i++) {
    vdata->eitem[i] = Hv_VaCreateItem (View,
                         Hv_TYPE,          Hv_TEXTENTRYITEM,
                         Hv_DOMAIN,        Hv_OUTSIDEHOTRECT,
                         Hv_RELATIVEPLACEMENT,  Hv_POSITIONBELOW, 
                         Hv_PARENT,        evbox,
                         Hv_WIDTH,         evbox->area->width-40,
                         Hv_HEIGHT,        25,
			 Hv_USER1,         i,   /* mark for the comment */
                         Hv_DRAWCONTROL,   Hv_FRAMEAREA + Hv_DONTAUTOSIZE,
                         Hv_PLACEMENTGAP,  2,
                         Hv_SINGLECLICK,   JAM_FieldSingleClick,
                         NULL) ; 
    vdata->eitem[i]->singleclick = JAM_FieldSingleClick;
    
  }

  box1 = Hv_VaCreateItem (View, 
                         Hv_TYPE,               Hv_BOXITEM,
                         Hv_DOMAIN,             Hv_OUTSIDEHOTRECT,
                         Hv_PARENT,             evbox,
                         Hv_LEFT,               evbox->area->left+5,
                         Hv_TOP,                evbox->area->bottom-75,
                         Hv_WIDTH,              View->local->left + 125,
                         Hv_HEIGHT,             (68),
                         NULL) ;

  vdata->butAnd = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_PARENT,            box1,
                         Hv_RELATIVEPLACEMENT, Hv_POSITIONBELOW, 
                         Hv_LABEL,             "And",
                         Hv_LEFT,              box1->area->left+10,
                         Hv_TOP,               box1->area->top+12,
                         Hv_WIDTH,             30,
                         Hv_HEIGHT,            18,
                         Hv_SINGLECLICK,       EV_ButtonAndClick,
                         NULL) ;

  Hv_InvertColorScheme (vdata->butAnd->scheme) ;
  Hv_DrawItem (vdata->butAnd, NULL) ;

  vdata->butOr = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_PARENT,            box1,
                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_LABEL,             "Or",
                         Hv_WIDTH,             30,
                         Hv_HEIGHT,            18,
                         Hv_PLACEMENTGAP,      5,
                         Hv_SINGLECLICK,       EV_ButtonOrClick,
                         NULL) ;

  vdata->butChk = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_PARENT,            box1,
                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_LABEL,             "Check",
                         Hv_WIDTH,             50,
                         Hv_HEIGHT,            38,
                         Hv_PLACEMENTGAP,      5,
                         Hv_SINGLECLICK,       EV_ButtonChkClick,
                         NULL) ;

  vdata->butNot = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_PARENT,            box1,
                         Hv_PLACEMENTITEM,     vdata->butAnd,
                         Hv_RELATIVEPLACEMENT, Hv_POSITIONBELOW,
                         Hv_LABEL,             "Not",
                         Hv_WIDTH,             30,
                         Hv_HEIGHT,            18,
                         Hv_PLACEMENTGAP,      5,
                         Hv_SINGLECLICK,       EV_ButtonNotClick,
                         NULL) ;

  vdata->butClr = Hv_VaCreateItem (View,
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_PARENT,            box1,
                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_LABEL,             "Clr",
                         Hv_WIDTH,             30,
                         Hv_HEIGHT,            18,
                         Hv_PLACEMENTGAP,      5,
                         Hv_SINGLECLICK,       EV_ButtonClrClick,
                         NULL) ;

  item = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            evbox,
                         Hv_LEFT,              evbox->area->left+200,
                         Hv_TOP,               View->local->bottom-80,
                         Hv_TEXT,              "Scint Data",
                         NULL) ;

  button = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_LABEL,             "1",
                         Hv_FILLCOLOR,         scintColor[0],
                         Hv_LEFT,              hotrect->left+180,
                         Hv_TOP,               View->local->bottom-60,
                         Hv_WIDTH,             18,
                         Hv_HEIGHT,            18,
                         Hv_SINGLECLICK,       EV_ButtonSDClick,
                         NULL) ;
  button->data = (void *)1 ;   


  button = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_LABEL,             "2",
                         Hv_FILLCOLOR,         scintColor[1],
                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_PLACEMENTGAP,      2,
                         Hv_WIDTH,             18,
                         Hv_HEIGHT,            18,
                         Hv_SINGLECLICK,       EV_ButtonSDClick,
                         NULL) ;
  button->data = (void *)2 ;


  button = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_LABEL,             "3",
                         Hv_FILLCOLOR,         scintColor[2],
                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_PLACEMENTGAP,      2,
                         Hv_WIDTH,             18,
                         Hv_HEIGHT,            18,
                         Hv_SINGLECLICK,       EV_ButtonSDClick,
                         NULL) ;
  button->data = (void *)3 ;
            

  button = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_LABEL,             "4",
                         Hv_FILLCOLOR,         scintColor[3],
                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_PLACEMENTGAP,      2,
                         Hv_WIDTH,             18,
                         Hv_HEIGHT,            18,
                         Hv_SINGLECLICK,       EV_ButtonSDClick,
                         NULL) ;
  button->data = (void *)4 ;


  button = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_LABEL,             "5",
                         Hv_FILLCOLOR,         scintColor[4],
                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_PLACEMENTGAP,      2,
                         Hv_WIDTH,             18,
                         Hv_HEIGHT,            18,
                         Hv_SINGLECLICK,       EV_ButtonSDClick,
                         NULL) ;
  button->data = (void *)5 ;


  button = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_LABEL,             "6",
                         Hv_FILLCOLOR,         scintColor[5],
                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_PLACEMENTGAP,      2,
                         Hv_WIDTH,             18,
                         Hv_HEIGHT,            18,
                         Hv_SINGLECLICK,       EV_ButtonSDClick,
                         NULL) ;
  button->data = (void *)6 ;

  item = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            evbox,
                         Hv_LEFT,              evbox->area->left+330,
                         Hv_TOP,               View->local->bottom-80,
                         Hv_TEXT,              "Spare Data",
                         NULL) ;

  base = 3;
  button = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_LABEL,             "1",
                         Hv_FILLCOLOR,         scintColor[ (base+0) ],
                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_PLACEMENTGAP,      20,
                         Hv_WIDTH,             18,
                         Hv_HEIGHT,            18,
			 Hv_SINGLECLICK,       EV_ButtonSPClick,
                         NULL) ;     
  button->data = (void *)1;

  for (i=2; i<5; i++) {
    sprintf (b, "%d", i) ;
    button = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_LABEL,             b,
                         Hv_FILLCOLOR,         scintColor[ (base+(i-1)) ],
                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_PLACEMENTGAP,      2,
                         Hv_WIDTH,             18,
                         Hv_HEIGHT,            18,
			 Hv_SINGLECLICK,       EV_ButtonSPClick,
                         NULL) ;
    button->data = (void *)i;
  }                         

  item = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            evbox,
                         Hv_LEFT,              evbox->area->left+450,
                         Hv_TOP,               View->local->bottom-80,
                         Hv_TEXT,              "Extra Data",
                         NULL) ;

  base = 0;
  button = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_LABEL,             "1",
                         Hv_FILLCOLOR,         scintColor[ (base) ],
                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_PLACEMENTGAP,      20,
                         Hv_WIDTH,             18,
                         Hv_HEIGHT,            18,
			 Hv_SINGLECLICK,       EV_ButtonFCClick,
                         NULL) ;     

  button->data = (void *)1;

  for (i=2; i<4; i++) {
    sprintf (b, "%d", i) ;
    button = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_LABEL,             b,
                         Hv_FILLCOLOR,         scintColor[ (base+(i-1)) ],
                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_PLACEMENTGAP,      2,
                         Hv_WIDTH,             18,
                         Hv_HEIGHT,            18,
			 Hv_SINGLECLICK,       EV_ButtonFCClick,
                         NULL) ;

    button->data = (void *)i;
  } 
  base = 2;
  for (i=1; i<4; i++) {
    sprintf (b, "%d", i) ;
    button = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_BUTTONITEM,
                         Hv_LABEL,             b,
                         Hv_FILLCOLOR,         scintColor[ (base+(i)) ],
                         Hv_RELATIVEPLACEMENT, Hv_POSITIONRIGHT,
                         Hv_PLACEMENTGAP,      2,
                         Hv_WIDTH,             18,
                         Hv_HEIGHT,            18,
			 Hv_SINGLECLICK,       EV_ButtonCCClick,
                         NULL) ;

    button->data = (void *)i;
  } 

  /* by default the first one has data in it. */
  /*
  for (i=0; i<12; i++) {
    SECommentText[vdata->sectorNumber][i] = XtMalloc (512);
    sprintf (SECommentText[vdata->sectorNumber][i], "this text number %d", i);
  }
  */
  vdata->OldCommentIndex = 0;

  vdata->evcommentbox = Hv_VaCreateItem (View,
                         Hv_TYPE,          Hv_TEXTENTRYITEM,
                         Hv_DOMAIN,        Hv_OUTSIDEHOTRECT, 
                         Hv_LEFT,          evcomboundbox->area->left+5,
                         Hv_TOP,           evcomboundbox->area->top+5,
                         Hv_PARENT,        evcomboundbox,
                         Hv_WIDTH,         evcomboundbox->area->width-10,
                         Hv_HEIGHT,        evcomboundbox->area->height-10,
                         Hv_TEXT,          SECommentText[vdata->sectorNumber][0],
                         Hv_DRAWCONTROL,   Hv_FRAMEAREA + Hv_DONTAUTOSIZE,
			 Hv_EDITABLE,      True,
			 Hv_INITIALIZE,    JAM_TextInit,
			 Hv_SINGLECLICK,   JAM_CreateMTextEdit,
			 Hv_TEXTENTRYMODE, Hv_MULTILINEEDIT,

                         NULL) ; 

}

/* ------------------ label_the_se ---------------------- */

void
setuptheSElabels(Hv_View View, int offset)
{
  int      i, j=0;
  char     b[10], *sp;
  int      tcolor=Hv_white;
  ViewData vdata;

  vdata = GetViewData (View) ;

  i=offset;  /* so... offset is 1 or 7  */
  sprintf (b, "SE%d ", i) ;
  vdata->SELabels[j++] = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            evbox,
                         Hv_TEXTCOLOR,         tcolor,
                         Hv_FILLCOLOR,         trigColor[j],
                         Hv_LEFT,              evbox->area->left+6,
                         Hv_TOP,               evbox->area->top+32,
                         Hv_TEXT,              (char *)b,
                         NULL) ;

  sprintf (b, "SE%d ", ++i) ;  /* i=2 */
  vdata->SELabels[j++] = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            evbox,
                         Hv_TEXTCOLOR,         tcolor,
                         Hv_FILLCOLOR,         trigColor[j],
                         Hv_LEFT,              evbox->area->left+6, 
                         Hv_TOP,               evbox->area->top+32+(28),
                         Hv_PLACEMENTGAP,      18,
                         Hv_TEXT,              (char *)b,
                         NULL) ;

  sprintf (b, "SE%d ", ++i) ;  /* i=3 */
  vdata->SELabels[j++] = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            evbox,
                         Hv_TEXTCOLOR,         tcolor,
                         Hv_FILLCOLOR,         trigColor[j],
                         Hv_LEFT,              evbox->area->left+6, 
                         Hv_TOP,               evbox->area->top+32+(55),
                         Hv_PLACEMENTGAP,      18,
                         Hv_TEXT,              (char *)b,
                         NULL) ;

  sprintf (b, "SE%d ", ++i) ;  /* i=4 */
  vdata->SELabels[j++] = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            evbox,
                         Hv_TEXTCOLOR,         tcolor,
                         Hv_FILLCOLOR,         trigColor[j],
                         Hv_LEFT,              evbox->area->left+6, 
                         Hv_TOP,               evbox->area->top+32+(80),
                         Hv_TEXT,              (char *)b,
                         NULL) ;
  if (i > 9)
    sp = "SE%d";
  else
    sp = "SE%d ";

  sprintf (b, sp, ++i) ;  /* i=5 */
  vdata->SELabels[j++] = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            evbox,
                         Hv_TEXTCOLOR,         tcolor,
                         Hv_FILLCOLOR,         trigColor[j],
                         Hv_LEFT,              evbox->area->left+6, 
                         Hv_TOP,               evbox->area->top+32+(107),
                         Hv_PLACEMENTGAP,      18,
                         Hv_TEXT,              (char *)b,
                         NULL) ;

  sprintf (b, sp, ++i) ;  /* i=6 */
  vdata->SELabels[j++] = Hv_VaCreateItem (View, 
                         Hv_TYPE,              Hv_TEXTITEM,
                         Hv_PARENT,            evbox,
                         Hv_TEXTCOLOR,         tcolor,
                         Hv_FILLCOLOR,         trigColor[j],
                         Hv_LEFT,              evbox->area->left+6, 
                         Hv_TOP,               evbox->area->top+32+(135),
                         Hv_PLACEMENTGAP,      18,
                         Hv_TEXT,              (char *)b,
                         NULL) ;

}

/* -------------------------------- */

void
label_the_se(Hv_View View, int offset)
{
  int j, se_num;
  ViewData vdata;
  char b[1024], *sp;

  vdata = GetViewData (View) ;
  se_num = offset;  /* the offset is 1 or 7 */

  for (j=0; j<6; j++) {
    if (offset > 9) 
      sp = "SE%d" ;   /* handle 11 & 12 this way */
    else
      sp = "SE%d ";
    
    sprintf (b, sp, se_num++);
    Hv_SetItemText (vdata->SELabels[j], b);
    Hv_DrawItem (vdata->SELabels[j], NULL);
  }
}

#else

void
evbutton_dummy()
{
}

#endif
