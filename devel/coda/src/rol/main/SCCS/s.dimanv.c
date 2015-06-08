h11807
s 00084/00027/00100
d D 1.5 11/12/16 13:27:10 sergpozd 5 4
c *** empty log message ***
e
s 00015/00005/00112
d D 1.4 10/12/09 10:40:59 sergpozd 4 3
c corrected of path for "Chosen config file"
c 
e
s 00003/00002/00114
d D 1.3 10/11/19 16:38:06 sergpozd 3 2
c *** empty log message ***
e
s 00002/00001/00114
d D 1.2 10/09/28 13:20:30 sergpozd 2 1
c add "primexts2"
c 
e
s 00115/00000/00000
d D 1.1 10/09/28 13:13:56 sergpozd 1 0
c date and time created 10/09/28 13:13:56 by sergpozd
e
u
U
f e 0
t
T
I 1
/* UNIX version only */

#ifndef VXWORKS

/* dimanv.c
 *
 * Program to set VME Discriminators CAEN v895
 *
 *   written  by SP  08-March-2007
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <libtcp.h>


#define F_NAME    100       /* length of config. file name */
D 5
#define TEXT_STR  20000     /* size of some text strings */
E 5
I 5
#define TEXT_STR  50000     /* size of some text strings */
E 5


/* get main env variables */
char *getenv();
char *clonparms;


/* function prototypes */
I 5
int GetBoardName(char *fname, char *boardName);
E 5


D 5

E 5
/*************************************************************************/
/*************************************************************************/
/*  main program                                                         */
/*************************************************************************/
int
main(int argc, char *argv[])
{
  char fma[F_NAME] = { "" };  /* config file name */
I 4
  char dir[F_NAME];           /* current working directory name */
I 5
  char board[F_NAME];
E 5
E 4
  /* char *det = "pretrig3";  */
  char *mod, *roc;
  int  ii, kk, stt = 0;
  char result[TEXT_STR];      /* string for messages from tcpClientCmd */
  char exename[200];          /* VME discr vxworks exec name */


D 4
/* get the environment variables */
E 4
I 4
/* get the environment variables and current working directory name */
E 4
  clonparms = getenv("CLON_PARMS");
I 4
  getcwd(dir,F_NAME);
D 5
/* printf(" fma 0    >%s< \n", dir); */
E 5
I 5
  /* printf(" dir 0    >%s< \n", dir); */
E 5
E 4


/* check input parameters */
  if ((argc < 3) ||  (argc > 4))                     stt = 1;
  else
  {
    mod = argv[1];
    roc = argv[2];
D 5
    if (argc == 4)  sprintf(fma, "%s", argv[3]);
E 5

D 5
    if      ( (strcmp(mod, "set") != 0) &&
	      (strcmp(mod, "mon") != 0) )            stt = 2;
D 3
    else if ( (strcmp(roc, "pretrig3") != 0) &&
E 3
I 3
    else if ( (strcmp(roc, "pretrig3")  != 0) &&
	      (strcmp(roc, "ec4")       != 0) &&
E 3
I 2
	      (strcmp(roc, "primexts2") != 0) &&
E 2
	      (strcmp(roc, "croctest4") != 0) )      stt = 3;
    if ( (stt == 0) && !strcmp(mod, "set") )
E 5
I 5
    if (argc == 4)
E 5
    {
D 5
      if (argc != 4)                                 stt = 4;
      else if ( fopen(fma, "r") == NULL )
E 5
I 5
      sprintf(fma, "%s", argv[3]);

      if ( fopen(fma, "r") == NULL )
E 5
      {
I 5
	/* printf(" fma 0    >%s< \n", fma); */
E 5
	sprintf(fma, "%s/pretrigger/conf/%s", clonparms, argv[3]);
I 4
D 5
/* printf(" fma 1    >%s< \n", fma); */
E 5
I 5
	/* printf(" fma 1    >%s< \n", fma); */
E 5
E 4
	if ( fopen(fma, "r") == NULL ) {
	  sprintf(fma, "%s/pretrigger/conf/%s.conf", clonparms, roc);
I 4
D 5
/* printf(" fma 2    >%s< \n", fma); */
E 5
I 5
	  /* printf(" fma 2    >%s< \n", fma); */
E 5
E 4
	}
      }
I 4
D 5
     else if (strstr(fma, "/")  == NULL)
E 5
I 5
      else if (strstr(fma, "/")  == NULL)
E 5
      {
	sprintf(fma, "%s/%s", dir, argv[3]);
D 5
/* printf(" fma 3    >%s< \n", fma); */
E 5
I 5
	/* printf(" fma 3    >%s< \n", fma); */
E 5
      }
E 4
D 5
      printf("\n Chosen config file for \"set\" mode:\n   %s\n", fma);
E 5
    }
I 5
    else
      sprintf(fma, "%s/pretrigger/conf/%s.conf", clonparms, roc);


    if      ( (strcmp(mod, "set") != 0) &&
	      (strcmp(mod, "mon") != 0) )            stt = 2;
    else if ( (strcmp(roc, "pretrig3")  != 0) &&
	      (strcmp(roc, "ec4")       != 0) &&
	      (strcmp(roc, "jlab12vme") != 0) &&
	      (strcmp(roc, "primexts2") != 0) &&
	      (strcmp(roc, "croctest2") != 0) &&
	      (strcmp(roc, "croctest4") != 0) )      stt = 3;
E 5
  }
D 4
  /*
  printf(" argv[3]  >%s< \n", argv[3]);
  printf(" fma      >%s< \n", fma);
  */
E 4
I 4
D 5
/*
printf(" argv[3]  >%s< \n", argv[3]);
printf(" fma      >%s< \n", fma);
*/
E 5
I 5
  /* printf(" stt      >%d< \n", stt); */
E 5
E 4

I 5

E 5
  if (stt != 0)
  {
    printf("\n Usage: dimanv");
    printf(" set/mon roc_name [config_file_name] \n ======         \n");
D 5
    printf("   \"set/mon\" - to set or monitor discriminators      \n");
D 2
    printf("   \"roc_name\" - ROC name from the list: pretrig3, croctest4 ...\n");
E 2
I 2
D 3
    printf("   \"roc_name\" - ROC name from the list: pretrig3, primexts2, croctest4 \n");
E 3
I 3
    printf("   \"roc_name\" - ROC name from the list: pretrig3, ec4, primexts2, croctest4 \n");
E 5
I 5
    printf("    \"set/mon\" - to set or monitor discriminators      \n");
    printf("   \"roc_name\" - ROC name from the list:  \n");
    printf("                pretrig3, ec4, jlab12vme, croctest2, croctest4 \n");
E 5
E 3
E 2
    printf("   \"config_file_name\" - any reasonable config file   \n");
    printf("      from  $CLON_PARMS/pretrigger/conf/               \n");
    printf("      not necessary in monitoring mode               \n\n");
    exit(stt);
  }

I 5
  printf("\n Chosen config file: \n   %s\n", fma);
E 5

I 5

  kk = GetBoardName(fma, board);
  if ( kk < 0 )       exit(6);
  /* printf(" board    >%s< \n", board); */


E 5
  if ( !strcmp(mod, "set") )
D 5
    sprintf(exename, "v895SetConfig(\"%s\")", fma);
E 5
I 5
    sprintf(exename, "%sSetConfig(\"%s\")", board, fma);
E 5
  else if (argc == 4)
D 5
    sprintf(exename, "v895MonConfig");
E 5
I 5
    sprintf(exename, "%sMonConfig", board);
E 5
  else
D 5
    sprintf(exename, "v895MonAll");
E 5
I 5
    sprintf(exename, "%sMonAll", board);
  /* printf(" exename  >%s< \n", exename); */
E 5


  memset(result,0,TEXT_STR);
  tcpClientCmd(roc, exename, result);


  printf("\n%s", result);


  exit(stt);   /* end of main() */
}


I 5
int
GetBoardName(char *fname, char *boardName)
{
  FILE   *ff;
  int    ch, kk = -1;
  char   str_tmp[F_NAME], keyword[F_NAME];

  /* Open config file */
  if ( (ff = fopen(fname,"r")) == NULL)
  {
    printf("\n Chosen config file is not readable or no exists: \n%s\n\n",fname);
    return(-2);
  }


  /* Parsing of config file */
  while ((ch = getc(ff)) != EOF)
  {
    if     ( ch == '#' || ch == ' ' || ch == '\t' )
    {
      while (getc(ff) != '\n') {}
    }
    else if( ch == '\n' ) {}
    else
    {
      ungetc(ch,ff);
      fgets(str_tmp, F_NAME, ff);
      sscanf (str_tmp, "%s", keyword);

      if(strcmp(keyword,"BOARD") == 0)
      {
	kk++;
	sscanf (str_tmp, "%*s %s ", boardName);
	/* printf(" kk       >%d< \n", kk); */
	  break;
      }
    }
  }
  fclose(ff);

  return(kk);
}

E 5
#endif

E 1
