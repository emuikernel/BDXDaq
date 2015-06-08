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

static int    Hv_UseThisFile(char *file,
			     char *prompt);

static int    Hv_FirstNonBlank(char *);

Boolean       dontPromptDir = False;

/**
 * Hv_AddPath
 * @purpose Add a path for searching for named files.
 * @param   str   The path to add.
 */

void Hv_AddPath(char *str) {

  static short  current = 0;
  int len = 0;
  int i;
  char *pstr;
 
  if (str == NULL)
    return;

  Hv_InitCharStr(&pstr, str);
  if (pstr == NULL)
		return;
   
  if (current >= Hv_NUMSEARCHPATH) {
    Hv_Warning("Cannot allocate any more paths.");
    return;
  }

/* replace separator chars with the OS appropriate one */

  len = strlen(pstr);

  for (i = 0; i < len; i++) {
	  if ((pstr[i] == '/') || (pstr[i] == '\\')) {
		  pstr[i] = Hv_FileSeparator;
	  }	
  }


  Hv_InitCharStr(&(Hv_ioPaths[current]), pstr);
  Hv_Free(pstr);
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

/* fname should be a complete path starting with FileSeparator */

{
  char  *newfname;

  if (fname[0] != Hv_FileSeparator)
    return NULL;

  if (!Hv_hostName)
    return NULL;

  newfname = (char *)Hv_Malloc(strlen(Hv_hostName) + strlen(fname) + 3);
  strcpy(newfname, Hv_FileSeparatorString);
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
    Hv_Println("DATABASE ERROR: did not find record: %s", RecName);
    Hv_Free(line);
    return False;
  }

  Hv_Free(line);
  return True;
}

/* ---- Hv_FindFileWithExtension -------*/

char           *Hv_FindFileWithExtension(char *extension,
					 char **paths,
					 char *prompt) {
  
  char         *fname = NULL;
  char         *dname;
  int           i, j, nfiles;
  char        **names = NULL;
  int           status;

/* first look in current dir */

  Hv_InitCharStr(&dname, ".");

  dontPromptDir = True;
  nfiles = Hv_AlphaSortDirectory(dname, extension, &names, True, False);
  dontPromptDir = False;

  if (nfiles > 0)
    for (i = 0; i < nfiles; i++) {

      status = Hv_UseThisFile(names[i], prompt);

      if (status == Hv_DONE)
	return NULL;
      else if (status == Hv_OK) {
	Hv_InitCharStr(&fname, names[i]);
	break;
      }
    }


  if (names != NULL)
    for (i = 0; i < nfiles; i++)
      Hv_Free(names[i]);
  Hv_Free(names);

  Hv_Free(dname);


  if (fname != NULL)
    return fname;

/* if fname still NULL, look in paths */

  j = 0;

  while ((fname == NULL) && (j < Hv_NUMSEARCHPATH) && (paths[j] != NULL)) {
    
    if (strlen(paths[j]) > 0) {
      dontPromptDir = True;
      nfiles = Hv_AlphaSortDirectory(paths[j], extension, &names, True, False);
      dontPromptDir = False;
      
      if (nfiles > 0)
	for (i = 0; i < nfiles; i++) {
	  status = Hv_UseThisFile(names[i], prompt);
	  if (status == Hv_DONE)
	    return NULL;
	  else if (status == Hv_OK) {
	    Hv_InitCharStr(&fname, names[i]);
	    break;
	  }
	}
      
      
      if (names != NULL)
	for (i = 0; i < nfiles; i++)
	  Hv_Free(names[i]);
      Hv_Free(names);
      
    } 
    j++;
  }
  
  return fname;
}


/* ---- Hv_UseThisFile --------*/

static int Hv_UseThisFile(char *file,
			  char *prompt) {

  Hv_Widget        dummy, dialog, rowcol, rc;
  int              reason;
  char            *tstr;


  if ((file == NULL) || (prompt == NULL))
    return Hv_DONE;
  

  Hv_VaCreateDialog(&dialog,
		    Hv_TITLE, "File Suggestion",
		    NULL);
  
  rowcol = Hv_DialogColumn(dialog, 6);



  tstr = (char *)Hv_Malloc(strlen(prompt) + 100);
  sprintf(tstr, "Could not find %s where expected. Should I use:", prompt);
  dummy = Hv_StandardLabel(rowcol, tstr, 0);
  Hv_Free(tstr);

  dummy = Hv_StandardLabel(rowcol, file, 0);

/* create the close out buttons */

  dummy = Hv_SimpleDialogSeparator(rowcol);
  rc = Hv_DialogRow(rowcol, 20);

  dummy = Hv_VaCreateDialogItem(rc,
				Hv_TYPE,            Hv_BUTTONDIALOGITEM,
				Hv_FONT,            Hv_fixed2,
				Hv_LABEL,           " OK ",
				Hv_DATA,            (void *)Hv_OK,
				Hv_CALLBACK,        Hv_CloseOutCallback,
				NULL);


  dummy = Hv_VaCreateDialogItem(rc,
				Hv_TYPE,            Hv_BUTTONDIALOGITEM,
				Hv_FONT,            Hv_fixed2,
				Hv_LABEL,           " NO ",
				Hv_DATA,            (void *)Hv_CANCEL,
				Hv_CALLBACK,        Hv_CloseOutCallback,
				NULL);


  dummy = Hv_VaCreateDialogItem(rc,
				Hv_TYPE,            Hv_BUTTONDIALOGITEM,
				Hv_FONT,            Hv_fixed2,
				Hv_LABEL,           " GIVE UP ",
				Hv_DATA,            (void *)Hv_DONE,
				Hv_CALLBACK,        Hv_CloseOutCallback,
				NULL);


  reason = Hv_DoDialog(dialog, NULL);

  Hv_DestroyWidget(dialog);
  return reason;
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



