/*************************************************************************\
* Copyright (c) 1994-2004 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 1997-2003 Southeastern Universities Research Association,
* as Operator of Thomas Jefferson National Accelerator Facility.
* Copyright (c) 1997-2002 Deutches Elektronen-Synchrotron in der Helmholtz-
* Gemelnschaft (DESY).
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

#define DEBUG_OPEN 0

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "Strip.h"
#include "StripMisc.h"
#include "StripDAQ.h"

#ifdef WIN32
# include <direct.h> /* for getcwd (usually in sys/parm.h or unistd.h) */
#endif

#define CPU_CURVE_NAME          "CPU_Usage"
#define CPU_CURVE_EGU           "%"
#define CPU_CURVE_PRECISION     2
#define CPU_CURVE_MIN           0.00
#define CPU_CURVE_MAX           100.00

Strip strip;
StripDAQ strip_daq;              /* the Strip data source */

#ifdef USE_OLD_FILE_SEARCH
static FILE     *StripTool_open_file    (char *, char *, int, char *, char *);
#else
static FILE     *StripTool_open_file (char *file_name, char *path_used,
                  int size_path_used);
static          Boolean extractStringBetweenColons(char *input, char *output,
                  int  startPos, int  *endPos);
static int      isPath(const char *fileString);
static int      convertNameToFullPath(const char *name, char *pathName,
                  int nChars);
#endif

static int      request_connect         (StripCurve, void *);
static int      request_disconnect      (StripCurve, void *);

static double   get_cpu_usage           (void *);

int StripTool_main (int argc, char *argv[])
{
  int           status;
  FILE          *f;
  int           configFileRead = 0;

#if 0
  /* KE: Debugging */
  print("SCFGMASK_TERMINATOR=%d\n",SCFGMASK_TERMINATOR);
  print("SCFGMASK_FIRST_ELEMENT=%d\n",SCFGMASK_FIRST_ELEMENT);
  print("SCFGMASK_LAST_ELEMENT=%d\n",SCFGMASK_LAST_ELEMENT);
  print("SCFGMASK_NUM_ELEMENTS=%d\n",SCFGMASK_NUM_ELEMENTS);
  print("STRIPCFGMASK_NBYTES=%d\n",STRIPCFGMASK_NBYTES);
  print("STRIPCURVE_COMMENT=%d\n",STRIPCURVE_COMMENT);
  print("STRIPCURVE_COMMENT_SET=%d\n",STRIPCURVE_COMMENT_SET);
#endif  

  /* WIN32 initialization */
#if defined(WIN32) & 0
  // This has not been needed since at least Exceed 7 and the
  // prototype is no longer available in Exceed 12
    HCLXmInit();
#endif

    /* create and initialize the Strip structure */
  if (!(strip = Strip_init (&argc, argv, tmpfile())))
  {
    fprintf (stderr, "%s: Unable to initialize Strip\n", argv[0]);
    exit (1);
  }
  
  /* fire up the data source  */
  if (!(strip_daq = StripDAQ_initialize (strip)))
  {
    fprintf
	(stderr, "%s: Unable to initialize Strip data source\n", argv[0]);
    status = 1;
    goto done;
  }
  
  /* set the strip callback functions */
  Strip_setattr
    (strip,
     STRIP_CONNECT_FUNC,        request_connect,
     STRIP_DISCONNECT_FUNC,     request_disconnect,
     STRIP_DAQ,                 strip_daq,
     0);

  /* now load a config file if requested */
  if (argc >= 2)
  {
#ifdef USE_OLD_FILE_SEARCH
    f = StripTool_open_file
      (getenv(STRIP_FILE_SEARCH_PATH_ENV), path_used, sizeof(path_used),
       argv[1], "r");

    if (f)
    {
	fprintf
	  (stdout, "StripTool: Using config file, %s.\n", argv[1]);
	Strip_readconfig (strip, f, SCFGMASK_ALL, argv[1]);
	configFileRead = 1;
	fclose (f);
    }
    else fprintf
           (stdout,
		 "StripTool: Can't open %s; using default config.\n",
		 argv[1]);
#else
    char path_used[STRIP_PATH_MAX];
    f = StripTool_open_file (argv[1], path_used, sizeof(path_used));
    
    if (f)
    {
	fprintf
	  (stdout, "StripTool: Using config file %s.\n", path_used);
	Strip_readconfig (strip, f, SCFGMASK_ALL, path_used);
	configFileRead = 1;
	fclose (f);
    }
    else
    {
	fprintf
	  (stdout,
	    "StripTool: Can't open %s; using default config.\n",
	    argv[1]);
    }
  }
#endif
  
  /* look for and load defaults file if none sucessfully loaded yet */
  if (!configFileRead && (f = fopen (STRIP_DEFAULT_FILENAME, "r")) != NULL)
  {
    fprintf
	(stdout, "StripTool: Using default file %s.\n",
	  STRIP_DEFAULT_FILENAME);
    Strip_readconfig (strip, f, SCFGMASK_ALL, STRIP_DEFAULT_FILENAME);
    configFileRead = 1;
    fclose (f);
  }
  
  status = 0;
  Strip_go (strip);

  done:
  StripDAQ_terminate (strip_daq);
  Strip_delete (strip);
  return status;
}


/*
 *      StripOpenFile()
 *      (attempt to open a file using a path string of the same format 
 *      as the "PATH" environment variable)
 *
 *      2nd and 3rd arguments can be omitted by setting them to NULL and 0
 *      respectively.
 *
 *      ppath_used is left unmodified if no file could be opened.
 *      Otherwise, ppath_used will contain the actual path used 
 *      to open a file.
 *
 *      Copied from open_display.c written by Jeff Hill for edd/dm.
 *
 *      Nick Rees 16 June 1999
 */

#ifdef UNIX
#define errnoSet(E) (errno = (E))
extern int errno;
#endif

#ifdef USE_OLD_FILE_SEARCH
static FILE * StripTool_open_file    ( char * pld_display_path,
                                       char * ppath_used,
                                       int    length_path_used,
                                       char * pfile_name,
                                       char * mode )
{     
  FILE *  fd;
  char    *pcolon = NULL;
  char    *pslash;
  int     name_length;
  int     path_length;
  char    pname[128];

  if(pfile_name == NULL){
    errnoSet(EINVAL);
    return NULL;
  }

  name_length = strlen(pfile_name);
  if(name_length+1 > sizeof(pname)){
    errnoSet(EINVAL);
    return NULL;
  }

  if(name_length == 0){
    errnoSet(EINVAL);
    return NULL;
  }

  pslash = strrchr(pfile_name, '/');
  if(pslash!=NULL || pld_display_path==NULL){
    fd = fopen(pfile_name, mode);
    if(ppath_used && fd != NULL){
      if(pslash){
        path_length = pslash-pfile_name;
      }
      else{
        path_length = 0;
      }

      if(path_length+1>length_path_used){
        fclose(fd);
        errnoSet(EINVAL);
        return NULL;
      }
      memcpy(ppath_used, pfile_name, path_length);
      ppath_used[path_length] = 0;
    }
    /*
     * errno is set by open
     *
     */
    return fd;
  }

  while(1){

    if(pcolon){
      pld_display_path = pcolon+1;
    }
                        
    pcolon = strchr(pld_display_path, ':');
    if(pcolon){     
      path_length = pcolon-pld_display_path;
    }
    else{
      path_length = strlen(pld_display_path);
    }


    if(path_length+name_length+2 > sizeof(pname))
      continue;
    strncpy(
      pname, 
      pld_display_path, 
      path_length);

    if(pld_display_path[path_length-1] != '/'){
      pname[path_length] = '/';
      path_length++;
    }
                        
    strncpy(
      &pname[path_length], 
      pfile_name,
      name_length+1);

#ifdef DEBUG
    printf("Attempting to open [%s]\n", pname);
#endif
    fd = fopen(pname, mode );
    if(fd != NULL){
      if(ppath_used){
        if(path_length+1>length_path_used){
          fclose(fd);
          errnoSet(EINVAL);
          return NULL;
        }
        memcpy(ppath_used, pname, path_length);
        ppath_used[path_length] = 0;
      }
      return fd;
    }

    /*
     * errno is set by open
     *
     */
    if(pcolon == NULL){
      return NULL;
    }
  }
}
#else /* #ifdef USE_OLD_FILE_SEARCH	*/
/* Function to open a file.  If unsuccessful, try to attach path of
 * each directory in STRIP_FILE_SEARCH_PATH.  If successful, the full
 * path name used is returned in path_used, limited by the given
 * size_path_used. */
static FILE *StripTool_open_file (char *file_name, char *path_used,
  int size_path_used)
{
  FILE *filePtr;
  int startPos;
  char fullPathName[STRIP_PATH_MAX], dirName[STRIP_PATH_MAX];
  char *dir;
  
#ifdef WIN32
  convertDirDelimiterToWIN32(file_name);
#endif
  
#if DEBUG_OPEN
  print("\nStripTool_open_file\n"
    "  file_name=%s\n",
    file_name?file_name:"NULL");
#endif	
  
  /* Try to open with the given name first
   *   (Will be in cwd if not an absolute pathname) */
  if(path_used && size_path_used) *path_used='\0';
  filePtr = fopen(file_name,"r");
  if(filePtr) {
    if(path_used && size_path_used) {
	convertNameToFullPath(file_name, path_used, size_path_used);
    }
#if DEBUG_OPEN
    print("  [Direct] %s\n",file_name?file_name:"NULL");	
#endif	
    return(filePtr);
  }
  
  /* If the name is a path, then we can do no more */
  if(isPath(file_name)) {
#if DEBUG_OPEN
    print("  [Fail:IsPath] %s\n",file_name?file_name:"NULL");
#endif	
    return(NULL);
  }
  
  /* Look in STRIP_FILE_SEARCH_PATH directories */
  dir = getenv(STRIP_FILE_SEARCH_PATH_ENV);
  if(dir != NULL) {
    startPos = 0;
    while(filePtr == NULL &&
	extractStringBetweenColons(dir,dirName,startPos,&startPos)) {
	strncpy(fullPathName, dirName, STRIP_PATH_MAX);
	fullPathName[STRIP_PATH_MAX-1] = '\0';
#ifdef WIN32
	convertDirDelimiterToWIN32(fullPathName);
#endif
	strcat(fullPathName, STRIP_DIR_DELIMITER_STRING);
	strcat(fullPathName, file_name);
#if DEBUG_OPEN
	print("  [SFSP:Try] %s\n",fullPathName);
#endif	
	filePtr = fopen(fullPathName, "r");
	if(filePtr) {
	  if(path_used && size_path_used) {
	    strncpy(path_used, fullPathName, size_path_used);
	    path_used[size_path_used-1]='\0';
	  }
#if DEBUG_OPEN
	  print("  [STSP:Found] %s\n",fullPathName);
#endif	
	  return (filePtr);
	}
    }
  }
  
  /* Not found */
#if DEBUG_OPEN
  print("  [Fail:NotFound] %s",file_name?file_name:"NULL");
#endif	
  return (NULL);
}


/*
 *  extract strings between colons from input to output
 *    this function works as an iterator...
 */
static Boolean extractStringBetweenColons(char *input, char *output,
  int  startPos, int  *endPos)
{
  int i, j;
  
  i = startPos; j = 0;
  while(input[i] != '\0') {
    if(input[i] != STRIP_PATH_DELIMITER) {
	output[j++] = input[i];
    } else break;
    i++;
  }
  output[j] = '\0';
  if(input[i] != '\0') {
    i++;
  }
  *endPos = i;
  if(j == 0)
    return(False);
  else
    return(True);
}

/* Check if the name is a pathname */
static int isPath(const char *fileString)
{
  int pathTest;
  
  /* Look for the appropriate leading character */
#if defined(VMS)
  pathTest = (strchr(fileString, '[') != NULL);
#elif defined(WIN32)
  /* A drive specification will also indicate a full path name */
  pathTest = (fileString[0] == STRIP_DIR_DELIMITER_CHAR ||
    fileString[1] == ':');
#else
  pathTest = (fileString[0] == STRIP_DIR_DELIMITER_CHAR);
#endif
  
  /* Note: Do not assume names starting with . or .. are full path
     names.  Paths can be prepended to these as for the related
     display, so we don't want to eliminate them. */
  
  return(pathTest);
}  

/* Convert name to a full path name.  Returns 1 if converted, 0 if
   failed. */
static int convertNameToFullPath(const char *name, char *pathName, int nChars)
{
  int retVal = 1;
  
  if(isPath(name)) {
    /* Is a full path name */
    strncpy(pathName, name, nChars);
    pathName[nChars-1] = '\0';
  } else {
    char currentDirectoryName[STRIP_PATH_MAX];
    
    /* Insert the path before the file name */
    currentDirectoryName[0] = '\0';
    getcwd(currentDirectoryName, STRIP_PATH_MAX);
    
    if(strlen(currentDirectoryName) + strlen(name) <
	(size_t)(nChars - 1)) {
	strcpy(pathName, currentDirectoryName);
#ifndef VMS
	strcat(pathName, STRIP_DIR_DELIMITER_STRING);
#endif
	strcat(pathName, name);
    } else {
	/* Hopefully won't get here */
	strncpy(pathName, name, nChars);
	pathName[nChars-1] = '\0';
	retVal = 0;
    }
  }
  
  return retVal;
}
#endif /* #ifdef USE_OLD_FILE_SEARCH */

static int      request_connect         (StripCurve curve, void *BOGUS(1))
{
  int ret_val = 0;
  
  if (strcmp
      ((char *)StripCurve_getattr_val (curve, STRIPCURVE_NAME), CPU_CURVE_NAME)
      == 0)
  {
    StripCurve_setattr
      (curve,
       STRIPCURVE_NAME,       CPU_CURVE_NAME,
       STRIPCURVE_EGU,        CPU_CURVE_EGU,
       STRIPCURVE_PRECISION,  CPU_CURVE_PRECISION,
       STRIPCURVE_MIN,        CPU_CURVE_MIN,
       STRIPCURVE_MAX,        CPU_CURVE_MAX,
       STRIPCURVE_SAMPLEFUNC, get_cpu_usage,
       0);
    Strip_setconnected (strip, curve);
    ret_val = 1;
  }
  else ret_val = StripDAQ_request_connect (curve, strip_daq);
  return ret_val;
}


static int      request_disconnect      (StripCurve curve, void *BOGUS(1))
{
  int ret_val = 0;
  
  if (strcmp
      ((char *)StripCurve_getattr_val (curve, STRIPCURVE_NAME), CPU_CURVE_NAME)
      == 0)
    ret_val = 1;
  else ret_val = StripDAQ_request_disconnect (curve, strip_daq);

  return ret_val;
}


static double   get_cpu_usage           (void *BOGUS(1))
{
  static int                    initialized = 0;
  static struct timeval         time_a = { 0, 0 };
  static struct timeval         time_b = { 0, 0 };
  static struct timeval         t = { 0, 0 };
  static clock_t                clock_a = (clock_t)-1;
  static clock_t                clock_b = (clock_t)-1;
  static double                 cpu_usage = 0;
  
  if (!initialized)
  {
    get_current_time (&time_a);
    clock_a = clock();
    initialized = 1;
  }

  get_current_time (&time_b);
  clock_b = clock();

  /* when the time value wraps, just return the previous percentage */
  if (clock_b >= clock_a)
  {
    cpu_usage = subtract_times (&t, &time_a, &time_b);
    cpu_usage = (double)(clock_b - clock_a) / (double)CLOCKS_PER_SEC;
    cpu_usage /= subtract_times (&t, &time_a, &time_b);
    cpu_usage *= 100;
  }
  
  clock_a = clock_b;
  time_a = time_b;
  
  return cpu_usage;
}

/* **************************** Emacs Editing Sequences ***************** */
/* Local Variables: */
/* tab-width: 6 */
/* c-basic-offset: 2 */
/* c-comment-only-line-offset: 0 */
/* c-indent-comments-syntactically-p: t */
/* c-label-minimum-indentation: 1 */
/* c-file-offsets: ((substatement-open . 0) (label . 2) */
/* (brace-entry-open . 0) (label .2) (arglist-intro . +) */
/* (arglist-cont-nonempty . c-lineup-arglist) ) */
/* End: */
