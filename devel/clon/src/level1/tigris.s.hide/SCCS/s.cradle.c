h23904
s 00009/00000/00311
d D 1.3 06/09/01 00:36:38 boiarino 4 3
c dummy VXWORKS
c 
e
s 00186/00141/00125
d D 1.2 02/08/25 19:39:26 boiarino 3 1
c big cleanup (globals, locals, prototypes)
e
s 00000/00000/00000
d R 1.2 02/03/29 12:13:01 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/level1/tigris/cradle.c
e
s 00266/00000/00000
d D 1.1 02/03/29 12:13:00 boiarino 1 0
c date and time created 02/03/29 12:13:00 by boiarino
e
u
U
f e 0
t
T
I 1

I 4
#ifndef VXWORKS

E 4
I 3
/* cradle.c */

E 3
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

D 3
#include "def.h"
E 3
I 3
#include "Hv.h"
#include "ced.h"
#include "tigris.h"
E 3

D 3
int Compile_Done = 0;
E 3
I 3
struct Look look;     /* global, used by evbutton.c and sectbuttons.c */
int Compile_Done = 0; /* global, used by evbutton.c and sectbuttons.c */
E 3

D 3
FILE *fp, *outlst;
char emit_buffer[130];
char debug_buffer [130];
E 3

D 3
struct Look look;
char readch ();
E 3

/* -------------------------------------------------- 
 */
D 3
void skip_op () {
  look.ch = readch ();
E 3
I 3
static char
readch()
{
  if(look.thepoint >= (look.theline+look.thelength-1))
  {
    Compile_Done = 1;
    return(127);
  }
  else
  {
    look.thepoint += 1;
    return(*look.thepoint);
  }
E 3
}
I 3

E 3
/* -------------------------------------------------- 
 */
D 3
int Is_SE () {

  if ( (strcasecmp (look.s, "SE")) == 0) 
E 3
I 3
void
skip_op()
{
  look.ch = readch();
}
/* -------------------------------------------------- 
 */
int
Is_SE()
{
  if( (strcasecmp (look.s, "SE")) == 0) 
E 3
    return 1;
  else
    return 0;
}
/* -------------------------------------------------- 
 */
D 3
int Is_SD () {

E 3
I 3
int
Is_SD()
{
E 3
  if ( (strcasecmp (look.s, "SD")) == 0) 
    return 1;
  else
    return 0;
}
/* -------------------------------------------------- 
 */
D 3
int Is_FC () {

E 3
I 3
int
Is_FC()
{
E 3
  if ( (strcasecmp (look.s, "FC")) == 0) 
    return 1;
  else
    return 0;
}
/* -------------------------------------------------- 
 */
D 3
int Is_CC () {

E 3
I 3
int
Is_CC()
{
E 3
  if ( (strcasecmp (look.s, "CC")) == 0) 
    return 1;
  else
    return 0;
}
/* -------------------------------------------------- 
 */
D 3
int Is_ED () {

E 3
I 3
int
Is_ED()
{
E 3
  if ( (strcasecmp (look.s, "ED")) == 0) 
    return 1;
  else
    return 0;
}
/* -------------------------------------------------- 
 */
D 3
int Is_SP () {

E 3
I 3
int
Is_SP()
{
E 3
  if ( (strcasecmp (look.s, "SP")) == 0) 
    return 1;
  else
    return 0;
}
/* -------------------------------------------------- 
 */
D 3
int Is_and_op ()
E 3
I 3
int
Is_and_op()
E 3
{
D 3
int r = 1;

	if (look.ch != '*')
	  r = 0;
	return r;
E 3
I 3
  int r = 1;
  if(look.ch != '*') r = 0;
  return(r);
E 3
}
/* -------------------------------------------------- 
 */
D 3
int Is_or_op ()
E 3
I 3
int
Is_or_op()
E 3
{
D 3
int r = 1;

	if (look.ch != '+')
	  r = 0;
	return r;
E 3
I 3
  int r = 1;
  if(look.ch != '+') r = 0;
  return(r);
E 3
}
/* -------------------------------------------------- 
 */
D 3
int Is_White_Space ()
E 3
I 3
int
Is_White_Space()
E 3
{
D 3
	if (look.ch != ' ') 
		if (look.ch != '\t') 
			if (look.ch != '\n')
				if (look.ch != '\r') return 0; 
	return 1;
E 3
I 3
  if(look.ch != ' ') 
    if(look.ch != '\t') 
      if(look.ch != '\n')
        if(look.ch != '\r') return(0); 
  return(1);
E 3
}
/* -------------------------------------------------- 
 */
D 3
int Is_Digit ()
E 3
I 3
int
Is_Digit()
E 3
{
	if (isdigit (look.ch)) 
		return 1;
	else
		return 0;
}
/* -------------------------------------------------- 
 */
D 3
int Is_Alpha ()
E 3
I 3
int
Is_Alpha()
E 3
{
D 3
	if (isalpha (look.ch)) 
		return 1;
	else
		return 0;
E 3
I 3
  if(isalpha(look.ch)) 
    return(1);
  else
    return(0);
E 3
}
/* -------------------------------------------------- 
 */
D 3
void init (str)
char *str;
E 3
I 3
void
init(char *str)
E 3
{
D 3
        look.theline = str;
	look.thelength = strlen (str);
	look.thepoint = str;
	look.ch = *str;
        Compile_Done = 0;
E 3
I 3
  look.theline = str;
  look.thelength = strlen (str);
  look.thepoint = str;
  look.ch = *str;
  Compile_Done = 0;
E 3
}
/* -------------------------------------------------- 
 */
D 3
void show_look_ahead ()
E 3
I 3
void
show_look_ahead()
E 3
{
D 3
  printf ("the line -> %s\n", look.theline);
  printf ("the length -> %d\n", look.thelength);
  printf ("char -> %X  -- %c\n", look.ch, look.ch);
  printf ("number -> %d\n", look.num);
  printf ("token -> %s\n", look.s);
  printf ("type -> %d\n", look.type);
E 3
I 3
  printf("   look: the line -> %s\n", look.theline);
  printf("   look: the length -> %d\n", look.thelength);
  printf("   look: char -> %X  -- %c\n", look.ch, look.ch);
  printf("   look: number -> %d\n", look.num);
  printf("   look: token -> %s\n", look.s);
  printf("   look: type -> %d\n", look.type);
E 3
	
}
/* -------------------------------------------------- 
 */
D 3
char readch ()
E 3
I 3
void
Skip_White()
E 3
{
D 3

  if (look.thepoint >= (look.theline+look.thelength-1)) {
    Compile_Done = 1;
    return 127;
  } else {
    look.thepoint += 1;
    return *look.thepoint;
E 3
I 3
  while(1)
  {
    look.ch = readch();
    if(Compile_Done) break;
    if(!Is_White_Space()) break;
E 3
  }
}
D 3
/* -------------------------------------------------- 
 */
void Skip_White ()
E 3
I 3

/* -------------------------------------------------- */

int
get_number(char *b)
E 3
{
D 3
	while (1) {
		look.ch = readch ();
		if (Compile_Done) break;
		if (!Is_White_Space ())  break ;
	}
}
/* -------------------------------------------------- 
 */
int get_number (b)
char *b;
{
int i;
E 3
I 3
  int i;
E 3

D 3
	for (i=0; i<TOKEN_LENGTH; i++)
	{
		if (!Is_Digit ()) break;
		look.s[i] = look.ch;
		look.ch = readch () ;      
		if (Compile_Done) {
		  sprintf (b, "Get Number: End of File\n");
		  break ;
		}
		if (Is_White_Space ()) break;
	}
	return atoi (look.s);
E 3
I 3
  for(i=0; i<TOKEN_LENGTH; i++)
  {
    if(!Is_Digit()) break;
    look.s[i] = look.ch;
    look.ch = readch();      
    if(Compile_Done)
    {
      sprintf(b,"Get Number: End of File\n");
      break;
    }
    if(Is_White_Space()) break;
  }

  return(atoi(look.s));
E 3
}
D 3
/* -------------------------------------------------- 
 */
int get_name (b)
char *b;
E 3
I 3

/* -------------------------------------------------- */

int
get_name(char *b)
E 3
{
  int i, ret=0; 
  
D 3
  for (i=0; i<TOKEN_LENGTH; i++)
E 3
I 3
  for(i=0; i<TOKEN_LENGTH; i++)
  {
    look.s[i] = look.ch;
    look.ch = readch();
    if(Compile_Done)
E 3
    {
D 3
      look.s[i] = look.ch;
      look.ch = readch ();
      if (Compile_Done) {
	printf ("get_name: End of File\n");
	break;
      }
      if (Is_White_Space ()) break;
      if (isdigit (look.ch)) break;
E 3
I 3
      printf("get_name: End of File\n");
      break;
E 3
    }
I 3
    if(Is_White_Space()) break;
    if(isdigit(look.ch)) break;
  }
E 3

D 3
  if (i >= TOKEN_LENGTH) {
    sprintf (b, "get_name: token too long\n");
E 3
I 3
  if(i >= TOKEN_LENGTH)
  {
    sprintf(b,"get_name: token too long\n");
E 3
    ret = 1;
  }

D 3
  return ret;
E 3
I 3
  return(ret);
E 3
}
D 3
/* -------------------------------------------------- 
 */
void clean_look ()
E 3
I 3

/* -------------------------------------------------- */

void
clean_look()
E 3
{
  int i;
  
  look.num = 0;
  look.type = 0;
D 3
  
  for (i=0; i<TOKEN_LENGTH; i++) look.s[i] = 0;
E 3
I 3
  for(i=0; i<TOKEN_LENGTH; i++) look.s[i] = 0;
E 3
}
D 3
/* -------------------------------------------------- 
 */
int lex (bfr)
char  *bfr; 
E 3
I 3

/* -------------------------------------------------- */

int
lex(char *bfr)
E 3
{
D 3
  int yes, no;
  clean_look ();
  
  yes = 1;
  no = 0;
  if (Is_White_Space ())
    Skip_White ();
  
  if (Is_and_op ()) {
E 3
I 3
  int yes=1, no=0;

  clean_look();
  /*
  show_look_ahead();
  */
  if(Is_White_Space()) Skip_White();

  if(Is_and_op())
  {
E 3
    look.type = AND_OP;
    look.s[0] = look.ch;
D 3
    return yes;
E 3
I 3
    /*printf("lex: Is_and_op: type=%d s[0]='%c'\n",look.type,look.s[0]);*/
    return(yes);
E 3
  }
D 3
  if (Is_or_op ()) {
E 3
I 3

  if(Is_or_op())
  {
E 3
    look.type = OR_OP;
D 3
    look.s[0] = '+' ;
    return yes;
E 3
I 3
    look.s[0] = '+';
    /*printf("lex: Is_or_op: type=%d s[0]='%c'\n",look.type,look.s[0]);*/
    return(yes);
E 3
  }
D 3
  if (Is_Alpha ()) {
E 3
I 3

  if(Is_Alpha())
  {
E 3
    look.type = ALPHA;
D 3
    if ((get_name (bfr)) == 0)
      return yes;
E 3
I 3
    if((get_name(bfr)) == 0)
    {
      /*printf("lex: Is_Alpha: type=%d s=>%32.32s<\n",look.type,look.s);*/
      return(yes);
    }
E 3
    else
D 3
      return no;
E 3
I 3
    {
      /*printf("lex: Is_Alpha: ERROR: bfr=>%16.16s<\n",bfr);*/
      return(no);
    }
E 3
  }
  else
D 3
    if (Is_Digit ()) {
E 3
I 3
  {
    if(Is_Digit())
    {
E 3
      look.type = DIGIT;
D 3
      look.num = get_number (bfr);
      return yes;
E 3
I 3
      look.num = get_number(bfr);
      /*printf("lex: Is_Digit: type=%d num=%d\n",look.type,look.num);*/
      return(yes);
E 3
    }
D 3
  return no;
E 3
I 3
  }

  /*printf("lex: ERROR !!!!!!!!! look.ch=>%c<\n",look.ch);*/

  return(no);
E 3
}
I 3

I 4
#else /* UNIX only */
E 4
E 3

I 4
void
cradle_dummy()
{
}

#endif
E 4
E 1
