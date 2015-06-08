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
#define TEXT_STR  50000     /* size of some text strings */


/* get main env variables */
char *getenv();
char *clonparms;


/* function prototypes */
int GetBoardName(char *fname, char *boardName);


/*************************************************************************/
/*************************************************************************/
/*  main program                                                         */
/*************************************************************************/
int
main(int argc, char *argv[])
{
  char fma[F_NAME] = { "" };  /* config file name */
  char dir[F_NAME];           /* current working directory name */
  char board[F_NAME];
  /* char *det = "pretrig3";  */
  char *mod, *roc;
  int  ii, kk, stt = 0;
  char result[TEXT_STR];      /* string for messages from tcpClientCmd */
  char exename[200];          /* VME discr vxworks exec name */


/* get the environment variables and current working directory name */
  clonparms = getenv("CLON_PARMS");
  getcwd(dir,F_NAME);
  /* printf(" dir 0    >%s< \n", dir); */


/* check input parameters */
  if ((argc < 3) ||  (argc > 4))                     stt = 1;
  else
  {
    mod = argv[1];
    roc = argv[2];

    if (argc == 4)
    {
      sprintf(fma, "%s", argv[3]);

      if ( fopen(fma, "r") == NULL )
      {
	/* printf(" fma 0    >%s< \n", fma); */
	sprintf(fma, "%s/pretrigger/conf/%s", clonparms, argv[3]);
	/* printf(" fma 1    >%s< \n", fma); */
	if ( fopen(fma, "r") == NULL ) {
	  sprintf(fma, "%s/pretrigger/conf/%s.conf", clonparms, roc);
	  /* printf(" fma 2    >%s< \n", fma); */
	}
      }
      else if (strstr(fma, "/")  == NULL)
      {
	sprintf(fma, "%s/%s", dir, argv[3]);
	/* printf(" fma 3    >%s< \n", fma); */
      }
    }
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
  }
  /* printf(" stt      >%d< \n", stt); */


  if (stt != 0)
  {
    printf("\n Usage: dimanv");
    printf(" set/mon roc_name [config_file_name] \n ======         \n");
    printf("    \"set/mon\" - to set or monitor discriminators      \n");
    printf("   \"roc_name\" - ROC name from the list:  \n");
    printf("                pretrig3, ec4, jlab12vme, croctest2, croctest4 \n");
    printf("   \"config_file_name\" - any reasonable config file   \n");
    printf("      from  $CLON_PARMS/pretrigger/conf/               \n");
    printf("      not necessary in monitoring mode               \n\n");
    exit(stt);
  }

  printf("\n Chosen config file: \n   %s\n", fma);


  kk = GetBoardName(fma, board);
  if ( kk < 0 )       exit(6);
  /* printf(" board    >%s< \n", board); */


  if ( !strcmp(mod, "set") )
    sprintf(exename, "%sSetConfig(\"%s\")", board, fma);
  else if (argc == 4)
    sprintf(exename, "%sMonConfig", board);
  else
    sprintf(exename, "%sMonAll", board);
  /* printf(" exename  >%s< \n", exename); */


  memset(result,0,TEXT_STR);
  tcpClientCmd(roc, exename, result);


  printf("\n%s", result);


  exit(stt);   /* end of main() */
}


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

#endif

