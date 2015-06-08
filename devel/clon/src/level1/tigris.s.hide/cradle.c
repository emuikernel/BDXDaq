
#ifndef VXWORKS

/* cradle.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "Hv.h"
#include "ced.h"
#include "tigris.h"

struct Look look;     /* global, used by evbutton.c and sectbuttons.c */
int Compile_Done = 0; /* global, used by evbutton.c and sectbuttons.c */



/* -------------------------------------------------- 
 */
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
}

/* -------------------------------------------------- 
 */
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
    return 1;
  else
    return 0;
}
/* -------------------------------------------------- 
 */
int
Is_SD()
{
  if ( (strcasecmp (look.s, "SD")) == 0) 
    return 1;
  else
    return 0;
}
/* -------------------------------------------------- 
 */
int
Is_FC()
{
  if ( (strcasecmp (look.s, "FC")) == 0) 
    return 1;
  else
    return 0;
}
/* -------------------------------------------------- 
 */
int
Is_CC()
{
  if ( (strcasecmp (look.s, "CC")) == 0) 
    return 1;
  else
    return 0;
}
/* -------------------------------------------------- 
 */
int
Is_ED()
{
  if ( (strcasecmp (look.s, "ED")) == 0) 
    return 1;
  else
    return 0;
}
/* -------------------------------------------------- 
 */
int
Is_SP()
{
  if ( (strcasecmp (look.s, "SP")) == 0) 
    return 1;
  else
    return 0;
}
/* -------------------------------------------------- 
 */
int
Is_and_op()
{
  int r = 1;
  if(look.ch != '*') r = 0;
  return(r);
}
/* -------------------------------------------------- 
 */
int
Is_or_op()
{
  int r = 1;
  if(look.ch != '+') r = 0;
  return(r);
}
/* -------------------------------------------------- 
 */
int
Is_White_Space()
{
  if(look.ch != ' ') 
    if(look.ch != '\t') 
      if(look.ch != '\n')
        if(look.ch != '\r') return(0); 
  return(1);
}
/* -------------------------------------------------- 
 */
int
Is_Digit()
{
	if (isdigit (look.ch)) 
		return 1;
	else
		return 0;
}
/* -------------------------------------------------- 
 */
int
Is_Alpha()
{
  if(isalpha(look.ch)) 
    return(1);
  else
    return(0);
}
/* -------------------------------------------------- 
 */
void
init(char *str)
{
  look.theline = str;
  look.thelength = strlen (str);
  look.thepoint = str;
  look.ch = *str;
  Compile_Done = 0;
}
/* -------------------------------------------------- 
 */
void
show_look_ahead()
{
  printf("   look: the line -> %s\n", look.theline);
  printf("   look: the length -> %d\n", look.thelength);
  printf("   look: char -> %X  -- %c\n", look.ch, look.ch);
  printf("   look: number -> %d\n", look.num);
  printf("   look: token -> %s\n", look.s);
  printf("   look: type -> %d\n", look.type);
	
}
/* -------------------------------------------------- 
 */
void
Skip_White()
{
  while(1)
  {
    look.ch = readch();
    if(Compile_Done) break;
    if(!Is_White_Space()) break;
  }
}

/* -------------------------------------------------- */

int
get_number(char *b)
{
  int i;

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
}

/* -------------------------------------------------- */

int
get_name(char *b)
{
  int i, ret=0; 
  
  for(i=0; i<TOKEN_LENGTH; i++)
  {
    look.s[i] = look.ch;
    look.ch = readch();
    if(Compile_Done)
    {
      printf("get_name: End of File\n");
      break;
    }
    if(Is_White_Space()) break;
    if(isdigit(look.ch)) break;
  }

  if(i >= TOKEN_LENGTH)
  {
    sprintf(b,"get_name: token too long\n");
    ret = 1;
  }

  return(ret);
}

/* -------------------------------------------------- */

void
clean_look()
{
  int i;
  
  look.num = 0;
  look.type = 0;
  for(i=0; i<TOKEN_LENGTH; i++) look.s[i] = 0;
}

/* -------------------------------------------------- */

int
lex(char *bfr)
{
  int yes=1, no=0;

  clean_look();
  /*
  show_look_ahead();
  */
  if(Is_White_Space()) Skip_White();

  if(Is_and_op())
  {
    look.type = AND_OP;
    look.s[0] = look.ch;
    /*printf("lex: Is_and_op: type=%d s[0]='%c'\n",look.type,look.s[0]);*/
    return(yes);
  }

  if(Is_or_op())
  {
    look.type = OR_OP;
    look.s[0] = '+';
    /*printf("lex: Is_or_op: type=%d s[0]='%c'\n",look.type,look.s[0]);*/
    return(yes);
  }

  if(Is_Alpha())
  {
    look.type = ALPHA;
    if((get_name(bfr)) == 0)
    {
      /*printf("lex: Is_Alpha: type=%d s=>%32.32s<\n",look.type,look.s);*/
      return(yes);
    }
    else
    {
      /*printf("lex: Is_Alpha: ERROR: bfr=>%16.16s<\n",bfr);*/
      return(no);
    }
  }
  else
  {
    if(Is_Digit())
    {
      look.type = DIGIT;
      look.num = get_number(bfr);
      /*printf("lex: Is_Digit: type=%d num=%d\n",look.type,look.num);*/
      return(yes);
    }
  }

  /*printf("lex: ERROR !!!!!!!!! look.ch=>%c<\n",look.ch);*/

  return(no);
}

#else /* UNIX only */

void
cradle_dummy()
{
}

#endif
