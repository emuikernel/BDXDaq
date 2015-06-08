#ifdef Linux

/********************************************************/
/*  rcc_error package 					*/
/*  							*/
/*  17. Feb. 1998   HP Beck (iom_error) 		*/
/*   7. Nov. 2001   MAJO  Adapted to RCC environment 	*/
/********************************************************/

#include <string.h>     /*  memcpy */
#include <stdio.h>      /*  printf */
#include "rcc_error.h" 

/*  initialise an array of function pointers, one entry per possible */
/*  package ID (0...0xff) to hold the xxx_err_get functions from the */
/*  various packages.  */

static err_type (* get_err[0x100])(err_pack,err_str,err_str) = {0}; 
static int rcc_error_opened = 0;

static err_type rcc_error_err_get(err_pack, err_str, err_str);


/***************************/
err_type rcc_error_open(void) 
/***************************/
{
  rcc_error_opened++;
  return(RCC_ERROR_RETURN(0, ERCC_OK));
}


/****************************/
err_type rcc_error_close(void) 
/****************************/
{
  if (rcc_error_opened) 
  {
    rcc_error_opened--;
    return(RCC_ERROR_RETURN(0, ERCC_OK)); 
  } 
  else 
    return(RCC_ERROR_RETURN(0, ERCC_NOTOPEN));
}


/*****************************************************************************/
err_type iom_error_init(err_pid pid, err_type (*f)(err_pack, err_str, err_str))
/*****************************************************************************/
{
  get_err[pid & 0xff] = f;  /*  keep the corresponding function pointer */
  return(RCC_ERROR_RETURN(0, ERCC_OK));
}

/*****************************************************************************/
err_type rcc_error_init(err_pid pid, err_type (*f)(err_pack, err_str, err_str)) 
/*****************************************************************************/
{
  get_err[pid & 0xff] = f;  /*  keep the corresponding function pointer */
  return(RCC_ERROR_RETURN(0, ERCC_OK));
}

 
/**************************************************/
err_type rcc_error_print(FILE* stream, err_type err)
/**************************************************/
{
  err_str  Maj_Pid_str, Maj_en_str, Min_Pid_str, Min_en_str;
  err_type state;
  int      flag;

  state = 0;
  flag  = 0;

  if (err) 
  {        /*  there is something to be printed */
    if (fprintf(stream, RCC_ERROR_MAJOR(err) == RCC_ERROR_MINOR(err) ? 
		"error: 0x%-8x => major: " : "error: 0x%-8x => minor: " , err) > 0) 
    {
      state = rcc_error_get(err, Maj_Pid_str, Maj_en_str,Min_Pid_str, Min_en_str);

      if (strlen(Maj_Pid_str)) 
        strcat(Maj_Pid_str,": ");
      if (strlen(Min_Pid_str)) 
        strcat(Min_Pid_str,": ");

      fprintf(stream, "Error %3u in package %3u => %s%s\n",RCC_ERROR_MINOR_ERRNO(err),
	      RCC_ERROR_MINOR_PID(err), Min_Pid_str, Min_en_str);
    } 
    else 
      flag = 1;   /*  There was a problem with fprintf */

    /*  test whether we are on a higher level */
    /*  and therefore have to print 2 lines  */

    if (RCC_ERROR_MAJOR(err) != RCC_ERROR_MINOR(err)) 
    { 
      if (fprintf(stream, "                  => major: ") >0) 
	fprintf(stream, "Error %3u in package %3u => %s%s\n", RCC_ERROR_MAJOR_ERRNO(err), 
                RCC_ERROR_MAJOR_PID(err), Maj_Pid_str, Maj_en_str);
      else 
	flag = 1;   /*  There was a problem with fprintf */
    }
  }  
  else 
  {        /*  the function was invoked with a zero */
    if (fprintf(stream, "error: 0x0        => All OK\n") < 0) 
      flag = 1;
  }
  
  if (flag) 
    return(RCC_ERROR_RETURN(0, ERCC_STREAM));
  else
    return(state); /*  blindly pass MAJOR and MAINOR error as gotten from the rcc_error_get call */
}


/*************************************************/
err_type rcc_error_string(char *text, err_type err)
/*************************************************/
{
  err_str  Maj_Pid_str, Maj_en_str, Min_Pid_str, Min_en_str;
  err_type state;
  int      flag, len, nchars;

  state = 0;
  flag  = 0;
  len   = 0;

  if (err) 
  {        
    nchars = sprintf(text + len, RCC_ERROR_MAJOR(err) == RCC_ERROR_MINOR(err) ? 
                     "error: 0x%-8x => major: " : "error: 0x%-8x => minor: " , err);
    if(nchars > 0) 
    {
      len += nchars;
      state = rcc_error_get(err, Maj_Pid_str, Maj_en_str,Min_Pid_str, Min_en_str);

      if (strlen(Maj_Pid_str)) 
        strcat(Maj_Pid_str,": ");
      if (strlen(Min_Pid_str)) 
        strcat(Min_Pid_str,": ");

      len += sprintf(text + len, "Error %3u in package %3u => %s%s\n",RCC_ERROR_MINOR_ERRNO(err),
	             RCC_ERROR_MINOR_PID(err), Min_Pid_str, Min_en_str);
    } 
    else 
      flag = 1;   /*  There was a problem with sprintf */

    /*  test whether we are on a higher level and therefore have to print 2 lines  */

    if (RCC_ERROR_MAJOR(err) != RCC_ERROR_MINOR(err)) 
    { 
      nchars = sprintf(text + len, "                  => major: ");
      if (nchars >0) 
      {
        len += nchars;
	len += sprintf(text + len, "Error %3u in package %3u => %s%s\n", RCC_ERROR_MAJOR_ERRNO(err), 
                       RCC_ERROR_MAJOR_PID(err), Maj_Pid_str, Maj_en_str);
      }
      else 
	flag = 1;   /*  There was a problem with sprintf */
    }
  }  
  else 
  {        /*  the function was invoked with a zero */
    nchars = sprintf(text + len, "error: 0x0        => All OK\n");
    if (nchars < 0) 
      flag = 1;
  }
  
  if (flag) 
    return(RCC_ERROR_RETURN(0, ERCC_STREAM));
  else
    return(state); /*  blindly pass MAJOR and MAINOR error as gotten from the rcc_error_get call */
}


/***************************************************************************/
err_type rcc_error_get(err_type err, err_str Maj_Pid_str, err_str Maj_en_str,
		       err_str Min_Pid_str, err_str Min_en_str)
/***************************************************************************/
{
  err_type err_min, err_maj;
  int      flag;

  strcpy(Maj_Pid_str, "");
  strcpy(Maj_en_str,  "All OK");
  strcpy(Min_Pid_str, "");
  strcpy(Min_en_str,  "All OK");

  err_min = 0;
  err_maj = 0;
  flag = 0;

  get_err[P_ID_RCC_ERROR] = rcc_error_err_get; /*  initialize itself */

  /*  first the major */

  if (RCC_ERROR_MAJOR(err)) 
  {
    if (get_err[RCC_ERROR_MAJOR_PID( err )] != NULL) 
      err_maj =  get_err[RCC_ERROR_MAJOR_PID( err )](RCC_ERROR_MAJOR( err ), Maj_Pid_str, Maj_en_str);
    else 
    {
      flag = 1;
      strcpy(Maj_Pid_str, "");
      strcpy(Maj_en_str, "<rcc_error_init not called>");
    }
  }

  /*  now the minor */

  if (RCC_ERROR_MINOR_ERRNO(err)) 
  {
    if (get_err[RCC_ERROR_MINOR_PID( err )] != NULL) 
       err_min = get_err[RCC_ERROR_MINOR_PID( err )](RCC_ERROR_MINOR( err ), Min_Pid_str, Min_en_str);
    else 
    {
      flag = 1;
      strcpy(Min_Pid_str, "");
      strcpy(Min_en_str, "<rcc_error_init not called>");
    }
  }

  if (flag) 
    return(RCC_ERROR_RETURN(0,ERCC_NOINIT));

  if (err_maj) 
    return(RCC_ERROR_RETURN(err_maj, ERCC_NOCODE));
  else if (err_min ) 
    return(RCC_ERROR_RETURN(err_min, ERCC_NOCODE));
  else 
    return(RCC_ERROR_RETURN(0, ERCC_OK));
}


/***********************************************************************/ 
err_type rcc_error_err_get(err_pack err, err_str Pid_str, err_str en_str) 
/***********************************************************************/ 
{
  strcpy(Pid_str, P_ID_RCC_ERROR_STR);
 
  switch (err) 
  {
    case ERCC_OK:         strcpy(en_str, ERCC_OK_STR);       break;
    case ERCC_NOTOPEN:    strcpy(en_str, ERCC_NOTOPEN_STR);  break;
    case ERCC_NOINIT:     strcpy(en_str, ERCC_NOINIT_STR);   break;
    case ERCC_STREAM:     strcpy(en_str, ERCC_STREAM_STR);   break;
    case ERCC_NOCODE:     strcpy(en_str, ERCC_NOCODE_STR);   break;
    default:              strcpy(en_str, ERCC_NOCODE_STR);   return(RCC_ERROR_RETURN(0, ERCC_NOCODE));  break; 
  }
  return( RCC_ERROR_RETURN(0,ERCC_OK) );
}

#endif 
