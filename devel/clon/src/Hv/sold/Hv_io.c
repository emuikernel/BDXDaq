/*
==================================================================================
The Hv library was developed at CEBAF under contract to the
Department of Energy and is the property of the U.S. Government.

It may be used to develop commercial software, but the Hv
source code and/or compiled modules/libraries may not be sold.

Questions or comments should be directed to heddle@cebaf.gov
==================================================================================
*/


#include "Hv.h"

/*------- local prototypes -------*/

static int    Hv_FirstNonBlank(char *);


/*------ Hv_AlphaSortDirectory --------*/

int Hv_AlphaSortDirectory(char *dirname,
			  char *extension,
			  char ***names,
			  Boolean prepend,
			  Boolean remext)
/*
   Boolean  prepend   if true, then prepend directory
   Boolean  remext    if true, then remove extension
*/

{
  DIR     *dp;
  int     num = 0;
  int     i, j;
  FILE    *fp;
  char    fname[256];
  char    **narry;
  char    *tstr;

/* open the directory and count the files that match */

  if ((dp = opendir(dirname)) == NULL)
    return 0;

  while(Hv_GetNextFile(dp, dirname, extension, &fp, fname))
    if (fp != NULL) {
      num++;
      fclose(fp);
    }
  
  closedir(dp);

/* now allocate the names array */

  *names = (char **)Hv_Malloc(num * sizeof(char *));
  narry = *names;

/* now fill the names array */

  dp = opendir(dirname);
  i = 0;

  while(Hv_GetNextFile(dp, dirname, extension, &fp, fname))
    if (fp != NULL) {

      if (prepend) {
	narry[i] = (char *)Hv_Malloc(strlen(extension) +
				     strlen(dirname) +
				     strlen(fname) + 6);
	strcpy(narry[i], dirname);
	if (dirname[strlen(dirname)-1] != '/')
	  strcat(narry[i], "/");
	strcat(narry[i], fname);
      }
      else {
	narry[i] = (char *)Hv_Malloc(strlen(extension) +
				     strlen(fname) + 6);
	strcpy(narry[i], fname);
      }

/* remove extension ? */

      if (!remext && (extension != NULL)) {
	if (extension[0] != '.')
	  strcat(narry[i], ".");
	strcat(narry[i], extension);
      }

      fclose(fp);
      i++;
    }
  
  closedir(dp);

/* now sort the names array */
  
  if (num > 1) {
    for (i = 0; i < (num - 1); i++)
      for (j = i+1; j < num; j++)
	if (strcmp(narry[i], narry[j]) > 0) {
	  tstr = narry[i];
	  narry[i] = narry[j];
	  narry[j] = tstr; 
	}
  }
  
  return num;
}


/* --- Hv_GetNextFile --- */

Boolean  Hv_GetNextFile(DIR *dp,
			char *dirname,
			char *extension,
			FILE **fp,
			char *fname)

{
   struct dirent   *dir;
   char            *rfname;
   char             *sstr;
   
   
   *fp = NULL;

   dir = readdir(dp);

   if (dir == NULL)
     return False;
  

/* skip removed files */

    if (dir->d_ino == 0)
      return True;

    if (strstr(dir->d_name, extension) != 0) {

/* rfname is "real file name", may have to prepend "dirname" and "/" */

      rfname = (char *)Hv_Malloc(strlen(dir->d_name) + strlen(dirname) + 5);
      if ((dir->d_name[0] != '.') && (dir->d_name[0] != '/')) {
        strcpy(rfname, dirname);
	strcat(rfname, "/");
        strcat(rfname, dir->d_name);
      }
      else
        strcpy(rfname, dir->d_name);

/* remove file extension */

      strcpy(fname, dir->d_name);
      sstr = strstr(fname, extension);
      if (sstr != NULL)
        sstr[0] = '\0';

      *fp = fopen(rfname, "r");
/*      fprintf(stdout, "file name: %s   fp = %10d\n", rfname, *fp);*/

      Hv_Free(rfname);
    }  /* end of matched extension */
      
      
   return True;
}

/*----- Hv_OpenDirectory ----*/

DIR  *Hv_OpenDirectory(char *dname)

{
  DIR  *dp;
  char *tstr;
  char *tname;
  
  if ((dp = opendir(dname)) == NULL) {
    tstr = (char *)Hv_Malloc(120 + strlen(dname));
    sprintf(tstr,  "Can't find %s. Do you want to look for it?",  dname);
    
    if (Hv_Question(tstr)) {
      sprintf(tstr,  "Please locate %s.",  dname);
      tname = Hv_DirectorySelect(tstr, "*");
      dp = opendir(tname);
      Hv_Free(tname);
    }  /* end of yes response */
    
    Hv_Free(tstr);
  } /*end dp == NULL*/
  return dp;
}

/* ----- Hv_AddPath ------*/

void Hv_AddPath(char *str)

{
  static short  current = 0;

  if (str == NULL)
    return;

  if (current >= Hv_NUMSEARCHPATH) {
    Hv_Warning("Cannot allocate any more paths.");
    return;
  }

  Hv_InitCharStr(&(Hv_ioPaths[current]), str);
  current++;
}

/*----- Hv_NotAComment -------*/

Boolean  Hv_NotAComment(char *line)

/* Returns nonzero result if line is NOT a comment. For Hv purposes,
   comments are lines whose first non-white space is a '!' or
   blank lines */

{
  int    c;
  
  c = Hv_FirstNonBlank(line);
  return((c != '!') && (c != 0));
}

/*----- Hv_IsAComment -------*/

Boolean  Hv_IsAComment(char *line)

/* Returns nonzero result if line IS a comment. For shell purposes,
   comments are lines whose first non-white space is a '!' or
   blank lines */
{
  int    c;
  
  c = Hv_FirstNonBlank(line);
  return((c == '!') || (c == 0));
}

/* ------ Hv_GetNextNonComment -------*/

char  *Hv_GetNextNonComment(FILE *fp,
			    char *line)

/* returns the next non comment line (copied into
   line) or returns NULL (leaving line alone) if 
   not found */

{
  char   *localline;

  localline = (char *)Hv_Malloc(Hv_LINESIZE);
  while (fgets(localline, Hv_LINESIZE, fp)) {
    if(Hv_NotAComment(localline)) {
      strcpy(line, localline);
      Hv_Free(localline);
      return line;
    }
  }
  Hv_Free(localline);
  return  NULL;
}

/*----- Hv_FindRecord -----*/

char   *Hv_FindRecord(FILE   *fp,
		      char   *recname,
		      Boolean Rewind,
		      char   *line)

/* returns nonzero if RecName is found in the file. CASE INSENSITIVE
   Rather primative database function. Leaves fp pointing at beginning
   of line following line containing RecName.*/

{
  char        *RecName;
  char        *localline;
  Boolean     found;
  char        *therecname;
  int         reclen;
  char        savechar;

/* copy the recname, in case someone pass a const str */

  if (fp == NULL)
    return NULL;

  Hv_InitCharStr(&RecName, recname);

  reclen = strlen(RecName);
  localline = (char *) Hv_Malloc(Hv_LINESIZE);      /*beg for memory*/
  Hv_UpperCase(RecName);                    /*for case insensitivity*/

/* all Hv_recnames should start with a "[" */

  if (RecName[0] == '[')
    Hv_InitCharStr(&therecname, RecName);
  else {
    therecname = (char *)Hv_Malloc(strlen(RecName) + 2);
    strcpy(therecname + 1, RecName);
    therecname[0] = '[';
  }
   
  found = False;                          /*assume failure*/
  
  if (Rewind)
    rewind(fp);                            /*reset file*/

  while((fgets(localline, Hv_LINESIZE, fp)) != NULL) {
    if (Hv_NotAComment(localline)) {

/* avoid converting whole line to upper case */

      savechar = localline[reclen];
      localline[reclen] = '\0';
      Hv_UpperCase(localline);
      localline[reclen] = savechar;

      found = (strstr(localline, therecname) != NULL);
      if (found) {
	strcpy(line, localline);
	Hv_Free(localline);
	Hv_Free(therecname);
	Hv_Free(RecName);
	return line;
      }
    }
  }
  
  Hv_Free(localline);
  Hv_Free(therecname);
  Hv_Free(RecName);
  return NULL;
}

/* ---- Hv_ReadNextLine ---- */

int   Hv_ReadNextLine(FILE *fp,
		      char *line,
		      char *srec,
		      char *erec)

/* Reads next line -- DOES NOT EXCLUDE COMMENTS */

{
  int     val;    /*returned*/
  char    *result;
  char    *line2;
  char    *startrec, *endrec;

  Hv_InitCharStr(&startrec, srec);
  Hv_InitCharStr(&endrec,   erec);

  Hv_UpperCase(startrec);
  Hv_UpperCase(endrec);

  result = fgets(line, Hv_LINESIZE, fp);
  
  if (!result)
    val = Hv_FOUNDEOF;
  else {
    Hv_InitCharStr(&line2, line);
    Hv_UpperCase(line2);
    if (strstr(line2, startrec) != NULL)
      val = Hv_FOUNDSTARTREC;
    else if (strstr(line2, endrec) != NULL)
      val = Hv_FOUNDENDREC;
    else
      val = 0;
    Hv_Free(line2);
  }
  
  Hv_Free(startrec);
  Hv_Free(endrec);
  return val;
}

/*------- Hv_RecordLineCount ------*/

void Hv_RecordLineCount(FILE *fp,
			char *StartRec,
			char *EndRec,
			int  *numlines,
			int  *numnclines)

/* numnclines  number of  non comment lines */

{
   char    *line;
#ifdef sparc
   long   offset;
#else
   fpos_t  pos;
#endif

   int     result;


#ifdef sparc
   offset = ftell(fp);
#else   
   result = fgetpos(fp, &pos);
#endif

   line = (char *)Hv_Malloc(Hv_LINESIZE);
   *numlines = 0;
   *numnclines = 0;

  while (Hv_ReadNextLine(fp, line, StartRec, EndRec) == 0) {
    *numlines += 1;
    if (Hv_NotAComment(line))
      *numnclines += 1;
  }
  
  Hv_Free(line);
#ifdef sparc
   fseek(fp, offset, 0);
#else 
   result = fsetpos(fp, &pos);
#endif
}

/*------- Hv_NextRecordSize ------*/

void Hv_NextRecordSize(FILE  *fp,
		       char  *StartRec,
		       char  *EndRec,
		       int   *numlines,
		       int   *numchars)

{
  char  *line;
  
  line = (char *)Hv_Malloc(Hv_LINESIZE);
  *numlines = 0;
  *numchars = 0;

  while (Hv_ReadNextLine(fp, line, StartRec, EndRec) == 0) {
    *numlines += 1;
    *numchars += strlen(line) + 1;
  }
  
  Hv_Free(line);
}

/*------- Hv_PrependHostToFilename -----*/

char  *Hv_PrependHostToFilename(char *fname)

/* fname should be a complete path starting with "/" */

{
  char  *newfname;

  if (fname[0] != '/')
    return NULL;

  if (!Hv_hostName)
    return NULL;

  newfname = (char *)Hv_Malloc(strlen(Hv_hostName) + strlen(fname) + 3);
  strcpy(newfname, "/");
  strcat(newfname, Hv_hostName);
  strcat(newfname, fname);
  return newfname;
}

/*---- Hv_ReportDatabaseSearch ------ */

Boolean Hv_ReportDatabaseSearch(FILE   *fp,
				char   *RecName,
				Boolean Rewind)


/*Simply calls the database search program and reports
  on its search*/

{
  char   *line;

  line = (char *)Hv_Malloc(Hv_LINESIZE);

  if(!Hv_FindRecord(fp, RecName, Rewind, line)) {
    fprintf(stderr, "DATABASE ERROR: did not find record: %s\n", RecName);
    Hv_Free(line);
    return False;
  }

  Hv_Free(line);
  return True;
}


/* ---- Hv_LookForNamedFile -------*/

FILE  *Hv_LookForNamedFile(char  *filename,
			   char **paths,
			   char  *mode)

{
  FILE   *fp;
  int     i = 0;
  char   *tname;
  char   *tnamehost;

  fp = fopen(filename, mode);      /*open database file*/

  while ((fp == NULL) && (paths[i] != NULL)) {
    tname = (char *)Hv_Malloc(strlen(paths[i]) + strlen(filename) + 5);
    strcpy(tname, paths[i]);
    strcat(tname, "/");
    strcat(tname, filename);
    fp = fopen(tname, mode);
    if (fp == NULL) {
      tnamehost = Hv_PrependHostToFilename(tname);
      if (tnamehost) { 
	fp = fopen(tnamehost, mode);
	Hv_Free(tnamehost);
      }
    }
    Hv_Free(tname);
    i++;
  }
  
  return  fp;
}

/*----- Hv_UpperCase ------*/

void   Hv_UpperCase(line)

/*converts text to upper case. Useful so that database
  searches can be case insensitive*/

char *line;

{
  while((*line = toupper(*line)) != '\0')
    line++;
}


/*---- Hv_FirstNonBlank -----*/

static int    Hv_FirstNonBlank(char *line)

/*returns the first non white-space character of a line, or NULL if all blanks.
  Useful so that we can check whether the line was a comment line.*/
       
{
  while (isspace(*line) && (*line != '\0'))
    line++;
	 
  if (*line != '\0')
    return(*line);
  else
    return 0;
}


/*-------- Hv_NextTextSegment ------*/

char   *Hv_NextTextSegment(char **line)

/*NOTE: the returned segment MUST BE FREED BY THE CALLER */

{
  char    *temp;
  char    *result;
  int      n;
  Boolean stopquote = False;

  while (isspace(**line) && (**line != '\0'))
    (*line)++;

  if (**line == '\0')
    return NULL;

/* if Hv_quoteDelimitsSegment is set AND the first char
   was a quote, we will terminate on the next quote,
   else terminate on next space */

  stopquote = (Hv_quoteDelimitsSegment && (**line == '"'));

  if (stopquote) {
    (*line)++;
    temp = *line;
    while ((*temp != '"') && (*temp != '\0'))
      temp++;

    if (*temp == '"')
      *temp = ' ';
  }
  else
    for (temp = *line; !(isspace(*temp)) && (*temp != '\0'); temp++)
      ;


  n = temp - *line;
  result = (char *)Hv_Malloc(n + 1);
  strncpy(result, *line, n);
  result[n] = '\0';

/*fprintf(stdout, "segment = [%s]\n", result); */
  *line = temp;
  return result;
}


/*------- Hv_CountSegments ------*/

int Hv_CountSegments(char *line)

{
  char  *temp, *segment;
  int   count = 0;

  temp = line;
  
  while ((segment = Hv_NextTextSegment(&temp)) != NULL) {
    Hv_Free(segment);
    count++;
  }
  
  return count;
}



