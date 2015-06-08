#ifdef Linux

/********************************************************
*   rorc_aux.c                                          *
*   last update:        23/04/07                        *
*   written by: Peter Csato and Ervin Denes             *
* history:                                              *
*                                                       *
* v.4.5.1  24-May-2006: new routines xdgtoi() and       *
*                                   getIntelHexRecord() *
* v.4.4.5  25-Jan-2006: new routine th()                *
* v.4.4    07-Nov-2005: minor bug in read_mif()         *
* v.4.3.11 28-Jan-2005: no use of strtok in browse()    *
* v.4.3.5  08-Aug-2004: add routin elapsed()            *
* v.4.3.1  29-Apr-2004: add possibility of in-line      *
*                       comments (getToken, getVal)     *
* v.4.2.2  19-Nov-2003: add routines lock_file and      *
*                                    lock_test          *
* v.4.1    28-Oct-2003: add routine read_mif            *   
*          10-Jul-2003: add JTAG read from and to file  *   
*          26-Feb-2003: original version                *   
*                                                       *
********************************************************/
#define LIB_SOURCE_FILE "rorc_aux.c"
#define LIB_DESCRIPTION "ALICE RORC auxillary library"

#include <ctype.h>
#include "rorc_aux.h"
char auxIdent[]="@(#)""" LIB_SOURCE_FILE """: """ LIB_DESCRIPTION \
     """ """ RORC_LIBRARY_VERSION """ compiled """ __DATE__ """ """ __TIME__;

/********************************************************
*  calculate elapsed time                               *
********************************************************/
void elapsed(struct timeval *tv2, struct timeval *tv1, 
             int *dsec, int *dusec)
{
  *dsec = tv2->tv_sec - tv1->tv_sec;
  *dusec = tv2->tv_usec - tv1->tv_usec;
  if (*dusec < 0)
  {
    (*dsec)--;
    *dusec += MEGA;
  }
}


/********************************************************
*  print statistics                                     *
********************************************************/
void print_time(int l, 
                struct timeval *tv2, struct timeval *tv1, time_t *start_time,
                int sum_Gbyte, int sum_byte, char *stat_file_name)
{
  time_t time_val, time_now;
  int dsec, dusec;
  double dtime;
  char oldDayW[8], oldMon[8], oldTime[16];
  int oldDay, oldYear;
  char newDayW[8], newMon[8], newTime[16];
  int newDay, newYear;
  int summG, summB;
  FILE *sfile;


  printf("\n-------------------------------------------\n");
  printf("The program has ended after %d cycle(s).\n",l);

  elapsed (tv2, tv1, &dsec, &dusec);
  dtime = (double)dsec + (double)dusec/MEGA;
  if (dtime) 
  {
    printf("The program moved ");
    if (sum_Gbyte > 0)
      printf("%d.%09d Gbyte data ", sum_Gbyte, sum_byte);
    else
      printf("%d byte data ", sum_byte);
    printf("in %d.%06d seconds\nthat corresponds to ", dsec, dusec);
    if (sum_Gbyte > 0)
      printf("%.4f Gbyte/sec speed.\n", 
                    ((double)sum_Gbyte+(double)sum_byte/GIGA)/dtime);
    else
      printf("%sbyte/sec speed.\n", order((double)sum_byte/dtime, 3));
  }

  time_now = time(&time_val);
  if(stat_file_name != (char *)NULL)
  {
    sfile = fopen(stat_file_name, "r");
    if(sfile == NULL)
    {
      sscanf(ctime(start_time), "%s %s %d %s %d",
                     oldDayW, oldMon, &oldDay, oldTime, &oldYear);
      summG = 0;
      summB = 0;
    }
    else
    {
      fscanf(sfile, "%s %s %d %s %d %s %s %d %s %d %d %d",
                     oldDayW, oldMon, &oldDay, oldTime, &oldYear,
                     newDayW, newMon, &newDay, newTime, &newYear,
                     &summG, &summB);
      fclose(sfile);
    }
    sfile = fopen(stat_file_name, "w");
    if(sfile == NULL)
    {
      printf("Can not open statistic file %s - number of transferred bytes not written\n", 
             stat_file_name);
    }
    else
    {
      sscanf(ctime(&time_val), "%s %s %d %s %d",
                                newDayW, newMon, &newDay, newTime, &newYear);

      summB += sum_byte;
      if (summB >= GIGA)
      {
        summG++;
        summB -= GIGA;
      }
      summG += sum_Gbyte;

      fprintf(sfile, "%s %s %d %s %d %s %s %d %s %d %d %d\n",
                      oldDayW, oldMon, oldDay, oldTime, oldYear,
                      newDayW, newMon, newDay, newTime, newYear,
                      summG, summB);
      fclose(sfile);
    }
  }

  printf("%s\n", ctime(&time_val));
  printf("end\n");

}

/******************************************************************************
*    Format serial number endings: "st", "nd", "rd", or "th"                  *
******************************************************************************/
char *th(int num)
{
  int tempint;
  static char res[3];

  tempint = abs(num);
  if (tempint == 1)
    strcpy(res, "st");
  else if (tempint == 2)
    strcpy(res, "nd");
  else if (tempint == 3)
    strcpy (res, "rd");
  else 
    strcpy(res, "th");
  return res;
}

/******************************************************************************
*    Calculate percentage and format for printing                             *
******************************************************************************/
char *percent(double in_val, double in_base, int prec)    
{
  int i, sig;
  char tmp[80];
  static char res[80];
  double fl_val, percent, limit;

  strcpy(res, "");
  if (in_base == 0)
    return res;

  if (in_val == 0)
  {
    sprintf(res, "0 %%");
    return res;
  }

  percent = 100.0 * in_val / in_base;
  sig = 1;
  if (percent < 0.0)
     sig = -1;
  fl_val = sig * percent;

  limit = 1.0;
  for (i = 0; i < prec; i++) 
    limit *= 10.0;
  limit = 1.0 / limit;
    
  if (fl_val < limit)
    sprintf(tmp, "< %.*f %%", prec, limit);
  else 
    sprintf(tmp, "%.*f %%", prec, fl_val);

  if (sig == -1) 
   strcpy(res, "-");
  strcat(res, tmp);

  return res;
}

/******************************************************************************
*    Format value for printing as Mega, kilo, ...., nano, etc format          *
******************************************************************************/
char *order(double in_val, int prec)    /* format fl_val for printing */
{
  char tmp[80];
  static char res[80];
  double fl_val;

  if (in_val < 0.0) fl_val = -in_val;
  else fl_val = in_val;

  if (fl_val == 0.0)
    sprintf(tmp, "%.*f ", prec, fl_val);
  else if (fl_val < 0.000001)
    sprintf(tmp, "%.*f n", prec, fl_val * 1000000000.0);
  else if (fl_val < 0.001)
    sprintf(tmp, "%.*f u", prec, fl_val * 1000000.0);
  else if (fl_val < 1.0)
    sprintf(tmp, "%.*f m", prec, fl_val * 1000.0);
  else if (fl_val < 1000.0)
    sprintf(tmp, "%.*f ", prec, fl_val);
  else if (fl_val < 1000000.0)
    sprintf(tmp, "%.*f k", prec, fl_val / 1000.0);
  else if (fl_val < 1000000000.0)
    sprintf(tmp, "%.*f M", prec, fl_val / 1000000.0);
  else
    sprintf(tmp, "%.*f G", prec, fl_val / 1000000000.0);

  if (in_val < 0.0) strcpy(res, "-");
  else strcpy(res, "");

  strcat(res, tmp);

  return res;
}

/******************************************************************************
*    Format time value for printing as ..h..m..s, or ms, us, ns format          *
******************************************************************************/
char *time_order(double in_val, int prec)    /* format fl_val for printing */
{
  char tmp[80];
  static char res[80];
  double fl_val, sec;
  int hour, min;

  if (in_val < 0.0) fl_val = -in_val;
  else fl_val = in_val;

  if (fl_val == 0.0)
    sprintf(tmp, "%.*f s", prec, fl_val);
  else if (fl_val < 0.000001)
    sprintf(tmp, "%.*f ns", prec, fl_val * 1000000000.0);
  else if (fl_val < 0.001)
    sprintf(tmp, "%.*f us", prec, fl_val * 1000000.0);
  else if (fl_val < 1.0)
    sprintf(tmp, "%.*f ms", prec, fl_val * 1000.0);
  else if (fl_val < 60.0)
    sprintf(tmp, "%.*f s", prec, fl_val);
  else if (fl_val < 3600.0)
  {
    min = (int)fl_val / 60;
    sec = fl_val - min * 60.0;
    sprintf(tmp, "%.*fs = %dm%.*fs", prec, fl_val, min, prec, sec);
  }
  else
  {
    hour = (int)fl_val / 3600;
    sec = fl_val - hour * 3600.0;
    min = (int)sec / 60;
    sec = sec - min * 60.0;
    sprintf(tmp, "%.*fs = %dh%dm%.*fs", prec, fl_val, hour, min, prec, sec);
  }

  if (in_val < 0.0) strcpy(res, "-");
  else strcpy(res, "");

  strcat(res, tmp);

  return res;
}

/******************************************************************************
*    Format value for printing as (binary) Mega,  kilo, ... etc format        *
******************************************************************************/
char *border(double in_val, int prec)    /* format fl_val for printing */
{
  char tmp[80];
  static char res[80];
  double fl_val;

  if (in_val < 0.0) fl_val = -in_val;
  else fl_val = in_val;

  if (fl_val == 0.0)
    sprintf(tmp, "%.*f ", prec, fl_val);
  else if (fl_val < 1024.0)
    sprintf(tmp, "%.*f ", prec, fl_val);
  else if (fl_val < 1048576.0)
    sprintf(tmp, "%.*f k", prec, fl_val / 1024.0);
  else if (fl_val < 1073741824.0)
    sprintf(tmp, "%.*f M", prec, fl_val / 1048576.0);
  else
    sprintf(tmp, "%.*f G", prec, fl_val / 1073741824.0);

  if (in_val < 0.0) strcpy(res, "-");
  else strcpy(res, "");

  strcat(res, tmp);

  return res;
}

/******************************************************************************/
/* change string to upper case                                                */
/******************************************************************************/
void upper(char *string)

{
  int i, j;

  j = strlen(string);
  for (i = 0; i < j; i++)
    string[i] = toupper(string[i]);

}

/******************************************************************************/
/* cut trailing white spaces and end-of-lines                                 */
/******************************************************************************/
int trim(char *string)

{
  int i;
  char ch;

  i = strlen(string);
  while(i)
  {
    ch = string[i-1];
    if ( (ch == ' ') || (ch == '\t') || (ch == '\n') || (ch == 0x0d) )
      string[--i] = '\0';
    else
      break;
  }

  return(i);
}


/******************************************************************************/
/* get integer value of a hexadecimal digit                                   */
/******************************************************************************/
int xdgtoi(char c)
{
  if (!isxdigit(c))
    return (0);
  else if (isdigit(c))
    return (c - '0');
  else if (isupper(c))
    return (0xA + (c - 'A'));
  else
    return (0xa + (c - 'a'));
}


/******************************************************************************/
/* find a string in an array of key_words                                     */
/* The last key_word must be ""                                               */
/* Maximum string length is 80 characters                                     */
/* returns the serial number of the token or 0                                */
/******************************************************************************/
int n_key_word(char *key_words[], char *string)
{
  int i = 0, j = 0, l;
  char tmp_str[80];

  strcpy(tmp_str, string);
  upper(tmp_str);
  while (1)
  {
    l = strlen(key_words[i]);
    if (l == 0) break;                  /* end of key_words, string not found */
    if (strcmp(key_words[i], tmp_str) == 0)
    {
      j = i + 1;                        /* found */
      break;
    }
    i++;                                /* next key_word */
  }

  return (j);
}

/******************************************************************************/
/* Browse a string into tokens.                                               */
/* The tokens are separated by one or more characters from separator string.  */
/* Load token addresses into an array of pointers to string.                  */
/* Returns the number of tokens.                                              */
/* The original string is modified, '\0' characters are inserted.             */
/******************************************************************************/
int browse(char *string, char *token[], int max_token, char *separator)
{
  char *t, *t1, *t2;
  int i=0, j, l;

  t2 = string;

  while (1)
  {
    if (i >= max_token)
      break;

    l = strlen(t2);
    if (l == 0) 
      break;                         /* end of string, no token found */

    for (j = 0; j < l; j++)     
    {
      if (strchr(separator, t2[j]) == NULL)
      {                              /* the character is a not separator */
        t1 = t2 + j;                 /* start of token found */
        token[i++] = t1;
        break;
      }
    }
    if (j == l)
      break;                         /* all remaining chars are separators */

    if ((t = strpbrk(t1, separator)) == NULL)
      break;                         /* no more separator: last token found */

    *t = '\0';                       /* separator: end of token found */
    t2 = t + 1;
  }

  return(i);

}

/******************************************************************************/
/* Get the given number of tokens from the next line of an open file.         */
/* Comments (#, *, ;) and empty lines are skipped.                            */
/* Returns the number of tokens returned.                                     */
/******************************************************************************/
int getToken(char *val[], int maxVal, FILE* fp)
{
  static char token_line[256];
  char line[256];
  char *token[32];
  static char text[32][80];
  int i, j, ret, len, line_len, n_token, max_token = 32;
  int n_cat, nt;

  ret = 0;
  while (1)
  {
    if (fgets(line, 256, fp) == NULL)  /* EOF */
      break;

    line_len = strlen(line);
    if (line[line_len - 1] != '\n')
      while (1) /* skip to end of line in case of long line */
        if ( (getc(fp) == '\n') || (feof(fp)) )
          break;
    line_len = trim(line);     /* trim white spaces and end-of-line */
    if (line_len < 1)          /* empty line */
      continue;

    /* browse the line into tokens */
    strcpy(token_line, line);
    n_token = browse(token_line, token, max_token, " \t=");

    if (n_token == 0)
      continue;                /* empty line */
    if ((token[0][0] == '#') || (token[0][0] == '*') || (token[0][0] == ';'))
      continue;                /* comment line */

    if ((ret + n_token) > maxVal)
      n_token = maxVal - ret;

    /* concatenate tokens starting and ending with " or ' */
    nt = 0;
    for (i = 0; i < n_token; i++)
    {
      if ((token[i][0] != '"') && (token[i][0] != '\047'))
        continue;
      text[nt][0] = '\0';  /* empty text */
      len = strlen(token[i]);
      if (len > 1 ) 
      {
        if ((token[i][len-1] == '"') || (token[i][len-1] == '\047'))
        {
          token[i][len-1] = '\0';    /* trim first and last characters */
          token[i] = &token[i][1];
          continue;
        }
        strcat(text[nt], &token[i][1]);   /* trim the first character */
      }
      strcat(text[nt], " ");
      n_cat = 0;
      for (j = i+1; j < n_token; j++)
      {
        len = strlen(token[j]);
        if ((token[j][len-1] == '"') || (token[j][len-1] == '\047'))
        {
          token[j][len-1] = '\0';    /* trim the last characters */
          strcat(text[nt], token[j]);
          n_cat++;
          break;
        }
        else
        {
          strcat(text[nt], token[j]);
          strcat(text[nt], " ");
          n_cat++;
        }
      } 
      token[i] = text[nt];
      nt++;
      n_token -= n_cat;  
      for (j = i + 1; j < n_token; j++)
        token[j] = token[j + n_cat];
    }

    /* skip in-line comments */
    for (i = 1; i< n_token; i++)
    {
      if ( (token[i][0] == ';') ||
          ((token[i][0] == '/') && (token[i][1] == '/'))
         )
      {
        n_token = i;
        break;
      }
    }

    for (i = 0; i < n_token; i++)
      val[ret++] = token[i];

    break;
  }
  return (ret);
}


/******************************************************************************/
/* Get the given number of values from an open file.                          */
/* Comments (#, *, ;) and empty lines are skipped.                            */
/* Returns the number of values read.                                         */
/******************************************************************************/
int getVal(unsigned long *val, int maxVal, FILE* fp, char *format)
{
  static char token_line[256];
  char line[256]; 
  char *token[32];
  int i, ret, line_len, n_token, max_token = 32;

  ret = 0;
  while (1)
  {
    if (fgets(line, 256, fp) == NULL)  /* EOF */
      break;

    line_len = strlen(line);
    if (line[line_len - 1] != '\n')
      while (1) /* skip to end of line in case of long line */
        if ( (getc(fp) == '\n') || (feof(fp)) )
          break;
    line_len = trim(line);     /* trim white spaces and end-of-line */
    if (line_len < 1)          /* empty line */
      continue;
 
    /* browse the line into tokens */
    strcpy(token_line, line);
    n_token = browse(token_line, token, max_token, " \t=");
    if (n_token == 0)
      continue;                /* empty line */
    if ((token[0][0] == '#') || (token[0][0] == '*') || (token[0][0] == ';'))
      continue;                /* comment line */

    /* skip in-line comments */
    for (i = 1; i< n_token; i++)
    {
      if ( (token[i][0] == ';') ||
          ((token[i][0] == '/') && (token[i][1] == '/'))
         )
      {
        n_token = i;
        break;
      }
    }

    if ((ret + n_token) > maxVal)
      n_token = maxVal - ret;
    for (i = 0; i < n_token; i++)
    {
      sscanf(token[i], format, &val[ret++]);
//      val[ret++] = strtoul(token[i], (char**)NULL, 16);
    }
    if (ret >= maxVal)
      break;
  }
  return (ret);
}


/******************************************************************************/
/* Get length, address, data type and data values of an IntelHEX file.        */
/* Returns 0 if format is OK, error code if format is erroneous.              */
/* Error codes: 1 = not IntelHEX format                                       */
/*              2 = record too short                                          */
/*              3 = not hexadecimal digit in record                           */
/*              4 = length error                                              */
/*              5 = type error                                                */
/*              6 = checksum is not correct                                   */
/******************************************************************************/
int getIntelHexRecord(FILE *fp,
                      int *len, int *addr, int *type, int *val, char *err)
{
  char c, line[MAX_INTELHEX_RECORD+1];
  int i, j, line_len, data_len, data, chks, checksum;

  for (i = 0; i < MAX_INTELHEX_RECORD; i++) 
  {
    c = getc(fp);
    if (feof(fp))                              /* EOF */
      break;
    if ((c == '\n') || (c == 0xa) || (c == 0x0d)  || (c == '\0'))
    {
      if (i)
        break;                                 /* end of record */
      else
      {
        i--;
        continue;                              /* skip CR/LF */
      }
    }
    line[i] = c;
  }
  line[i] = '\0';

  if (line[0] != ':')                          /* not IntelHEX record */
  {
    strcpy(err, "not IntelHEX record"); 
    return (1);
  }

  line_len = strlen(line);
  if (line_len < MIN_INTELHEX_RECORD)          /* record too short */
  {
    sprintf(err, "record too short: %d byte(s)", line_len);
    return (2);
  }

  for (i = 1; i < line_len; i ++)
  {
    if (!isxdigit(line[i]))                    /* not hexadecimal digit */
    {
      sprintf(err, "char %d (0x%x = '%c') is non-hexadecimal digit",
                           i+1, line[i], line[i]);
      return (3);
    }
  }

  checksum = 0;
  data_len = xdgtoi(line[1]) * 16 + xdgtoi(line[2]);
  *len = data_len;
  checksum += data_len;

  if (line_len != (2*data_len + MIN_INTELHEX_RECORD))
  {
    strcpy(err, "length error");              /* length error */
    return (4);
  }

  *addr = 0;
  for (i = 3; i < 7; i += 2)
  {
    data = xdgtoi(line[i]) * 16 + xdgtoi(line[i+1]);
    *addr = (*addr) * 256 + data;
    checksum += data;
  }
 
  data = xdgtoi(line[7]) * 16 + xdgtoi(line[8]);
  *type = data;
  checksum += data;
  if (data > 5)                              /* type error */
  {
    strcpy(err, "type error");
    return (5);
  }

  for (i = 9, j = 0; j < data_len; i += 2, j++)
  {
    data = xdgtoi(line[i]) * 16 + xdgtoi(line[i+1]);
    val[j] = data;
    checksum += data;
  }
  checksum = (0x100 - (0xFF & checksum)) & 0xFF;

  chks = xdgtoi(line[line_len-2]) * 16 + xdgtoi(line[line_len-1]);
  if (checksum != chks)                       /* checksum error */
  {
    sprintf(err, "checksum error: calculated=%02x, on file=%02x\n", 
                          checksum, chks);
    return (6);
  }

  strcpy(err, "");
  return (0);
}


/******************************************************************************/
/* Prepare an IntelHEX record and put to the file.                            */
/* Returns 0 if the write is OK, error code otherwise.                        */
/* Error codes: 4 = length error                                              */
/*              5 = type error                                                */
/*              7 = write error                                               */
/******************************************************************************/
int putIntelHexRecord(FILE *fp,
                      int len, int addr, int type, int *val, char *err)
{
  char line[MAX_INTELHEX_RECORD+1];
  int i, j, line_len, checksum;

  line[0] = ':';
  checksum = 0;

  if (len > 255)
  {
    strcpy(err, "length error");              /* length error */
    return (4);
  }
  sprintf(&line[1], "%02x", len);
  checksum += len;

  sprintf(&line[3], "%04x", addr);
  checksum += ((addr & 0xFF00) >> 8) + (addr & 0xFF);
 
  if (type > 5)                              /* type error */
  {
    strcpy(err, "type error");
    return (5);
  }
  sprintf(&line[7], "%02x", type);
  checksum += type;

  line_len = 2 * len + 11;
  for (i = 9, j = 0; j < len; i += 2, j++)
  {
    sprintf(&line[i], "%02x", val[j]);
    checksum += val[j];
  }

  checksum = (0x100 - (0xFF & checksum)) & 0xFF;
  sprintf(&line[line_len-2], "%02x", checksum);

  line[line_len] = '\n';
  line[line_len+1] = '\0';
  upper(line);
  if (fputs(line, fp) < 0)
  {
    strcpy(err, "write error");
    return (7);
  }

  strcpy(err, "");
  return (0);
}


/******************************************************************************/
/* Read JTAG data from text file and put it to the physmem memory             */
/******************************************************************************/
int JTAG2Memory(FILE *i_fp, volatile unsigned long *data_addr, int length)
{
  int i, j, n, nible, n_word;
  unsigned long buffer;
  char jtag[4];

  n_word = 0;
  j = 0;
  buffer = 0;
  for (i = 0; i < length; i++)
  {
    if (fgets(jtag, 4, i_fp) == NULL) /* EOF */
      break;
    n = strlen(jtag);
    if (jtag[n-1] != '\n')
      while (1) /* skip to end of line in case of long line */
        if ( (getc(i_fp) == '\n') || (feof(i_fp)) )
          break;

    nible =  0;
    for (n = 0; n < 3; n++)
    {
      if (jtag[n] == '1')
        nible |= (1 << n);
    }
    buffer |= (nible << j);
    j += 4;
    if (j > 28)
    {
      *(data_addr + n_word) = buffer;
      n_word++;
      j = 0;
      buffer = 0;
    }
  }
  if (j != 0)
  {
    *(data_addr + n_word) = buffer;
    n_word++;
  }

  return(n_word);
}



/******************************************************************************/
/* Read JTAG data and TDO from memory and write them to a text file           */
/******************************************************************************/
void Memory2JTAG(FILE *o_fp, volatile unsigned long *data_addr,
                             volatile unsigned long *return_addr, int n_word)
{
  int i, j, k, n, nible, tdo;
  unsigned long buffer, r_buffer;
  char jtag[6];

  jtag[5] = '\0';
  for (i = 0; i < n_word; i++)
  {
    buffer = *(data_addr + i);
    r_buffer = *(return_addr + i/4);
    for (j = 0, k = (i & 3)*8; j < 32; j += 4, k++)
    {
      nible =  (buffer >> j) & 0xff;
      for (n = 0; n < 3; n++)
        jtag[n] =  ((nible >> n) & 1) ?  '1' : '0';
      jtag[3] = ' ';

      tdo = (r_buffer >> k) & 1;
      jtag[4] = tdo ? '1' : '0';

      fprintf(o_fp, "%s\n", jtag);
    }
  }
}



/******************************************************************************/
/* Read Memory Initialization File                                            */
/******************************************************************************/
int read_mif(FILE *fp, unsigned long *memory, int max_word, int *read_word)
{
  int i, j, line_len, line_no, n_token, n_word, max_token = 32;
  int content, width, depth, a_radix, d_radix;
  char *token[32], *endptr;
  static char token_line[256];
  char line[256];
  unsigned long addr1, addr2, value, waddr, word;

  /* defaults */
  a_radix = 16;
  d_radix = 10;
  width = 16;
  depth = 2048;

  *read_word = 0;
  line_no = 0;
  content = 0;
  n_word = 0;
  for (i = 0; i < max_word; i++)
  {
    if (fgets(line, 256, fp) == NULL) /* EOF */
      break;
    line_no++;
    line_len = strlen(line);
    if (line[line_len - 1] != '\n')
      while (1) /* skip to end of line in case of long line */
        if ( (getc(fp) == '\n') || (feof(fp)) )
          break;
    line_len = trim(line);     /* trim white spaces and end-of-line */
    if (line_len < 1)          /* empty line */
      continue;

    /* browse the line into tokens */
    strcpy(token_line, line);
    n_token = browse(token_line, token, max_token, " \t=:;");
    if (n_token == 0)
      continue;                /* empty line */
    if ((token[0][0] == '-') && (token[0][1] == '-'))
      continue;                /* comment line */

    if (strcmp(token[0], "WIDTH") == 0) 
    {
      width = atoi(token[1]);
      if ((width != 8) && (width != 16) && (width != 32))
        return (line_no);       /* width could be 8, 16 or 32 bits */
      continue;
    }
  
    if (strcmp(token[0], "DEPTH") == 0) 
    {
      depth = atoi(token[1]);
      continue;
    }
   
    if (strcmp(token[0], "ADDRESS_RADIX") == 0) 
    {
      if (strcmp(token[1], "HEX") == 0)
        a_radix = 16;
      else if (strcmp(token[1], "UNS") == 0)
        a_radix = 10;
      else
        return (line_no);
      continue;
    }

    if (strcmp(token[0], "DATA_RADIX") == 0) 
    {
      if (strcmp(token[1], "HEX") == 0)
        d_radix = 16;
      else if (strcmp(token[1], "UNS") == 0)
        d_radix = 10;
      else
        return (line_no);    /* UNS and HEX are accepted */
      continue;
    }

    if (strcmp(token[0], "CONTENT") == 0)
    {
      if (strcmp(token[1], "BEGIN") == 0)
        content = 1;
      else
        return (line_no);
      continue; 
    }

   if (strcmp(token[0], "END") == 0)
   {
     if (content)
       content = 0;
     else
       return (line_no);
     *read_word = depth * width / 32;
     continue;
   }

    if (content)
    {
      if (token[0][0] == '[')
      {
        token[0] = &token[0][1];
        addr1 = strtoul(token[0], &endptr, a_radix);
        token[0] = endptr + 2;
        addr2 = strtoul(token[0], &endptr, a_radix);
        value = strtoul(token[1], &endptr, d_radix);
      }
      else
      {
        addr1 = strtoul(token[0], &endptr, a_radix);
        addr2 = addr1;
        value = strtoul(token[1], &endptr, d_radix);
      }
      if ((addr1 > addr2) || (addr2 >= depth))
        return (line_no);     /* end address > depth */
      if ((width != 32) && (value >> width))
        return (line_no);     /* value larger than width */

      for (j = addr1; j <= addr2; j++)
      {
        switch (width)
        {
          case 8:
            waddr = j/4;
            if (waddr >= max_word) break;
            word = memory[waddr];
            switch (j%4)
            {
              case 0: word = (word & 0xffffff00) | value; break;
              case 1: word = (word & 0xffff00ff) | (value <<  8); break;
              case 2: word = (word & 0xff00ffff) | (value << 16); break;
              case 3: word = (word & 0x00ffffff) | (value << 24); break;
            }
            break;
          case 16:
            waddr = j/2;
            if (waddr >= max_word) break;
            word = memory[waddr];
            if (j == 2*waddr)
              word = (word & 0xffff0000) | value;
            else
              word = (word & 0x0000ffff) | (value << 16);
            break;
          default:
            waddr = j;
            if (waddr >= max_word) break;
            word = value;
            break;
        }
        if (waddr >= max_word) continue;
        memory[waddr] = word;
      }
    }
    else
      return (line_no);
  }

  return (0);
}



/******************************************************************************/
/* Calculate the 2 base logarithmic function of an integer number             */
/******************************************************************************/
int logi2(unsigned long number)
{
  int i;
  unsigned long mask = 0x80000000;

  for (i = 0; i < 32; i++)
  {
    if (number & mask)
      break;
    mask = mask >> 1;
  }

  return (31 - i);  
}


/******************************************************************************/
/* Find the nearest lower power of 2 integer                                  */
/******************************************************************************/
int roundPowerOf2(int number)
{
  int logNum;

    logNum = logi2(number);
    return (1 << logNum);
}


/******************************************************************************/
/* Lock a file                                                                */
/******************************************************************************/
int lock_file(int fd)
{
  struct flock lock;

  lock.l_type = F_WRLCK;
  lock.l_start = 0;
  lock.l_whence = SEEK_SET;
  lock.l_len = 0;

  return (fcntl(fd, F_SETLK, &lock));
}

/******************************************************************************/
/* Unlock a file                                                              */
/******************************************************************************/
int unlock_file(int fd)
{
  struct flock lock;

  lock.l_type = F_UNLCK;
  lock.l_start = 0;
  lock.l_whence = SEEK_SET;
  lock.l_len = 0;

  return (fcntl(fd, F_SETLK, &lock));
}

/******************************************************************************/
/* Test for locking conditions                                                */
/******************************************************************************/
int lock_test(int fd)
{
  struct flock lock;

  lock.l_type = F_WRLCK;
  lock.l_start = 0;
  lock.l_whence = SEEK_SET;
  lock.l_len = 0;

   if (fcntl(fd, F_GETLK, &lock) < 0)
     return (-1);
   if (lock.l_type == F_UNLCK)
    return (0);          // file is not locked
   return (lock.l_pid);  // pid of the lock owner
}

/******************************************************************************/

#endif
